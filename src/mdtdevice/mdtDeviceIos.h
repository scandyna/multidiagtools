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
#ifndef MDT_DEVICE_IOS_H
#define MDT_DEVICE_IOS_H

#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"
#include <QObject>
#include <QList>
#include <QMap>
#include <QVariant>

/*! \brief Container for a set of I/O's
 *
 * This class can contain 4 type of I/O:
 *  - Analog input (based on mdtAnalogIo)
 *  - Analog output (based on mdtAnalogIo)
 *  - Digital input (based on mdtDigitalIo)
 *  - Digital output (based on mdtDigitalIo)
 *
 * The main goal here is the functionnal
 *  part. The (G)UI part is done in other place.
 * For example, it's possible to use mdtDeviceIosWidget
 *  class to have a generic representation af all
 *  type of I/O.
 *
 * Concept of address:
 *  Internally, each I/O is stored in a QMap with an address as key.
 *  In some cases, this can reflect a real situations, for exaple:
 *  - Relative address of a MODBUS query
 *  - Memory address of physical device
 *
 * You can choose to use this address as simple index.
 */
class mdtDeviceIos : public QObject
{
 Q_OBJECT

 public:

  mdtDeviceIos(QObject *parent = 0);
  ~mdtDeviceIos();

  /*! \brief Set the auto delete flag
   *
   * If true, all internal I/O's will
   *  be deleted when this object is deleted.
   *
   * By default, this flag is true.
   */
  void setAutoDeleteIos(bool autoDelete);

  /*! \brief Delete all I/O objects
   *
   * Will free all stored I/O objects.
   */
  void deleteIos();

  /*! \brief Add a analog input
   *
   * \pre ai must be a valid pointer
   */
  void addAnalogInput(mdtAnalogIo *ai);

  /*! \brief Get analog input at given address
   *
   * If no object exists at given address,
   *  a null pointer is returned.
   */
  mdtAnalogIo *analogInputAt(int address);

  /*! \brief Get a list containing all analog inputs
   */
  QList<mdtAnalogIo*> analogInputs();

  /*! \brief Get the number of analog inputs
   */
  int analogInputsCount() const;

  /*! \brief Add a analog output
   *
   * \pre ao must be a valid pointer
   */
  void addAnalogOutput(mdtAnalogIo *ao);

  /*! \brief Get analog output at given address
   *
   * If no object exists at given address,
   *  a null pointer is returned.
   */
  mdtAnalogIo *analogOutputAt(int address);

  /*! \brief Get a list containing all analog outputs
   */
  QList<mdtAnalogIo*> analogOutputs();

  /*! \brief Get the number of analog outputs
   */
  int analogOutputsCount() const;

  /*! \brief Get all analog outputs values in device specific format
   */
  const QList<int> analogOutputsValuesInt() const;

  /*! \brief Set a value and validity for all analog outputs
   */
  void setAnalogOutputsValue(QVariant value);

  /*! \brief Add a digital input
   *
   * \pre di must be a valid pointer
   */
  void addDigitalInput(mdtDigitalIo *di);

  /*! \brief Get digital input at given address
   *
   * If no object exists at given address,
   *  a null pointer is returned.
   */
  mdtDigitalIo *digitalInputAt(int address);

  /*! \brief Get a list containing all digital inputs
   */
  QList<mdtDigitalIo*> digitalInputs();

  /*! \brief Get the number of digital inputs
   */
  int digitalInputsCount() const;

  /*! \brief Add a digital output
   *
   * \pre dout must be a valid pointer
   */
  void addDigitalOutput(mdtDigitalIo *dout);

  /*! \brief Get digital output at given address
   *
   * If no object exists at given address,
   *  a null pointer is returned.
   */
  mdtDigitalIo *digitalOutputAt(int address);

  /*! \brief Get a list containing all digital outputs
   */
  QList<mdtDigitalIo*> digitalOutputs();

  /*! \brief Get the number of digital outputs
   */
  int digitalOutputsCount() const;

  /*! \brief Get all digital outputs states
   */
  const QList<bool> digitalOutputsStates() const;

 signals:

  /*! \brief This signal is emited when a analog output value has changed
   *
   * This signal is used to notify mdtDevice that a analog output must be set on device.
   */
  void analogOutputValueChanged(int address);

  /*! \brief This signal is emited when a digital output state has changed
   *
   * This signal is used to notify mdtDevice that a digital output must be set on device.
   */
  void digitalOutputStateChanged(int address);

 public slots:

  /*! \brief Update (G)UI's value for a set of analog inputs
   *
   * Note:
   *  - Here, it is assumed that values are stored from address 0 to N-1
   *  - If one address not exists, nothing will happen for this address.
   *  - If one value is invalid, concerned input will be set invalid.
   *  - If type value is of type integer, mdtAnalogIo::setValueInt() is used,
   *     else mdtAnalogIo::setValue() is used.
   */
  void updateAnalogInputValues(const QList<QVariant> &values);

  /*! \brief Update (G)UI's value for a set of analog outputs
   *
   * Note:
   *  - Here, it is assumed that values are stored from address 0 to N-1
   *  - If one address not exists, nothing will happen for this address.
   *  - If one value is invalid, concerned input will be set invalid.
   *  - If type value is of type integer, mdtAnalogIo::setValueInt() is used,
   *     else mdtAnalogIo::setValue() is used.
   */
  void updateAnalogOutputValues(const QList<QVariant> &values);

  /*! \brief Enable/disable (G)UI's analog outputs
   */
  void setAnalogOutputsEnabled(bool enabled);

  /*! \brief Update (G)UI's state for a set of digital inputs
   *
   * Note:
   *  - Here, it is assumed that values are stored from address 0 to N-1
   *  - If one address not exists, nothing will happen for this address.
   *  - If one value is invalid, concerned input will be set invalid.
   */
  void updateDigitalInputStates(const QList<QVariant> &values);

  /*! \brief Update (G)UI's state for a set of digital outputs
   *
   * Note:
   *  - Here, it is assumed that values are stored from address 0 to N-1
   *  - If one address not exists, nothing will happen for this address.
   *  - If one value is invalid, concerned input will be set invalid.
   */
  void updateDigitalOutputStates(const QList<QVariant> &values);

 private:

  Q_DISABLE_COPY(mdtDeviceIos);

  bool pvAutoDeleteIos;
  QMap<int, mdtAnalogIo*> pvAnalogInputs;
  QMap<int, mdtAnalogIo*> pvAnalogOutputs;
  QMap<int, mdtDigitalIo*> pvDigitalInputs;
  QMap<int, mdtDigitalIo*> pvDigitalOutputs;
};

#endif  // #ifndef MDT_DEVICE_IOS_H
