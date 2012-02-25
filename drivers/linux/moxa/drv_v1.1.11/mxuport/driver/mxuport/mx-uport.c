/*
 *      mx-uport.c - MOXA UPort series drvier
 *
 *      Copyright (c) 2006 Moxa Technologies Co., Ltd. 
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      Supports the following Moxa USB to serial converters:
 *       2 ports : UPort 1250, UPort 1250I
 *       4 ports : UPort 1410, UPort 1450, UPort 1450I
 *       8 ports : UPort 1610-8, UPort 1650-8
 *      16 ports : UPort 1610-16, UPort 1650-16
 *
 *      For questions or problems with this driver, contact Moxa Technologies
 *      technical support (support@moxa.com.tw).
 *
 *      Version history: 
 *      
 *      Revision    Date        Author                  Comment
 *      --------  ----------  ----------  -----------------------------------
 *        v0.3    08-09-2006    Danny       First version release.
 *
 */
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/tty_driver.h>
#include <linux/serial.h>
#include <linux/serial_reg.h>
#include <linux/usb.h>
#include <asm/uaccess.h>
#include "UPort1250FW.h" 
#include "UPort1250IFW.h"
#include "UPort1410FW.h" 
#include "UPort1450FW.h" 
#include "UPort1450IFW.h"
#include "UPort1610_8FW.h"
#include "UPort1650_8FW.h"
#include "UPort1610_16FW.h"
#include "UPort1650_16FW.h"
#include "mxusb-serial.h"
#include "mx-uport.h"

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19))
#ifndef FC6
#include <linux/config.h>
#endif
#endif

/*
 *  Global variables
 */ 
static int debug;

/*
 *  Table of devices that work with this driver 
 */
static struct usb_device_id mxuport_idtable[] = {
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1250_PID) },
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1251_PID) },
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1410_PID) },
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1450_PID) },
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1451_PID) },
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1618_PID) },
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1658_PID) },
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1613_PID) },
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1653_PID) },
	{ } /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, mxuport_idtable);

static struct usb_device_id mxuport_2p_idtable[] = {
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1250_PID) },
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1251_PID) },
	{ } /* Terminating entry */
};

static struct usb_device_id mxuport_4p_idtable[] = {
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1410_PID) },
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1450_PID) },
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1451_PID) },
	{ } /* Terminating entry */
};

static struct usb_device_id mxuport_8p_idtable[] = {
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1618_PID) },
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1658_PID) },
	{ } /* Terminating entry */
};

static struct usb_device_id mxuport_16p_idtable[] = {
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1613_PID) },
	{ USB_DEVICE(MX_USBSERIAL_VID, MX_UPORT1653_PID) },
	{ } /* Terminating entry */
};

static struct usb_driver mxuport_driver = {
#if (defined(_SCREEN_INFO_H) || (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15)))
    	.no_dynamic_id  = 1,
#else
	.owner		    = THIS_MODULE,
#endif
	.name		    = "mxuport",
	.probe		    = mx_usbserial_probe,
	.disconnect	    = mx_usbserial_disconnect,
	.id_table	    = mxuport_idtable,
};

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
static struct usb_serial_driver mxuport_2p_device = {
	.driver = {
        .owner        = THIS_MODULE,
        .name         = "MxUPort-2P",
	},
	.description      = "MOXA UPort 1200 series",
#else
static struct usb_serial_device_type mxuport_2p_device = {
	.owner            = THIS_MODULE,
	.name             = "MOXA UPort 1200 series",
	.short_name 	  = "MxUPort-2P",
#endif	
	.id_table         = mxuport_2p_idtable,
	.num_interrupt_in = 0,
	.num_bulk_in 	  = 2,
	.num_bulk_out 	  = 1,
	.num_ports 	  = 2,
	.open             = mxuport_open,
	.close            = mxuport_close,
	.write            = mxuport_write,
	.chars_in_buffer  = mxuport_chars_in_buffer,
	.write_room 	  = mxuport_write_room,
	.ioctl            = mxuport_ioctl, 
	.set_termios      = mxuport_set_termios,
	.break_ctl 	      = mxuport_break_ctl,
	.tiocmget 	      = mxuport_tiocmget,
	.tiocmset 	      = mxuport_tiocmset, 
	.attach 	      = mxuport_startup,
	.shutdown 	      = mxuport_shutdown,
	.throttle 	      = mxuport_throttle,
	.unthrottle 	  = mxuport_unthrottle, 
};

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
static struct usb_serial_driver mxuport_4p_device = {
	.driver = {
        .owner        = THIS_MODULE,
        .name         = "MxUPort-4P",
	},
	.description      = "MOXA UPort 1400 series",
#else
static struct usb_serial_device_type mxuport_4p_device = {
	.owner            = THIS_MODULE,
	.name             = "MOXA UPort 1400 series",
	.short_name 	  = "MxUPort-4P",
#endif	
	.id_table         = mxuport_4p_idtable,
	.num_interrupt_in = 0,
	.num_bulk_in 	  = 2,
	.num_bulk_out 	  = 1,
	.num_ports 	  = 4,
	.open             = mxuport_open,
	.close            = mxuport_close,
	.write            = mxuport_write,
	.chars_in_buffer  = mxuport_chars_in_buffer,
	.write_room 	  = mxuport_write_room,
	.ioctl            = mxuport_ioctl, 
	.set_termios      = mxuport_set_termios,
	.break_ctl 	  = mxuport_break_ctl,
	.tiocmget 	  = mxuport_tiocmget,
	.tiocmset 	  = mxuport_tiocmset, 
	.attach 	  = mxuport_startup,
	.shutdown 	  = mxuport_shutdown,
	.throttle 	  = mxuport_throttle,
	.unthrottle 	  = mxuport_unthrottle, 
};

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
static struct usb_serial_driver mxuport_8p_device = {
	.driver = {
        .owner        = THIS_MODULE,
        .name         = "MxUPort-8P",
	},
	.description      = "MOXA UPort 1600-8 series",
#else
static struct usb_serial_device_type mxuport_8p_device = {
	.owner            = THIS_MODULE,
	.name             = "MOXA UPort 1600-8 series",
	.short_name       = "MxUPort-8P",
#endif	
	.id_table         = mxuport_8p_idtable,
	.num_interrupt_in = 0,
	.num_bulk_in 	  = 2,
	.num_bulk_out 	  = 1,
	.num_ports        = 8,
    .open             = mxuport_open,
    .close            = mxuport_close,
    .write            = mxuport_write,
    .chars_in_buffer  = mxuport_chars_in_buffer,
    .write_room       = mxuport_write_room,
    .ioctl            = mxuport_ioctl, 
    .set_termios      = mxuport_set_termios,
    .break_ctl        = mxuport_break_ctl,
    .tiocmget         = mxuport_tiocmget,
    .tiocmset         = mxuport_tiocmset, 
    .attach           = mxuport_startup,
    .shutdown         = mxuport_shutdown,
    .throttle         = mxuport_throttle,
    .unthrottle       = mxuport_unthrottle, 
};

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)) 
static struct usb_serial_driver mxuport_16p_device = {
	.driver = {
        .owner        = THIS_MODULE,
        .name         = "MxUPort-16P",
	},
	.description      = "MOXA UPort 1600-16 series",
#else
static struct usb_serial_device_type mxuport_16p_device = {
	.owner            = THIS_MODULE,
	.name             = "MOXA UPort 1600-16 series",
	.short_name       = "MxUPort-16P",
#endif	
	.id_table         = mxuport_16p_idtable,
	.num_interrupt_in = 0,
	.num_bulk_in 	  = 2,
	.num_bulk_out 	  = 1,
	.num_ports        = 16,
    .open             = mxuport_open,
    .close            = mxuport_close,
    .write            = mxuport_write,
    .chars_in_buffer  = mxuport_chars_in_buffer,
    .write_room       = mxuport_write_room,
    .ioctl            = mxuport_ioctl, 
    .set_termios      = mxuport_set_termios,
    .break_ctl        = mxuport_break_ctl,
    .tiocmget         = mxuport_tiocmget,
    .tiocmset         = mxuport_tiocmset, 
    .attach           = mxuport_startup,
    .shutdown         = mxuport_shutdown,
    .throttle         = mxuport_throttle,
    .unthrottle       = mxuport_unthrottle, 
};


/*  
 *  mxuport_startup - attach function of driver
 *
 *	This will be called when the struct usb_serial structure is fully 
 *	set up. Do any local initialization of the device, or any private
 *	memory structure allocation at this point in time.
 *
 */
static int  mxuport_startup(struct usb_serial *serial)
{

	int i;
	struct mxuport_serial *mx_serial;
	struct mxuport_port *mx_port;
	struct usb_device *dev;
	struct usb_serial_port *port0, *port1;
	int status = 0;

	/*
	 *  Create our private serial structure 
	 */
	mx_serial = kmalloc(sizeof(struct mxuport_serial), GFP_ATOMIC);

	if (mx_serial == NULL) {
		dev_err(&serial->dev->dev, "%s - Out of memory\n", __FUNCTION__);
		return -ENOMEM;
	}

	memset(mx_serial, 0, sizeof(struct mxuport_serial));

	/*
	 *  Initial global spin lock
	 */

	sema_init(&mx_serial->open_close_sem,1);

	/*
	 *  Loop back to the owner of this object
	 */
	
	mx_serial->serial = serial;
    
	/* 
	 *  Get product info from device 
	 */
     
	dev = serial->dev;
	mx_serial->productid = dev->descriptor.idProduct;
	mx_get_string(dev, dev->descriptor.iProduct, mx_serial->dev_name);
	dev_info(&serial->dev->dev, "%s detected\n", mx_serial->dev_name);	
	mx_serial->open_cnt = 0;

	/* 
	 *  Initial USB settings 
	 */

	port0 = serial->port[0];
	port1 = serial->port[1];
	
	mx_serial->bulkout_size       = port0->bulk_out_size;	
	mx_serial->write_bulkout_addr = port0->bulk_out_endpointAddress;   /* for write data */
	mx_serial->read_bulkin_addr   = port0->bulk_in_endpointAddress;    /* for read data */
	mx_serial->event_bulkin_addr  = port1->bulk_in_endpointAddress;    /* for receive event */

	dbg("%s - Bulk in/out MAX pkt size : %d", __FUNCTION__, mx_serial->bulkout_size);
	dbg("%s - Read bulk in address     : %d", __FUNCTION__, mx_serial->read_bulkin_addr);
	dbg("%s - Write bulk out address   : %d", __FUNCTION__, mx_serial->write_bulkout_addr);
	dbg("%s - Event bulk in address    : %d", __FUNCTION__, mx_serial->event_bulkin_addr);

	

    /*
     *  Load our firmware
     */

	if((status = mx_send_ctrl_urb(mx_serial->serial->dev,
                                  RQ_VENDOR_QUERY_FW_CONFIG,
                                  0,
                                  0,
                                  NULL,
                                  0)) != 0)
	{



		mx_send_ctrl_urb(mx_serial->serial->dev,
                                 RQ_VENDOR_RESET_DEVICE,
                                 0,
                                 0,
                                 NULL,
                                 0);
		
   
		return 0;	
  
       

	}
    
	mx_load_firmware(mx_serial);

        
    
    /*
     *  Register bulk IN/OUT callback functions
     */
    usb_fill_bulk_urb (port0->read_urb, 
                       serial->dev,
	    			   usb_rcvbulkpipe (serial->dev, mx_serial->read_bulkin_addr),
				       port0->bulk_in_buffer, 
				       mx_serial->bulkout_size,
				       mxuport_read_bulk_callback,
				       port0); 

    usb_fill_bulk_urb (port1->read_urb, 
                       serial->dev,
	    			   usb_rcvbulkpipe (serial->dev, mx_serial->event_bulkin_addr),
				       port1->bulk_in_buffer, 
				       mx_serial->bulkout_size,
				       mxuport_event_bulk_callback,
				       port1); 
    	
	/*
	 *  Set the our private serial data
	 */
	usb_set_serial_data(serial, mx_serial);	

	
	
	/* 
	 *  Setup our port private structures 
	 */
	for (i = 0; i < serial->num_ports; ++i) {
		mx_port = kmalloc (sizeof(struct mxuport_port), GFP_KERNEL);
		if (mx_port == NULL) {
			dev_err(&serial->dev->dev, "%s - Out of memory\n", __FUNCTION__);
			
			goto EXIT_STARTUP;
		}	
		memset(mx_port, 0, sizeof(struct mxuport_port));

	    /*
	     *  Initial local port spin lock
	     */
		spin_lock_init(&mx_port->port_splock);
		spin_lock_init(&mx_port->read_splock);
		spin_lock_init(&mx_port->write_splock);
		
		
		/*
		 *  Initial serial settings
		 */
		  
		mx_port->portno = serial->port[i]->number - serial->minor;
		mx_port->flags = ASYNC_LOW_LATENCY;
		
    	    /*
	     *  Loop back to the owner of this object
	     */
		mx_port->port = serial->port[i];
		mx_port->open_cnt = 0;
		spin_lock_init(&mx_port->cnt_splock);

		
           	    
	    /*
	     *  Initial UART configuration
	     */
    	mx_port->uart_cfg = kmalloc (sizeof(struct mx_uart_config), GFP_ATOMIC);
		if(mx_port->uart_cfg == NULL){
			dev_err(&serial->dev->dev, "%s - Out of memory\n", __FUNCTION__);
			goto EXIT_STARTUP;		    
	    }	    
		memset(mx_port->uart_cfg, 0, sizeof(struct mx_uart_config));
        
        mx_port->uart_cfg->wBaudRate  = 9600;
        mx_port->uart_cfg->bDataBits  = MX_WORDLENGTH_8;
        mx_port->uart_cfg->bParity    = MX_PARITY_NONE;
        mx_port->uart_cfg->bStopBits  = MX_STOP_BITS_1;
        mx_port->uart_cfg->bFlowCtrl  = MX_NO_FLOWCTRL;
        mx_port->uart_cfg->cXon       = MX_START_CHAR;
        mx_port->uart_cfg->cXoff      = MX_STOP_CHAR;
        mx_port->uart_cfg->interface  = MX_INT_RS232;
	 
	

		
        /*
         *  Initial read/write queue
         */ 
         
        mx_port->write_q = mx_init_queue(i, MAX_QUEUE_SIZE);
	  		
		if(mx_port->write_q == NULL){
			dev_err(&serial->dev->dev, "%s - Out of memory\n", __FUNCTION__);
			goto EXIT_STARTUP;
		}
        
        mx_port->read_q = mx_init_queue(i, MAX_QUEUE_SIZE);
	 			
		if(mx_port->read_q == NULL){
			dev_err(&serial->dev->dev, "%s - Out of memory\n", __FUNCTION__);
			goto EXIT_STARTUP;
		}

	  
    	  
    	    /*
	     *  Set the port private data
	     */		
	      
		usb_set_serial_port_data(serial->port[i], mx_port);
		


		
	
	}
	
	return 0;
	
EXIT_STARTUP:
	for (; i>=0; i--) {
		mx_port = usb_get_serial_port_data(serial->port[i]);
		if(mx_port == NULL)
		    continue;
		    
        if(mx_port->uart_cfg != NULL)
            kfree(mx_port->uart_cfg);
        
        if(mx_port->write_q != NULL)
		    mx_free_queue(mx_port->write_q);
		    
        if(mx_port->read_q != NULL)
		    mx_free_queue(mx_port->read_q);

		kfree(mx_port);
		usb_set_serial_port_data(serial->port[i], NULL);        
	}
	kfree(mx_serial);
	usb_set_serial_data(serial, NULL);

	
	
	return -ENOMEM;    
}

/*
 *  mxuport_shutdown - shutdown function of driver
 *
 *	This function will be called when the device is removed from the usb bus.
 */
static void mxuport_shutdown(struct usb_serial *serial)
{
	int i;
	struct mxuport_port *mx_port;

	dbg("%s", __FUNCTION__);

	/* 
	 *  If disconnected, exit directly.
	 */
	 
	if (!usb_get_intfdata(serial->interface))
		return;

	/*
	 *  Stop reads and writes on all ports 
	 */

	
	for (i=0; i < serial->num_ports; ++i) {
        	mx_port = usb_get_serial_port_data(serial->port[i]);

		         
	        if(mx_port == NULL)
        	    continue;
		
	        if(mx_port->uart_cfg != NULL)
        	    kfree(mx_port->uart_cfg);
        
		if(mx_port->opened){
			mx_check_close_port(mx_port, i);
		}
		mx_free_queue(mx_port->write_q);
		kfree(usb_get_serial_port_data(serial->port[i]));
		usb_set_serial_port_data(serial->port[i],  NULL);
	}
	
	kfree(usb_get_serial_data(serial));
	usb_set_serial_data(serial, NULL);
}


/*
 *  mxuport_open - open function of driver					
 *
 *	This function is called by the tty driver when a port is opened
 *	If successful, we return 0.
 *	Otherwise, we return a negative error number.
 */

static int  mxuport_open(struct usb_serial_port *port, struct file *filp)
{
	int status = 0;
	struct usb_serial_port	*port0, *port1;
	struct mxuport_port *mx_port = usb_get_serial_port_data(port);
	struct mxuport_serial *mx_serial = usb_get_serial_data(port->serial);
	unsigned long cntflags;


	dbg("[Enter] %s - port %d", __FUNCTION__, mx_port->portno);
	
	if((mx_port == NULL) || (mx_serial == NULL))
	{	
		return -ENODEV;
	}
        if(down_interruptible(&mx_serial->open_close_sem))
                return -ERESTARTSYS;

	port0 = port->serial->port[0];
	port1 = port->serial->port[1];

	/*
	 *  Allocate write URB for this port
	 */

	if(mx_port->write_urb == NULL)
		mx_port->write_urb = usb_alloc_urb(0, GFP_ATOMIC);

	if(mx_port->write_urb == NULL)
	{
	        up(&mx_serial->open_close_sem);
		return -ENOMEM;	
	}
	
	/* 
	 *  Force low_latency on so that our tty_push actually forces the data through, 
	 *  otherwise it is scheduled, and with high data rates (like with OHCI) data
	 *  can get lost. 
	 */
	 
	if (port->tty)
		port->tty->low_latency = (mx_port->flags & ASYNC_LOW_LATENCY) ? 1 : 0;

	
	/* 
	 *  Initial port settings 
	 */

	status = mx_init_port(port);
	
	if(status)
	{
		up(&mx_serial->open_close_sem);
		return status;
	}
	/*
	 *  Initial port termios
	 */
     
	mxuport_set_termios(port, NULL);
	/* 
	 *  Flush driver and line discipline buffers 
	 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))  
	 
    if(port->tty->driver->flush_buffer)
             port->tty->driver->flush_buffer(port->tty);
#else

    if(port->tty->driver->ops->flush_buffer)
             port->tty->driver->ops->flush_buffer(port->tty);

#endif        
    
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,9))        		        
    	if(port->tty->ldisc.flush_buffer)
		port->tty->ldisc.flush_buffer(port->tty);
#else
	tty_ldisc_flush(port->tty);
#endif


	/*
	 *  reset the data toggle on the bulk endpoints to work around bug in
	 *  host controllers where things get out of sync some times
	 */ 
    //usb_clear_halt(port->serial->dev, port0->read_urb->pipe);
    //usb_clear_halt(port->serial->dev, port1->read_urb->pipe);
	
	/*
	 *  Start to read data and receive event from the device
	 */
	 
	if(port0->read_urb->status != -EINPROGRESS){	
		status = usb_submit_urb(port0->read_urb, GFP_ATOMIC);
		if(status){
			dbg("%s - USB submit read bulk urb failed. (status = %d)", __FUNCTION__, status);
		        up(&mx_serial->open_close_sem);
			return status;
		}
	}

	if(port1->read_urb->status != -EINPROGRESS){	
		status = usb_submit_urb(port1->read_urb, GFP_ATOMIC);
		if(status){
			dbg("%s - USB submit read bulk urb failed. (status = %d)", __FUNCTION__, status);
			up(&mx_serial->open_close_sem);

			return status;
		}
	}
	
	
	/*
	 *  Initial private parameters of port
	 */
	 
	spin_lock_irqsave(&mx_port->cnt_splock, cntflags);	
	mx_serial->open_cnt++;
	mx_port->open_cnt++;
  	spin_unlock_irqrestore(&mx_port->cnt_splock, cntflags);	
   	mx_port->opened = 1;
   	mx_port->send_len = 0;
	mx_port->hold_reason = 0;
	mx_port->comm_err = 0;
	mx_port->lsr_state = 0;
	mx_port->msr_state = 0;
	mx_port->set_B0 = 0;
	mx_port->mcr_state = UART_MCR_DTR | UART_MCR_RTS;

	
	mx_port->ch = 0;
	mx_port->ch1= 0;
	
       
	atomic_set(&mx_port->write_in_progress, 0);
        atomic_set(&mx_port->read_in_progress, 0);

	mx_clear_queue(mx_port->write_q);
	mx_clear_queue(mx_port->read_q);
	
  	init_waitqueue_head(&mx_port->delta_msr_wait);

	memset (&(mx_port->icount), 0x0, sizeof(mx_port->icount));

	dbg("[Exit] %s - port %d, open count : %d", __FUNCTION__, mx_port->portno, port->open_count);
     
	up(&mx_serial->open_close_sem);
	return status;
}

/*
 *  mxuport_close - close function of driver
 *
 *	This function is called by the tty driver when a port is closed.
 */
static void mxuport_close (struct usb_serial_port *port, struct file * filp)
{
	struct mxuport_port *mx_port = usb_get_serial_port_data(port);
	struct mxuport_serial *mx_serial = usb_get_serial_data(port->serial);
	struct usb_serial_port *port0, *port1;
	struct tty_struct *tty = port->tty;
	unsigned long cntflags;
	
	/* 
	 *  If disconnected, exit directly.
	 */

	if (!usb_get_intfdata(port->serial->interface))
		return;
	
	if ((mx_port == NULL) || (mx_serial == NULL))
		return;
        
	if(down_interruptible(&mx_serial->open_close_sem))
                return;
	dbg("%s - port %d", __FUNCTION__, mx_port->portno);

    /*
     *  Check the port has already closed
     */

    if(mx_check_close_port(mx_port, mx_port->portno))
    {
	up(&mx_serial->open_close_sem);
	return;
    }

    /*
     *  Reenable the S/W flow control to prevent cannot write
     *  data out that receive XOFF char before.
     */
    

    mx_send_ctrl_urb(mx_port->port->serial->dev,
                         RQ_VENDOR_SET_XONXOFF,
                         0,
                         mx_port->portno,
                         NULL,
                         0);
    if(mx_port->uart_cfg->bFlowCtrl & (MX_XON_FLOWCTRL | MX_XOFF_FLOWCTRL)){
    	mx_send_ctrl_urb(mx_port->port->serial->dev,
        	         RQ_VENDOR_SET_XONXOFF,
                         1,
                         mx_port->portno,
                         NULL,
                         0);
    }


    /*
     *  Shutdown our urb
     */

	spin_lock_irqsave(&mx_port->cnt_splock, cntflags);	
	mx_serial->open_cnt--;
	mx_port->open_cnt--;
	spin_unlock_irqrestore(&mx_port->cnt_splock, cntflags);

    if(mx_serial->open_cnt <= 0) {
	port0 = port->serial->port[0];
	port1 = port->serial->port[1];   
	usb_unlink_urb(port0->read_urb);
	usb_unlink_urb(port1->read_urb);
    }

    if(mx_port->open_cnt <= 0) {
	if(mx_port->write_urb)
	    usb_unlink_urb(mx_port->write_urb);

	if(mx_port->write_urb){
	    if(mx_port->write_urb->transfer_buffer != NULL){
		kfree(mx_port->write_urb->transfer_buffer);
	    }
	    usb_free_urb(mx_port->write_urb);
	    mx_port->write_urb = NULL;
	}

    }

	/* 
	 *  Tell line discipline to process only XON/XOFF 
	 */
	tty->closing = 1;

	/* 
	 *  Flush driver and line discipline buffers 
	 */
	 
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))  
	 
