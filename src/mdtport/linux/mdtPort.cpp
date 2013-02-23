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
#include "mdtPort.h"
#include "mdtError.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

//#include <QDebug>

mdtPort::mdtPort(QObject *parent)
 : mdtAbstractPort(parent)
{
  pvFd = -1;
  pvPortLock = new mdtPortLock;
  Q_ASSERT(pvPortLock != 0);
}

mdtPort::~mdtPort()
{
  close();
  delete pvPortLock;
}

void mdtPort::setReadTimeout(int timeout)
{
  if(timeout == -1){
    pvReadTimeout.tv_sec = -1;
    pvReadTimeout.tv_usec = 0;
  }else{
    pvReadTimeout.tv_sec = timeout/1000;
    pvReadTimeout.tv_usec = 1000*(timeout%1000);
  }
}

void mdtPort::setWriteTimeout(int timeout)
{
  if(timeout == -1){
    pvWriteTimeout.tv_sec = -1;
    pvWriteTimeout.tv_usec = 0;
  }else{
    pvWriteTimeout.tv_sec = timeout/1000;
    pvWriteTimeout.tv_usec = 1000*(timeout%1000);
  }
}

mdtAbstractPort::error_t mdtPort::waitForReadyRead()
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
    ///updateReadTimeoutState(true);
    return ReadTimeout;
  }else{
    ///updateReadTimeoutState(false);
    if(n < 0){
      switch(errno){
        case EINTR:
          // Thread has send the stop signal
          return ReadCanceled;
        default:
          // Unhandled error
          mdtError e(MDT_UNDEFINED_ERROR, "select() call failed", mdtError::Error);
          e.setSystemError(errno, strerror(errno));
          MDT_ERROR_SET_SRC(e, "mdtPort");
          e.commit();
          return UnhandledError;
      }
    }
  }

  return NoError;
}

qint64 mdtPort::read(char *data, qint64 maxSize)
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
        ///updateReadTimeoutState(true);
        return ReadTimeout;
      default:
        mdtError e(MDT_UNDEFINED_ERROR, "read() call failed", mdtError::Error);
        e.setSystemError(err, strerror(err));
        MDT_ERROR_SET_SRC(e, "mdtPort");
        e.commit();
        return n;
    }
  }

  return n;
}

mdtAbstractPort::error_t mdtPort::waitEventWriteReady()
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
    ///updateWriteTimeoutState(true);
    return WriteTimeout;
  }else{
    ///updateWriteTimeoutState(false);
    if(n < 0){
      switch(errno){
        case EINTR:
          // Thread has send the stop signal
          return WriteCanceled;
        default:
          // Unhandled error
          mdtError e(MDT_UNDEFINED_ERROR, "select() call failed", mdtError::Error);
          e.setSystemError(errno, strerror(errno));
          MDT_ERROR_SET_SRC(e, "mdtPort");
          e.commit();
          return UnhandledError;
      }
    }
  }

  return NoError;
}

qint64 mdtPort::write(const char *data, qint64 maxSize)
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
        MDT_ERROR_SET_SRC(e, "mdtPort");
        e.commit();
        return n;
    }
  }

  return n;
}

mdtAbstractPort::error_t mdtPort::pvOpen()
{
  Q_ASSERT(!isOpen());

  int err;

  // Try to open port
  pvFd = pvPortLock->openLocked(pvPortName, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if(pvFd < 0){
    err = errno;
    // Check if port was locked by another
    if(pvPortLock->isLockedByAnother()){
      mdtError e(MDT_SERIAL_PORT_IO_ERROR, "Port " + pvPortName + " allready locked, cannot open it", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtPort");
      e.commit();
      return PortLocked;
    }
    mdtError e(MDT_PORT_IO_ERROR, "Unable to open port: " + pvPortName, mdtError::Error);
    e.setSystemError(err, strerror(err));
    MDT_ERROR_SET_SRC(e, "mdtPort");
    e.commit();
    pvPortLock->unlock();
    // Check error and return a possibly correct code
    switch(err){
      case EACCES:
        return PortAccess;
      case ENOENT:
        return PortNotFound;
      default:
        return UnhandledError;
    }
  }

  return NoError;
}

void mdtPort::pvClose()
{
  Q_ASSERT(isOpen());

  pvPortLock->unlock();
  ::close(pvFd);
  pvFd = -1;
}

mdtAbstractPort::error_t mdtPort::pvSetup()
{
  Q_ASSERT(isOpen());

  // Set the read/write timeouts
  setReadTimeout(config().readTimeout());
  setWriteTimeout(config().writeTimeout());

  return NoError;
}

void mdtPort::pvFlushIn()
{
}

void mdtPort::pvFlushOut()
{
}
