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
#include "mdtTcpServer.h"
#include "mdtTcpServerThread.h"

mdtTcpServer::mdtTcpServer(QObject *parent)
 : QTcpServer(parent)
{
}

void mdtTcpServer::setResponseData(const QStringList &data)
{
  pvMutex.lock();
  pvResponses = data;
  pvMutex.unlock();
}

void mdtTcpServer::incomingConnection(int socketDescriptor)
{
  // Init a new TCP thread and start it
  pvMutex.lock();
  mdtTcpServerThread *tcpThd = new mdtTcpServerThread(socketDescriptor, pvResponses, this);
  connect(tcpThd, SIGNAL(finished()), tcpThd, SLOT(deleteLater()));
  tcpThd->start();
  tcpThd->wait();
  pvMutex.unlock();
}