    if(port->tty->driver->flush_buffer)
             port->tty->driver->flush_buffer(port->tty);
#else

    if(port->tty->driver->ops->flush_buffer)
             port->tty->driver->ops->flush_buffer(port->tty);

#endif   
	 
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,9))        		        
    	if(port->tty->ldisc.flush_buffer)
		port->tty->ldisc.flush_buffer(port->tty);
#else
	tty_ldisc_flush(port->tty);
#endif		

	tty->closing = 0;
    
        mx_port->opened = 0;
    
	dbg("%s exited", __FUNCTION__);    

	up(&mx_serial->open_close_sem);
	return;
}

static int mxuport_tiocmset (struct usb_serial_port *port, struct file *file, unsigned int set, unsigned int clear)
{

	int  status = 0;	
	unsigned int mcr;
	struct mxuport_port *mx_port = usb_get_serial_port_data(port);

	dbg("%s - port %d", __FUNCTION__, mx_port->portno);

	mcr = mx_port->mcr_state;

	/*
	 *  set MCR status
	 */
	if (set & TIOCM_RTS){

  	    if ((status = mx_send_ctrl_urb (port->serial->dev,
                                   	   RQ_VENDOR_SET_RTS,
                       	               1,
                                       mx_port->portno,
                                       NULL,
                                       0)) != 0) {
		    dbg("%s - fail to set port %d RTS", __FUNCTION__, mx_port->portno);
	    return(status);
	    }
		mcr |= UART_MCR_RTS;
	}

	if (set & TIOCM_DTR){
  	    if ((status = mx_send_ctrl_urb (port->serial->dev,
                                   	   RQ_VENDOR_SET_DTR,
                       	               1,
                                       mx_port->portno,
                                       NULL,
                                       0)) != 0) {
		    dbg("%s - fail to set port %d DTR", __FUNCTION__, mx_port->portno);
    	    return(status);
	    }
		mcr |= UART_MCR_DTR;
    }
    
	/*
	 *  clear MCR status
	 */
	if (clear & TIOCM_RTS){
  	    if ((status = mx_send_ctrl_urb (port->serial->dev,
                                   	   RQ_VENDOR_SET_RTS,
                       	               0,
                                       mx_port->portno,
                                       NULL,
                                       0)) != 0) {
		    dbg("%s - fail to clear port %d RTS", __FUNCTION__, mx_port->portno);
    	    return(status);
	    }
		mcr &= ~UART_MCR_RTS;
    }
    
	if (clear & TIOCM_DTR){
  	    if ((status = mx_send_ctrl_urb (port->serial->dev,
                                   	   RQ_VENDOR_SET_DTR,
                       	               0,
                                       mx_port->portno,
                                       NULL,
                                       0)) != 0) {
		    dbg("%s - fail to clear port %d DTR", __FUNCTION__, mx_port->portno);
    	    return(status);
	    }
		mcr &= ~UART_MCR_DTR;
    }
    
	mx_port->mcr_state = mcr;

	return status;
}

static int mxuport_tiocmget(struct usb_serial_port *port, struct file *file)
{
	struct mxuport_port *mx_port;
	unsigned int result = 0;
	unsigned int msr;
	unsigned int mcr;
	unsigned long flags;

	mx_port = usb_get_serial_port_data(port);
	if(mx_port == NULL)
		return -ENODEV;

	spin_lock_irqsave(&mx_port->port_splock, flags);

	msr = mx_port->msr_state;

	spin_unlock_irqrestore(&mx_port->port_splock, flags);	

	mcr = mx_port->mcr_state;
	
	result = ((mcr & UART_MCR_DTR)	? TIOCM_DTR: 0)|   /* 0x002 */
		     ((mcr & UART_MCR_RTS)	? TIOCM_RTS: 0)|   /* 0x004 */
		     ((msr & UART_MSR_CTS)	? TIOCM_CTS: 0)|   /* 0x020 */
		     ((msr & UART_MSR_DCD)	? TIOCM_CAR: 0)|   /* 0x040 */
		     ((msr & UART_MSR_RI)	? TIOCM_RI:  0)|   /* 0x080 */
		     ((msr & UART_MSR_DSR)	? TIOCM_DSR: 0);   /* 0x100 */

	dbg("%s - MSR return 0x%x", __FUNCTION__, result);

	return result;
}

/*
 *  mxuport_break_ctl - break function of driver
 * 
 *	This function sends a break to the port.
 */
