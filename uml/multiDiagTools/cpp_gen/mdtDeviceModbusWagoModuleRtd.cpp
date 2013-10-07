
#include "mdtDeviceModbusWagoModuleRtd.h"
#include "mdtDeviceModbusWago.h"

mdtDeviceModbusWagoModuleRtd::mdtDeviceModbusWagoModuleRtd(bool autoDeleteIos, const mdtDeviceModbusWago & device) {
}

mdtDeviceModbusWagoModuleRtd::~mdtDeviceModbusWagoModuleRtd() {
}

bool mdtDeviceModbusWagoModuleRtd::getSpecialModuleSetup(const quint16 & word, int firstAddressRead, int firstAddressWrite) {
}

//Will also enter password 0x1235 in register 31
bool mdtDeviceModbusWagoModuleRtd::enterSetupMode() {
}

bool mdtDeviceModbusWagoModuleRtd::exitSetupMode() {
}

void mdtDeviceModbusWagoModuleRtd::setIosCount(int count) {
}

void mdtDeviceModbusWagoModuleRtd::setPssrMode(const pssrMode_t & mode) {
}

pssrMode_t mdtDeviceModbusWagoModuleRtd::pssrMode() const {
}

void mdtDeviceModbusWagoModuleRtd::setSensorType(int channel, const sensorType_t & type) {
}

sensorType_t mdtDeviceModbusWagoModuleRtd::sensorType(int channel) const {
}

void mdtDeviceModbusWagoModuleRtd::setConnectionTypeWiresCount(int channel, int wiresCount) {
}

int mdtDeviceModbusWagoModuleRtd::connectionTypeWiresCount(int channel) const {
}

void mdtDeviceModbusWagoModuleRtd::setWatchdogTimerEnabled(int channel, bool enable) {
}

bool mdtDeviceModbusWagoModuleRtd::watchdogTimerIsEnabled(int channel) const {
}

void mdtDeviceModbusWagoModuleRtd::setAverageValueFilterEnabled(int channel, bool enable) {
}

bool mdtDeviceModbusWagoModuleRtd::averageValueFilterIsEnabled(int channel) const {
}

void mdtDeviceModbusWagoModuleRtd::setProcessValueRepresentation(int channel, const processValueRepresentation_t & representation) {
}

processValueRepresentation_t mdtDeviceModbusWagoModuleRtd::processValueRepresentation(int channel) {
}

void mdtDeviceModbusWagoModuleRtd::setSiemensFormatEnabled(int channel, bool enable) {
}

bool mdtDeviceModbusWagoModuleRtd::siemensFormatIsEnabled(int channel) const {
}

void mdtDeviceModbusWagoModuleRtd::setWireBreakShortCircuitIndicationEnabled(int channel, bool enable) {
}

bool mdtDeviceModbusWagoModuleRtd::wireBreakShortCircuitIndicationIsEnabled(int channel) const {
}

void mdtDeviceModbusWagoModuleRtd::setOutOfRangeIndicationEnabled(int channel, bool enable) {
}

bool mdtDeviceModbusWagoModuleRtd::outOfRangeIndicationIsEnabled(int channel) const {
}

void mdtDeviceModbusWagoModuleRtd::setOutOfRangeIndicationLimits(int channel, int underrange, int overrange) {
}

int mdtDeviceModbusWagoModuleRtd::underrangeIndicationLimit(int channel) {
}

int mdtDeviceModbusWagoModuleRtd::overrangeIndicationLimit(int channel) {
}

void mdtDeviceModbusWagoModuleRtd::setManufacturerCalibration(int channel) {
}

void mdtDeviceModbusWagoModuleRtd::setUserCalibration(int channel, double offset, double gain) {
}

double mdtDeviceModbusWagoModuleRtd::calibrationOffset() const {
}

double mdtDeviceModbusWagoModuleRtd::calibrationGain() const {
}

void mdtDeviceModbusWagoModuleRtd::setScaling(int channel, bool useManufacturerScaling, double userOffset, double userGain, double twoWireOffset) {
}

bool mdtDeviceModbusWagoModuleRtd::manufacturerScalingIsEnabled(int channel) const {
}

double mdtDeviceModbusWagoModuleRtd::manufacturerScalingOffset(int channel) const {
}

double mdtDeviceModbusWagoModuleRtd::manufacturerScalingGain(int channel) const {
}

double mdtDeviceModbusWagoModuleRtd::userScalingOffset(int channel) const {
}

double mdtDeviceModbusWagoModuleRtd::userScalingGain(int channel) const {
}

double mdtDeviceModbusWagoModuleRtd::twoWireOffset(int channel) const {
}

