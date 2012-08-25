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
#ifndef MDT_ABSTRACT_PORT_H
#define MDT_ABSTRACT_PORT_H

#include "mdtFrame.h"
#include "mdtFrameModbusTcp.h"
#include "mdtFrameAscii.h"
#include "mdtPortConfig.h"
#include <QObject>
#include <QString>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>

/*! \brief Base class for port I/O
 */
class mdtAbstractPort : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Error
   * 
   * \todo add UnhandledError to complete UnknownError (+ adapt in threads)
   * \todo add a DeviceDisconnected (+ adapt in threads)
   */
  enum error_t {
                NoError = 0,      /*!< No error */
                PortLocked,       /*!< Port is allready locked */
                PortNotFound,     /*!< Port was not found */
                PortAccess,       /*!< Port cannot be open with requierd access (read, write) */
                SetupError,       /*!< Setup failed on a configuration option */
                WaitingCanceled,  /*!< When a thread (mdtPortThread or subclass) is stopping, it will
                                        cancel blocking calls (like waitForReadyRead() or waitEventWriteReady() ).
                                        At this case, this error is returned, and the thread knows that it can 
                                        cleanup and end. */
                UnknownError      /*!< Unknown error is happen.
                                       Logfile could give more information, see mdtError and mdtApplication */
               };

  mdtAbstractPort(QObject *parent = 0);

  /*! \brief Destructor
   *
   * Subclass notes:<br>
   * This destructor cannot call the close() method,
   *  because this will call the pvClose() from destructed
   *  inherited object.
   * So, the subclass must call close() in its own destructor.
   */
  virtual ~mdtAbstractPort();

  /*! \brief Set the port name
   *
   * Port name can be, f.ex. /dev/ttyS0 on Linux,
   *  or COM1 on Windows.
   * This method just store given port name and does nothing else.
   */
  void setPortName(const QString &portName);

  /*! \brief Get port name
   */
  QString &portName();

  /*! \brief Open the port given by setPortName()
   *
   * If port can be open successfull, NoError code is returned.
   * If port is open, it will be closed first (by calling close()).
   *
   * The mutex is not handled by this method.
   *
   * Subclass notes:<br>
   * Internally, this method calls pvOpen(). Once done,
   *  the flags are updated.
   *
   * \sa error_t
   */
  error_t open();

  /*! \brief Get port's open state
   */
  bool isOpen() const;

  /*! \brief Close the port
   *
   * Flush and close port if it is open.
   *
   * The mutex is not handled by this method.
   *
   * Subclass notes:<br>
   * Internally, this method calls pvClose(). Once done,
   *  the flags are updated.
   * Note that this method must be called from subclass destructor.
   *  See ~mdtAbstractPort() for details.
   *
   * \todo Actuellement, les queues sont deletée ici, que faire ?
   */
  virtual void close();

  /*! \brief Set configuration
   */
  void setConfig(mdtPortConfig *cfg);

  /*! \brief Get the stored configuration
   */
  mdtPortConfig &config();

  /*! \brief Setup port with given configurations.
   *
   * If port is not open, open() will be called first.
   * Then, pvSetup() is called, and finally queues initialized with initQueues().
   * If somethig fails, the port is closed again, and error code returned.
   *
   * The mutex is not handled by this method.
   *
   * \pre A valid configuration must be set before calling this method.
   *
   * \sa error_t
   */
  error_t setup();

  /*! \brief Set the read data timeout
   *
   * The mutex is not handled by this method.
   *
   * Subclass notes:<br>
   * This method must be re-implemented in subclass.
   * The subclass can convert and store the value in system specific type
   * (f.ex: timeval struct on Posix)
   *
   * \param timeout Timeout [ms]. A value of -1 means a infinite timeout.
   */
  virtual void setReadTimeout(int timeout) = 0;

  /*! \brief Set the write data timeout
   *
   * The mutex is not handled by this method.
   *
   * Subclass notes:<br>
   * This method must be re-implemented in subclass.
   * The subclass can convert and store the value in system specific type
   * (f.ex: timeval struct on Posix)
   *
   * \param timeout Timeout [ms]. A value of -1 means a infinite timeout.
   */
  virtual void setWriteTimeout(int timeout) = 0;

  /*! \brief Wait until data is available on port.
   *
   * This method is called from mdtPortReadThread , and should not be used directly.<br>
   * Mutex must be locked before calling this method with lockMutex(). The mutex is locked when method returns.
   *
   * Subclass notes:<br>
   * This method must be re-implemented in subclass.
   * The read timeout state must be updated with updateReadTimeoutState()<br>
   * Notes about mutex handling:
   *  - Mutex must be released during wait, and relocked befor return.
   *
   * \return On success, NoError is returned. If waiting is canceled, WaitingCanceled is returned
   *          and the thread knows that it must end (case of stopping thread).
   *          On unhandled error, UnknownError is returned. The thread also stop working, and emit
   *          a error signal (that can be handled in mdtPortManager, or in another place in application).
   *
   * \sa mdtPortThread
   * \sa mdtPortConfig
   * \sa mdtTcpSocketThread
   */
  virtual error_t waitForReadyRead() = 0;

  /*! \brief Wait until data is available on port.
   *
   * This method calls setReadTimeout() and waitForReadyRead()
   * (it is a little bit slower than setting timeout one time, and call waitForReadyRead() ).<br>
   * Note that the reader thread will call waitForReadyRead() without argument.<br>
   * Note: this method is called from thread , and should not be used directly<br>
   * Mutex: see waitForReadyRead()
   *
   * \param timeout Timeout [ms]. A value of -1 means a infinite timeout.
   * \sa waitForReadyRead()
   */
  error_t waitForReadyRead(int msecs);

  /*! \brief Read data from port
   *
   * This method is called from mdtPortReadThread , and should not be used directly.
   *
   * Mutex is not handled by this method.
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass.<br>
   *
   * \return Number of bytes readen, or a error < 0
   */
  virtual qint64 read(char *data, qint64 maxSize) = 0;

  /*! \brief Request to suspend transmission
   *
   * This method is called from mdtPortReadThread , and should not be used directly.<br>
   * Mutex is not handled by this method.
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass if requierd.<br>
   * Note about serial port subclass:<br>
   *  the right flow control must be used regarding enabled flow control.<br>
   * Default implementation does nothing.<br>
   *
   * \return False on error, in this case, the reader thread will be stopped.
   */
  virtual bool suspendTransmission();

  /*! \brief Request to resume transmission
   *
   * This method is called from mdtPortReadThread , and should not be used directly.<br>
   * Mutex is not handled by this method.
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass if requierd.<br>
   * Note about serial port subclass:<br>
   *  the right flow control must be used regarding enabled flow control.<br>
   * Default implementation does nothing.<br>
   *
   * \return False on error, in this case, the reader thread will be stopped.
   */
  virtual bool resumeTransmission();

  /*! \brief Flush read buffers
   *
   * Will move all readen frames to pool,
   *  and call pvFlushIn() to flush system's buffers.
   *
   * Mutex is locked in this method.
   */
  void flushIn();

  /*! \brief Just for special cases
   * 
   * In some case, this method is implemented in subclass.
   * Default implementation does nothing.
   * \sa mdtUsbtmcPort
   */
  virtual void readOneFrame();

  /*! \brief Just for special cases
   * 
   * In some case, this method is implemented in subclass.
   * Default implementation does nothing.
   * \sa mdtUsbtmcPort
   */
  virtual void writeOneFrame();

  /*! \brief Wait until data can be written to port.
   *
   * This method is called from mdtPortWriteThread , and should not be used directly.<br>
   * Mutex must be locked before calling this method with lockMutex(). The mutex is locked when method returns.
   *
   * Subclass notes:<br>
   * This method must be re-implemented in subclass. The write timeout state must be updated with updateWriteTimeoutState()
   * Notes about mutex handling:
   *  - Mutex must be released during wait, and relocked befor return.
   *
   * \return On success, NoError is returned. If waiting is canceled, WaitingCanceled is returned
   *          and the thread knows that it must end (case of stopping thread).
   *          On unhandled error, UnknownError is returned. The thread also stop working, and emit
   *          a error signal (that can be handled in mdtPortManager, or in another place in application).
   */
  virtual error_t waitEventWriteReady() = 0;

  /*! \brief Write data to port
   *
   * This method is called from mdtPortWriteThread , and should not be used directly.
   *
   * Mutex is not handled by this method.
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass.<br>
   *
   * \return Number of bytes written, or <0 on error
   */
  virtual qint64 write(const char *data, qint64 maxSize) = 0;

  /*! \brief Flush write buffers
   *
   * Will move all write frames to pool,
   *  and call pvFlushOut() to flush system's buffers.
   *
   * Mutex is locked in this method.
   */
  void flushOut();

  /*! \brief Update the read timeout state
   *
   * This method must be called by system dependant waitEventRead() method.
   * When the read timeout state chages, the signal readTimeoutStateChanged() is emited.<br>
   * Note: this method is called from mdtPortReadThread , and should not be used directly<br>
   * Mutex is not handled by this method.
   */
  void updateReadTimeoutState(bool state);

  /*! \brief Update the write timeout state
   *
   * This method must be called by system dependant waitEventWriteReady() method
   * When the write timeout state chages, the signal writeTimeoutStateChanged() is emited.<br>
   * Note: this method is called from mdtPortWriteThread , and should not be used directly<br>
   * Mutex is not handled by this method.
   */
  void updateWriteTimeoutState(bool state);

  /*! \brief Returns read timeout state
   * 
   * Mutex is not handled by this method.
   */
  bool readTimeoutOccured();

  /*! \brief Returns write timeout state
   * 
   * Mutex is not handled by this method.
   */
  bool writeTimeoutOccured();

  /*! \brief Flush read/write buffers
   *
   * Will move all read and write frames to their pool,
   *  and call pvFlushIn() and pvFlushOut() to flush system's buffers.
   *
   * Mutex is locked in this method.
   */
  void flush();

  /*! \brief Init the read ans write queues
   *
   * \pre A valid configuration must be set before using this method.
   */
  void initQueues();

  /*! \brief Get the readen frames Queue
   *
   * Readen frames queue contains frames that where received from device
   *
   * Mutex is not handled by this method.
   */
  QQueue<mdtFrame*> &readenFrames();

  /*! \brief Get the read frames Queue pool
   *
   * Read frames queue pool contains frames that are ready to use for reception
   *
   * Mutex is not handled by this method.
   */
  QQueue<mdtFrame*> &readFramesPool();

  /*! \brief Add a frame to write
   *
   * Once the frame is added to the write queue,
   *  waiting thread will be woken up (if waiting)
   *  and will send the frame.
   *
   * The mutex is locked internally, and should not be locked
   *  before calling this method.
   * The mutex is unlocked before returning.
   *
   * \pre frame must be a vail pointer.
   */
  void addFrameToWrite(mdtFrame *frame);

  /*! \brief Wait until a frame to write is available and get it
   *
   * If a frame is available in write frames queue, it will be returned.
   *  Else, thit method will block calling thread until a frame is available in
   *  write frames queue, and return one.
   *
   * This method is called from mdtPortThread
   *  and should not be used directly.
   *
   * The mutex must be locked before calling this method.
   *  Internally, it will be unlocked during wait, and locked
   *  again before returning.
   *
   * \return A pointer to a frame, or Null (Null happens after stop request)
   */
  mdtFrame *getFrameToWrite();

  /*! \brief Abort the frame tp write waiting
   *
   * Will wake thread(s) waiting on a frame to write
   *  with getFrameToWrite() .
   * This method is called from mdtPortThread::stop() ,
   *  and should not be called directly.
   *
   * Mutex is not handled by this method.
   */
  void abortFrameToWriteWait();

  /*! \brief Get the write frames Queue
   *
   * Write frames queue contains frames that must be sent
   *
   * Mutex is not handled by this method.
   */
  QQueue<mdtFrame*> &writeFrames();

  /*! \brief Get the write frames Queue pool
   *
   * Write frames queue pool contains frames that are ready to use for transmission
   *
   * Mutex is not handled by this method.
   */
  QQueue<mdtFrame*> &writeFramesPool();

  /*! \brief Lock the mutex
   */
  void lockMutex();

  /*! \brief Unlock the mutex
   */
  void unlockMutex();

 signals:

  /*!  \brief Emited when read timeout state changed
   */
  void readTimeoutStateChanged(bool state);

  /*!  \brief Emited when write timeout state changed
   */
  void writeTimeoutStateChanged(bool state);

 protected:

  /*! \brief Open the port given by setPortName()
   *
   * If port can be open successfull, NoError code is returned.
   *
   * The mutex is not handled by this method.
   *
   * \pre The port must not be open whenn calling this method.
   *
   * Subclass notes:<br>
   * This method must be re-implemented in subclass.<br>
   * To handle the port correctly, the subclass method must:
   *  - Do the specific work. Note that port must be open in exclusive mode. On Linux, the mdtPortLock should be used for this.
   *  - The mdtError system should be used (on error) to keep trace in logfile.
   *  - If port must be closed (f.ex. on error), use the close() method to keep flags coherent.
   *  - Return the correct error code on failure (see the error_t enum)
   *
   * \sa close()
   * \sa error_t
   */
  virtual error_t pvOpen() = 0;

  /*! \brief Close port
   *
   * The mutex is not handled by this method.
   *
   * \pre The port must be open whenn calling this method.
   *
   * Subclass notes:<br>
   * This method must be re-implemented in subclass.<br>
   * To handle the port correctly, the subclass method must:
   *  - Do the specific work. Note that port was open in exclusive mode. On Linux, don't forget to unlock the port with mdtPortLock.
   *  - The mdtError system should be used (on error) to keep trace in logfile.
   */
  virtual void pvClose() = 0;

  /*! \brief Setup port with given configurations.
   *
   * The mutex is not handled by this method.
   *
   * \pre The port must be open whenn calling this method.
   * \pre A valid configuration must be set before calling this method.
   *
   * Subclass notes:<br>
   * This method must be re-implemented in subclass.<br>
   * To handle the port correctly, the subclass method must:
   *  - Do the specific work
   *  - Set read/write timeouts using setReadTimeout() and setWriteTimeout().
   *     See mdtPortConfig to know how to get these timeouts.
   *  - Return the correct error code on failure (see the error_t enum)
   *
   * Configuration is available using config().
   *
   * \sa error_t
   */
  virtual error_t pvSetup() = 0;

  /*! \brief Flush read port
   *
   * This method is called from flushIn(),
   *  and is usefull if subsystem needs to be flushed.
   *  (For ex. serial port).
   *
   * This method must be implemented in subclass.
   *
   * The mutex is handled by flushIn() and should not
   *  be handled here.
   */
  virtual void pvFlushIn() = 0;

  /*! \brief Flush write port
   *
   * This method is called from flushout(),
   *  and is usefull if subsystem needs to be flushed.
   *  (For ex. serial port).
   *
   * This method must be implemented in subclass.
   *
   * The mutex is handled by flushOut() and should not
   *  be handled here.
   */
  virtual void pvFlushOut() = 0;

  bool pvReadTimeoutOccured;
  bool pvReadTimeoutOccuredPrevious;
  bool pvWriteTimeoutOccured;
  bool pvWriteTimeoutOccuredPrevious;
  // Frames queues
  QQueue<mdtFrame*> pvReadenFrames;
  QQueue<mdtFrame*> pvReadFramesPool;
  QQueue<mdtFrame*> pvWriteFrames;
  QQueue<mdtFrame*> pvWriteFramesPool;
  // Configuration
  mdtPortConfig *pvConfig;
  // Attributes
  QString pvPortName;     // Port name, like /dev/ttyS0 , COM1, ...
  // mutex
  QMutex pvMutex;

 private:

  // Diseable copy
  Q_DISABLE_COPY(mdtAbstractPort);

  // Some flags
  bool pvIsOpen;
  // Wait conditions
  QWaitCondition pvWriteFrameAvailable;
};

#endif  // #ifndef MDT_ABSTRACT_PORT_H
