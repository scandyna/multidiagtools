#ifndef MDT_SERIAL_PORT_MANAGER_H
#define MDT_SERIAL_PORT_MANAGER_H

#include <QtGlobal>
#include <QStringList>

#include "mdtAbstractSerialPort.h"
#include "mdtSerialPort.h"

#include "mdtPortManager.h"

class mdtSerialPortManager : public mdtPortManager
{
 public:

  mdtSerialPortManager(QObject *parent = 0);
  ~mdtSerialPortManager();

  /*! \brief Scan for available serial ports
   * 
   * Try to open available ports on system and get some attributes.
   * If both steps works, the found port is considered existing.
   * \return List of available serial ports on system (f.ex: /dev/ttyS0 on Unix , or COM1 on Windows)
   */
  QStringList scan();
};

#endif  // #ifndef MDT_SERIAL_PORT_MANAGER_H
