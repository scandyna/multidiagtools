================================================================================

                 MOXA UPort Series Driver Installation Guide
	    	          for Linux Kernel 2.6.x

	         Copyright(C)2008 Moxa Inc.

================================================================================
Date: 09/25/2008

CONTENTS

1. Introduction
2. System Requirements
3. Installation
   3.1 Driver files   
   3.2 Device naming convention
   3.3 Module driver configuration   
   3.4 Verify driver installation
4. Setserial
5. Troubleshooting

-----------------------------------------------------------------------------
1. Introduction

   The MOXA UPort series driver supports following devices. 

   [2 Port]
    - UPort 1250 : 2 Port RS-232/422/485 USB to Serial Hub
    - UPort 1250I : 2 Port RS-232/422/485 USB to Serial Hub with Isolation

   [4 Port]    
    - UPort 1410 : 4 Port RS-232 USB to Serial Hub
    - UPort 1450 : 4 Port RS-232/422/485 USB to Serial Hub
    - UPort 1450I : 4 Port RS-232/422/485 USB to Serial Hub with Isolation
    
   [8 Port]    
    - UPort 1610-8 : 8 Port RS-232 USB to Serial Hub
    - UPort 1650-8 : 8 Port RS-232/422/485 USB to Serial Hub
    
   [16 Port]    
    - UPort 1610-16 : 16 Port RS-232 USB to Serial Hub
    - UPort 1650-16 : 16 Port RS-232/422/485 USB to Serial Hub

   This driver supports x86 and x64(AMD64/EM64T) hardware platform. In 
   order to maintain compatibility, this version has also been properly 
   tested with several Linux distribution (see version.txt). However, 
   if compatibility problem occurs, please contact Moxa Technologies 
   technical support. (support@moxa.com.tw)

   All the drivers are published in form of source code under GNU General 
   Public License in this version. Please refer to GNU General Public 
   License announcement in each source code file for more detail.

   This version of driver can be only installed as Loadable Module (Module 
   driver). Before you install the driver, please refer to installation 
   procedure in the User's Manual.

-----------------------------------------------------------------------------
2. System Requirements

   - Hardware Platform: x86, x64 
   - Kernel Version: 2.6.x
   - GCC version 3.x 
   - Kernel source

