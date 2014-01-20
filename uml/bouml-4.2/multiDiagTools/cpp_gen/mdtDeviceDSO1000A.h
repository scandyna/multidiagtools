#ifndef _MDTDEVICEDSO1000A_H
#define _MDTDEVICEDSO1000A_H


#include "mdtDeviceScpi.h"

class mdtDeviceInfo;

class mdtDeviceDSO1000A : public mdtDeviceScpi {
  public:
    mdtDeviceDSO1000A(const QObject & parent);

    ~mdtDeviceDSO1000A();

    mdtAbstractPort::error_t connectToDevice(const mdtDeviceInfo & devInfo);


  private:
    virtual void decodeReadenFrame(const mdtPortTransaction & transaction);

    mdtFrameCodecScpi pvCodec;

};
#endif
