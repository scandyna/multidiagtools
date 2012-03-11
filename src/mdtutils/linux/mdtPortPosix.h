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

  bool setAttributes(const QString &portName);

  bool open(mdtPortConfig &cfg);

  void close();

  void setReadTimeout(int timeout);

  void setWriteTimeout(int timeout);

  bool waitEventRead();

  int readData(char *data, int maxLen);

  void flushIn();

  bool waitEventWriteReady();

  int writeData(const char *data, int maxLen);

  void flushOut();

 protected:

  int pvFd;

 private:

  struct timeval pvReadTimeout;
  struct timeval pvWriteTimeout;
};

#endif  // #ifndef MDT_PORT_POSIX_H
