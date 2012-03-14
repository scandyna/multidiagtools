#ifndef MDT_PORT_POSIX_H
#define MDT_PORT_POSIX_H

#include "mdtAbstractPort.h"

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <QObject>
#include <QString>

class mdtPortPosix : public mdtAbstractPort
{
 public:

  mdtPortPosix(QObject *parent = 0);
  ~mdtPortPosix();

  // Implemtation of mdtAbstractPort
  bool setAttributes(const QString &portName);

  // Implemtation of mdtAbstractPort
  bool open(mdtPortConfig &cfg);

  // Implemtation of mdtAbstractPort
  void close();

  // Implemtation of mdtAbstractPort
  void setReadTimeout(int timeout);

  // Implemtation of mdtAbstractPort
  void setWriteTimeout(int timeout);

  // Implemtation of mdtAbstractPort
  bool waitForReadyRead();

  // Implemtation of mdtAbstractPort method
  qint64 read(char *data, qint64 maxSize);

  // Implemtation of mdtAbstractPort
  bool waitEventWriteReady();

  // Implemtation of mdtAbstractPort method
  qint64 write(const char *data, qint64 maxSize);

 protected:

  int pvFd;

 private:

  struct timeval pvReadTimeout;
  struct timeval pvWriteTimeout;
};

#endif  // #ifndef MDT_PORT_POSIX_H
