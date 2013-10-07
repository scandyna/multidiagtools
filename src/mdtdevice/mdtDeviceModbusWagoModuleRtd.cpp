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
#include "mdtDeviceModbusWagoModuleRtd.h"
#include "mdtDeviceModbusWago.h"
#include "mdtError.h"
#include <QString>

mdtDeviceModbusWagoModuleRtd::mdtDeviceModbusWagoModuleRtd(bool autoDeleteIos, mdtDeviceModbusWago *device)
 : mdtDeviceModbusWagoModule(autoDeleteIos, device)
{
  // Init register 47 to 0
  pvRegister47 = 0;
}

mdtDeviceModbusWagoModuleRtd::~mdtDeviceModbusWagoModuleRtd()
{
}

bool mdtDeviceModbusWagoModuleRtd::getSpecialModuleSetup(quint16 word, int firstAddressRead, int firstAddressWrite)
{
  clear();
  // Add a register to be able to get real channel count of module in register 47
  addRegisters(1, firstAddressRead, firstAddressWrite);
  
  qDebug() << "* Get register 31 ...";
  getRegister(31, 0, 0, true);
  qDebug() << "* Register 31 data words: " << registerDataWord(0) << " [hex]: " << hex << registerDataWord(0);
  
  return false;

  
  qDebug() << "* enter setup ...";
  if(!enterSetupMode()){
    return false;
  }
  
  qDebug() << "* get register 47 ...";
  if(!getRegister47()){
    return false;
  }
  qDebug() << "* get register 47 DONE";
  qDebug() << "* Channels: " << ioCountRegisters();
  qDebug() << "* PSRR mode: " << psrrMode();
  
  
  qDebug() << "* get register 32 (for all channels) ...";
  if(!getRegister32()){
    return false;
  }
  qDebug() << "* Sensor type (channel 0) : " << sensorType(0);
  
  if(!exitSetupMode()){
    return false;
  }

  return false;
}

bool mdtDeviceModbusWagoModuleRtd::enterSetupMode()
{
  setRegisterDataWord(0, 0x1235);
  return setRegister(31, 0, 0, true);
}

bool mdtDeviceModbusWagoModuleRtd::exitSetupMode()
{
  setRegisterDataWord(0, 0x0000);
  return setRegister(31, 0, 0, true);
}

void mdtDeviceModbusWagoModuleRtd::setIoCount(int count)
{
  Q_ASSERT((count == 2)||(count == 4));

  if(count == 2){
    pvRegister47 |= 1;
  }else{
    pvRegister47 &= ~1;
  }
}

int mdtDeviceModbusWagoModuleRtd::ioCountRegisters() const
{
  if(pvRegister47 & 1){
    return 2;
  }else{
    return 4;
  }
}

void mdtDeviceModbusWagoModuleRtd::setPsrrMode(psrrMode_t mode)
{
  switch(mode){
    case Psrr50Hz:
      pvRegister47 &= ~(1 << 8);
      pvRegister47 &= ~(1 << 9);
      break;
    case Psrr60Hz:
      pvRegister47 |= (1 << 8);
      pvRegister47 &= ~(1 << 9);
      break;
    case Psrr5060Hz:
      pvRegister47 &= ~(1 << 8);
      pvRegister47 |= (1 << 9);
      break;
    case PsrrUnknown:
      pvRegister47 |= (1 << 8);
      pvRegister47 |= (1 << 9);
  }
}

mdtDeviceModbusWagoModuleRtd::psrrMode_t mdtDeviceModbusWagoModuleRtd::psrrMode() const
{
  quint8 bits = 0;

  bits = ( ( pvRegister47 & ((1 << 8) | (1 << 9)) ) >> 8);
  switch(bits){
    case 0:
      return Psrr50Hz;
    case 1:
      return Psrr60Hz;
    case 2:
      return Psrr5060Hz;
    default:
      return PsrrUnknown;
  }
}

