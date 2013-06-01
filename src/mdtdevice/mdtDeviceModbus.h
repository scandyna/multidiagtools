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
#ifndef MDT_DEVICE_MODBUS_H
#define MDT_DEVICE_MODBUS_H

#include "mdtDevice.h"
#include "mdtPortInfo.h"
#include <QMap>
#include <QVariant>
#include <QList>
#include <QByteArray>
#include <QStringList>

class mdtFrameCodecModbus;
class mdtModbusTcpPortManager;

/*! \brief Represent a MODBUS device
 *
 * References:
 *  - MODBUS Application Protocol Specification V1.1b
 *  - http://www.Modbus-IDA.org
 */
class mdtDeviceModbus : public mdtDevice
{
 Q_OBJECT

 public:

  mdtDeviceModbus(QObject *parent = 0);
  ~mdtDeviceModbus();

  /*! \brief Get internal port manager instance
   */
  mdtPortManager *portManager();

  /*! \brief Get internal port manager instance
   */
  mdtModbusTcpPortManager *modbusTcpPortManager();

  /*! \brief Search and connect to physical device.
   *
   * Will try to connect to device listed in scanResult until
   *  hardwareNodeId is found.
   *
   * \param scanResult List of mdtPortInfo containing a port name in format host:port.
   * \param hardwareNodeId See mdtModbusTcpPortManager::getHardwareNodeAddress().
   * \param bitsCount See mdtModbusTcpPortManager::getHardwareNodeAddress().
   * \param startFrom See mdtModbusTcpPortManager::getHardwareNodeAddress().
   * \return A error listed in mdtAbstractPort::error_t (NoError on success).
   * \pre scanResult must contain valid pointers.
   * \pre Internal port manager not runnig and port must be closed before calling this method.
   */
  mdtAbstractPort::error_t connectToDevice(const QList<mdtPortInfo*> &scanResult, int hardwareNodeId, int bitsCount, int startFrom = 0);

  /*! \brief Helper method for register service
   *
   * Usefull to get resgister values (f.ex. configurations regsisters, ...).
   *
   * Note: to get analog I/O values, the mdtDevice API should be used.
   *
   * \return True on success. Values are the available with registerValues()
   * \pre address and n must be > 0
   * \sa mdtModbusTcpPortManager::getRegisterValues()
   */
  bool getRegisterValues(int address, int n);

  /*! \brief Helper method for register service
   *
   * Return result set by getRegisterValues().
   * Note that values are keeped until next call of getRegisterValues().
   *
   * \sa mdtModbusTcpPortManager::registerValues()
   */
  const QList<int> &registerValues() const;

 private slots:

  /*! \brief Decode incoming frames
   *
   * \pre I/O's container must be set with setIos()
   */
  void decodeReadenFrame(mdtPortTransaction *transaction);

  /*! \brief Update (G)UI when device's state has changed
   */
  ///void onStateChanged(int state);

 protected:

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

  /*! \brief Read all analog inputs on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has analog inputs, this method should be implemented.
   *
   * This method is called from getAnalogInputs().
   *
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  int readAnalogInputs(mdtPortTransaction *transaction);

  /*! \brief Read one analog output on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has analog inputs, this method should be implemented.
   *
   * This method is called from getAnalogOutputValue().
   *
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  int readAnalogOutput(mdtPortTransaction *transaction);

  /*! \brief Read all analog outputs on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has analog inputs, this method should be implemented.
   *
   * This method is called from getAnalogOutputValue().
   *
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int readAnalogOutputs(mdtPortTransaction *transaction);

  /*! \brief Write value on a analog output to physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has analog outputs, this method should be implemented.
   *
   * This method is called from setAnalogOutputValue().
   *
   * \param value Value encoded regarding device format.
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int writeAnalogOutput(int value, mdtPortTransaction *transaction);

  /*! \brief Write all analog outputs to physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has analog outputs, this method should be implemented.
   *
   * This method is called from setAnalogOutputs().
   *
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  int writeAnalogOutputs(mdtPortTransaction *transaction);

  /*! \brief Read one digital input on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has digital inputs, this method should be implemented.
   *
   * This method is called from getDigitalInputState().
   *
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  int readDigitalInput(mdtPortTransaction *transaction);

  /*! \brief Read all digital inputs on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has digital inputs, this method should be implemented.
   *
   * This method is called from getDigitalInputs().
   *
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  int readDigitalInputs(mdtPortTransaction *transaction);

  /*! \brief Read one digital output on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has digital outputs, this method should be implemented.
   *
   * This method is called from getDigitalOutputState().
   *
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  int readDigitalOutput(mdtPortTransaction *transaction);

  /*! \brief Read all digital outputs on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has digital outputs, this method should be implemented.
   *
   * This method is called from getDigitalOutputs().
   *
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  int readDigitalOutputs(mdtPortTransaction *transaction);

  /*! \brief Write state on a digital output to physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has digital outputs, this method should be implemented.
   *
   * This method is called from setDigitalOutputState().
   *
   * \param state State (ON/OFF).
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  int writeDigitalOutput(bool state, mdtPortTransaction *transaction);

  /*! \brief Write all digital outputs to physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has digital outputs, this method should be implemented.
   *
   * This method is called from setDigitalOutputs().
   *
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  int writeDigitalOutputs(mdtPortTransaction *transaction);

  // Sequence of periodic queries
  bool queriesSequence();

  mdtModbusTcpPortManager *pvTcpPortManager;
  mdtFrameCodecModbus *pvCodec;
};

#endif  // #ifndef MDT_DEVICE_MODBUS_H
