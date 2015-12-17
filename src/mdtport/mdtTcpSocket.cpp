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
#include "mdtTcpSocket.h"
#include "mdtError.h"
#include <QStringList>

mdtTcpSocket::mdtTcpSocket(QObject *parent)
 : mdtAbstractPort(parent)
{
  pvPeerPort = 0;
}

mdtTcpSocket::~mdtTcpSocket()
{
  close();
}

QString mdtTcpSocket::peerName() const
{
  return pvPeerName;
}

quint16 mdtTcpSocket::peerPort() const
{
  return pvPeerPort;
}

void mdtTcpSocket::setReadTimeout(int timeout)
{
}

void mdtTcpSocket::setWriteTimeout(int timeout)
{
}

mdtAbstractPort::error_t mdtTcpSocket::waitForReadyRead()
{
  return mdtAbstractPort::UnhandledError;
}

qint64 mdtTcpSocket::read(char *data, qint64 maxSize)
{
  return mdtAbstractPort::UnhandledError;
}

mdtAbstractPort::error_t mdtTcpSocket::waitEventWriteReady()
{
  return mdtAbstractPort::UnhandledError;
}

qint64 mdtTcpSocket::write(const char *data, qint64 maxSize)
{
  return mdtAbstractPort::UnhandledError;
}

void mdtTcpSocket::addFrameToWrite(mdtFrame *frame)
{
  Q_ASSERT(frame != 0);

  pvWriteFrames.enqueue(frame);
  emit newFrameToWrite();
}

mdtAbstractPort::error_t mdtTcpSocket::pvOpen()
{
  Q_ASSERT(!isOpen());

  QStringList items;
  bool ok;

  // Extract host name and port number from port name
  items = pvPortName.split(":");
  if(items.size() != 2){
//     mdtError e(MDT_TCP_IO_ERROR, "Cannot extract host name and port name in (format must be hostname:port) " + pvPortName, mdtError::Error);
//     MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
//     e.commit();
    return SetupError;
  }
  pvPeerName = items.at(0);
  pvPeerPort = items.at(1).toUShort(&ok);
  if(!ok){
//     mdtError e(MDT_TCP_IO_ERROR, "Cannot extract port in (format must be hostname:port) " + pvPortName, mdtError::Error);
//     MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
//     e.commit();
    return SetupError;
  }

  return NoError;
}

void mdtTcpSocket::pvClose()
{
  Q_ASSERT(isOpen());
}

mdtAbstractPort::error_t mdtTcpSocket::pvSetup()
{
  Q_ASSERT(isOpen());

  // Set R/W timeouts
  setReadTimeout(config().readTimeout());
  setWriteTimeout(config().writeTimeout());

  return NoError;
}

void mdtTcpSocket::pvFlushIn()
{
}

void mdtTcpSocket::pvFlushOut()
{
}
