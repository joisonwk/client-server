/*
 * srv_src/data/data_buffer.c
 *
 * A Logging Subsystem
 *
 * Copyright (C) 2007-2008 Google, Inc.
 *
 * Robert Love <rlove@google.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define pr_fmt(fmt) "data_buffer: " fmt

#include <linux/sched.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/vmalloc.h>
#include <linux/aio.h>
#include "data_buffer.h"

#include <asm/ioctls.h>

/**
 * struct data_buffer - represents a specific data, such as 'main' or 'radio'
 * @buffer:	The actual ring buffer
 * @misc:	The "misc" device representing the data
 * @wq:		The wait queue for @readers
 * @readers:	This data's readers
 * @mutex:	The mutex that protects the @buffer
 * @w_off:	The current write head offset
 * @head:	The head, or location that readers start reading at.
 * @size:	The size of the data
 * @datas:	The list of data channels
 *
 * This structure lives from module insertion until module removal, so it does
 * not need additional reference counting. The structure is protected by the
 * mutex 'mutex'.
 */
struct data_buffer {
	unsigned char		*buffer;
	struct miscdevice	misc;
	wait_queue_head_t	wq;
	struct list_head	readers;
	struct mutex		mutex;
	size_t			w_off;
	size_t			head;
	size_t			size;
	struct list_head	datas;
};

static LIST_HEAD(data_list);


/**
 * struct data_reader - a dataging device open for reading
 * @data:	The associated data
 * @list:	The associated entry in @data_buffer's list
 * @r_off:	The current read head offset.
 * @r_all:	Reader can read all entries
 *
 * This object lives from open to release, so we don't need additional
 * reference counting. The structure is protected by data->mutex.
 */
struct data_reader {
	struct data_buffer	*data;
	struct list_head	list;
	size_t			r_off;
	bool			r_all;
};

/* data_offset - returns index 'n' into the data via (optimized) modulus */
static size_t data_offset(struct data_buffer *data, size_t n)
{
	return n & (data->size - 1);
}


/*
 * file_get_data - Given a file structure, return the associated data
 *
 * This isn't aesthetic. We have several goals:
 *
 *	1) Need to quickly obtain the associated data during an I/O operation
 *	2) Readers need to maintain state (data_reader)
 *	3) Writers need to be very fast (open() should be a near no-op)
 *
 * In the reader case, we can trivially go file->data_reader->data_buffer.
 * For a writer, we don't want to maintain a data_reader, so we just go
 * file->data_buffer. Thus what file->private_data points at depends on whether
 * or not the file was opened for reading. This function hides that dirtiness.
 */
static inline struct data_buffer *file_get_data(struct file *file)
{
	if (file->f_mode & FMODE_READ) {
		struct data_reader *reader = file->private_data;

		return reader->data;
	}
	return file->private_data;
}

/*
 * get_entry_header - returns a pointer to the data_entry header within
 * 'data' starting at offset 'off'. A temporary data_entry 'scratch' must
 * be provided. Typically the return value will be a pointer within
 * 'data->buf'.  However, a pointer to 'scratch' may be returned if
 * the data entry spans the end and beginning of the circular buffer.
 */
static struct data_entry *get_entry_header(struct data_buffer *data,
		size_t off, struct data_entry *scratch)
{
	size_t len = min(sizeof(struct data_entry), data->size - off);

	if (len != sizeof(struct data_entry)) {
		memcpy(((void *) scratch), data->buffer + off, len);
		memcpy(((void *) scratch) + len, data->buffer,
			sizeof(struct data_entry) - len);
		return scratch;
	}

	return (struct data_entry *) (data->buffer + off);
}

/*
 * get_entry_msg_len - Grabs the length of the message of the entry
 * starting from from 'off'.
 *
 * An entry length is 2 bytes (16 bits) in host endian order.
 * In the data, the length does not include the size of the data entry structure.
 * This function returns the size including the data entry structure.
 *
 * Caller needs to hold data->mutex.
 */
static __u32 get_entry_msg_len(struct data_buffer *data, size_t off)
{
	struct data_entry scratch;
	struct data_entry *entry;

	entry = get_entry_header(data, off, &scratch);
	return entry->len;
}

static size_t get_hdr_len(void)
{
	return sizeof(struct data_entry);
}

static ssize_t copy_header_to_user(struct data_entry *entry,
					 char __user *buf)
{
	size_t hdr_len;
	hdr_len     = sizeof(struct data_entry);
	return copy_to_user(buf, entry, hdr_len);
}