static void mxuport_break_ctl (struct usb_serial_port *port, int break_state)
{
	struct mxuport_port *mx_port;
	int status;

	mx_port = usb_get_serial_port_data(port);
	if(mx_port == NULL)
		return;

	if (break_state == -1) {
		dbg("%s - Sending break to port %d", __FUNCTION__, mx_port->portno);
		status = mx_send_ctrl_urb (port->serial->dev,
                              	   RQ_VENDOR_SET_BREAK,
                              	   1,
                                   mx_port->portno,
                                   NULL,
                                   0);
	} else {
		dbg("%s - Clearing break of port %d", __FUNCTION__, mx_port->portno);
		status = mx_send_ctrl_urb (port->serial->dev,
                              	   RQ_VENDOR_SET_BREAK,
                              	   0,
                                   mx_port->portno,
                                   NULL,
                                   0);
	}
	
	if (status) {
		dbg("%s - error sending break set/clear command.", __FUNCTION__);
	}

	return;
}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))        		        
static void mxuport_set_termios (struct usb_serial_port *port, struct ktermios *old_termios)
#else
static void mxuport_set_termios (struct usb_serial_port *port, struct termios *old_termios)
#endif
{
	struct mxuport_port *mx_port = usb_get_serial_port_data(port);
	struct tty_struct *tty = port->tty;
	unsigned int cflag, iflag;

	dbg("%s - port %d", __FUNCTION__, mx_port->portno);

	if (!port->tty || !port->tty->termios) {
		dbg ("%s - no tty or termios", __FUNCTION__);
		return;
	}

	/* 
	 *  check that they really want us to change something 
	 */

	cflag = tty->termios->c_cflag;
	iflag = tty->termios->c_iflag;
	
	
	if (old_termios) {
		if (cflag == old_termios->c_cflag &&
		    iflag == old_termios->c_iflag) {
			dbg ("%s - nothing to change", __FUNCTION__);
			return;
		}
	}

	if (old_termios) {
		dbg("%s - old clfag %08x old iflag %08x", __FUNCTION__,
		    old_termios->c_cflag, old_termios->c_iflag);
	}

	if (mx_port == NULL)
		return;

	/* 
	 *  change the port settings to the new ones specified 
	 */
	mx_change_port_settings(mx_port);	

	return;
}

/*
 *  mxuport_ioctl - I/O control function of driver
 *
 *	This function handles any ioctl calls to the driver.
 */
static int mxuport_ioctl (struct usb_serial_port *port, struct file *file, unsigned int cmd, unsigned long arg)
{
    int status, req_if;
    long baud;
    unsigned char buf[4];
    unsigned char stop_char, start_char;
    unsigned long flags;	
	struct async_icount cnow;
	struct async_icount cprev;
	struct mxuport_port *mx_port;
    struct mxuport_serial *mx_serial;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))        		        
    struct ktermios old_termios;
#else
    struct termios old_termios;
#endif
    
    printk(KERN_DEBUG "[File mx-uport.c , line %d] entering mxuport_ioctl(): cmd: %u , arg: %lu\n", __LINE__ , cmd, arg);
    
    mx_port = usb_get_serial_port_data(port);
    mx_serial = usb_get_serial_data(port->serial);
    if((mx_port == NULL) || (mx_serial == NULL))
        return -ENODEV;

	dbg("%s - port %d, cmd = 0x%x", __FUNCTION__, mx_port->portno, cmd);

	switch(cmd){

        case TIOCMSET:
        case TIOCMBIS:
	    case TIOCMBIC: 
            return mx_set_modem_info(port, file, cmd, arg);
 
		case TCSBRK:	/* SVID version: non-zero arg --> no break */

			status = tty_check_change(port->tty);
			if (status)
				return status;
				
            /*
             *  wait tty transmit buffer empties
             */
			tty_wait_until_sent(port->tty, 0);

            /*
             *  wait firmware to transmit data completly
             */
            mx_wait_fw_until_sent(port, 0);
            
			if (!arg)
				mx_send_break(port, HZ/4);    /* 1/4 second */
			return 0;

		case TCSBRKP:	/* support for POSIX tcsendbreak() */

			status = tty_check_change(port->tty);
			if (status)
				return status;

            /*
             *  wait tty transmit buffer empties
             */
			tty_wait_until_sent(port->tty, 0);

            /*
             *  wait firmware to transmit data completly
             */
            mx_wait_fw_until_sent(port, 0);

			mx_send_break(port, arg ? arg*(HZ/10) : HZ/4);
			return 0;
		    
		case TIOCMIWAIT:
			dbg("%s - port %d : TIOCMIWAIT", __FUNCTION__, mx_port->portno);
			
			cprev = mx_port->icount;
			atomic_set(&mx_port->wait_msr, 1); 
						
       	    wait_event_interruptible_timeout(mx_port->delta_msr_wait, 
                                    	     (atomic_read(&mx_port->wait_msr) == 0), 
                                    	     WAIT_MSR_TIMEOUT);
											
			/*
			 *  if no change, return error 
			 */
			cnow = mx_port->icount;			 
			if (cnow.rng == cprev.rng && cnow.dsr == cprev.dsr &&
			    cnow.dcd == cprev.dcd && cnow.cts == cprev.cts)
				return -EIO; 
				
			/*
			 *  status change, return 0 
			 */					
			if (((arg & TIOCM_RNG) && (cnow.rng != cprev.rng)) ||
			    ((arg & TIOCM_DSR) && (cnow.dsr != cprev.dsr)) ||
			    ((arg & TIOCM_CD)  && (cnow.dcd != cprev.dcd)) ||
			    ((arg & TIOCM_CTS) && (cnow.cts != cprev.cts)) ) {
				return 0;
			}
			break;

		case TIOCGSERIAL:
			dbg("%s - port %d : TIOCGSERIAL", __FUNCTION__, mx_port->portno);
			return mx_get_serial_info(mx_port, (struct serial_struct __user *)arg);
			break;

		case TIOCSSERIAL:
			dbg("%s - port %d : TIOCSSERIAL", __FUNCTION__, mx_port->portno);
			return mx_set_serial_info(mx_port, (struct serial_struct __user *)arg);
			break;

		case TIOCGICOUNT:
			dbg ("%s - port %d : TIOCGICOUNT RX = %d, TX = %d", __FUNCTION__,
			     mx_port->portno, mx_port->icount.rx, mx_port->icount.tx);
			if (copy_to_user((void __user *)arg, &mx_port->icount, sizeof(mx_port->icount)))
				return -EFAULT;
			return 0;
			
		case TIOCGSOFTCAR:
	        if(!access_ok(VERIFY_WRITE, (void *)arg, sizeof(unsigned int)))
		        return -EFAULT;
			return put_user(C_CLOCAL(port->tty) ? 1 : 0, (unsigned int *)arg);

		case TIOCSSOFTCAR:			
	        if(!access_ok(VERIFY_READ, (void *)arg, sizeof(unsigned int)))
		        return -EFAULT;

			if(get_user(arg, (unsigned int *)arg))
			    return -EFAULT;
			
			port->tty->termios->c_cflag = ((port->tty->termios->c_cflag & ~CLOCAL) | 
                                          (arg ? CLOCAL : 0));
			return 0;

    	case TCSETSW:

            mx_wait_fw_until_sent(port, 0);

		    memcpy(&old_termios, port->tty->termios, sizeof(struct termios));  		
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25))

                       if(user_termios_to_kernel_termios(port->tty->termios, (struct termios __user *)arg))
			    return -EFAULT;
#else

                       if(user_termios_to_kernel_termios(port->tty->termios, (struct termios2 __user *)arg))
			    return -EFAULT;
#endif
		    
		    mxuport_set_termios(port, &old_termios);

		    return -ENOIOCTLCMD;

        case TCSETSF:
            
            mx_wait_fw_until_sent(port, 0);
        
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,9))        		        
			if(port->tty->ldisc.flush_buffer)
				port->tty->ldisc.flush_buffer(port->tty);
#else
			tty_ldisc_flush(port->tty);
#endif
	    
			if ((status = mx_send_ctrl_urb(port->serial->dev,
							               RQ_VENDOR_PURGE,
							               0x8,
									       mx_port->portno,
									       NULL,
									       0)) != 0) {
		        dbg("%s - fail to flush port %d input buffer", __FUNCTION__, mx_port->portno);
		        return status;
	        }		        

		    memcpy(&old_termios, port->tty->termios, sizeof(struct termios));  		
		    
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25))

                    if(user_termios_to_kernel_termios(port->tty->termios, (struct termios __user *)arg))
			    return -EFAULT;

#else

                    if(user_termios_to_kernel_termios(port->tty->termios, (struct termios2 __user *)arg))
			    return -EFAULT;
#endif

		    mxuport_set_termios(port, &old_termios);

		    return -ENOIOCTLCMD;

   

        case TCXONC:
            switch(arg){
                case TCOOFF:
			spin_lock_irqsave(&mx_port->write_splock, flags);			
                    mx_port->hold_reason |= MX_WAIT_FOR_START_TX;
			spin_unlock_irqrestore(&mx_port->write_splock, flags);		
                    break;

                case TCOON:
			spin_lock_irqsave(&mx_port->write_splock, flags);				
                    mx_port->hold_reason &= ~MX_WAIT_FOR_START_TX;
                    mx_prepare_write(port);
			spin_unlock_irqrestore(&mx_port->write_splock, flags);			
                    break;

                case TCIOFF:
                    if(STOP_CHAR(port->tty) != __DISABLED_CHAR){
                        stop_char = STOP_CHAR(port->tty);
						
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))        		        
				mxuport_write (port, &stop_char, 1);        
#else
				mxuport_write (port, 0, &stop_char, 1);        
#endif
                            
                    }
                    break;

                case TCION:
                    if(START_CHAR(port->tty) != __DISABLED_CHAR){
                        start_char = START_CHAR(port->tty);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))        		        
        		        mxuport_write (port, &start_char, 1);
#else
        		        mxuport_write (port, 0, &start_char, 1);
#endif
                    }
                    break;

                default:
                    return -EINVAL;
            }
            return 0;

		case TCFLSH:
			switch(arg){
				case TCIFLUSH:
                    mx_clear_queue(mx_port->read_q);
                    
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,9))        		        
    			if(port->tty->ldisc.flush_buffer)
				port->tty->ldisc.flush_buffer(port->tty);
#else
		tty_ldisc_flush(port->tty);
#endif    
					if ((status = mx_send_ctrl_urb(port->serial->dev,
									              RQ_VENDOR_PURGE,
									              0x8,
									              mx_port->portno,
									              NULL,
									              0)) != 0) {
				        dbg("%s - fail to flush port %d input buffer", __FUNCTION__, mx_port->portno);
				        return status;
			        }		        
		            break;

				case TCOFLUSH:
                    mx_clear_queue(mx_port->write_q);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))  	
 
                    if(port->tty->driver->flush_buffer)
                            port->tty->driver->flush_buffer(port->tty);
#else

                    if(port->tty->driver->ops->flush_buffer)
                            port->tty->driver->ops->flush_buffer(port->tty);

#endif   

					if ((status = mx_send_ctrl_urb(port->serial->dev,
								              	  RQ_VENDOR_PURGE,
									              0x4,
									              mx_port->portno,
									              NULL,
									              0)) != 0) {
				        dbg("%s - fail to flush port %d output buffer", __FUNCTION__, mx_port->portno);
				        return status;
			        }		        
		            break;

				case TCIOFLUSH:
                    mx_clear_queue(mx_port->read_q);
                    mx_clear_queue(mx_port->write_q);
                    
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,9))        		        
		    	if(port->tty->ldisc.flush_buffer)
				port->tty->ldisc.flush_buffer(port->tty);
#else
		tty_ldisc_flush(port->tty);
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))  
	 
   			if(port->tty->driver->flush_buffer)
        			port->tty->driver->flush_buffer(port->tty);
#else

                        if(port->tty->driver->ops->flush_buffer)
                                port->tty->driver->ops->flush_buffer(port->tty);

#endif   
				
					if ((status = mx_send_ctrl_urb(port->serial->dev,
									              RQ_VENDOR_PURGE,
									              0xC,
									              mx_port->portno,
									              NULL,
									              0)) != 0) {
				        dbg("%s - fail to flush port %d input/output buffer", __FUNCTION__, mx_port->portno);
				        return status;
			        }		        
		            break;
			}
			break;
		
        case MOXA_SET_INTERFACE:
            if((mx_serial->productid != MX_UPORT1450_PID) ||
               (mx_serial->productid != MX_UPORT1451_PID) ||                        
               (mx_serial->productid != MX_UPORT1658_PID) ||
               (mx_serial->productid != MX_UPORT1653_PID) ||
               (mx_serial->productid != MX_UPORT1250_PID) ||
               (mx_serial->productid != MX_UPORT1251_PID))
                return -EINVAL;

	        if(!access_ok(VERIFY_READ, (void *)arg, sizeof(int)))
		        return -EFAULT;
                    
            if(get_user(req_if, (int *)arg))
		        return -EFAULT;
            
            if((req_if < 0) || (req_if > 3))
                return -EINVAL;

            if(mx_port->uart_cfg->interface != req_if){
                mx_port->uart_cfg->interface = req_if;
                if((status = mx_send_ctrl_urb(port->serial->dev,
                                              RQ_VENDOR_SET_INTERFACE,
                                              mx_port->uart_cfg->interface,
                                              mx_port->portno,
                                              NULL,
                                              0)) != 0)
            		return status;                  
            }
            break;
    
        case MOXA_SET_SPECIAL_BAUD:
	        if(!access_ok(VERIFY_READ, (void *)arg, sizeof(long)))
		        return -EFAULT;

            if(get_user(baud, (long *)arg))
		        return -EFAULT;
            
            if((baud < 0) || (baud > 921600))
                return -EINVAL;
            
            mx_port->uart_cfg->wBaudRate = baud;
            
            buf[0] = (unsigned char)(mx_port->uart_cfg->wBaudRate & 0xFF);
            buf[1] = (unsigned char)((mx_port->uart_cfg->wBaudRate >> 8) & 0xFF);
            buf[2] = (unsigned char)((mx_port->uart_cfg->wBaudRate >> 16) & 0xFF);
            buf[3] = (unsigned char)((mx_port->uart_cfg->wBaudRate >> 24) & 0xFF);
    
            status = mx_send_ctrl_urb(port->serial->dev,
                                      RQ_VENDOR_SET_BAUD,
                                      0,
                                      mx_port->portno,
                                      buf,
                                      sizeof(buf));

	        if(status != sizeof(buf))
		          return status;
            break;
    
		default:
			dbg("%s - not support. (code : 0x%x)", __FUNCTION__, cmd);
			printk(KERN_DEBUG "[File mx-uport.c , line %d] unknow cmd: %u\n" , __LINE__ , cmd);
			return -ENOIOCTLCMD;
	}
    
    return 0;
}    
    
