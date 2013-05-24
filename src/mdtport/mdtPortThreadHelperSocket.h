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

  /*! \brief Utile ??????
   */
  void onSocketClosing();

  /*! \brief Map socket error to mdtAbstractPort error and notify it
   *
   * \pre Port must be set with mdtPortThreadHelper::setPort().
   * \pre Thread must be set with mdtPortThreadHelper::setThread().
   * \pre socket must be set with setSocket().
   */
  void onSocketError(QAbstractSocket::SocketError socketError);

  /*! \brief  Utile ??
   */
  void onSocketHostFound();

  /*! \brief Some states are mapped to mdtAbstractPort error and notified
   *
   * We try to not interfer with onSocketError(),
   *  so only a few QAbstractSocket states are notified.
   */
  void onSocketStateChanged(QAbstractSocket::SocketState socketState);

  /*! \brief
   */
  void onSocketReadyRead();

  /*! \brief
   */
  void onSocketBytesWritten(qint64 bytes);

  /*! \brief
   */
  void onConnectionTimeout();

  /*! \brief
   */
  void onReadTimeout();

  /*! \brief
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
   * \return NoError on success or a mdtAbstractPort::error_t error (See also mdtPortThreadHelper::handleCommonReadErrors() ).
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

  
public:
  
  
  /*! \brief Try to (re-)connect to host
   *
   * Each time this method is called,
   *  the internal connections counter will be decremented.
   *  On success, it is reset to initial value (set by setSocket() ).
   *  If a counter reaches 0, UnhandledError will be returned and
   *  a message is logged with mdtError system.
   *
   * \param notify If true, errors are notified (using mdtPortThreadHelper::notifyError() ).
   *
   * \pre port must be set with mdtPortThreadHelper::setPort().
   * \pre thread must be set with mdtPortThreadHelper::setThread().
   * \pre socket must be set with setSocket().
   */
  mdtAbstractPort::error_t reconnect(bool notify);

  
private:
  
  
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
  int pvConnectionTimeout;
  int pvConnectMaxTry;
  int pvConnectTryLeft;
  char *pvReadBuffer;
  int pvReadBufferSize;

  int pvReadTimeout;
  QTimer *pvReadTimeoutTimer;
  int pvWriteTimeout;
  QTimer *pvWriteTimeoutTimer;
  QTcpSocket * pvSocket;
  bool pvConnected;

  Q_DISABLE_COPY(mdtPortThreadHelperSocket);
};

#endif

// #ifndef MDT_PORT_THREAD_HELPER_SOCKET_H
