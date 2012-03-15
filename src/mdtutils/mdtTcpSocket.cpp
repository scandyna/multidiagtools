
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
