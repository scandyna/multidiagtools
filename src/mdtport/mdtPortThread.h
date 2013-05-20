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
#ifndef MDT_PORT_THREAD_H
#define MDT_PORT_THREAD_H

#include "mdtError.h"
#include "mdtAbstractPort.h"
#include <QThread>
#include <QObject>

#ifdef Q_OS_UNIX
 #include <pthread.h>
 #include <signal.h>
#endif

class mdtFrame;

class mdtPortThread : public QThread
{
 Q_OBJECT

 public:

  mdtPortThread(QObject *parent = 0);
  virtual ~mdtPortThread();

  /*! \brief Set the port instance
   *
   * \pre port must be a valid pointer
   * \pre The thread must not running
   */
  void setPort(mdtAbstractPort *port);

  /*! \brief Detach the port from thread
   *
   * \param releaseMemory If true, the port object that was set with setPort() will be deleted.
   * \pre The thread must not running
   */
  void detachPort(bool releaseMemory);

  /*! \brief Start the thread
   *
   * \return True on sucsessfull start, or flase on start timeout
   *
   * \pre Port instance must be defined.
   * \pre Port must have a valid configuration.
   */
  bool start();

  /*! \brief Stop the running thread
   *
   * \pre Port instance must be defined with setPort().
   */
  virtual void stop();

  /*! \brief Returns true if the thread is running
   *
   *  This function overloads the QThread::isRunning() function.
   *  Note for subclass: when the thread is started and ready, the private member pvRunning
   *  must be set to true.
   */
  bool isRunning() const;

  /*! \brief Get the running flag
   *
   * This method is used by mdtPortThreadHelper
   *  and should not be used directly.
   *
   * Port's mutex is not handled in this method.
   */
  bool runningFlagSet() const;

  /*! \brief Returns false if the thread is running
   *  \see isRunning()
   */
  bool isFinished() const;

  /*! \brief Returns true if this thread reads data and send the newFrameReaden() signal
   *
   * mdtPortManager can handle many threads. It needs to know wich one will send the
   *  newFrameReaden() signal, so it can connect it to his slot.
   *
   * Default implementation returns allways false.
   *
   * This method should be implemented in reader subclass.
   */
  virtual bool isReader() const;

  /*! \brief Returns true if this thread writes date
   *
   * Is used by mdtPortThread::stop() 
   *  to know if mdtAbstractPort::abortFrameToWriteWait() must be called.
   *
   * Default implementation returns false
   */
  virtual bool isWriter() const;

  /*! \brief Returns true is thread handles timeout(s) itself.
   *
   * For example, mdtUsbPortThread and subclasses handles timeout
   *  itself. mdtPortReadThread does not.
   *
   * mdtPortManager uses this flag to know how to deal with timeouts.
   *
   * This implementation returns false.
   */
  virtual bool handlesTimeout() const;

  /*! \brief Emit the errorOccured() signal if new error is different from current
   *
   * \param renotifySameError For some cases, the same error must be notified each time it happens.
   *                           If this flag is true, signal is emitted without checking previous error.
   */
  void notifyError(mdtAbstractPort::error_t error, bool renotifySameError = false);

  /*! \brief Get current error
   *
   * This is used by mdtPortManager.
   *
   * Port's mutex is not handled by this method.
   */
  mdtAbstractPort::error_t currentError() const;

 signals:

  /*! \brief Emitted when a read process begins
   * 
   * This can be used to display read state.
   * Please consider that this signal is emitted each time the read process begins,
   * and not when it ends.
   * This is because asynch I/O calls are fast, and nothing will be seen from user
   * if we update state before/after I/O call.
   * To handle this, use this signal as trigger, and hold the state some stime (f.ex. 100 [ms])
   */
  void readProcessBegin();

  /*! \brief Emitted when a write process begins
   * 
   * This can be used to display write state.
   * Please consider that this signal is emitted each time the write process begins,
   * and not when it ends.
   * This is because asynch I/O calls are fast, and nothing will be seen from user
   * if we update state before/after I/O call.
   * To handle this, use this signal as trigger, and hold the state some stime (f.ex. 100 [ms])
   */
  void writeProcessBegin();

  /*! \brief Emited when a new frame is available
   *
   * This signal is emited when a new frame is available.
   *
   * To get the frame, the simplest way is to use a mdtPortManager.
   *
   * It's also possible to use mdtPort, but this solution needs to handle
   * the mutex, verify the readen queue state, ...
   *
   * \sa mdtPortManager
   * \sa mdtPort
   */
  void newFrameReaden();

