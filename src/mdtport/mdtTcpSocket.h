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
#ifndef MDT_TCP_SOCKET_H
#define MDT_TCP_SOCKET_H

#include "mdtAbstractPort.h"
#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QTcpSocket>
#include <QQueue>

class mdtTcpSocketThread;

class mdtTcpSocket : public mdtAbstractPort
{
 Q_OBJECT

 public:

  mdtTcpSocket(QObject *parent = 0);
  ~mdtTcpSocket();

  /*! \brief Reconnect to peer
   *
   * If one of the method returns a Disconnected error,
   *  the thread will call this method to try to reconnect.
   *
   * \param timeout Timeout [ms]
   * \return NoError if connection could be done.
   *          Disconnected if connection could not be done,
   *          in wich case the thread will retry (until max retry).
   *          A UnhandledError can be returned.
   */
  error_t reconnect(int timeout);

  /*! \brief Get peer host or IP
   *
   * Returns the host part set by setPortName()
   *
   * Mutex is not handled by this method,
   *  it should be locked before calling this method.
   */
  QString peerName() const;

  /*! \brief Get peer port number
   *
   * Returns the port part set by setPortName()
   *
   * Mutex is not handled by this method,
   *  it should be locked before calling this method.
   */
  quint16 peerPort() const;

  /*! \brief Set the socket
   *
   * QTcpSocket must be created from thread on witch it will be used.
   * In this case, the mdtTcpSocketThread create the instance.
   *
   * This method will be called to store this instance and an instance of the thread itself.
   *
   * Note: should not be used directly.
   *
   * \pre socket must be a valid pointer
   * \pre thread must be a valid pointer
   * \sa mdtTcpSocketThread
   */
  void setThreadObjects(QTcpSocket *socket, mdtTcpSocketThread *thread);

  /*! \brief Set the read data timeout
   *
   * The mutex is not handled by this method.
   *
   * \param timeout Timeout [ms]. A value of -1 means a infinite timeout.
   */
  void setReadTimeout(int timeout);

  /*! \brief Set the write data timeout
   *
   * The mutex is not handled by this method.
   *
   * \param timeout Timeout [ms]. A value of -1 means a infinite timeout.
   */
  void setWriteTimeout(int timeout);

  /*! \brief Set unknown read size flag
   *
   * The write/read processes are done in one thread.
   *  First, the thread waits a write event from application,
   *  then data is sent. After this, thread still blocked
   *  until data are available or timeout (set with setReadTimeout() ).
   *  After this, data are readen until one frame is completely received, then thread goes back idle.
   *
   * The problem is that more data can be sent from peer, and these data are lost, or stored in
   *  frames after next transaction (write/read process), and application will probably becomes uncoherent.
   *
   * The solution for this was implemented so:
   *  - Request is sent to host
   *  - Data comes back, and is store into current frame until it is complete (or timeout).
   *  - If the unknown read size flag is set, a loop begins with a couple of wait/read with short
   *     timeout begins (50ms) until no more data comes in.
   *
   * This flag should be set as follow:
   *  - For RAW frames: true
   *  - For ASCII frames: true
   *  - For MODBUS/TCP: false
   *
   * Default is true.
   */
  void setUnknownReadSize(bool unknown);
 
  /*! \brief Get the unknown frame size flag
   *
   * For details, see setUnknownReadSize()
   */
  bool unknownReadSize() const;

  /*! \brief Wait until data is available on port.
   *
   * This method is called from mdtPortReadThread , and should not be used directly.
   *
   * Mutex must be locked before calling this method with lockMutex(). The mutex is locked when method returns.
   */
  error_t waitForReadyRead();

  /*! \brief Read data from port
   *
   * This method is called from mdtPortReadThread , and should not be used directly.
   *
   * Mutex is not handled by this method.
   *
   * \return Number of bytes readen, or a error < 0 (one of the mdtAbstractPort::error_t)
   */
  qint64 read(char *data, qint64 maxSize);

  /*! \brief Wait until data can be written to port.
   *
   * This method is called from mdtPortWriteThread , and should not be used directly.<br>
   * Mutex must be locked before calling this method with lockMutex(). The mutex is locked when method returns.
   */
  error_t waitEventWriteReady();

  /*! \brief Write data to port
   *
   * This method is called from mdtPortWriteThread , and should not be used directly.
   *
   * Mutex is not handled by this method.
   *
   * \return Number of bytes written, or a error < 0 (one of the mdtAbstractPort::error_t)
   */
  qint64 write(const char *data, qint64 maxSize);

  /*! \brief Add a frame to write
   *
   * Once the frame is added to the write queue,
   *  newFrameToWrite() is emitted.
   *
   * The mutex must be locked before calling this method,
   *  and still locked inside.
   *
   * \pre frame must be a valid pointer.
   */
  void addFrameToWrite(mdtFrame *frame);

 signals:

  /*! \brief Emitted when a new frame is available to write
   *
   * See addFrameToWrite().
   */
  void newFrameToWrite();

 private:

  /*! \brief Open the port given by setPortName()
   *
   * If port can be open successfull, NoError code is returned.
   *
   * The mutex is not handled by this method.
   *
   * \pre The port must not be open whenn calling this method.
   *
   * \sa close()
   * \sa error_t
   */
  error_t pvOpen();

  /*! \brief Close port
   *
   * The mutex is not handled by this method.
   *
   * \pre The port must be open whenn calling this method.
   */
  void pvClose();

  /*! \brief Setup port with given configurations.
   *
   * The mutex is not handled by this method.
   *
   * \pre The port must be open whenn calling this method.
   *
   * \sa error_t
   */
  error_t pvSetup();

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
  void pvFlushIn();

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
  void pvFlushOut();

  /*! \brief Map error returned by QTcpSocket to mdtAbstractPort error.
   *
   * Unhandled error is reported with mdtError and UnhandledError is returned.
   *
   * \param error Error returned by QTcpSocket
   * \param byRead Error occured during waitForReadyRead or read, else during waitEventWriteReady or write
   */
  error_t mapSocketError(QAbstractSocket::SocketError error, bool byRead);

  int pvReadTimeout;
  int pvWriteTimeout;
  QTcpSocket *pvSocket;             // QTcpSocket object passed from thread
  mdtTcpSocketThread *pvThread;
  QString pvPeerName;               // Host name or IP
  quint16 pvPeerPort;               // Host port
  bool pvUnknownReadSize;           // If false, read will end after one frame is complete (see mdtTcpSocketThread::run() ).
};

#endif  // #ifndef MDT_TCP_SOCKET_H
