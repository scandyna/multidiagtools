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
#include "mdtTcpSocket.h"
#include "mdtError.h"
#include <QStringList>


#include <QDebug>

mdtTcpSocket::mdtTcpSocket(QObject *parent)
 : mdtAbstractPort(parent)
{
}

mdtTcpSocket::~mdtTcpSocket()
{
  close();
}

bool mdtTcpSocket::setAttributes(const QString &portName)
{
  //qDebug() << "setAttributes(): " << portName.split(":");
  
  return true;
}

bool mdtTcpSocket::open(mdtPortConfig &cfg)
{
  // Close previous opened connection
  close();
  // Open new connection
  lockMutex();
  qDebug() << "Open ....";
  
  // Set R/W timeouts
  setReadTimeout(cfg.readTimeout());
  setWriteTimeout(cfg.writeTimeout());

  return mdtAbstractPort::open(cfg);
  
  int available;

  QByteArray buffer;
  
  // Trame Modbus
  buffer.clear();
  buffer.append((char)0x15);
  buffer.append((char)0x01);
  buffer.append((char)0);
  buffer.append((char)0);
  buffer.append((char)0);
  buffer.append((char)6);
  buffer.append((char)0xFF);
  buffer.append((char)3);
  buffer.append((char)0);
  buffer.append((char)4);
  buffer.append((char)0);
  buffer.append((char)1);


  return mdtAbstractPort::open(cfg);
}

void mdtTcpSocket::close()
{
  lockMutex();
  qDebug() << "Close ....";
  mdtAbstractPort::close();
}

bool mdtTcpSocket::connectToHost(const QString &host, int port)
{
  return false;
}

void mdtTcpSocket::setReadTimeout(int timeout)
{
  pvReadTimeout = timeout;
}

void mdtTcpSocket::setWriteTimeout(int timeout)
{
  pvWriteTimeout = timeout;
}

bool mdtTcpSocket::waitForReadyRead()
{
  return false;
}

qint64 mdtTcpSocket::read(char *data, qint64 maxSize)
{
  return -1;
}

bool mdtTcpSocket::waitEventWriteReady()
{
  return false;
}

qint64 mdtTcpSocket::write(const char *data, qint64 maxSize)
{
  return -1;
}
