/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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

#include "mdtDeviceScpi.h"
#include "mdtUsbtmcPortManager.h"
#include "mdtFrameCodecScpiU3606A.h"

/*! \brief Representation of a Agilent U3606A
 */
class mdtDeviceU3606A : public mdtDeviceScpi
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
   *                       Optionnaly, a serial ID can be set (usefull if many U3606A devices are connected)
   * \return A error listed in mdtAbstractPort::error_t (NoError on success)
   */
  mdtAbstractPort::error_t connectToDevice(const mdtDeviceInfo &devInfo);

 public slots:

  /*! \brief Update (G)UI when device's state has changed
   */
  void onStateChanged(int state);

 private slots:

  /*! \brief Decode incoming frames
   *
   * \pre I/O's container must be set with setIos()
   */
  void decodeReadenFrame(mdtPortTransaction *transaction);

 private:

  /*! \brief Read one analog input on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has analog inputs, this method should be implemented.
   *
   * This method is called from getAnalogInput().
   *
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details).
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  int readAnalogInput(mdtPortTransaction *transaction);

  /*! \brief Sequence of queries to send periodically
   *
   * This method is called from runQueries().
   *
   * \return true if all queries are sent successfully.
   *
   * Subclass notes:<br>
   *  - This default implementation does nothing and allways returns false.
   *  - This method can be reimplemented periodic queries must be sent to device.
   */
  bool queriesSequence();

  /*! \brief Handle device's specific error
   *
   * This method is called from handleDeviceError().
   */
  ///void handleDeviceSpecificError();

  Q_DISABLE_COPY(mdtDeviceU3606A);

  mdtFrameCodecScpiU3606A *pvCodec;
};

#endif  // #ifndef MDT_DEVICE_U3606A_H
