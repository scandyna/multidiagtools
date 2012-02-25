/*
 * USB Serial Converter Bus specific functions
 *
 * Copyright (C) 2002 Greg Kroah-Hartman (greg@kroah.com)
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License version
 *	2 as published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/tty.h>
#include <linux/module.h>
#include <linux/usb.h>
#include "mxusb-serial.h"
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)) 	    
#ifndef FC6
#include <linux/config.h>
#endif
#endif

static int usb_serial_device_match (struct device *dev, struct device_driver *drv)
{
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
	struct usb_serial_driver *driver;
#else
	struct usb_serial_device_type *driver;
#endif
	const struct usb_serial_port *port;

	/*
	 * drivers are already assigned to ports in serial_probe so it's
	 * a simple check here.
	 */
	port = to_usb_serial_port(dev);
	if (!port)
		return 0;

	driver = to_usb_serial_driver(drv);

	if (driver == port->serial->type)
		return 1;

	return 0;
}

static int usb_serial_device_probe (struct device *dev)
{
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
	struct usb_serial_driver *driver;
#else
	struct usb_serial_device_type *driver;
#endif
	struct usb_serial_port *port;
	int retval = 0;
	int minor;

	port = to_usb_serial_port(dev);
	if (!port) {
		retval = -ENODEV;
		goto exit;
	}

	driver = port->serial->type;
	if (driver->port_probe) {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
		if (!try_module_get(driver->driver.owner)) {
#else
		if (!try_module_get(driver->owner)) {
#endif
			dev_err(dev, "module get failed, exiting\n");
			retval = -EIO;
			goto exit;
		}
		retval = driver->port_probe (port);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
		module_put(driver->driver.owner);
#else
		module_put(driver->owner);
#endif
		if (retval)
			goto exit;
	}

	minor = port->number;
	tty_register_device (mx_usbserial_tty_driver, minor, dev);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
	dev_info(&port->serial->dev->dev, 
		 "%s converter now attached to ttyMXUSB%d\n",
		 driver->description, minor);
#else
	dev_info(&port->serial->dev->dev, 
		 "%s converter now attached to ttyMXUSB%d\n",
		 driver->name, minor);
#endif
exit:
	return retval;
}

static int usb_serial_device_remove (struct device *dev)
{
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
	struct usb_serial_driver *driver;
#else
	struct usb_serial_device_type *driver;
#endif
	struct usb_serial_port *port;
	int retval = 0;
	int minor;

	port = to_usb_serial_port(dev);
	if (!port) {
		return -ENODEV;
	}

	driver = port->serial->type;
	if (driver->port_remove) {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
		if (!try_module_get(driver->driver.owner)) {
#else
		if (!try_module_get(driver->owner)) {
#endif
			dev_err(dev, "module get failed, exiting\n");
			retval = -EIO;
			goto exit;
		}
		retval = driver->port_remove (port);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
		module_put(driver->driver.owner);
#else
		module_put(driver->owner);
#endif
	}
exit:
	minor = port->number;
	tty_unregister_device (mx_usbserial_tty_driver, minor);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
	dev_info(dev, "%s converter now disconnected from ttyMXUSB%d\n",
		 driver->description, minor);
#else
	dev_info(dev, "%s converter now disconnected from ttyMXUSB%d\n",
		 driver->name, minor);
#endif
	return retval;
}

struct bus_type mx_usb_serial_bus_type = {
	.name =		"mxusbserial",
	.match =	usb_serial_device_match,
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15))
	.probe =	usb_serial_device_probe,
	.remove =	usb_serial_device_remove,
#endif
};


#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
int mx_usbserial_bus_register(struct usb_serial_driver *driver)
#else
int mx_usbserial_bus_register(struct usb_serial_device_type *driver)
#endif
{
	int retval;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,15)) 	    
	if (driver->short_name)
		driver->driver.name = (char *)driver->short_name;
	else
		driver->driver.name = (char *)driver->name;

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9)) 	    
	driver->driver.owner = driver->owner;
#endif
#endif
	
	driver->driver.bus = &mx_usb_serial_bus_type;
	
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16))
	driver->driver.probe = usb_serial_device_probe;
	driver->driver.remove = usb_serial_device_remove;
#endif

	retval = driver_register(&driver->driver);

	return retval;
}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
void mx_usbserial_bus_deregister(struct usb_serial_driver *driver)
#else
void mx_usbserial_bus_deregister(struct usb_serial_device_type *driver)
#endif
{
	driver_unregister (&driver->driver);
}

