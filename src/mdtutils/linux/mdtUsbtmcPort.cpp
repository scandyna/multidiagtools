
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
}

mdtUsbtmcPort::~mdtUsbtmcPort()
{
  this->close();
}

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
    pvName = "";
    return false;
  }
  // Set attributes
  pvName = portName;
  // Close the port
  this->close();

  return true;
}

bool mdtUsbtmcPort::open(mdtPortConfig &cfg)
{
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
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPort");
    e.commit();
    return false;
  }
  // Set the read/write timeouts
  setReadTimeout(cfg.readTimeout());
  setWriteTimeout(cfg.writeTimeout());

  return mdtAbstractPort::open(cfg);
}

void mdtUsbtmcPort::close()
{
  lockMutex();
  if(pvFd >= 0){
    ::close(pvFd);
    pvFd = -1;
  }
  mdtAbstractPort::close();
}

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
  pvReadWriteMutex.lock();
  if(!pvReadCondition.wait(&pvReadWriteMutex, pvReadTimeout)){
    updateReadTimeoutState(true);
    pvReadWriteMutex.unlock();
    return true;
  }
  updateReadTimeoutState(false);
  pvReadWriteMutex.unlock();

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
  pvReadWriteMutex.lock();
  if(!pvWriteCondition.wait(&pvReadWriteMutex, pvWriteTimeout)){
    updateWriteTimeoutState(true);
    pvReadWriteMutex.unlock();
    return true;
  }
  updateWriteTimeoutState(false);
  pvReadWriteMutex.unlock();

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
  pvReadWriteMutex.lock();
  pvWriteCondition.wakeAll();
  pvReadWriteMutex.unlock();
}

void mdtUsbtmcPort::readOneFrame()
{
  pvReadWriteMutex.lock();
  pvReadCondition.wakeAll();
  pvReadWriteMutex.unlock();
}
