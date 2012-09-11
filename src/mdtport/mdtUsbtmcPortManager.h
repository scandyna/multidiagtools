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
#ifndef MDT_USBTMC_PORT_MANAGER_H
#define MDT_USBTMC_PORT_MANAGER_H

///#include "mdtUsbtmcPort.h"
#include "mdtUsbPort.h"
#include "mdtUsbPortThread.h"
#include "mdtPortManager.h"
#include <QObject>
#include <QStringList>

/*! \brief USBTMC port manager
 *
 * This is the easiest way to use the USBTMC port API.
 *
 * All needed object are created by constructor.
 *  To alter configuration, use config().
 *  To access the usb port object, use port().
 *
 * Use start() to begin read/write and stop to end.
 */
class mdtUsbtmcPortManager : public mdtPortManager
{
 Q_OBJECT

 public:

  /*! \brief Construct a USBTMC port manager
   *
   * Creates a mdtPortConfig, a mdtUsbPort,
   *  and thread object.
   */
  mdtUsbtmcPortManager(QObject *parent = 0);

  /*! \brief Destructor
   *
   * Stop the manager (if running), and close the port (if open).
   *  All internal objects (port, config and threads) are deleted here.
   */
  ~mdtUsbtmcPortManager();

  /*! \brief Scan available USBTMC ports
   */
  ///QStringList scan();

  /*! \brief Write data to device
   *
   * By sending data, it's important to tell if
   * the device will return some data back.
   * This is due to large timeout of USBTMC kernel driver
   * whenn no data are available on port.
   * \sa mdtPortManager
   * \sa mdtUsbtmcPort
   */
  ///bool writeData(QByteArray data, bool waitAnswer);
  
  /*! \brief Write data by copy
   *
   * Data will be encoded regarding USBTMC standard and passed to the mdtUsbPort's write queue by copy.
   *  This method returns immediatly after enqueue,
   *  and don't wait until data was written.
   *
   * \param data Data to write
   * \return True on success.
   */
  bool writeData(QByteArray data);

  /*! \brief Wait until a complete frame is available
   *
   * Send a read request and call mdtPortManager::waitReadenFrame() .
   *
   * \param timeout Maximum wait time [ms]. Must be a multiple of 50 [ms]
   * \return True if Ok, false on timeout
   */
  bool waitReadenFrame(int timeout = 500);

  QByteArray readData();
  
 public slots:

  /*! \brief Called by the read thread whenn a complete frame was readen
   *
   */
  void fromThreadNewFrameReaden();

 private:

  ///bool pvFrameWritten;
  ///bool pvWaitingFrame;  // If true, frameWritten() will enable the read thread flag
  
  quint8 pvCurrentWritebTag;  // USBTMC's frame bTag
};

#endif  // #ifndef MDT_USBTMC_PORT_MANAGER_H
