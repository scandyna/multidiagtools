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
#ifndef MDT_MULTI_IO_DEVICE_H
#define MDT_MULTI_IO_DEVICE_H

#include "mdtDevice.h"
#include "mdtDeviceIos.h"
#include "mdtValue.h"
#include <memory>

class mdtDeviceIosSegment;
class mdtPortTransaction;
class mdtPortManager;

/*! \brief Base class for a device that contains multiple I/Os
 *
 * Notes about I/O container:
 *  In first version of mdtDevice, I/O container (mdtDeviceIos object) was handled by caller.
 *  Experience showed that this was a bad solution.
 *  Now, the container is created, and destroyed, by mdtDevice itself.
 *  To deal with I/O container, some methods are available:
 *  - addInput(mdtAnalogIo *) : add a analog input.
 *  - addInputs(QList<mdtAnalogIo*> &) : add a list of analog inputs.
 *  - addOutput(mdtAnalogIo *) : add a analog output.
 *  - addOutputs(QList<mdtAnalogIo*> &) : add a list of analog outputs.
 *  - addInput(mdtDigitalIo *) : add a digital input.
 *  - addInputs(QList<mdtDigitalIo*> &) : add a list of digital inputs.
 *  - addOutput(mdtDigitalIo *) : add a digital output.
 *  - addOutputs(QList<mdtDigitalIo*> &) : add a list of digital outputs.
 *  - ios() : access I/O container.
 *  - deleteIos() : delete all I/O in container.
 */
class mdtMultiIoDevice : public mdtDevice
{
 Q_OBJECT

 public:

  /*! \brief Construct a multi I/O device object.
   */
  mdtMultiIoDevice(QObject *parent = 0);

  /*! \brief Destructor
   */
  virtual ~mdtMultiIoDevice();

  /*! \brief Get internal port manager instance
   *
   * Note that port manager is set by subclass,
   *  and that a Null pointer can be returned.
   *  (See subclass documentation for details)
   */
  virtual mdtPortManager *portManager()
  {
    return 0;
  }

  /*! \brief Add a analog input
   */
  void addInput(mdtAnalogIo *analogInput);

  /*! \brief Add a list of analog inputs
   */
  void addInputs(const QList<mdtAnalogIo*> & analogInputs);

  /*! \brief Add a analog output
   */
  void addOutput(mdtAnalogIo *analogOutput);

  /*! \brief Add a list of analog outputs
   */
  void addOutputs(const QList<mdtAnalogIo*> & analogOutputs);

  /*! \brief Add a digital input
   */
  void addInput(mdtDigitalIo *digitalInput);

  /*! \brief Add a list of digital inputs
   */
  void addInputs(const QList<mdtDigitalIo*> & digitalInputs);

  /*! \brief Add a digital output
   */
  void addOutput(mdtDigitalIo *digitalOutput);

  /*! \brief Add a list of digital outputs
   */
  void addOutputs(const QList<mdtDigitalIo*> & digitalOutputs);

  /*! \brief Delete all I/O in container
   *
   * All I/O's will be deleted,
   *  but I/O container itself will remain valid.
   */
  void deleteIos();

  /*! \brief Access internal I/Os container
   */
  std::shared_ptr<mdtDeviceIos> ios()
  {
    return pvIos;
  }

  /*! \brief Get analog input value
   *
   * Get device or internal analog value.
   *  Internal value is updated if queryDevice is set.
   *
   * \param analogInput Pointer to a analog input object.
   * \param queryDevice If true, value is readen from device by calling readAnalogInput(), else cached value is returned.
   *                      Behaviour of this method can vary, depending on device specific subclass.
   *                      (See sublcass readAnalogInput() for details).
   * \param waitOnReply If true, this method will wait until reply comes in, or timeout (See waitTransactionDone() ),
   *                     else it will return immediately after the query was sent. \deprecated waitOnReply must not be used anymore
   * \return A mdtValue with valueDouble as real value and valueInt as device specific encoded fromat (if available).
   *          See mdtValue documentation for details about validity and other flags.
   * \pre analogInput must be valid.
   */
  mdtValue getAnalogInputValue(mdtAnalogIo *analogInput, bool queryDevice, bool waitOnReply);

  /*! \brief Get analog input value
   *
   * \param address Depending on device organisation and protocol,
   *                 this can be a relative or absolute address (f.ex. MODBUS queries),
   *                 a input number, etc...
   *
   * \sa getAnalogInputValue(mdtAnalogIo*, bool, bool)
   */
  mdtValue getAnalogInputValue(int address, bool queryDevice, bool waitOnReply = true);

