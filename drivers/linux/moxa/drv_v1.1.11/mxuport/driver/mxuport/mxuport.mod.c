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
	{ 0xaa5b0f7, "kmalloc_caches" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x8578c61d, "mx_usbserial_disconnect" },
	{ 0xd0d8621b, "strlen" },
	{ 0x174dd5c1, "dev_set_drvdata" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0xa0c503ac, "mx_usbserial_register" },
	{ 0x8949858b, "schedule_work" },
	{ 0x670c0597, "down_interruptible" },
	{ 0x6729d3df, "__get_user_4" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0xb78c61e8, "param_ops_bool" },
	{ 0x5402d93b, "tty_buffer_request_room" },
	{ 0xd81fff49, "usb_unlink_urb" },
	{ 0xd2749e01, "mx_usbserial_probe" },
	{ 0xe174aa7, "__init_waitqueue_head" },
	{ 0x2be0364, "usb_get_descriptor" },
	{ 0x54b70d84, "dev_err" },
	{ 0x88941a06, "_raw_spin_unlock_irqrestore" },
	{ 0xfcf580f6, "current_task" },
	{ 0x43eba1e8, "usb_deregister" },
	{ 0xb2780962, "tty_get_baud_rate" },
	{ 0x50eedeb8, "printk" },
	{ 0xc1e61d8b, "tty_ldisc_flush" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0xb4390f9a, "mcount" },
	{ 0xf7d0a900, "usb_control_msg" },
	{ 0xdef8b776, "tty_insert_flip_string_fixed_flag" },
	{ 0xc6cbbc89, "capable" },
	{ 0x13513484, "_dev_info" },
	{ 0xeec0b3f0, "usb_submit_urb" },
	{ 0xb2fd5ceb, "__put_user_4" },
	{ 0xb3c8d99a, "tty_wait_until_sent" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xd62c833f, "schedule_timeout" },
	{ 0x86a4889a, "kmalloc_order_trace" },
	{ 0x6e712077, "kmem_cache_alloc_trace" },
	{ 0x587c70d8, "_raw_spin_lock_irqsave" },
	{ 0xf5e6d244, "mx_usbserial_deregister" },
	{ 0xf09c7f68, "__wake_up" },
	{ 0x37a0cba, "kfree" },
	{ 0x2e60bace, "memcpy" },
	{ 0xc1d03e45, "tty_check_change" },
	{ 0xe75663a, "prepare_to_wait" },
	{ 0x57b09822, "up" },
	{ 0xd28de0e8, "usb_register_driver" },
	{ 0xb00ccc33, "finish_wait" },
	{ 0x2ef6795e, "tty_flip_buffer_push" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0x3ce9820b, "dev_get_drvdata" },
	{ 0x4bbccf99, "usb_free_urb" },
	{ 0xf73853cb, "usb_alloc_urb" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=mxusbserial";

MODULE_ALIAS("usb:v110Ap1250d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v110Ap1251d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v110Ap1410d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v110Ap1450d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v110Ap1451d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v110Ap1618d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v110Ap1658d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v110Ap1613d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v110Ap1653d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "1B3C6C158294FAC5606F85F");
