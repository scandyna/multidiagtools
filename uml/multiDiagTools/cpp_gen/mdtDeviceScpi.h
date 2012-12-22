#ifndef _MDTDEVICESCPI_H
#define _MDTDEVICESCPI_H


#include "mdtUsbPortManager.h"

class mdtDeviceScpi : public mdtDevice {
  public:
    mdtDeviceScpi(const QObject & parent);

    ~mdtDeviceScpi();

    int sendCommand(const QByteArray & command, int timeout);

    QByteArray sendQuery(const QByteArray & query, int writeTimeout, int readTimeout);


  private:
    mdtUsbPortManager pvUsbtmcPortManager;


  public:
    int waitOperationComplete(int timeout);

};
#endif
