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
#include "mdtSerialPortCtlThread.h"
#include "mdtPortManager.h"

/*! \brief Port manager base class
 * 
 * This is the easiest way to use the serial port API.<br>
 * 
 * 
 * 
 */
class mdtSerialPortManager : public mdtPortManager
{
 public:

  /*! \brief Contruct a serial port manager
   * 
   * This will create a serial port and config instances
   */
  mdtSerialPortManager(QObject *parent = 0);
  ~mdtSerialPortManager();

  /*! \brief Scan for available serial ports
   * 
   * Try to open available ports on system and get some attributes.
   * If both steps works, the found port is considered existing.
   * \return List of available serial ports on system (f.ex: /dev/ttyS0 on Unix , or COM1 on Windows)
   */
  QStringList scan();

  /*! \brief Set the port
   * Affects port to the line control thread,
   * and calls mdtPortManager::setPort()
   */
  void setPort(mdtSerialPort *port);

  /*! \brief Open the port
   * 
   * \return True on success, false else
   */
  bool openPort();

  /*! \brief Start threads
   * 
   * Start the line control thread, then calls mdtPortManager::start()
   * \sa mdtPortManager
   */
  bool start();

  /*! \brief Stop threads
   * 
   * Calls mdtPortManager::stop() and stop the line control thread.
   * \sa mdtPortManager
   */
  void stop();

  /*! \brief Get the config object
   * 
   * Usefull to alter internal port configuration
   */
  mdtSerialPortConfig &config();

  /*! \brief Get the serial port instance
   * \pre Port must be set with a mdtAbstractSerialPort instance (not mdtAbstractPort)
   */
  mdtAbstractSerialPort &port();
  
 private:

  // Diseable copy
  mdtSerialPortManager(mdtSerialPortManager &other);
  
  mdtSerialPortCtlThread pvCtlThread;
};

#endif  // #ifndef MDT_SERIAL_PORT_MANAGER_H
