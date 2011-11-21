#ifndef MDT_ABSTRACT_SERIALPORT_MANAGER_H
#define MDT_ABSTRACT_SERIALPORT_MANAGER_H

#include "mdtAbstractSerialPort.h"
#include "mdtSerialPortInterface.h"
#include "mdtSerialPortConfig.h"
#include <QString>
#include <QList>

class mdtAbstractSerialPortManager
{
 public:

  mdtAbstractSerialPortManager();
  ~mdtAbstractSerialPortManager();

  /*! \brief Scan system and store a list of available devices
   * This method must be implemented in system dependent class
   */
  bool scan();
  
  /*! \brief Set the serial port instance
   */
  void setSerialPortObj(mdtAbstractSerialPort *sp);

 protected:

  QList<mdtSerialPortInterface*> pvInterfaces;
  mdtAbstractSerialPort *pvSerialPort;
  mdtSerialPortConfig pvConfig;
};
#endif  // #ifndef MDT_ABSTRACT_SERIALPORT_MANAGER_H