/*
 * do_read_data_buffer_to_user - reads exactly 'count' bytes from 'data' into the
 * user-space buffer 'buf'. Returns 'count' on success.
 *
 * Caller must hold data->mutex.
 */
static ssize_t do_read_data_buffer_to_user(struct data_buffer *data,
				   struct data_reader *reader,
				   char __user *buf,
				   size_t count)
{
	struct data_entry scratch;
	struct data_entry *entry;
	size_t len;
	size_t msg_start;

	/*
	 * First, copy the header to userspace, using the version of
	 * the header requested
	 */
	entry = get_entry_header(data, reader->r_off, &scratch);
	if (copy_header_to_user(entry, buf))
		return -EFAULT;

	count -= get_hdr_len();
	buf += get_hdr_len();
	msg_start = data_offset(data,
		reader->r_off + sizeof(struct data_entry));

	/*
	 * We read from the msg in two disjoint operations. First, we read from
	 * the current msg head offset up to 'count' bytes or to the end of
	 * the data, whichever comes first.
	 */
	len = min(count, data->size - msg_start);
	if (copy_to_user(buf, data->buffer + msg_start, len))
		return -EFAULT;

	/*
	 * Second, we read any remaining bytes, starting back at the head of
	 * the data.
	 */
	if (count != len)
		if (copy_to_user(buf + len, data->buffer, count - len))
			return -EFAULT;

	reader->r_off = data_offset(data, reader->r_off +
		sizeof(struct data_entry) + count);

	return count + get_hdr_len();
}

/*
 * get_next_entry_by_uid - Starting at 'off', returns an offset into
 * 'data->buffer' which contains the first entry readable by 'euid'
 */
static size_t get_next_entry_by_uid(struct data_buffer *data,
		size_t off, kuid_t euid)
{
	while (off != data->w_off) {
		struct data_entry *entry;
		struct data_entry scratch;
		size_t next_len;

		entry = get_entry_header(data, off, &scratch);

		if (uid_eq(entry->euid, euid))
			return off;

		next_len = sizeof(struct data_entry) + entry->len;
		off = data_offset(data, off + next_len);
	}

	return off;
}

/*
 * data_read - our data's read() method
 *
 * Behavior:
 *
 *	- O_NONBLOCK works
 *	- If there are no data entries to read, blocks until data is written to
 *	- Atomically reads exactly one data entry
 *
 * Will set errno to EINVAL if read
 * buffer is insufficient to hold next entry.
 */
static ssize_t data_read(struct file *file, char __user *buf,
			   size_t count, loff_t *pos)
{
	struct data_reader *reader = file->private_data;
	struct data_buffer *data = reader->data;
	ssize_t ret;
	DEFINE_WAIT(wait);

start:
	while (1) {
		mutex_lock(&data->mutex);

		prepare_to_wait(&data->wq, &wait, TASK_INTERRUPTIBLE);

		ret = (data->w_off == reader->r_off);
		mutex_unlock(&data->mutex);
		if (!ret)
			break;

		if (file->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			break;
		}

		if (signal_pending(current)) {
			ret = -EINTR;
			break;
		}

		schedule();
	}

	finish_wait(&data->wq, &wait);
	if (ret)
		return ret;

	mutex_lock(&data->mutex);

	if (!reader->r_all)
		reader->r_off = get_next_entry_by_uid(data,
			reader->r_off, current_euid());

	/* is there still something to read or did we race? */
	if (unlikely(data->w_off == reader->r_off)) {
		mutex_unlock(&data->mutex);
		goto start;
	}

	/* get the size of the next entry */
	ret = get_hdr_len() +
		get_entry_msg_len(data, reader->r_off);
	if (count < ret) {
		ret = -EINVAL;
		goto out;
	}

	/* get exactly one entry from the data */
	ret = do_read_data_buffer_to_user(data, reader, buf, ret);

out:
	mutex_unlock(&data->mutex);

	return ret;
}

/*
 * is_between - is a < c < b, accounting for wrapping of a, b, and c
 *    positions in the buffer
 *
 * That is, if a<b, check for c between a and b
 * and if a>b, check for c outside (not between) a and b
 *
 * |------- a xxxxxxxx b --------|
 *               c^
 *
 * |xxxxx b --------- a xxxxxxxxx|
 *    c^
 *  or                    c^
 */
