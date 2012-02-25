#ifndef _MDTDEVICEFILECONFIG_H
#define _MDTDEVICEFILECONFIG_H


#include <string>
using namespace std;

class mdtDeviceFileConfig {
  public:
    mdtDeviceFileConfig();

    ~mdtDeviceFileConfig();


  private:
    string pvInterface;

    int pvReadFrameSize;

    int pvReadQueueSize;

    int pvWriteFrameSize;

    int pvWriteQueueSize;

    mdtFrame::type_t pvFrameType;

    QByteArray pvEndOfFrameSeq;


  public:
    void setDefault();

    void setInterface(const QString & interface);

    QString interface();

    void setReadFrameSize(int size);

    int readFrameSize();

    void setReadQueueSize(int size);

    int readQueueSize();

    void setWriteFrameSize(int size);

    int writeFrameSize();

    void setWriteQueueSize(int size);

    int writeQueueSize();

    void setFrameType(const mdtFrame::type_t & type);

    mdtFrame::type_t frameType();

    void setEndOfFrameSeq(const QByteArray & seq);

    QByteArray endOfFrameSeq();

};
#endif
