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
   * This method can be reimplemented in subclass if specific
   *  tasks must be done during connection .
   *
   * \param portInfo mdtPortInfo object . The portName attribute of portInfo must contain host:port format (or ip:port) .
   * \return A error listed in mdtAbstractPort::error_t (NoError on success) .
   */
  mdtAbstractPort::error_t connectToDevice(const mdtPortInfo & portInfo);

  /*! \brief Search and connect to physical device.
   *
   * \sa mdtDeviceModbus::connectToDevice(const mdtDeviceInfo&) .
   */
  mdtAbstractPort::error_t connectToDevice(const mdtDeviceInfo &devInfo);

  /*! \brief Search and connect to physical device.
   *
   * Work as mdtDeviceModbus::connectToDevice() , but checks that device is a Wago 750 fieldbus coupler.
   */
  mdtAbstractPort::error_t connectToDevice(const QList<mdtPortInfo*> &scanResult, int hardwareNodeId, int bitsCount, int startFrom = 0);

  /*! \brief Search and connect to physical device.
   *
   * See mdtDeviceModbus::connectToDevice(const QList<int>&) .
   */
  mdtAbstractPort::error_t connectToDevice(const QList<int> & existingHwNodeIdList);

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
   * The detected I/Os are added to given ios,
   *  but not added to internal ios container (use setIos() for this).
   *  Goal is to be able to detect setup without alter the internal container,
   *  so a check could be made before running really, or something else.
   *
   * For some special configurable modules, channels count and process image format can vary.
   *  Because some of these parameters cannot be readen from module via MODBUS,
   *  a instance of a module object with corresponding setup must be given as argument.
   *  (It's also possible to specify only the special modules. Internally, 
   *   setup regarding register word returned from fieldbus coupler will be used for
   *   positions that are not present in the map).
   *
   * \param ios I/O container in witch the result will be stored.
   * \param specialModules Map with module position as key and special module object as value.
   *          Note: position 0 is the fieldbus coupler, so module position starts at 1.
   *          Module objects contained in the map are not deleted in this method.
   * \return True on success. A failure is mostly due to a communication error, an unsupported
   *          fieldbus coupler, a unsupported module or a mistmatch with a special module configuration.
   */
  bool detectIos(mdtDeviceIos *ios, const QMap<int, mdtDeviceModbusWagoModule*> specialModules = QMap<int, mdtDeviceModbusWagoModule*>());

 private:

  Q_DISABLE_COPY(mdtDeviceModbusWago);

  QList<mdtDeviceModbusWagoModule*> pvModules;
};

#endif  // #ifndef MDT_DEVICE_MODBUS_WAGO_H