static inline int is_between(size_t a, size_t b, size_t c)
{
	if (a < b) {
		/* is c between a and b? */
		if (a < c && c <= b)
			return 1;
	} else {
		/* is c outside of b through a? */
		if (c <= b || a < c)
			return 1;
	}

	return 0;
}


/*
 * get_next_entry - return the offset of the first valid entry at least 'len'
 * bytes after 'off'.
 *
 * Caller must hold data->mutex.
 */
static size_t get_next_entry(struct data_buffer *data, size_t off, size_t len)
{
	size_t old = data->w_off;
	size_t new = old + len;

	do {
		size_t nr = sizeof(struct data_entry) +
			get_entry_msg_len(data, off);
		off = data_offset(data, off + nr);
	} while (is_between(old,new,off));

	return off;
}
/*
 * fix_up_readers - walk the list of all readers and "fix up" any who were
 * lapped by the writer; also do the same for the default "start head".
 * We do this by "pulling forward" the readers and start head to the first
 * entry after the new write head.
 *
 * The caller needs to hold data->mutex.
 */
static void fix_up_readers(struct data_buffer *data, size_t len)
{
	size_t old = data->w_off;
	size_t new = data_offset(data, old + len);
	struct data_reader *reader;

	if (is_between(old, new, data->head))
		data->head = get_next_entry(data, data->head, len);

	list_for_each_entry(reader, &data->readers, list)
		if (is_between(old, new, reader->r_off))
			reader->r_off = get_next_entry(data, reader->r_off, len);
}

/*
 * data_write_iter - our write method, implementing support for write(),
 * writev(), and aio_write(). Writes are our fast path, and we try to optimize
 * them above all else.
 */
#if 0 
static ssize_t data_write_iter(struct kiocb *iocb, struct iov_iter *from)
{
	struct data_buffer *data = file_get_data(iocb->ki_filp);
	struct data_entry header;
	struct timespec now;
	size_t len, count, w_off;

	count = min_t(size_t, iocb->ki_nbytes, DATAGER_ENTRY_MAX_PAYLOAD);

	now = current_kernel_time();

	header.pid = current->tgid;
	header.tid = current->pid;
	header.sec = now.tv_sec;
	header.nsec = now.tv_nsec;
	header.euid = current_euid();
	header.len = count;
	header.hdr_size = sizeof(struct data_entry);

	/* null writes succeed, return zero */
	if (unlikely(!header.len))
		return 0;

	mutex_lock(&data->mutex);

	/*
	 * Fix up any readers, pulling them forward to the first readable
	 * entry after (what will be) the new write offset. We do this now
	 * because if we partially fail, we can end up with clobbered data
	 * entries that encroach on readable buffer.
	 */
	fix_up_readers(data, sizeof(struct data_entry) + header.len);

	len = min(sizeof(header), data->size - data->w_off);
	memcpy(data->buffer + data->w_off, &header, len);
	memcpy(data->buffer, (char *)&header + len, sizeof(header) - len);

	/* Work with a copy until we are ready to commit the whole entry */
	w_off =  data_offset(data, data->w_off + sizeof(struct data_entry));

	len = min(count, data->size - w_off);

	if (copy_from_iter(data->buffer + w_off, len, from) != len) {
		/*
		 * Note that by not updating data->w_off, this abandons the
		 * portion of the new entry that *was* successfully
		 * copied, just above.  This is intentional to avoid
		 * message corruption from missing fragments.
		 */
		mutex_unlock(&data->mutex);
		return -EFAULT;
	}

	if (copy_from_iter(data->buffer, count - len, from) != count - len) {
		mutex_unlock(&data->mutex);
		return -EFAULT;
	}

	data->w_off = data_offset(data, w_off + count);
	mutex_unlock(&data->mutex);

	/* wake up any blocked readers */
	wake_up_interruptible(&data->wq);

	return len;
}
#endif

