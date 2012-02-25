/*
 * USB Serial Converter driver
 *
 * Copyright (C) 1999 - 2005 Greg Kroah-Hartman (greg@kroah.com)
 * Copyright (C) 2000 Peter Berger (pberger@brimson.com)
 * Copyright (C) 2000 Al Borchers (borchers@steinerpoint.com)
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License version
 *	2 as published by the Free Software Foundation.
 *
 * This driver was originally based on the ACM driver by Armin Fuerst (which was
 * based on a driver by Brad Keryan)
 *
 * See Documentation/usb/usb-serial.txt for more information on using this driver
 *
 */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <asm/uaccess.h>
#include <linux/usb.h>
#include "mxusb-serial.h"

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39))
#include <linux/smp_lock.h>
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19))
#ifndef FC6
#include <linux/config.h>
#endif
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15))
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17))
#include <asm/semaphore.h>
#endif
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,16))
#include <linux/mutex.h>
#endif

/*
 * Version Information
 */
#define DRIVER_AUTHOR "Greg Kroah-Hartman, greg@kroah.com, http://www.kroah.com/linux/"
#define DRIVER_DESC "USB Serial Driver core"


/* Driver structure we register with the USB core */
static struct usb_driver usb_serial_driver = {
	.name =		"mxusbserial",
	.probe =	mx_usbserial_probe,
	.disconnect =	mx_usbserial_disconnect,
#if (defined(_SCREEN_INFO_H) || (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15)))
	.no_dynamic_id = 	1,
#else
	.owner =	THIS_MODULE,	
#endif	
};

/* There is no MODULE_DEVICE_TABLE for usbserial.c.  Instead
   the MODULE_DEVICE_TABLE declarations in each serial driver
   cause the "hotplug" program to pull in whatever module is necessary
   via modprobe, and modprobe will load usbserial because the serial
   drivers depend on it.
*/

static int debug;
static struct usb_serial *serial_table[SERIAL_TTY_MINORS];	/* initially all NULL */
static LIST_HEAD(usb_serial_driver_list);

struct usb_serial *mx_usbserial_get_by_index(unsigned index)
{
	struct usb_serial *serial = serial_table[index];

	if (serial)
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,4))
		kref_get(&serial->kref);
#else
		kobject_get (&serial->kobj);
#endif

	return serial;
}

static struct usb_serial *get_free_serial (struct usb_serial *serial, int num_ports, unsigned int *minor)
{
	unsigned int i, j;
	int good_spot;

	dbg("%s %d", __FUNCTION__, num_ports);

	*minor = 0;
	for (i = 0; i < SERIAL_TTY_MINORS; ++i) {
		if (serial_table[i])
			continue;

		good_spot = 1;
		for (j = 1; j <= num_ports-1; ++j)
			if ((i+j >= SERIAL_TTY_MINORS) || (serial_table[i+j])) {
				good_spot = 0;
				i += j;
				break;
			}
		if (good_spot == 0)
			continue;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,7))			
		serial->magic = USB_SERIAL_MAGIC;
#endif		
		*minor = i;
		dbg("%s - minor base = %d", __FUNCTION__, *minor);
		for (i = *minor; (i < (*minor + num_ports)) && (i < SERIAL_TTY_MINORS); ++i)
			serial_table[i] = serial;
		return serial;
	}
	return NULL;
}

static void return_serial(struct usb_serial *serial)
{
	int i;

	dbg("%s", __FUNCTION__);

	if (serial == NULL)
		return;

	for (i = 0; i < serial->num_ports; ++i) {
		serial_table[serial->minor + i] = NULL;
	}
}


#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,4))
static void destroy_serial(struct kref *kref)
#else
static void destroy_serial (struct kobject *kobj)
#endif
{
	struct usb_serial *serial;
	struct usb_serial_port *port;
	int i;

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,4))
	serial = to_usb_serial(kref);
	
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
	dbg("%s - %s", __FUNCTION__, serial->type->description);
#else
	dbg ("%s - %s", __FUNCTION__, serial->type->name);
#endif
#else	
	dbg ("%s - %s", __FUNCTION__, kobj->name);
	serial = to_usb_serial(kobj);
#endif	


	serial->type->shutdown(serial);

	/* return the minor range that this device had */
	return_serial(serial);

	for (i = 0; i < serial->num_ports; ++i)
		serial->port[i]->open_count = 0;

	/* the ports are cleaned up and released in port_release() */
	for (i = 0; i < serial->num_ports; ++i)
		if (serial->port[i]->dev.parent != NULL) {
			device_unregister(&serial->port[i]->dev);
			serial->port[i] = NULL;
		}

	/* If this is a "fake" port, we have to clean it up here, as it will
	 * not get cleaned up in port_release() as it was never registered with
	 * the driver core */
	if (serial->num_ports < serial->num_port_pointers) {
		for (i = serial->num_ports; i < serial->num_port_pointers; ++i) {
			port = serial->port[i];
			if (!port)
				continue;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))			
			usb_kill_urb(port->read_urb);
			usb_free_urb(port->read_urb);
			usb_kill_urb(port->write_urb);
			usb_free_urb(port->write_urb);
			usb_kill_urb(port->interrupt_in_urb);
			usb_free_urb(port->interrupt_in_urb);
			usb_kill_urb(port->interrupt_out_urb);
			usb_free_urb(port->interrupt_out_urb);
			kfree(port->bulk_in_buffer);
			kfree(port->bulk_out_buffer);
			kfree(port->interrupt_in_buffer);
			kfree(port->interrupt_out_buffer);
#else				
			if (port->read_urb) {
				usb_unlink_urb(port->read_urb);
				usb_free_urb(port->read_urb);
			}
			if (port->write_urb) {
				usb_unlink_urb(port->write_urb);
				usb_free_urb(port->write_urb);
			}
			if (port->interrupt_in_urb) {
				usb_unlink_urb(port->interrupt_in_urb);
				usb_free_urb(port->interrupt_in_urb);
			}
			kfree(port->bulk_in_buffer);
			kfree(port->bulk_out_buffer);
			kfree(port->interrupt_in_buffer);
