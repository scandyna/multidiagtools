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
#include "mdtSerialPort.h"
#include "mdtError.h"
#include "mdtPortThread.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <errno.h> 
#include <cstring>
#include <QString>

#include <QDebug>

mdtSerialPort::mdtSerialPort(QObject *parent)
 : mdtAbstractSerialPort(parent)
{
  pvPreviousCarState = 0;
  pvPreviousDsrState = 0;
  pvPreviousCtsState = 0;
  pvPreviousRngState = 0;
  pvFd = -1;
  pvPortLock = new mdtPortLock;
  Q_ASSERT(pvPortLock != 0);
}

mdtSerialPort::~mdtSerialPort()
{
  close();
  delete pvPortLock;
}

bool mdtSerialPort::setBaudRate(int rate)
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
  if(tcsetattr(pvFd, TCSANOW, &pvTermios) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcsetattr() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return false;
  }
  // Check if baud rate could be applied
  if(baudRate() != rate){
    return false;
  }

  return true;
}

int mdtSerialPort::baudRate()
{
  speed_t brMask = 0;
  int rate;

  // Get configured baud rate
  if(tcgetattr(pvFd, &pvTermios) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcgetattr() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return 0;
  }
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

bool mdtSerialPort::setDataBits(int n)
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
  if(tcsetattr(pvFd, TCSANOW, &pvTermios) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcsetattr() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return false;
  }
  // Check if data bits count could be applied
  if(dataBits() != n){
    return false;
  }

  return true;
}

int mdtSerialPort::dataBits()
{
  int nb = 0;

  // Get configured data bits count
  if(tcgetattr(pvFd, &pvTermios) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcgetattr() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return 0;
  }
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

bool mdtSerialPort::setStopBits(int n)
{
  if(n == 1){
    pvTermios.c_cflag &= ~CSTOPB;
  }else if(n == 2){
    pvTermios.c_cflag |= CSTOPB;
  }else{
    return false;
  }
  // Apply to configuration
  if(tcsetattr(pvFd, TCSANOW, &pvTermios) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcsetattr() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return false;
  }
  // Check
  if(stopBits() != n){
    return false;
  }

  return true;
}

int mdtSerialPort::stopBits()
{
  // Get configured stop bits count
  if(tcgetattr(pvFd, &pvTermios) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcgetattr() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return -1;
  }
  if(pvTermios.c_cflag & CSTOPB){
    return 2;
  }
  return 1;
}

bool mdtSerialPort::setParity(mdtSerialPortConfig::parity_t p)
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
  // Apply to configuration
  if(tcsetattr(pvFd, TCSANOW, &pvTermios) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcsetattr() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return false;
  }
  // Check
  if(parity() != p){
    return false;
  }

  return true;
}

mdtSerialPortConfig::parity_t mdtSerialPort::parity()
{
  // Get configured stop bits count
  if(tcgetattr(pvFd, &pvTermios) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcgetattr() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return mdtSerialPortConfig::NoParity;
  }
  if(!(pvTermios.c_cflag & PARENB)){
    return mdtSerialPortConfig::NoParity;
  }
  // Here, parity is enabled - Check if it's even or odd
  if((pvTermios.c_cflag & PARENB)&&(pvTermios.c_cflag & PARODD)){
    return mdtSerialPortConfig::ParityOdd;
  }
  return mdtSerialPortConfig::ParityEven;
}

bool mdtSerialPort::setFlowCtlRtsCts(bool on)
{
  if(on){
    pvTermios.c_cflag |= CRTSCTS;
  }else{
    pvTermios.c_cflag &= ~CRTSCTS;
  }
  // Apply to configuration
  if(tcsetattr(pvFd, TCSANOW, &pvTermios) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcsetattr() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return false;
  }
  // Check
  if(flowCtlRtsCtsOn() != on){
    return false;
  }

  return true;
}

bool mdtSerialPort::flowCtlRtsCtsOn()
{
  // Get configured RTS/CTS flow control
  if(tcgetattr(pvFd, &pvTermios) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcgetattr() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return false;
  }

  return (pvTermios.c_cflag & CRTSCTS);
}

