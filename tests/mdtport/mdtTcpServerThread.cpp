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
#include "mdtTcpServerThread.h"
#include <QTcpSocket>
#include <QtTest/QtTest>

#include <QDebug>

mdtTcpServerThread::mdtTcpServerThread(int socketDescriptor, const QStringList &data, QObject *parent)
 : QThread(parent)
{
  pvSocketDescriptor = socketDescriptor;
  pvResponses = data;
}

void mdtTcpServerThread::run()
{
  QTcpSocket socket;
  int i;

  QVERIFY(socket.setSocketDescriptor(pvSocketDescriptor));

  qDebug() << "*SRV mdtTcpServerThread::run(): start ...";
  
  // Send the data
  for(i=0; i<pvResponses.size(); i++){
    socket.write(pvResponses.at(i).toAscii());
    if(!socket.waitForBytesWritten()){
      qDebug() << "*SRV mdtTcpServerThread::run(): waitForBytesWritten() failed";
      qDebug() << "*SRV mdtTcpServerThread::run(): system returned: " << socket.errorString();
      break;
    }
    qDebug() << "*SRV mdtTcpServerThread::run(): bytes written";
    ///QVERIFY2(socket.waitForBytesWritten() , "Note: it's possible that host (client) has closed the connection");
  }
  pvResponses.clear();
  socket.disconnectFromHost();
  qDebug() << "*SRV mdtTcpServerThread::run(): END (disconnected)";
}