#endif			
		}
	}

	usb_put_dev(serial->dev);

	/* free up any memory that we allocated */
	kfree (serial);
}


/*****************************************************************************
 * Driver tty interface functions
 *****************************************************************************/
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,16))
static int serial_open (struct tty_struct *tty, struct file * filp)
{
	struct usb_serial *serial;
	struct usb_serial_port *port;
	unsigned int portNumber;
	int retval;
	
	dbg("%s", __FUNCTION__);

	/* get the serial object associated with this tty pointer */
	serial = mx_usbserial_get_by_index(tty->index);
	if (!serial) {
		tty->driver_data = NULL;
		return -ENODEV;
	}

	portNumber = tty->index - serial->minor;
	port = serial->port[portNumber];
	if (!port) {
		retval = -ENODEV;
		goto bailout_kref_put;
	}

	if (mutex_lock_interruptible(&port->mutex)) {
		retval = -ERESTARTSYS;
		goto bailout_kref_put;
	}
	 
	++port->open_count;

	if (port->open_count == 1) {

		/* set up our port structure making the tty driver
		 * remember our port object, and us it */
		tty->driver_data = port;
		port->tty = tty;

		/* lock this module before we call it
		 * this may fail, which means we must bail out,
		 * safe because we are called with BKL held */
		if (!try_module_get(serial->type->driver.owner)) {
			retval = -ENODEV;
			goto bailout_mutex_unlock;
		}

		/* only call the device specific open if this 
		 * is the first time the port is opened */
		retval = serial->type->open(port, filp);
		if (retval)
			goto bailout_module_put;
	}

	mutex_unlock(&port->mutex);
	return 0;

bailout_module_put:
	module_put(serial->type->driver.owner);
bailout_mutex_unlock:
	port->open_count = 0;
	mutex_unlock(&port->mutex);
bailout_kref_put:
	kref_put(&serial->kref, destroy_serial);
	return retval;
}
#else
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))			
static int serial_open (struct tty_struct *tty, struct file * filp)
{
	struct usb_serial *serial;
	struct usb_serial_port *port;
	unsigned int portNumber;
	int retval;
	
	dbg("%s", __FUNCTION__);

	/* get the serial object associated with this tty pointer */
	serial = mx_usbserial_get_by_index(tty->index);
	if (!serial) {
		tty->driver_data = NULL;
		return -ENODEV;
	}

	portNumber = tty->index - serial->minor;
	port = serial->port[portNumber];
	if (!port)
		return -ENODEV;

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15))
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17))
	if (down_interruptible(&port->sem))
		return -ERESTARTSYS;
#endif
#endif
	 
	++port->open_count;

	if (port->open_count == 1) {

		/* set up our port structure making the tty driver
		 * remember our port object, and us it */
		tty->driver_data = port;
		port->tty = tty;

		/* lock this module before we call it
		 * this may fail, which means we must bail out,
		 * safe because we are called with BKL held */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
		if (!try_module_get(serial->type->driver.owner)) {
#else
		if (!try_module_get(serial->type->owner)) {
#endif		    
			retval = -ENODEV;
			goto bailout_kref_put;
		}

		/* only call the device specific open if this 
		 * is the first time the port is opened */
		retval = serial->type->open(port, filp);
		if (retval)
			goto bailout_module_put;
	}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15))
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17))
	up(&port->sem);
#endif
#endif
	
	return 0;

bailout_module_put:
	module_put(serial->type->driver.owner);
bailout_kref_put:
	kref_put(&serial->kref, destroy_serial);
	port->open_count = 0;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15))
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17))
	up(&port->sem);
#endif
#endif	
	return retval;
}
#else
static int serial_open (struct tty_struct *tty, struct file * filp)
{
	struct usb_serial *serial;
	struct usb_serial_port *port;
	unsigned int portNumber;
	int retval = 0;
	
	dbg("%s", __FUNCTION__);

	/* initialize the pointer incase something fails */
	tty->driver_data = NULL;

	/* get the serial object associated with this tty pointer */
	serial = mx_usbserial_get_by_index(tty->index);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,6))
	if (!serial) {
		retval = -ENODEV;
		goto bailout;
	}
#else
	if (serial_paranoia_check (serial, __FUNCTION__))
		return -ENODEV;
#endif

	/* set up our port structure making the tty driver remember our port object, and us it */
	portNumber = tty->index - serial->minor;
	port = serial->port[portNumber];
	tty->driver_data = port;

	port->tty = tty;
	 
	/* lock this module before we call it,
	   this may, which means we must bail out, safe because we are called with BKL held */
	if (!try_module_get(serial->type->owner)) {
		retval = -ENODEV;
		goto bailout;
	}

	++port->open_count;
	if (port->open_count == 1) {
		/* only call the device specific open if this 
		 * is the first time the port is opened */
		retval = serial->type->open(port, filp);
		if (retval) {
			port->open_count = 0;
			module_put(serial->type->owner);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,4))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,8)) 			
			kref_put(&serial->kref, destroy_serial);
#else
			kref_put(&serial->kref);			
#endif
#else
		    kobject_put (&serial->kobj);
#endif

		}
	}
bailout:
	return retval;
}
#endif
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,10))
static void serial_close(struct tty_struct *tty, struct file * filp)
{
	struct usb_serial_port *port = tty->driver_data;

	if (!port)
		return;

	dbg("%s - port %d", __FUNCTION__, port->number);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15))
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17))
	down(&port->sem);

	if (port->open_count == 0) {
		up(&port->sem);
		return;
	}
#endif
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,16))
	mutex_lock(&port->mutex);

	if (port->open_count == 0) {
		mutex_unlock(&port->mutex);
		return;
	}