static size_t data_write (struct file *fp, const char __user * buf, size_t count, loff_t *pos){
	struct data_buffer *data = file_get_data(fp);
	struct data_entry header;
	struct timespec now;
	size_t len, w_off;

	if(count <= sizeof(header)){
		return EINVAL;
	}

	count = min_t(size_t, count, DATAGER_ENTRY_MAX_PAYLOAD);

	now = current_kernel_time();
	copy_from_user(&header, buf, sizeof(header));

	header.pid = current->tgid;
	header.tid = current->pid;
	header.sec = now.tv_sec;
	header.nsec = now.tv_nsec;
	header.euid = current_euid();
	header.len = count;
	header.hdr_size = sizeof(struct data_entry);

	/* null writes succeed, return zero */
	if (unlikely(!header.len))
		return 0;

	mutex_lock(&data->mutex);

	/*
	 * Fix up any readers, pulling them forward to the first readable
	 * entry after (what will be) the new write offset. We do this now
	 * because if we partially fail, we can end up with clobbered data
	 * entries that encroach on readable buffer.
	 */
	fix_up_readers(data, sizeof(struct data_entry) + header.len);

	len = min(sizeof(header), data->size - data->w_off);
	memcpy(data->buffer + data->w_off, &header, len);
	memcpy(data->buffer, (char *)&header + len, sizeof(header) - len);
	count -= len;

	/* Work with a copy until we are ready to commit the whole entry */
	w_off =  data_offset(data, data->w_off + sizeof(struct data_entry));

	len = min(count, data->size - w_off);

	copy_from_user(data->buffer + w_off, buf+sizeof(header), len);

	copy_from_user(data->buffer, buf+sizeof(header)+len, count-len) ;

	data->w_off = data_offset(data, w_off + count);
	mutex_unlock(&data->mutex);

	/* wake up any blocked readers */
	wake_up_interruptible(&data->wq);

	return count+sizeof(header);
}

static struct data_buffer *get_data_buffer_from_minor(int minor)
{
	struct data_buffer *data;

	list_for_each_entry(data, &data_list, datas)
		if (data->misc.minor == minor)
			return data;
	return NULL;
}

/*
 * data_open - the data's open() file operation
 *
 * Note how near a no-op this is in the write-only case. Keep it that way!
 */
static int data_open(struct inode *inode, struct file *file)
{
	struct data_buffer *db;
	int ret;

	ret = nonseekable_open(inode, file);
	if (ret)
		return ret;

	db = get_data_buffer_from_minor(MINOR(inode->i_rdev));
	if (!db)
		return -ENODEV;

	if (file->f_mode & FMODE_READ) {
		struct data_reader *reader;

		reader = kmalloc(sizeof(struct data_reader), GFP_KERNEL);
		if (!reader)
			return -ENOMEM;

		reader->data = db;
		reader->r_all = in_egroup_p(inode->i_gid);

		INIT_LIST_HEAD(&reader->list);

		mutex_lock(&db->mutex);
		reader->r_off = db->head;
		list_add_tail(&reader->list, &db->readers);
		mutex_unlock(&db->mutex);

		file->private_data = reader;
	} else
		file->private_data = db;

	return 0;
}

/*
 * data_release - the data's release file operation
 *
 * Note this is a total no-op in the write-only case. Keep it that way!
 */
static int data_release(struct inode *ignored, struct file *file)
{
	if (file->f_mode & FMODE_READ) {
		struct data_reader *reader = file->private_data;
		struct data_buffer *data = reader->data;

		mutex_lock(&data->mutex);
		list_del(&reader->list);
		mutex_unlock(&data->mutex);

		kfree(reader);
	}

	return 0;
}

/*
 * data_poll - the data's poll file operation, for poll/select/epoll
 *
 * Note we always return POLLOUT, because you can always write() to the data.
 * Note also that, strictly speaking, a return value of POLLIN does not
 * guarantee that the data is readable without blocking, as there is a small
 * chance that the writer can lap the reader in the interim between poll()
 * returning and the read() request.
 */
static unsigned int data_poll(struct file *file, poll_table *wait)
{
	struct data_reader *reader;
	struct data_buffer *data;
	unsigned int ret = POLLOUT | POLLWRNORM;

	if (!(file->f_mode & FMODE_READ))
		return ret;

	reader = file->private_data;
	data = reader->data;

	poll_wait(file, &data->wq, wait);

	mutex_lock(&data->mutex);
	if (!reader->r_all)
		reader->r_off = get_next_entry_by_uid(data,
			reader->r_off, current_euid());

	if (data->w_off != reader->r_off)
		ret |= POLLIN | POLLRDNORM;
	mutex_unlock(&data->mutex);

	return ret;
}

