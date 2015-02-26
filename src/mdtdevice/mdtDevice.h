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
#include "mdtDeviceAddress.h"
#include "mdtValue.h"
#include "mdtError.h"
#include <QVariant>

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QList>

class QTimer;

/*! \brief Base class for a device connected to a port
 *
 * \todo This detailed description is obselete !
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

  /*! \brief Set device address
   *
   * Will also call deviceAddressChangedEvent().
   *
   * \pre This function must only be called when currentState is Disconnected.
   */
  void setDeviceAddress(const mdtDeviceAddress & address, const QString & alias = QString());

  /*! \brief Get device address
   */
  mdtDeviceAddress deviceAddress() const
  {
    return pvDeviceAddress;
  }

  /*! \brief Get device alias
   */
  QString alias() const
  {
    return pvDeviceAddress.alias();
  }

  /*! \brief Connect to physical device
   *
   * Connect to device set with setDeviceAddress().
   *
   * Whenn implementing connectToDevice() functions
   *  in subclass, it is recommended to set device address
   *  with setDeviceAddress(). This is because some informations
   *  are also available later, f.ex. with deviceAddress(),
   *  alias(). deviceIdString(), witch is useful to build error messages,
   *  also reuse internal device address string object.
   *
   * This default implementation does nothing and allways returns false.
   */
  virtual bool connectToDevice()
  {
    return false;
  }

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

  /*! \brief Wait some time without breaking main event loop
   *
   * \param t time [ms]
   */
  void wait(int t);

  /*! \brief Get last error
   *
   * \todo Not completly used currently, must be implemented in this class + subclasses .
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

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

 protected:

  /*! \brief Get device identification string
   *
   * Can be used for error messages.
   *  Returns a string with \"Device 'alias' : \"
   */
  QString deviceIdString() const
  {
    return tr("Device") + " '" + alias() + "' : ";
  }

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

  /*! \brief Called by setDeviceAddress()
   *
   * Can be reimplemented in subclass if some actions
   *  are needed when device address has changed.
   *
   * This default implementation does nothing,
   *  it's also not necessery to call this base function from subclass.
   */
  virtual void deviceAddressChangedEvent(const mdtDeviceAddress & address)
  {
  }

 signals:

  /*! \brief Emitted when state has changed
   */
  void stateChanged(State_t state);

  /*! \brief Emitted when a new status message is to display
   *
   * Typically used with mdtPortStatusWidget
   */
  void statusMessageChanged(const QString &message, const QString &details, int timeout);

 private slots:

  /*! \brief Set pvWaitTimeReached flag
   *
   * See wait()
   */
  void setWaitTimeReached();

 private:

  /*! \brief Allow query timer to run
   */
  void allowQueryTimer();

  /*! \brief Block query timer
   */
  void blockQueryTimer();

  Q_DISABLE_COPY(mdtDevice);

  mdtDeviceAddress pvDeviceAddress;
  State_t pvCurrentState;
  QTimer *pvQueryTimer;
  bool pvAutoQueryEnabled;  // Flag used for state handling
  bool pvWaitTimeReached;   // Used by wait()
  int pvBackToReadyStateTimeout;
};

#endif  // #ifndef MDT_DEVICE_H