  /*! \brief Emitted on error
   *
   * \todo Adapt threads + port managers
   *
   * When a error occurs, this signal is emited.
   *
   * The error is one of the mdtAbstractPort::error_t.
   *
   * mdtPortManager uses this signal.
   */
  void errorOccured(int error);

 protected:

  /*! \brief Handle common read errors
   *
   * This is a helper method for port specific subclass.
   *
   * Handled errors are:
   *  - mdtAbstractPort::Disconnected: will try to reconnect
   *  - mdtAbstractPort::ReadCanceled.
   *  - mdtAbstractPort::ReadTimeout.
   *
   * The current frame is cleared and the notification is sent.
   *
   * If this method is called by a non running thread, a warning will be logged.
   *
   * \param frame Current read frame. Note that another frame can be pointed after a call ofthis method (f.ex. after disconnection)
   * \return ErrorHandled if error could be handled or other error (most of cases a UnhandledError).
   * \pre frame must be a valid pointer.
   * \pre port must be set with setPort().
   * \post If frame is null, a UnhandledError is returned.
   */
  ///mdtAbstractPort::error_t handleCommonReadErrors(mdtAbstractPort::error_t portError, mdtFrame **frame);

  /*! \brief Handle common write errors
   *
   * This is a helper class for port specific subclass.
   *
   * Handled errors are:
   *  - mdtAbstractPort::Disconnected: will try to reconnect
   *  - mdtAbstractPort::WriteCanceled.
   *  - mdtAbstractPort::WriteTimeout.
   *
   * The current frame is put back to write pool and the notification is sent.
   *
   * If this method is called by a non running thread, a warning will be logged.
   *
   * \param frame Current write frame. Note that frame will be null after a call of this method.
   * \return ErrorHandled if error could be handled or other error (most of cases a UnhandledError).
   * \pre frame must be a valid pointer.
   * \pre port must be set with setPort().
   */
  ///mdtAbstractPort::error_t handleCommonWriteErrors(mdtAbstractPort::error_t portError, mdtFrame **frame);

  /*! \brief Handle common read and write errors
   *
   * This is a helper class for port specific subclass.
   *
   * If readFrame is not null, handleCommonReadErrors() is called first.
   *  If no error was handled, and writeFrame is not null, handleCommonWriteErrors() is called.
   *
   * \param readframe Current read frame. Note that another frame can be pointed after a call ofthis method (f.ex. after disconnection).
   * \param writeFrame Current write frame. Note that frame can be null after a call of this method (if a write error occured).
   * \return ErrorHandled if error could be handled or other error (most of cases a UnhandledError).
   */
  ///mdtAbstractPort::error_t handleCommonReadWriteErrors(mdtAbstractPort::error_t portError, mdtFrame **readFrame, mdtFrame **writeFrame);

  /*! \brief Get a new frame for reading data from port
   *
   * This is a helper method for subclass to get a new frame
   *  in port's read frames pool.
   *
   * It can hapen that the read frames pool is empty.
   *  In this case, a error will be generated in the mdtError
   *  log system, the signal errorOccured() will be emited
   *  with MDT_PORT_QUEUE_EMPTY_ERROR value, and method
   *  will go sleep until a new frame is available in pool.
   *
   * \todo Currently implemented in polling, should be done with a wait condition.
   *
   * Note about port mutex handling:<br>
   *  The port mutext must be locked before calling this method.
   *  Internally, it will be unlocked during wait, and will be
   *  locked again. So, the port mutex is allways locked when this
   *  method returns.
   *
   * \return A pointer to a new frame. This frame is cleared (with mdtFrame::clear() ).
   *          If the runnig flag becomes false, a Null pointer is returned, and thread
   *          should stop.
   *
   * \pre Port must be set with setPort() before using this method.
   */
  ///mdtFrame *getNewFrameRead();

  /*! \brief Read data from port
   *
   * This is a helper method for subclass
   *  to store chunk of data into a frame.
   *
   * Note about port mutex handling:<br>
   *  The port mutex must be locked before calling this method.
   *  Internally, it will be unlocked during wait, and will be
   *  locked again. So, the port mutex is allways locked when this
   *  method returns.
   *
   * \param frame Data readen from port will be stored in this frame.
   *               Note that pointer is updated when a frame was completed
   *               and that it can be Null (in this case the mdtAbstractPort::UnhandledError is returned)
   * \param emitNewFrameReaden If true, newFrameReaden() will be emitted each time a complete frame was generated.
   *                            If false, complete frames are enqueued (as ever), but signal is not emitted.
   *                            (This flag is used by mdtUsbtmcPortThread).
   *
   * \return The number of frames completed during the process.
   *          This can be helpful for query/reply protocols in witch the standard
   *          reply should be one frame.
   *         On error, one of the mdtAbstractPort::error_t is returned and thread must handle error itself.
   *
   * \pre Port must be set with setPort() before using this method.
   * \pre frame must be a valid pointer (not Null).
   */
  ///int readFromPort(mdtFrame **frame, bool emitNewFrameReaden = true);

