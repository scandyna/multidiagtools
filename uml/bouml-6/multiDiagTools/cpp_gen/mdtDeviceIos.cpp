
#include "mdtDeviceIos.h"
#include "mdtDeviceIosSegment.h"

mdtDeviceIos::mdtDeviceIos(const QObject & parent) {
}

mdtDeviceIos::~mdtDeviceIos() {
}

void mdtDeviceIos::setAutoDeleteIos(bool autoDelete) {
}

void mdtDeviceIos::deleteIos() {
}

void mdtDeviceIos::addAnalogInput(const mdtAnalogIo & ai) {
}

mdtAnalogIo mdtDeviceIos::analogInputAt(int address) {
}

QList<mdtAnalogIo> mdtDeviceIos::analogInputs() {
}

void mdtDeviceIos::addAnalogOutput(const mdtAnalogIo & ao) {
}

mdtAnalogIo mdtDeviceIos::analogOutputAt(int address) {
}

QList<mdtAnalogIo> mdtDeviceIos::analogOutputs() {
}

void mdtDeviceIos::addDigitalInput(const mdtDigitalIo & di) {
}

mdtDigitalIo mdtDeviceIos::digitalInputAt(int address) {
}

QList<mdtDigitalIo> mdtDeviceIos::digitalInputs() {
}

void mdtDeviceIos::addDigitalOutput(const mdtDigitalIo & di) {
}

mdtDigitalIo mdtDeviceIos::digitalOutputAt(int address) {
}

QList<mdtDigitalIo> mdtDeviceIos::digitalOutputs() {
}

void mdtDeviceIos::analogOutputValueChanged(int address, double value) {
}

void mdtDeviceIos::digitalOutputStateChanged(int address, bool state) {
}

void mdtDeviceIos::updateAnalogInputValue(int address, double value) {
}

void mdtDeviceIos::updateAnalogOutputValue(int address, double value) {
}

void mdtDeviceIos::updateDigitalInputState(int address, bool state) {
}

void mdtDeviceIos::updateDigitalOutputState(int address, bool state) {
}

