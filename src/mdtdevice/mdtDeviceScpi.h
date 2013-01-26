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
#include <QList>
#include <QVariant>

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
   * This method will not update internal device state.
   *  It's recommended to use mdtDevice functions if available.
   *
   * \param command Command to send.
   * \param timeout Write timeout [ms]
   *           If 0, internal defined timeout is used (see mdtPortManager::adjustedWriteTimeout() for details).
   * \return bTag on success, or a error < 0
   *          (see mdtUsbtmcPortManager::writeData() for details).
   */
  int sendCommand(const QByteArray &command, int timeout = 0);

  /*! \brief Send a query to device
   *
   * Wait until it's possible to write to device,
   *  send the query and wait until a response
   *  is available or timeout.
   *
   * Note that the wait will not break the GUI's event loop.
   *
   * This method will not update internal device and I/O states.
   *  It's recommended to use mdtDevice functions if available.
   *
   * \param query Query to send.
   *      \todo adjust write timeout
   * \param writeTimeout Write timeout [ms]
   *                 If 0, internal defined timeout is used (see mdtPortManager::adjustedWriteTimeout() for details).
   * \param readTimeout Response timeout [ms]
   *                 If 0, internal defined timeout is used (see mdtPortManager::adjustedReadTimeout() for details).
   * \return Result as string (empty string on error)
   *          Note that mdtFrameCodecScpi can be helpful to decode returned result.
   */
  QByteArray sendQuery(const QByteArray &query, int writeTimeout = 0, int readTimeout = 0);

  /*! \brief Wait until operation is complete (Using *OPC? query)
   *
   * Currently, this method queries device with *OPC?.
   *
   * \param timeout Timeout [ms]
   * \param interval Interval between 2 *OPC? query [ms]
   * \pre interval > 0
   */
  bool waitOperationComplete(int timeout, int interval);

  /*! \brief Query device about error
   *
   * Will send the SYST:ERR? and return.
   *
   * Subclass notes:<br>
   * The SYST:ERR? query is sent to device.
   *  A new transaction is initalized, with type
   *  MDT_FC_SCPI_ERR.
   *  When subclass receives such transaction type,
   *  it should call handleDeviceError() without decoding frame.
   *
   * \return 0 if query could be sent or a error of type mdtAbstractPort::error_t else.
   *          Note: returned error is not device's error.
   */
  int checkDeviceError();

 protected:

  /*! \brief Handle device error
   *
   * This method is called from subclass's
   *  decodeReadenFrame(mdtPortTransaction) method
   *  when a frame of type MDT_FC_SCPI_ERR was received.
   *
   * Errors <= 0 are handled directly.
   *  If error is > 0, handleDeviceSpecificError() is called.
   *
   * See the SCPI99 standard for more details.
   *
   * \param decodedValues See mdtFrameCodecScpi::decodeError().
   * \post If internal codec has < 2 values, handleDeviceSpecificError() is not called.
   */
  void handleDeviceError(QList<QVariant> &decodedValues);

  /*! \brief Handle device's specific error
   *
   * This method is called from handleDeviceError().
   *
   * This default implementation simply
   *  log the error (with mdtError system).
   *
   * \param errNum Error number
   * \param errText Error message
   * \param deviceSpecificTexts Device's specific informations
   * \pre Internal codec has at least 2 values.
   */
  virtual void handleDeviceSpecificError(int errNum, const QString &errText, const QList<QString> &deviceSpecificTexts);

  /*! \brief Log a device error with mdtError system
   * \param errNum Error number
   * \param errText Error message
   * \param deviceSpecificTexts Device's specific informations
   */
  void logDeviceError(int errNum, const QString &errText, const QList<QString> &deviceSpecificTexts, mdtError::level_t level);

  mdtUsbtmcPortManager *pvUsbtmcPortManager;

 private:

  bool pvOperationComplete;
};

#endif  // #ifndef MDT_DEVICE_SCPI_H
