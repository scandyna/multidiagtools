#ifndef _MDTPORTTHREADHELPERSOCKET_H
#define _MDTPORTTHREADHELPERSOCKET_H


#include "mdtPortThreadHelper.h"

class mdtPortThreadHelperSocket : public mdtPortThreadHelper {
  private:
    bool pvConnected;


  public:
    void setSocket(const QAbstractSocket & socket);


  private:
    void onSocketConnected();

    void onSocketDisconnected();

    void onSocketClosing();

    void onSocketError(const QAbstractSocket & socketError);

    void onSocketHostFound();

    void onSocketStateChanged(const QAbstractSocket::SocketState & socketState);

    void onSocketReadyRead();

    void onSocketBytesWritten(const qInt64 & bytes);

    void onConnectionTimeout();

    void onReadTimeout();

    void onWriteTimeout();

    virtual void requestWrite();

    mdtAbstractPort::error_t writeToSocket();

    mdtAbstractPort::error_t mapSocketError(const QAbstractSocket::SocketError & error, bool byRead);

    QString pvHost;

    quint16 pvPort;

    int pvConnectionTimeout;

    QTimer pvConnectionTimeoutTimer;

    int pvReadTimeout;

    QTimer pvReadTimeoutTimer;

    int pvWriteTimeout;

    QTimer pvWriteTimeoutTimer;

};
#endif