/*
 *  mxuport_chars_in_buffer - chars in buffer function of driver
 *
 *	This function is called by the tty driver when it wants to know how many
 *	bytes of data we currently have outstanding in the port (data that has
 *	been written, but hasn't made it out the port yet)
 *	If successful, we return the number of bytes left to be written in the 
 *	system, 
 *	Otherwise we return a negative error number.
 */
static int mxuport_chars_in_buffer (struct usb_serial_port *port)
{
	int chars = 0;
	struct mxuport_port *mx_port = usb_get_serial_port_data(port);

    chars = mx_data_in_queue(mx_port->write_q);

	dbg("%s - returns %d", __FUNCTION__, chars);

	return chars;
}

/*
 *  mxuport_write_room - write room function of driver
 *
 *	This function is called by the tty driver when it wants to know how many
 *	bytes of data we can accept for a specific port.
 *	If successful, we return the amount of room that we have for this port
 *	(the txCredits), 
 *	Otherwise we return a negative error number.
 */
static int mxuport_write_room (struct usb_serial_port *port)
{
	int room = 0;
	struct mxuport_port *mx_port = usb_get_serial_port_data(port);

    room = mx_space_in_queue(mx_port->write_q);

	dbg("%s - returns %d", __FUNCTION__, room);

	return room;
}

/*
 *  mxuport_throttle - throttle function of driver
 *
 *	This function is called by the tty driver when it wants to stop the data
 *	being read from the port.
 */
static void mxuport_throttle (struct usb_serial_port *port)
{
	struct mxuport_port *mx_port = usb_get_serial_port_data(port);
	unsigned long flags;	

	dbg("%s - port %d", __FUNCTION__, mx_port->portno);

	if (mx_port == NULL)
		return;
	if(!mx_port->opened)
		return ;
    spin_lock_irqsave(&mx_port->read_splock, flags);	
    mx_port->hold_reason |= MX_WAIT_FOR_UNTHROTTLE;

    mx_send_async_ctrl_urb(port->serial->dev,
                           RQ_VENDOR_SET_RX_HOST_EN,
                           0,
                           mx_port->portno,
                           NULL,
                           0);   
    spin_unlock_irqrestore(&mx_port->read_splock, flags); 		
}

/*
 *  mxuport_unthrottle - unthrottle function of driver
 *
 *	This function is called by the tty driver when it wants to resume the data
 *	being read from the port (called after SerialThrottle is called)
 */
static void mxuport_unthrottle (struct usb_serial_port *port)
{
    int    data_len;	
    unsigned long flags;	
    struct mxuport_port *mx_port = usb_get_serial_port_data(port);

	dbg("%s - port %d", __FUNCTION__, mx_port->portno);

	if (mx_port == NULL)
		return;
	if(!mx_port->opened)
		return ;
	
    spin_lock_irqsave(&mx_port->read_splock, flags);
    mx_port->hold_reason &= ~MX_WAIT_FOR_UNTHROTTLE; 
    
    mx_send_async_ctrl_urb(port->serial->dev,
                           RQ_VENDOR_SET_RX_HOST_EN,
                           1,
                           mx_port->portno,
                           NULL,
                           0);   

    data_len = mx_data_in_queue(mx_port->read_q);
    spin_unlock_irqrestore(&mx_port->read_splock, flags);
	
    if((data_len > 0) && (!atomic_read(&mx_port->read_in_progress))){
        mx_tty_recv(mx_port->port->tty, mx_port);
    }

		
}

/*
 *  mxuport_write -  write function of driver
 *
 *	This function is called by the tty driver when data should be written to the port.
 *	If successful, we return the number of bytes written, 
 *  otherwise we return a negative error number.
 */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
static int mxuport_write(struct usb_serial_port *port, const unsigned char *data, int count)
{
#else
static int mxuport_write(struct usb_serial_port *port, int from_user, const unsigned char *data, int count)
{
    unsigned char *usr_buf;
    
#endif
    unsigned long flags;
    int len_sent = 0;
    struct mxuport_port *mx_port = usb_get_serial_port_data(port);
    struct mxuport_serial *mx_serial = usb_get_serial_data(port->serial);
	
	dbg("[Enter] %s - port %d, count = %d", __FUNCTION__, mx_port->portno, count);

	if (count == 0) {
		dbg("%s - write request of 0 bytes", __FUNCTION__);
		return 0;
	}

	if ((mx_serial == NULL) || (mx_port == NULL))
		return -ENODEV;

	
if(atomic_read(&mx_port->write_q->add_sn)){
	
        return 0;
}

	if(count > mx_serial->bulkout_size)
		count = mx_serial->bulkout_size;


#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
spin_lock_irqsave(&mx_port->write_splock, flags);
	len_sent = mx_insert_queue(mx_port->write_q, data, count);
spin_unlock_irqrestore(&mx_port->write_splock, flags);   	
#else
	if(from_user){
	    usr_buf = kmalloc(sizeof(unsigned char) * count, GFP_KERNEL);
	    if(usr_buf == NULL){
			dev_err(&port->serial->dev->dev, "%s - Out of memory\n", __FUNCTION__);
	        return -ENOMEM;
	    }    	    
	    
		if(copy_from_user(usr_buf, data, count))
			return -EFAULT;

        spin_lock_irqsave(&mx_port->write_splock, flags);	
	    len_sent = mx_insert_queue(mx_port->write_q, usr_buf, count);
        spin_unlock_irqrestore(&mx_port->write_splock, flags);   

        kfree(usr_buf);			
	}else{
        spin_lock_irqsave(&mx_port->write_splock, flags);	
	    len_sent = mx_insert_queue(mx_port->write_q, data, count);
        spin_unlock_irqrestore(&mx_port->write_splock, flags);   
	}
#endif
spin_lock_irqsave(&mx_port->write_splock, flags);	
    mx_prepare_write(port);
spin_unlock_irqrestore(&mx_port->write_splock, flags);
	dbg("[Exit] %s - port %d, insert len = %d", __FUNCTION__, mx_port->portno, count);
    
	return len_sent;	
} 

/*
 *  mxuport_write_bulk_callback
 *
 *  This is the callback function for when we have finished sending serial data
 *  on the bulk out endpoint.
 */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))        		        
static void mxuport_write_bulk_callback (struct urb *urb)
#else
static void mxuport_write_bulk_callback (struct urb *urb, struct pt_regs *regs)
#endif
{
    unsigned long flags;
    struct mxuport_port *mx_port;
	struct usb_serial_port *port = (struct usb_serial_port *)urb->context;
    
    mx_port = usb_get_serial_port_data(port);
    if(mx_port == NULL)
        return;

    kfree(mx_port->write_urb->transfer_buffer);
    mx_port->write_urb->transfer_buffer = NULL;

    atomic_set(&mx_port->write_in_progress, 0);

	switch (urb->status) {
	    case 0: 
    	    /* 
	         *  Write successfully 
	         */
		    break;
		
	    case -ECONNRESET:
	    case -ENOENT:
	    case -ESHUTDOWN:
    		/* 
		     *  This urb is terminated, clean up 
		     */
		    dbg("%s - urb shutting down with status: %d", __FUNCTION__, urb->status);
		    return;
		
	    default:
    		dev_err (&urb->dev->dev, "%s - nonzero write bulk status received: %d\n",
		            __FUNCTION__, urb->status);
	}
        
    /*
     *  trigger next write
     */
     
   
    if(!(mx_port->hold_reason & MX_WAIT_FOR_SEND_NEXT)){
	spin_lock_irqsave(&mx_port->write_splock, flags);		
        mx_prepare_write(port);
	spin_unlock_irqrestore(&mx_port->write_splock, flags);	
    }	
	
}

/*
 *  mxuport_read_bulk_callback
 *
 *  This is the callback function for when we have received data on the
 *  bulk in endpoint. 
 */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))        		        
static void mxuport_read_bulk_callback (struct urb *urb)
#else
static void mxuport_read_bulk_callback (struct urb *urb, struct pt_regs *regs)
#endif
{
	int					status, data_len;
	__u16               raw_data_length;
	unsigned long       bufIndex, flags;
	unsigned long       rcv_port, rcv_buflen, rcv_buflen_bak;
	unsigned char       *data;
	struct tty_struct   *tty;
	struct mxuport_port    *mx_port;
	struct mxuport_serial  *mx_serial;
	struct usb_serial_port *port0;
	struct usb_serial_port *port = (struct usb_serial_port *)urb->context;	

	switch (urb->status) {
	    case 0:
    		/* 
		     *  Read successfully 
		     */
		    break;
    		    
	    case -ECONNRESET:
	    case -ENOENT:
	    case -ESHUTDOWN:
    		/* 
		     *  This urb is terminated, clean up 
		     */
		    dbg("%s - urb shutting down with status: %d", __FUNCTION__, urb->status);
		    return;
		    
	    default:
    		dev_err (&urb->dev->dev, "%s - nonzero read bulk status received: %d\n",
		            __FUNCTION__, urb->status );
	}


	if (urb->status) {
		dev_err(&urb->dev->dev, "%s - stopping read!\n", __FUNCTION__);
		return;
	}

	if (urb->actual_length == 0) {
		dbg("%s - read bulk callback with no data", __FUNCTION__);
		return;
	}

	mx_serial = usb_get_serial_data(port->serial);
	if(mx_serial == NULL)
		return;

	bufIndex = 0;
    data = urb->transfer_buffer;
	raw_data_length = urb->actual_length;

	//usb_serial_debug_data(debug, &port->serial->dev->dev, __FUNCTION__, raw_data_length, data);

    while(1)
    {
    	rcv_port = ((unsigned long)data[bufIndex + 1] & 0xFF) + (((unsigned long)data[bufIndex] & 0xFF) * 256);
    	rcv_buflen = ((unsigned long)data[bufIndex + 3] & 0xFF) + (((unsigned long)data[bufIndex + 2] & 0xFF) * 256);
    	rcv_buflen_bak = rcv_buflen;

        if((raw_data_length - (bufIndex + 4)) < rcv_buflen)
	    {
	       dbg("port %ld buf len over URB len\n", rcv_port);
           }

        if ((rcv_port >= port->serial->num_ports) || (rcv_buflen > mx_serial->bulkout_size)){
            dev_err(&urb->dev->dev, "%s - exception error : port %ld, receive len %ld\n", __FUNCTION__, rcv_port, rcv_buflen);
            return;
        }

        mx_port = usb_get_serial_port_data(port->serial->port[rcv_port]);

        dbg("%s - receive port : %ld, open state : %d", __FUNCTION__, rcv_port, mx_port->opened);

        if(mx_port->opened){
            
            spin_lock_irqsave(&mx_port->read_splock, flags);
            data_len = mx_insert_queue(mx_port->read_q, &data[bufIndex + HEADER_SIZE], rcv_buflen);
            if(data_len < rcv_buflen){
                dbg("port %ld receive buffer overflow\n", rcv_port);   
	        }
           spin_unlock_irqrestore(&mx_port->read_splock, flags);		
            if((mx_port->hold_reason & MX_WAIT_FOR_LOW_WATER) == 0){
		  spin_lock_irqsave(&mx_port->read_splock, flags);		
                data_len = mx_data_in_queue(mx_port->read_q);
		   spin_unlock_irqrestore(&mx_port->read_splock, flags);		
                if(data_len >= HIGH_WATER_SIZE){
                      spin_lock_irqsave(&mx_port->read_splock, flags);		
  	                mx_port->hold_reason |= MX_WAIT_FOR_LOW_WATER;
                      spin_unlock_irqrestore(&mx_port->read_splock, flags);
					  
                    mx_send_async_ctrl_urb(port->serial->dev,
                                           RQ_VENDOR_SET_RX_HOST_EN,
                                           0,
                                           mx_port->portno,
                                           NULL,
                                           0);    
                }                                           
            }
                
           	
    	    
    	    if(!(mx_port->hold_reason & MX_WAIT_FOR_UNTHROTTLE) &&
	      (!atomic_read(&mx_port->read_in_progress))){
	       spin_lock_irqsave(&mx_port->read_splock, flags);		
                tty = mx_port->port->tty;
		  spin_unlock_irqrestore(&mx_port->read_splock, flags);		
                if(tty){
                    mx_tty_recv(tty, mx_port);
                }                    
            }
		 
        }

		bufIndex += (rcv_buflen_bak + HEADER_SIZE);

		if((bufIndex >= raw_data_length) || ((raw_data_length - bufIndex) < HEADER_SIZE))
        	break;        
    }

    port0 = port->serial->port[0];
	
	status = usb_submit_urb(port0->read_urb, GFP_ATOMIC);
	if(status){
		dbg("%s - USB submit read urb failed (status = %d)", __FUNCTION__, status);
		return;
	}
}


/*
 *  mxuport_event_bulk_callback
 *
 *  This is the callback function for when we have received event on the
 *  bulk in endpoint. 
 */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))        		        
static void mxuport_event_bulk_callback (struct urb *urb)
#else
static void mxuport_event_bulk_callback (struct urb *urb, struct pt_regs *regs)
#endif
{
	int		    status;
	__u16               raw_data_length;
	unsigned long       bufIndex;
	unsigned long       rcv_port, rcv_event;
	unsigned char       *data;
	struct mxuport_port    *mx_port;
	struct mxuport_serial  *mx_serial;
	struct usb_serial_port *port1;
	struct usb_serial_port *port = (struct usb_serial_port *)urb->context;
	
	switch (urb->status) {
	    case 0:
    		/* 
		     *  Read successfully 
		     */
		    break;
    		    
	    case -ECONNRESET:
	    case -ENOENT:
	    case -ESHUTDOWN:
    		/* 
		     *  This urb is terminated, clean up 
		     */
		    dbg("%s - urb shutting down with status: %d", __FUNCTION__, urb->status);
		    return;
		    
	    default:
    		dev_err (&urb->dev->dev, "%s - nonzero event bulk status received: %d\n",
		            __FUNCTION__, urb->status );
	}


	if (urb->status) {
		dev_err(&urb->dev->dev, "%s - stopping read!\n", __FUNCTION__);
		return;
	}

	if (urb->actual_length == 0) {
		dbg("%s - event bulk callback with no data", __FUNCTION__);
		return;
	}
	
	mx_serial = usb_get_serial_data(port->serial);
	
	if(mx_serial == NULL)
	    return;
	
	bufIndex = 0;
	data = urb->transfer_buffer;
	raw_data_length = urb->actual_length;

	//usb_serial_debug_data(debug, &port->serial->dev->dev, __FUNCTION__, raw_data_length, data);
    
    while(1){
        
    	rcv_port = ((unsigned long)data[bufIndex + 1] & 0xFF) + (((unsigned long)data[bufIndex + 0] & 0xFF) * 256);
    	rcv_event = ((unsigned long)data[bufIndex + 3] & 0xFF) + (((unsigned long)data[bufIndex + 2] & 0xFF) * 256);
	
        if (rcv_port >= port->serial->num_ports){
            dev_err(&urb->dev->dev, "%s - exception error : port %ld\n", __FUNCTION__, rcv_port);
            return;
        }

        mx_port = usb_get_serial_port_data(port->serial->port[rcv_port]);
        mx_update_recv_event(mx_port, rcv_event, data, bufIndex);

    	bufIndex += MAX_EVENT_LENGTH;
	    if((bufIndex >= raw_data_length) || 
	       ((raw_data_length - bufIndex) < MAX_EVENT_LENGTH))
			break;
    }
    	
	port1 = port->serial->port[1];
	status = usb_submit_urb(port1->read_urb, GFP_ATOMIC);
	if(status){
		dbg("%s - USB submit event urb failed (status = %d)", __FUNCTION__, status);
		return;
	}    
}	