#endif

	--port->open_count;
	if (port->open_count == 0) {
		/* only call the device specific close if this 
		 * port is being closed by the last owner */
		port->serial->type->close(port, filp);

		if (port->tty) {
			if (port->tty->driver_data)
				port->tty->driver_data = NULL;
			port->tty = NULL;
		}

		module_put(port->serial->type->driver.owner);
	}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15))
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17))
	up(&port->sem);
#endif
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,16))
	mutex_unlock(&port->mutex);
#endif

	kref_put(&port->serial->kref, destroy_serial);
}
#else
static void serial_close(struct tty_struct *tty, struct file * filp)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;

	if (!port)
		return;

	dbg("%s - port %d", __FUNCTION__, port->number);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
	if (port->open_count == 0)
		return;

	--port->open_count;
	if (port->open_count == 0) {
		/* only call the device specific close if this 
		 * port is being closed by the last owner */
		port->serial->type->close(port, filp);
#else
	--port->open_count;
	if (port->open_count <= 0) {
		/* only call the device specific close if this 
		 * port is being closed by the last owner */
		port->serial->type->close(port, filp);
		port->open_count = 0;
#endif
		if (port->tty) {
			if (port->tty->driver_data)
				port->tty->driver_data = NULL;
			port->tty = NULL;
		}

		module_put(port->serial->type->owner);
	}
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,4))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,8)) 			
			kref_put(&port->serial->kref, destroy_serial);
#else
			kref_put(&port->serial->kref);			
#endif
#else
		    kobject_put (&port->serial->kobj);
#endif

}
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
static int serial_write (struct tty_struct * tty, const unsigned char *buf, int count)
#else
static int serial_write (struct tty_struct * tty, int from_user, const unsigned char *buf, int count)
#endif
{
	struct usb_serial_port *port = tty->driver_data;
	int retval = -EINVAL;

	if (!port)
		goto exit;

	dbg("%s - port %d, %d byte(s)", __FUNCTION__, port->number, count);

	if (!port->open_count) {
		dbg("%s - port not opened", __FUNCTION__);
		goto exit;
	}

	/* pass on to the driver specific version of this function */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
	retval = port->serial->type->write(port, buf, count);
#else	
	retval = port->serial->type->write(port, from_user, buf, count);
#endif
exit:
	return retval;
}

static int serial_write_room (struct tty_struct *tty) 
{
	struct usb_serial_port *port = tty->driver_data;
	int retval = -EINVAL;

	if (!port)
		goto exit;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg("%s - port not open", __FUNCTION__);
		goto exit;
	}

	/* pass on to the driver specific version of this function */
	retval = port->serial->type->write_room(port);

exit:
	return retval;
}

static int serial_chars_in_buffer (struct tty_struct *tty) 
{
	struct usb_serial_port *port = tty->driver_data;
	int retval = -EINVAL;

	if (!port)
		goto exit;

	dbg("%s = port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg("%s - port not open", __FUNCTION__);
		goto exit;
	}

	/* pass on to the driver specific version of this function */
	retval = port->serial->type->chars_in_buffer(port);

exit:
	return retval;
}

static void serial_throttle (struct tty_struct * tty)
{
	struct usb_serial_port *port = tty->driver_data;

	if (!port)
		return;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg ("%s - port not open", __FUNCTION__);
		return;
	}

	/* pass on to the driver specific version of this function */
	if (port->serial->type->throttle)
		port->serial->type->throttle(port);
}

static void serial_unthrottle (struct tty_struct * tty)
{
	struct usb_serial_port *port = tty->driver_data;

	if (!port)
		return;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg("%s - port not open", __FUNCTION__);
		return;
	}

	/* pass on to the driver specific version of this function */
	if (port->serial->type->unthrottle)
		port->serial->type->unthrottle(port);
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39))
static int serial_ioctl (struct tty_struct *tty, struct file * file, unsigned int cmd, unsigned long arg)
#else
static int serial_ioctl (struct tty_struct *tty, unsigned int cmd, unsigned long arg)
#endif
{
	struct usb_serial_port *port = tty->driver_data;
	int retval = -ENODEV;

	if (!port)
		goto exit;

	dbg("%s - port %d, cmd 0x%.4x", __FUNCTION__, port->number, cmd);

	if (!port->open_count) {
		dbg ("%s - port not open", __FUNCTION__);
		goto exit;
	}

	/* pass on to the driver specific version of this function if it is available */
	if (port->serial->type->ioctl)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39))
		retval = port->serial->type->ioctl(port, file, cmd, arg);
#else
		retval = port->serial->type->ioctl(port, cmd, arg);
#endif
	else
		retval = -ENOIOCTLCMD;

exit:
	return retval;
}


#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))
static void serial_set_termios (struct tty_struct *tty, struct ktermios * old)
#else
static void serial_set_termios (struct tty_struct *tty, struct termios * old)
#endif
{
	struct usb_serial_port *port = tty->driver_data;

	if (!port)
		return;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg("%s - port not open", __FUNCTION__);
		return;
	}

	/* pass on to the driver specific version of this function if it is available */
	if (port->serial->type->set_termios)
		port->serial->type->set_termios(port, old);
}


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28))
static void serial_break (struct tty_struct *tty, int break_state)
{
	struct usb_serial_port *port = tty->driver_data;

	if (!port)
		return ;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg("%s - port not open", __FUNCTION__);
		return ;
	}

	/* pass on to the driver specific version of this function if it is available */
	if (port->serial->type->break_ctl)
		port->serial->type->break_ctl(port, break_state);
}