bool mdtSerialPort::setFlowCtlXonXoff(bool on, char xonChar, char xoffChar)
{
  if(on){
    pvTermios.c_iflag |= (IXON | IXOFF | IXANY);
    pvTermios.c_cc[VSTART] = xonChar;
    pvTermios.c_cc[VSTOP] = xoffChar;
  }else{
    pvTermios.c_iflag &= ~(IXON | IXOFF | IXANY);
  }
  // Apply to configuration
  if(tcsetattr(pvFd, TCSANOW, &pvTermios) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcsetattr() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return false;
  }
  // Check
  if(flowCtlXonXoffOn() != on){
    return false;
  }

  return true;
}

bool mdtSerialPort::flowCtlXonXoffOn()
{
  // Get configured Xon/Xoff flow control
  if(tcgetattr(pvFd, &pvTermios) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcgetattr() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return false;
  }

  return (pvTermios.c_iflag & (IXON | IXOFF | IXANY));
}

void mdtSerialPort::setReadTimeout(int timeout)
{
  if(timeout == -1){
    pvReadTimeout.tv_sec = -1;
    pvReadTimeout.tv_usec = 0;
  }else{
    pvReadTimeout.tv_sec = timeout/1000;
    pvReadTimeout.tv_usec = 1000*(timeout%1000);
  }
}

void mdtSerialPort::setWriteTimeout(int timeout)
{
  if(timeout == -1){
    pvWriteTimeout.tv_sec = -1;
    pvWriteTimeout.tv_usec = 0;
  }else{
    pvWriteTimeout.tv_sec = timeout/1000;
    pvWriteTimeout.tv_usec = 1000*(timeout%1000);
  }
}

mdtAbstractPort::error_t mdtSerialPort::waitForReadyRead()
{
  fd_set input;
  int n;
  struct timeval tv = pvReadTimeout;  // Create a copy, because select() alter the timeout

  // Init the select call
  FD_ZERO(&input);
  FD_SET(pvFd, &input);

  pvMutex.unlock();
  if(tv.tv_sec == -1){
    n = select(pvFd+1, &input, 0, 0, 0);
  }else{
    n = select(pvFd+1, &input, 0, 0, &tv);
  }
  pvMutex.lock();
  if(n == 0){
    updateReadTimeoutState(true);
  }else{
    updateReadTimeoutState(false);
    if(n < 0){
      switch(errno){
        case EINTR:
          // Thread has send the stop signal
          return WaitingCanceled;
        default:
          // Unhandled error
          mdtError e(MDT_UNDEFINED_ERROR, "select() call failed", mdtError::Error);
          e.setSystemError(errno, strerror(errno));
          MDT_ERROR_SET_SRC(e, "mdtSerialPort");
          e.commit();
          return UnknownError;
      }
    }
  }

  return NoError;
}

qint64 mdtSerialPort::read(char *data, qint64 maxSize)
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
        mdtError e(MDT_SERIAL_PORT_IO_ERROR, "read() call failed", mdtError::Error);
        e.setSystemError(err, strerror(err));
        MDT_ERROR_SET_SRC(e, "mdtSerialPort");
        e.commit();
        return n;
    }
  }

  return n;
}

bool mdtSerialPort::suspendTransmission()
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
      mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcflow() call failed", mdtError::Error);
      e.setSystemError(err, strerror(err));
      MDT_ERROR_SET_SRC(e, "mdtSerialPort");
      e.commit();
      return false;
    }
  }

  return true;
}

bool mdtSerialPort::resumeTransmission()
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
      mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcflow() call failed", mdtError::Error);
      e.setSystemError(err, strerror(err));
      MDT_ERROR_SET_SRC(e, "mdtSerialPort");
      e.commit();
      return false;
    }
  }

  return true;
}

/**
void mdtSerialPort::flushIn()
{
  int err;

  lockMutex();
  if(tcflush(pvFd, TCIFLUSH) < 0){
    err = errno;
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcflush() call failed", mdtError::Error);
    e.setSystemError(err, strerror(err));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
  }
  mdtAbstractPort::flushIn();
}
*/

