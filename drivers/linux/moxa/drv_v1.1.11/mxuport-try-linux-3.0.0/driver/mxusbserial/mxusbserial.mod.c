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
	{ 0xb52b6eab, "bus_register" },
	{ 0xaa5b0f7, "kmalloc_caches" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xd5b037e1, "kref_put" },
	{ 0x5ef8c111, "driver_register" },
	{ 0x174dd5c1, "dev_set_drvdata" },
	{ 0xdc5f2bc0, "alloc_tty_driver" },
	{ 0xd55071e4, "usb_kill_urb" },
	{ 0x6dbca59e, "tty_register_driver" },
	{ 0xb78c61e8, "param_ops_bool" },
	{ 0xc27eb257, "mutex_unlock" },
	{ 0xa0391a7, "put_tty_driver" },
	{ 0xeff548ea, "tty_set_operations" },
	{ 0xb7c26ad9, "device_register" },
	{ 0x54b70d84, "dev_err" },
	{ 0x43eba1e8, "usb_deregister" },
	{ 0xfc676416, "mutex_lock_interruptible" },
	{ 0x1c7ab1d1, "__mutex_init" },
	{ 0x50eedeb8, "printk" },
	{ 0x62cd8754, "__tracepoint_module_get" },
	{ 0x5f6b9cd9, "driver_unregister" },
	{ 0xb4390f9a, "mcount" },
	{ 0x84a896a2, "mutex_lock" },
	{ 0x4764cc54, "bus_unregister" },
	{ 0x40b10cb2, "usb_match_id" },
	{ 0xdfa7855a, "tty_register_device" },
	{ 0x375c70dc, "module_put" },
	{ 0x68283c12, "tty_unregister_device" },
	{ 0x13513484, "_dev_info" },
	{ 0x67b27ec1, "tty_std_termios" },
	{ 0x3db697ee, "usb_get_dev" },
	{ 0x5bf65cec, "usb_put_dev" },
	{ 0x6e712077, "kmem_cache_alloc_trace" },
	{ 0x6312237, "tty_unregister_driver" },
	{ 0x2675762e, "tty_hangup" },
	{ 0x83800bfa, "kref_init" },
	{ 0x37a0cba, "kfree" },
	{ 0x9775cdc, "kref_get" },
	{ 0xd28de0e8, "usb_register_driver" },
	{ 0x54202b00, "device_unregister" },
	{ 0x8dbedceb, "dev_set_name" },
	{ 0x9e55ed6, "tty_wakeup" },
	{ 0x3ce9820b, "dev_get_drvdata" },
	{ 0x4bbccf99, "usb_free_urb" },
	{ 0xf73853cb, "usb_alloc_urb" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "74F77B097B8B08EB8CAD6DC");