#else
static int serial_break (struct tty_struct *tty, int break_state)
{
	struct usb_serial_port *port = tty->driver_data;

	if (!port)
		return 0;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg("%s - port not open", __FUNCTION__);
		return 0;
	}

	/* pass on to the driver specific version of this function if it is available */
	if (port->serial->type->break_ctl)
		port->serial->type->break_ctl(port, break_state);
	
	return 0;
}
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
static int serial_read_proc (char *page, char **start, off_t off, int count, int *eof, void *data)
{
	struct usb_serial *serial;
	int length = 0;
	int i;
	off_t begin = 0;
	char tmp[40];

	dbg("%s", __FUNCTION__);
	length += sprintf (page, "usbserinfo:1.0 driver:2.0\n");
	for (i = 0; i < SERIAL_TTY_MINORS && length < PAGE_SIZE; ++i) {
		serial = mx_usbserial_get_by_index(i);
		if (serial == NULL)
			continue;

		length += sprintf (page+length, "%d:", i);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
		if (serial->type->driver.owner)
			length += sprintf (page+length, " module:%s", module_name(serial->type->driver.owner));
		length += sprintf (page+length, " name:\"%s\"", serial->type->description);
#else
		if (serial->type->owner)
			length += sprintf (page+length, " module:%s", module_name(serial->type->owner));
		length += sprintf (page+length, " name:\"%s\"", serial->type->name);		    		    
#endif		    
		length += sprintf (page+length, " vendor:%04x product:%04x", 
				   le16_to_cpu(serial->dev->descriptor.idVendor), 
				   le16_to_cpu(serial->dev->descriptor.idProduct));
		length += sprintf (page+length, " num_ports:%d", serial->num_ports);
		length += sprintf (page+length, " port:%d", i - serial->minor + 1);

		usb_make_path(serial->dev, tmp, sizeof(tmp));
		length += sprintf (page+length, " path:%s", tmp);
			
		length += sprintf (page+length, "\n");
		if ((length + begin) > (off + count))
			goto done;
		if ((length + begin) < off) {
			begin += length;
			length = 0;
		}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,4))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,8)) 			
			kref_put(&serial->kref, destroy_serial);
#else
			kref_put(&serial->kref);			
#endif
#else
		    kobject_put (&serial->kobj);
#endif

    }
	*eof = 1;
done:
	if (off >= (length + begin))
		return 0;
	*start = page + (off-begin);
	return ((count < begin+length-off) ? count : begin+length-off);
}
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39))
static int serial_tiocmget (struct tty_struct *tty, struct file *file)
#else
static int serial_tiocmget (struct tty_struct *tty)
#endif
{
	struct usb_serial_port *port = tty->driver_data;

	if (!port)
		goto exit;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg("%s - port not open", __FUNCTION__);
		goto exit;
	}

	if (port->serial->type->tiocmget)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39))
		return port->serial->type->tiocmget(port, file);
#else
		return port->serial->type->tiocmget(port);
#endif
exit:
	return -EINVAL;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39))
static int serial_tiocmset (struct tty_struct *tty, struct file *file,
			    unsigned int set, unsigned int clear)
#else
static int serial_tiocmset (struct tty_struct *tty,
			    unsigned int set, unsigned int clear)
#endif
{
	struct usb_serial_port *port = tty->driver_data;

	if (!port)
		goto exit;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg("%s - port not open", __FUNCTION__);
		goto exit;
	}

	if (port->serial->type->tiocmset)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39))
		return port->serial->type->tiocmset(port, file, set, clear);
#else
		return port->serial->type->tiocmset(port, set, clear);
#endif
exit:
	return -EINVAL;
}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))
void mx_usbserial_port_softint(struct work_struct *work)
{
	struct usb_serial_port *port = 
               container_of(work, struct usb_serial_port, work);
#else
void mx_usbserial_port_softint(void *private)
{
	struct usb_serial_port *port = private;
#endif
	struct tty_struct *tty;

	dbg("%s - port %d", __FUNCTION__, port->number);
	
	if (!port)
		return;

	tty = port->tty;
	if (!tty)
		return;
		
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,8))
	tty_wakeup(tty);
#else
	if ((tty->flags & (1 << TTY_DO_WRITE_WAKEUP)) && tty->ldisc.write_wakeup) {
		dbg("%s - write wakeup call.", __FUNCTION__);
		(tty->ldisc.write_wakeup)(tty);
	}

	wake_up_interruptible(&tty->write_wait);
#endif
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,5))
static struct kobj_type usb_serial_kobj_type = {
	.release = destroy_serial,
};
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
static void port_release(struct device *dev)
{
	struct usb_serial_port *port = to_usb_serial_port(dev);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
	dbg ("%s - %s", __FUNCTION__, dev->bus_id);
#else

	dbg ("%s - %s", __FUNCTION__, dev_name(dev));
#endif
	usb_kill_urb(port->read_urb);
	usb_free_urb(port->read_urb);
	usb_kill_urb(port->write_urb);
	usb_free_urb(port->write_urb);
	usb_kill_urb(port->interrupt_in_urb);
	usb_free_urb(port->interrupt_in_urb);
	usb_kill_urb(port->interrupt_out_urb);
	usb_free_urb(port->interrupt_out_urb);
	kfree(port->bulk_in_buffer);
	kfree(port->bulk_out_buffer);
	kfree(port->interrupt_in_buffer);
	kfree(port->interrupt_out_buffer);
	kfree(port);
}
#else
static void port_release(struct device *dev)
{
	struct usb_serial_port *port = to_usb_serial_port(dev);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30))
	dbg ("%s - %s", __FUNCTION__, dev->bus_id);
#else

	dbg ("%s - %s", __FUNCTION__, dev_name(dev));
#endif
	if (port->read_urb) {
		usb_unlink_urb(port->read_urb);
		usb_free_urb(port->read_urb);
	}
	if (port->write_urb) {
		usb_unlink_urb(port->write_urb);
		usb_free_urb(port->write_urb);
	}
	if (port->interrupt_in_urb) {
		usb_unlink_urb(port->interrupt_in_urb);
		usb_free_urb(port->interrupt_in_urb);
	}
	kfree(port->bulk_in_buffer);
	kfree(port->bulk_out_buffer);
	kfree(port->interrupt_in_buffer);
	kfree(port);
}
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
static struct usb_serial * create_serial (struct usb_device *dev, 
					  struct usb_interface *interface,
					  struct usb_serial_driver *driver)
