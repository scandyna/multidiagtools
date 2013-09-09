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
   * This method will not wait until thread is running and ready.
   * Once thread is ready, ready() signal is emitted.
   *
   * \pre Port instance must be defined.
   * \pre Port must have a valid configuration.
   */
  void start();

  /*! \brief Stop the running thread
   *
   * This method will not wait until thread is stopped.
   * Once thread is stopped, finished() signal is emitted.
   *
   * \pre Port instance must be defined with setPort().
   */
  virtual void stop();

  /*! \brief Wait until thread is ready
   *
   * This is a conveniance method,
   *  mainly used by test suite.
   * Port manager, for example, use the ready signal ensteatd of this method.
   */
  void waitReady();

  /*! \brief Wait until thread is finished
   *
   * This is a conveniance method,
   *  mainly used by test suite.
   * Port manager, for example, use the finished signal ensteatd of this method.
   */
  void waitFinished();

  /*! \brief Returns true if the thread is running
   *
   *  This function overloads the QThread::isRunning() function.
   *  Note for subclass: when the thread is started and ready, the private member pvRunning
   *  must be set to true.
   */
  bool isRunning() const;

  /*! \brief Set the running flag
   *
   * This method is used by mdtPortThreadHelper
   *  and should not be used directly.
   *
   * Port's mutex is not handled in this method.
   */
  void setRunningFlag(bool running);

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

  /*! \brief Emitted when thread is started and ready
   */
  void ready(mdtPortThread *thread);

  /*! \brief Emitted when thread has finished executing
   */
  void finished(mdtPortThread *thread);

 protected:

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
  mdtAbstractPort *pvPort;

#ifdef Q_OS_UNIX
  // Members needed to abort blocking
  //  functions with pthread_kill()
  static void sigactionHandle(int signum);
  pthread_t pvNativePthreadObject;
  struct sigaction pvSigaction;
#endif

 private slots:

  /*! \brief Emit the finished(mdtPortThread*) signal with this instance
   */
  void onThreadFinished();

 private:

  mdtAbstractPort::error_t pvCurrentError;
  int pvReconnectTimeout;
  int pvReconnectMaxTry;

  Q_DISABLE_COPY(mdtPortThread);
};

#endif  // #ifndef MDT_PORT_THREAD_H