void mdtDeviceModbusWagoModuleRtd::setSensorType(int channel, sensorType_t type)
{
  Q_ASSERT(channel >= 0);
  Q_ASSERT(channel < pvRegister32.size());

  quint8 bits = 0;
  quint16 reg32 = pvRegister32.at(channel);

  switch(type){
    case Pt100:
      bits = 0;
      break;
    case Ni100:
      bits = 1;
      break;
    case Pt1000:
      bits = 2;
      break;
    case Pt500:
      bits = 3;
      break;
    case Pt200:
      bits = 4;
      break;
    case Ni1000:
      bits = 5;
      break;
    case Ni120:
      bits = 6;
      break;
    case Ni1000TK:
      bits = 7;
      break;
    case Potentiometer:
      bits = 13;
      break;
    case R10R5000:
      bits = 14;
      break;
    case R10R1200:
      bits = 15;
      break;
    case Unknown:
      bits = 12;
      break;
  }
  // Set bits 12 to 15 of register 32 to 0 and set do a OR with bits
  reg32 &= ~(1 << 12);
  reg32 &= ~(1 << 13);
  reg32 &= ~(1 << 14);
  reg32 &= ~(1 << 15);
  reg32 |= (bits << 12);
  pvRegister32[channel] = reg32;
}

mdtDeviceModbusWagoModuleRtd::sensorType_t mdtDeviceModbusWagoModuleRtd::sensorType(int channel) const
{
  Q_ASSERT(channel >= 0);
  Q_ASSERT(channel < pvRegister32.size());

  quint8 bits = 0;
  quint16 mask = 0;

  mask = (1 << 12) | (1 << 13) | (1 << 14) | (1 << 15);
  bits = (pvRegister32.at(channel) & mask) >> 12;
  switch(bits){
    case 0:
      return Pt100;
    case 1:
      return Ni100;
    case 2:
      return Pt1000;
    case 3:
      return Pt500;
    case 4:
      return Pt200;
    case 5:
      return Ni1000;
    case 6:
      return Ni120;
    case 7:
      return Ni1000TK;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
      return Unknown;
    case 13:
      return Potentiometer;
    case 14:
      return R10R5000;
    case 15:
      return R10R1200;
  }
}

void mdtDeviceModbusWagoModuleRtd::setConnectionTypeWiresCount(int channel, int wiresCount)
{
}

int mdtDeviceModbusWagoModuleRtd::connectionTypeWiresCount(int channel) const
{
}

void mdtDeviceModbusWagoModuleRtd::setWatchdogTimerEnabled(int channel, bool enable)
{
}

bool mdtDeviceModbusWagoModuleRtd::watchdogTimerIsEnabled(int channel) const
{
}

void mdtDeviceModbusWagoModuleRtd::setAverageValueFilterEnabled(int channel, bool enable)
{
}

bool mdtDeviceModbusWagoModuleRtd::averageValueFilterIsEnabled(int channel) const
{
}

void mdtDeviceModbusWagoModuleRtd::setProcessValueRepresentation(int channel, processValueRepresentation_t representation)
{
}

mdtDeviceModbusWagoModuleRtd::processValueRepresentation_t mdtDeviceModbusWagoModuleRtd::processValueRepresentation(int channel)
{
}

/**
void mdtDeviceModbusWagoModuleRtd::setSiemensFormatEnabled(int channel, bool enable) {
}
*/

bool mdtDeviceModbusWagoModuleRtd::siemensFormatIsEnabled(int channel) const
{
}

void mdtDeviceModbusWagoModuleRtd::setWireBreakShortCircuitIndicationEnabled(int channel, bool enable)
{
}

bool mdtDeviceModbusWagoModuleRtd::wireBreakShortCircuitIndicationIsEnabled(int channel) const
{
}

void mdtDeviceModbusWagoModuleRtd::setOutOfRangeIndicationEnabled(int channel, bool enable)
{
}