  /*! \brief Get analog input value
   *
   * \param position See the concept of position in mdtDeviceIos class detailed description .
   *
   * \sa getAnalogInputValue(mdtAnalogIo*, bool, bool)
   */
  mdtValue getAnalogInputValueAt(int position, bool queryDevice, bool waitOnReply = true);

  /*! \brief Get analog input value
   *
   * \overload getAnalogInputValue(mdtAnalogIo*, bool, bool)
   *
   * \param labelShort Short label set in I/O (see mdtAnalogIo for details).
   */
  mdtValue getAnalogInputValue(const QString & labelShort, bool queryDevice, bool waitOnReply = true);

  /*! \brief Read all analog inputs on physical device and update (G)UI representation
   *
   * Will call readAnalogInputs(), witch also sends the request to device.
   *  Depending on result, device's state can be updated.
   *
   * Behaviour of this method can vary, depending on device specific subclass.
   *  (See sublcass readAnalogInputs() for details).
   *
   * Once results are available or error occured, the internal analog inputs are updated.
   *  (See mdtDeviceIos for details).
   *
   * \param waitOnReply If true, this method will wait until reply comes in, or timeout (See waitTransactionDone() ),
   *                     else it will return immediately after the query was sent. \deprecated waitOnReply must not be used anymore
   */
  bool getAnalogInputs(bool waitOnReply = true);

  /*! \brief Get analog output value
   *
   * Get device or internal analog value.
   *  Internal value is updated if queryDevice is set.
   *
   * \param analogOutput Pointer to a analog output object.
   * \param queryDevice If true, value is readen from device by calling readAnalogOutput(), else cached value is returned.
   *                      Behaviour of this method can vary, depending on device specific subclass.
   *                      (See sublcass readAnalogOutput() for details).
   * \param waitOnReply If true, this method will wait until reply comes in, or timeout (See waitTransactionDone() ),
   *                     else it will return immediately after the query was sent. \deprecated waitOnReply must not be used anymore
   * \return A mdtValue with valueDouble as real value and valueInt as device specific encoded fromat (if available).
   *          See mdtValue documentation for details about validity and other flags.
   * \pre analogOutput must be valid.
   */
  mdtValue getAnalogOutputValue(mdtAnalogIo *analogOutput, bool queryDevice, bool waitOnReply = true);

  /*! \brief Get analog output value
   *
   * \param addressRead Depending on device organisation and protocol,
   *                 this can be a relative or absolute address (f.ex. MODBUS queries),
   *                 a output number, etc...
   *
   * \sa getAnalogOutputValue(mdtAnalogIo*, bool, bool)
   */
  mdtValue getAnalogOutputValue(int addressRead, bool queryDevice, bool waitOnReply = true);

  /*! \brief Get analog output value
   *
   * \param position See the concept of position in mdtDeviceIos class detailed description .
   *
   * \sa getAnalogOutputValue(mdtAnalogIo*, bool, bool)
   */
  mdtValue getAnalogOutputValueAt(int position, bool queryDevice, bool waitOnReply = true);

  /*! \brief Get analog output value
   *
   * \overload getAnalogOutputValue(mdtAnalogIo*, bool, bool)
   *
   * \param labelShort Short label set in I/O (see mdtAnalogIo for details).
   */
  mdtValue getAnalogOutputValue(const QString &labelShort, bool queryDevice, bool waitOnReply = true);

  /*! \brief Read all analog outputs on physical device and update (G)UI representation
   *
   * Will call readAnalogOutputs(), witch also sends the request to device.
   *  Depending on result, device's state can be updated.
   *
   * Behaviour of this method can vary, depending on device specific subclass.
   *  (See sublcass readAnalogOutputs() for details).
   *
   * Once results are available or error occured, the internal analog outputs are updated.
   *  (See mdtDeviceIos for details).
   *
   * \param waitOnReply If true, this method will wait until reply comes in, or timeout (See waitTransactionDone() ),
   *                     else it will return immediately after the query was sent. \deprecated waitOnReply must not be used anymore
   */
  bool getAnalogOutputs(bool waitOnReply = true);