#else					  
static struct usb_serial * create_serial (struct usb_device *dev, 
					  struct usb_interface *interface,
					  struct usb_serial_device_type *driver)
#endif					  
{
	struct usb_serial *serial;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17))
	serial = kmalloc(sizeof(*serial), GFP_KERNEL);
#else
	serial = kzalloc (sizeof (*serial), GFP_KERNEL);
#endif
	if (!serial) {
		dev_err(&dev->dev, "%s - out of memory\n", __FUNCTION__);
		return NULL;
	}
	
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17))
	memset (serial, 0, sizeof(*serial));
#endif	
	serial->dev = usb_get_dev(dev);
	serial->type = driver;
	serial->interface = interface;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,10))
	serial->vendor = dev->descriptor.idVendor;
	serial->product = dev->descriptor.idProduct;
#endif
	
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,4))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,8)) 			
	kref_init(&serial->kref);
#else
	kref_init(&serial->kref, destroy_serial);
#endif
#else
	kobject_init(&serial->kobj);
	serial->kobj.ktype = &usb_serial_kobj_type;
#endif

	return serial;
}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
static struct usb_serial_driver *search_serial_device(struct usb_interface *iface)
#else
static struct usb_serial_device_type *search_serial_device(struct usb_interface *iface)
#endif
{
	struct list_head *p;
	const struct usb_device_id *id;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
	struct usb_serial_driver *t;
#else
	struct usb_serial_device_type *t;
#endif
	/* Check if the usb id matches a known device */
	list_for_each(p, &usb_serial_driver_list) {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
		t = list_entry(p, struct usb_serial_driver, driver_list);
#else
		t = list_entry(p, struct usb_serial_device_type, driver_list);
#endif
		id = usb_match_id(iface, t->id_table);
		if (id != NULL) {
			dbg("descriptor matches");
			return t;
		}
	}

	return NULL;
}
#endif

int mx_usbserial_probe(struct usb_interface *interface,
			       const struct usb_device_id *id)
{
	struct usb_device *dev = interface_to_usbdev (interface);
	struct usb_serial *serial = NULL;
	struct usb_serial_port *port;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	struct usb_endpoint_descriptor *interrupt_in_endpoint[MAX_NUM_PORTS];
	struct usb_endpoint_descriptor *bulk_in_endpoint[MAX_NUM_PORTS];
	struct usb_endpoint_descriptor *bulk_out_endpoint[MAX_NUM_PORTS];
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
	struct usb_serial_driver *type = NULL;
#else
	struct usb_serial_device_type *type = NULL;	
#endif	
	int retval;
	int minor;
	int buffer_size;
	int i;
	int num_interrupt_in = 0;
	int num_bulk_in = 0;
	int num_bulk_out = 0;
	int num_ports = 0;
	int max_endpoints;
	
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
	struct usb_endpoint_descriptor *interrupt_out_endpoint[MAX_NUM_PORTS];
	int num_interrupt_out = 0;
	
	type = search_serial_device(interface);
	if (!type) {
#else	
	int found;
	struct list_head *tmp;
	const struct usb_device_id *id_pattern = NULL;

	/* loop through our list of known serial converters, and see if this
	   device matches. */
	found = 0;
	list_for_each (tmp, &usb_serial_driver_list) {
		type = list_entry(tmp, struct usb_serial_device_type, driver_list);
		id_pattern = usb_match_id(interface, type->id_table);
		if (id_pattern != NULL) {
			dbg("descriptor matches");
			found = 1;
			break;
		}
	}
	if (!found) {
#endif
		dbg("none matched");
		return -ENODEV;
	}

	serial = create_serial (dev, interface, type);
	if (!serial) {
		dev_err(&interface->dev, "%s - out of memory\n", __FUNCTION__);
		return -ENOMEM;
	}

	/* if this device type has a probe function, call it */
	if (type->probe) {

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
		const struct usb_device_id *id;

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
		if (!try_module_get(type->driver.owner)) {
#else
		if (!try_module_get(type->owner)) {
#endif
			dev_err(&interface->dev, "module get failed, exiting\n");
			kfree (serial);
			return -EIO;
		}

		id = usb_match_id(interface, type->id_table);
		retval = type->probe(serial, id);
#else	    
		if (!try_module_get(type->owner)) {
			dev_err(&interface->dev, "module get failed, exiting\n");
			kfree (serial);
			return -EIO;
		}
		retval = type->probe (serial, id_pattern);
#endif
			
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
		module_put(type->driver.owner);
#else
		module_put(type->owner);
#endif

		if (retval) {
			dbg ("sub driver rejected device");
			kfree (serial);
			return retval;
		}
	}

	/* descriptor matches, let's find the endpoints needed */
	/* check out the endpoints */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,4))
	iface_desc = interface->cur_altsetting;
#else
	iface_desc = &interface->altsetting[0];
#endif
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;
		
		if ((endpoint->bEndpointAddress & 0x80) &&
		    ((endpoint->bmAttributes & 3) == 0x02)) {
			/* we found a bulk in endpoint */
			dbg("found bulk in on endpoint %d", i);
			bulk_in_endpoint[num_bulk_in] = endpoint;
			++num_bulk_in;
		}

		if (((endpoint->bEndpointAddress & 0x80) == 0x00) &&
		    ((endpoint->bmAttributes & 3) == 0x02)) {
			/* we found a bulk out endpoint */
			dbg("found bulk out on endpoint %d", i);
			bulk_out_endpoint[num_bulk_out] = endpoint;
			++num_bulk_out;
		}
		
		if ((endpoint->bEndpointAddress & 0x80) &&
		    ((endpoint->bmAttributes & 3) == 0x03)) {
			/* we found a interrupt in endpoint */
			dbg("found interrupt in on endpoint %d", i);
			interrupt_in_endpoint[num_interrupt_in] = endpoint;
			++num_interrupt_in;
		}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
		if (((endpoint->bEndpointAddress & 0x80) == 0x00) &&
		    ((endpoint->bmAttributes & 3) == 0x03)) {
			/* we found an interrupt out endpoint */
			dbg("found interrupt out on endpoint %d", i);
			interrupt_out_endpoint[num_interrupt_out] = endpoint;
			++num_interrupt_out;
		}
#endif		
	}


	/* found all that we need */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
	dev_info(&interface->dev, "%s converter detected\n", type->description);