-----------------------------------------------------------------------------
3. Installation

   3.1 Driver files   
   3.2 Device naming convention
   3.3 Module driver configuration   
   3.4 Verify driver installation
       
   3.1 Driver files

       The driver file may be obtained from CD-ROM or floppy disk. The first 
       step is to copy driver file "driv_linux2.6_uport_v1.0.1_build_xxx.tgz" 
       into specified directory. e.g. /moxa. The execute commands as below.

       # cd / 
       # mkdir moxa 		
       # cd /moxa
       # cp /dev/fd0/driv_linux2.6_uport_v1.0.1_build_xxx.tgz . 
       # tar xvfz driv_linux2.6_uport_v1.0.1_build_xxx.tgz  
       
       or
       
       # cd /
       # mkdir moxa
       # cd /moxa
       # cp /mnt/cdrom/<driver dir>/driv_linux2.6_uport_v1.0.1_build_xxx.tgz      
       # tar xvfz mxuport.tgz

   3.2 Device naming convention
   
       You may find all the driver files in /moxa/v1.0.1/source.
       Following installation procedure depends on the model you'd like to
       run the driver.

       Dialin and callout port
       -----------------------
       This driver remains traditional serial device properties. Because the
       limitation of the USB build-in module "usbcore", There is only one 
       device name for each serial port. This one is dial-in port which is 
       named "ttyMXUSBxx". 

   3.3 Module driver configuration

       3.3.1 Build the MOXA driver
         
          Before using the MOXA driver, you need compile all the source 
          code. This step is only need to be executed once. But you need to 
          re-compile the source code when you modify the source code. 

       	  Find "Makefile" in /moxa/mxuport, then run	
             
             # make install   
           
          The driver files "mxuport.ko" and "mxusbserial.ko" will be properly
	  compiled and copied to system directories respectively. 

	  Or you can use "mxinstall" in /moxa/mxuport to install Moxa
          software. "mxinstall" is a script that make it easy to install
          driver and then load the driver in one single step.
             
       3.3.2 Load the MOXA driver  
         
          Insert the MOXA driver as following:

	      # modprobe mxuport 

	  It will active the MOXA UPort driver.
	  
       3.3.3 Unload the MOXA driver
        
          Unload the MOXA driver as following:
            
              #rmmod mxuport
              
              #rmmod mxusbserial	  
	
       3.3.4 Uninstall the MOXA driver  
         
          Find "Makefile" in /moxa/mxuport, then run

	      # make remove 

	  It will uninstall the MOXA UPort driver.  
	      
   3.4 Static driver configuration
       
       Note: To use static driver, you must install the linux kernel
             source package.
   
       3.4.1 Copy  file
	  # cd /usr/src/<kernel-source directory>/drivers/usb/serial/
	  # cp /moxa/mxuport/mxuport/mx-uport.* .
	  # cp /moxa/mxuport/mxuport/UPort*.* .
	  # cp /moxa/mxuport/mxusbserial/mxbus.c .
	  # cp /moxa/mxuport/mxusbserial/mxusb-serial.* .
	  
       3.4.2 Modify kernel configuration file.
          Add the following line into configuration file.
	  /usr/src/<kernel-source directory>/drivers/usb/serial/Kconfig
	      ...
	      config USB_SERIAL_CONSOLE
	      ...
	      config USB_SERIAL_GENERIC
	      ...
	      config MOXA_UPORT_SERIAL	              <-- Add the lines.
			tristate "Moxa USB Serial "   <-- 
	      config MOXA_UPORT		              <-- 
			tristate "Moxa UPort "        <-- 
			depends on MOXA_UPORT_SERIAL  <--
		...

       3.4.3 Modify the kernel Makefile 
	  Add the following line to the last line of Makefile.
	  /usr/src/<kernel-source directory>/drviers/usb/serial/Makefile
	     ...
	     ...
	     ...
	     obj-$(CONFIG_MOXA_UPORT_SERIAL) += mxusbserial.o <-- Add the line.
	     obj-$(CONFIG_MOXA_UPORT) += mxuport.o            <--         
	     mxusbserial-objs :=mxusb-serial.o mxbus.o        <-- 
	     mxuport-objs :=mx-uport.o                        <-- 

       3.4.4 Setup kernel configuration
          
          Configure the kernel:

            # cd /usr/src/<kernel-source directory>
            # make menuconfig
            
          You will go into a menu-driven system. Please select [Device Drivers]
	  [USB Support], [USB Serial Converter support], enable the 
	  [USB Serial Converter support],the [ MOXA USB SERIAL ] and
	  [ MOXA UPORT ]drivers with "[*]" by pressing space bar for
	  built-in (not "[M]"),then select [Exit] to exit this program and 
	  save kernel configurations. 
          
       3.4.5 Rebuild kernel
      	  The following are for Linux kernel rebuilding, for your 
          reference only.
	  For appropriate details, please refer to the Linux document.

	  a. cd /usr/src/<kernel-source directory>
	  b. make 	     
	  c. make modules	
	  d. make modules_install
	  e. make install

   3.5 Verify driver installation
   
       You may refer to /var/log/messages to check the latest status
       log reported by this driver whenever it's activated or type command
       "dmesg" to get driver informations.
       
-----------------------------------------------------------------------------
4. Setserial

   Supported Setserial parameters are listed as below.

   port        use for change port interface
               0x3 RS-485 4W
               0x2 RS-422
               0x1 RS-485 2W
               0x0 RS-232               
		
   spd_hi      Use 57.6kb  when the application requests 38.4kb.
   spd_vhi     Use 115.2kb when the application requests 38.4kb.
   spd_shi     Use 230kb   when the application requests 38.4kb.
   spd_warp    Use 460kb   when the application requests 38.4kb.
   spd_normal  Use 38.4kb  when the application requests 38.4kb.

   [Example]	

    i) The following command sets interface of "/dev/ttyMXUSB0" to RS-422.	

       # setserial /dev/ttyMXUSB0 port 0x2
       
    ii) Lookup the serial settings of current port.
    
       # setserial -g /dev/ttyMXUSB0 
         	   
-----------------------------------------------------------------------------
5. Troubleshooting 

   1. Compile error
      To build Moxa driver, it needs kernel header files. If you got
      some compile error, please run "rpm -qa | grep kernel" to check 
      whether the kernel-source package is installed properly. If not,
      please get the kernel-source irpm package and run "rpm -ivh <pacakge>"
      to install. You also need to make sure the build tool is ready,
      including make/gcc/lib. Please see Linux relative document to 
      get more information.
   
   2. No such file or directory
      This problem is that device node not make automatically during
      loading driver. It usually happens under previous kernel version such
      as "2.6.0" - "2.6.5". Therefore, just execute the make node script 
      "upmknod" to generate device node.
      
-----------------------------------------------------------------------------

