/** 
 * This is a very minimal tool for looking what goes wrong
 * Author: philippe.steinmann@gmail.com
 * Copyright: None...
 * ! NO WARANTY, USE AT YOUR OWN RISK !
 * 
 * Date: 2011-11-23
 * 
 * Hardware: MOXA UPort 1250
 * Driver: v1.1   , package: driv_linux2.6_uport_v1.1_build_09091814.tgz
 *
 * Kernel: Linux 3.0.0-13-generic #22-Ubuntu SMP Wed Nov 2 13:25:36 UTC 2011 i686 athlon i386 GNU/Linux
 * 
 * Compilation: gcc -Wall -o essais_drv_moxa essais_drv_moxa.c
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>

int main(void)
{
  int fd;
  struct termios term;
  
  printf("Minimal tool to check what works\n");
  
  /* Open port */
  fd = open("/dev/ttyMXUSB0", O_RDWR | O_NOCTTY | O_NONBLOCK );
  if(fd < 0){
    perror("open");
    return 1;
  }
  printf("Open port OK\n");
  
  if(tcgetattr(fd, &term) < 0){
    perror("tcgetattr");
    close(fd);
    return 1;
  }
  printf("tcgetattr() OK\n");
  
  // Try to modify speed
  if(cfsetispeed(&term, B4800) < 0){
    perror("cfsetispeed");
    close(fd);
    return 1;
  }
  if(cfsetospeed(&term, B4800) < 0){
    perror("cfsetospeed");
    close(fd);
    return 1;
  }
  
  // Try to apply setup
  if(tcsetattr(fd, TCSANOW, &term) < 0){
    perror("tcsetattr");
    close(fd);
    return 1;
  }
  printf("Apply setup (with tcsetattr() ) OK\n");
  
  // Try ioctl()
  printf("Try ioctl() call with TIOCGSERIAL (Num: 0x%x)\n", TIOCGSERIAL);
  if(ioctl(fd, TIOCGSERIAL, &term) < 0){
    perror("TIOCGSERIAL");
    close(fd);
    return 1;
  }
  printf("ioctl() with TIOCGSERIAL Ok\n");

  // Finish
  close(fd);
  
  return 0;
}
