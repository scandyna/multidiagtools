#ifndef MDT_SERIAL_PORT_INTERFACE_OPT_ATTRIBUTE_H
#define MDT_SERIAL_PORT_INTERFACE_OPT_ATTRIBUTE_H

#include <QString>

class mdtSerialPortInterfaceOptAttribute
{
 public:

  mdtSerialPortInterfaceOptAttribute();
  ~mdtSerialPortInterfaceOptAttribute();

 private:

  QString pvName;
  QString pvParameter;
};

#endif  // #ifndef MDT_SERIAL_PORT_INTERFACE_OPT_ATTRIBUTE_H
