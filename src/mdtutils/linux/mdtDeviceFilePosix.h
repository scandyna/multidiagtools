#ifndef MDT_DEVICE_FILE_POSIX_H
#define MDT_DEVICE_FILE_POSIX_H

#include "mdtAbstractDeviceFile.h"

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <QObject>
#include <QString>

class mdtDeviceFilePosix : public mdtAbstractDeviceFile
{
 public:

  mdtDeviceFilePosix(QObject *parent = 0);
  ~mdtDeviceFilePosix();

  bool open(mdtDeviceFileConfig &cfg);

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

#endif  // #ifndef MDT_DEVICE_FILE_POSIX_H
