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
#ifndef MDT_TCP_SERVER_H
#define MDT_TCP_SERVER_H

#include <QTcpServer>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QMutex>

// Minimal TCP server used by the Unit Tests
class mdtTcpServer : public QTcpServer
{
 Q_OBJECT

 public:

  mdtTcpServer(QObject *parent = 0);

  // Set the list of data to return
  void setResponseData(const QStringList &data);
  
 protected:

  // Called by incomming TCP connection
  void incomingConnection(int socketDescriptor);

 private:
  
  QStringList pvResponses;
  QMutex pvMutex;
};

#endif  // #ifndef MDT_TCP_SERVER_H
