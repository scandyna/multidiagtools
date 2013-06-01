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

  QTcpSocket *socket;
  mdtPortThreadHelperSocket threadHelper;

  pvPort->lockMutex();
#ifdef Q_OS_UNIX
  pvNativePthreadObject = pthread_self();
  Q_ASSERT(pvNativePthreadObject != 0);
#endif
  // Create a Tcp socket
  socket = new QTcpSocket;
  Q_ASSERT(socket != 0);
  // Setup thread helper
  threadHelper.setPort(pvPort);
  threadHelper.setThread(this);
  threadHelper.setSocket(socket);

  pvPort->unlockMutex();
  // Run...
  exec();
  notifyError(mdtAbstractPort::Disconnected);
  // Cleanup
  delete socket;
}