/*
 *  mxuport_control_callback
 *
 *  This is the callback function for when we have finished sending control urb
 *  on the control pipe.
 */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))        		        
static void mxuport_control_callback (struct urb *urb)
#else
static void mxuport_control_callback (struct urb *urb, struct pt_regs *regs)
#endif
{
    struct usb_ctrlrequest *req = (struct usb_ctrlrequest *)urb->context;

	switch (urb->status) {
	    case 0:
		    break;
    		    
	    case -ECONNRESET:
	    case -ENOENT:
	    case -ESHUTDOWN:
    		/* 
		     *  This urb is terminated, clean up 
		     */
		    dbg("%s - urb shutting down with status: %d", __FUNCTION__, urb->status);
            break;
		    
	    default:
    		dev_err (&urb->dev->dev, "%s - nonzero control status received: %d\n", __FUNCTION__, urb->status );
            break;
	}

    kfree(req);
    usb_free_urb(urb);
}

static void mx_load_firmware(struct mxuport_serial *mxserial)
{
	int	status = 0;	
	unsigned long txlen, fwsize, fwidx;
    unsigned long dev_ver, local_ver;
    unsigned char *fw_buf, *fw_tmp;
    unsigned char ver_buf[4];

#if 1
    /*
     *  Send vendor request - Get firmware version of SDRAM
     */
    status = mx_recv_ctrl_urb(mxserial->serial->dev,
                              RQ_VENDOR_GET_VERSION,
                              0,
                              0,
                              ver_buf,
                              sizeof(ver_buf));
	
	if(status < 0){
		dbg("%s - receive control URB failed. (status = %d)", __FUNCTION__, status);
		return;      
    }
    
	dev_ver = ver_buf[0] * 65536 + ver_buf[1] * 256 + ver_buf[2];
	
#endif	
	switch(mxserial->productid){
	    case MX_UPORT1250_PID:
            fwsize = sizeof(UPort1250FW) - 1;
          	fw_tmp = kmalloc (sizeof(unsigned char) * sizeof(UPort1250FW), GFP_KERNEL);
        	if (fw_tmp == NULL) {
        		dbg("%s - out of memory", __FUNCTION__);
	        	return;
	        }
	        memcpy(fw_tmp, UPort1250FW, sizeof(UPort1250FW));
	        break;

	    case MX_UPORT1251_PID:
            fwsize = sizeof(UPort1250IFW) - 1;
          	fw_tmp = kmalloc (sizeof(unsigned char) * sizeof(UPort1250IFW), GFP_KERNEL);
        	if (fw_tmp == NULL) {
        		dbg("%s - out of memory", __FUNCTION__);
	        	return;
	        }
	        memcpy(fw_tmp, UPort1250IFW, sizeof(UPort1250IFW));
	        break;

	    case MX_UPORT1410_PID:
            fwsize = sizeof(UPort1410FW) - 1;
          	fw_tmp = kmalloc (sizeof(unsigned char) * sizeof(UPort1410FW), GFP_KERNEL);
        	if (fw_tmp == NULL) {
        		dbg("%s - out of memory", __FUNCTION__);
	        	return;
	        }
	        memcpy(fw_tmp, UPort1410FW, sizeof(UPort1410FW));
	        break;

	    case MX_UPORT1450_PID:
            fwsize = sizeof(UPort1450FW) - 1;
          	fw_tmp = kmalloc (sizeof(unsigned char) * sizeof(UPort1450FW), GFP_KERNEL);
        	if (fw_tmp == NULL) {
        		dbg("%s - out of memory", __FUNCTION__);
	        	return;
	        }
	        memcpy(fw_tmp, UPort1450FW, sizeof(UPort1450FW));
	        break;

	    case MX_UPORT1451_PID:
            fwsize = sizeof(UPort1450IFW) - 1;
          	fw_tmp = kmalloc (sizeof(unsigned char) * sizeof(UPort1450IFW), GFP_KERNEL);
        	if (fw_tmp == NULL) {
        		dbg("%s - out of memory", __FUNCTION__);
	        	return;
	        }
	        memcpy(fw_tmp, UPort1450IFW, sizeof(UPort1450IFW));
	        break;

	    case MX_UPORT1618_PID:
            fwsize = sizeof(UPort1610_8FW) - 1;
          	fw_tmp = kmalloc (sizeof(unsigned char) * sizeof(UPort1610_8FW), GFP_KERNEL);
        	if (fw_tmp == NULL) {
        		dbg("%s - out of memory", __FUNCTION__);
	        	return;
	        }
	        memcpy(fw_tmp, UPort1610_8FW, sizeof(UPort1610_8FW));
	        break;

	    case MX_UPORT1658_PID:
            fwsize = sizeof(UPort1650_8FW) - 1;
          	fw_tmp = kmalloc (sizeof(unsigned char) * sizeof(UPort1650_8FW), GFP_KERNEL);
        	if (fw_tmp == NULL) {
        		dbg("%s - out of memory", __FUNCTION__);
	        	return;
	        }
	        memcpy(fw_tmp, UPort1650_8FW, sizeof(UPort1650_8FW));
	        break;

	    case MX_UPORT1613_PID:
            fwsize = sizeof(UPort1610_16FW) - 1;
          	fw_tmp = kmalloc (sizeof(unsigned char) * sizeof(UPort1610_16FW), GFP_KERNEL);
        	if (fw_tmp == NULL) {
        		dbg("%s - out of memory", __FUNCTION__);
	        	return;
	        }
	        memcpy(fw_tmp, UPort1610_16FW, sizeof(UPort1610_16FW));            
	        break;

	    case MX_UPORT1653_PID:
            fwsize = sizeof(UPort1650_16FW) - 1;
          	fw_tmp = kmalloc (sizeof(unsigned char) * sizeof(UPort1650_16FW), GFP_KERNEL);
        	if (fw_tmp == NULL) {
        		dbg("%s - out of memory", __FUNCTION__);
	        	return;
	        }
	        memcpy(fw_tmp, UPort1650_16FW, sizeof(UPort1650_16FW));
	        break;

	    default:
	        dbg("%s - Invalid UPort ID", __FUNCTION__);
	        return;
    }

    local_ver = fw_tmp[VER_ADDR_1] * 65536 + fw_tmp[VER_ADDR_2] * 256 + fw_tmp[VER_ADDR_3];

    if(local_ver <= dev_ver){
        kfree(fw_tmp);
	dev_info(&mxserial->serial->dev->dev, 
		"Device firmware version v%x.%x.%x\n", 
		ver_buf[0], ver_buf[1], ver_buf[2]);
        return;
    }
    
    dbg("[mxuport] - Starting download firmware ...");
    
    if((status = mx_send_ctrl_urb(mxserial->serial->dev,
                                  RQ_VENDOR_START_FW_DOWN,
                                  0,
                                  0,
                                  NULL,
                                  0)) != 0){
        kfree(fw_tmp);
        return;
    }
    
    /*
     *  download firmware to device.    
     */      
	fw_buf = kmalloc (sizeof(unsigned char) * DOWN_BLOCK_SIZE, GFP_KERNEL);
	if (fw_buf == NULL) {
		dbg("%s - out of memory", __FUNCTION__);
		kfree(fw_tmp);
		return;
	}
	
    fwidx = 0;
    do{
	txlen = (fwsize - fwidx) > DOWN_BLOCK_SIZE ? DOWN_BLOCK_SIZE : (fwsize - fwidx);

	memcpy(fw_buf, &fw_tmp[fwidx], txlen); 

	if((status = mx_send_ctrl_urb(mxserial->serial->dev,
                                      RQ_VENDOR_FW_DATA,
                                      0,
                                      0,
                                      fw_buf,
                                      txlen)) != txlen){
		kfree(fw_buf);
		kfree(fw_tmp);
		return;
	}

	fwidx += txlen;
	mdelay(1);

    }while(fwidx < fwsize);

    kfree(fw_buf);
    kfree(fw_tmp);
    mdelay(1000);        
    if((status = mx_send_ctrl_urb(mxserial->serial->dev,
                                  RQ_VENDOR_STOP_FW_DOWN,
                                  0,
                                  0,
                                  NULL,
                                  0)) != 0)
    return;
    mdelay(1000);       
	  
    if((status = mx_send_ctrl_urb(mxserial->serial->dev,
                                  RQ_VENDOR_QUERY_FW_READY,
                                  0,
                                  0,
                                  NULL,
                                  0)) != 0)
        return;

    dbg("[mxuport] - Download firmware completely.");

    status = mx_recv_ctrl_urb(mxserial->serial->dev,
                              RQ_VENDOR_GET_VERSION,
                              0,
                              0,
                              ver_buf,
                              sizeof(ver_buf));
	
	if(status < 0){
		dbg("%s - receive control URB failed. (status = %d)", __FUNCTION__, status);
		return;      
    }
    
	dev_ver = ver_buf[0] * 65536 + ver_buf[1] * 256 + ver_buf[2];
	
	dev_info(&mxserial->serial->dev->dev, 
		"Device firmware version v%x.%x.%x\n", 
		ver_buf[0], ver_buf[1], ver_buf[2]);
}
/*
 *  mx_wait_fw_until_sent - wait firmware transmit buffer empties
 *
 *  Parameter timeout is a flag (1 or 0). 
 *  If timeout = 1, using a best effort transmit approach 
 *             = 0, exit until firmware transmit buffer empties
 */
static void mx_wait_fw_until_sent(struct usb_serial_port *port, int timeout)
{
    int status, count;
	int equal_cnt, pass_cnt;
    unsigned long fw_txlen;
    unsigned char len_buf[4];
    struct mxuport_port *mx_port = usb_get_serial_port_data(port);

    count = 0;
    pass_cnt = 0;
    equal_cnt = 0;    

    while(1){
        /*
         *  get remaining data length of transmit buffer
         */
        status = mx_recv_ctrl_urb(port->serial->dev,
                                  RQ_VENDOR_GET_OUTQUEUE,
                                  0,
                                  mx_port->portno,
                                  len_buf,
                                  sizeof(len_buf));
	
	    if(status < 0){
    		dbg("%s - receive control URB failed. (status = %d)", __FUNCTION__, status);
		    return;      
        }
    
	    fw_txlen = (len_buf[0] << 24) | (len_buf[1] << 16) | (len_buf[2] << 8) | len_buf[3];

        dbg("%s - firmware tx len = %ld\n", __FUNCTION__, fw_txlen);

        if(fw_txlen == 0)
            break;
    
        if(timeout){
			if(count != fw_txlen){
				count = fw_txlen;
				pass_cnt++;
				equal_cnt = 0;
			}else{
				equal_cnt++;
			}
    	
    		if((pass_cnt > 20)||(equal_cnt > 30))
	    		break;            
        }
        
        set_current_state(TASK_INTERRUPTIBLE);
        schedule_timeout(HZ);     /* delay 1 second */
    }
}   
static int mx_init_port (struct usb_serial_port *port)
{
    int status = 0;
    struct mxuport_port *mx_port = usb_get_serial_port_data(port);

	dbg("%s - Port number : %d", __FUNCTION__, mx_port->portno);

    if(mx_port == NULL)
        return -ENODEV;

		
    /*
     *  Send vendor request - set FIFO (Enable)
     */
    status = mx_send_ctrl_urb(port->serial->dev,
                              RQ_VENDOR_SET_FIFO_DISABLE,
                              0,
                              mx_port->portno,
                              NULL,
                              0);

	if(status)
		return status;  

    /*
     *  Send vendor request - set transmition mode (Hi-Performance)
     */
    status = mx_send_ctrl_urb(port->serial->dev,
                              RQ_VENDOR_SET_HIGH_PERFOR,
                              0,
                              mx_port->portno,
                              NULL,
                              0);

	if(status)
		return status;  
    
    /*
     *  Send vendor request - set interface (RS-232)
     */
    status = mx_send_ctrl_urb(port->serial->dev,
                              RQ_VENDOR_SET_INTERFACE,
                              mx_port->uart_cfg->interface,
                              mx_port->portno,
                              NULL,
                              0);

	if(status)
		return status;  


	
    /*
     *  Send vendor request - port open
     */

    status = mx_send_ctrl_urb(port->serial->dev,
                              RQ_VENDOR_SET_OPEN,
                              1,
                              mx_port->portno,
                              NULL,
                              0);

	
	if(status)
		return status;  
    
    /*
     *  Send vendor request - set receive host (enable)
     */

    status = mx_send_ctrl_urb(port->serial->dev,
                              RQ_VENDOR_SET_RX_HOST_EN,
                              1,
                              mx_port->portno,
                              NULL,
                              0);

	if(status)
		return status;  

	
    return status;		   
} 

/*
 *  mx_tty_recv - put receive data to tty layer
 *
 *	This function passes data on to the tty flip buffer
 */
