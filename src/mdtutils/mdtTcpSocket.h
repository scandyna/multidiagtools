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

  // Implementation of mdtAbstractPort
  bool setAttributes(const QString &portName);

  // Implementation of mdtAbstractPort - Locks the mutex
  bool open(mdtPortConfig &cfg);

  // Implementation of mdtAbstractPort - Locks the mutex
  void close();

  /*! \brief Connect to host
   * 
   * Use this method to start a connection to a host.<br>
   * Mutex is locked by this method.
   * \param hostName Hostname or IP address
   * \param hostPort Port
   * \pre hostPort must be > 0
   */
  void connectToHost(const QString &hostName, int hostPort);

  /*! \brief Set the socket
   * 
   * QTcpSocket must be created from thread on witch it will be used.
   * In this case, the mdtTcpSocketThread create the instance.<br>
   * This method will be called to store this instance and an instance of the thread itself.
   * Note: should not be used.
   * \pre socket must be a valid pointer
   * \pre thread must be a valid pointer
   * \sa mdtTcpSocketThread
   */
  void setThreadObjects(QTcpSocket *socket, mdtTcpSocketThread *thread);

  // Implementation of mdtAbstractPort
  void setReadTimeout(int timeout);

  // Implementation of mdtAbstractPort
  void setWriteTimeout(int timeout);

  /*! \brief Start a new transaction
   * 
   * Mutex is locked by this method.
   */
  void beginNewTransaction();

  /*! \brief Block until a new transaction starts
   * 
   * Note: used by thread , should not be used directly.<br>
   * Mutex is locked by this method.
   * \sa mdtTcpSocketThread
   */
  void waitForNewTransaction();
  
  // Implementation of mdtAbstractPort - Locks the mutex
  bool waitForReadyRead();

  // Implementation of mdtAbstractPort
  qint64 read(char *data, qint64 maxSize);

  // Implementation of mdtAbstractPort - Locks the mutex
  bool waitEventWriteReady();

  // Implementation of mdtAbstractPort
  qint64 write(const char *data, qint64 maxSize);

 private:

  int pvReadTimeout;
  int pvWriteTimeout;
  QWaitCondition pvNewTransaction;
  QTcpSocket *pvSocket;             // QTcpSocket object passed from thread
  QQueue<int> pvTransactionIds;     // Transactions list
  mdtTcpSocketThread *pvThread;
};

#endif  // #ifndef MDT_TCP_SOCKET_H
