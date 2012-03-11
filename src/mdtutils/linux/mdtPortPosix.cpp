
#include "mdtPortPosix.h"
#include "mdtError.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <QDebug>

mdtPortPosix::mdtPortPosix(QObject *parent)
 : mdtAbstractPort(parent)
{
  pvFd = -1;
}

mdtPortPosix::~mdtPortPosix()
{
  this->close();
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
  pvFd = ::open(pvName.toStdString().c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK );
  if(pvFd < 0){
    mdtError e(MDT_UNDEFINED_ERROR, "Unable to open port: " + pvName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortPosix");
    e.commit();
    return false;
  }

  return true;
}

void mdtPortPosix::close()
{
  if(pvFd >= 0){
    ::close(pvFd);
    pvFd = -1;
  }
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

bool mdtPortPosix::waitEventRead()
{
  fd_set input;
  int n;

  // Init the select call
  FD_ZERO(&input);
  FD_SET(pvFd, &input);

  n = select(pvFd+1, &input, 0, 0, &pvReadTimeout);
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

int mdtPortPosix::readData(char *data, int maxLen)
{
  int n;

  n = read(pvFd, data, maxLen);
  if(n < 0){
    switch(errno){
      case EAGAIN:      // No data available
        return 0;
      case ETIMEDOUT:   // Read timeout (happens with USBTMC)
        updateReadTimeoutState(true);
        return 0;
      default:
        mdtError e(MDT_UNDEFINED_ERROR, "read() call failed", mdtError::Error);
        e.setSystemError(errno, strerror(errno));
        MDT_ERROR_SET_SRC(e, "mdtPortPosix");
        e.commit();
        return 0;
    }

/*
    switch(errno){
      case EINTR:
        perror("EINTR");
        break;
      case EAGAIN:
        perror("EAGAIN");
        break;
      case EIO:
        perror("EIO");
        break;
      case EISDIR:
        perror("EISDIR");
        break;
      case EBADF:
        perror("EBADF");
        break;
      case EINVAL:
        perror("EINVAL");
        break;
      case EFAULT:
        perror("EFAULT");
        break;
      default:
        qDebug() << "unknow error";
    }
*/
  }

  return n;
}

void mdtPortPosix::flushIn()
{
  qDebug() << "mdtPortPosix::flushIn() - NOT implemented !!!";
}

bool mdtPortPosix::waitEventWriteReady()
{
  fd_set output;
  int n;

  // Init the select call
  FD_ZERO(&output);
  FD_SET(pvFd, &output);

  n = select(pvFd+1, 0, &output, 0, &pvWriteTimeout);
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

int mdtPortPosix::writeData(const char *data, int maxLen)
{
  int n;

  n = write(pvFd, data, maxLen);
  if(n < 0){
    mdtError e(MDT_UNDEFINED_ERROR, "write() call failed", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtPortPosix");
    e.commit();
/*
    switch(errno){
      case EINTR:
        perror("EINTR");
        break;
      case EAGAIN:
        perror("EAGAIN");
        break;
      case EIO:
        perror("EIO");
        break;
      case EPIPE:
        perror("EPIPE");
        break;
      case EBADF:
        perror("EBADF");
        break;
      case EINVAL:
        perror("EINVAL");
        break;
      case EFAULT:
        perror("EFAULT");
        break;
      case ENOSPC:
        perror("ENOSPC");
        break;
      default:
        qDebug() << "unknow error";
    }
*/
  }

  return n;
}

void mdtPortPosix::flushOut()
{
  qDebug() << "mdtPortPosix::flushOut() - NOT implemented !!!";
}