#if (defined(_SCREEN_INFO_H) || (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15)))
static void mx_tty_recv(struct tty_struct *tty, struct mxuport_port *mxport)
{
    int room_cnt, rx_cnt, length, data_len;
    unsigned char *rcv_buf;
   unsigned long flags;	 	
  
   
    length = mx_data_in_queue(mxport->read_q);   

    rcv_buf = kmalloc(sizeof(unsigned char) * length, GFP_ATOMIC);
    if (rcv_buf == NULL)
        return;

    atomic_set(&mxport->read_in_progress, 1);

	do {
        if(mxport->hold_reason & MX_WAIT_FOR_UNTHROTTLE)
            break;

  	    room_cnt = tty_buffer_request_room(tty, length);
		if (room_cnt < length) {
			dbg("%s - dropping data, %d bytes lost\n", __FUNCTION__, length);
            if((room_cnt == 0) || (room_cnt < TTY_THRESHOLD_THROTTLE))
	            break;
        }
		spin_lock_irqsave(&mxport->read_splock, flags);
		rx_cnt = mx_remove_rx_queue(mxport->read_q, rcv_buf, room_cnt);
		
		spin_unlock_irqrestore(&mxport->read_splock, flags);	

		
		tty_insert_flip_string(tty, rcv_buf, rx_cnt);
		
		 
        length -= rx_cnt;
	 spin_lock_irqsave(&mxport->read_splock, flags);
        mxport->icount.rx += rx_cnt;
        spin_unlock_irqrestore(&mxport->read_splock, flags);	
    	tty_flip_buffer_push(tty);
		
		
    } while (length > 0);

    if(mxport->hold_reason & MX_WAIT_FOR_LOW_WATER){
	spin_lock_irqsave(&mxport->read_splock, flags);	
        data_len = mx_data_in_queue(mxport->read_q);
	spin_unlock_irqrestore(&mxport->read_splock, flags);	
        if(data_len <= LOW_WATER_SIZE){
           spin_lock_irqsave(&mxport->read_splock, flags);
            mxport->hold_reason &= ~MX_WAIT_FOR_LOW_WATER;
           spin_unlock_irqrestore(&mxport->read_splock, flags);	
            mx_send_async_ctrl_urb(mxport->port->serial->dev,
                                   RQ_VENDOR_SET_RX_HOST_EN,
                                   1,
                                   mxport->portno,
                                   NULL,
                                   0);                      
        }
    }        

    atomic_set(&mxport->read_in_progress, 0);
    kfree(rcv_buf);
}
#else
static void mx_tty_recv(struct tty_struct *tty, struct mxuport_port *mxport)
{
	int		data_cnt, get_cnt, data_len; 
    int             	room_cnt, rmv_cnt;
    char            	*fp;
    unsigned char   	*ch;
    unsigned char  		*rcv_buf;
    unsigned long flags;		

    ch = tty->flip.char_buf;
    fp = tty->flip.flag_buf;

    rcv_buf = kmalloc(sizeof(unsigned char) * TTY_FLIPBUF_SIZE, GFP_ATOMIC);
    if (rcv_buf == NULL){
        return;
	}

    data_cnt = mx_data_in_queue(mxport->read_q);    
	if (data_cnt == 0)
		return;

    atomic_set(&mxport->read_in_progress, 1);

    do{
    	if(mxport->hold_reason & MX_WAIT_FOR_UNTHROTTLE)
		    break;
	spin_lock_irqsave(&mxport->read_splock, flags);
        room_cnt = tty->ldisc.receive_room(tty);
	spin_unlock_irqrestore(&mxport->read_splock, flags);

        if((room_cnt == 0) || (room_cnt < TTY_THRESHOLD_THROTTLE))
            break;    
        
        if(room_cnt < data_cnt)
            data_cnt = room_cnt;    

	    if(data_cnt > TTY_FLIPBUF_SIZE){
    		get_cnt = TTY_FLIPBUF_SIZE;
    	}else{
		    get_cnt = data_cnt;
        }
   	 spin_lock_irqsave(&mxport->read_splock, flags);
	    rmv_cnt = mx_remove_rx_queue(mxport->read_q, rcv_buf, get_cnt);
	spin_unlock_irqrestore(&mxport->read_splock, flags);	  
	    if(rmv_cnt == 0)
    		break;

        memcpy(ch, rcv_buf, rmv_cnt);
        memset(fp, 0, rmv_cnt);
        spin_lock_irqsave(&mxport->read_splock, flags);
        tty->flip.count += rmv_cnt;
        spin_unlock_irqrestore(&mxport->read_splock, flags);	
	data_cnt -= rmv_cnt;

        tty->ldisc.receive_buf(tty, 
                           tty->flip.char_buf, 
                           tty->flip.flag_buf,
                           rmv_cnt); 

    }while(data_cnt > 0);

    if(mxport->hold_reason & MX_WAIT_FOR_LOW_WATER){
    	spin_lock_irqsave(&mxport->read_splock, flags);
        data_len = mx_data_in_queue(mxport->read_q);
        spin_unlock_irqrestore(&mxport->read_splock, flags);	
        if(data_len <= LOW_WATER_SIZE){
	    spin_lock_irqsave(&mxport->read_splock, flags);
            mxport->hold_reason &= ~MX_WAIT_FOR_LOW_WATER;
	    spin_unlock_irqrestore(&mxport->read_splock, flags);
            mx_send_async_ctrl_urb(mxport->port->serial->dev,
                                   RQ_VENDOR_SET_RX_HOST_EN,
                                   1,
                                   mxport->portno,
                                   NULL,
                                   0);                      
        }
    }    
    
    atomic_set(&mxport->read_in_progress, 0);

    kfree(rcv_buf);
}
#endif

static int mx_change_port_settings(struct mxuport_port *mxport)
{
	struct mx_uart_config *config;
	struct tty_struct *tty;
	int baud, status;
	unsigned int cflag;
	unsigned char buf[4];

	dbg("%s - port %d enter", __FUNCTION__, mxport->portno);

	tty = mxport->port->tty;
	if ((!tty)||(!tty->termios)) {
		dbg("%s - no tty structures", __FUNCTION__);
		return 0;
	}

	cflag = tty->termios->c_cflag;
	
	config = kmalloc (sizeof(struct mx_uart_config), GFP_KERNEL);
	if (!config) {
		dev_err (&mxport->port->dev, "%s - out of memory\n", __FUNCTION__);
		return -ENOMEM;
	}
    
    /*
     *  initial local uart config
     */
    memset(config, 0, sizeof(struct mx_uart_config));
    config->cXon = mxport->uart_cfg->cXon;
    config->cXoff = mxport->uart_cfg->cXoff;
    config->interface = mxport->uart_cfg->interface;

	/*
	 *  Set date bit of termios
	 */
	switch (cflag & CSIZE) {
		case CS5:
			    config->bDataBits = MX_WORDLENGTH_5;
			    dbg ("%s - data bits = 5", __FUNCTION__);
			    break;
		case CS6:
			    config->bDataBits = MX_WORDLENGTH_6;
			    dbg ("%s - data bits = 6", __FUNCTION__);
			    break;
		case CS7:
			    config->bDataBits = MX_WORDLENGTH_7;
			    dbg ("%s - data bits = 7", __FUNCTION__);
			    break;
		case CS8:
        default:
			    config->bDataBits = MX_WORDLENGTH_8;
			    dbg ("%s - data bits = 8", __FUNCTION__);
			    break;
	}

	/*
	 *  Set parity of termios
	 */
	if (cflag & PARENB) {

		if (cflag & CMSPAR) {
			if (cflag & PARODD) {
				config->bParity = MX_PARITY_MARK;   
				dbg("%s - parity = mark", __FUNCTION__);
			} else {
				config->bParity = MX_PARITY_SPACE;  
				dbg("%s - parity = space", __FUNCTION__);
			}
		} else if (cflag & PARODD) {
		    config->bParity = MX_PARITY_ODD;
			dbg("%s - parity = odd", __FUNCTION__);
		} else {
			config->bParity = MX_PARITY_EVEN;  
			dbg("%s - parity = even", __FUNCTION__);
		}
		
	} else {
	    config->bParity = MX_PARITY_NONE;
		dbg("%s - parity = none", __FUNCTION__);
	}

	/*
	 *  Set stop bit of termios
	 */
	if (cflag & CSTOPB) {
		config->bStopBits = MX_STOP_BITS_2;
		dbg("%s - stop bits = 2", __FUNCTION__);
	} else {
		config->bStopBits = MX_STOP_BITS_1;
		dbg("%s - stop bits = 1", __FUNCTION__);
	}

    /*
     *  Submit the vendor request
     */
    buf[0] = (unsigned char)config->bDataBits;
    buf[1] = (unsigned char)config->bParity;
    buf[2] = (unsigned char)config->bStopBits;
    buf[3] = 0;
    
    status = mx_send_ctrl_urb(mxport->port->serial->dev,
                              RQ_VENDOR_SET_LINE,
                              0,
                              mxport->portno,
                              buf,
                              sizeof(buf));

	if(status != sizeof(buf))
		return status;

	/* 
	 *  Flow control settings 
	 */
	config->bFlowCtrl = 0;
	
	/* 
	 *  H/W flow control settings 
	 */
	if ((cflag & CRTSCTS) && (cflag & CBAUD)){
		config->bFlowCtrl |= MX_HW_FLOWCTRL;
		dbg("%s - RTS/CTS is enabled", __FUNCTION__);
	} else {
		config->bFlowCtrl &= ~MX_HW_FLOWCTRL;
		dbg("%s - RTS/CTS is disabled", __FUNCTION__);
	}

    /*
     *  Submit the vendor request
     */  
  	if(config->bFlowCtrl & MX_HW_FLOWCTRL){
	

        status = mx_send_ctrl_urb(mxport->port->serial->dev,
                                  RQ_VENDOR_SET_RTS,
                                  0x2,
                                  mxport->portno,
                                  NULL,
                                  0);
		

		if(status)
			return status;
	
		
  	}
	
	else
	{

		 status = mx_send_ctrl_urb(mxport->port->serial->dev,
                	          RQ_VENDOR_SET_RTS,
        	                  0x1,
                                  mxport->portno,
                                  NULL,
                                  0);

	}
		
	
	
    /*
     *  if baud rate is B0, drop DTR and RTS
     */
   
    if(!(cflag & CBAUD)){

	mxport->set_B0 = 1;    

	mxport->mcr_state &= ~(UART_MCR_DTR | UART_MCR_RTS);	

	if ((status = mx_send_ctrl_urb (mxport->port->serial->dev,
      	                   	    RQ_VENDOR_SET_MCR,
               	      	            mxport->mcr_state,
                       	            mxport->portno,
                               	    NULL,
                                    0)) != 0) {
		    return status;
	}
    }

    else {

	if(mxport->set_B0) {
		
		mxport->set_B0 = 0;

		if ((status = mx_send_ctrl_urb (mxport->port->serial->dev,
      		                      	    RQ_VENDOR_SET_DTR,
               		      	            1,
                       		            mxport->portno,
                               		    NULL,
	                                    0)) != 0) {
			    return status;
		}
	}
    }
	/* 
	 *  S/W flow control settings 
	 */
	if (I_IXOFF(tty) || I_IXON(tty)) {
		config->cXon  = START_CHAR(tty);
		config->cXoff = STOP_CHAR(tty);
		
        /*
         *  Submit the vendor request
         */
        buf[0] = (unsigned char)config->cXon;
        buf[1] = (unsigned char)config->cXoff;
    
        status = mx_send_ctrl_urb(mxport->port->serial->dev,
                                  RQ_VENDOR_SET_CHARS,
                                  0,
                                  mxport->portno,
                                  buf,
                                  2);

		if(status != 2)
			return status;
		}	

		/* 
		 *  if we are implementing OUTBOUND XON/XOFF 
		 */
		if (I_IXON(tty)) {
    		config->bFlowCtrl |= MX_XON_FLOWCTRL;
			dbg ("%s - OUTBOUND XON/XOFF is enabled, XON = 0x%2x, XOFF = 0x%2x",
			     __FUNCTION__, config->cXon, config->cXoff);
		} else {
    		config->bFlowCtrl &= ~MX_XON_FLOWCTRL;		    
			dbg ("%s - OUTBOUND XON/XOFF is disabled", __FUNCTION__);
		}
		
		/* 
		 *  if we are implementing INBOUND XON/XOFF 
		 */
		if (I_IXOFF(tty)) {
    		config->bFlowCtrl |= MX_XOFF_FLOWCTRL;
			dbg ("%s - INBOUND XON/XOFF is enabled, XON = 0x%2x, XOFF = 0x%2x",
			     __FUNCTION__, config->cXon, config->cXoff);
		} else {
    		config->bFlowCtrl &= ~MX_XOFF_FLOWCTRL;
			dbg ("%s - INBOUND XON/XOFF is disabled", __FUNCTION__);
		}

		

        /*
         *  Submit the vendor request
         */  
    	if(config->bFlowCtrl & (MX_XON_FLOWCTRL | MX_XOFF_FLOWCTRL)){
            status = mx_send_ctrl_urb(mxport->port->serial->dev,
                                      RQ_VENDOR_SET_XONXOFF,
                                      1,
                                      mxport->portno,
                                      NULL,
                                      0);

			if(status)
				return status;
	}else{
            status = mx_send_ctrl_urb(mxport->port->serial->dev,
                                      RQ_VENDOR_SET_XONXOFF,
                                      0,
                                      mxport->portno,
                                      NULL,
                                      0);

			if(status)
				return status;
        }
	

	/* 
	 *  Set baud rate of termios
	 */
	baud = tty_get_baud_rate(tty);
	if (!baud) {
		/* 
		 *  pick a default using 9600 
		 */
		baud = 9600;
	}
    config->wBaudRate = (long)baud;

    /*
     *  Submit the vendor request
     */
    buf[0] = (unsigned char)(config->wBaudRate & 0xFF);
    buf[1] = (unsigned char)((config->wBaudRate >> 8) & 0xFF);
    buf[2] = (unsigned char)((config->wBaudRate >> 16) & 0xFF);
    buf[3] = (unsigned char)((config->wBaudRate >> 24) & 0xFF);
    
    status = mx_send_ctrl_urb(mxport->port->serial->dev,
                              RQ_VENDOR_SET_BAUD,
                              0,
                              mxport->portno,
                              buf,
                              sizeof(buf));

	if(status != sizeof(buf))
		return status;

    /*
     *  Finally, store the uart settings to private structure
     */
    memcpy(mxport->uart_cfg, config, sizeof(struct mx_uart_config));     
    
    /*
     *  Dump serial settings
     */

	dbg ("wBaudRate  : %ld",  mxport->uart_cfg->wBaudRate);
	dbg ("bDataBits  : %d",   mxport->uart_cfg->bDataBits);
	dbg ("bParity    : %d",   mxport->uart_cfg->bParity);
	dbg ("bStopBits  : %d",   mxport->uart_cfg->bStopBits);
	dbg ("bFlowCtrl  : %d",   mxport->uart_cfg->bFlowCtrl);
	dbg ("cXon       : 0x%x", mxport->uart_cfg->cXon);
	dbg ("cXoff      : 0x%x", mxport->uart_cfg->cXoff);
	dbg ("Interface  : %d",   mxport->uart_cfg->interface);

	kfree (config);

	dbg("%s - port %d exit", __FUNCTION__, mxport->portno);
	
	return 0;
}

