#ifndef _MDTSERIALPORTINTERFACEMANAGER_H
#define _MDTSERIALPORTINTERFACEMANAGER_H


#include "mdtSerialPortInterfaceManagerSys.h"

class mdtSerialPortConfig;

class mdtSerialPortInterfaceManager : public mdtSerialPortInterfaceManagerSys {
  private:
    mdtSerialPortConfig * ;


  public:
     mdtSerialPortInterfaceManager();

     ~mdtSerialPortInterfaceManager();

};
#endif
