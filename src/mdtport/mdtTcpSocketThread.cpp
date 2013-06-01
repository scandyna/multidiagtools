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
#include "mdtTcpSocketThread.h"
#include "mdtPortThreadHelperSocket.h"

#include "mdtPortThreadHelperPort.h"

#include "mdtTcpSocket.h"
#include "mdtError.h"
#include <QApplication>

#include <QDebug>

mdtTcpSocketThread::mdtTcpSocketThread(QObject *parent)
 : mdtPortThread(parent)
{
  ///pvSocket = 0;
}

bool mdtTcpSocketThread::isReader() const
{
  return true;
}

bool mdtTcpSocketThread::isWriter() const
{
  return true;
}

bool mdtTcpSocketThread::handlesTimeout() const
{
  return true;
}

void mdtTcpSocketThread::run()
{
  Q_ASSERT(pvPort != 0);

  mdtTcpSocket *port = 0;
  QTcpSocket *socket;
  mdtPortThreadHelperSocket threadHelper;

  pvPort->lockMutex();
#ifdef Q_OS_UNIX
  pvNativePthreadObject = pthread_self();
  Q_ASSERT(pvNativePthreadObject != 0);
#endif
  // Create a Tcp socket
  socket = new QTcpSocket;  /// \todo Check about delete !
  Q_ASSERT(socket != 0);
  // We must pass this socket object to pvPort
  // pvPort must be a instance of mdtTcpSocket
  port = dynamic_cast<mdtTcpSocket*>(pvPort);
  Q_ASSERT(port != 0);
  ///port->setThreadObjects(socket, this);

  threadHelper.setPort(pvPort);
  threadHelper.setThread(this);
  threadHelper.setSocket(socket);

  qDebug() << "TCPTHD: starting ...";
  pvPort->unlockMutex();
  // Run...
  exec();
  /// \todo Cleanup
  notifyError(mdtAbstractPort::Disconnected);
  // Cleanup
  delete socket;
  qDebug() << "TCPTHD: exec() END";
}
