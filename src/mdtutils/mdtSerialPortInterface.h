#ifndef MDT_SERIAL_PORT_INTERFACE_H
#define MDT_SERIAL_PORT_INTERFACE_H

#include "mdtSerialPortInterfaceOptAttribute.h"
#include <QString>
#include <QList>

class mdtSerialPortInterface
{
 public:

  // Source: http://lxr.oss.org.cn/source/include/linux/serial.h#L33
  enum sp_uart_type_t { UT_UNKNOWN,
                        UT_8250,
                        UT_16450,
                        UT_16550,
                        UT_16550A,
                        UT_CIRRUS,
                        UT_16650,
                        UT_16650V2,
                        UT_16750,
                        UT_STARTECH
                        // NOTE: to be completed
                      };
   
  mdtSerialPortInterface();
  ~mdtSerialPortInterface();

 private:

  int pvNumber;
  QString pvName;
  QString pvVendorId;
  QString pvVendorName;
  QString pvModelId;
  QList<mdtSerialPortInterfaceOptAttribute*> pvOptAttributes;
  QString pvModelName;
  int pvIRQ;
  QString pvUARTname;
  QList<int> pvAvailableBaudRates;
};

#endif  // #ifndef MDT_SERIAL_PORT_INTERFACE_H
