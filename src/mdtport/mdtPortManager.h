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
#ifndef MDT_PORT_MANAGER_H
#define MDT_PORT_MANAGER_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <QThread>
#include "mdtAbstractPort.h"
#include "mdtPortInfo.h"
#include "mdtPortConfig.h"
#include "mdtPortThread.h"
#include "mdtError.h"
#include "mdtFrame.h"
#include "mdtPortTransaction.h"
#include <QByteArray>
#include <QList>
#include <QQueue>
#include <QMap>

class mdtPortManagerStateMachine;
class QTimer;

/// \todo Port manager has no raison to inherit QThread -> make it inherit QObject

/*! \brief Port manager base class
 *
 * Manages a port based on mdtAbstractPort an several threads based on mdtPortThread.
 *  The goal is to hide the complexity of the port API.
 *
 * Example:
 * \code
 * mdtPortManager m;
 * mdtPort *port;
 * mdtPortConfig *config;
 * QList<QByteArray> responses;
 *
 * // Setup
 * config = new mdtPortConfig;
 * config->setFrameType(mdtFrame::FT_ASCII);
 * config->setEndOfFrameSeq("$");
 *
 * // Init port
 * port = new mdtPort;
 * port->setConfig(config);
 *
 * // Init port manager - We wand blocking mode
 * m.setTransactionsDisabled(true);
 * m.setPort(port);
 * m.addThread(mew mdtPortWriteThread);
 * m.addThread(mew mdtPortReadThread);
 * m.setPortName("/dev/xyz"));
 *
 * // Start port manager - Will open port, start threads and return once port manager is ready
 * if(!m.start()){
 *  // Handle error
 * }
 *
 * // Send some data
 * if(!m.writeData("Test$")){
 *  // Handle error
 * }
 *
 * // Wait on answer - Timout: 1500 [ms]
 * if(!m.waitReadenFrame(1500)){
 *  // Timout , handle error
 * }
 *
 * // Do something with received data
 * responses = m.readenFrames();
 * for(int i=0; i<responses.size(); i++){
 *  qDebug() << responses.at(i);
 * }
 *
 * // Stop port manager
 * m.stop();
 *
 * // Cleanup - detachPort() will delete port and threads objects
 * m.detachPort(true, true);
 * delete config;
 *
 * \endcode
 *
 * A (recommended) alternative is to disable frames enqueuing and use the newReadenFrame(QByteArray) signal.
 *  - Disable enqueuing with setTransactionsDisabled(false)
 *  - Connect newReadenFrame(QByteArray) to a slot
 *
 * ---------------------------------------------------
 *
 * Manage a port based on mdtAbstractPort and related threads.
 *  Goal is to hide complexity of mdtAbstractPort API.
 *
 * Depending on application, it's possible to use a event approach,
 *  or also a blocking approach with wait methods.
 *
 * Some protocols support frame identifier (like MODBUS/TCP, USBTMC, or others),
 *  and other don't (for example raw bytes transfert on a serial port).
 *  For this reason, the concept of transactions is aodpted here.
 *  A transaction is based on mdtPortTransaction class.
 *  It was introduced for protocols that support identification, and can store some
 *  other meta data, usefull to decode a reply.
 *
 * This base class can be used directly for ports that supports raw transfers.
 *  For specific protocol, it can be subclassed.
 *
 * \todo Document init, open, close, ...
 *
 * Now, we can see the data transfer workflow:
 *  - 1) Wait until it's possible to send some data with waitOnWriteReady()
 *  - 2) Get a new transaction with getNewTransaction() , and setup it as needed (see remarks below).
 *  - 3) Send data using writeData(mdtPortTransaction*). This will set the currentTransactionId to the ID given in transaction.
 *  - 4a) If you choosed a blocking approach, use waitTransactionDone().
 *  - 4b) If you choosed a event approach, connect the transactionDone() signal
 *         to a slot. Each time a transaction is done, you will receive a transaction.
 *  - 5) Get the transaction with transactionDone().
 *  - 6) Restore transaction with restoreTransaction().
 *
 * Steps 2) and 3) can be done once by using writeData(const QByteArray&, bool).
 * Steps 5) and 6) can be done once by using getReadenData(int).
 *
 * Here is a little example of the simple approach:
 * \code
 * mdtPortManager m;
 * int transactionId;
 * // Setup...
 *
 * // Wait until we can write
 * if(!waitOnWriteReady()){
 *   // Handle error
 * }
 *
 * // Send data to port
 * transactionId = writeData("SomeData", true);
 * if(transactionId < 0){
 *   // Handle error
 * }
 *
 * // Wait until data come in
 * if(waitTransactionDone(transactionId)){
 *   // Handle error
 * }
 *
 * // Get only data for a specific transaction (should be used only for protocols that support transaction)
 * qDebug() << readenFrame(transactionId);
 * // Get all received data
 * qDebug() << readenFrames();
 * \endcode
 *
 * Internally, once data was send to port's write queue, the transaction is stored
 *  in the pending transactions queue.
 *
 * Dealing with incomming data:
 *  In above example, 2 methods are shown to get incomming data:
 *   - readenFrame(int) : will work with protocols that support frame ID. See specific port manager subclass for details.
 *   - readenFrames() : see below.
 *
 *  If we are using a protocol that not support frame ID, it's possible that we receive
 *  data each time, also without sending any request.
 *  This must be handled in fromThreadNewFrameReaden() slot.
 *  In this mdtPortManager class, the following will happen:
 *   - When sending data with writeData(const QByteArray&, bool), current transaction ID will be incremented with incrementCurrentTransactionId().
 *   - In fromThreadNewFrameReaden() slot, currentTransactionId() is used to get pending transaction with transactionPending().
 *      If it was found, frame's data is copied, and transaction is moved to done queue.
 *      If not found, a new transaction is taken with getNewTransaction(), frame's data is copied,
 *       and transaction is enqueued to done queue with addTransactionDone().
 *   - readenFrames() will return a list of data that comes from transactions Done list. Current transaction ID will be restet to 0.
 *
 * The Query/Reply mode:
 *  port manager is able to handle event or blocking mode in same time.
 *  This cause a problem for done transactions handling.
 *  For example, when we choosed the event based aproach for a transaction,
 *   port manager will emit the newTransactionDone(mdtPortTransaction*) signal,
 *   then restore transaction back to pool.
 * When we choosed blocking approach, port manager will also emit newTransactionDone(mdtPortTransaction*) signal,
 *  wich can be usefull to keep, for example, a GUI coherent in each mode. But here, the transaction must not be
 *  restored directly, but only once data was readen with readenFrame() or readenFrames().
 *  To handle this, commitFrames() will use mdtPortTransaction::isQueryReplyMode().
 */
