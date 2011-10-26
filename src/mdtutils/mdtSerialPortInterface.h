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
                        UT_STARTECH,
                        UT_16C950,
                        UT_16654,
                        UT_16850,
                        UT_RSA
                      };
   
  mdtSerialPortInterface();
  ~mdtSerialPortInterface();
  
  /*! \brief Define UART type
   * */
  void setUartType(sp_uart_type_t type);

  /*! \brief Get UART type
   * */
  sp_uart_type_t uartType();
  
  /*! \brief Get UART type string
   */
  QString uartTypeStr();

  /*! \brief Set port number
   */
  void setPortNumber(int portNumber);
  
  /*! \brief Get port number
   */
  int portNumber();

  /*! \brief Set port name (for.ex: ttyS0 on Posix, or COM1 on Windows)
   */
  void setPortName(const QString &portName);

  /*! \brief Get port name (for.ex: ttyS0 on Posix, or COM1 on Windows)
   */
  QString portName();

  /*! \brief Define the available baud rates 
   */
  void setAvailableBaudRates(const QList<int> &availableBaudRates);

  /*! \brief Get available baud rates list
   */
  QList<int> availableBaudRates();
  
 private:

  int pvPortNumber;
  QString pvPortName;
  QString pvVendorId;
  QString pvVendorName;
  QString pvModelId;
  QList<mdtSerialPortInterfaceOptAttribute*> pvOptAttributes;
  QString pvModelName;
  int pvIRQ;
  sp_uart_type_t pvUartType;
  QString pvUartTypeStr;
  QList<int> pvAvailableBaudRates;
};

#endif  // #ifndef MDT_SERIAL_PORT_INTERFACE_H