static int mx_set_modem_info(struct usb_serial_port *port, struct file *file, unsigned int cmd, unsigned long arg)
{
    int status;
    unsigned int val, set, clear; 
    
    if (get_user(val, (unsigned int *) arg))
	   return -EFAULT;
    
    set = clear = 0;

    switch(cmd){
        case TIOCMSET:
            set = val;
            clear = ~val;
            break;    
        case TIOCMBIS:
            set = val;
            break;
        case TIOCMBIC:
            clear = val;
            break;
    }

    status = mxuport_tiocmset(port, file, set, clear);

    return status;
}

static int mx_get_serial_info(struct mxuport_port *mxport,
						      struct serial_struct __user *ret_arg)
{
	struct usb_serial_port *port = mxport->port;
	struct serial_struct tmp_serial;

	if (!ret_arg)
		return -EFAULT;

	memset(&tmp_serial, 0, sizeof(tmp_serial));

	tmp_serial.type           = PORT_16550A;
	tmp_serial.line           = port->serial->minor;
	tmp_serial.port           = mxport->uart_cfg->interface;
	tmp_serial.flags          = mxport->flags;
	tmp_serial.xmit_fifo_size = MAX_QUEUE_SIZE;
	tmp_serial.baud_base      = 921600;

	if (copy_to_user(ret_arg, &tmp_serial, sizeof(*ret_arg)))
		return -EFAULT;

	return 0;
}

static int mx_set_serial_info(struct mxuport_port *mxport,
							  struct serial_struct __user *new_arg)
{
	int status;
    struct usb_serial_port *port = mxport->port;
	struct mxuport_port old_cfg;
    struct mxuport_serial *mx_serial;
	struct serial_struct new_serial;

	if (copy_from_user(&new_serial, new_arg, sizeof(new_serial)))
		return -EFAULT;

    mx_serial = usb_get_serial_data(mxport->port->serial);
    if(mx_serial == NULL)
        return -ENODEV;

    old_cfg = *mxport;

	if (!capable(CAP_SYS_ADMIN)) {
		if (((new_serial.flags & ~ASYNC_USR_MASK) !=
		     (mxport->flags & ~ASYNC_USR_MASK)))
			return -EPERM;
		mxport->flags = ((mxport->flags & ~ASYNC_USR_MASK) |
			            (new_serial.flags & ASYNC_USR_MASK));
	}else{ 
      	/* 
      	 *  Mark the changes - these are for privileged user
      	 *  (we don't support it!)
      	 */
    	mxport->flags = ((mxport->flags & ~ASYNC_FLAGS) |
	                     (new_serial.flags & ASYNC_FLAGS));		                     	                     
	}

    /*
     *  set port interface
     */
    if((mx_serial->productid == MX_UPORT1450_PID) ||
       (mx_serial->productid == MX_UPORT1451_PID) ||        
       (mx_serial->productid == MX_UPORT1658_PID) ||
       (mx_serial->productid == MX_UPORT1653_PID) ||
       (mx_serial->productid == MX_UPORT1250_PID) ||
       (mx_serial->productid == MX_UPORT1251_PID)){

        if((new_serial.port >= 0) && (new_serial.port <= 3)){
            if(old_cfg.uart_cfg->interface != new_serial.port){
                mxport->uart_cfg->interface = new_serial.port;
                if((status = mx_send_ctrl_urb(mxport->port->serial->dev,
                                              RQ_VENDOR_SET_INTERFACE,
                                              mxport->uart_cfg->interface,
                                              mxport->portno,
                                              NULL,
                                              0)) != 0)     
                	return status;                
            }
        }
    }else{
        if(old_cfg.uart_cfg->interface != new_serial.port)
            return -EINVAL;
    }

    /*
     *  set low_latency flag
     */
	if (port->tty)
        port->tty->low_latency = (mxport->flags & ASYNC_LOW_LATENCY) ? 1 : 0;

    /*
     *  set alternative baud rate
     */
	if ((old_cfg.flags & ASYNC_SPD_MASK) != (mxport->flags & ASYNC_SPD_MASK)) {
    	if ((mxport->flags & ASYNC_SPD_MASK) == ASYNC_SPD_HI)
			port->tty->alt_speed = 57600;
		if ((mxport->flags & ASYNC_SPD_MASK) == ASYNC_SPD_VHI)
			port->tty->alt_speed = 115200;
		if ((mxport->flags & ASYNC_SPD_MASK) == ASYNC_SPD_SHI)
			port->tty->alt_speed = 230400;
		if ((mxport->flags & ASYNC_SPD_MASK) == ASYNC_SPD_WARP)
			port->tty->alt_speed = 460800;
		mx_change_port_settings(mxport);
	}
		
	return 0;
}

static void mx_send_break(struct usb_serial_port *port, int period)
{
    struct mxuport_port *mx_port = usb_get_serial_port_data(port);
    
    set_current_state(TASK_INTERRUPTIBLE);

	mx_send_ctrl_urb (port->serial->dev,
              	      RQ_VENDOR_SET_BREAK,
              	      1,
                      mx_port->portno,
                      NULL,
                      0);
    
    schedule_timeout(period);
}    


 
static void mx_update_recv_event(struct mxuport_port *mxport, unsigned long event,
                                unsigned char *buf, unsigned long idx)
{
    unsigned long rcv_msr_event, rcv_msr_hold;
    unsigned long lsr_event, flags;
    unsigned long mcr_event;
    struct async_icount *icount;

	
    if(mxport->opened){

		dbg("%s - port %d receive event : %ld", __FUNCTION__, mxport->portno, event);        
        
		switch(event){
            case UPORT_EVNET_SEND_NEXT:
                if(mxport->hold_reason & MX_WAIT_FOR_SEND_NEXT){
			spin_lock_irqsave(&mxport->write_splock, flags);
                    mxport->hold_reason &= ~ MX_WAIT_FOR_SEND_NEXT;
                    mxport->send_len = 0;
                    mx_prepare_write(mxport->port);
			spin_unlock_irqrestore(&mxport->write_splock, flags);		
                }
                break;
                
            case UPORT_EVNET_MSR:
				rcv_msr_event = ((unsigned long)buf[idx + 5] & 0xFF) + (((unsigned long)buf[idx + 4] & 0xFF) * 256);
                rcv_msr_hold = ((unsigned long)buf[idx + 6] & 0xF0);
				icount = &mxport->icount;
				
				dbg("%s - current MSR status = 0x%x", __FUNCTION__, mxport->msr_state);
                
                /*
                 *  Update hold reason
                 */
		if(rcv_msr_event != 0 ) {

			spin_lock_irqsave(&mxport->port_splock, flags);


				if(!(rcv_msr_hold & UART_MSR_CTS)){
					mxport->hold_reason |= MX_WAIT_FOR_CTS;
					mxport->msr_state &= ~UART_MSR_CTS;
					dbg("CTS Low");
				}else{
					mxport->hold_reason &= ~MX_WAIT_FOR_CTS;
					mxport->msr_state |= UART_MSR_CTS;
					dbg("CTS High");
				}
      

				if(!(rcv_msr_hold & UART_MSR_DSR)){
					mxport->hold_reason |= MX_WAIT_FOR_DSR;
					mxport->msr_state &= ~UART_MSR_DSR;
					dbg("DSR Low");
				}else{
					mxport->msr_state |= UART_MSR_DSR;
					mxport->hold_reason &= ~MX_WAIT_FOR_DSR;
					dbg("DSR High");
				}
						
				if(!(rcv_msr_hold & UART_MSR_DCD)){
					mxport->hold_reason |= MX_WAIT_FOR_DCD;
					mxport->msr_state &= ~UART_MSR_DCD;
					dbg("DCD Low");
				}else{
					mxport->msr_state |= UART_MSR_DCD;
					mxport->hold_reason &= ~MX_WAIT_FOR_DCD;
					dbg("DCD High");
				}		
			spin_unlock_irqrestore(&mxport->port_splock, flags);	
		}
		
                /*
                 *  Update MSR status
                 */
            	if (rcv_msr_event & (SERIAL_EV_CTS | SERIAL_EV_DSR | SERIAL_EV_RLSD)) {
				
				    if(rcv_msr_event & SERIAL_EV_CTS){
    				    icount->cts++;
    					dbg("CTS change");
    				}
				    
				    if(rcv_msr_event & SERIAL_EV_DSR){
    				    icount->dsr++;
    					dbg("DSR change");
    				}
				    				    
				    if(rcv_msr_event & SERIAL_EV_RLSD){
    				    icount->dcd++;				   
					    dbg("DCD change");
    				} 
    				
                    if(atomic_read(&mxport->wait_msr)){
                        atomic_set(&mxport->wait_msr, 0);
            		    wake_up_interruptible(&mxport->delta_msr_wait);
            		}
            	}               
                break;
                
            case UPORT_EVNET_LSR:
				lsr_event = (unsigned long)buf[idx + 6] & 0xFF;
				icount = &mxport->icount;
                
				if(lsr_event & UART_LSR_BI){ 
					mxport->comm_err |= UART_LSR_BI;
					icount->brk++;
					dbg("Break error");
				}
				
				if(lsr_event & UART_LSR_FE){ 
					mxport->comm_err |= UART_LSR_FE;
					icount->frame++;
					dbg("Frame error");
    			}
	
				if(lsr_event & UART_LSR_PE){ 
					mxport->comm_err |= UART_LSR_PE;
					icount->parity++;
					dbg("Parity error");
				}

				if(lsr_event & UART_LSR_OE){ 
					mxport->comm_err |= UART_LSR_OE;
					icount->overrun++;
					dbg("Overrun error");
				}
				
				mxport->lsr_state = lsr_event;
				               
                break;
                
            case UPORT_EVNET_MCR:
                mcr_event = (unsigned long)buf[idx + 4] & 0xFF;
                
            	if(mcr_event & 0x40){ 
					mxport->hold_reason |= MX_WAIT_FOR_XON;
				}else{ 
					mxport->hold_reason &= ~MX_WAIT_FOR_XON;
				}
                break;
                
            default:
                break;
	    }	    
    }    
}    

static int mx_check_close_port (struct mxuport_port *mxport, int portnum)
{
	int status;

	dbg("%s - port number %d", __FUNCTION__, portnum);

    /*
     *  wait firmware to transmit data (best effort)
     */
    mx_wait_fw_until_sent(mxport->port, 1);

		
    /*
     *  Build the vendor request
     */


	status = mx_send_ctrl_urb(mxport->port->serial->dev,
             		          RQ_VENDOR_SET_OPEN,
                    		  0,
                     		  portnum,
                     		  NULL,
                     		  0);

	return status;                     
}

static void mx_send_async_ctrl_urb(struct usb_device *dev,
                                   __u8   request,
                                   __u16  value,
				                   __u16  index,
                                   u8     *data,
                                   int    size)
{
	struct usb_ctrlrequest *req;
	int status;
	struct urb *urb;

	if ((urb = usb_alloc_urb(0, GFP_ATOMIC)) == NULL) {
		dbg("Error allocating URB in write_cmd_async!");
		return;
	}

	if ((req = kmalloc(sizeof(struct usb_ctrlrequest), GFP_ATOMIC)) == NULL) {
		dbg("Failed to allocate memory for control request");
		usb_free_urb(urb);
		return;
	}

	req->bRequestType = USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE;
	req->bRequest     = request;
	req->wValue       = cpu_to_le16(value);
	req->wIndex       = cpu_to_le16(index); 
	req->wLength      = cpu_to_le16(size);

	usb_fill_control_urb(urb, 
	                     dev,
			             usb_sndctrlpipe(dev, 0),
			             (void *)req, 
			             data, 
			             size,
			             mxuport_control_callback, 
			             req);
	
	if((status = usb_submit_urb(urb, GFP_ATOMIC)) < 0) {
		dbg("Error submitting the control message: status=%d", status);
		kfree(req);
		usb_free_urb(urb);
	}
}


/*
 *  mx_recv_ctrl_urb - receive vendor request
 *
 *	This function read the given buffer in from the control pipe.
 */
static int mx_recv_ctrl_urb (struct usb_device *dev,
                              __u8   request,
                              __u16  value,
				              __u16  index,
                              u8     *data,
                              int    size)
{
	int status;

	status = usb_control_msg (dev,
	                          usb_rcvctrlpipe(dev, 0),
				              request,
				              (USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE),
				              value,
				              index,
				              data,
				              size,
				              HZ * USB_CTRL_GET_TIMEOUT);

	if (status < 0){
		dbg("%s - send usb_control_msg failed. (%d)",
		    __FUNCTION__, status);
	    return status;
	}

	if (status != size) {
		dbg("%s - wanted to write %d, but only wrote %d",
		    __FUNCTION__, size, status);
		return -ECOMM;
	}
	
	return status;
}
/*
 *  mx_send_ctrl_urb - send vendor equest
 *
 *	This function writes the given buffer out to the control pipe.
 */
static int mx_send_ctrl_urb (struct usb_device *dev,
				__u8   request,
				__u16  value,
				__u16  index,
				u8     *data,
				int    size)
{
	int status;
	status = usb_control_msg (dev,
	                          usb_sndctrlpipe(dev, 0),
				              request,
				              (USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE),
				              value,
				              index,
				              data,
				              size,
				              HZ * USB_CTRL_SET_TIMEOUT);
	if (status < 0){
		dbg("%s - send usb_control_msg failed. (%d)",
		    __FUNCTION__, status);
	    return status;
	}

	if (status != size) {
		dbg("%s - wanted to write %d, but only wrote %d",
		    __FUNCTION__, size, status);
		return -ECOMM;
	}
	
	return status;
}
/*
 *  mx_prepare_write - prepare write data to device
 *
 *  caculate the data length in write queue and write it out to device.
 *  if success, return write bytes
 *  otherwise, return negtive value. 
 */