class mdtPortManager : public QThread
{
 Q_OBJECT

 public:

  /*! \brief State of device
   *
   * States are handled by mdtPortManagerStateMachine
   */
  enum state_t {
                PortClosed = 0 ,  /*!< Port is closed */
                Stopped ,         /*!< Threads are stopped and port is closing */
                Starting ,        /*!< Threads are starting */
                Stopping ,        /*!< Threads are stopping */
                Running ,         /*!< Global state (sub machine) */
                PortError ,       /*!< A unhandled error occured - Threads will be stopped and port closed */
                PortReady ,       /*!< Port is open and threads are running.
                                        At this point, communication is possible if not connection is required (f.ex. serial port terminal) */
                Disconnected ,    /*!< Port is ready, but not connected to device */
                Connecting,       /*!< Trying to connect to device */
                Connected ,       /*!< Global state (sub machine) */
                Ready ,           /*!< Port is ready and connection to device was made. Communication is possible */
                Busy ,            /*!< Port is up and device is connected but cannot accept requests for the moment */
                Warning,          /*!< \todo Obselete ! - Device or port communication handled error occured */
                Error             /*!< \todo Obselete ! - Device or port communication unhandled error occured */
               };

  /*! \brief Contruct a port manager
   *
   * keepTransactionsDone flag will be unset (set false).
   */
  mdtPortManager(QObject *parent = 0);

  /*! \brief Destructor
   *
   * If a port was set, the manager will stop (if running), and port will be closed (if open).
   *
   * Note that port set by setPort() and threads are not deleted.
   *  This is because port and threads are set by subclass, or from application.
   */
  virtual ~mdtPortManager();

