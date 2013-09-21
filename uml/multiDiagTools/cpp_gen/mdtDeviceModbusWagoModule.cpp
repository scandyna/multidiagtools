
#include "mdtDeviceModbusWagoModule.h"
#include "mdtDeviceModbusWago.h"
#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"

mdtDeviceModbusWagoModule::mdtDeviceModbusWagoModule(bool autoDeleteIos) {
}

mdtDeviceModbusWagoModule::~mdtDeviceModbusWagoModule() {
}

bool mdtDeviceModbusWagoModule::setupFromRegisterWord(const quint16 & word) {
}

void mdtDeviceModbusWagoModule::setFirstAddresse(int addressRead, int addressWrite) {
}

int mdtDeviceModbusWagoModule::firstAddressRead() {
}

int mdtDeviceModbusWagoModule::firstAddressWrite() {
}

int mdtDeviceModbusWagoModule::lastAddressRead() {
}

int mdtDeviceModbusWagoModule::lastAddressWrite() {
}

int mdtDeviceModbusWagoModule::ioCount() const {
}

QList<mdtAnalogIo*> mdtDeviceModbusWagoModule::analogIos() {
}

QList<mdtDigitalIo*> mdtDeviceModbusWagoModule::digitalIos() {
}

type_t mdtDeviceModbusWagoModule::type() const {
}

QString mdtDeviceModbusWagoModule::typeText() const {
}

int mdtDeviceModbusWagoModule::partNumber() const {
}

QString mdtDeviceModbusWagoModule::partNumberText() const {
}

QVariant mdtDeviceModbusWagoModule::analogIoModuleValueMin(int partNumber) const {
}

QVariant mdtDeviceModbusWagoModule::analogIoModuleValueMax(int partNumber) const {
}

QVariant mdtDeviceModbusWagoModule::analogIoModuleValueBitsCount(int partNumber) const {
}

QVariant mdtDeviceModbusWagoModule::analogIoModuleValueLsbIndex(int partNumber) const {
}

QVariant mdtDeviceModbusWagoModule::analogIoModuleValueSigned(int partNumber) const {
}

QVariant mdtDeviceModbusWagoModule::analogIoModuleIsInput(int partNumber) const {
}

QVariant mdtDeviceModbusWagoModule::analogIoModuleIosCount(int partNumber) const {
}

QVariant mdtDeviceModbusWagoModule::analogIoModuleUnit(int partNumber) const {
}

QVariant mdtDeviceModbusWagoModule::digitalIoModuleIsInput(const quint16 & word) const {
}

QVariant mdtDeviceModbusWagoModule::digitalIoModuleIosCount(const quint16 & word) const {
}

bool mdtDeviceModbusWagoModule::addAnalogIos() {
}

bool mdtDeviceModbusWagoModule::addDigitalIos() {
}

quint8 mdtDeviceModbusWagoModule::statusByte(int channel) const {
}

void mdtDeviceModbusWagoModule::setControlByte(int channel, const quint8 & byte) {
}

quint8 mdtDeviceModbusWagoModule::controlByte(int channel) const {
}

void mdtDeviceModbusWagoModule::setRegisterDataWord(int channel, const quint16 & word) {
}

quint16 mdtDeviceModbusWagoModule::registerDataWord(int channel) {
}

void mdtDeviceModbusWagoModule::setRegisterDataBytes(int channel, const quint8 & byteD1, const quint8 & byteD0) {
}

quint8 mdtDeviceModbusWagoModule::registerDataByteD1(int channel) {
}

quint8 mdtDeviceModbusWagoModule::registerDataByteD0(int channel) {
}

//Will also read status byte
bool mdtDeviceModbusWagoModule::readRegisters(int firstChannel, int lastChannel, bool needDeviceSetupState) {
}

//Will also write control byte
bool mdtDeviceModbusWagoModule::writeRegisters(int firstChannel, int lastChannel, bool needDeviceSetupState) {
}

bool mdtDeviceModbusWagoModule::readStatusBytes(int firstChannel, int lastChannel, bool needDeviceSetupState) {
}

bool mdtDeviceModbusWagoModule::writeControlBytes(int firstChannel, int lastChannel, bool needDeviceSetupState) {
}

