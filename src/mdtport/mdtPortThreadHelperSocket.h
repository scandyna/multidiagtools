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
#ifndef MDT_PORT_THREAD_HELPER_SOCKET_H
#define MDT_PORT_THREAD_HELPER_SOCKET_H


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

    QAbstractSocket * pvSocket;
};

#endif

// #ifndef MDT_PORT_THREAD_HELPER_SOCKET_H