  /*! \brief Scan for available ports
   *
   * This method is implemented is port's specific subclass.
   *  Default implementation returns a empty list.
   *
   * Note that returned list must be freed by user
   *  after usage. (for.ex. with qDeletAll() and QList::clear() ).
   */
  virtual QList<mdtPortInfo*> scan();

  /*! \brief Set port object
   *
   * \pre port must be a valid pointer to the expected class instance (for ex: mdtSerialPort).
   * \pre Port manager must be closed (see isClosed() ).
   */
  virtual void setPort(mdtAbstractPort *port);

  /*! \brief Lock the port's mutex
   *
   * This method will check a internal flag before locking port's mutex.
   *  If the flag syas that mutex is allready locked,
   *  a warning is logged with mdtError system.
   *  Then, this method reurns without locking the port mutex again.
   *
   * \pre Port must be set with setPort before using this method
   */
  void lockPortMutex();

  /*! \brief Unlock port's mutex
   *
   * \pre Port must be set with setPort before using this method
   */
  void unlockPortMutex();

  /*! \brief Detach port
   *
   * Will detach port from each thread and from port manager.
   *  If port was not set, this method does nothing.
   *  If manager is running, it will be stopped.
   *  If port is open, it will be closed.
   *
   * \param deletePort If true, the port object that was set with setPort() will be deleted.
   * \param deleteThreads If true, each thread added with addThread() will be deleted.
   */
  void detachPort(bool deletePort, bool deleteThreads);

  /*! \brief Add a thread and assign it to port
   *
   * \pre Port must be set with setPort before using this method
   * \pre Manager must no running
   * \pre Port manager must be closed (see isClosed() ).
   */
  void addThread(mdtPortThread *thread);

  /*! \brief Returns instance of reader thread.
   *
   * Note that a Null pointer can be returned
   */
  mdtPortThread *readThread();

  /*! \brief Returns instance of writer thread.
   *
   * Note that a Null pointer can be returned
   */
  mdtPortThread *writeThread();

  /*! \brief Detach threads from port and remove threads
   *
   * \param releaseMemory If true, all threads are deleted
   * \pre Port manager must be closed (see isClosed() ).
   */
  void removeThreads(bool releaseMemory);

  /*! \brief Start port manager
   *
   * Will open port and start threads.
   *  This method returns once port manager is ready (see isReady() ).
   *  Note: event loop continues to work during wait of ready state, GUI will also not freeze
   *
   * \pre Port must be set with setPort() before use of this method.
   */
  virtual bool start();

  /*! \brief Stop port manager
   *
   * Will stop threads and close port.
   *  This method returns once port manager is closed ( see isClosed() ).
   *  Note: event loop continues to work during wait of ready state, GUI will also not freeze
   *
   * \pre Port must be set with setPort() before use of this method.
   */
  void stop();

  /*! \brief Set port name
   *
   * Set the port name to internally port object.
   * Does nothing else. To open the port, use start().
   *
   * \pre Port must be set before with setPort()
   * \pre Port manager must be closed (see isClosed() ).
   * \sa mdtAbstractPort
   */
  void setPortName(const QString &portName);

  /*! \brief Set port info
   *
   * Store given port info, and set port name to internall port object.
   *  (see mdtPortInfo::portName() ).
   *
   * Setting a port info can be usefull if other informations are
   *  needed later in application (f.ex. mdtPortInfo::displayText() ).
   * You can get port informations later with portInfo().
   *
   * \pre Port must be set before with setPort()
   * \pre Port manager must be closed (see isClosed() ).
   */
  void setPortInfo(mdtPortInfo info);

  /*! \brief Get current port info
   */
  mdtPortInfo portInfo();

  /*! \brief Get current port name
   */
  QString portName() const;

  /*! \brief Get the port's config object
   * 
   * Usefull to alter internal port configuration
   *
   * \pre Port must be set with setPort() before use of this method.
   */
  virtual mdtPortConfig &config();

  /*! \brief Get the port's config object
   * 
   * Usefull to read internal port configuration
   *
   * \pre Port must be set with setPort() before use of this method.
   */
  mdtPortConfig &config() const;

  /*! \brief Force transactions done to be keeped
   *
   * If keep is true, commitFrames() will
   *  ignore the mdtPortTransaction::isQueryReplyMode() and allways
   *  keep transactions in done queue until
   *  readenFrame() or readenFrames() is called.
   */
  void setKeepTransactionsDone(bool keep);

