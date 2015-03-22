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
#ifndef MDT_DEVICE_MODBUS_H
#define MDT_DEVICE_MODBUS_H

#include "mdtMultiIoDevice.h"
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
class mdtDeviceModbus : public mdtMultiIoDevice
{
 Q_OBJECT

 public:

  mdtDeviceModbus(QObject *parent = 0);
  virtual ~mdtDeviceModbus();

  /*! \brief Get internal port manager instance
   */
  mdtPortManager *portManager();

  /*! \brief Get internal port manager instance
   */
  mdtModbusTcpPortManager *modbusTcpPortManager();

  /*! \brief Connect to physical device
   *
   * Connect to device set with setDeviceAddress().
   *  Depending on device address that was set, we have several cases:
   *  - For a complete address, including host, port and MODBUS HW node ID,
   *    a connection will be established to given host and port.
   *    If connection succeed, it will also be checked that MODBUS HW node ID matches.
   *  - For a address that contains only host and port,
   *    a connection is established to given host and port.
   *  - For a address that contains only MODBUS HW node ID,
   *    a MODBUS/TCP device that has matching MODBUS HW node ID will be searched on the network.
   *
   * \sa mdtDevice::setDeviceAddress().
   */
  virtual bool connectToDevice();

  /*! \brief Connect to physical device with given hardware node ID
   *
   * Will search a MODBUS/TCP device on network that has requested hardware node ID,
   *  starting with cached known hosts.
   *
   * \param hwNodeId Hardware node ID
   * \param alias Alias to give. Will be ignored if empty
   * \param scanTimeout Time to wait during scan before considering device is not reachable [ms]
   * \param port Port. MODBUS/TCP default port is 502.
   */
  virtual bool connectToDevice(const mdtModbusHwNodeId & hwNodeId, const QString & alias = QString(), int scanTimeout = 500, int port = 502);

  /*! \brief Query device about filed bus + I/Os setup
   *
   * I/O detection is device vendor specific.
   *  Some subclass reimplement this function.
   *
   * This implementation does nothing and allways return false.
   */
  virtual bool detectIos();

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

  /*! \brief Helper method for register service
   *
   * Usefull to set register values (f.ex. configurations regsisters, ...).
   *
   * Note: to set analog I/O values, the mdtDevice API should be used.
   *
   * \return True on success.
   * \pre startAddress must be >= 0
   * \sa mdtModbusTcpPortManager::setRegisterValues()
   */
  bool setRegisterValues(int startAddress, QList<int> &values);

 private slots:

  /*! \brief Decode incoming frames
   *
   * \pre I/O's container must be set with setIos()
   */
  void decodeReadenFrame(mdtPortTransaction *transaction);

  /*! \brief Update (G)UI when device's state has changed
   */
  ///void onStateChanged(int state);

  /*! \brief Set the device state from port manager state
   *
   * Internal port manager handles a state machine.
   *  When a event occurs, it will emit mdtPortManager::stateChanged().
   *  If port manager's signal is connected to this slot,
   *  some things are made in mdtDevice, and
   *  stateChanged() will be emitted.
   */
  void setStateFromPortManager(int portManagerState);

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
  int writeAnalogOutputs(mdtPortTransaction *transaction, mdtDeviceIosSegment *segment);

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
  int writeDigitalOutputs(mdtPortTransaction *transaction, mdtDeviceIosSegment *segment);

  // Sequence of periodic queries
  ///bool queriesSequence();

  mdtModbusTcpPortManager *pvTcpPortManager;
  mdtFrameCodecModbus *pvCodec;

 private:

  /*! \brief Search MODBUS/TCP device that has given MODBUS HW node ID
   * \todo Set private
   */
  mdtDeviceAddress searchDeviceWithModbusHwNodeId(const mdtModbusHwNodeId & hwNodeId, int scanTimeout = 500, int port = 502);

  /*! \brief Called by disconnectFromDevice()
   */
  void disconnectFromDeviceEvent();

  Q_DISABLE_COPY(mdtDeviceModbus);
};

#endif  // #ifndef MDT_DEVICE_MODBUS_H
