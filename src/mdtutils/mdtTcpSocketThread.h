#ifndef MDT_TCP_SOCKET_THREAD_H
#define MDT_TCP_SOCKET_THREAD_H

#include <QObject>
#include <QTcpSocket>
#include <QQueue>
#include <QWaitCondition>
#include <QMutex>
#include "mdtPortThread.h"
#include "mdtFrame.h"

class mdtTcpSocketThread : public mdtPortThread
{
 Q_OBJECT

 public:

  mdtTcpSocketThread(QObject *parent = 0);

  // Overload of mdtPortThread
  void stop();
  
  void connectToHost(QString hostName, quint16 hostPort);
  
  void beginNewTransaction();
  
 signals:

  void newFrameReaden();

 private:

  void readFromSocket();

  mdtFrame *getNewFrameRead();

  void writeToSocket();

  mdtFrame *getNewFrameWrite();

  // Try to reconnect when host disconnected
  // Returns true on success reconnection
  // If the maximum reconnect is reached, this method returns false
  // Note: on each retry, a sleep time is increased
  bool reconnectToHost();

  void run();

  char *pvReadBuffer;
  qint64 pvReadBufferSize;
  mdtFrame *pvReadCurrentFrame;
  mdtFrame *pvWriteCurrentFrame;
  QTcpSocket *pvSocket;
  int pvMaxReconnect;               // Max. number of reconnections
  QQueue<int>pvTransactionIds;      // Transactions list
  QWaitCondition pvNewTransaction;  // Transaction begin condition
  QMutex pvMutex;
  QString pvPeerName;               // Host name or IP
  quint16 pvPeerPort;               // Host port
};

#endif  // #ifndef MDT_TCP_SOCKET_THREAD_H
