/* include/linux/logger.h
 *
 * Copyright (C) 2007-2008 Google, Inc.
 * Author: Robert Love <rlove@android.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _LINUX_DATAGER_H
#define _LINUX_DATAGER_H

#include <linux/types.h>
#include <linux/ioctl.h>

/**
 * struct user_logger_entry_compat - defines a single entry that is given to a logger
 * @len:	The length of the payload
 * @__pad:	Two bytes of padding that appear to be required
 * @pid:	The generating process' process ID
 * @tid:	The generating process' thread ID
 * @sec:	The number of seconds that have elapsed since the Epoch
 * @nsec:	The number of nanoseconds that have elapsed since @sec
 * @msg:	The message that is to be logged
 *
 * The userspace structure for version 1 of the logger_entry ABI.
 * This structure is returned to userspace unless the caller requests
 * an upgrade to a newer ABI version.
 */
struct user_logger_entry_compat {
	__u16		len;
	__u16		__pad;
	__s32		pid;
	__s32		tid;
	__s32		sec;
	__s32		nsec;
	char		msg[0];
};

/**
 * struct logger_entry - defines a single entry that is given to a logger
 * @len:	The length of the payload
 * @hdr_size:	sizeof(struct logger_entry_v2)
 * @pid:	The generating process' process ID
 * @tid:	The generating process' thread ID
 * @sec:	The number of seconds that have elapsed since the Epoch
 * @nsec:	The number of nanoseconds that have elapsed since @sec
 * @euid:	Effective UID of logger
 * @msg:	The message that is to be logged
 *
 * The structure for version 2 of the logger_entry ABI.
 * This structure is returned to userspace if ioctl(DATAGER_SET_VERSION)
 * is called with version >= 2
 */
struct data_entry {
	__u16		len;
	__u16		hdr_size;
	__s32		pid;
	__s32		tid;
	int		fd;
	int		type;
	__s32		sec;
	__s32		nsec;
	kuid_t		euid;
	char		msg[0];
};

#define DATA_BUFFER_RECV	"data_recv"	/* radio-related messages */
#define DATA_BUFFER_SEND	"data_send"	/* system/hardware events */

#define DATAGER_ENTRY_MAX_PAYLOAD	4076

#define __DATAGERIO	0xAE

#define DATAGER_GET_DATA_BUF_SIZE		_IO(__DATAGERIO, 1) /* size of log */
#define DATAGER_GET_DATA_LEN		_IO(__DATAGERIO, 2) /* used log len */
#define DATAGER_GET_NEXT_ENTRY_LEN	_IO(__DATAGERIO, 3) /* next entry len */
#define DATAGER_FLUSH_DATA		_IO(__DATAGERIO, 4) /* flush log */

#endif /* _LINUX_DATAGER_H */
