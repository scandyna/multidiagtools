#ifndef _MDTDEVICEU3606A_H
#define _MDTDEVICEU3606A_H


#include "mdtDevice.h"

class mdtDeviceU3606A : public mdtDevice {
  public:
    mdtDeviceU3606A(const QObject & parent);

    ~mdtDeviceU3606A();


  private:
    mdtUsbtmcPortManager ;

    mdtUsbtmcPortManager pvPortManager;

    mdtFrameCodecScpiU3606A pvCodec;

};
#endif
