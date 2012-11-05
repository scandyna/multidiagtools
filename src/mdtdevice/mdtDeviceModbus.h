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
#ifndef MDT_DEVICE_MODBUS_H
#define MDT_DEVICE_MODBUS_H

#include "mdtDevice.h"
#include <QMap>
#include <QVariant>
#include <QList>
#include <QByteArray>

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

  virtual ~mdtDeviceModbus();

  /*! \brief Set the number of analog inputs
   *
   * \param count Number of analog inputs
   * \pre count must be in range from 1 to 125
   */
  ///void setupAnalogInputs(int count);

  /*! \brief Get the value of a analog input
   *
   * When delaying the request, the currently stored value is returned.
   *
   * \param address Address as describe in MODBUS specification
   *                 (first input of a node has address 0, second has address 1, ...)
   *                 If address is out of range, a invalid value is returned (see setupAnalogInputs() ).
   * \param readDirectly If true, the request is sent to device directly, else it can be sent with readAnalogInputs()
   * \return The value of requested input, or an invalid value on error.
   */
  QVariant getAnalogInputValue(int address, bool readDirectly);

  /*! \brief Read all analog inputs on physical device and update (G)UI representation
   *
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's container must be set with setIos()
   */
  int readAnalogInputs();

  /*! \brief Set the number of analog outputs
   *
   * \param count Number of analog outputs
   * \pre count must be in range from 1 to 123
   */
  void setupAnalogOutputs(int count);

  /*! \brief Set the value of a analog output
   *
   * \param address Address as describe in MODBUS specification
   *                 (first output of a node has address 0, second has address 1, ...)
   *                 If address is out of range, a invalid value is returned (see setupAnalogOutputs() ).
   * \param readDirectly If true, the request is sent to device directly, else it can be sent with writeAnalogOutputs()
   * \return True on success.
   */
  bool setAnalogOutput(int address, int value, bool writeDirectly);

  /*! \brief Get the value of a analog output
   *
   * \param address Address as describe in MODBUS specification
   *                 (first output of a node has address 0, second has address 1, ...)
   *                 If address is out of range, a invalid value is returned (see setupAnalogOutputs() ).
   * \param readDirectly If true, the request is sent to device directly, else it can be sent with writeAnalogOutputs()
   * \return The value of requested output, or an invalid value on error.
   */
  virtual QVariant getAnalogOutputValue(int address, bool readDirectly);

  /*! \brief Write analog output values to device
   *
   * \return True on success.
   */
  bool writeAnalogOutputs();

  /*! \brief Request values from device
   *
   * Request values of all analog outputs.
   *
   * \todo Offset sur adresse départ selon marque
   *
   * \return True on success.
   */
  virtual bool readAnalogOutputs();

  /*! \brief Set the number of digital inputs
   *
   * \param count Number of digital inputs
   * \pre count must be in range from 1 to 2000
   */
  void setupDigitalInputs(int count);

  /*! \brief Get the state of a digital input
   *
   * When delaying the request, the currently stored state is returned.
   *
   * \param address Address as describe in MODBUS specification
   *                 (first input of a node has address 0, second has address 1, ...)
   *                 If address is out of range, a invalid value is returned (see setupDigitalInputs() ).
   * \param readDirectly If true, the request is sent to device directly, else it can be sent with readDigitalInputs()
   * \return The state of requested input, or an invalid value on error.
   */
  QVariant getDigitalInputState(int address, bool readDirectly);

  /*! \brief Request states from device
   *
   * Request states of all digital inputs.
   *
   * \return True on success.
   */
  bool readDigitalInputs();

  /*! \brief Set the number of digital outputs
   *
   * \param count Number of digital outputs
   * \pre count must be in range from 1 to 1968
   */
  void setupDigitalOutputs(int count);

  /*! \brief Set the state of a digital output
   *
   * \param address Address as describe in MODBUS specification
   *                 (first output of a node has address 0, second has address 1, ...)
   *                 If address is out of range, a invalid value is returned (see setupDigitalOutputs() ).
   * \param readDirectly If true, the request is sent to device directly, else it can be sent with writeDigitalOutputs()
   * \return True on success.
   */
  bool setDigitalOutput(int address, bool state, bool writeDirectly);

  /*! \brief Get the state of a digital output
   *
   * When delaying the request, the currently stored state is returned.
   *
   * \param address Address as describe in MODBUS specification
   *                 (first output of a node has address 0, second has address 1, ...)
   *                 If address is out of range, a invalid value is returned (see setupDigitalOutputs() ).
   * \param readDirectly If true, the request is sent to device directly, else it can be sent with readDigitalInputs()
   * \return The state of requested output, or an invalid value on error.
   */
  virtual QVariant getDigitalOutputState(int address, bool readDirectly);

  /*! \brief Write digital output states to device
   *
   * \return True on success.
   */
  bool writeDigitalOutputs();

  /*! \brief Request states from device
   *
   * Request states of all digital outputs.
   *
   * \todo Offset sur adresse départ selon marque
   *
   * \return True on success.
   */
  virtual bool readDigitalOutputs();

 public slots:

  /*! \brief Decode incoming frames
   *
   * \pre I/O's container must be set with setIos()
   *
   * Subclass notes:<br>
   *  - This default implementation does nothing.
   *  - This slot should be connected with mdtPortManager::newReadenFrame() signal.
   *  - In this class, this connection is not made, it is the sublcass responsability to do this.
   *  - The incoming frames are available with mdtPortManager::readenFrames().
   */
  void decodeReadenFrames();

 private:

  mdtModbusTcpPortManager *pvTcpPortManager;
  mdtFrameCodecModbus *pvCodec;
  QList<QByteArray> pvReadenFrames;     // Frames readen from port manager
  ///QMap<quint16,quint16> pvAnalogInputs;
  QMap<quint16,quint16> pvAnalogOutputs;
  QMap<quint16,bool> pvDigitalInputs;
  QMap<quint16,bool> pvDigitalOutputs;
};

#endif  // #ifndef MDT_DEVICE_MODBUS_H