  /*! \brief Set analog output value
   *
   * Set device or internal analog value.
   *  Internal value is updated if queryDevice is set.
   *
   * \param analogOutput Pointer to a analog output object.
   * \param value Value to send/store.
   * \param sendToDevice If true, value is sent device by calling writeAnalogOutput(), else value is only cached.
   *                      Behaviour of this method can vary, depending on device specific subclass.
   *                      (See sublcass writeAnalogOutput() for details).
   * \param waitOnReply If true, this method will wait until reply comes in, or timeout (See waitTransactionDone() ),
   *                     else it will return immediately after the query was sent. \deprecated waitOnReply must not be used anymore
   * \pre analogOutput must be valid.
   */
  bool setAnalogOutputValue(mdtAnalogIo *analogOutput, const mdtValue &value, bool sendToDevice, bool waitOnReply = true);

  /*! \brief Set analog output value
   *
   * \param addressWrite Depending on device organisation and protocol,
   *                 this can be a relative or absolute address (f.ex. MODBUS queries),
   *                 a output number, etc...
   *
   * \sa setAnalogOutputValue(mdtAnalogIo*, const mdtValue&, bool, bool)
   */
  bool setAnalogOutputValue(int addressWrite, const mdtValue &value, bool sendToDevice, bool waitOnReply = true);

  /*! \brief Set analog output value
   *
   * \param position See the concept of position in mdtDeviceIos class detailed description .
   *
   * \sa setAnalogOutputValue(mdtAnalogIo*, const mdtValue&, bool, bool)
   */
  bool setAnalogOutputValueAt(int position, const mdtValue &value, bool sendToDevice, bool waitOnReply = true);

  /*! \brief Set analog output value
   *
   * \overload setAnalogOutputValue(mdtAnalogIo*, const mdtValue&, bool, bool)
   *
   * \param labelShort Short label set in I/O (see mdtAnalogIo for details).
   */
  bool setAnalogOutputValue(const QString &labelShort, const mdtValue &value, bool sendToDevice, bool waitOnReply = true);

  /*! \brief Write all analog outputs on physical device and update (G)UI representation
   *
   * To send the analog outputs values once, it's possible to set them without sending query.
   *  See mdtDeviceIos and mdtAnalogIo for details.
   *  Then, call this method to sent all analog outputs values to device.
   *
   * writeAnalogOutputs() will be called, witch also sends the request to device.
   *  Depending on result, device's state can be updated.
   *
   * Behaviour of this method can vary, depending on device specific subclass.
   *  (See sublcass writeAnalogOutputs() for details).
   *
   * Once results are available or on error, the internal analog outputs are updated.
   *  (See mdtDeviceIos for details).
   *
   * \param waitOnReply If true, this method will wait until reply comes in, or timeout (See waitTransactionDone() ),
   *                     else it will return immediately after the query was sent. \deprecated waitOnReply must not be used anymore
   */
  bool setAnalogOutputs(bool waitOnReply = true);

  /*! \brief Get digital input value
   *
   * Get device or internal digital value.
   *  Internal value is updated if queryDevice is set.
   *
   * \param digitalInput Pointer to a digital input.
   * \param queryDevice If true, value is readen from device by calling readDigitalInput(), else cached value is returned.
   *                      Behaviour of this method can vary, depending on device specific subclass.
   *                      (See sublcass readDigitalInput() for details).
   * \param waitOnReply If true, this method will wait until reply comes in, or timeout (See waitTransactionDone() ),
   *                     else it will return immediately after the query was sent. \deprecated waitOnReply must not be used anymore
   * \return A mdtValue with valueBool. See mdtValue documentation for details about validity and other flags.
   * \pre digitalInput must be valid.
   */
  mdtValue getDigitalInputValue(mdtDigitalIo *digitalInput, bool queryDevice, bool waitOnReply = true);

  /*! \brief Get digital input value
   *
   * \param address Depending on device organisation and protocol,
   *                 this can be a relative or absolute address (f.ex. MODBUS queries),
   *                 a input number, etc...
   *
   * \sa getDigitalInputValue(mdtDigitalIo*, bool, bool)
   */
  mdtValue getDigitalInputValue(int address, bool queryDevice, bool waitOnReply = true);

  /*! \brief Get digital input value
   *
   * \param position See the concept of position in mdtDeviceIos class detailed description .
   *
   * \sa getDigitalInputValue(mdtDigitalIo*, bool, bool)
   */
  mdtValue getDigitalInputValueAt(int position, bool queryDevice, bool waitOnReply = true);

