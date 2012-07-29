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
#include "mdtUsbtmcPort.h"
#include "mdtError.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <errno.h>

mdtUsbtmcPort::mdtUsbtmcPort(QObject *parent)
 : mdtAbstractPort(parent)
{
  pvFd = -1;
  pvPortLock = new mdtPortLock;
  Q_ASSERT(pvPortLock != 0);
}

mdtUsbtmcPort::~mdtUsbtmcPort()
{
  this->close();  /// NOTE!!
  delete pvPortLock;
}

/**
bool mdtUsbtmcPort::setAttributes(const QString &portName)
{
  // Close previous opened device
  this->close();
  // Try to open given port
  pvFd = ::open(portName.toStdString().c_str(), O_RDWR);
  if(pvFd < 0){
    mdtError e(MDT_UNDEFINED_ERROR, "Unable to open port: " + portName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPort");
    e.commit();
    pvPortName = "";
    return false;
  }
  // Set attributes
  pvPortName = portName;
  // Close the port
  this->close();

  return true;
}
*/

/**
mdtAbstractPort::error_t mdtUsbtmcPort::tryOpen()
{
  Q_ASSERT(!isOpen());

  int err;

  // Try to open port
  pvFd = ::open(pvPortName.toStdString().c_str(), O_RDWR);
  if(pvFd < 0){
    err = errno;
    mdtError e(MDT_PORT_IO_ERROR, "Unable to open port: " + pvPortName, mdtError::Error);
    e.setSystemError(err, strerror(err));
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPort");
    e.commit();
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
  // Close the port
  ::close(pvFd);
  pvFd = -1;

  return NoError;
}
*/

/**
bool mdtUsbtmcPort::open(mdtPortConfig &cfg)
{
  // Close previous opened device
  this->close();
  // Try to open port
  //  O_RDWR : Read/write access
  //  O_NOCTTY: not a terminal
  //  O_NDELAY: ignore DCD signal
  lockMutex();
  pvFd = ::open(pvPortName.toStdString().c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK );
  if(pvFd < 0){
    mdtError e(MDT_UNDEFINED_ERROR, "Unable to open port: " + pvPortName, mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPort");
    e.commit();
    return false;
  }
  // Set the read/write timeouts
  setReadTimeout(cfg.readTimeout());
  setWriteTimeout(cfg.writeTimeout());

  return mdtAbstractPort::open(cfg);
}
*/

/**
void mdtUsbtmcPort::close()
{
  if(!isOpen()){
    return;
  }
  lockMutex();
  if(pvFd >= 0){
    ::close(pvFd);
    pvFd = -1;
  }
  mdtAbstractPort::close();
}
*/

void mdtUsbtmcPort::setReadTimeout(int timeout)
{
  pvReadTimeout = timeout;
}

void mdtUsbtmcPort::setWriteTimeout(int timeout)
{
  pvWriteTimeout = timeout;
}

bool mdtUsbtmcPort::waitForReadyRead()
{
  if(!pvReadCondition.wait(&pvMutex, pvReadTimeout)){
    updateReadTimeoutState(true);
    return true;
  }
  updateReadTimeoutState(false);

  return true;
}

qint64 mdtUsbtmcPort::read(char *data, qint64 maxSize)
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
        MDT_ERROR_SET_SRC(e, "mdtUsbtmcPort");
        e.commit();
        return n;
    }
  }

  return n;
}

bool mdtUsbtmcPort::waitEventWriteReady()
{
  if(!pvWriteCondition.wait(&pvMutex, pvWriteTimeout)){
    updateWriteTimeoutState(true);
    return true;
  }
  updateWriteTimeoutState(false);

  return true;
}

qint64 mdtUsbtmcPort::write(const char *data, qint64 maxSize)
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
        MDT_ERROR_SET_SRC(e, "mdtUsbtmcPort");
        e.commit();
        return n;
    }
  }

  return n;
}

void mdtUsbtmcPort::writeOneFrame()
{
  pvMutex.lock();
  pvWriteCondition.wakeAll();
  pvMutex.unlock();
}

void mdtUsbtmcPort::readOneFrame()
{
  pvMutex.lock();
  pvReadCondition.wakeAll();
  pvMutex.unlock();
}

mdtAbstractPort::error_t mdtUsbtmcPort::pvOpen()
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
      MDT_ERROR_SET_SRC(e, "mdtUsbtmcPort");
      e.commit();
      return PortLocked;
    }
    mdtError e(MDT_PORT_IO_ERROR, "Unable to open port: " + pvPortName, mdtError::Error);
    e.setSystemError(err, strerror(err));
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPort");
    e.commit();
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

  return NoError;
}

void mdtUsbtmcPort::pvClose()
{
  Q_ASSERT(isOpen());

  pvPortLock->unlock();
  ::close(pvFd);
  pvFd = -1;
}

mdtAbstractPort::error_t mdtUsbtmcPort::pvSetup()
{
  Q_ASSERT(isOpen());

  // Set the read/write timeouts
  setReadTimeout(config().readTimeout());
  setWriteTimeout(config().writeTimeout());

  return NoError;
}
