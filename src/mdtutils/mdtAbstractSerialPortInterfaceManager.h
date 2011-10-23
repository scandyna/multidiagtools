#ifndef MDT_ABSTRACT_SERIALPORT_INTERFACE_MANAGER_H
#define MDT_ABSTRACT_SERIALPORT_INTERFACE_MANAGER_H

#include "mdtSerialPortInterface.h"
#include <QString>
#include <QList>

class mdtAbstractSerialPortInterfaceManager
{
 public:

  mdtAbstractSerialPortInterfaceManager();
  ~mdtAbstractSerialPortInterfaceManager();

  bool scan();

 protected:

  QList<mdtSerialPortInterface*> pvInterfaces;

};
#endif  // #ifndef MDT_ABSTRACT_SERIALPORT_INTERFACE_MANAGER_H
