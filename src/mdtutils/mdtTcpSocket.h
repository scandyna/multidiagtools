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

class mdtTcpSocket : public mdtAbstractPort
{
 Q_OBJECT

 public:

  mdtTcpSocket(QObject *parent = 0);
  ~mdtTcpSocket();

  // Implementation of mdtAbstractPort
  bool setAttributes(const QString &portName);

  // Implementation of mdtAbstractPort
  bool open(mdtPortConfig &cfg);

  // Implementation of mdtAbstractPort
  void close();

  bool connectToHost(const QString &host, int port);
  
  // Implementation of mdtAbstractPort
  void setReadTimeout(int timeout);

  // Implementation of mdtAbstractPort
  void setWriteTimeout(int timeout);

  // Implementation of mdtAbstractPort
  bool waitForReadyRead();

  // Implementation of mdtAbstractPort
  qint64 read(char *data, qint64 maxSize);

  // Implementation of mdtAbstractPort
  bool waitEventWriteReady();

  // Implementation of mdtAbstractPort
  qint64 write(const char *data, qint64 maxSize);

 private:

  int pvReadTimeout;
  int pvWriteTimeout;
  QWaitCondition pvConnected;
  QMutex pvSocketMutex;
};

#endif  // #ifndef MDT_TCP_SOCKET_H
