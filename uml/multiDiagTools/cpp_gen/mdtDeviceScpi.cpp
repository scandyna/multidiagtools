
#include "mdtDeviceScpi.h"
#include "mdtDeviceInfo.h"

mdtDeviceScpi::mdtDeviceScpi(const QObject & parent) {
}

mdtDeviceScpi::~mdtDeviceScpi() {
}

mdtAbstractPort::error_t mdtDeviceScpi::connectToDevice(const mdtDeviceInfo & devInfo) {
}

int mdtDeviceScpi::sendCommand(const QByteArray & command, int timeout) {
}

QByteArray mdtDeviceScpi::sendQuery(const QByteArray & query, int writeTimeout, int readTimeout) {
}

int mdtDeviceScpi::waitOperationComplete(int timeout) {
}

