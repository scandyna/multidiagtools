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
   *  \return True on sucsessfull start, or flase on start timeout
   *  \pre Serial port instance must be defined. \see setSerialPort()
   */
  bool start();

  /*! \brief Stop the running thread
   *  \pre Serial port instance must be defined. \see setSerialPort()
   */
  virtual void stop();

  /*! \brief Returns true if the thread is running
   *  This function overloads the QThread::isRunning() function.
   *  Note for subclass: when the thread is started and ready, the private member pvRunning
   *  must be set to true.
   */
  bool isRunning() const;

  /*! \brief Returns false if the thread is running
   *  \see isRunning()
   */
  bool isFinished() const;

  /*! \brief Returns true if this thread reads data and send the newFrameReaden() signal
   *
   * mdtPortManager can handle many threads. It needs to know wich one will send the
   *  newFrameReaden() signal, so it can connect it to his slot.
   *
   * This method must be implemented in subclass.
   */
  virtual bool isReader() const = 0;

 signals:

  /*! \brief Emitted when a I/O process begins
   * 
   * This can be used to display read/write state.
   * Please consider that this signal is emitted each time the I/O process begins,
   * and not when it ends.
   * This is because asynch I/O calls are fast, and nothing will be seen from user
   * if we update state before/after I/O call.
   * To handle this, use this signal as trigger, and hold the state some stime (f.ex. 100 [ms])
   */
  void ioProcessBegin();

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
   *  If the mdtAbstractPort::Disconnected error happens,
   *  the port manager stops the thread and closes the port.
   *  Optionnaly, it can try to re-open the port, and start
   *  the thread again on success.
   */
  void errorOccured(int error);

 protected:

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
  mdtFrame *getNewFrameRead();

  /*! \brief Read data from port
   *
   * This is a helper method for subclass
   *  to store chunk of data into a frame.
   *
   * Note about port mutex handling:<br>
   *  The port mutext must be locked before calling this method.
   *  Internally, it will be unlocked during wait, and will be
   *  locked again. So, the port mutex is allways locked when this
   *  method returns.
   *
   * \param frame Data readen from port will be stored in this frame.
   *
   * \return A pointer to the current frame (can be different from frame passed as argument!). It can happen that a Null pointer
   *          is returned, and this is a fatal error, and thread should be stopped.
   *
   * \pre Port must be set with setPort() before using this method.
   * \pre frame must be a valid pointer (not Null).
   */
  mdtFrame *readFromPort(mdtFrame *frame);

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
  mdtFrame *getNewFrameWrite();

  /*! \brief Write data to port
   *
   * This is a helper method for subclass
   *  to write a frame to the port.
   *  Internally, it will call mdtAbstractPort::waitEventWriteReady()
   *   before writing.
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
   *
   * \return NoError on success or a mdtAbstractPort::error_t error.
   *
   * \pre Port must be set with setPort() before using this method.
   * \pre frame must be a valid pointer (not Null).
   */
  mdtAbstractPort::error_t writeToPort(mdtFrame *frame, bool bytePerByteWrite, int interByteTime);

  /*! \brief Try to reconnect to device/peer
   *
   * This is a helper method for subclass.
   *  When a port method returns a Disconnected error,
   *  the thread can call this method, wich will try
   *  to reconnect (using mdtAbstractPort::reconnect() )
   *  until max retry was reached.
   *
   * \param timeout Timeout per try [ms]
   * \param maxTry Maximum try
   * \return NoError if reconnection worked or UnhandledError else.
   *
   * \pre Port must be set with setPort() before using this method.
   */
  mdtAbstractPort::error_t reconnect(int timeout, int maxTry);

  volatile bool pvRunning;
  mdtAbstractPort *pvPort;
#ifdef Q_OS_UNIX
  // Members needed to abort blocking
  //  functions with pthread_kill()
  static void sigactionHandle(int signum);
  pthread_t pvNativePthreadObject;
  struct sigaction pvSigaction;
#endif
  // 

 private:

  // Members used by readFromPort()
  char *pvReadBuffer;
  qint64 pvReadBufferSize;
};

#endif  // #ifndef MDT_PORT_THREAD_H