mdtAbstractPort::error_t mdtSerialPort::waitEventWriteReady()
{
  fd_set output;
  int n;
  struct timeval tv = pvWriteTimeout;  // Create a copy, because select() alter the timeout

  // Init the select call
  FD_ZERO(&output);
  FD_SET(pvFd, &output);

  pvMutex.unlock();
  if(tv.tv_sec == -1){
    n = select(pvFd+1, 0, &output, 0, 0);
  }else{
    n = select(pvFd+1, 0, &output, 0, &tv);
  }
  pvMutex.lock();
  if(n == 0){
    updateWriteTimeoutState(true);
  }else{
    updateWriteTimeoutState(false);
    if(n < 0){
      switch(errno){
        case EINTR:
          // Thread has send the stop signal
          return WaitingCanceled;
        default:
          // Unhandled error
          mdtError e(MDT_UNDEFINED_ERROR, "select() call failed", mdtError::Error);
          e.setSystemError(errno, strerror(errno));
          MDT_ERROR_SET_SRC(e, "mdtSerialPort");
          e.commit();
          return UnknownError;
      }
    }
  }
  // Check about flushOut
  if(pvCancelWrite){
    return WriteCanceled;
  }

  return NoError;
}

qint64 mdtSerialPort::write(const char *data, qint64 maxSize)
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
        mdtError e(MDT_SERIAL_PORT_IO_ERROR, "write() call failed", mdtError::Error);
        e.setSystemError(err, strerror(err));
        MDT_ERROR_SET_SRC(e, "mdtSerialPort");
        e.commit();
        return n;
    }
  }
  ///qDebug() << "wrote " << n << " bytes";

  return n;
}

/**
void mdtSerialPort::flushOut()
{
  int err;

  lockMutex();
  if(tcflush(pvFd, TCOFLUSH) < 0){
    err = errno;
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcflush() call failed", mdtError::Error);
    e.setSystemError(err, strerror(err));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
  }
  mdtAbstractPort::flushOut();
}
*/

mdtAbstractPort::error_t mdtSerialPort::waitEventCtl()
{
  int retVal;

  // We wait until a line status change happens
  ///qDebug() << "mdtSerialPort::waitEventCtl(), waiting...";
  pvMutex.unlock();
  retVal = ioctl(pvFd, TIOCMIWAIT, (TIOCM_CAR | TIOCM_DSR | TIOCM_CTS | TIOCM_RNG));
  pvMutex.lock();
  qDebug() << "mdtSerialPort::waitEventCtl(), waiting DONE";
  if(retVal < 0){
    switch(errno){
      case EINTR:
        // Thread has send the stop signal
        return WaitingCanceled;
      default:
        // Unhandled error
        mdtError e(MDT_UNDEFINED_ERROR, "ioctl() call failed with command TIOCMIWAIT", mdtError::Error);
        e.setSystemError(errno, strerror(errno));
        MDT_ERROR_SET_SRC(e, "mdtSerialPort");
        e.commit();
        return UnknownError;
    }
  }

  return NoError;
}