bool mdtDeviceModbusWagoModuleRtd::outOfRangeIndicationIsEnabled(int channel) const
{
}

void mdtDeviceModbusWagoModuleRtd::setOutOfRangeIndicationLimits(int channel, int underrange, int overrange)
{
}

int mdtDeviceModbusWagoModuleRtd::underrangeIndicationLimit(int channel)
{
}

int mdtDeviceModbusWagoModuleRtd::overrangeIndicationLimit(int channel)
{
}

void mdtDeviceModbusWagoModuleRtd::setManufacturerCalibration(int channel)
{
}

void mdtDeviceModbusWagoModuleRtd::setUserCalibration(int channel, double offset, double gain)
{
}

double mdtDeviceModbusWagoModuleRtd::calibrationOffset() const
{
}

double mdtDeviceModbusWagoModuleRtd::calibrationGain() const
{
}

void mdtDeviceModbusWagoModuleRtd::setScaling(int channel, bool useManufacturerScaling, bool useUserScaling, double userOffset, double userGain, double twoWireOffset)
{
  Q_ASSERT(channel < pvRegister32.size());
  Q_ASSERT(channel < pvRegister33.size());

  quint16 reg32 = pvRegister32.at(channel);
  quint16 reg33 = pvRegister33.at(channel);

  // register 32 - bit 0 - User scaling ON flag
  if(useUserScaling){
    reg32 |= 1;
  }else{
    reg32 &= ~1;
  }
  // register 32 - bit 1 - manufacturer scaling ON flag
  if(useManufacturerScaling){
    reg32 |= (1 << 1);
  }else{
    reg32 &= ~(1 << 1);
  }
  // register 33 - User scaling offset
  /// \todo finish .........
  // Store registers
  pvRegister32[channel] = reg32;
  pvRegister33[channel] = reg33;
  qDebug() << "Register 32 [hex]: " << hex << pvRegister32.at(channel);
}

bool mdtDeviceModbusWagoModuleRtd::manufacturerScalingIsEnabled(int channel) const
{
  Q_ASSERT(channel < pvRegister32.size());

  if(pvRegister32.at(channel) & (1 << 1)){
    return true;
  }else{
    return false;
  }
}

double mdtDeviceModbusWagoModuleRtd::manufacturerScalingOffset(int channel) const
{
}

double mdtDeviceModbusWagoModuleRtd::manufacturerScalingGain(int channel) const
{
}

bool mdtDeviceModbusWagoModuleRtd::userScalingIsEnabled(int channel) const
{
  Q_ASSERT(channel < pvRegister32.size());

  if(pvRegister32.at(channel) & 1){
    return true;
  }else{
    return false;
  }
}

double mdtDeviceModbusWagoModuleRtd::userScalingOffset(int channel) const
{
}

double mdtDeviceModbusWagoModuleRtd::userScalingGain(int channel) const
{
}

double mdtDeviceModbusWagoModuleRtd::twoWireOffset(int channel) const
{
}

void mdtDeviceModbusWagoModuleRtd::setInitialSetupForTests(int channelCount, int firstAddressRead, int firstAddressWrite)
{
  addRegisters(channelCount, firstAddressRead, firstAddressWrite);
  setIoCount(channelCount);
  updateRegisterCount();
}