#else
	dev_info(&interface->dev, "%s converter detected\n", type->name);
#endif

	if (!num_ports) {
		/* if this device type has a calc_num_ports function, call it */
		if (type->calc_num_ports) {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
			if (!try_module_get(type->driver.owner)) {
#else
			if (!try_module_get(type->owner)) {
#endif			    
				dev_err(&interface->dev, "module get failed, exiting\n");
				kfree (serial);
				return -EIO;
			}
			num_ports = type->calc_num_ports (serial);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
			module_put(type->driver.owner);
#else
			module_put(type->owner);
#endif
		}
		if (!num_ports)
			num_ports = type->num_ports;
	}

	if (get_free_serial (serial, num_ports, &minor) == NULL) {
		dev_err(&interface->dev, "No more free serial devices\n");
		kfree (serial);
		return -ENOMEM;
	}

	serial->minor = minor;
	serial->num_ports = num_ports;
	serial->num_bulk_in = num_bulk_in;
	serial->num_bulk_out = num_bulk_out;
	serial->num_interrupt_in = num_interrupt_in;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
	serial->num_interrupt_out = num_interrupt_out;
#endif
	/* create our ports, we need as many as the max endpoints */
	/* we don't use num_ports here cauz some devices have more endpoint pairs than ports */
	max_endpoints = max(num_bulk_in, num_bulk_out);
	max_endpoints = max(max_endpoints, num_interrupt_in);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
	max_endpoints = max(max_endpoints, num_interrupt_out);
#endif	
	max_endpoints = max(max_endpoints, (int)serial->num_ports);
	serial->num_port_pointers = max_endpoints;
	dbg("%s - setting up %d port structures for this device", __FUNCTION__, max_endpoints);
	for (i = 0; i < max_endpoints; ++i) {
	    
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17))
		port = kmalloc(sizeof(struct usb_serial_port), GFP_KERNEL);
#else
		port = kzalloc(sizeof(struct usb_serial_port), GFP_KERNEL);
#endif
		if (!port)
			goto probe_error;
			
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17))
		memset(port, 0x00, sizeof(struct usb_serial_port));			
#endif
		port->number = i + serial->minor;
		port->serial = serial;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,7))
		port->magic = USB_SERIAL_PORT_MAGIC;
		
#endif		

		
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,12))		
		spin_lock_init(&port->lock);
#endif		

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15))
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17))
		sema_init(&port->sem, 1);
#endif
#endif
		
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,16))
		mutex_init(&port->mutex);
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))
		INIT_WORK(&port->work, mx_usbserial_port_softint);
#else
		INIT_WORK(&port->work, mx_usbserial_port_softint, port);
#endif
		serial->port[i] = port;
	}

	/* set up the endpoint information */
	for (i = 0; i < num_bulk_in; ++i) {
		endpoint = bulk_in_endpoint[i];
		port = serial->port[i];
		port->read_urb = usb_alloc_urb (0, GFP_KERNEL);
		if (!port->read_urb) {
			dev_err(&interface->dev, "No free urbs available\n");
			goto probe_error;
		}
		buffer_size = le16_to_cpu(endpoint->wMaxPacketSize);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
		port->bulk_in_size = buffer_size;
#endif		
		port->bulk_in_endpointAddress = endpoint->bEndpointAddress;
		port->bulk_in_buffer = kmalloc (buffer_size, GFP_KERNEL);
		if (!port->bulk_in_buffer) {
			dev_err(&interface->dev, "Couldn't allocate bulk_in_buffer\n");
			goto probe_error;
		}
		usb_fill_bulk_urb (port->read_urb, dev,
				   usb_rcvbulkpipe (dev,
					   	    endpoint->bEndpointAddress),
				   port->bulk_in_buffer, buffer_size,
				   serial->type->read_bulk_callback,
				   port);
	}

	for (i = 0; i < num_bulk_out; ++i) {
		endpoint = bulk_out_endpoint[i];
		port = serial->port[i];
		port->write_urb = usb_alloc_urb(0, GFP_KERNEL);
		if (!port->write_urb) {
			dev_err(&interface->dev, "No free urbs available\n");
			goto probe_error;
		}
		buffer_size = le16_to_cpu(endpoint->wMaxPacketSize);
		port->bulk_out_size = buffer_size;
		port->bulk_out_endpointAddress = endpoint->bEndpointAddress;
		port->bulk_out_buffer = kmalloc (buffer_size, GFP_KERNEL);
		if (!port->bulk_out_buffer) {
			dev_err(&interface->dev, "Couldn't allocate bulk_out_buffer\n");
			goto probe_error;
		}
		usb_fill_bulk_urb (port->write_urb, dev,
				   usb_sndbulkpipe (dev,
						    endpoint->bEndpointAddress),
				   port->bulk_out_buffer, buffer_size, 
				   serial->type->write_bulk_callback,
				   port);
	}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
	if (serial->type->read_int_callback) {
#endif
		for (i = 0; i < num_interrupt_in; ++i) {
			endpoint = interrupt_in_endpoint[i];
			port = serial->port[i];
			port->interrupt_in_urb = usb_alloc_urb(0, GFP_KERNEL);
			if (!port->interrupt_in_urb) {
				dev_err(&interface->dev, "No free urbs available\n");
				goto probe_error;
			}
			buffer_size = le16_to_cpu(endpoint->wMaxPacketSize);
			port->interrupt_in_endpointAddress = endpoint->bEndpointAddress;
			port->interrupt_in_buffer = kmalloc (buffer_size, GFP_KERNEL);
			if (!port->interrupt_in_buffer) {
				dev_err(&interface->dev, "Couldn't allocate interrupt_in_buffer\n");
				goto probe_error;
			}
			usb_fill_int_urb (port->interrupt_in_urb, dev, 
					  usb_rcvintpipe (dev,
							  endpoint->bEndpointAddress),
					  port->interrupt_in_buffer, buffer_size, 
					  serial->type->read_int_callback, port, 
					  endpoint->bInterval);
		}
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
	} else if (num_interrupt_in) {
		dbg("the device claims to support interrupt in transfers, but read_int_callback is not defined");
	}
	
	if (serial->type->write_int_callback) {
		for (i = 0; i < num_interrupt_out; ++i) {
			endpoint = interrupt_out_endpoint[i];
			port = serial->port[i];
			port->interrupt_out_urb = usb_alloc_urb(0, GFP_KERNEL);
			if (!port->interrupt_out_urb) {
				dev_err(&interface->dev, "No free urbs available\n");
				goto probe_error;
			}
			buffer_size = le16_to_cpu(endpoint->wMaxPacketSize);
			port->interrupt_out_size = buffer_size;
			port->interrupt_out_endpointAddress = endpoint->bEndpointAddress;
			port->interrupt_out_buffer = kmalloc (buffer_size, GFP_KERNEL);
			if (!port->interrupt_out_buffer) {
				dev_err(&interface->dev, "Couldn't allocate interrupt_out_buffer\n");
				goto probe_error;
			}
			usb_fill_int_urb (port->interrupt_out_urb, dev,
					  usb_sndintpipe (dev,
							  endpoint->bEndpointAddress),
					  port->interrupt_out_buffer, buffer_size,
					  serial->type->write_int_callback, port,
					  endpoint->bInterval);
		}
	} else if (num_interrupt_out) {
		dbg("the device claims to support interrupt out transfers, but write_int_callback is not defined");
	}
