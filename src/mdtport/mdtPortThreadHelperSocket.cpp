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
#include "mdtPortThreadHelperSocket.h"

void mdtPortThreadHelperSocket::setSocket(const QAbstractSocket & socket) {
}

void mdtPortThreadHelperSocket::onSocketConnected() {
}

void mdtPortThreadHelperSocket::onSocketDisconnected() {
}

void mdtPortThreadHelperSocket::onSocketClosing() {
}

void mdtPortThreadHelperSocket::onSocketError(const QAbstractSocket & socketError) {
}

void mdtPortThreadHelperSocket::onSocketHostFound() {
}

void mdtPortThreadHelperSocket::onSocketStateChanged(const QAbstractSocket::SocketState & socketState) {
}

void mdtPortThreadHelperSocket::onSocketReadyRead() {
}

void mdtPortThreadHelperSocket::onSocketBytesWritten(const qInt64 & bytes) {
}

void mdtPortThreadHelperSocket::onConnectionTimeout() {
}

void mdtPortThreadHelperSocket::onReadTimeout() {
}

void mdtPortThreadHelperSocket::onWriteTimeout() {
}

void mdtPortThreadHelperSocket::requestWrite() {
}

mdtAbstractPort::error_t mdtPortThreadHelperSocket::writeToSocket() {
}

mdtAbstractPort::error_t mdtPortThreadHelperSocket::mapSocketError(const QAbstractSocket::SocketError & error, bool byRead) {
}

