/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_SERIAL_PORT_MANAGER_H
#define MDT_SERIAL_PORT_MANAGER_H

#include <QtGlobal>
#include <QStringList>

#include "mdtAbstractSerialPort.h"
#include "mdtSerialPort.h"
#include "mdtSerialPortConfig.h"
#include "mdtPortWriteThread.h"
#include "mdtPortReadThread.h"
#include "mdtSerialPortCtlThread.h"
#include "mdtPortManager.h"


/*! \brief Serial port manager class
 *
 * This is the easiest way to use the serial port API.
 *
 * All needed object are created by constructor.
 *  To alter configuration, use config().
 *  To access the serial port object, use port().
 *
 * Use start() to begin read/write and stop to end.
 */
class mdtSerialPortManager : public mdtPortManager
{
 public:

  /*! \brief Construct a serial port manager
   *
   * Creates a mdtSerialPortConfig, a mdtSerialPort,
   *  and threads objects.
   */
  mdtSerialPortManager(QObject *parent = 0);

  /*! \brief Destructor
   *
   * Stop the manager (if running), and close the port (if open).
   *  All internal objects (port, config and threads) are deleted here.
   */
  ~mdtSerialPortManager();

  /*! \brief Scan for available serial ports
   * 
   * Try to open available ports on system and get some attributes.
   * If both steps works, the found port is considered existing.
   *
   * \pre Manager must no running.
   * \return List of available serial ports on system (f.ex: /dev/ttyS0 on Unix , or COM1 on Windows)
   */
  QList<mdtPortInfo*> scan();

  /*! \brief Get the config object
   * 
   * Usefull to alter internal port configuration
   */
  mdtSerialPortConfig &config();

  /*! \brief Get the serial port instance
   *
   * \pre Port must be set with a mdtAbstractSerialPort instance (not mdtAbstractPort)
   */
  mdtAbstractSerialPort &port();

  /*! \brief Get the write thread object
   */
  mdtPortWriteThread *writeThread();

  /*! \brief Get the read thread object
   */
  mdtPortReadThread *readThread();

  /*! \brief Get the modem lines thread object
   */
  mdtSerialPortCtlThread *ctlThread();

 private:

  // Diseable copy
  Q_DISABLE_COPY(mdtSerialPortManager);

  // Threads
  mdtPortWriteThread *pvWriteThread;
  mdtPortReadThread *pvReadThread;
  mdtSerialPortCtlThread *pvCtlThread;
};

#endif  // #ifndef MDT_SERIAL_PORT_MANAGER_H
