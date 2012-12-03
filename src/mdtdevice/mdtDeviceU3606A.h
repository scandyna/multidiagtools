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

  /*! \brief Decode incoming frames
   *
   * \pre I/O's container must be set with setIos()
   *
   * Subclass notes:<br>
   *  - This slot should be connected with mdtPortManager::newReadenFrame() signal.
   *  - The incoming frames are available with mdtPortManager::readenFrames().
   */
  void decodeReadenFrame(int id, QByteArray data);

  /*! \brief Update (G)UI when device's state has changed
   */
  void onStateChanged(int state);

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
  int readAnalogInput(int address);

  mdtUsbtmcPortManager *pvPortManager;
  mdtFrameCodecScpiU3606A *pvCodec;

};

#endif  // #ifndef MDT_DEVICE_U3606A_H
