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
#ifndef MDT_PORT_THREAD_HELPER_SOCKET_H
#define MDT_PORT_THREAD_HELPER_SOCKET_H

#include "mdtPortThreadHelper.h"
#include <QTcpSocket>
#include <QObject>

class QTimer;

/*! \brief Help the usage of QTcpSocket's asynchrone API
 */
class mdtPortThreadHelperSocket : public mdtPortThreadHelper
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtPortThreadHelperSocket(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtPortThreadHelperSocket();

  /*! \brief Set socket
   *
   * Take some parameters in port and port's config
   *  and make all needed signal/slot connections.
   *
   * Parameters that are used by port's config are:
   *  - connectTimeout
   *  - connectMaxTry
   *  - readTimeout
   *  - writeTimeout
   *
   * See mdtPortConfig for details.
   *
   * Port's mutex is not handled by this method,
   *  it should be locked before call.
   *
   * \pre socket must be a valid pointer.
   * \pre port must be set with mdtPortThreadHelper::setPort().
   */
  void setSocket(QTcpSocket *socket);

  /*! \brief Connect to host set during setSocket()
   *
   * \pre port must be set with mdtPortThreadHelper::setPort()
   * \pre socket must be set with setSocket()
   */
  void connectToHost();

 private slots:

  /*! \brief Notify the connected state
   *
   * Uses mdtPortThreadHelper::notifyError() with mdtAbstractPort::NoError.
   */
  void onSocketConnected();

  /*! \brief Notify the disconnected state
   *
   * Uses mdtPortThreadHelper::notifyError() with mdtAbstractPort::Disconnected.
   */
  void onSocketDisconnected();

  /*! \brief Restore current frames to pools
   */
  void onSocketClosing();

  /*! \brief Handle and/or notify errors reported by QTcpSocket
   *
   * \pre Port must be set with mdtPortThreadHelper::setPort().
   * \pre Thread must be set with mdtPortThreadHelper::setThread().
   * \pre socket must be set with setSocket().
   */
  void onSocketError(QAbstractSocket::SocketError socketError);

  /*! \brief Some states are mapped to mdtAbstractPort error and notified
   *
   * We try to not interfer with onSocketError(), onSocketConnected(), ... , 
   *  so only a few QAbstractSocket states are notified.
   */
  void onSocketStateChanged(QAbstractSocket::SocketState socketState);

  /*! \brief Read from socket and store into frame (using readFromSocket() ).
   */
  void onSocketReadyRead();

  /*! \brief Write current write frame to socket.
   */
  void onSocketBytesWritten(qint64 bytes);

  /*! \brief Handle timeout reported by QTcpSocket
   */
  void onConnectionTimeout();

  /*! \brief Handle read timeout (Uses internal timer)
   */
  void onReadTimeout();

  /*! \brief Handle write timeout (Uses internal timer)
   */
  void onWriteTimeout();

  /*! \brief Called by mdtTcpSocket when a new frame is to write
   *
   * \pre Port must be set with mdtPortThreadHelper::setPort().
   * \pre Thread must be set with mdtPortThreadHelper::setThread().
   * \pre socket must be set with setSocket().
   */
  void requestWrite();

 private:

  /*! \brief Read data from port
   *
   * This is a helper method to read data from socket and
   *  to store chunk of data into a frame.
   *
   * Note about port mutex handling:<br>
   *  The port mutex must be locked before calling this method.
   *
   * newFrameReaden() will be emitted each time a complete frame was generated.
   *
   * \return NoError on success or a mdtAbstractPort::error_t error.
   *
   * \pre Port must be set with mdtPortThreadHelper::setPort().
   * \pre Thread must be set with mdtPortThreadHelper::setThread().
   * \pre socket must be set with setSocket().
   */
  mdtAbstractPort::error_t readFromSocket();

  /*! \brief Write a complete frame to socket
   *
   * This is a helper method
   *  to write a frame to the socket.
   *
   * Note that currentWriteFrame will be put back to write pool after complete write, cancel, flush or error.
   *  That says that frame will not be valid after call of this method.
   *
   * Note about port mutex handling:<br>
   *  The port mutex must be locked before calling this method.
   *
   * \return NoError on success or a mdtAbstractPort::error_t error (See also mdtPortThreadHelper::handleCommonWriteErrors() ).
   *
   * \pre Port must be set with mdtPortThreadHelper::setPort().
   * \pre Thread must be set with mdtPortThreadHelper::setThread().
   * \pre socket must be set with setSocket().
   */
  mdtAbstractPort::error_t writeToSocket();

  /*! \brief Map error returned by QTcpSocket to mdtAbstractPort error.
   *
   * Unhandled error is reported with mdtError and UnhandledError is returned.
   *
   * \param error Error returned by QTcpSocket
   * \param byRead Error occured during waitForReadyRead or read, else during waitEventWriteReady or write
   * \pre socket must be set with setSocket().
   */
  mdtAbstractPort::error_t mapSocketError(QAbstractSocket::SocketError error, bool byRead);

  QString pvHost;
  quint16 pvPortNumber;
  int pvConnectMaxTry;
  char *pvReadBuffer;
  int pvReadBufferSize;
  QTimer *pvConnectionTimeoutTimer;
  QTimer *pvReadTimeoutTimer;
  QTimer *pvWriteTimeoutTimer;
  QTcpSocket * pvSocket;

  Q_DISABLE_COPY(mdtPortThreadHelperSocket);
};

#endif

// #ifndef MDT_PORT_THREAD_HELPER_SOCKET_H
