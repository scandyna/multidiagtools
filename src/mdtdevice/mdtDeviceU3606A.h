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
#ifndef MDT_DEVICE_U3606A_H
#define MDT_DEVICE_U3606A_H

#include "mdtDevice.h"
#include "mdtUsbtmcPortManager.h"
#include "mdtFrameCodecScpiU3606A.h"

/*! \brief Representation of a Agilent U3606A
 */
class mdtDeviceU3606A : public mdtDevice
{
 Q_OBJECT

 public:

  mdtDeviceU3606A(QObject *parent = 0);
  ~mdtDeviceU3606A();

  /*! \brief Search and connect to physical device.
   *
   * Will scan available ports and open the first port that
   *  has device attached maching request.
   *
   * \param devInfo Requested device's informations.
   *                 Note: Vendor ID and/or product ID are automatically set.
   *                       (in other words, a empty mdtDeviceInfo can be passed).
   *                       Optionnaly, a serial device can be set (usefull if many U3606A devices are connected)
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

 public slots:

  /*! \brief Update (G)UI when device's state has changed
   */
  void onStateChanged(int state);

 private slots:

  /*! \brief Decode incoming frames
   *
   * \pre I/O's container must be set with setIos()
   *
   * Subclass notes:<br>
   *  - This slot should be connected with mdtPortManager::newReadenFrame() signal.
   *  - The incoming frames are available with mdtPortManager::readenFrames().
   */
  void decodeReadenFrame(int id, QByteArray data);
  void decodeReadenFrame(mdtPortTransaction transaction);

 private:

  /*! \brief Read one analog input on physical device
   *
   * This method is called from getAnalogInput().
   *
   * \param address Depending on device organisation and protocol,
   *                 this can be a relative or absolute address (f.ex. MODBUS queries),
   *                 a input number, etc...
   * \return bTag on success, value < 0 on error (see mdtUsbtmcPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int readAnalogInput(int address, mdtAnalogIo *ai, bool enqueueResponse);
  
  /// \todo comment!
  bool waitTransactionDone(int id, int timeout, int granularity = 50);

  mdtUsbtmcPortManager *pvPortManager;
  mdtFrameCodecScpiU3606A *pvCodec;
  ///QByteArray pvGenericData;   // Used by sendQuery()
};

#endif  // #ifndef MDT_DEVICE_U3606A_H
