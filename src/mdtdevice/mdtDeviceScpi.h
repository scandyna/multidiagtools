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
#ifndef MDT_DEVICE_SCPI_H
#define MDT_DEVICE_SCPI_H

#include "mdtDevice.h"
#include "mdtUsbtmcPortManager.h"

/*! \brief Base class for SCPI based instruments
 *
 * Most of current instruments (Agilent, NI, ...)
 *  support SCPI command format.
 *
 * This class give some common functions
 *  requierd to handle such devices.
 *
 * Currently, only USBTMC port is supported.
 */
class mdtDeviceScpi : public mdtDevice
{
 Q_OBJECT

 public:

  /*! \brief Construct a SCPI device
   *
   * Will create and configure a USBTMC
   *  port manager.
   */
  mdtDeviceScpi(QObject *parent = 0);

  virtual ~mdtDeviceScpi();

  /*! \brief Get internal port manager instance
   */
  mdtPortManager *portManager();

  /*! \brief Search and connect to physical device.
   *
   * Will scan available ports and open the first port that
   *  has device attached maching request.
   *
   * \param devInfo Requested device's informations.
   * \return A error listed in mdtAbstractPort::error_t (NoError on success)
   */
  mdtAbstractPort::error_t connectToDevice(const mdtDeviceInfo &devInfo);

  /*! \brief Send a command to device
   *
   * Wait until it's possible to write to device,
   *  then send the command.
   *
   * Note that the wait will not break the GUI's event loop
   *  (see mdtPortManager::waitOnWriteReady() for details).
   *
   * \param command Command to send.
   * \param timeout Write timeout [ms]
   * \return bTag on success, or a error < 0
   *          (see mdtUsbtmcPortManager::writeData() for details).
   */
  int sendCommand(const QByteArray &command, int timeout = 1000);

  /*! \brief Send a query to device
   *
   * Wait until it's possible to write to device,
   *  send the query and wait until a response
   *  is available or timeout.
   *
   * Note that the wait will not break the GUI's event loop.
   *
   * This method will not update internal I/O states.
   *  It's recommended to use mdtDevice functions if available.
   *
   * \param query Query to send.
   * \param writeTimeout Write timeout [ms]
   * \param readTimeout Response timeout [ms]
   * \return Result as string (empty string on error)
   *          Note that mdtFrameCodecScpi can be helpful to decode returned result.
   */
  QByteArray sendQuery(const QByteArray &query, int writeTimeout = 1000, int readTimeout = 30000);

  /*! \brief Wait until operation is complete (Using *OPC? query)
   *
   * Currently, this method queries device with *OPC?.
   *
   * \param timeout Timeout [ms] Note that timeout must be a multiple of internal.
   * \param internal Interval between 2 *OPC? query [ms]
   * \pre internal > 0
   */
  bool waitOperationComplete(int timeout, int interval);

 private:

  mdtUsbtmcPortManager *pvUsbtmcPortManager;
};

#endif  // #ifndef MDT_DEVICE_SCPI_H
