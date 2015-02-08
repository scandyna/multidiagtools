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
#ifndef MDT_DEVICE_H
#define MDT_DEVICE_H

#include "mdtAbstractPort.h"
///#include "mdtPortManager.h"
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
///class mdtDeviceIosSegment;

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
class mdtDevice : public QObject
{
 Q_OBJECT

 public:

  /*! \brief State of device
   */
  enum class State_t
  {
    Disconnected,   /*!< Device is disconnected */
    Connecting,     /*!< A connection is about to be established with the device */
    Ready,          /*!< Device attached/connection established, ready for processing */
    Error           /*!< A unhandled error occured */
  };

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

  /*! \brief Set device identification
   */
  void setIdentification(const QVariant & id);

  /*! \brief Get device identification
   */
  inline QVariant identification() const { return pvIdentification; }

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
   * Will stop periodic querying ( see stop() ).
   *
   * Will also call disconnectFromDeviceEvent().
   */
  void disconnectFromDevice();

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
  //virtual mdtPortManager *portManager();

  /*! \brief Check if device is ready
   *
   * This default implementation returns allways false.
   *
   * Note: isReady() should return true only if device it ready for querying.
   */
  virtual bool isReady()
  {
    return false;
  }

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

  /*! \brief Get current state
   */
  State_t currentState() const
  {
    return pvCurrentState;
  }

  public slots:

  /*! \brief Queries to send periodically
   *
   * This slot is periodically called after a call of start().
   */
  void runQueries();

 ///protected slots:

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
  ///virtual void decodeReadenFrame(mdtPortTransaction *transaction);

 protected:

  /*! \brief Last error object
   */
  mdtError pvLastError;

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

//   /*! \brief Get a new transaction
//    *
//    * \sa mdtPortManager::getNewTransaction()
//    *
//    * \pre portManager must be set before calling this method
//    */
//   mdtPortTransaction *getNewTransaction();
// 
//   /*! \brief Restore a transaction into pool
//    *
//    * \sa mdtPortManager::restoreTransaction()
//    *
//    * \pre portManager must be set before calling this method
//    */
//   void restoreTransaction(mdtPortTransaction *transaction);
// 
//   /*! \brief Wait until a transaction is done without break the GUI's event loop
//    *
//    * \todo Adapt, comment
//    *
//    * This is a helper method that provide a blocking wait.
//    *  Internally, a couple of sleep and event processing
//    *  is done, avoiding freezing the GUI.
//    *
//    * Internally, mdtPortManager::waitTransactionDone() is called.
//    *
//    * \param id Id returned by query method
//    * \return True on success, false on timeout. If id was not found in transactions list,
//    *           a warning will be generated in mdtError system, and false will be returned.
//    * \pre granularity must be > 0.
//    */
//   bool waitTransactionDone(int id);

  /*! \brief Set current state
   *
   * Will call stateChangedEvent() and emit stateChanged() signal.
   *
   * Subclass must call tis function to update current state.
   */
  void setCurrentState(State_t state);

  /*! \brief Called when current state has changed
   *
   * Can be reimplemented in subclass if some actions
   *  are needed when state has changed.
   *
   * This default implementation does nothing,
   *  it's also not necessery to call this base function from subclass.
   */
  virtual void stateChangedEvent(State_t newState)
  {
  }

  /*! \brief Called by disconnectFromDevice()
   *
   * Must be reimplemented in subclass to disconnect from device,
   *  stop threads, close port, ...
   *
   * This default implementation does nothing,
   *  it's also not necessery to call this base function from subclass.
   */
  virtual void disconnectFromDeviceEvent()
  {
  }

 signals:

  /*! \brief Emitted when state has changed
   */
  void stateChanged(State_t state);

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
  ///void stateChanged(int state);

  /*! \brief Emitted when a new status message is to display
   *
   * Typically used with mdtPortStatusWidget
   */
  void statusMessageChanged(const QString &message, const QString &details, int timeout);

//  private slots:
// 
//   /*! \brief Set the device state from port manager state
//    *
//    * Internal port manager handles a state machine.
//    *  When a event occurs, it will emit mdtPortManager::stateChanged().
//    *  If port manager's signal is connected to this slot,
//    *  some things are made in mdtDevice, and
//    *  stateChanged() will be emitted.
//    *
//    * Note for subclass developpers:
//    *  The connection between port manager and mdtDevice
//    *  must be done in subclass.
//    *
//    * \deprecated Use setCurrentState()
//    */
//   void setStateFromPortManager(int portManagerState);

  /*! \brief Set the PortClosed state
   *
   * Emit stateChanged()
   */
  //void setStatePortClosed();

  /*! \brief Set the disconnected state
   *
   * Emit stateChanged()
   */
  //void setStateDisconnected();

  /*! \brief Set the connecting state
   *
   * Emit stateChanged()
   */
  //void setStateConnecting();

  /*! \brief Set the ready state
   *
   * Emit stateChanged()
   */
  //void setStateReady();

  /*! \brief Set the busy state
   *
   * Busy state can be used when physical device or computer (this software) cannot process more requests.
   * Emit stateChanged()
   */
  //void setStateBusy();

  /*! \brief Set the error state
   *
   * Emit stateChanged() if current state was not Error.
   */
  //void setStateError();

 private:

  /*! \brief Allow query timer to run
   */
  void allowQueryTimer();

  /*! \brief Block query timer
   */
  void blockQueryTimer();

  Q_DISABLE_COPY(mdtDevice);

  QString pvName;
  QVariant pvIdentification;
  State_t pvCurrentState;
  QTimer *pvQueryTimer;
  bool pvAutoQueryEnabled;  // Flag used for state handling
  int pvBackToReadyStateTimeout;
  ///QTimer *pvBackToReadyStateTimer;
};

#endif  // #ifndef MDT_DEVICE_H
