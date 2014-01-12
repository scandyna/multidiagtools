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
#ifndef MDT_DEVICE_H
#define MDT_DEVICE_H

#include "mdtDeviceIos.h"
#include "mdtAbstractPort.h"
#include "mdtPortManager.h"
#include "mdtDeviceInfo.h"
#include "mdtValue.h"
#include "mdtError.h"
#include <QVariant>

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QList>

class QTimer;
class mdtDeviceIosSegment;

/*! \brief Base class for a device connected to a port
 *
 * Querying a device can be done several ways.
 *  Some devices sends a confirmation after each query.
 *
 * Some queries (f.ex. read a input value or state) give
 *  a result after query. In some applications, we need
 *  to display the reult priodically. In other application,
 *  we need to send the query and wait on result before
 *  we can continue. This base class gives a interface for
 *  these two cases.
 *
 * At basis, all query methods are not blocking by default.
 *  The normal way is to connect mdtPortManager::newTransactionDone()
 *  to decodeReadenFrame() slot (must be done in subclass).
 *  This way, a query will be sent to device and method returns.
 *  When device returns a result, mdtPortManager will send
 *  the newTransactionDone() signal, and decoding will be processed
 *  in decodeReadenFrame() and I/O's are updated.
 *
 * Because the blocking behaviour is sometimes needed, the concept
 *  of transaction was implemented in mdtPortManager.
 *  At first, we tell the query method (for example getAnalogInputValue() )
 *  that we will wait on a reply by setting the waitOnReply flag.
 *  Then, the query is sent to device and a transaction ID is added
 *  to a list (see mdtPortManager for details). At next, waitTransactionDone()
 *  is called, and finally result is returned.
 *  Note that waitTransactionDone() will not breack the Qt's event loop
 *  because it uses the mdtPortManager::wait() method (see documentation 
 *  of mdtPortManager for details).
 *
 * If it is needed to continiusly update the representation
 *  of device's inputs (digital/analog I/O, voltage, temperature, ...),
 *  two things can happen:
 *  - 1) Used port can receive data continiusly (f.ex. RS232) and device send data automatically
 *  - 2) Used port need that a request is sent for each read (TCP, USB, ...) or device need such request
 * Continous update is typical usefull for a GUI representing a real device.
 *
 * A other usage is automated sequence. In this case, the sequence will tell when it need
 *  to set a output and when a input value is reuqierd.
 *
 * A device can accept one or many request before results must be read. This depends most case
 *  on used port and protocol.
 *
 * It can be useful to find on witch port a device is attached (f.ex. setup of a automated test application).
 *  For this, connectToDevice() was introduced.
 *
 * A device can have several states (ready, busy, disconnected, ...). To help the application programmer
 *  to keep consistency, this states are updated in this class using mdtPortManager 's state machine.
 *  This state machine is based on QStateMachine.
 *
 * Notes about I/O container:
 *  In first version of mdtDevice, I/O container (mdtDeviceIos object) was handled by caller.
 *  Experience showed that this was a bad solution.
 *  Now, the container is created, and destroyed, bay mdtDevice itself.
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
class mdtDevice : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Construct a device object.
   */
  mdtDevice(QObject *parent = 0);

  /*! \brief Destructor
   *
   * Will disconnect from device
   *  and delete I/O container.
   */
  virtual ~mdtDevice();

  /*! \brief Set the device name
   */
  void setName(const QString &name);

  /*! \brief Get device name
   */
  QString name() const;

  /*! \brief Search and connect to physical device.
   *
   * Will scan available ports and open the first port that
   *  has device attached matching request.
   *
   * \param devInfo Requested device's informations.
   * \return A error listed in mdtAbstractPort::error_t (NoError on success)
   *
   * Note: default implementation does nothing and returns allways a UnhandledError.
   *        See subclass documentation for more details.
   */
  virtual mdtAbstractPort::error_t connectToDevice(const mdtDeviceInfo &devInfo);

  /*! \brief Disconnect from device
   *
   * Will stop periodic querying ( see stop() ),
   *  and stop internal port manager (if set).
   *
   * If specific work must be done,
   *  subclass can re-implement this method.
   *
   * Note: in the implementation, ios are keeped as they are.
   */
  virtual void disconnectFromDevice();

  /*! \brief Set the I/O's container
   *
   * \param ios A pointer to a mdtDeviceIos object
   * \param autoOutputUpdate If true, mdtAnalogIo::valueChanged() signal will be connected to
   *                                setAnalogOutputValue(int, int) slot for each analog output
   *                            and mdtDigitalIo::stateChanged() signal will be connected to
   *                                setDigitalOutputState(int, bool) slot for each digital output.
   *
   * \pre ios must be a valid pointer
   */
  ///void setIos(mdtDeviceIos *ios, bool autoOutputUpdate = false);

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

  /*! \brief Get the I/Os container
   *
   * Note: once this mdtDevice object was destroyed,
   *  the I/O container is deleted.
   *
   * \post During the lifetime of this mdtDevice object,
   *        a valid pointer will be returned.
   */
  mdtDeviceIos *ios();

  /*! \brief Set back to ready state timeout
   *
   * After some event (device disconnected, response timeout, ...)
   *  the state can be changed from Ready to another.
   *
   * Note: if state is different than Ready, the slots setAnalogOutputValue(int)
   *  and setDigitalOutputState(int) will return immediately, without
   *  trying to send a request.
   *
   * In some application, it can be usefull that state comes back
   *  automatically to ready.
   *
   * \param timeout Time before go back to ready state [ms]
   *                 If timeout is < 0, device still in current state,
   *                 and setStateReady() can be used.
   */
  void setBackToReadyStateTimeout(int timeout);

  /*! \brief Get internal port manager instance
   *
   * Note that port manager is set by subclass,
   *  and that a Null pointer can be returned.
   *  (See subclass documentation for details)
   */
  virtual mdtPortManager *portManager();

  /*! \brief Check if device is ready
   *
   * This default implementation calls mdtPortManager::isReady() ,
   *  or returns false if portManager was not set.
   *
   * Note: isReady() should return true only if device it ready for querying.
   */
  virtual bool isReady();

  /*! \brief Start periodic device querying
   *
   * Internally, the runQueries() slot will be called each queryInterval of time.
   *
   * Note: if device is ready, querying starts immediatly,
   *        else it will start once device becomes ready.
   *
   * \sa isReady()
   * \param queryInterval Query interval [ms]
   */
  void start(int queryInterval);

  /*! \brief Stop periodic device querying
   */
  void stop();

  /*! \brief Wait some time
   *
   * \sa mdtPortManager::wait()
   * \pre Port manager must be set before using this method
   */
  void wait(int ms);

  /*! \brief Get last error
   *
   * \todo Not completly used currently, must be implemented in this class + subclasses .
   */
  mdtError lastError() const;

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
   *                     else it will return immediately after the query was sent.
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
  mdtValue getAnalogInputValue(int address, bool queryDevice, bool waitOnReply);

  /*! \brief Get analog input value
   *
   * \param position See the concept of position in mdtDeviceIos class detailed description .
   *
   * \sa getAnalogInputValue(mdtAnalogIo*, bool, bool)
   */
  mdtValue getAnalogInputValueAt(int position, bool queryDevice, bool waitOnReply);

  /*! \brief Get analog input value
   *
   * \overload getAnalogInputValue(mdtAnalogIo*, bool, bool)
   *
   * \param labelShort Short label set in I/O (see mdtAnalogIo for details).
   */
  mdtValue getAnalogInputValue(const QString &labelShort, bool queryDevice, bool waitOnReply);

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
   *                     else it will return immediately after the query was sent.
   * \return 0 or a ID on success. If no I/O's are set, on timeout or other error, a value < 0 is returned.
   */
  int getAnalogInputs(bool waitOnReply);

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
   *                     else it will return immediately after the query was sent.
   * \return A mdtValue with valueDouble as real value and valueInt as device specific encoded fromat (if available).
   *          See mdtValue documentation for details about validity and other flags.
   * \pre analogOutput must be valid.
   */
  mdtValue getAnalogOutputValue(mdtAnalogIo *analogOutput, bool queryDevice, bool waitOnReply);

  /*! \brief Get analog output value
   *
   * \param addressRead Depending on device organisation and protocol,
   *                 this can be a relative or absolute address (f.ex. MODBUS queries),
   *                 a output number, etc...
   *
   * \sa getAnalogOutputValue(mdtAnalogIo*, bool, bool)
   */
  mdtValue getAnalogOutputValue(int addressRead, bool queryDevice, bool waitOnReply);

  /*! \brief Get analog output value
   *
   * \param position See the concept of position in mdtDeviceIos class detailed description .
   *
   * \sa getAnalogOutputValue(mdtAnalogIo*, bool, bool)
   */
  mdtValue getAnalogOutputValueAt(int position, bool queryDevice, bool waitOnReply);

  /*! \brief Get analog output value
   *
   * \overload getAnalogOutputValue(mdtAnalogIo*, bool, bool)
   *
   * \param labelShort Short label set in I/O (see mdtAnalogIo for details).
   */
  mdtValue getAnalogOutputValue(const QString &labelShort, bool queryDevice, bool waitOnReply);

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
   *                     else it will return immediately after the query was sent.
   * \return 0 or a ID on success. If no I/O's are set, on timeout or other error, a value < 0 is returned.
   */
  int getAnalogOutputs(bool waitOnReply);

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
   *                     else it will return immediately after the query was sent.
   * \return 0 or a ID on success. If no I/O's are set, on timeout, on invalid value or other error, a value < 0 is returned.
   * \pre analogOutput must be valid.
   */
  int setAnalogOutputValue(mdtAnalogIo *analogOutput, const mdtValue &value, bool sendToDevice, bool waitOnReply);

  /*! \brief Set analog output value
   *
   * \param addressWrite Depending on device organisation and protocol,
   *                 this can be a relative or absolute address (f.ex. MODBUS queries),
   *                 a output number, etc...
   *
   * \sa setAnalogOutputValue(mdtAnalogIo*, const mdtValue&, bool, bool)
   */
  int setAnalogOutputValue(int addressWrite, const mdtValue &value, bool sendToDevice, bool waitOnReply);

  /*! \brief Set analog output value
   *
   * \param position See the concept of position in mdtDeviceIos class detailed description .
   *
   * \sa setAnalogOutputValue(mdtAnalogIo*, const mdtValue&, bool, bool)
   */
  int setAnalogOutputValueAt(int position, const mdtValue &value, bool sendToDevice, bool waitOnReply);

  /*! \brief Set analog output value
   *
   * \overload setAnalogOutputValue(mdtAnalogIo*, const mdtValue&, bool, bool)
   *
   * \param labelShort Short label set in I/O (see mdtAnalogIo for details).
   */
  int setAnalogOutputValue(const QString &labelShort, const mdtValue &value, bool sendToDevice, bool waitOnReply);

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
   *                     else it will return immediately after the query was sent.
   * \return 0 or a ID on success. If no I/O's are set, on timeout ar other error, a value < 0 is returned.
   */
  int setAnalogOutputs(bool waitOnReply);

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
   *                     else it will return immediately after the query was sent.
   * \return A mdtValue with valueBool. See mdtValue documentation for details about validity and other flags.
   * \pre digitalInput must be valid.
   */
  mdtValue getDigitalInputValue(mdtDigitalIo *digitalInput, bool queryDevice, bool waitOnReply);

  /*! \brief Get digital input value
   *
   * \param address Depending on device organisation and protocol,
   *                 this can be a relative or absolute address (f.ex. MODBUS queries),
   *                 a input number, etc...
   *
   * \sa getDigitalInputValue(mdtDigitalIo*, bool, bool)
   */
  mdtValue getDigitalInputValue(int address, bool queryDevice, bool waitOnReply);

  /*! \brief Get digital input value
   *
   * \param position See the concept of position in mdtDeviceIos class detailed description .
   *
   * \sa getDigitalInputValue(mdtDigitalIo*, bool, bool)
   */
  mdtValue getDigitalInputValueAt(int position, bool queryDevice, bool waitOnReply);

  /*! \brief Get digital input value
   *
   * \overload getDigitalInputValue(mdtDigitalIo*, bool, bool)
   *
   * \param labelShort Short label set in I/O (see mdtDigitalIo for details).
   */
  mdtValue getDigitalInputValue(const QString &labelShort, bool queryDevice, bool waitOnReply);

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
   *                     else it will return immediately after the query was sent.
   * \return 0 or a ID on success. If no I/O's are set, on timeout ar other error, a value < 0 is returned.
   */
  int getDigitalInputs(bool waitOnReply);

  /*! \brief Get digital output value
   *
   * \param digitalOutput Pointer to a digital output.
   * \param queryDevice If true, value is readen from device by calling readDigitalOutput(), else cached value is returned.
   *                      Behaviour of this method can vary, depending on device specific subclass.
   *                      (See sublcass readDigitalOutput() for details).
   * \param waitOnReply If true, this method will wait until reply comes in, or timeout (See waitTransactionDone() ),
   *                     else it will return immediately after the query was sent.
   * \return A mdtValue with valueBool. See mdtValue documentation for details about validity and other flags.
   * \pre digitalOutput must be valid.
   */
  mdtValue getDigitalOutputValue(mdtDigitalIo *digitalOutput, bool queryDevice, bool waitOnReply);

  /*! \brief Get digital output value
   *
   * \param addressRead Depending on device organisation and protocol,
   *                 this can be a relative or absolute address (f.ex. MODBUS queries),
   *                 a output number, etc...
   *
   * \sa getDigitalOutputValue(mdtDigitalIo*, bool, bool)
   */
  mdtValue getDigitalOutputValue(int addressRead, bool queryDevice, bool waitOnReply);

  /*! \brief Get digital output value
   *
   * \param position See the concept of position in mdtDeviceIos class detailed description .
   *
   * \sa getDigitalOutputValue(mdtDigitalIo*, bool, bool)
   */
  mdtValue getDigitalOutputValueAt(int position, bool queryDevice, bool waitOnReply);

  /*! \brief Get digital output value
   *
   * \overload getDigitalOutputValue(mdtDigitalIo*, bool, bool)
   *
   * \param labelShort Short label set in I/O (see mdtDigitalIo for details).
   */
  mdtValue getDigitalOutputValue(const QString &labelShort, bool queryDevice, bool waitOnReply);

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
   *                     else it will return immediately after the query was sent.
   * \return 0 or a ID on success. If no I/O's are set, on timeout ar other error, a value < 0 is returned.
   */
  int getDigitalOutputs(bool waitOnReply);

  /*! \brief Set a digital output value
   *
   * Set device or internal digital value.
   *  Internal value is updated if queryDevice is set.
   *
   * \param digitalOutput Pointer to a digital output object.
   * \param value Value to send/store.
   * \param sendToDevice If true, value is sent device by calling writeDigitalOutput(), else value is only cached.
   *                      Behaviour of this method can vary, depending on device specific subclass.
   *                      (See sublcass writeDigitalOutput() for details).
   * \param waitOnReply If true, this method will wait until reply comes in, or timeout (See waitTransactionDone() ),
   *                     else it will return immediately after the query was sent.
   * \return 0 or a ID on success. If no I/O's are set, on timeout, on invalid value or other error, a value < 0 is returned.
   * \pre digitalOutput must be valid.
   */
  int setDigitalOutputValue(mdtDigitalIo *digitalOutput, const mdtValue &value, bool sendToDevice, bool waitOnReply);

  /*! \brief Set digital output value
   *
   * \param addressWrite Depending on device organisation and protocol,
   *                 this can be a relative or absolute address (f.ex. MODBUS queries),
   *                 a output number, etc...
   *
   * \sa setDigitalOutputValue(mdtDigitalIo*, const mdtValue&, bool, bool)
   */
  int setDigitalOutputValue(int addressWrite, const mdtValue &value, bool sendToDevice, bool waitOnReply);

  /*! \brief Set digital output value
   *
   * \param position See the concept of position in mdtDeviceIos class detailed description .
   *
   * \sa setDigitalOutputValue(mdtDigitalIo*, const mdtValue&, bool, bool)
   */
  int setDigitalOutputValueAt(int position, const mdtValue &value, bool sendToDevice, bool waitOnReply);

  /*! \brief Set digital output value
   *
   * \overload setDigitalOutputValue(mdtDigitalIo*, const mdtValue&, bool, bool)
   *
   * \param labelShort Short label set in I/O (see mdtDigitalIo for details).
   */
  int setDigitalOutputValue(const QString &labelShort, const mdtValue &value, bool sendToDevice, bool waitOnReply);

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
   *                     else it will return immediately after the query was sent.
   * \return 0 or a ID on success. If no I/O's are set, on timeout ar other error, a value < 0 is returned.
   */
  int setDigitalOutputs(bool waitOnReply);

  /*! \brief Get device state
   */
  mdtPortManager::state_t currentState();

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

  /*! \brief Queries to send periodically
   *
   * This slot is periodically called after a call of start().
   */
  void runQueries();

 protected slots:

  /*! \brief Decode incoming frames
   *
   * Subclass notes:
   *  - This default implementation does nothing.
   *  - This slot should be connected with mdtPortManager::newTransactionDone(mdtPortTransaction*) signal.
   *  - In this class, this connection is not made, it is the sublcass responsability to do this.
   *  - Once decoding was done, the concerned I/O(s) sould be updated with new value,
   *     or set to invalid value on error (see mdtValue class).
   *     Doing so will keep (G)UI consistency.
   */
  virtual void decodeReadenFrame(mdtPortTransaction *transaction);

 protected:

  /*! \brief Get last error for write access
   */
  mdtError & lastErrorW();

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

  /*! \brief Sequence of queries to send periodically
   *
   * This method is called from runQueries().
   *
   * \return true if all queries are sent successfully.
   *
   * Subclass notes:<br>
   *  - This default implementation does nothing and allways returns false.
   *  - This method can be reimplemented periodic queries must be sent to device.
   */
  virtual bool queriesSequence();

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

  ///mdtDeviceIos *pvIos;    // I/O's container

 signals:

  /*! \brief Emitted when state has changed
   *
   * See setStateFromPortManager() for some details.
   *
   * This signal should be used by application developpers,
   *  and not directly mdtPortManager::stateChanged().
   *  This is because some actions are made when
   *  entering new state, and stateChanged() is sent after.
   *
   * But, to display current state in a easier way (f.ex. using mdtPortStatusWidget),
   *  mdtPortManager::stateChangedForUi() can be used.
   */
  void stateChanged(int state);

  /*! \brief Emitted when a new status message is to display
   *
   * Typically used with mdtPortStatusWidget
   */
  void statusMessageChanged(const QString &message, const QString &details, int timeout);

 private slots:

  /*! \brief Set the device state from port manager state
   *
   * Internal port manager handles a state machine.
   *  When a event occurs, it will emit mdtPortManager::stateChanged().
   *  If port manager's signal is connected to this slot,
   *  some things are made in mdtDevice, and
   *  stateChanged() will be emitted.
   *
   * Note for subclass developpers:
   *  The connection between port manager and mdtDevice
   *  must be done in subclass.
   */
  void setStateFromPortManager(int portManagerState);

  /*! \brief Set the PortClosed state
   *
   * Emit stateChanged()
   */
  void setStatePortClosed();

  /*! \brief Set the disconnected state
   *
   * Emit stateChanged()
   */
  void setStateDisconnected();

  /*! \brief Set the connecting state
   *
   * Emit stateChanged()
   */
  void setStateConnecting();

  /*! \brief Set the ready state
   *
   * Emit stateChanged()
   */
  void setStateReady();

  /*! \brief Set the busy state
   *
   * Busy state can be used when physical device or computer (this software) cannot process more requests.
   * Emit stateChanged()
   */
  void setStateBusy();

  /*! \brief Set the error state
   *
   * Emit stateChanged() if current state was not Error.
   */
  void setStateError();

 private:

  Q_DISABLE_COPY(mdtDevice);

  QString pvName;
  QTimer *pvQueryTimer;
  bool pvAutoQueryEnabled;  // Flag used for state handling
  int pvBackToReadyStateTimeout;
  QTimer *pvBackToReadyStateTimer;
  mdtDeviceIos *pvIos;    // I/O's container
  mdtError pvLastError;
};

#endif  // #ifndef MDT_DEVICE_H