static long data_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct data_buffer *data = file_get_data(file);
	struct data_reader *reader;
	long ret = -EINVAL;
	void __user *argp = (void __user *) arg;

	mutex_lock(&data->mutex);

	switch (cmd) {
	case DATAGER_GET_DATA_BUF_SIZE:
		ret = data->size;
		break;
	case DATAGER_GET_DATA_LEN:
		if (!(file->f_mode & FMODE_READ)) {
			ret = -EBADF;
			break;
		}
		reader = file->private_data;
		if (data->w_off >= reader->r_off)
			ret = data->w_off - reader->r_off;
		else
			ret = (data->size - reader->r_off) + data->w_off;
		break;
	case DATAGER_GET_NEXT_ENTRY_LEN:
		if (!(file->f_mode & FMODE_READ)) {
			ret = -EBADF;
			break;
		}
		reader = file->private_data;

		if (!reader->r_all)
			reader->r_off = get_next_entry_by_uid(data,
				reader->r_off, current_euid());

		if (data->w_off != reader->r_off)
			ret = get_hdr_len() +
				get_entry_msg_len(data, reader->r_off);
		else
			ret = 0;
		break;
	case DATAGER_FLUSH_DATA:
		if (!(file->f_mode & FMODE_WRITE)) {
			ret = -EBADF;
			break;
		}
		if (!(in_egroup_p(file_inode(file)->i_gid))) {
			ret = -EPERM;
			break;
		}
		list_for_each_entry(reader, &data->readers, list)
			reader->r_off = data->w_off;
		data->head = data->w_off;
		ret = 0;
		break;
	}

	mutex_unlock(&data->mutex);

	return ret;
}

static const struct file_operations data_fops = {
	.owner = THIS_MODULE,
	.write = data_write,
	.read = data_read,
	//.write_iter = data_write_iter,
	.poll = data_poll,
	.unlocked_ioctl = data_ioctl,
	.compat_ioctl = data_ioctl,
	.open = data_open,
	.release = data_release,
};

/*
 * Log size must must be a power of two, and greater than
 * (DATAGER_ENTRY_MAX_PAYLOAD + sizeof(struct data_entry)).
 */
static int __init create_data_buffer(char *data_name, int size)
{
	int ret = 0;
	struct data_buffer *data;
	unsigned char *buffer;

	buffer = vmalloc(size);
	if (buffer == NULL)
		return -ENOMEM;

	data = kzalloc(sizeof(struct data_buffer), GFP_KERNEL);
	if (data == NULL) {
		ret = -ENOMEM;
		goto out_free_buffer;
	}
	data->buffer = buffer;

	data->misc.minor = MISC_DYNAMIC_MINOR;
	data->misc.name = kstrdup(data_name, GFP_KERNEL);
	if (data->misc.name == NULL) {
		ret = -ENOMEM;
		goto out_free_data;
	}

	data->misc.fops = &data_fops;
	data->misc.parent = NULL;

	init_waitqueue_head(&data->wq);
	INIT_LIST_HEAD(&data->readers);
	mutex_init(&data->mutex);
	data->w_off = 0;
	data->head = 0;
	data->size = size;

	INIT_LIST_HEAD(&data->datas);
	list_add_tail(&data->datas, &data_list);

	/* finally, initialize the misc device for this data */
	ret = misc_register(&data->misc);
	if (unlikely(ret)) {
		pr_err("failed to register misc device for data '%s'!\n",
				data->misc.name);
		goto out_free_misc_name;
	}

	pr_info("created %luK data '%s'\n",
		(unsigned long) data->size >> 10, data->misc.name);

	return 0;

out_free_misc_name:
	kfree(data->misc.name);

out_free_data:
	kfree(data);

out_free_buffer:
	vfree(buffer);
	return ret;
}

static int __init data_init(void)
{
	int ret;

	ret = create_data_buffer(DATA_BUFFER_RECV, 256*1024);
	if (unlikely(ret))
		goto out;

	ret = create_data_buffer(DATA_BUFFER_SEND, 256*1024);
	if (unlikely(ret))
		goto out;
out:
	return ret;
}

static void __exit data_exit(void)
{
	struct data_buffer *current_data_buffer, *next_data_buffer;

	list_for_each_entry_safe(current_data_buffer, next_data_buffer, &data_list, datas) {
		/* we have to delete all the entry inside data_list */
		misc_deregister(&current_data_buffer->misc);
		vfree(current_data_buffer->buffer);
		kfree(current_data_buffer->misc.name);
		list_del(&current_data_buffer->datas);
		kfree(current_data_buffer);
	}
}


device_initcall(data_init);
module_exit(data_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joison, <joison_dome@sina.cn>");
MODULE_DESCRIPTION("Server data");