  /*! \brief Get the keepTransactionsDone flag
   */
  bool keepTransactionsDone() const;

  /*! \brief Get a new transaction
   *
   * If transactions pool is empty, a new transaction is created.
   *  Note that each transaction should be put back in
   *  pool with restoreTransaction().
   *  In most cases, if a method that uses transactions fails, the transaction
   *  must be restored with restoreTransaction().
   *  See writeData(mdtPortTransaction*), writeData(const QByteArray&),
   *   flushTransactionsPending(), flushTransactionsDone(), readenFrame() and readenFrames()
   *   for details about restoration of transaction.
   *
   * \return A empty transaction (mdtPortTransaction::clear() is called internally).
   *
   * \post Returned transaction is valid (never Null).
   */
  mdtPortTransaction *getNewTransaction();

  /*! \brief Restore a transaction into pool
   *
   * \pre transaction must be a valid pointer (not Null)
   * \pre One transaction must be commited once (avoid corrupted double free)
   */
  void restoreTransaction(mdtPortTransaction *transaction);

  /*! \brief Wait until data can be written
   *
   * Wait until state is ready and that a frame is available in write frames pool.
   *  The wait will not breack Qt's event loop, so this method
   *  can be called from GUI Thread. (See wait() for details).
   *
   * \param timeout Timeout [ms].
   *              If < 0, a infinite timeout is considered,
   *              else timeout will be used, adjusted to the minimal timeout (see adjustedWriteTimeout() ).
   *              Note: if a timeout > 0 is given, and it's lesser than defined in port configuration,
   *               a warning will be logged. To avoid this, use a timeout > than in configuration, or 0.
   * \param granularity Sleep time between each call of event processing [ms].
   *                     A little value needs more CPU and big value can freese the GUI.
   *                     Should be between 50 and 100, and must be > 0.
   *                     Note that timeout must be a multiple of granularity.
   * \return True if a frame is available before timeout, false else.
   * \pre Granularity must be > 0.
   * \pre Port must be set with setPort() before calling this method.
   * 
   * \todo Obselete
   */
  bool waitOnWriteReady(int timeout = 0, int granularity = 50);

  /*! \brief Send data on port
   *
   * At first, this method waits until the ready state is set calling isReady() , and a frame is available in port's write frames pool.
   *  This wait will not break event loop, so no GUI freeze occurs.
   *  If port manager is stopped during this wait (unhandled error, stop request),
   *  this method returns.
   *
   * Then, data contained in transaction will be passed to the mdtPort's write queue by copy.
   *  This method returns immediatly after enqueue,
   *  and don't wait until data was written.
   *
   * \param transaction Transaction used to send data. Following members are used by this method:
   *                     - id : transaction will be added to pending transactions with this id,
   *                            and currentTransactionId will be set with it.
   *                     - data : will be sent to port.
   *                     - isQueryReplyMode : if true, the transaction will be keeped in transactions done queue
   *                                          until readenFrame() or readenFrames() is called.
   *                                          Note: it's possible to force keeping all incomming data (wich also owerwrite this flag)
   *                                           by setting the global keepTransactionsDone flag.
   *
   * \return Transaction ID on success or value < 0 on error.
   *          In this implementation, the only possible error is mdtAbstractPort::WriteCanceled,
   *          witch typically occurs when port manager stops.
   *          Some subclass can return a frame ID on success,
   *          or a other error. See subclass documentation for details.
   *          Note: on failure, the transaction is restored to pool.
   * \pre Port must be set with setPort() before use of this method.
   * \pre transaction must be a valid pointer, and not allready exists in transactions pending or transactions done queue.
   *
   * Subclass notes:<br>
   *  This method can be reimplemented in subclass if needed.
   *  Typically usefull if some encoding is needed before the
   *  frame is submitted to port.
   *  A frame must be taken from port's write frames pool with mdtAbstractPort::writeFramesPool()
   *  dequeue() method (see Qt's QQueue documentation for more details on dequeue() ),
   *  then added to port's write queue with mdtAbstractPort::addFrameToWrite() .
   *  If protocol supports frame identification (like MODBUS's transaction ID or USBTMC's bTag),
   *   it should be returned here and incremented using incrementCurrentTransactionId().
   */
  virtual int sendData(mdtPortTransaction *transaction);

