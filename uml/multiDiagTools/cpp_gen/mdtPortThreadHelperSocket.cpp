
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

