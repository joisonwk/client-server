#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x6a91b2bc, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x68dd117b, __VMLINUX_SYMBOL_STR(misc_deregister) },
	{ 0x999e8297, __VMLINUX_SYMBOL_STR(vfree) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xff4d10c2, __VMLINUX_SYMBOL_STR(misc_register) },
	{ 0x2e6523b, __VMLINUX_SYMBOL_STR(__mutex_init) },
	{ 0x68dfc59f, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0xc499ae1e, __VMLINUX_SYMBOL_STR(kstrdup) },
	{ 0xd6ee688f, __VMLINUX_SYMBOL_STR(vmalloc) },
	{ 0x2f287f0d, __VMLINUX_SYMBOL_STR(copy_to_user) },
	{ 0x4292364c, __VMLINUX_SYMBOL_STR(schedule) },
	{ 0x75bb675a, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0x622fa02a, __VMLINUX_SYMBOL_STR(prepare_to_wait) },
	{ 0xc8b57c27, __VMLINUX_SYMBOL_STR(autoremove_wake_function) },
	{ 0xe45f60d8, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0x362ef408, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0x34184afe, __VMLINUX_SYMBOL_STR(current_kernel_time) },
	{ 0xe2491fc0, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0x2e60bace, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x8e28cd42, __VMLINUX_SYMBOL_STR(in_egroup_p) },
	{ 0x439a4a19, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xf5769951, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x81b7eec4, __VMLINUX_SYMBOL_STR(nonseekable_open) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xd1a87653, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0x4582cd80, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "A870EF184F3D6ACBE6A54C2");