#endif
	/* if this device type has an attach function, call it */
	if (type->attach) {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
		if (!try_module_get(type->driver.owner)) {
#else		    
		if (!try_module_get(type->owner)) {
#endif		    
			dev_err(&interface->dev, "module get failed, exiting\n");
			goto probe_error;
		}
		retval = type->attach (serial);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
		module_put(type->driver.owner);
#else
		module_put(type->owner);
#endif		
		if (retval < 0)
			goto probe_error;
		if (retval > 0) {
			/* quietly accept this device, but don't bind to a serial port
			 * as it's about to disappear */
			goto exit;
		}
	}

	/* register all of the individual ports with the driver core */
	for (i = 0; i < num_ports; ++i) {
		port = serial->port[i];
		port->dev.parent = &interface->dev;
		port->dev.driver = NULL;
		port->dev.bus = &mx_usb_serial_bus_type;
		port->dev.release = &port_release;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
		snprintf (&port->dev.bus_id[0], sizeof(port->dev.bus_id),
		"ttyMXUSB%d", port->number);

		dbg ("%s - registering %s", __FUNCTION__, port->dev.bus_id);

#else
		dev_set_name (&port->dev, "ttyMXUSB%d", port->number);
		dbg ("%s - registering %s", __FUNCTION__, dev_name(&port->dev));
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25))
		retval = device_register (&port->dev);
#else
		retval = device_register (&port->dev);

#endif
	}

	//mx_usbserial_console_init (debug, minor);

exit:
	/* success */
	usb_set_intfdata (interface, serial);
	return 0;

probe_error:
	for (i = 0; i < num_bulk_in; ++i) {
		port = serial->port[i];
		if (!port)
			continue;
		if (port->read_urb)
			usb_free_urb (port->read_urb);
		kfree(port->bulk_in_buffer);
	}
	for (i = 0; i < num_bulk_out; ++i) {
		port = serial->port[i];
		if (!port)
			continue;
		if (port->write_urb)
			usb_free_urb (port->write_urb);
		kfree(port->bulk_out_buffer);
	}
	for (i = 0; i < num_interrupt_in; ++i) {
		port = serial->port[i];
		if (!port)
			continue;
		if (port->interrupt_in_urb)
			usb_free_urb (port->interrupt_in_urb);
		kfree(port->interrupt_in_buffer);
	}
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
	for (i = 0; i < num_interrupt_out; ++i) {
		port = serial->port[i];
		if (!port)
			continue;
		if (port->interrupt_out_urb)
			usb_free_urb (port->interrupt_out_urb);
		kfree(port->interrupt_out_buffer);
	}
#endif

	/* return the minor range that this device had */
	return_serial (serial);

	/* free up any memory that we allocated */
	for (i = 0; i < serial->num_port_pointers; ++i)
		kfree(serial->port[i]);
	kfree (serial);
	return -EIO;
}

void mx_usbserial_disconnect(struct usb_interface *interface)
{
	struct usb_serial *serial = usb_get_intfdata (interface);
	struct device *dev = &interface->dev;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,10))
	int i;
	struct usb_serial_port *port;
#endif

	dbg ("%s", __FUNCTION__);

	usb_set_intfdata (interface, NULL);
	if (serial) {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,10))
		for (i = 0; i < serial->num_ports; ++i) {
			port = serial->port[i];
			if (port && port->tty)
				tty_hangup(port->tty);
		}
#endif		
		/* let the last holder of this object 
		 * cause it to be cleaned up */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,4))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,8)) 			
			kref_put(&serial->kref, destroy_serial);
#else
			kref_put(&serial->kref);			
#endif
#else
		    kobject_put (&serial->kobj);
#endif
	}
	dev_info(dev, "device disconnected\n");
}