  /*! \brief Send data to port
   *
   * Will increment the current transaction ID,
   *  get a new transaction, setup it
   *  and finally send it with sendData(mdtPortTransaction*).
   *
   * \param data Data to write.
   * \param queryReplyMode If true,
   *                        reply will be keeped in transactions done queue
   *                        until readenFrame() or readenFrames() is called.
   *                       If false, behaviour depends on keepTransactionsDone flag.
   *
   * \sa sendData(mdtPortTransaction*)
   */
  virtual int sendData(const QByteArray &data, bool queryReplyMode = false);

  /*! \brief Wait until a transaction is done
   *
   * Will return when transaction with given id is done or after timeout.
   *
   * This wait method does not break Qt's event loop, no GUI freeze occurs.
   *
   * This method can return if timeout occurs, or for other
   *  reason depending on specific port (port timeout, read cancelled, ...).
   *
   * \param id Frame ID. Depending on protocol, this can be a transaction ID or what else.
   * \return True if Ok, false on timeout or other error. If id was not found in transactions pending lists,
   *           a warning will be generated in mdtError system, and false will be returned.
   *           On failure, transaction is restored to pool (see onThreadsErrorOccured() for details).
   */
  bool waitTransactionDone(int id);

  /*! \brief Wait until current transaction is done
   *
   * Use this method if protocol does not support
   *  frame ID.
   *
   * Internally, it will wait until one transaction is available
   *  in done queue.
   *
   * \return True if Ok, false on timeout or other error.
   */
  bool waitOneTransactionDone();

  /*! \brief Get data by frame ID
   *
   * The frame ID is a protocol specific identification.
   *  F.ex. in MODBUS/TCP, the transaction ID is used,
   *  or bTag for USBTMC.
   *
   * If found, the frame is removed from done queue.
   *  (A second call with same ID will return a empty QByteArray).
   *
   * If ID was not found, a empty QByteArray is returned.
   */
  QByteArray readenFrame(int id);

  /*! \brief Get all readen data
   *
   * Note that calling this methode will clear the internal queue.
   *  (A second call will return a empty list).
   */
  QList<QByteArray> readenFrames();

  /*! \brief Adjust a requested timeout to minimal timeout
   *
   * We have 2 different timeouts:
   *  - Port timeout: If data are expected on the port, and nothing comes in,
   *     a port timeout occurs.
   *  - Response (or complete frame) timeout: Timeout to receive a complete frame
   *
   * For example, we expect a complete frame of, say, 200 Bytes.
   *  The device will send , f.ex. , 20 x 10 Bytes.
   *  During the read process, a port timeout can occur (device busy, or something else).
   *  If read process works fine, all data can be readen in a time less than port timeout.
   *
   * \param requestedTimeout Requested timeout [ms]
   * \param warn If true, a warning will be logged if requested timeout is to small.
   * \return Requested timeout or port's timeout + offset (offset is currently hardcoded: 1000 [ms])
   * \pre Port must be set with setPort() and contain a valid configuration.
   */
  int adjustedReadTimeout(int requestedTimeout, bool warn = true) const;

  /*! \brief Adjust a requested timeout to minimal timeout
   *
   * See adjustedReadTimeout() for details.
   *
   * \param requestedTimeout Requested timeout [ms]
   * \param warn If true, a warning will be logged if requested timeout is to small.
   * \return Requested timeout or port's timeout + offset (offset is currently hardcoded: 1000 [ms])
   * \pre Port must be set with setPort() and contain a valid configuration.
   */
  int adjustedWriteTimeout(int requestedTimeout, bool warn = true) const;

  /*! \brief Wait some time without break the GUI's event loop
   *
   * This is a helper method that provide a blocking wait.
   *  Internally, a couple of sleep and event processing
   *  is done, avoiding freesing the GUI.
   *
   * This wait method is not precise.
   *
   * \param msecs Time to wait [ms]
   * \param granularity Sleep time between each call of event processing [ms]<br>
   *                     A little value needs more CPU and big value can freese the GUI.
   *                     Should be between 50 and 100, and must be > 0.
   *                     Note that msecs must be a multiple of granularity.
   * \pre granularity must be > 0.
   */
  static void wait(int msecs, int granularity = 50);