void mdtDeviceModbusWagoModuleRtd::updateRegisterCount()
{
  int firstAddressRD, firstAddressWR;
  int count;
  int i;

  // Get module's addreeses and channel count
  firstAddressRD = firstAddressRead();
  firstAddressWR = firstAddressWrite();
  Q_ASSERT(firstAddressRD >= 0);
  Q_ASSERT(firstAddressWR >= 0);
  count = ioCountRegisters();
  // Update mdtDeviceModbusWagoModule's register count
  if(channelCountRegisters() != count){
    qDebug() << "* Have a new count of C/S/D bytes: " << count;
    clearRegisters();
    addRegisters(count, firstAddressRD, firstAddressWR);
  }
  // Update internal registers
  Q_ASSERT(pvRegister33.size() == pvRegister32.size());
  Q_ASSERT(pvRegister33.size() == pvRegister32.size());
  Q_ASSERT(pvRegister34.size() == pvRegister32.size());
  Q_ASSERT(pvRegister35.size() == pvRegister32.size());
  Q_ASSERT(pvRegister36.size() == pvRegister32.size());
  Q_ASSERT(pvRegister37.size() == pvRegister32.size());
  Q_ASSERT(pvRegister39.size() == pvRegister32.size());
  Q_ASSERT(pvRegister40.size() == pvRegister32.size());
  if(pvRegister32.size() != count){
    qDebug() << "* Have a new count of registers: " << count;
    pvRegister32.clear();
    pvRegister33.clear();
    pvRegister34.clear();
    pvRegister35.clear();
    pvRegister36.clear();
    pvRegister37.clear();
    pvRegister39.clear();
    pvRegister40.clear();
    for(i = 0; i < count; ++i){
      pvRegister32.append((quint16)0);
      pvRegister33.append((quint16)0);
      pvRegister34.append((quint16)0);
      pvRegister35.append((quint16)0);
      pvRegister36.append((quint16)0);
      pvRegister37.append((quint16)0);
      pvRegister39.append((quint16)0);
      pvRegister40.append((quint16)0);
    }
  }
}

bool mdtDeviceModbusWagoModuleRtd::setRegister(int registerNumber, int firstChannel, int lastChannel, bool needDeviceSetupState)
{
  Q_ASSERT(firstChannel >= 0);
  Q_ASSERT(lastChannel >= 0);
  Q_ASSERT(firstChannel <= lastChannel);

  quint8 scByte = 0;
  int channel;

  // Setup control byte for register request
  scByte |= (registerNumber & 0x3F);  // Register (B5..B0)
  scByte |= (1 << 6);                 // Write access (B6 to 1)
  scByte |= (1 << 7);                 // Reg_Comm set (B7 to 1)
  for(channel = firstChannel; channel <= lastChannel; ++channel){
    setControlByte(channel, scByte);
  }
  qDebug() << "setRegister() - control byte:";
  qDebug() << "-> Register number: " << (controlByte(0) & 0x3F);
  qDebug() << "-> R/W: " << ( ( controlByte(0) & (1 << 6) ) >> 6);
  qDebug() << "-> Reg_Comm: " << ( ( controlByte(0) & (1 << 7) ) >> 7 );
  // Write registers
  if(!writeRegisters(firstChannel, lastChannel, needDeviceSetupState)){
    return false;
  }

  return true;
}

