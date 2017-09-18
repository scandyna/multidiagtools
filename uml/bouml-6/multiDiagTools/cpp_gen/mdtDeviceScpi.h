#ifndef _MDTDEVICESCPI_H
#define _MDTDEVICESCPI_H


class mdtDeviceInfo;

class mdtDeviceScpi : public mdtDevice {
  public:
    mdtDeviceScpi(const QObject & parent);

    ~mdtDeviceScpi();

    mdtAbstractPort::error_t connectToDevice(const mdtDeviceInfo & devInfo);

    int sendCommand(const QByteArray & command, int timeout);

    QByteArray sendQuery(const QByteArray & query, int writeTimeout, int readTimeout);

    int waitOperationComplete(int timeout);


  private:
    mdtUsbtmcPortManager pvUsbtmcPortManager;

};
#endif
