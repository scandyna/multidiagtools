#ifndef _MDTSERIALPORTINTERFACE_H
#define _MDTSERIALPORTINTERFACE_H


class mdtSerialPortInterfaceOptAttribute;

class mdtSerialPortInterface {
  private:
    int pvNumber;

    QString pvName;

    QString pvVendorId;

    QString pvVendorName;

    QString pvModelId;

    QList<mdtSerialPortInterfaceOptAttribute> pvOptAttributes;

    QString pvModelName;

    int pvIRQ;

    QString pvUARTname;

    QList<int> pvAvailableBaudRates;


  public:
     mdtSerialPortInterface();

     ~mdtSerialPortInterface();

};
#endif
