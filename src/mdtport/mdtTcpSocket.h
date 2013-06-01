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
#ifndef MDT_TCP_SOCKET_H
#define MDT_TCP_SOCKET_H

#include "mdtAbstractPort.h"
#include <QObject>

class mdtTcpSocketThread;

/*! \brief Wrapper class between QTcpSocket and mdtAbstractPort
 */
class mdtTcpSocket : public mdtAbstractPort
{
 Q_OBJECT

 public:

  mdtTcpSocket(QObject *parent = 0);
  ~mdtTcpSocket();

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

  /*! \brief Just to be compatible with mdtAbstractPort API
   *
   * Does nothing
   */
  void setReadTimeout(int timeout);

  /*! \brief Just to be compatible with mdtAbstractPort API
   *
   * Does nothing
   */
  void setWriteTimeout(int timeout);

  /*! \brief Just to be compatible with mdtAbstractPort API
   *
   * \return UnhandledError
   */
  error_t waitForReadyRead();

  /*! \brief Just to be compatible with mdtAbstractPort API
   *
   * \return UnhandledError
   */
  qint64 read(char *data, qint64 maxSize);

  /*! \brief Just to be compatible with mdtAbstractPort API
   *
   * \return UnhandledError
   */
  error_t waitEventWriteReady();

  /*! \brief Just to be compatible with mdtAbstractPort API
   *
   * \return UnhandledError
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
   * Will just extract host and port number in portName.
   *  Returns SetupError if portName has wrong format.
   *  (Expected format is host:port).
   *
   * The real connection is made in mdtTcpSocketThread.
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

  QString pvPeerName;               // Host name or IP
  quint16 pvPeerPort;               // Host port
};

#endif  // #ifndef MDT_TCP_SOCKET_H