  /*! \brief Flush input buffers
   *
   * \param flushPortManagerBuffers If true, port manager buffers (transactions done and pending) will be cleared.
   * \param flushPortBuffers If true, port's buffers will be cleared.
   * \pre Port must be set with setPort() before calling this method.
   */
  virtual void flushIn(bool flushPortManagerBuffers, bool flushPortBuffers);

  /*! \brief Flush input buffers
   *
   * Will flush portmanger and port's buffers.
   *
   * Internally, flushIn(bool, bool) is called.
   */
  void flushIn();

  /*! \brief Flush output buffers
   *
   * \param flushPortManagerBuffers If true, port manager buffers will be cleared.
   *                                 (Has no effect in this implementation, but some subclass can have output buffers).
   * \param flushPortBuffers If true, port's buffers will be cleared.
   * \pre Port must be set with setPort() before calling this method.
   */
  virtual void flushOut(bool flushPortManagerBuffers, bool flushPortBuffers);

  /*! \brief Flush output buffers
   *
   * Will flush portmanger and port's buffers.
   *
   * Internally, flushOut(bool, bool) is called.
   */
  void flushOut();

  /*! \brief Flush I/O buffers
   *
   * \param flushPortManagerBuffers If true, port manager buffers will be cleared.
   *                                 (Has no effect in this implementation, but some subclass can have output buffers).
   * \param flushPortBuffers If true, port's buffers will be cleared.
   * \pre Port must be set with setPort() before calling this method.
   */
  void flush(bool flushPortManagerBuffers, bool flushPortBuffers);

  /*! \brief Flush I/O buffers
   *
   * \pre Port must be set with setPort() before calling this method.
   */
  void flush();

  /*! \brief Get current state
   */
  state_t currentState() const;

  /*! \brief Check if port manager is ready
   *
   * Internally, the currentState is used to check if port manager is ready.
   *  This implementation returns true if currentState is PortReady or Ready.
   *
   * For some port type, it can be mandatory to be connected to device before any data
   *  can be transferred. In such case, reimplement this method.
   */
  virtual bool isReady() const;

  /*! \brief Check if port is closed
   *
   * Returns true if threads are all stopped and port is closed
   */
  bool isClosed() const;

 public slots:

  /*! \brief Cancel read and write operations
   *
   * Default implementation calls mdtAbstractPort::flush().
   */
  virtual void abort();

 signals:

  /*! \brief Emitted each time a transaction is done.
   *
   * Note: the transaction pointer is only valid for one call,
   *  do not store it for later usage.
   *  Connect this signal to a slot that runs in GUI thread only.
   *
   * This signal is emited by commitFrames().
   */
  void newTransactionDone(mdtPortTransaction *transaction);

  /*! \brief Emitted when error number has changed
   *
   * The error is one of the mdtAbstractPort::error_t.
   */
  void errorStateChanged(int error, const QString & message = QString(), const QString & details = QString());

  /*! \brief Emitted when a new status message is to display
   *
   * Typically used with mdtDeviceStatusWidget.
   */
  void statusMessageChanged(const QString &message, const QString &details, int timeout);

  /*! \brief Emitted when state has changed
   * \todo Obselete ?
   */
  void stateChanged(int newState);

  /*! \brief Sent when port was closed
   *
   * Used by state machine
   */
  void pmPortClosedEvent();

  /*! \brief Sent when threads must be started
   *
   * Used by state machine
   */
  void pmStartThreadsEvent();

  /*! \brief Sent when threads must be stopped
   *
   * Used by state machine
   */
  void pmStopThreadsEvent();

  /*! \brief Sent when all threads are ready
   *
   * Used by state machine
   */
  void pmAllThreadsReadyEvent();

  /*! \brief Sent when all threads are stopped
   *
   * Used by state machine
   */
  void pmAllThreadsStoppedEvent();

  /*! \brief Sent when a unhandled error occured
   *
   * Used by state machine
   */
  void pmUnhandledErrorEvent();

  /*! \brief Sent when connection with device beginns
   *
   * Used by state machine
   */
  void pmConnectingEvent();

  /*! \brief Sent when connection with device is established
   *
   * Used by state machine
   */
  void pmConnectedEvent();

  /*! \brief Sent when connection with device was lost/closed
   *
   * Used by state machine
   */
  void pmDisconnectedEvent();

