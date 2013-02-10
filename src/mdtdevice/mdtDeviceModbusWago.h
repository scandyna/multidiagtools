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
#ifndef MDT_DEVICE_MODBUS_WAGO_H
#define MDT_DEVICE_MODBUS_WAGO_H

#include "mdtDeviceModbus.h"
#include "mdtDeviceInfo.h"
#include <QList>

/*! \brief
 */
class mdtDeviceModbusWago : public mdtDeviceModbus
{
 Q_OBJECT

 public:

  mdtDeviceModbusWago(QObject *parent = 0);

  ~mdtDeviceModbusWago();

  /*! \brief Search and connect to physical device.
   *
   * Will scan available ports and open the first port that
   *  has device attached maching request.
   *
   * Note: in current version, only Wago 750 system is supported.
   *
   * \param devInfo Requested device's informations (can be empty, no information is used in current version).
   * \return A error listed in mdtAbstractPort::error_t (NoError on success)
   */
  mdtAbstractPort::error_t connectToDevice(const mdtDeviceInfo &devInfo);

  /*! \brief Check if device is a Wago 750 series
   */
  bool isWago750();
  
  void sandBox();

 private:

  // Helper members for Register service
  bool getRegisterValues(int address, int n);
  QList<int> pvRegisterValues;
};

#endif  // #ifndef MDT_DEVICE_MODBUS_WAGO_H
