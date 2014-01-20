#ifndef _MDTDEVICEFILEPOSIX_H
#define _MDTDEVICEFILEPOSIX_H


#include "mdtAbstractDeviceFile.h"

class mdtDeviceFilePosix : public mdtAbstractDeviceFile {
  public:
     mdtDeviceFilePosix(const QObject & parent = 0);

     ~mdtDeviceFilePosix();

    virtual bool open(const QString & path);

    virtual void close();

    virtual void setReadTimeout(int timeout);

    virtual void setWriteTimeout(int timeout);

    virtual bool waitEventRead();

    virtual int readData(char data, int maxLen);

    virtual void flushIn();

    virtual bool waitEventWriteReady();

    virtual int writeData(char data, int maxLen);

    virtual void flushOut();


  protected:
    int pvFd;


  private:
    struct timeval pvReadTimeout;

    struct timeval pvWriteTimeout;

};
#endif
