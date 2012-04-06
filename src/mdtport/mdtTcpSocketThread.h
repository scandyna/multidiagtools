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
#ifndef MDT_TCP_SOCKET_THREAD_H
#define MDT_TCP_SOCKET_THREAD_H

#include <QObject>
#include <QTcpSocket>
#include <QQueue>
#include <QWaitCondition>
#include <QMutex>
#include "mdtPortThread.h"
#include "mdtFrame.h"

class mdtTcpSocketThread : public mdtPortThread
{
 Q_OBJECT

 public:

  mdtTcpSocketThread(QObject *parent = 0);

  // Overload of mdtPortThread
  void stop();

  void connectToHost(QString hostName, quint16 hostPort);

 signals:

  /*! \brief New frame was readen.
   * 
   * This signal is emitted when a new frame was readen.
   * The frame can be complete or not (depending of the EOF condition)
   * \sa mdtFrame
   */
  void newFrameReaden();

 private:

  void readFromSocket();

  // Get a frame in read frames pool
  mdtFrame *getNewFrameRead();

  void writeToSocket();

  // Get a frame in frames to write
  mdtFrame *getNewFrameWrite();

  // Try to reconnect when host disconnected
  // Returns true on success reconnection
  // If the maximum reconnect is reached, this method returns false
  // Note: on each retry, a sleep time is increased
  bool reconnectToHost();

  void run();

  char *pvReadBuffer;
  qint64 pvReadBufferSize;
  mdtFrame *pvReadCurrentFrame;
  mdtFrame *pvWriteCurrentFrame;
  QTcpSocket *pvSocket;
  int pvMaxReconnect;               // Max. number of reconnections
  QString pvPeerName;               // Host name or IP
  quint16 pvPeerPort;               // Host port
};

#endif  // #ifndef MDT_TCP_SOCKET_THREAD_H
