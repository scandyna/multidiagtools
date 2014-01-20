#ifndef _MDTDEVICEMODBUSWAGO_H
#define _MDTDEVICEMODBUSWAGO_H


#include "mdtDeviceModbus.h"

class mdtDeviceInfo;
class mdtDeviceModbusWagoModule;

class mdtDeviceModbusWago : public mdtDeviceModbus {
  public:
    mdtDeviceModbusWago(const QObject & parent);

    ~mdtDeviceModbusWago();

    mdtAbstractPort::error_t connectToDevice(const mdtDeviceInfo & devInfo);


  private:
    <mdtDeviceModbusWagoModule> pvModules;

};
#endif
