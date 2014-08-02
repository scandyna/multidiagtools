/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
 * Concept of position
 *
 *  Internally, I/Os are stored in 4 QList:
 *   - 1 for analog inputs
 *   - 1 for analog outputs
 *   - 1 for digital inputs
 *   - 1 for digital outputs
 *
 * The position is simply a index in a list, with a range from 0 to N-1 I/O for a type .
 *  The order of position is the order of call to add method .
 *
 * Concept of address
 *
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
 *
 * Some helper methods are also available:
 *  - updateAnalogInputValues()
 *  - updateAnalogOutputValues()
 *  - updateDigitalInputValues()
 *  - updateDigitalOutputValues()
 */
class mdtDeviceIos : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Adress access type
   *
   * It can happen that a I/O must be accessed by 2 different
   *  addresses for write (set) and read (get).
   *  A typical example is MODBUS.
   */
  enum addressAccess_t {
                        Read ,  /*!< Considere the read access address */
                        Write   /*!< Considere the write access address */
                       };

  /*! \brief Constructor
   */
  mdtDeviceIos(QObject *parent = 0);

  /*! \brief Destructor
   */
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
   * \pre ao must be a valid pointer allready be stored
   * \pre No I/O with same address must be set as double
   */
  void addAnalogInput(mdtAnalogIo *ai);

  /*! \brief Add a list of analog inputs
   *
   * \sa addAnalogInput()
   */
  void addAnalogInputs(const QList<mdtAnalogIo*> & aiList);

  /*! \brief Get analog input at given address
   *
   * If no object exists at given address,
   *  a null pointer is returned.
   */
  mdtAnalogIo *analogInputAtAddress(int address);

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
   * Note: the returned list is not sorted by address .
   *        for grouped queries, see analogInputsSegments() and updateAnalogInputValues()
   */
  const QList<mdtAnalogIo*> analogInputs() const;

  /*! \brief Get a list of all analog inputs segments
   */
  const QList<mdtDeviceIosSegment*> &analogInputsSegments() const;

  /*! \brief Get the number of analog inputs
   */
  int analogInputsCount() const;

  /*! \brief Set a value and validity for all analog inputs
   */
  void setAnalogInputsValue(const mdtValue &value);

  /*! \brief Add a analog output
   *
   * \param sortSegmentsBy Output will be added to a segment.
   *          sortSegmentsBy gives the address access to consider.
   *          Note: if device maps addresses for read and write access
   *                in the same order, but just with a offset, this parameter is not important
   *                (The address set in mdtAnalogIo object are not changed).
   *
   * \pre ao must be a valid pointer allready be stored
   * \pre No I/O with same address must be set as double
   */
  void addAnalogOutput(mdtAnalogIo *ao, addressAccess_t sortSegmentsBy = Write);

  /*! \brief Add a list of analog outputs
   *
   * \sa addAnalogOutput()
   */
  void addAnalogOutputs(const QList<mdtAnalogIo*> & aoList, addressAccess_t sortSegmentsBy = Write);

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
   * Note: the returned list is not sorted by address .
   */
  const QList<mdtAnalogIo*> analogOutputs() const;

  /*! \brief Get a list of all analog inputs segments
   */
  const QList<mdtDeviceIosSegment*> &analogOutputsSegments() const;

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
   * \pre ao must be a valid pointer allready be stored
   * \pre No I/O with same address must be set as double
   */
  void addDigitalInput(mdtDigitalIo *di);

  /*! \brief Add a list of digital inputs
   *
   * \sa addDigitalInput()
   */
  void addDigitalInputs(const QList<mdtDigitalIo*> & diList);

  /*! \brief Get digital input at given address
   *
   * If no object exists at given address,
   *  a null pointer is returned.
   */
  mdtDigitalIo *digitalInputAtAddress(int address);

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
   * Note: the returned list is not sorted by address .
   */
  const QList<mdtDigitalIo*> digitalInputs() const;

  /*! \brief Get a list of all digital inputs segments
   */
  const QList<mdtDeviceIosSegment*> &digitalInputsSegments() const;

  /*! \brief Get the number of digital inputs
   */
  int digitalInputsCount() const;

  /*! \brief Set a value and validity for all digital inputs
   */
  void setDigitalInputsValue(const mdtValue &value);

  /*! \brief Add a digital output
   *
   * \param sortSegmentsBy Output will be added to a segment.
   *          sortSegmentsBy gives the address access to consider.
   *          Note: if device maps addresses for read and write access
   *                in the same order, but just with a offset, this parameter is not important
   *                (The address set in mdtAnalogIo object are not changed).
   *
   * \pre ao must be a valid pointer allready be stored
   * \pre No I/O with same address must be set as double
   */
  void addDigitalOutput(mdtDigitalIo *dout, addressAccess_t sortSegmentsBy = Write);

  /*! \brief Add a list of digital outputs
   *
   * \sa addDigitalOutput()
   */
  void addDigitalOutputs(const QList<mdtDigitalIo*> & doList, addressAccess_t sortSegmentsBy = Write);

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
   * Note: the returned list is not sorted by address .
   */
  const QList<mdtDigitalIo*> digitalOutputs() const;

  /*! \brief Get a list of all digital outputs segments
   */
  const QList<mdtDeviceIosSegment*> &digitalOutputsSegments() const;

  /*! \brief Get the number of digital outputs
   */
  int digitalOutputsCount() const;

  /*! \brief Set a value and validity for all digital outputs
   */
  void setDigitalOutputsValue(const mdtValue &value);

  /*! \brief Get all digital outputs states sorted by address for write access
   */
  QList<bool> digitalOutputsStatesByAddressWrite() const;

  /*! \brief Set a default short label for each I/O
   *
   * Default short label is the I/O type suffixed with his number
   *  (AI1, AI2, AO1, DI1, DO1, ...) 
   */
  void setIosDefaultLabelShort();

 public slots:

  /*! \brief Update (G)UI's value for a set of analog inputs
   *
   * \param values The values of inputs, must be sorted by address, ascending.
   *                If a value is invalid, concerned input will be set invalid.
   * \param firstAddress Address of first input to update:
   *                      - firstAddress < 0 : the I/O in container that has the lowest address will fix the first address.
   *                      - firstAddress >= 0 : will be token as first address.
   *
   * \param n Maximum quantity of inputs to update (can be < values size).
   *           If < 0, the maximum possible values to store is determined between list size and the internal quantity of inputs.
   *           Note: this limit was introduced for conveniance during MODBUS decoding.
   *
   * \param matchAddresses If true, first index of values will be considered as first address ,
   *                        and only I/O's for witch address match will be updated.
   *           For example, if we have following I/O's addresses in container: 0,1,2,5,6 and we give 4 values:
   *            - If firstAddress = 2 and matchAddresses is set : I/O's with address 2, 5 and 6 will be updated
   *                with values at index 0 and 3 
   *                (by giving firstAddress = 2, addresses are considered 2,3,4,5, so only 2 values will be updated).
   *            - If firstAddress = -1 and matchAddresses is set : I/O's with address 0, 1 and 2 will be updated
   *               (4th value must match address 3 that not exit in container, so only 3 values will be updated).
   *            - If firstAddress = 2 and matchAddresses not set : I/O's with address 2, 5 and 6 will be updated (only 3 values).
   *            - If firstAddress = -1 and matchAddresses not set : I/O's with address 0, 1, 2 and 5 will be updated (all 4 values).
   *
   * Note:
   *  - If one value is invalid, concerned input will be set invalid.
   *  - Regarding on internal values type (double, int, ...), conversions will be done by internal mdtAnalogIo.
   *
   * \pre n must be <= values size
   */
  void updateAnalogInputValues(const QList<QVariant> &values, const int firstAddress, const int n, bool matchAddresses);

  /*! \brief Update (G)UI's value for a set of analog outputs
   *
   * \param values The values of outputs, must be sorted by address, ascending.
   *                If a value is invalid, concerned outputs will be set invalid.
   * \param firstAddress Address of first output to update:
   *                      - firstAddress < 0 : the I/O in container that has the lowest address will fix the first address.
   *                      - firstAddress >= 0 : will be token as first address.
   *
   * \param firstAddressAccess Tell here if given firstAddress is supposed for read or write access.
   *               Note: f.ex: by decoding a MODBUS PDU, we update I/O's of this container once we had
   *                     read states from device, here the address access will also be Read.
   *
   * \param n Maximum quantity of outputs to update (can be < values size).
   *           If < 0, the maximum possible values to store is determined between list size and the internal quantity of outputs.
   *           Note: this limit was introduced for conveniance during MODBUS decoding.
   *
   * \param matchAddresses If true, first index of values will be considered as first address ,
   *                        and only I/O's for witch address match will be updated.
   *           For example, if we have following I/O's addresses in container: 0,1,2,5,6 and we give 4 values:
   *            - If firstAddress = 2 and matchAddresses is set : I/O's with address 2, 5 and 6 will be updated
   *                with values at index 0 and 3 
   *                (by giving firstAddress = 2, addresses are considered 2,3,4,5, so only 2 values will be updated).
   *            - If firstAddress = -1 and matchAddresses is set : I/O's with address 0, 1 and 2 will be updated
   *               (4th value must match address 3 that not exit in container, so only 3 values will be updated).
   *            - If firstAddress = 2 and matchAddresses not set : I/O's with address 2, 5 and 6 will be updated (only 3 values).
   *            - If firstAddress = -1 and matchAddresses not set : I/O's with address 0, 1, 2 and 5 will be updated (all 4 values).
   *
   * Note:
   *  - If one value is invalid, concerned output will be set invalid.
   *  - Regarding on internal values type (double, int, ...), conversions will be done by internal mdtAnalogIo.
   *
   * \pre n must be <= values size
   */
  void updateAnalogOutputValues(const QList<QVariant> &values, int firstAddress, addressAccess_t firstAddressAccess, int n, bool matchAddresses);

  /*! \brief Enable/disable (G)UI's analog outputs
   */
  void setAnalogOutputsEnabled(bool enabled);

  /*! \brief Update (G)UI's value for a set of digital inputs
   *
   * \param values The values of inputs, must be sorted by address, ascending.
   *                If a value is invalid, concerned input will be set invalid.
   * \param firstAddress Address of first input to update:
   *                      - firstAddress < 0 : the I/O in container that has the lowest address will fix the first address.
   *                      - firstAddress >= 0 : will be token as first address.
   *
   * \param n Maximum quantity of inputs to update (can be < values size).
   *           If < 0, the maximum possible values to store is determined between list size and the internal quantity of inputs.
   *           Note: this limit was introduced for conveniance during MODBUS decoding.
   *
   * \param matchAddresses If true, first index of values will be considered as first address ,
   *                        and only I/O's for witch address match will be updated.
   *           For example, if we have following I/O's addresses in container: 0,1,2,5,6 and we give 4 values:
   *            - If firstAddress = 2 and matchAddresses is set : I/O's with address 2, 5 and 6 will be updated
   *                with values at index 0 and 3 
   *                (by giving firstAddress = 2, addresses are considered 2,3,4,5, so only 2 values will be updated).
   *            - If firstAddress = -1 and matchAddresses is set : I/O's with address 0, 1 and 2 will be updated
   *               (4th value must match address 3 that not exit in container, so only 3 values will be updated).
   *            - If firstAddress = 2 and matchAddresses not set : I/O's with address 2, 5 and 6 will be updated (only 3 values).
   *            - If firstAddress = -1 and matchAddresses not set : I/O's with address 0, 1, 2 and 5 will be updated (all 4 values).
   *
   * Note:
   *  - If one value is invalid, concerned input will be set invalid.
   *  - Regarding on internal values type (double, int, ...), conversions will be done by internal mdtAnalogIo.
   *
   * \pre n must be <= values size
   */
  void updateDigitalInputValues(const QList<QVariant> &values, int firstAddress, int n, bool matchAddresses);

  /*! \brief Update (G)UI's value for a set of digital outputs
   *
   * \param values The values of outputs, must be sorted by address, ascending.
   *                If a value is invalid, concerned outputs will be set invalid.
   * \param firstAddress Address of first output to update:
   *                      - firstAddress < 0 : the I/O in container that has the lowest address will fix the first address.
   *                      - firstAddress >= 0 : will be token as first address.
   *
   * \param firstAddressAccess Tell here if given firstAddress is supposed for read or write access.
   *               Note: f.ex: by decoding a MODBUS PDU, we update I/O's of this container once we had
   *                     read states from device, here the address access will also be Read.
   *
   * \param n Maximum quantity of outputs to update (can be < values size).
   *           If < 0, the maximum possible values to store is determined between list size and the internal quantity of outputs.
   *           Note: this limit was introduced for conveniance during MODBUS decoding.
   *
   * \param matchAddresses If true, first index of values will be considered as first address ,
   *                        and only I/O's for witch address match will be updated.
   *           For example, if we have following I/O's addresses in container: 0,1,2,5,6 and we give 4 values:
   *            - If firstAddress = 2 and matchAddresses is set : I/O's with address 2, 5 and 6 will be updated
   *                with values at index 0 and 3 
   *                (by giving firstAddress = 2, addresses are considered 2,3,4,5, so only 2 values will be updated).
   *            - If firstAddress = -1 and matchAddresses is set : I/O's with address 0, 1 and 2 will be updated
   *               (4th value must match address 3 that not exit in container, so only 3 values will be updated).
   *            - If firstAddress = 2 and matchAddresses not set : I/O's with address 2, 5 and 6 will be updated (only 3 values).
   *            - If firstAddress = -1 and matchAddresses not set : I/O's with address 0, 1, 2 and 5 will be updated (all 4 values).
   *
   * Note:
   *  - If one value is invalid, concerned output will be set invalid.
   *  - Regarding on internal values type (double, int, ...), conversions will be done by internal mdtAnalogIo.
   *
   * \pre n must be <= values size
   */
  void updateDigitalOutputValues(const QList<QVariant> &values, int firstAddress, addressAccess_t firstAddressAccess, int n, bool matchAddresses);

  /*! \brief Enable/disable (G)UI's digital outputs
   */
  void setDigitalOutputsEnabled(bool enabled);

 private:

  Q_DISABLE_COPY(mdtDeviceIos);

  bool pvAutoDeleteIos;
  // Analog inputs members
  QList<mdtAnalogIo*> pvAnalogInputs;
  QMap<int, mdtAnalogIo*> pvAnalogInputsByAddressRead;
  QList<mdtDeviceIosSegment*> pvAnalogInputsSegments;
  // Analog outputs members
  QList<mdtAnalogIo*> pvAnalogOutputs;
  QMap<int, mdtAnalogIo*> pvAnalogOutputsByAddressRead;
  QMap<int, mdtAnalogIo*> pvAnalogOutputsByAddressWrite;
  QList<mdtDeviceIosSegment*> pvAnalogOutputsSegments;
  // Digital inputs members
  QList<mdtDigitalIo*> pvDigitalInputs;
  QMap<int, mdtDigitalIo*> pvDigitalInputsByAddressRead;
  QList<mdtDeviceIosSegment*> pvDigitalInputsSegments;
  // Digital outputs members
  QList<mdtDigitalIo*> pvDigitalOutputs;
  QMap<int, mdtDigitalIo*> pvDigitalOutputsByAddressRead;
  QMap<int, mdtDigitalIo*> pvDigitalOutputsByAddressWrite;
  QList<mdtDeviceIosSegment*> pvDigitalOutputsSegments;
};

#endif  // #ifndef MDT_DEVICE_IOS_H