bool mdtSerialPort::getCtlStates()
{
  int states;

  if(ioctl(pvFd, TIOCMGET, &states) < 0){
    mdtError e(MDT_UNDEFINED_ERROR, "ioctl() call failed with command TIOCMGET", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
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

void mdtSerialPort::setRts(bool on)
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

void mdtSerialPort::setDtr(bool on)
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
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
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
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
  }
  pvMutex.unlock();
}

mdtAbstractPort::error_t mdtSerialPort::pvOpen()
{
  Q_ASSERT(!isOpen());

  int err;

  // Try to open port locked
  //  O_RDWR : Read/write access
  //  O_NOCTTY: not a terminal
  //  O_NDELAY: ignore DCD signal
  pvFd = pvPortLock->openLocked(pvPortName, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if(pvFd < 0){
    err = errno;
    // Check if port was locked by another
    if(pvPortLock->isLockedByAnother()){
      mdtError e(MDT_SERIAL_PORT_IO_ERROR, "Port " + pvPortName + " allready locked, cannot open it", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtSerialPort");
      e.commit();
      return PortLocked;
    }
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "Unable to open port: " + pvPortName, mdtError::Error);
    e.setSystemError(err, strerror(err));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    // Here me must explicitly unlock the port because we return a error
    pvPortLock->unlock();
    // Check error and return a possibly correct code
    switch(err){
      case EACCES:
        return PortAccess;
      case ENOENT:
        return PortNotFound;
      default:
        return UnknownError;
    }
  }
  // Get current config and save it to pvOriginalTermios
  if(tcgetattr(pvFd, &pvOriginalTermios) < 0){
    close();
    // Here me must explicitly do close because we return a error
    pvPortLock->unlock();
    ::close(pvFd);
    pvFd = -1;
    // Most of cases, this function fails because the physical port is not attached
    return PortNotFound;
  }
  // Get current system config on "work copy"
  if(tcgetattr(pvFd, &pvTermios) < 0){
    close();
    // Here me must explicitly do close because we return a error
    pvPortLock->unlock();
    ::close(pvFd);
    pvFd = -1;
    // Most of cases, this function fails because the physical port is not attached
    return PortNotFound;
  }

  return NoError;
}

void mdtSerialPort::pvClose()
{
  Q_ASSERT(isOpen());

  // Restore original termios struct
  if(tcsetattr(pvFd, TCSANOW, &pvOriginalTermios) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcsetattr() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
  }
  // Close the port
  pvPortLock->unlock();
  ::close(pvFd);
  pvFd = -1;
}

mdtAbstractPort::error_t mdtSerialPort::pvSetup()
{
  Q_ASSERT(isOpen());

  QString strNum;

  // Set baud rate
  if(!setBaudRate(config().baudRate())){
    strNum.setNum(config().baudRate());
    mdtError e(MDT_UNDEFINED_ERROR, "unsupported baud rate '" + strNum + "' for port " + pvPortName, mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return SetupError;
  }
  // Set local mode and enable the receiver
  pvTermios.c_cflag |= (CLOCAL | CREAD);
  // Set data bits
  if(!setDataBits(config().dataBitsCount())){
    strNum.setNum(config().dataBitsCount());
    mdtError e(MDT_UNDEFINED_ERROR, "unsupported data bits count '" + strNum + "' for port " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return SetupError;
  }
  // Set stop bits
  if(!setStopBits(config().stopBitsCount())){
    strNum.setNum(config().stopBitsCount());
    mdtError e(MDT_UNDEFINED_ERROR, "unsupported stop bits count '" + strNum + "' for port " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return SetupError;
  }
  // Set parity
  if(!setParity(config().parity())){
    mdtError e(MDT_UNDEFINED_ERROR, "unsupported parity requested for port " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return SetupError;
  }
  // Set flow control
  if(!setFlowCtlRtsCts(config().flowCtlRtsCtsEnabled())){
    mdtError e(MDT_UNDEFINED_ERROR, "unsupported flow control for port " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return SetupError;
  }
  if(!setFlowCtlXonXoff(config().flowCtlXonXoffEnabled(), config().xonChar(), config().xoffChar())){
    mdtError e(MDT_UNDEFINED_ERROR, "unsupported flow control for port " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return SetupError;
  }
  // Set raw data mode
  pvTermios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  pvTermios.c_oflag &= ~OPOST;
  if(tcsetattr(pvFd, TCSANOW, &pvTermios) < 0){
    mdtError e(MDT_UNDEFINED_ERROR, "cannot set raw mode on port " + pvPortName, mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return SetupError;
  }
  // Set the read/write timeouts
  setReadTimeout(config().readTimeout());
  setWriteTimeout(config().writeTimeout());
  // Init flags
  pvCancelWrite = false;

  qDebug() << "mdtSerialPort::pvSetup() setup:";
  qDebug() << "-> Baudrate: " << baudRate();
  qDebug() << "-> Data bits: " << dataBits();
  qDebug() << "-> Stop bits: " << stopBits();
  qDebug() << "-> parity: " << parity();
  qDebug() << "-> Xon/Xoff: " << flowCtlXonXoffOn();
  qDebug() << "-> CTS/RTS: " << flowCtlRtsCtsOn();

  return NoError;
}

void mdtSerialPort::pvFlushIn()
{
  qDebug() << "mdtSerialPort::pvFlushIn() ...";
  if(tcflush(pvFd, TCIFLUSH) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcflush() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
  }
}

void mdtSerialPort::pvFlushOut()
{
  qDebug() << "mdtSerialPort::pvFlushOut() ...";
  pvCancelWrite = true;
  if(tcflush(pvFd, TCOFLUSH) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "tcflush() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
  }
}

void mdtSerialPort::mapUartType()
{
  Q_ASSERT(isOpen());

  struct serial_struct serinfo;

  // Request system's UART type.
  if(ioctl(pvFd, TIOCGSERIAL, &serinfo) < 0){
    pvUartType = UT_UNKNOW;
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "ioctl() failed for port " + pvPortName, mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    return;
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
  }
}

void mdtSerialPort::buildAvailableBaudRates()
{
  Q_ASSERT(isOpen());

  QList<int> tmpAvailableBaudRates;
  int i;

  // Build defined baud rates list
  tmpAvailableBaudRates << 50;
  tmpAvailableBaudRates << 75;
  tmpAvailableBaudRates << 110;
  tmpAvailableBaudRates << 134; // Note: is 134.5
  tmpAvailableBaudRates << 150;
  tmpAvailableBaudRates << 200;
  tmpAvailableBaudRates << 300;
  tmpAvailableBaudRates << 600;
  tmpAvailableBaudRates << 1200;
  tmpAvailableBaudRates << 1800;
  tmpAvailableBaudRates << 2400;
  tmpAvailableBaudRates << 4800;
  tmpAvailableBaudRates << 9600;
  tmpAvailableBaudRates << 19200;
  tmpAvailableBaudRates << 38400;
#ifdef  B57600
  tmpAvailableBaudRates << 57600;
#endif
#ifdef B76800
  tmpAvailableBaudRates << 76800;
#endif
#ifdef  B115200
  tmpAvailableBaudRates << 115200;
#endif
#ifdef  B230400
  tmpAvailableBaudRates << 230400;
#endif
#ifdef  B460800
  tmpAvailableBaudRates << 460800;
#endif
#ifdef  B500000
  tmpAvailableBaudRates << 500000;
#endif
#ifdef  B576000
  tmpAvailableBaudRates << 576000;
#endif
#ifdef  B921600
  tmpAvailableBaudRates << 921600;
#endif
#ifdef  B1000000
  tmpAvailableBaudRates << 1000000;
#endif
#ifdef  B1152000
  tmpAvailableBaudRates << 1152000;
#endif
#ifdef  B1500000
  tmpAvailableBaudRates << 1500000;
#endif
#ifdef  B2000000
  tmpAvailableBaudRates << 2000000;
#endif
#ifdef  B2500000
  tmpAvailableBaudRates << 2500000;
#endif
#ifdef  B3000000
  tmpAvailableBaudRates << 3000000;
#endif
#ifdef  B3500000
  tmpAvailableBaudRates << 3500000;
#endif
#ifdef  B4000000
  tmpAvailableBaudRates << 4000000;
#endif

  // Check each defined baud rate, try to apply, and store to container if Ok
  pvAvailableBaudRates.clear();
  for(i=0; i<tmpAvailableBaudRates.size(); ++i){
    if(setBaudRate(tmpAvailableBaudRates.at(i))){
      pvAvailableBaudRates << tmpAvailableBaudRates.at(i);
    }
  }
}

bool mdtSerialPort::setRtsOn()
{
  int states;

  // Get the current ctl states
  if(ioctl(pvFd, TIOCMGET, &states) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "ioctl() call failed with command TIOCMGET", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return false;
  }
  // Set RTS state
  states |= TIOCM_RTS;
  // Commit to system
  if(ioctl(pvFd, TIOCMSET, &states) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "ioctl() call failed with command TIOCMSET", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return false;
  }

  return true;
}

bool mdtSerialPort::setRtsOff()
{
  int states;

  // Get the current ctl states
  if(ioctl(pvFd, TIOCMGET, &states) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "ioctl() call failed with command TIOCMGET", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return false;
  }
  // Set RTS state
  states &= ~TIOCM_RTS;
  // Commit to system
  if(ioctl(pvFd, TIOCMSET, &states) < 0){
    mdtError e(MDT_SERIAL_PORT_IO_ERROR, "ioctl() call failed with command TIOCMSET", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPort");
    e.commit();
    return false;
  }

  return true;
}
