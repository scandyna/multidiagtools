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
#include "mdtPortPosix.h"
#include "mdtError.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <errno.h>

//#include <QDebug>

mdtPortPosix::mdtPortPosix(QObject *parent)
 : mdtAbstractPort(parent)
{
  pvFd = -1;
  pvPortLock = new mdtPortLock;
  Q_ASSERT(pvPortLock != 0);
}

mdtPortPosix::~mdtPortPosix()
{
  this->close();
  delete pvPortLock;
}

bool mdtPortPosix::setAttributes(const QString &portName)
{
  // Close previous opened device
  this->close();
  // Try to open given port
  pvFd = ::open(portName.toStdString().c_str(), O_RDWR);
  if(pvFd < 0){
    mdtError e(MDT_UNDEFINED_ERROR, "Unable to open port: " + portName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortPosix");
    e.commit();
    pvName = "";
    return false;
  }
  // Set attributes
  pvName = portName;
  // Close the port
  this->close();

  return true;
}

bool mdtPortPosix::open(mdtPortConfig &cfg)
{
  // Close previous opened device
  this->close();
  // Try to open port
  //  O_RDWR : Read/write access
  //  O_NOCTTY: not a terminal
  //  O_NDELAY: ignore DCD signal
  lockMutex();
  // In read only mode, we handle no lock
  if(cfg.readOnly()){
    pvFd = ::open(pvName.toStdString().c_str(), O_RDONLY | O_NOCTTY | O_NONBLOCK );
  }else{
    pvFd = pvPortLock->openLocked(pvName, O_RDWR | O_NOCTTY | O_NONBLOCK);
  }
  if(pvFd < 0){
    mdtError e(MDT_UNDEFINED_ERROR, "Unable to open port: " + pvName, mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtPortPosix");
    e.commit();
    pvPortLock->unlock();
    unlockMutex();
    return false;
  }
  // Set the read/write timeouts
  setReadTimeout(cfg.readTimeout());
  setWriteTimeout(cfg.writeTimeout());

  return mdtAbstractPort::open(cfg);
}

void mdtPortPosix::close()
{
  if(!isOpen()){
    return;
  }
  lockMutex();
  ///if(pvFd >= 0){
  if(pvPortLock->isLocked()){
    pvPortLock->unlock();
  }
  ::close(pvFd);
  pvFd = -1;
  ///}
  mdtAbstractPort::close();
}

void mdtPortPosix::setReadTimeout(int timeout)
{
  pvReadTimeout.tv_sec = timeout/1000;
  pvReadTimeout.tv_usec = 1000*(timeout%1000);
}

void mdtPortPosix::setWriteTimeout(int timeout)
{
  pvWriteTimeout.tv_sec = timeout/1000;
  pvWriteTimeout.tv_usec = 1000*(timeout%1000);
}

bool mdtPortPosix::waitForReadyRead()
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
      MDT_ERROR_SET_SRC(e, "mdtPortPosix");
      e.commit();
      return false;
    }
  }

  return true;
}

qint64 mdtPortPosix::read(char *data, qint64 maxSize)
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
        mdtError e(MDT_UNDEFINED_ERROR, "read() call failed", mdtError::Error);
        e.setSystemError(err, strerror(err));
        MDT_ERROR_SET_SRC(e, "mdtPortPosix");
        e.commit();
        return n;
    }
  }

  return n;
}

void mdtPortPosix::flushIn()
{
  lockMutex();
  mdtAbstractPort::flushIn();
}

bool mdtPortPosix::waitEventWriteReady()
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
      MDT_ERROR_SET_SRC(e, "mdtPortPosix");
      e.commit();
      return false;
    }
  }

  return true;
}

qint64 mdtPortPosix::write(const char *data, qint64 maxSize)
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
        mdtError e(MDT_UNDEFINED_ERROR, "write() call failed", mdtError::Error);
        e.setSystemError(err, strerror(err));
        MDT_ERROR_SET_SRC(e, "mdtPortPosix");
        e.commit();
        return n;
    }
  }

  return n;
}

void mdtPortPosix::flushOut()
{
  lockMutex();
  mdtAbstractPort::flushOut();
}
