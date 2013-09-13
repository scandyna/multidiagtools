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
#ifndef MDT_DEVICE_IOS_H
#define MDT_DEVICE_IOS_H

#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"
#include "mdtValue.h"
#include <QObject>
#include <QList>
#include <QMap>
#include <QVariant>

class mdtDeviceIosSegment;

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
 *
 * Note: if a address is changed by accessing directly by mdtAnalogIo or mdtDigitalIo,
 *   this will not be reflected, and stored I/O's will become incoherent.
 *   The address must allways be set before adding the I/O to this container.
 *   Example:
 *    - mdtAnalogIo *analogIn = new mdtAnalogIo;
 *    - analogIn->setAddress(25);
 *    - ios.addAnalogInput(analogIn);
 *
 * For some devices it's possible to read/write multiple I/O's in one query
 *  (f.ex. I/O systems communicating with MODBUS).
 *  To use this functionnality, this I/O container provide a solution to get values for several I/O's.
 *  Because addresses of all I/O's that will be stored in this container are not necessarily contiguous,
 *  the concept of segment is used. For example: ADDRESS|0|1|2|5|6| . Here, we have 2 segments: |0|1|2| and |5|6| .
 *  If we would write all I/O's present in this example, we could do it with 2 queries (1 per segment).
 *  To set/get data for segments, the folowing methods can be used:
 *   - analogInputsSegments()
 *   - analogOutputsSegments()
 *   - digitalInputsSegments()
 *   - digitalOutputsSegments()
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

  /*! \brief Get analog input with a given short label
   *
   * If no object exists at given short label,
   *  a null pointer is returned.
   *  If more than one object exists with the same short label,
   *  the first found is returned.
   */
  mdtAnalogIo *analogInputWithLabelShort(const QString &labelShort);

  /*! \brief Get a list containing all analog inputs
   *
   * Note: the returned list is not sorted.
   *        for grouped queries, see analogInputsSegments()
   */
  const QList<mdtAnalogIo*> analogInputs() const;

  /*! \brief Get a list of all analog inputs segments
   */
  const QList<mdtDeviceIosSegment*> analogInputsSegments() const;

  /*! \brief Get address of the first analog input
   */
  int analogInputsFirstAddress() const;

  /*! \brief Get the number of analog inputs
   */
  int analogInputsCount() const;

  /*! \brief Set a value and validity for all analog inputs
   */
  void setAnalogInputsValue(const mdtValue &value);

  /*! \brief Add a analog output
   *
   * \pre ao must be a valid pointer
   */
  void addAnalogOutput(mdtAnalogIo *ao);

  /*! \brief Get analog output at given address for read access
   *
   * If no object exists at given address,
   *  a null pointer is returned.
   */
  mdtAnalogIo *analogOutputAtAddressRead(int address);

  /*! \brief Get analog output at given address for write access
   *
   * If no object exists at given address,
   *  a null pointer is returned.
   */
  mdtAnalogIo *analogOutputAtAddressWrite(int address);

  /*! \brief Get analog output with a given short label
   *
   * If no object exists at given short label,
   *  a null pointer is returned.
   *  If more than one object exists with the same short label,
   *  the first found is returned.
   */
  mdtAnalogIo *analogOutputWithLabelShort(const QString &labelShort);

  /*! \brief Get a list containing all analog outputs
   *
   * Note: the returned list is not sorted
   */
  const QList<mdtAnalogIo*> analogOutputs() const;

  /*! \brief Get address for read access of the first analog output
   */
  int analogOutputsFirstAddressRead() const;

  /*! \brief Get address for write access of the first analog output
   */
  int analogOutputsFirstAddressWrite() const;

  /*! \brief Get the number of analog outputs
   */
  int analogOutputsCount() const;

  /*! \brief Get all analog outputs values in device specific format sorted by address for write access
   */
  QList<int> analogOutputsValuesIntByAddressWrite() const;

  /*! \brief Set a value and validity for all analog outputs
   */
  void setAnalogOutputsValue(const mdtValue &value);

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

  /*! \brief Get digital input with a given short label
   *
   * If no object exists at given short label,
   *  a null pointer is returned.
   *  If more than one object exists with the same short label,
   *  the first found is returned.
   */
  mdtDigitalIo *digitalInputWithLabelShort(const QString &labelShort);

  /*! \brief Get a list containing all digital inputs
   *
   * Note: the returned list is not sorted
   */
  const QList<mdtDigitalIo*> digitalInputs() const;

  /*! \brief Get address of the first digital input
   */
  int digitalInputsFirstAddress() const;

  /*! \brief Get the number of digital inputs
   */
  int digitalInputsCount() const;

  /*! \brief Set a value and validity for all digital inputs
   */
  void setDigitalInputsValue(const mdtValue &value);

  /*! \brief Add a digital output
   *
   * \pre dout must be a valid pointer
   */
  void addDigitalOutput(mdtDigitalIo *dout);

  /*! \brief Get digital output at given address for read access
   *
   * If no object exists at given address,
   *  a null pointer is returned.
   */
  mdtDigitalIo *digitalOutputAtAddressRead(int address);

  /*! \brief Get digital output at given address for write access
   *
   * If no object exists at given address,
   *  a null pointer is returned.
   */
  mdtDigitalIo *digitalOutputAtAddressWrite(int address);

  /*! \brief Get digital output with a given short label
   *
   * If no object exists at given short label,
   *  a null pointer is returned.
   *  If more than one object exists with the same short label,
   *  the first found is returned.
   */
  mdtDigitalIo *digitalOutputWithLabelShort(const QString &labelShort);

  /*! \brief Get a list containing all digital outputs
   *
   * Note: the returned list is not sorted
   */
  const QList<mdtDigitalIo*> digitalOutputs() const;

  /*! \brief Get address for read access of the first digital output
   */
  int digitalOutputsFirstAddressRead() const;

  /*! \brief Get address for write access of the first digital output
   */
  int digitalOutputsFirstAddressWrite() const;

  /*! \brief Get the number of digital outputs
   */
  int digitalOutputsCount() const;

  /*! \brief Set a value and validity for all digital outputs
   */
  void setDigitalOutputsValue(const mdtValue &value);

  /*! \brief Get all digital outputs states sorted by address for write access
   */
  QList<bool> digitalOutputsStatesByAddressWrite() const;

 public slots:

  /*! \brief Update (G)UI's value for a set of analog inputs
   *
   * \param values The values of inputs, must be sorted by address, ascending.
   *                If a value is invalid, concerned input will be set invalid.
   * \param firstAddress Address of first inputs to update.
   *                          If < 0, the internal first address is considered.
   * \param n Quantity of inputs to update. If < 0, the internal quantity is considered.
   *
   * Note:
   *  - If one value is invalid, concerned input will be set invalid.
   *  - Regarding on internal values type (double, int, ...), conversions will be done by internal mdtAnalogIo.
   */
  void updateAnalogInputValues(const QList<QVariant> &values, int firstAddress, int n);

  /*! \brief Update (G)UI's value for a set of analog outputs
   *
   * \param values The values of outputs, must be sorted by address, ascending.
   *                If a value is invalid, concerned output will be set invalid.
   * \param firstAddressRead Address (for device read access) of first output to update.
   *                          If < 0, the internal first address is considered.
   * \param n Quantity of outputs to update. If < 0, the internal quantity is considered.
   *
   * Note:
   *  - If one value is invalid, concerned output will be set invalid.
   *  - Regarding on internal values type (double, int, ...), conversions will be done by internal mdtAnalogIo.
   */
  void updateAnalogOutputValues(const QList<QVariant> &values, int firstAddressRead, int n);

  /*! \brief Enable/disable (G)UI's analog outputs
   */
  void setAnalogOutputsEnabled(bool enabled);

  /*! \brief Update (G)UI's state for a set of digital inputs
   *
   * \param values The values of inputs, must be sorted by address, ascending.
   *                If a value is invalid, concerned input will be set invalid.
   * \param firstAddress Address of first input to update.
   *                          If < 0, the internal first address is considered.
   * \param n Quantity of inputs to update. If < 0, the internal quantity is considered.
   */
  void updateDigitalInputValues(const QList<QVariant> &values, int firstAddress, int n);

  /*! \brief Update (G)UI's state for a set of digital outputs
   *
   * \param values The values of outputs, must be sorted by address, ascending.
   *                If a value is invalid, concerned output will be set invalid.
   * \param firstAddressRead Address (for device read access) of first output to update.
   *                          If < 0, the internal first address is considered.
   * \param n Quantity of outputs to update. If < 0, the internal quantity is considered.
   */
  void updateDigitalOutputValues(const QList<QVariant> &values, int firstAddressRead, int n);

  /*! \brief Enable/disable (G)UI's digital outputs
   */
  void setDigitalOutputsEnabled(bool enabled);

 private:

  Q_DISABLE_COPY(mdtDeviceIos);

  bool pvAutoDeleteIos;
  // Analog inputs members
  QList<mdtAnalogIo*> pvAnalogInputs;
  QMap<int, mdtAnalogIo*> pvAnalogInputsByAddressRead;
  int pvAnalogInputsFirstAddressRead;
  // Analog outputs members
  QList<mdtAnalogIo*> pvAnalogOutputs;
  QMap<int, mdtAnalogIo*> pvAnalogOutputsByAddressRead;
  QMap<int, mdtAnalogIo*> pvAnalogOutputsByAddressWrite;
  int pvAnalogOutputsFirstAddressRead;
  int pvAnalogOutputsFirstAddressWrite;
  // Digital inputs members
  QList<mdtDigitalIo*> pvDigitalInputs;
  QMap<int, mdtDigitalIo*> pvDigitalInputsByAddressRead;
  int pvDigitalInputsFirstAddressRead;
  // Digital outputs members
  QList<mdtDigitalIo*> pvDigitalOutputs;
  QMap<int, mdtDigitalIo*> pvDigitalOutputsByAddressRead;
  QMap<int, mdtDigitalIo*> pvDigitalOutputsByAddressWrite;
  int pvDigitalOutputsFirstAddressRead;
  int pvDigitalOutputsFirstAddressWrite;
};

#endif  // #ifndef MDT_DEVICE_IOS_H
