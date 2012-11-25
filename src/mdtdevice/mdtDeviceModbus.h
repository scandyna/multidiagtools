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
  ~mdtDeviceModbus();

  /*! \brief Get internal port manager instance
   */
  mdtPortManager *portManager();

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
  void decodeReadenFrame(int id, QByteArray pdu);

 private:

  /*! \brief Read one analog input on physical device
   *
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int readAnalogInput(int address);

  /*! \brief Read all analog inputs on physical device
   *
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int readAnalogInputs();

  /*! \brief Read one analog output on physical device
   *
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int readAnalogOutput(int address);

  /*! \brief Read all analog outputs on physical device
   *
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int readAnalogOutputs();

  /*! \brief Set value on a analog output on physical device
   *
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int writeAnalogOutput(int address, int value);

  /*! \brief Write all analog outputs to physical device
   *
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int writeAnalogOutputs();

  /*! \brief Read one digital input on physical device
   *
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int readDigitalInput(int address);

  /*! \brief Read all digital inputs on physical device
   *
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int readDigitalInputs();

  /*! \brief Read one digital output on physical device
   *
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int readDigitalOutput(int address);

  /*! \brief Read all digital outputs on physical device
   *
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int readDigitalOutputs();

  /*! \brief Write state on a digital output to physical device
   *
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int writeDigitalOutput(int address, bool state);

  /*! \brief Write all digital outputs to physical device
   *
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   */
  int writeDigitalOutputs();

  // Sequence of periodic queries
  bool queriesSequence();

  mdtModbusTcpPortManager *pvTcpPortManager;
  mdtFrameCodecModbus *pvCodec;
};

#endif  // #ifndef MDT_DEVICE_MODBUS_H