  /*! \brief Get digital input value
   *
   * \overload getDigitalInputValue(mdtDigitalIo*, bool, bool)
   *
   * \param labelShort Short label set in I/O (see mdtDigitalIo for details).
   */
  mdtValue getDigitalInputValue(const QString &labelShort, bool queryDevice, bool waitOnReply = true);

  /*! \brief Read all digital inputs on physical device and update (G)UI representation
   *
   * Will call readDigitalInputs(), witch also sends the request to device.
   *  Depending on result, device's state can be updated.
   *
   * Behaviour of this method can vary, depending on device specific subclass.
   *  (See sublcass readDigitalInputs() for details).
   *
   * Once results are available, the internal digital inputs are updated.
   *  (See mdtDeviceIos for details).
   *
   * \param waitOnReply If true, this method will wait until reply comes in, or timeout (See waitTransactionDone() ),
   *                     else it will return immediately after the query was sent. \deprecated waitOnReply must not be used anymore
   */
  bool getDigitalInputs(bool waitOnReply = true);

  /*! \brief Get digital output value
   *
   * \param digitalOutput Pointer to a digital output.
   * \param queryDevice If true, value is readen from device by calling readDigitalOutput(), else cached value is returned.
   *                      Behaviour of this method can vary, depending on device specific subclass.
   *                      (See sublcass readDigitalOutput() for details).
   * \param waitOnReply If true, this method will wait until reply comes in, or timeout (See waitTransactionDone() ),
   *                     else it will return immediately after the query was sent. \deprecated waitOnReply must not be used anymore
   * \return A mdtValue with valueBool. See mdtValue documentation for details about validity and other flags.
   * \pre digitalOutput must be valid.
   */
  mdtValue getDigitalOutputValue(mdtDigitalIo *digitalOutput, bool queryDevice, bool waitOnReply = true);

  /*! \brief Get digital output value
   *
   * \param addressRead Depending on device organisation and protocol,
   *                 this can be a relative or absolute address (f.ex. MODBUS queries),
   *                 a output number, etc...
   *
   * \sa getDigitalOutputValue(mdtDigitalIo*, bool, bool)
   */
  mdtValue getDigitalOutputValue(int addressRead, bool queryDevice, bool waitOnReply = true);

  /*! \brief Get digital output value
   *
   * \param position See the concept of position in mdtDeviceIos class detailed description .
   *
   * \sa getDigitalOutputValue(mdtDigitalIo*, bool, bool)
   */
  mdtValue getDigitalOutputValueAt(int position, bool queryDevice, bool waitOnReply = true);

  /*! \brief Get digital output value
   *
   * \overload getDigitalOutputValue(mdtDigitalIo*, bool, bool)
   *
   * \param labelShort Short label set in I/O (see mdtDigitalIo for details).
   */
  mdtValue getDigitalOutputValue(const QString &labelShort, bool queryDevice, bool waitOnReply = true);

  /*! \brief Read all digital outputs on physical device and update (G)UI representation
   *
   * Will call readDigitalOutputs(), witch also sends the request to device.
   *  Depending on result, device's state can be updated.
   *
   * Behaviour of this method can vary, depending on device specific subclass.
   *  (See sublcass readDigitalOutputs() for details).
   *
   * Once results are available, the internal digital outputs are updated.
   *  (See mdtDeviceIos for details).
   *
   * \param waitOnReply If true, this method will wait until reply comes in, or timeout (See waitTransactionDone() ),
   *                     else it will return immediately after the query was sent. \deprecated waitOnReply must not be used anymore
   */
  bool getDigitalOutputs(bool waitOnReply = true);

  /*! \brief Set a digital output value
   *
   * Set device or internal digital value.
   *  Depending on subclass implementation,
   *  if queryDevice is set, output can be updated regarding device response.
   *
   * \param digitalOutput Pointer to a digital output object.
   * \param value Value to send/store.
   * \param sendToDevice If true, value is sent to device by calling writeDigitalOutput(), else value is only cached.
   *                      Behaviour of this method can vary, depending on device specific subclass.
   *                      (See sublcass writeDigitalOutput() for details).
   * \param waitOnReply If true, this method will wait until reply comes in, or timeout (See waitTransactionDone() ),
   *                     else it will return immediately after the query was sent. \deprecated waitOnReply must not be used anymore
   * \pre digitalOutput must be valid.
   */
  bool setDigitalOutputValue(mdtDigitalIo *digitalOutput, const mdtValue &value, bool sendToDevice, bool waitOnReply = true);