  /*! \brief Sent when device is busy (or other slow down reason..)
   *
   * Used by state machine
   */
  void pmBusyEvent();

  /*! \brief Sent when device and communication becomes ready again
   *
   * Used by state machine
   */
  void pmReadyEvent();

  /*! \brief Sent when a transaction timeout occured
   *
   * A transaction timeout can be notified by a port thread,
   *  or/and internall transaction timer.
   *
   * \sa transactionTimeoutOccured()
   * \sa onTransactionTimeoutOccured()
   * \sa startTransactionTimer()
   * \sa stopTransactionTimer()
   */
  void pmTransactionTimeoutEvent();

 ///public:  /// \todo make protected

 protected:

  /*! \brief Open the port
   *
   * Will try to open port defined with setPortName() or setPortInfo()
   *
   * \return True on success, false else.
   * \pre Port must be set with setPort() before use of this method.
   * \todo Should return mdtAbstractPort::error_t
   */
  virtual bool openPort();

  /*! \brief Close the port
   *
   * If port was never set (with setPort() ), this method does nothing.
   */
  void closePort();

 ///protected:

  /*! \brief Set the current transaction ID
   */
  void setCurrentTransactionId(int id);

  /*! \brief Get the current transaction ID
   */
  int currentTransactionId() const;

  /*! \brief Increment current transaction ID
   */
  void incrementCurrentTransactionId(int min = 0, int max = INT_MAX);

  /*! \brief Add a transaction to pending queue
   *
   * Add a existing transaction to the pending queue.
   *  A transaction can be requested with getNewTransaction().
   *
   * \pre transaction must be a valid pointer.
   * \pre transaction ID must not allready exist in queue.
   */
  void addTransactionPending(mdtPortTransaction *transaction);

  /*! \brief Take pending transaction matching id
   *
   * \return A valid transaction if exists, else a Null pointer.
   */
  mdtPortTransaction *transactionPending(int id);

  /*! \brief Restore all transactions pending to pool
   */
  void flushTransactionsPending();

  /*! \brief Add a transaction to the DONE queue
   *
   * \pre transaction must be a valid pointer.
   */
  void addTransactionDone(mdtPortTransaction *transaction);

  /*! \brief Check if a transaction with given ID exists in done queue
   */
  bool transactionsDoneContains(int id) const;

  /*! \brief Take transaction from DONE queue matching id
   *
   * \return A valid transaction if exists, else a Null pointer.
   */
  mdtPortTransaction *transactionDone(int id);

  /*! \brief Restore all transactions done to pool
   */
  void flushTransactionsDone();

  /*! \brief Start transaction timer
   *
   * \param timeout Timeout [ms]
   *
   * \sa transactionTimeoutOccured()
   * \sa pmTransactionTimeoutEvent()
   * \sa onTransactionTimeoutOccured()
   * \sa stopTransactionTimer()
   */
  void startTransactionTimer(int timeout);

  /*! \brief Stop transaction timer
   *
   * \sa startTransactionTimer()
   * \sa transactionTimeoutOccured()
   * \sa pmTransactionTimeoutEvent()
   * \sa onTransactionTimeoutOccured()
   */
  void stopTransactionTimer();

  /*! \brief Check if a transaction timeout occured
   *
   * \sa pmTransactionTimeoutEvent()
   * \sa onTransactionTimeoutOccured()
   * \sa startTransactionTimer()
   * \sa stopTransactionTimer()
   */
  bool transactionTimeoutOccured() const;

  /*! \brief Emit signals for each done transactions
   *
   * Will emit newTransactionDone(mdtPortTransaction*) for
   *  each transaction found in done queue.
   *
   * If queryReplyMode and keepTransactionsDone are not set, the transaction is restored
   *  back to pool, else it will be restored only after a call of
   *  readenFrame() or readenFrames().
   */
  void commitFrames();

 protected slots:

  /*! \brief Called by the read thread whenn a complete frame was readen
   *
   * This variant will add data to transactions done queue.
   *  If pending transactions queue contains currentTransactionId(),
   *  it will be removed.
   *
   * For subclass that handle frames with IDs, this method must be reimplemented.
   *  The normal way is to get the frame in pending queue with transactionPending().
   *  For frames with ID that not exists in pending queue, it must be choosen to discard them,
   *  or adopt another behaviour.
   *
   * \sa mdtPortThread
   */
  virtual void fromThreadNewFrameReaden();

