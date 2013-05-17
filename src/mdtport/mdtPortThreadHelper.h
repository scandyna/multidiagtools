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
#ifndef MDT_PORT_THREAD_HELPER_H
#define MDT_PORT_THREAD_HELPER_H

#include <QObject>
#include "mdtAbstractPort.h"

class mdtPortThread;
class mdtFrame;

/*! \brief Helper class for mdtPortThread and subclasses
 *
 * This class was introduced during TCP socket part redesing.
 *
 * Goal is to put common methods in a dedicated class,
 *  avoiding the to big complexity of mdtPortThread.
 */
class mdtPortThreadHelper : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtPortThreadHelper(QObject *parent = 0);

  /*! \brief Destructor
   *
   * Will call:
   *  - restoreCurrentReadFrameToPool()
   */
  virtual ~mdtPortThreadHelper();

  /*! \brief Set the port instance
   *
   * \pre port must be a valid pointer.
   */
  void setPort(mdtAbstractPort *port);

  /*! \brief Set the thread instance
   *
   * \pre thread must be a valid pointer.
   */
  void setThread(mdtPortThread *thread);

  /*! \brief Handle common read errors
   *
   * This is a helper method for common errors.
   *
   * If no read frame is currently set (i.e. currentReadFrame() returns 0),
   *  a UnhandledError is returned and error is logged.
   *
   * Handled errors are:
   *  - mdtAbstractPort::Disconnected: will try to reconnect
   *  - mdtAbstractPort::ReadCanceled.
   *  - mdtAbstractPort::ReadTimeout.
   *
   * For all cases, except Disconnected, the current frame is cleared and the notification is sent.
   *  For Disconnected, the frame is restored to port's readFramesPool pool.
   *
   * If this method is called by a non running thread, a warning will be logged.
   *
   * \return ErrorHandled if error could be handled or other error (most of cases a UnhandledError).
   * \pre port must be set with setPort().
   * \pre Thread must be set with setThread().
   * \post If frame is null, a UnhandledError is returned.
   */
  mdtAbstractPort::error_t handleCommonReadErrors(mdtAbstractPort::error_t portError);

  /*! \brief Handle common write errors
   *
   * This is a helper class for port common errors.
   *
   * If no write frame is currently set (i.e. currentWriteFrame() returns 0),
   *  a UnhandledError is returned and error is logged.
   *
   * Handled errors are:
   *  - mdtAbstractPort::Disconnected: will try to reconnect
   *  - mdtAbstractPort::WriteCanceled.
   *  - mdtAbstractPort::WriteTimeout.
   *
   * For all cases, the current frame is put back to write pool and the notification is sent.
   *
   * If this method is called by a non running thread, a warning will be logged.
   *
   * \return ErrorHandled if error could be handled or other error (most of cases a UnhandledError).
   * \pre port must be set with setPort().
   * \pre Thread must be set with setThread().
   */
  mdtAbstractPort::error_t handleCommonWriteErrors(mdtAbstractPort::error_t portError);

  /*! \brief Handle common read and write errors
   *
   * This is a helper class for common errors.
   *
   * If currentReadFrame is not null, handleCommonReadErrors() is called first.
   *  If no error was handled, and currentWriteFrame is not null, handleCommonWriteErrors() is called.
   *
   * \return ErrorHandled if error could be handled or other error (most of cases a UnhandledError).
   * \pre port must be set with setPort().
   * \pre Thread must be set with setThread().
   */
  mdtAbstractPort::error_t handleCommonReadWriteErrors(mdtAbstractPort::error_t portError);

  /*! \brief Get a new frame for reading data from port
   *
   * This is a helper method to get a new frame
   *  in port's read frames pool.
   *
   * It can hapen that the read frames pool is empty.
   *  In this case, a error will be generated in the mdtError
   *  log system, and notified with ReadPoolEmpty error.
   *  This method will also block ( sleep ) until a frame
   *   is available in read pool.
   *
   * \todo Currently implemented in polling, should be done with a wait condition.
   *
   * Note about port mutex handling:<br>
   *  The port mutext must be locked before calling this method.
   *  Internally, it will be unlocked during wait, and will be
   *  locked again. So, the port mutex is allways locked when this
   *  method returns.
   *
   * Note: internally, currentReadFrame is updated.
   *
   * \return A pointer to a new frame. This frame is cleared (with mdtFrame::clear() and mdtFrame::clearSub() ).
   *          If the thread's runnig flag becomes false, a Null pointer is returned, and thread
   *          should stop.
   * \return True on success, false if thread is stopping.
   * \pre Port must be set with setPort().
   * \pre Thread must be set with setThread().
   * \post If this method returns true, currentReadFrame() will return a valid pointer.
   */
  ///mdtFrame *getNewFrameRead();
  bool getNewFrameRead();

  /*! \brief Submit current read frame
   *
   * currentReadFrame will be enqueued in port's
   *  readenFrames queue.
   *
   * If currentReadFrame is null, nothing happens.
   *
   * After this call, currentReadFrame() will return a null pointer.
   *
   * \param notify If true, newFrameReaden() signal is emitted.
   * \pre Port must be set with setPort().
   * \pre Thread must be set with setThread().
   * \pre Port's readenFrames queue must not allready contain currentReadFrame.
   */
  void submitCurrentReadFrame(bool notify);

  /*! \brief Restore current read frame to pool
   *
   * currentReadFrame will be enqueued in port's
   *  readFramesPool queue.
   *
   * If currentReadFrame is null, nothing happens.
   *
   * After this call, currentReadFrame() will return a null pointer.
   *
   * \pre Port must be set with setPort().
   * \pre Thread must be set with setThread().
   * \pre Port's readFramesPool queue must not allready contain currentReadFrame.
   */
  void restoreCurrentReadFrameToPool();

  /*! \brief Store readen data into frame
   *
   * This method helps to store data that was readen.
   *  If frame becomes full, it will be enqueued
   *  in port's readen frames, an newFrameReaden()
   * signal is emitted.
   *
   * Note about port mutex handling:<br>
   *  The port mutex must be locked before calling this method.
   *  Internally, it will be unlocked during wait, and will be
   *  locked again. So, the port mutex is allways locked when this
   *  method returns.
   *
   * \param data Pointer to buffer that contains readen data.
   * \param size Size of readen data.
   * \param emitNewFrameReaden If true, newFrameReaden() will be emitted each time a complete frame was generated.
   * \return The number of frames completed during the process.
   *          This can be helpful for query/reply protocols in witch the standard
   *          reply should be one frame.
   *         On error, one of the mdtAbstractPort::error_t is returned and thread must handle error itself.
   *
   * \pre Port must be set with setPort().
   * \pre Thread must be set with setThread().
   * \pre data must be valid.
   */
  int submitReadenData(const char *data, int size, bool emitNewFrameReaden);

  /*! \brief Get a new frame for writing data to port
   *
   * This is a helper method to get a new frame
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
   *  Note: internally, currentWriteFrame is updated.
   *
   * \return A pointer to a new frame, or Null on stop request.
   *
   * \pre Port must be set with setPort().
   * \pre Thread must be set with setThread().
   * \sa mdtAbstractPort
   */
  mdtFrame *getNewFrameWrite();

  /*! \brief Set current read frame
   */
  void setCurrentReadFrame(mdtFrame *frame);

  /*! \brief Get current read frame
   *
   * Can return a null pointer.
   */
  mdtFrame *currentReadFrame();

  /*! \brief Set current write frame
   */
  void setCurrentWriteFrame(mdtFrame *frame);

  /*! \brief Get current write frame
   *
   * Can return a null pointer.
   */
  mdtFrame *currentWriteFrame();

 signals:

  /*! \brief Emitted when a read process begins
   *
   * This signal is connected to mdtPortThread::void readProcessBegin()
   *  in setThread().
   */
  void readProcessBegin();

  /*! \brief Emitted when a write process begins
   *
   * This signal is connected to mdtPortThread::void writeProcessBegin()
   *  in setThread().
   */
  void writeProcessBegin();

  /*! \brief Emited when a new frame is available
   *
   * This signal is connected to mdtPortThread::void newFrameReaden()
   *  in setThread().
   */
  void newFrameReaden();

 protected:

  /*! \brief Calls mdtPortThread::notifyError()
   *
   * \pre Thread must be set with setThread().
   */
  void notifyError(int error, bool renotifySameError = false);

  /*! \brief Call subclass reconnect() method
   *
   * This implementation does nothing
   *  and allways returns a UnhandledError.
   */
  virtual mdtAbstractPort::error_t reconnect(bool notify);

  mdtAbstractPort *pvPort;
  mdtPortThread *pvThread;

 protected slots:

  virtual void requestWrite();

 private:

  mdtFrame *pvCurrentReadFrame;
  mdtFrame *pvCurrentWriteFrame;
};
#endif

// #ifndef MDT_PORT_THREAD_HELPER_H