  /*! \brief Set digital output value
   *
   * \param addressWrite Depending on device organisation and protocol,
   *                 this can be a relative or absolute address (f.ex. MODBUS queries),
   *                 a output number, etc...
   *
   * \sa setDigitalOutputValue(mdtDigitalIo*, const mdtValue&, bool, bool)
   */
  bool setDigitalOutputValue(int addressWrite, const mdtValue & value, bool sendToDevice, bool waitOnReply = true);

  /*! \brief Set digital output value
   *
   * \param position See the concept of position in mdtDeviceIos class detailed description .
   *
   * \sa setDigitalOutputValue(mdtDigitalIo*, const mdtValue&, bool, bool)
   */
  bool setDigitalOutputValueAt(int position, const mdtValue &value, bool sendToDevice, bool waitOnReply = true);

  /*! \brief Set digital output value
   *
   * \param labelShort Short label set in I/O (see mdtDigitalIo for details).
   *
   * \sa setDigitalOutputValue(mdtDigitalIo*, const mdtValue&, bool, bool)
   */
  bool setDigitalOutputValue(const QString &labelShort, const mdtValue &value, bool sendToDevice, bool waitOnReply = true);

  /*! \brief Set a value and validity for all digital outputs
   *
   * \sa setDigitalOutputValue(mdtDigitalIo*, const mdtValue&, bool, bool)
   */
  bool setDigitalOutputsValue(const mdtValue & value, bool sendToDevice, bool waitOnReply = true);

  /*! \brief Write all digital outputs to physical device and update (G)UI representation
   *
   * To send the digital outputs values once.
   *  It's possible to use setDigitalOutputValue() without sending the query to device,
   *  then using this method to commit all values once.
   *
   *  See mdtDeviceIos and mdtDigitalIo for details.
   *
   * writeDigitalOutputs() will be called, witch also sends the request to device.
   *  Depending on result, device's state can be updated.
   *
   * Behaviour of this method can vary, depending on device specific subclass.
   *  (See sublcass writeDigitalOutputs() for details).
   *
   * Once results are available, the internal digital outputs are updated.
   *  (See mdtDeviceIos for details).
   *
   * \param waitOnReply If true, this method will wait until reply comes in, or timeout (See waitTransactionDone() ),
   *                     else it will return immediately after the query was sent. \deprecated waitOnReply must not be used anymore
   * \return 0 or a ID on success. If no I/O's are set, on timeout ar other error, a value < 0 is returned.
   */
  bool setDigitalOutputs(bool waitOnReply = true);

 public slots:

  /*! \brief Set value on a analog output on physical device
   *
   * Checks device's state and send request if Ready.
   *
   * This slot is used by mdtDeviceIos to notify that a value has changed.
   *
   * See setAnalogOutputValue(mdtAnalogIo*, const mdtValue&, bool, bool) for details.
   */
  void setAnalogOutputValue(mdtAnalogIo* analogOutput);

  /*! \brief Set value on a digital output on physical device
   *
   * Checks device's state and send request if Ready.
   *
   * This slot is used by mdtDeviceIos to notify that a value has changed.
   *
   * See setDigitalOutputValue(mdtDigitalIo*, const mdtValue&, bool, bool) for details.
   */
  void setDigitalOutputValue(mdtDigitalIo* digitalOutput);

 protected:

  /*! \brief Get a new transaction
   *
   * \sa mdtPortManager::getNewTransaction()
   *
   * \pre portManager must be set before calling this method
   */
  mdtPortTransaction *getNewTransaction();

  /*! \brief Restore a transaction into pool
   *
   * \sa mdtPortManager::restoreTransaction()
   *
   * \pre portManager must be set before calling this method
   */
  void restoreTransaction(mdtPortTransaction *transaction);

  /*! \brief Wait until a transaction is done without break the GUI's event loop
   *
   * \todo Adapt, comment
   *
   * This is a helper method that provide a blocking wait.
   *  Internally, a couple of sleep and event processing
   *  is done, avoiding freezing the GUI.
   *
   * Internally, mdtPortManager::waitTransactionDone() is called.
   *
   * \param id Id returned by query method
   * \return True on success, false on timeout. If id was not found in transactions list,
   *           a warning will be generated in mdtError system, and false will be returned.
   * \pre granularity must be > 0.
   */
  bool waitTransactionDone(int id);