  /*! \brief Manage errors comming from port threads
   *
   * This implementation will handle some common errors,
   *  change the current state emiting transistion signal.
   *  If state has changed, stateChanged() signal is emited.
   *
   * Handled errors are:
   *  - NoError: go to ready state.
   *  - WriteCanceled: flush pending transaction with flushTransactionsPending() and go to warning state.
   *  - WriteTimeout: flush pending transaction with flushTransactionsPending() and go to busy state.
   *  - WritePoolEmpty: go to busy state.
   *  - ReadCanceled: flush pending transaction with flushTransactionsPending(),
   *                    done transactions with flushTransactionsDone() and go to warning state.
   *  - ReadTimeout: flush pending transaction with flushTransactionsPending(),
   *                    done transactions with flushTransactionsDone() and go to busy state.
   *  - ReadPoolEmpty: go to busy state.
   *  - Disconnected: flush pending transaction with flushTransactionsPending(),
   *                    done transactions with flushTransactionsDone() and go to disconnected state.
   *  - Connecting: go to connecting state.
   *
   * For other errors, including UnhandledError,
   *  pending transactions are flushed with flushTransactionsPending(),
   *  done transactions are flushed with flushTransactionsDone()
   *  and state will become error state.
   *
   * For other cases, the subclass can reimplement this method.
   *  Dont forget to emit transistion signals.
   */
  virtual void onThreadsErrorOccured(int error);

 protected:

  mdtAbstractPort *pvPort;
  QList<mdtPortThread*> pvThreads;  /// \todo move to private

 private slots:

  /*! \brief Called by threads when they are ready
   *
   * When a thread is started and ready it will emit his ready() signal.
   *  Given thread will also be added to ready threads list
   *
   * If all threads contained in pvThreads have called this method, pmAllThreadsReadyEvent is emitted
   */
  void onThreadReady(mdtPortThread *thread);

  /*! \brief Called by threads when they are stopped
   *
   * If all threads contained in pvThreads have called this method, pmAllThreadsStoppedEvent is emitted
   */
  void onThreadStopped(mdtPortThread *thread);

  /*! \brief Start threads
   */
  void startThreads();

  /*! \brief Stop threads
   */
  void stopThreads();

  /*! \brief Called when a transaction timeout occurs
   *
   * Will remove transaction pending and done from queues.
   *  The internall transaction timer will be stopped.
   *
   * After a call to this method, transactionTimeoutOccured() will return true.
   *
   * \sa pmTransactionTimeoutEvent()
   * \sa transactionTimeoutOccured()
   * \sa startTransactionTimer()
   * \sa stopTransactionTimer()
   */
  void onTransactionTimeoutOccured();

 private:

  /*! \brief Wait until ready state
   */
  bool waitOnReadyState();

  mdtPortInfo pvPortInfo;
  bool pvPortMutexLocked;                               // Used by lockPortMutex()

  // Transactions members
  int pvCurrentTransactionId;
  bool pvKeepTransactionsDone;                          // This flag overwrites the transaction's queryReplyMode flag
  QQueue<mdtPortTransaction*> pvTransactionsPool;
  int pvTransactionsAllocatedCount;                     // Used to watch how many transactions are allocated (memory leack watcher)
  QMap<int, mdtPortTransaction*> pvTransactionsPending; // Used for query that are sent to device
  QQueue<mdtPortTransaction*> pvTransactionsDone;       // Transactions for data that are received from device
  bool pvThreadHandlesReadTimeout;                      // Used by waitOneTransactionDone()
  int pvMaxTransactionsPending;                         // Limit before going to busy state
  bool pvTransactionTimeoutOccured;
  QTimer *pvTransactionTimer;
  // Threads
  QList<mdtPortThread*> pvThreadsReady;                 // See onThreadReady() and onThreadFinished()
  // Instance of reader and writer thread
  mdtPortThread *pvReadThread;
  mdtPortThread *pvWriteThread;
  // State machine
  mdtPortManagerStateMachine *pvStateMachine;
  // Diseable copy
  Q_DISABLE_COPY(mdtPortManager);
};
#endif  // #ifndef MDT_PORT_MANAGER_H