static struct tty_operations serial_ops = {
	.open =			serial_open,
	.close =		serial_close,
	.write =		serial_write,
	.write_room =		serial_write_room,
	.ioctl =		serial_ioctl,
	.set_termios =		serial_set_termios,
	.throttle =		serial_throttle,
	.unthrottle =		serial_unthrottle,
	.break_ctl =		serial_break,
	.chars_in_buffer =	serial_chars_in_buffer,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
	.read_proc =		serial_read_proc,
#endif
	.tiocmget =		serial_tiocmget,
	.tiocmset =		serial_tiocmset,
};

struct tty_driver *mx_usbserial_tty_driver;

int usb_serial_init(void)
{
	int i;
	int result;

	mx_usbserial_tty_driver = alloc_tty_driver(SERIAL_TTY_MINORS);
	if (!mx_usbserial_tty_driver)
		return -ENOMEM;

	/* Initialize our global data */
	for (i = 0; i < SERIAL_TTY_MINORS; ++i) {
		serial_table[i] = NULL;
	}


	result = bus_register(&mx_usb_serial_bus_type);
	if (result) {
		err("%s - registering bus driver failed", __FUNCTION__);
		goto exit_bus;
	}

	mx_usbserial_tty_driver->owner = THIS_MODULE;
	mx_usbserial_tty_driver->driver_name = "mxusbserial";
#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
	mx_usbserial_tty_driver->devfs_name = "usb/tts/";
	mx_usbserial_tty_driver->flags = TTY_DRIVER_REAL_RAW | TTY_DRIVER_NO_DEVFS;
#else
	mx_usbserial_tty_driver->flags = TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV;
#endif
	mx_usbserial_tty_driver->name = 	"ttyMXUSB";
	mx_usbserial_tty_driver->major = SERIAL_TTY_MAJOR;
	mx_usbserial_tty_driver->minor_start = 0;
	mx_usbserial_tty_driver->type = TTY_DRIVER_TYPE_SERIAL;
	mx_usbserial_tty_driver->subtype = SERIAL_TYPE_NORMAL;
	mx_usbserial_tty_driver->init_termios = tty_std_termios;
	mx_usbserial_tty_driver->init_termios.c_cflag = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
	tty_set_operations(mx_usbserial_tty_driver, &serial_ops);
	result = tty_register_driver(mx_usbserial_tty_driver);
	if (result) {
		err("%s - tty_register_driver failed", __FUNCTION__);
		goto exit_reg_driver;
	}

	/* register the USB driver */
	result = usb_register(&usb_serial_driver);
	if (result < 0) {
		err("%s - usb_register failed", __FUNCTION__);
		goto exit_tty;
	}

	printk(KERN_INFO DRIVER_DESC);

	return result;

exit_tty:
	tty_unregister_driver(mx_usbserial_tty_driver);

exit_reg_driver:
	bus_unregister(&mx_usb_serial_bus_type);

exit_bus:
	err ("%s - returning with error %d", __FUNCTION__, result);
	put_tty_driver(mx_usbserial_tty_driver);
	return result;
}


void usb_serial_exit(void)
{
	//mx_usbserial_console_exit();
	
	usb_deregister(&usb_serial_driver);
	tty_unregister_driver(mx_usbserial_tty_driver);
	put_tty_driver(mx_usbserial_tty_driver);
	bus_unregister(&mx_usb_serial_bus_type);
}


module_init(usb_serial_init);
module_exit(usb_serial_exit);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
int mx_usbserial_register(struct usb_serial_driver *driver)
#else
int mx_usbserial_register(struct usb_serial_device_type *driver)
#endif
{
	int retval;


#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
	if (!driver->description)
		driver->description = driver->driver.name;
#endif

	/* Add this device to our list of devices */
	list_add(&driver->driver_list, &usb_serial_driver_list);

	retval = mx_usbserial_bus_register(driver);
	if (retval) {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
		err("problem %d when registering driver %s", retval, driver->description);
#else
		err("problem %d when registering driver %s", retval, driver->name);
#endif		
		list_del(&driver->driver_list);
	}
	else{

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
		printk(KERN_INFO "USB Serial support registered for %s", driver->description);
#else
		info("USB Serial support registered for %s", driver->name);
#endif    

    }
    
	return retval;
}
#else
int mx_usbserial_register(struct usb_serial_device_type *driver)
{
	int retval;

	/* Add this device to our list of devices */
	list_add(&driver->driver_list, &usb_serial_driver_list);

	retval =  mx_usbserial_bus_register (driver);

	if (retval)
		goto error;

	printk(KERN_INFO "USB Serial support registered for %s", driver->name);

	return retval;
error:
	err("problem %d when registering driver %s", retval, driver->name);
	list_del(&driver->driver_list);

	return retval;
}
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
void mx_usbserial_deregister(struct usb_serial_driver *device)
#else
void mx_usbserial_deregister(struct usb_serial_device_type *device)
#endif
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,8))
	struct usb_serial *serial;
	int i;

	printk(KERN_INFO "USB Serial deregistering driver %s", device->name);

	/* clear out the serial_table if the device is attached to a port */
	for(i = 0; i < SERIAL_TTY_MINORS; ++i) {
		serial = serial_table[i];
		if ((serial != NULL) && (serial->type == device)) {
			usb_driver_release_interface (&usb_serial_driver, serial->interface);
			mx_usbserial_disconnect (serial->interface);
		}
	}
#else        

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 	    
	printk(KERN_INFO "USB Serial deregistering driver %s", device->description);
#else
	info("USB Serial deregistering driver %s", device->name);
#endif

#endif
	list_del(&device->driver_list);
	mx_usbserial_bus_deregister(device);
}



/* If the usb-serial core is built into the core, the usb-serial drivers
   need these symbols to load properly as modules. */

EXPORT_SYMBOL_GPL(mx_usbserial_register);
EXPORT_SYMBOL_GPL(mx_usbserial_deregister);
EXPORT_SYMBOL_GPL(mx_usbserial_probe);
EXPORT_SYMBOL_GPL(mx_usbserial_disconnect);
EXPORT_SYMBOL_GPL(mx_usbserial_port_softint);


/* Module information */
MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

module_param(debug, bool, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Debug enabled or not");
