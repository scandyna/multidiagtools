#ifndef _MDTABSTRACTSERIALPORTINTERFACEMANAGER_H
#define _MDTABSTRACTSERIALPORTINTERFACEMANAGER_H


class mdtSerialPortInterface;

class mdtAbstractSerialPortInterfaceManager {
  private:
    QList<mdtSerialPortInterface> pvInterfaces;


  public:
     mdtAbstractSerialPortInterfaceManager();

     ~mdtAbstractSerialPortInterfaceManager();

    bool scan();

};
#endif