static void mx_prepare_write(struct usb_serial_port *port)
{	
    int get_len, data_cnt;
    unsigned char *usb_buf;	
    struct mxuport_port *mx_port = usb_get_serial_port_data(port);
    struct mxuport_serial *mx_serial = usb_get_serial_data(port->serial);
	
    if((mx_port == NULL)||(mx_serial == NULL))
	    return;

    if((mx_port->hold_reason & MX_WAIT_FOR_START_TX) ||
       (mx_port->hold_reason & MX_WAIT_FOR_SEND_NEXT)){
        return;
    }

    if(atomic_read(&mx_port->write_in_progress)){
        return;
    }

    
    
	data_cnt = mx_data_in_queue(mx_port->write_q);	
	if(data_cnt == 0){
    	   
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))        		        
	    schedule_work(&port->work);
#else
             
           mx_usbserial_port_softint(mx_port->port);
#endif
	    return;
	}

    atomic_set(&mx_port->write_in_progress, 1);

	
	usb_buf = kmalloc(mx_serial->bulkout_size, GFP_ATOMIC);
	if (usb_buf == NULL) {
		dev_err(&port->serial->dev->dev, "%s - Out of memory\n", __FUNCTION__);
		return;
	}	

    if(mx_port->send_len >= TRIGGER_SEND_NEXT){
	
        atomic_set(&mx_port->write_q->add_sn, 1);
        mx_port->hold_reason |= MX_WAIT_FOR_SEND_NEXT;
		
    }

    
    get_len = mx_remove_tx_queue(mx_port->write_q, usb_buf, mx_serial->bulkout_size);
    

    mx_write_bulkout_urb(port, usb_buf, get_len);
   
    kfree(usb_buf);
}

/*
 *  mx_write_bulkout_urb - send bulk urb
 *
 *	This function writes the given buffer out to the bulk write endpoint.
 */
static int mx_write_bulkout_urb (struct usb_serial_port *port, unsigned char *buffer, int length)
{ 
	int    status = 0;
	struct mxuport_port	   *mx_port = usb_get_serial_port_data(port);
	struct mxuport_serial  *mx_serial = usb_get_serial_data(port->serial);	
   
	//usb_serial_debug_data(debug, &mx_port->port->dev, __FUNCTION__, length, buffer);

	/* 
	 *  Fill with write urb 
	 */
	if(mx_port->write_urb->transfer_buffer == NULL){
		mx_port->write_urb->transfer_buffer = kmalloc(mx_serial->bulkout_size, GFP_ATOMIC);
		if(mx_port->write_urb->transfer_buffer == NULL)
			return -ENOMEM;
	}

	memcpy(mx_port->write_urb->transfer_buffer, buffer, length); 
	mx_port->write_urb->transfer_buffer_length = length;
	 
	usb_fill_bulk_urb(mx_port->write_urb, 
	                  port->serial->dev, 
		              usb_sndbulkpipe(port->serial->dev, mx_serial->write_bulkout_addr),
		              mx_port->write_urb->transfer_buffer, 
		              mx_port->write_urb->transfer_buffer_length, 
		              mxuport_write_bulk_callback, 
		              port);

	status = usb_submit_urb(mx_port->write_urb, GFP_ATOMIC);

	if (status) {
		dbg("%s - USB submit write urb failed (status = %d)", __FUNCTION__, status);
		return status;
	}else{
	    mx_port->icount.tx += (length - HEADER_SIZE);
	    mx_port->send_len += (length - HEADER_SIZE);
    }

	return status;
}

/*
 *  Get string descriptor from device					
 */
static int mx_get_string (struct usb_device *dev, int Id, char *string)
{
	struct usb_string_descriptor StringDesc;
	struct usb_string_descriptor *pStringDesc;

	if (!usb_get_descriptor(dev, USB_DT_STRING, Id, &StringDesc, sizeof(StringDesc))) {
		return 0;
	}

	pStringDesc = kmalloc (StringDesc.bLength, GFP_KERNEL);

	if (!pStringDesc) {
		return 0;
	}

	if (!usb_get_descriptor(dev, USB_DT_STRING, Id, pStringDesc, StringDesc.bLength )) {
		kfree(pStringDesc);
		return 0;
	}

	mx_unicode_to_ascii(string, pStringDesc->wData, pStringDesc->bLength/2-1);

	kfree(pStringDesc);
	return strlen(string);
}

/*
 *  unicode_to_ascii
 *
 *	Turns a string from Unicode into ASCII.
 *	Doesn't do a good job with any characters that are outside the normal
 *	ASCII range, but it's only for debugging...
 *	NOTE: expects the unicode in LE format
 *
 */
static void mx_unicode_to_ascii (char *string, __le16 *unicode, int unicode_size)
{
	int i;

	if (unicode_size <= 0)
		return;

	for (i = 0; i < unicode_size; ++i)
		string[i] = (char)(le16_to_cpu(unicode[i]));
		
	string[unicode_size] = 0x00;
}

/*
 *  mx_init_queue - initial circular buffer
 *
 *  If success return Queue, otherwise return NULL.
 */
static struct mx_queue* mx_init_queue(int portno, unsigned int buflen)
{
    struct mx_queue *mx_q;
    
	if(buflen == 0)
		return NULL;

    mx_q = (struct mx_queue *)kmalloc(sizeof(struct mx_queue), GFP_KERNEL);
    if(mx_q == NULL)
		return NULL;

    mx_q->buffer = kmalloc(buflen, GFP_KERNEL);
    if(mx_q->buffer == NULL){
        kfree(mx_q);
		return NULL;
	}
        
    atomic_set(&mx_q->qfull, 0);
    atomic_set(&mx_q->add_sn, 0);
    mx_q->portno = portno;
    mx_q->size = buflen;
    mx_q->front = mx_q->rear = mx_q->buffer;

    return mx_q;            
}    

/*
 *  mx_free_queue - free circular buffer
 */
static void mx_free_queue(struct mx_queue *mx_q)
{
    if(mx_q != NULL){
        if(mx_q->buffer != NULL)
            kfree(mx_q->buffer);
        kfree(mx_q);    
    }
}

/*
 *  mx_clear_queue - clear circular buffer
 */
static void mx_clear_queue(struct mx_queue *mx_q)
{
    if(mx_q != NULL){
        mx_q->front = mx_q->rear;
        atomic_set(&mx_q->qfull, 0);   
    }        
}    

/*
 *  mx_data_in_queue - data in circular buffer
 *
 *  Return the number of bytes of data available in the circular
 *  buffer.
 */
static int mx_data_in_queue(struct mx_queue *mx_q)
{
    int len;
    
    if(mx_q == NULL){
		dbg("%s - queue is NULL", __FUNCTION__);
        return 0;
	}

    if((mx_q->front == mx_q->rear) && (atomic_read(&mx_q->qfull)))
        return (mx_q->size);
	

    len = mx_q->size - (unsigned long)mx_q->front + (unsigned long)mx_q->rear; 
    len %= mx_q->size;
   
	dbg("%s - data len = %d", __FUNCTION__, len);
        
    return len;
}

/*
 *  mx_space_in_queue - space in circular buffer
 *
 *  Return the number of bytes of space available in the circular
 *  buffer.
 */
static int mx_space_in_queue(struct mx_queue *mx_q)
{
    int len;
    
    if(mx_q == NULL){
		dbg("%s - queue is NULL", __FUNCTION__);
        return 0;
	}

    if((mx_q->rear == mx_q->front) && (!atomic_read(&mx_q->qfull)))
        return (mx_q->size);        

    len = mx_q->size - (unsigned long)mx_q->rear + (unsigned long)mx_q->front;
    len %= mx_q->size;

	dbg("%s - space len = %d", __FUNCTION__, len);

    return len;
}

/*
 *  mx_insert_queue - insert data into circular buffer
 *
 *  Copy data from a user buffer and put it into the circular buffer.
 *  Restrict to the amount of space available.
 *
 *  Return the number of bytes copied.
 */
static int mx_insert_queue(struct mx_queue *mx_q, const char *buf, int count)
{
    int free_sp;
    
    if(mx_q == NULL)
        return 0;
        
    free_sp = mx_space_in_queue(mx_q);    
    if(count > free_sp)
        count = free_sp;
        
    free_sp = mx_q->buffer + mx_q->size - mx_q->rear;
    if(count > free_sp){
        memcpy(mx_q->rear, buf, free_sp);
        memcpy(mx_q->buffer, buf + free_sp, count - free_sp);
        mx_q->rear = mx_q->buffer + count - free_sp;
    }else{
        memcpy(mx_q->rear, buf, count);
        if(count < free_sp)
            mx_q->rear += count;
        else
            mx_q->rear = mx_q->buffer;
    }
    
    if(mx_q->rear == mx_q->front)
        atomic_set(&mx_q->qfull, 1);
    
    return count;
}

/*
 *  mx_remove_tx_queue - remove transmit data from circular buffer
 *
 *  Get data from the circular buffer and copy to the given buffer.
 *  Restrict to the amount of data available.
 *
 *  Return the number of bytes removed.
 */
static int mx_remove_tx_queue(struct mx_queue *mx_q, unsigned char *buf, int count)
{
     int data_len;
        
     if(mx_q == NULL)
        return 0;

	 /*
	  *  Reserve four bytes for packing header
	  */
	 count -= HEADER_SIZE;
        
     data_len = mx_data_in_queue(mx_q);   
     if(count > data_len)
        count = data_len;
        
     data_len = mx_q->buffer + mx_q->size - mx_q->front;
     if(count > data_len){
        mx_pack_header(mx_q, buf, count);
        memcpy(buf + HEADER_SIZE, mx_q->front, data_len);
        memcpy(buf + HEADER_SIZE + data_len, mx_q->buffer, count - data_len);
        mx_q->front = mx_q->buffer + count - data_len;
     }else{
        mx_pack_header(mx_q, buf, count);
        memcpy(buf + HEADER_SIZE, mx_q->front, count);
        if(count < data_len)
            mx_q->front += count;
        else
            mx_q->front = mx_q->buffer;
     }
     
    if(mx_q->front == mx_q->rear)
        atomic_set(&mx_q->qfull, 0);

     return (count + HEADER_SIZE);
}    

/*
 *  mx_remove_rx_queue - remove receive data from circular buffer
 *
 *  Get data from the circular buffer and copy to the given buffer.
 *  Restrict to the amount of data available.
 *
 *  Return the number of bytes removed.
 */
static int mx_remove_rx_queue(struct mx_queue *mx_q, unsigned char *buf, int count)
{
     int data_len;
     
        
     if(mx_q == NULL)
        return 0;
        
     data_len = mx_data_in_queue(mx_q);   
     if(count > data_len)
        count = data_len;
        
     data_len = mx_q->buffer + mx_q->size - mx_q->front;
     if(count > data_len){
        memcpy(buf, mx_q->front, data_len);
        memcpy(buf + data_len, mx_q->buffer, count - data_len);
        mx_q->front = mx_q->buffer + count - data_len;
     }else{
        memcpy(buf, mx_q->front, count);
        if(count < data_len)
            mx_q->front += count;
        else
            mx_q->front = mx_q->buffer;
     }
     
    if(mx_q->front == mx_q->rear)
        atomic_set(&mx_q->qfull, 0);
	
   
     return count;
}

/*
 *  mx_pack_header - pack a header into tx data
 */
static void mx_pack_header(struct mx_queue *mx_q, unsigned char *buf, int count)
{
	buf[0] = (char)((mx_q->portno >> 8) & 0xFF);

    if(atomic_read(&mx_q->add_sn)){
        buf[0] |= 0x80;
    }

    buf[1] = (char)(mx_q->portno & 0xFF);
	buf[2] = (char)((count >> 8) & 0xFF);
	buf[3] = (char)(count & 0xFF); 		  

    if(atomic_read(&mx_q->add_sn)){
        atomic_set(&mx_q->add_sn, 0);
    }

    
}    

#if DBG_BURNIN_DATA 
static void mx_dbg_burnin_data(struct mxuport_port *mxport, unsigned char *buf, int count)
{
	int i, j;
	unsigned char ptn;
	
	if(mxport->txerr)
		return;

	for(i=0; i<count; i++){
		ptn = (unsigned char)(mxport->txcnt & 0xff);	
		if((ptn == 0x11) || (ptn == 0x13))
			ptn |= 0x40;

		if((buf[i] & 0xff) != ptn){
			mxport->txerr = 1;
			printk("\n[%d] buf[%d] = 0x%x, ptn = 0x%x\n", mxport->portno, i, buf[i], ptn);
			for(j=0; j<count; j++)
				printk("0x%x ", buf[j]);
		}
	
		if(mxport->txerr)
			break;

		mxport->txcnt++;	
	}
}
#endif
    
static int __init mxuport_init(void)
{
	int result;

	/*
	 *  register this driver to the USB-to-serial core 
	 */
	result = mx_usbserial_register(&mxuport_2p_device);
	if(result)
		goto failed_uport_2p_device_register;

	result = mx_usbserial_register(&mxuport_4p_device);
	if(result)
		goto failed_uport_4p_device_register;

	result = mx_usbserial_register(&mxuport_8p_device);
	if(result)
		goto failed_uport_8p_device_register;

	result = mx_usbserial_register(&mxuport_16p_device);
	if(result)
		goto failed_uport_16p_device_register;

	/*
	 *  register this driver to the USB core 
	 */
	result = usb_register(&mxuport_driver);	
	if(result){
		dbg("Register UPort driver failed. (Error number %d)", result);
		goto failed_usb_register;
	}
		
	printk(KERN_INFO DRIVER_DESC " " DRIVER_VERSION);
	return 0;
	
failed_usb_register:
	mx_usbserial_deregister(&mxuport_16p_device);
failed_uport_16p_device_register:
	mx_usbserial_deregister(&mxuport_8p_device);
failed_uport_8p_device_register:
	mx_usbserial_deregister(&mxuport_4p_device);
failed_uport_4p_device_register:
	mx_usbserial_deregister(&mxuport_2p_device);
failed_uport_2p_device_register:    
	return result;
	
}

static void __exit mxuport_exit(void)
{
	usb_deregister(&mxuport_driver);
	mx_usbserial_deregister(&mxuport_2p_device);
	mx_usbserial_deregister(&mxuport_4p_device);
	mx_usbserial_deregister(&mxuport_8p_device);
	mx_usbserial_deregister(&mxuport_16p_device);
}

module_init(mxuport_init);
module_exit(mxuport_exit);

/*
 *  Module Information
 */
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE(DRIVER_LICENSE);

#if(LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8))
module_param(debug, bool, S_IRUGO | S_IWUSR);
#else
MODULE_PARM(debug, "i");
#endif

MODULE_PARM_DESC(debug, "Enable debugging, 0=OFF, 1=ON, Default is OFF");
