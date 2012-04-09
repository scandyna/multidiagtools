/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "mdtSerialPortPosix.h"
#include "mdtError.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <errno.h> 
#include <cstring>
#include <QString>

#include <QDebug>

mdtSerialPortPosix::mdtSerialPortPosix(QObject *parent)
 : mdtAbstractSerialPort(parent)
{
  pvPreviousCarState = 0;
  pvPreviousDsrState = 0;
  pvPreviousCtsState = 0;
  pvPreviousRngState = 0;
  pvFd = -1;

  // Control signal thread kill utils
  pvCtlThread = 0;
  pvAbortingWaitEventCtl = false;
  // We must catch the SIGALRM signal, else the application process
  // will be killed by pthread_kill()
  sigemptyset(&(pvSigaction.sa_mask));
  pvSigaction.sa_flags = 0;
  pvSigaction.sa_handler = sigactionHandle;
  sigaction(SIGALRM, &pvSigaction, NULL);
}

mdtSerialPortPosix::~mdtSerialPortPosix()
{
  close();
}

bool mdtSerialPortPosix::setAttributes(const QString &portName)
{
  struct serial_struct serinfo;

  // Close previous opened port
  this->close();
  // Clear previous attributes
  pvAvailableBaudRates.clear();
  // Try to open port
  pvFd = ::open(portName.toStdString().c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if(pvFd < 0){
    mdtError e(MDT_UNDEFINED_ERROR, "can not open port '" + portName + "'", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    pvName = "";
    return false;
  }
  // Set port name
  pvName = portName;
  // Set the UART type
  if(ioctl(pvFd, TIOCGSERIAL, &serinfo) < 0){
    pvUartType = UT_UNKNOW;
    ::close(pvFd);
    return false;
  }
  // Map the type
  switch(serinfo.type){
    case PORT_8250:
      pvUartType = UT_8250;
    case PORT_16450:
      pvUartType = UT_16450;
      break;
    case PORT_16550:
      pvUartType = UT_16550;
      break;
    case PORT_16550A:
      pvUartType = UT_16550A;
      break;
    case PORT_CIRRUS:
      pvUartType = UT_CIRRUS;
      break;
    case PORT_16650:
      pvUartType = UT_16650;
      break;
    case PORT_16650V2:
      pvUartType = UT_16650V2;
      break;
    case PORT_16750:
      pvUartType = UT_16750;
      break;
    case PORT_STARTECH:
      pvUartType = UT_STARTECH;
      break;
    case PORT_16C950:
      pvUartType = UT_16C950;
      break;
    case PORT_16654:
      pvUartType = UT_16654;
      break;
    case PORT_16850:
      pvUartType = UT_16850;
      break;
    case PORT_RSA:
      pvUartType = UT_RSA;
      break;
    default:
      pvUartType = UT_UNKNOW;
      ::close(pvFd);
      return false;
  }
  // Get available baud rates list
  pvAvailableBaudRates << 50;
  pvAvailableBaudRates << 75;
  pvAvailableBaudRates << 110;
  pvAvailableBaudRates << 134; // Note: is 134.5
  pvAvailableBaudRates << 150;
  pvAvailableBaudRates << 200;
  pvAvailableBaudRates << 300;
  pvAvailableBaudRates << 600;
  pvAvailableBaudRates << 1200;
  pvAvailableBaudRates << 1800;
  pvAvailableBaudRates << 2400;
  pvAvailableBaudRates << 4800;
  pvAvailableBaudRates << 9600;
  pvAvailableBaudRates << 19200;
  pvAvailableBaudRates << 38400;
#ifdef  B57600
  pvAvailableBaudRates << 57600;
#endif
#ifdef B76800
  pvAvailableBaudRates << 76800;
#endif
#ifdef  B115200
  pvAvailableBaudRates << 115200;
#endif
#ifdef  B230400
  pvAvailableBaudRates << 230400;
#endif
#ifdef  B460800
  pvAvailableBaudRates << 460800;
#endif
#ifdef  B500000
  pvAvailableBaudRates << 500000;
#endif
#ifdef  B576000
  pvAvailableBaudRates << 576000;
#endif
#ifdef  B921600
  pvAvailableBaudRates << 921600;
#endif
#ifdef  B1000000
  pvAvailableBaudRates << 1000000;
#endif
#ifdef  B1152000
  pvAvailableBaudRates << 1152000;
#endif
#ifdef  B1500000
  pvAvailableBaudRates << 1500000;
#endif
#ifdef  B2000000
  pvAvailableBaudRates << 2000000;
#endif
#ifdef  B2500000
  pvAvailableBaudRates << 2500000;
#endif
#ifdef  B3000000
  pvAvailableBaudRates << 3000000;
#endif
#ifdef  B3500000
  pvAvailableBaudRates << 3500000;
#endif
#ifdef  B4000000
  pvAvailableBaudRates << 4000000;
#endif

  // End of attributes, close port
  ::close(pvFd);
  return true;
}

bool mdtSerialPortPosix::open(mdtSerialPortConfig &cfg)
{
  QString strNum;

  // Close previous opened device
  this->close();
  // Try to open port
  //  O_RDWR : Read/write access
  //  O_NOCTTY: not a terminal
  //  O_NDELAY: ignore DCD signal
  lockMutex();
  pvFd = ::open(pvName.toStdString().c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK );
  if(pvFd < 0){
    mdtError e(MDT_UNDEFINED_ERROR, "Unable to open port: " + pvName, mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    unlockMutex();
    return false;
  }
  // Get current config and save it to pvOriginalTermios
  if(tcgetattr(pvFd, &pvOriginalTermios) < 0){
    ::close(pvFd);
    pvFd = -1;
    mdtError e(MDT_UNDEFINED_ERROR, "tcgetattr() failed, " + pvName + " is not a serial port, or is not available", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    unlockMutex();
    return false;
  }
  // Get current system config on "work copy"
  if(tcgetattr(pvFd, &pvTermios) < 0){
    ::close(pvFd);
    pvFd = -1;
    mdtError e(MDT_UNDEFINED_ERROR, "tcgetattr() failed, " + pvName + " is not a serial port, or is not available", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    unlockMutex();
    return false;
  }
  // Set baud rate
  if(!setBaudRate(cfg.baudRate())){
    ::close(pvFd);
    pvFd = -1;
    strNum.setNum(cfg.baudRate());
    mdtError e(MDT_UNDEFINED_ERROR, "unsupported baud rate '" + strNum + "' for port " + pvName, mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    unlockMutex();
    return false;
  }
  // Set local mode and enable the receiver
  pvTermios.c_cflag |= (CLOCAL | CREAD);
  // Set data bits
  if(!setDataBits(cfg.dataBitsCount())){
    ::close(pvFd);
    pvFd = -1;
    strNum.setNum(cfg.dataBitsCount());
    mdtError e(MDT_UNDEFINED_ERROR, "unsupported data bits count '" + strNum + "' for port " + pvName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    unlockMutex();
    return false;
  }
  // Set stop bits
  if(!setStopBits(cfg.stopBitsCount())){
    ::close(pvFd);
    pvFd = -1;
    strNum.setNum(cfg.stopBitsCount());
    mdtError e(MDT_UNDEFINED_ERROR, "unsupported stop bits count '" + strNum + "' for port " + pvName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    unlockMutex();
    return false;
  }
  // Set parity
  setParity(cfg.parity());
  // Set flow control
  setFlowCtlRtsCts(cfg.flowCtlRtsCtsEnabled());
  setFlowCtlXonXoff(cfg.flowCtlXonXoffEnabled(), cfg.xonChar(), cfg.xoffChar());
  // Set raw data mode
  pvTermios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  pvTermios.c_oflag &= ~OPOST;
  // Apply the setup
  if(tcsetattr(pvFd, TCSANOW, &pvTermios) < 0){
    ::close(pvFd);
    pvFd = -1;
    mdtError e(MDT_UNDEFINED_ERROR, "unable to apply configuration for port " + pvName, mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    unlockMutex();
    return false;
  }
  // Check if configuration could really be set
  if(!checkConfig(cfg)){
    ::close(pvFd);
    pvFd = -1;
    unlockMutex();
    return false;
  }

  // Set the read/write timeouts
  setReadTimeout(cfg.readTimeout());
  setWriteTimeout(cfg.writeTimeout());

  return mdtAbstractPort::open(cfg);
}

void mdtSerialPortPosix::close()
{
  if(!isOpen()){
    return;
  }
  lockMutex();
  if(pvFd >= 0){
    tcsetattr(pvFd, TCSANOW, &pvOriginalTermios); 
    ::close(pvFd);
    pvFd = -1;
  }
  mdtAbstractPort::close();
}

void mdtSerialPortPosix::setReadTimeout(int timeout)
{
  pvReadTimeout.tv_sec = timeout/1000;
  pvReadTimeout.tv_usec = 1000*(timeout%1000);
}

void mdtSerialPortPosix::setWriteTimeout(int timeout)
{
  pvWriteTimeout.tv_sec = timeout/1000;
  pvWriteTimeout.tv_usec = 1000*(timeout%1000);
}

bool mdtSerialPortPosix::waitForReadyRead()
{
  fd_set input;
  int n;
  struct timeval tv = pvReadTimeout;  // Create a copy, because select() alter the timeout

  // Init the select call
  FD_ZERO(&input);
  FD_SET(pvFd, &input);

  pvMutex.unlock();
  n = select(pvFd+1, &input, 0, 0, &tv);
  pvMutex.lock();
  if(n == 0){
    updateReadTimeoutState(true);
  }else{
    updateReadTimeoutState(false);
    if(n < 0){
      mdtError e(MDT_UNDEFINED_ERROR, "select() call failed", mdtError::Error);
      e.setSystemError(errno, strerror(errno));
      MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
      e.commit();
      return false;
    }
  }

  return true;
}

qint64 mdtSerialPortPosix::read(char *data, qint64 maxSize)
{
  int n;
  int err;

  n = ::read(pvFd, data, maxSize);
  if(n < 0){
    // Store errno
    err = errno;
    switch(err){
      case EAGAIN:      // No data available
        return 0;
      case ETIMEDOUT:   // Read timeout (happens with USBTMC)
        updateReadTimeoutState(true);
        return 0;
      default:
        mdtError e(MDT_PORT_IO_ERROR, "read() call failed", mdtError::Error);
        e.setSystemError(err, strerror(err));
        MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
        e.commit();
        return n;
    }
  }

  return n;
}

bool mdtSerialPortPosix::suspendTransmission()
{
  int err;

  if(flowCtlRtsCtsOn()){
    if(!setRtsOff()){
      return false;
    }
  }
  if(flowCtlXonXoffOn()){
    if(tcflow(pvFd, TCIOFF) < 0){
      err = errno;
      mdtError e(MDT_PORT_IO_ERROR, "tcflow() call failed", mdtError::Error);
      e.setSystemError(err, strerror(err));
      MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
      e.commit();
      return false;
    }
  }

  return true;
}

bool mdtSerialPortPosix::resumeTransmission()
{
  int err;

  if(flowCtlRtsCtsOn()){
    if(!setRtsOn()){
      return false;
    }
  }
  if(flowCtlXonXoffOn()){
    if(tcflow(pvFd, TCION) < 0){
      err = errno;
      mdtError e(MDT_PORT_IO_ERROR, "tcflow() call failed", mdtError::Error);
      e.setSystemError(err, strerror(err));
      MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
      e.commit();
      return false;
    }
  }

  return true;
}

void mdtSerialPortPosix::flushIn()
{
  int err;

  lockMutex();
  if(tcflush(pvFd, TCIFLUSH) < 0){
    err = errno;
    mdtError e(MDT_PORT_IO_ERROR, "tcflush() call failed", mdtError::Error);
    e.setSystemError(err, strerror(err));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
  }
  mdtAbstractPort::flushIn();
}

bool mdtSerialPortPosix::waitEventWriteReady()
{
  fd_set output;
  int n;
  struct timeval tv = pvWriteTimeout;  // Create a copy, because select() alter the timeout

  // Init the select call
  FD_ZERO(&output);
  FD_SET(pvFd, &output);

  pvMutex.unlock();
  n = select(pvFd+1, 0, &output, 0, &tv);
  pvMutex.lock();
  if(n == 0){
    updateWriteTimeoutState(true);
  }else{
    updateWriteTimeoutState(false);
    if(n < 0){
      mdtError e(MDT_UNDEFINED_ERROR, "select() call failed", mdtError::Error);
      e.setSystemError(errno, strerror(errno));
      MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
      e.commit();
      return false;
    }
  }

  return true;
}

qint64 mdtSerialPortPosix::write(const char *data, qint64 maxSize)
{
  int n;
  int err;

  n = ::write(pvFd, data, maxSize);
  if(n < 0){
    // Store error (errno will be reset when readen)
    err = errno;
    switch(err){
      case EAGAIN:    // Can't write now
        return 0;
      default:
        mdtError e(MDT_PORT_IO_ERROR, "write() call failed", mdtError::Error);
        e.setSystemError(err, strerror(err));
        MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
        e.commit();
        return n;
    }
  }

  return n;
}

void mdtSerialPortPosix::flushOut()
{
  int err;

  lockMutex();
  if(tcflush(pvFd, TCOFLUSH) < 0){
    err = errno;
    mdtError e(MDT_PORT_IO_ERROR, "tcflush() call failed", mdtError::Error);
    e.setSystemError(err, strerror(err));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
  }
  mdtAbstractPort::flushOut();
}

bool mdtSerialPortPosix::waitEventCtl()
{
  int retVal;

  // We wait until a line status change happens
  pvMutex.unlock();
  retVal = ioctl(pvFd, TIOCMIWAIT, (TIOCM_CAR | TIOCM_DSR | TIOCM_CTS | TIOCM_RNG));
  pvMutex.lock();
  if(retVal < 0){
    if(errno == EINTR){
      // Probably sent by pthread_kill() to stop the thread
      if(pvAbortingWaitEventCtl){
        pvAbortingWaitEventCtl = false;
        return true;
      }
      // Unhandled case that abort the thread
      mdtError e(MDT_UNDEFINED_ERROR, "ioctl() call failed with command TIOCMIWAIT", mdtError::Info);
      e.setSystemError(errno, strerror(errno));
      MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
      e.commit();
      return false;
    }
    // Else, we have a unhandled error
    mdtError e(MDT_UNDEFINED_ERROR, "ioctl() call failed with command TIOCMIWAIT", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }

  return true;
}

bool mdtSerialPortPosix::getCtlStates()
{
  int states;

  if(ioctl(pvFd, TIOCMGET, &states) < 0){
    mdtError e(MDT_UNDEFINED_ERROR, "ioctl() call failed with command TIOCMGET", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }

  // See witch signals changed
  if((states & TIOCM_CAR) != pvPreviousCarState){
    // Store new state
    pvPreviousCarState = (states & TIOCM_CAR);
    // Determine current state and emit signal
    if(states & TIOCM_CAR){
      pvCarIsOn = true;
    }else{
      pvCarIsOn = false;
    }
    emit carChanged(pvCarIsOn);
  }
  if((states & TIOCM_DSR) != pvPreviousDsrState){
    // Store new state
    pvPreviousDsrState = (states & TIOCM_DSR);
    // Determine current state and emit signal
    if(states & TIOCM_DSR){
      pvDsrIsOn = true;
    }else{
      pvDsrIsOn = false;
    }
    emit dsrChanged(pvDsrIsOn);
  }
  if((states & TIOCM_CTS) != pvPreviousCtsState){
    // Store new state
    pvPreviousCtsState = (states & TIOCM_CTS);
    // Determine current state and emit signal
    if(states & TIOCM_CTS){
      pvCtsIsOn = true;
    }else{
      pvCtsIsOn = false;
    }
    emit ctsChanged(pvCtsIsOn);
  }
  if((states & TIOCM_RNG) != pvPreviousRngState){
    // Store new state
    pvPreviousRngState = (states & TIOCM_RNG);
    // Determine current state and emit signal
    if(states & TIOCM_RNG){
      pvRngIsOn = true;
    }else{
      pvRngIsOn = false;
    }
    emit rngChanged(pvRngIsOn);
  }

  return true;
}

void mdtSerialPortPosix::setRts(bool on)
{
  if(!isOpen()){
    return;
  }
  pvMutex.lock();
  if(on){
    setRtsOn();
  }else{
    setRtsOff();
  }
  pvMutex.unlock();
}

void mdtSerialPortPosix::setDtr(bool on)
{
  int states;

  if(!isOpen()){
    return;
  }
  pvMutex.lock();
  // Get the current ctl states
  if(ioctl(pvFd, TIOCMGET, &states) < 0){
    mdtError e(MDT_UNDEFINED_ERROR, "ioctl() call failed with command TIOCMGET", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    pvMutex.unlock();
    return;
  }
  // Set DTR state
  if(on){
    states |= TIOCM_DTR;
  }else{
    states &= ~TIOCM_DTR;
  }
  // Commit to system
  if(ioctl(pvFd, TIOCMSET, &states) < 0){
    mdtError e(MDT_UNDEFINED_ERROR, "ioctl() call failed with command TIOCMSET", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
  }
  pvMutex.unlock();
}

void mdtSerialPortPosix::defineCtlThread(pthread_t ctlThread)
{
  pvCtlThread = ctlThread;
}

void mdtSerialPortPosix::abortWaitEventCtl()
{
  Q_ASSERT(pvCtlThread != 0);

  // Set aborting flag and send the signal
  pvAbortingWaitEventCtl = true;
  pthread_kill(pvCtlThread, SIGALRM);
}

bool mdtSerialPortPosix::setBaudRate(int rate)
{
  speed_t brMask = 0;

  switch(rate){
    case 50:
      brMask = B50;
      break;
    case 75:
      brMask = B75;
      break;
    case 110:
      brMask = B110;
      break;
    case 134:   // Is 134.5 baud
      brMask = B134;
      break;
    case 150:
      brMask = B150;
      break;
    case 200:
      brMask = B200;
      break;
    case 300:
      brMask = B300;
      break;
    case 600:
      brMask = B600;
      break;
    case 1200:
      brMask = B1200;
      break;
    case 1800:
      brMask = B1800;
      break;
    case 2400:
      brMask = B2400;
      break;
    case 4800:
      brMask = B4800;
      break;
    case 9600:
      brMask = B9600;
      break;
#ifdef B19200
    case 19200:
      brMask = B19200;
      break;
#endif
    case 38400:
      brMask = B38400;
      break;
#ifdef  B57600
    case 57600:
      brMask = B57600;
      break;
#endif
#ifdef B76800
    case 76800:
      brMask = B76800;
      break;
#endif
#ifdef  B115200
    case 115200:
      brMask = B115200;
      break;
#endif
#ifdef  B230400
    case 230400:
      brMask = B230400;
      break;
#endif
#ifdef  B460800
    case 460800:
      brMask = B460800;
      break;
#endif
#ifdef  B500000
    case 500000:
      brMask = B500000;
      break;
#endif
#ifdef  B576000
    case 576000:
      brMask = B576000;
      break;
#endif
#ifdef  B921600
    case 921600:
      brMask = B921600;
      break;
#endif
#ifdef  B1000000
    case 1000000:
      brMask = B1000000;
      break;
#endif
#ifdef  B1152000
    case 1152000:
      brMask = B1152000;
      break;
#endif
#ifdef  B1500000
    case 1500000:
      brMask = B1500000;
      break;
#endif
#ifdef  B2000000
    case 2000000:
      brMask = B2000000;
      break;
#endif
#ifdef  B2500000
    case 2500000:
      brMask = B2500000;
      break;
#endif
#ifdef  B3000000
    case 3000000:
      brMask = B3000000;
      break;
#endif
#ifdef  B3500000
    case 3500000:
      brMask = B3500000;
      break;
#endif
#ifdef  B4000000
    case 4000000:
      brMask = B4000000;
      break;
#endif
    default:
      return false;
  }
  // Apply baud rate to the termios configuration structure
  if(cfsetispeed(&pvTermios, brMask) < 0){
    return false;
  }
  if(cfsetospeed(&pvTermios, brMask) < 0){
    return false;
  }

  return true;
}

int mdtSerialPortPosix::baudRate()
{
  speed_t brMask = 0;
  int rate;

  // Get configured baud rate
  // Note, we allways configure the same rate for input
  // or output, so we choose here to get input configured rate
  brMask = cfgetispeed(&pvTermios);
  // Get numeric value
  switch(brMask){
    case B50:
      rate = 50;
      break;
    case B75:
      rate = 75;
      break;
    case B110:
      rate = 110;
      break;
    case B134:   // Is 134.5 baud
      rate = 134;
      break;
    case B150:
      rate = 150;
      break;
    case B200:
      rate = 200;
      break;
    case B300:
      rate = 300;
      break;
    case B600:
      rate = 600;
      break;
    case B1200:
      rate = 1200;
      break;
    case B1800:
      rate = 1800;
      break;
    case B2400:
      rate = 2400;
      break;
    case B4800:
      rate = 4800;
      break;
    case B9600:
      rate = 9600;
      break;
#ifdef B19200
    case B19200:
      rate = 19200;
      break;
#endif
    case B38400:
      rate = 38400;
      break;
#ifdef  B57600
    case B57600:
      rate = 57600;
      break;
#endif
#ifdef B76800
    case B76800:
      rate = B76800;
      break;
#endif
#ifdef  B115200
    case B115200:
      rate = 115200;
      break;
#endif
#ifdef  B230400
    case B230400:
      rate = 230400;
      break;
#endif
#ifdef  B460800
    case B460800:
      rate = 460800;
      break;
#endif
#ifdef  B500000
    case B500000:
      rate = 500000;
      break;
#endif
#ifdef  B576000
    case B576000:
      rate = 576000;
      break;
#endif
#ifdef  B921600
    case B921600:
      rate = 921600;
      break;
#endif
#ifdef  B1000000
    case B1000000:
      rate = 1000000;
      break;
#endif
#ifdef  B1152000
    case B1152000:
      rate = 1152000;
      break;
#endif
#ifdef  B1500000
    case B1500000:
      rate = 1500000;
      break;
#endif
#ifdef  B2000000
    case B2000000:
      rate = 2000000;
      break;
#endif
#ifdef  B2500000
    case B2500000:
      rate = 2500000;
      break;
#endif
#ifdef  B3000000
    case B3000000:
      rate = 3000000;
      break;
#endif
#ifdef  B3500000
    case B3500000:
      rate = 3500000;
      break;
#endif
#ifdef  B4000000
    case B4000000:
      rate = 4000000;
      break;
#endif
    default:
      rate = 0;
  }

  return rate;
}

bool mdtSerialPortPosix::setDataBits(int n)
{
  tcflag_t nbMask = 0;

  switch(n){
    case 5:
      nbMask = CS5;
      break;
    case 6:
      nbMask = CS6;
      break;
    case 7:
      nbMask = CS7;
      break;
    case 8:
      nbMask = CS8;
      break;
    default:
      return false;
  }
  // Apply to configuration
  pvTermios.c_cflag &= ~CSIZE;
  pvTermios.c_cflag |= nbMask;
  
  return true;
}

int mdtSerialPortPosix::dataBits()
{
  int nb = 0;

  // Get configured bits count
  if((pvTermios.c_cflag & CSIZE) == CS8){
    nb = 8;
  }else if((pvTermios.c_cflag & CSIZE) == CS7){
    nb = 7;
  }else if((pvTermios.c_cflag & CSIZE) == CS6){
    nb = 6;
  }else if((pvTermios.c_cflag & CSIZE) == CS5){
    nb = 5;
  }else{
    nb = 0;
  }

  return nb;
}

bool mdtSerialPortPosix::setStopBits(int n)
{
  if(n == 1){
    pvTermios.c_cflag &= ~CSTOPB;
  }else if(n == 2){
    pvTermios.c_cflag |= CSTOPB;
  }else{
    return false;
  }
  return true;
}

int mdtSerialPortPosix::stopBits()
{
  if(pvTermios.c_cflag & CSTOPB){
    return 2;
  }
  return 1;
}

void mdtSerialPortPosix::setParity(mdtSerialPortConfig::parity_t p)
{
  if(p == mdtSerialPortConfig::NoParity){
    pvTermios.c_cflag &= ~PARENB;
    pvTermios.c_iflag &= ~(INPCK | ISTRIP);
  }else if(p == mdtSerialPortConfig::ParityOdd){
    pvTermios.c_cflag |= PARENB;
    pvTermios.c_cflag |= PARODD;
    // Check and strip parity
    pvTermios.c_iflag |= (INPCK | ISTRIP);
  }else{
    pvTermios.c_cflag |= PARENB;
    pvTermios.c_cflag &= ~PARODD;
    // Check and strip parity
    pvTermios.c_iflag |= (INPCK | ISTRIP);
  }
}

mdtSerialPortConfig::parity_t mdtSerialPortPosix::parity()
{
  if(!(pvTermios.c_cflag & PARENB)){
    return mdtSerialPortConfig::NoParity;
  }
  // Here, parity is enabled - Check if it's even or odd
  if((pvTermios.c_cflag & PARENB)&&(pvTermios.c_cflag & PARODD)){
    return mdtSerialPortConfig::ParityOdd;
  }
  return mdtSerialPortConfig::ParityEven;
}

void mdtSerialPortPosix::setFlowCtlRtsCts(bool on)
{
  if(on){
    pvTermios.c_cflag |= CRTSCTS;
  }else{
    pvTermios.c_cflag &= ~CRTSCTS;
  }
}

bool mdtSerialPortPosix::flowCtlRtsCtsOn()
{
  return (pvTermios.c_cflag & CRTSCTS);
}

void mdtSerialPortPosix::setFlowCtlXonXoff(bool on, char xonChar, char xoffChar)
{
  if(on){
    pvTermios.c_iflag |= (IXON | IXOFF | IXANY);
    pvTermios.c_cc[VSTART] = xonChar;
    pvTermios.c_cc[VSTOP] = xoffChar;
  }else{
    pvTermios.c_iflag &= ~(IXON | IXOFF | IXANY);
  }
}

bool mdtSerialPortPosix::flowCtlXonXoffOn()
{
  return (pvTermios.c_iflag & (IXON | IXOFF | IXANY));
}

bool mdtSerialPortPosix::checkConfig(mdtSerialPortConfig cfg)
{
  Q_ASSERT(pvFd >= 0);

  // Get current system config
  tcgetattr(pvFd, &pvTermios);

  // Check static coded flags
  if(!(pvTermios.c_cflag & (CLOCAL | CREAD))){
    mdtError e(MDT_UNDEFINED_ERROR, "CLOCAL and CREAD are not set", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }
  if(pvTermios.c_lflag & (ICANON | ECHO | ECHOE | ISIG)){
    mdtError e(MDT_UNDEFINED_ERROR, "raw data mode not set", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }
  if(pvTermios.c_oflag & OPOST){
    mdtError e(MDT_UNDEFINED_ERROR, "raw data mode not set (OPOST is on)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }
  // Check parameters given by cfg
  if(baudRate() != cfg.baudRate()){
    mdtError e(MDT_UNDEFINED_ERROR, "baud rate is not set", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }
  if(dataBits() != cfg.dataBitsCount()){
    mdtError e(MDT_UNDEFINED_ERROR, "data bits count is not set", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }
  if(stopBits() != cfg.stopBitsCount()){
    qDebug() << "stopBits(): " << stopBits() << " , cfg: " << cfg.stopBitsCount();
    mdtError e(MDT_UNDEFINED_ERROR, "stop bits count is not set", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }
  if(parity() != cfg.parity()){
    mdtError e(MDT_UNDEFINED_ERROR, "parity is not set", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }
  if(flowCtlRtsCtsOn() != cfg.flowCtlRtsCtsEnabled()){
    mdtError e(MDT_UNDEFINED_ERROR, "Flow ctl RTS/CTS is not set", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }
  if(flowCtlXonXoffOn() != cfg.flowCtlXonXoffEnabled()){
    mdtError e(MDT_UNDEFINED_ERROR, "Flow ctl Xon/Xoff is not set", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }
  if(cfg.flowCtlXonXoffEnabled()){
    if(pvTermios.c_cc[VSTART] != cfg.xonChar()){
      mdtError e(MDT_UNDEFINED_ERROR, "Xon char is not set", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
      e.commit();
      return false;
    }
    if(pvTermios.c_cc[VSTOP] != cfg.xoffChar()){
      mdtError e(MDT_UNDEFINED_ERROR, "Xoff char is not set", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
      e.commit();
      return false;
    }
  }

  return true;
}

void mdtSerialPortPosix::sigactionHandle(int/*signum*/)
{
}

bool mdtSerialPortPosix::setRtsOn()
{
  int states;

  // Get the current ctl states
  if(ioctl(pvFd, TIOCMGET, &states) < 0){
    mdtError e(MDT_PORT_IO_ERROR, "ioctl() call failed with command TIOCMGET", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }
  // Set RTS state
  states |= TIOCM_RTS;
  // Commit to system
  if(ioctl(pvFd, TIOCMSET, &states) < 0){
    mdtError e(MDT_PORT_IO_ERROR, "ioctl() call failed with command TIOCMSET", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }

  return true;
}

bool mdtSerialPortPosix::setRtsOff()
{
  int states;

  // Get the current ctl states
  if(ioctl(pvFd, TIOCMGET, &states) < 0){
    mdtError e(MDT_PORT_IO_ERROR, "ioctl() call failed with command TIOCMGET", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }
  // Set RTS state
  states &= ~TIOCM_RTS;
  // Commit to system
  if(ioctl(pvFd, TIOCMSET, &states) < 0){
    mdtError e(MDT_PORT_IO_ERROR, "ioctl() call failed with command TIOCMSET", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortPosix");
    e.commit();
    return false;
  }

  return true;
}
