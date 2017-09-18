
#include "mdtDeviceModbus.h"

mdtDeviceModbus::mdtDeviceModbus(const QObject & parent) {
}

mdtAbstractPort::error_t mdtDeviceModbus::connectToDevice(const QList<mdtPortInfo*> & scanResult, int hardwareNodeId, int bitsCount, int startFrom) 
{
}

bool mdtDeviceModbus::getRegisterValues(int address, int n) {
}

QList<int> mdtDeviceModbus::registerValues() const {
}

QVariant mdtDeviceModbus::getAnalogInputValue(int address, bool readDirectly) {
}

bool mdtDeviceModbus::setAnalogOutput(int address, int value, bool writeDirectly) {
}

QVariant mdtDeviceModbus::getDigitalInputState(int address, bool readDirectly) {
}

bool mdtDeviceModbus::setDigitalOutput(int address, bool state, bool writeDirectly) {
}

bool mdtDeviceModbus::readAnalogInputs() {
}

bool mdtDeviceModbus::writeAnalogOutputs() {
}

bool mdtDeviceModbus::readDigitalInputs() {
}

bool mdtDeviceModbus::writeDigitalOutputs(const mdtPortTransaction & transaction) {
}

