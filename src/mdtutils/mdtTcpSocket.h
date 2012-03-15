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