  /*! \brief Get a new frame for writing data to port
   *
   * This is a helper method for subclass to get a new frame
   *  in port's write frames.
   *
   * If no frame is available for write, this method
   *  will block the caller thread until one is avaliable.
   *
   * Note about port mutex handling:<br>
   *  The port mutext must be locked before calling this method.
   *  Internally, it will be unlocked during wait, and will be
   *  locked again. So, the port mutex is allways locked when this
   *  method returns.
   *
   * \return A pointer to a new frame, or Null on stop request.
   *
   * \pre Port must be set with setPort() before using this method.
   * \sa mdtAbstractPort
   */
  ///mdtFrame *getNewFrameWrite();

  /*! \brief Write data to port
   *
   *
   * This is a helper method for subclass
   *  to write some data from a frame to the port.
   *
   * Signal ioProcessBegin() is emitted.
   *
   * \param frame Data stored in this frame will be written to port.
   * \param maxSize If >= 0, maxSize will be written, else the frame size will be considered.
   * \return Amount of bytes written or a mdtAbstractPort::error_t error.
   *
   * \pre Port must be set with setPort() before using this method.
   * \pre frame must be a valid pointer (not Null).
   */
  ///qint64 writeDataToPort(mdtFrame *frame, int maxSize = -1);

  /*! \brief Write a complete frame to port
   *
   * This is a helper method for subclass
   *  to write a frame to the port.
   *  Internally, it will call mdtAbstractPort::waitEventWriteReady()
   *   before writing.
   *
   * Note that frame will be put back to write pool after complete write, cancel, flush or error.
   *  That says that frame will not be valid after call of this method.
   *
   * Note about port mutex handling:<br>
   *  The port mutex must be locked before calling this method.
   *  Internally, it will be unlocked during wait, and will be
   *  locked again. So, the port mutex is allways locked when this
   *  method returns.
   *
   * \param frame Data stored in this frame will be written to port.
   * \param bytePerByteWrite If true, one byte will be written once.
   * \param interByteTime Time between each byte write [ms] (has only effect if bytePerByteWrite is true)
   * \param maxWriteTry Some port can return 0 byte or a timeout error if busy. If this happens,
   *                     this method will sleep some time an try a write call again until
   *                     write call works successfull or maxWriteTry is reached.
   *
   * \return NoError on success or a mdtAbstractPort::error_t error (See also handleCommonWriteErrors() ).
   *
   * \pre Port must be set with setPort() before using this method.
   * \pre frame must be a valid pointer (not Null).
   */
  ///mdtAbstractPort::error_t writeToPort(mdtFrame *frame, bool bytePerByteWrite, int interByteTime, int maxWriteTry = 10);

  /*! \brief Try to reconnect to device/peer
   *
   * This is a helper method for subclass.
   *  When a port method returns a Disconnected error,
   *  the thread can call this method, wich will try
   *  to reconnect (using mdtAbstractPort::reconnect() )
   *  until max retry was reached.
   *
   * Reconnect timeout and max try are readen from port config, in start() method.
   *
   * \param notify If true, error will be notified (with errorOccured() ),
   *                and failure after maxTry will be reported with mdtError.
   * \return NoError if reconnection worked or UnhandledError else.
   *
   * \pre Port must be set with setPort() before using this method.
   */
  mdtAbstractPort::error_t reconnect(bool notify = true);

  volatile bool pvRunning;
  ///volatile bool pvHandlesTimeout;   // Set true if timeout is/are handled here (example: USBTMC thread handles timeouts iteself)
  mdtAbstractPort *pvPort;
#ifdef Q_OS_UNIX
  // Members needed to abort blocking
  //  functions with pthread_kill()
  static void sigactionHandle(int signum);
  pthread_t pvNativePthreadObject;
  struct sigaction pvSigaction;
#endif

 private:

  // Members used by readFromPort()
  char *pvReadBuffer;
  qint64 pvReadBufferSize;
  ///int pvCurrentError;
  mdtAbstractPort::error_t pvCurrentError;
  int pvReconnectTimeout;
  int pvReconnectMaxTry;
};

#endif  // #ifndef MDT_PORT_THREAD_H
