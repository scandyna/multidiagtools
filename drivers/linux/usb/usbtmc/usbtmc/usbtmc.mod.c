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
	{ 0x80567cab, "module_layout" },
	{ 0x43eba1e8, "usb_deregister" },
	{ 0xd28de0e8, "usb_register_driver" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0x19a0f2b6, "usb_reset_configuration" },
	{ 0xd0d8621b, "strlen" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0xecb9e7f, "usb_bulk_msg" },
	{ 0xf7d0a900, "usb_control_msg" },
	{ 0x174dd5c1, "dev_set_drvdata" },
	{ 0xcf83da5, "cdev_add" },
	{ 0x62b53a39, "cdev_init" },
	{ 0x6e712077, "kmem_cache_alloc_trace" },
	{ 0xaa5b0f7, "kmalloc_caches" },
	{ 0x3db697ee, "usb_get_dev" },
	{ 0x706aae9d, "cdev_del" },
	{ 0x3ce9820b, "dev_get_drvdata" },
	{ 0x37a0cba, "kfree" },
	{ 0x50eedeb8, "printk" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "84194BB6E47FAB883B3C313");