  /*! \brief Read one analog input on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has analog inputs, this method should be implemented.
   *
   * This method is called from getAnalogInputValue() .
   *
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details).
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  virtual int readAnalogInput(mdtPortTransaction *transaction);

  /*! \brief Read all analog inputs on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has analog inputs, this method should be implemented.
   *
   * This method is called from getAnalogInputs() .
   *
   * \param transaction Contains:
   *                      - ioCount : number of I/Os to get
   *                      - address : first I/O address to considere
   *                      - QueryReplyMode flag
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  virtual int readAnalogInputs(mdtPortTransaction *transaction);

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
  virtual int readAnalogOutput(mdtPortTransaction *transaction);

  /*! \brief Read all analog outputs on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has analog inputs, this method should be implemented.
   *
   * This method is called from getAnalogOutputs().
   *
   * \param transaction Contains:
   *                      - ioCount : number of I/Os to get
   *                      - address : first I/O address (for read access) to considere
   *                      - QueryReplyMode flag
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  virtual int readAnalogOutputs(mdtPortTransaction *transaction);

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
   * \pre transaction must be a valid pointer.
   */
  virtual int writeAnalogOutput(int value, mdtPortTransaction *transaction);

  /*! \brief Write all analog outputs to physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has analog outputs, this method should be implemented.
   *
   * This method is called from setAnalogOutputs().
   *
   * \param transaction Contains:
   *                      - ioCount : number of I/Os to set
   *                      - address : first I/O address (for write access) to considere
   *                      - QueryReplyMode flag
   * \param segment Contains contiguous outputs to write.
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  virtual int writeAnalogOutputs(mdtPortTransaction *transaction, mdtDeviceIosSegment *segment);

  /*! \brief Read one digital input on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has digital inputs, this method should be implemented.
   *
   * This method is called from getDigitalInputValue().
   *
   * \param transaction Contains:
   *                      - digitalIo object
   *                      - QueryReplyMode flag
   *                      - address
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  virtual int readDigitalInput(mdtPortTransaction *transaction);

  /*! \brief Read all digital inputs on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has digital inputs, this method should be implemented.
   *
   * This method is called from getDigitalInputs().
   *
   * \param transaction Contains:
   *                      - ioCount : number of I/Os to get
   *                      - address : first I/O address to considere
   *                      - QueryReplyMode flag
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  virtual int readDigitalInputs(mdtPortTransaction *transaction);

  /*! \brief Read one digital output on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has digital outputs, this method should be implemented.
   *
   * This method is called from getDigitalOutputValue().
   *
   * \param transaction Contains some flags used during query/reply process (address, id, I/O object, ...).
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  virtual int readDigitalOutput(mdtPortTransaction *transaction);

  /*! \brief Read all digital outputs on physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has digital outputs, this method should be implemented.
   *
   * This method is called from getDigitalOutputs().
   *
   * \param transaction Contains:
   *                      - ioCount : number of I/Os to get
   *                      - address : first I/O address (for read access) to considere
   *                      - QueryReplyMode flag
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  virtual int readDigitalOutputs(mdtPortTransaction *transaction);

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
  virtual int writeDigitalOutput(bool state, mdtPortTransaction *transaction);

  /*! \brief Write all digital outputs to physical device
   *
   * This is the device specific implementation to send the query.
   *  If device handled by subclass has digital outputs, this method should be implemented.
   *
   * This method is called from setDigitalOutputs().
   *
   * \param transaction Contains:
   *                      - ioCount : number of I/Os to set
   *                      - address : first I/O address (for write access) to considere
   *                      - QueryReplyMode flag
   * \param segment Contains contiguous outputs to write.
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   * \pre I/O's must be set with setIos().
   * \pre transaction must be a valid pointer.
   */
  virtual int writeDigitalOutputs(mdtPortTransaction *transaction, mdtDeviceIosSegment *segment);

  /*! \brief Called when current state has changed
   */
  void stateChangedEvent(State_t newState);

 private:

  /*! \brief Sequence of queries to send periodically
   *
   * This method is called from mdtDevice::runQueries()
   *  and will get analog and digital inputs.
   *
   * \return true if all queries are sent successfully.
   */
  bool queriesSequence();

  /*! \brief Enable I/Os
   */
  void enableIos();

  /*! \brief Disable I/Os
   */
  void disableIos();

  Q_DISABLE_COPY(mdtMultiIoDevice);

  std::shared_ptr<mdtDeviceIos> pvIos;
};

#endif // #ifndef MDT_MULTI_IO_DEVICE_H