bool mdtDeviceModbusWagoModuleRtd::getRegister(int registerNumber, int firstChannel, int lastChannel, bool needDeviceSetupState)
{
  Q_ASSERT(firstChannel >= 0);
  Q_ASSERT(lastChannel >= 0);
  Q_ASSERT(firstChannel <= lastChannel);

  quint8 scByte = 0;
  int channel;
  QString msg;

  // Setup control byte for register request
  scByte |= (registerNumber & 0x3F);  // Register (B5..B0)
  scByte |= (1 << 7);                 // Reg_Comm set (B7 to 1)
  for(channel = firstChannel; channel <= lastChannel; ++channel){
    
    qDebug() << "getRegister() - control byte C" << channel << ":";
    qDebug() << "-> Register number: " << (scByte & 0x3F);
    qDebug() << "-> R/W: " << ( ( scByte & (1 << 6) ) >> 6);
    qDebug() << "-> Reg_Comm: " << ( ( scByte & (1 << 7) ) >> 7);
    
    setControlByte(channel, scByte);
  }
  // Send query
  if(!writeControlBytes(firstChannel, lastChannel, needDeviceSetupState)){
    return false;
  }
  // Let the device do his job (if we don't wait, we will receive old values)
  wait(50);
  // Get registers from module
  if(!readRegisters(firstChannel, lastChannel, needDeviceSetupState)){
    return false;
  }
  // Check device returned status bytes
  for(channel = firstChannel; channel <= lastChannel; ++channel){
    scByte = statusByte(channel);
    
    qDebug() << "getRegister() - status byte:";
    qDebug() << "-> Register number: " << (scByte & 0x3F);
    qDebug() << "-> R/W: " << ( ( scByte & (1 << 6) ) >> 6);
    qDebug() << "-> Reg_Comm: " << ( ( scByte & (1 << 7) ) >> 7);
    
    // Check that register number returned from module is expected one
    if( (int)(scByte & 0x3F) != registerNumber ){
      msg = "Device: " + deviceName();
      msg += " : status byte contains not the requested register number.";
      msg += " (requested: " + QString::number(registerNumber) + " , returned: " + QString::number(scByte & 0x3F) + ")";
      mdtError e(MDT_DEVICE_ERROR,  msg , mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModuleRtd");
      e.commit();
      return false;
    }
    // Check that module acknowleged read access
    if( scByte & (1 << 6) ){
      mdtError e(MDT_DEVICE_ERROR, "Device: " + deviceName() + " : module did not acknowledge read access (bit 6 of status byte set)", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModuleRtd");
      e.commit();
      return false;
    }
    // Check that Reg_Comm bit is set (coherence test)
    if( !(scByte & (1 << 7)) ){
      mdtError e(MDT_DEVICE_ERROR, "Device: " + deviceName() + " : bit Reg_Comm (B7) not set in status byte - incoherent", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModuleRtd");
      e.commit();
      return false;
    }
  }

  return true;
}

bool mdtDeviceModbusWagoModuleRtd::setRegister47()
{
  Q_ASSERT(ioCountRegisters() > 0);

  setRegisterDataWord(0, pvRegister47);
  if(!setRegister(47, 0, 0, true)){
    mdtError e(MDT_DEVICE_ERROR, "Device: " + deviceName() + " : set register 47 to module failed", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModuleRtd");
    e.commit();
    return false;
  }

  return true;
}

bool mdtDeviceModbusWagoModuleRtd::getRegister47()
{
  Q_ASSERT(ioCountRegisters() > 0);

  if(!getRegister(47, 0, 0, true)){
    mdtError e(MDT_DEVICE_ERROR, "Device: " + deviceName() + " : get register 47 from module failed", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModuleRtd");
    e.commit();
    return false;
  }
  pvRegister47 = registerDataWord(0);
  updateRegisterCount();

  return true;
}

bool mdtDeviceModbusWagoModuleRtd::setRegister32()
{
  Q_ASSERT(ioCountRegisters() > 0);

  int chCount = ioCountRegisters();
  int channel;

  for(channel = 0; channel < chCount; ++channel){
    setRegisterDataWord(channel, pvRegister32.at(channel));
  }
  if(!setRegister(32, 0, chCount-1, true)){
    mdtError e(MDT_DEVICE_ERROR, "Device: " + deviceName() + " : set register 32 to module failed", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModuleRtd");
    e.commit();
    return false;
  }

  return true;
}

bool mdtDeviceModbusWagoModuleRtd::getRegister32()
{
  Q_ASSERT(ioCountRegisters() > 0);

  int chCount = ioCountRegisters();
  int channel;

  ///if(!getRegister(32, 0, chCount-1, true)){
  if(!getRegister(32, 1, 1, true)){
    mdtError e(MDT_DEVICE_ERROR, "Device: " + deviceName() + " : get register 32 from module failed", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModuleRtd");
    e.commit();
    return false;
  }
  for(channel = 0; channel < chCount; ++channel){
    pvRegister32[channel] = registerDataWord(channel);
  }

  return true;
}
