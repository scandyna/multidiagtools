/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtDeviceIos.h"
#include <QList>
#include <QMap>
#include <QVariant>

class mdtDeviceModbusWagoModule;

/*! \brief
 */
class mdtDeviceModbusWago : public mdtDeviceModbus
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtDeviceModbusWago(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtDeviceModbusWago();

  /*! \brief Connect to physical device
   *
   * Connect to device set with setDeviceAddress().
   *  It will be checked that device is a Wago 750 field bus coupler.
   *  If device address contains a valid hardware node ID,
   *  it will also be checked that it matches, else it will be ignored.
   *
   * \sa mdtDevice::setDeviceAddress().
   */
  bool connectToDevice();

  /*! \brief Connect to physical device with given hardware node ID
   *
   * Will search a MODBUS/TCP device on network that has requested hardware node ID,
   *  starting with cached known hosts.
   *  It will also be checked that device is a Wago 750 field bus coupler.
   *
   * \param hwNodeId Hardware node ID
   * \param alias Alias to give. Will be ignored if empty
   * \param scanTimeout Time to wait during scan before considering device is not reachable [ms]
   * \param port Port. MODBUS/TCP default port is 502.
   */
  bool connectToDevice(const mdtModbusHwNodeId & hwNodeId, const QString & alias = QString(), int scanTimeout = 500, int port = 502);

  /*! \brief Check if device is a Wago 750 series
   */
  bool isWago750();

  /*! \brief Get number of analog outputs
   *
   * Note: calling this method will query the device
   *
   * \return Number of analog outputs, or < 0 on error
   */
  int analogOutputsCount();

  /*! \brief Get number of analog inputs
   *
   * Note: calling this method will query the device
   *
   * \return Number of analog inputs, or < 0 on error
   */
  int analogInputsCount();

  /*! \brief Get number of digital outputs
   *
   * Note: calling this method will query the device
   *
   * \return Number of digital outputs, or < 0 on error
   */
  int digitalOutputsCount();

  /*! \brief Get number of digital inputs
   *
   * Note: calling this method will query the device
   *
   * \return Number of digital inputs, or < 0 on error
   */
  int digitalInputsCount();

  /*! \brief Query device about filed bus + I/Os setup
   *
   * For some special configurable modules, channels count and process image format can vary.
   *  Because some of these parameters cannot be readen from module via MODBUS,
   *  a instance of a module object with corresponding setup must be given as argument.
   *  (It's also possible to specify only the special modules. Internally, 
   *   setup regarding register word returned from fieldbus coupler will be used for
   *   positions that are not present in the map).
   *
   * \param specialModules Map with module position as key and special module object as value.
   *          Note: position 0 is the fieldbus coupler, so module position starts at 1.
   *          Module objects contained in the map are not deleted in this method.
   * \return True on success. A failure is mostly due to a communication error, an unsupported
   *          fieldbus coupler, a unsupported module or a mistmatch with a special module configuration.
   */
  bool detectIos(const QMap<int, mdtDeviceModbusWagoModule*> specialModules /** = QMap<int, mdtDeviceModbusWagoModule*>()*/);

  /*! \brief Query device about filed bus + I/Os setup
   */
  bool detectIos()
  {
    return detectIos(QMap<int, mdtDeviceModbusWagoModule*>());
  }

 private:

  Q_DISABLE_COPY(mdtDeviceModbusWago);

  QList<mdtDeviceModbusWagoModule*> pvModules;
};

#endif  // #ifndef MDT_DEVICE_MODBUS_WAGO_H
