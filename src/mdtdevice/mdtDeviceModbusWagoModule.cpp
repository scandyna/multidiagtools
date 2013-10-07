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
#include "mdtDeviceModbusWagoModule.h"
#include "mdtDeviceModbusWago.h"
#include "mdtPortManager.h"
#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"
#include "mdtError.h"

#include <QDebug>

mdtDeviceModbusWagoModule::mdtDeviceModbusWagoModule(bool autoDeleteIos, mdtDeviceModbusWago *device)
{
  Q_ASSERT(device != 0);

  pvAutoDeleteIos = autoDeleteIos;
  pvDevice = device;
  pvType = Unknown;
  pvPartNumber = 0;
}

mdtDeviceModbusWagoModule::~mdtDeviceModbusWagoModule()
{
  clear();
}

void mdtDeviceModbusWagoModule::clear(bool forceDeleteIos)
{
  pvType = Unknown;
  pvPartNumber = 0;
  if((pvAutoDeleteIos)||(forceDeleteIos)){
    qDeleteAll(pvAnalogIos);
    qDeleteAll(pvDigitalIos);
  }
  pvAnalogIos.clear();
  pvDigitalIos.clear();
  clearRegisters();
}

bool mdtDeviceModbusWagoModule::setupFromRegisterWord(quint16 word)
{
  bool ok;

  clear();
  // Check module type (analog or digital)
  if(word & 0x8000){
    ///qDebug() << "Module[" << i << "]: digital I/O";
    ok = addDigitalIos(word);
  }else{
    ///qDebug() << "Module[" << i << "]: analog I/O";
    ok = addAnalogIos(word);
  }
  if(!ok){
    return false;
  }
  // Check about Control/Status bytes
  /// \todo Implement

  return true;
}

void mdtDeviceModbusWagoModule::setFirstAddress(int addressRead, int addressWrite)
{
  Q_ASSERT(addressRead >= 0);

  mdtAnalogIo *aio;
  mdtDigitalIo *dio;
  int i;
  int currentAddressRead = addressRead;
  int currentAddressWrite = addressWrite;

  /*
   * Address I/O's and status/control register.
   * We assume that status/control byte address is allway before I/O address
   *  (this is the case for RTD module 750-464).
   * MODBUS read/write register functions allways works in words (2 bytes) for data part.
   *  Status/control byte is the low part of a word
   *  (described in "Speciality modules" chapiter of 750-352 coupler manual).
   *  -> status/control byte take 1 MODBUS address.
   */
  for(i = 0; i < ioCount(); ++i){
    // Setup control/status byte address if exists
    if(pvControlBytes.size() > i){
      Q_ASSERT(addressWrite >= 0);
      pvControlBytes[i].first = currentAddressWrite;
      ++currentAddressWrite;
    }
    if(pvStatusBytes.size() > i){
      pvStatusBytes[i].first = currentAddressRead;
      ++currentAddressRead;
    }
    // Setup I/O
    if(pvAnalogIos.size() > i){
      aio = pvAnalogIos.at(i);
      Q_ASSERT(aio != 0);
      aio->setAddress(currentAddressRead);
      ++currentAddressRead;
      if(pvType == AnalogOutputs){
        Q_ASSERT(addressWrite >= 0);
        aio->setAddressWrite(addressWrite);
        ++addressWrite;
      }
    }
    if(pvDigitalIos.size() > i){
      dio = pvDigitalIos.at(i);
      Q_ASSERT(dio != 0);
      dio->setAddress(currentAddressRead);
      ++currentAddressRead;
      if(pvType == DigitalOutputs){
        Q_ASSERT(addressWrite >= 0);
        dio->setAddressWrite(currentAddressWrite);
        ++currentAddressWrite;
      }
    }
  }
}

bool mdtDeviceModbusWagoModule::getSpecialModuleSetup(quint16 word, int firstAddressRead, int firstAddressWrite)
{
  return false;
}

int mdtDeviceModbusWagoModule::firstAddressRead() const
{
  int firstAddress = -1;

  // Check in I/O's
  if(pvAnalogIos.size() > 0){
    Q_ASSERT(pvAnalogIos.at(0) != 0);
    firstAddress = pvAnalogIos.at(0)->addressRead();
  }
  if(pvDigitalIos.size() > 0){
    Q_ASSERT(pvDigitalIos.at(0) != 0);
    if(firstAddress >= 0){
      firstAddress = qMin(firstAddress, pvDigitalIos.at(0)->addressRead());
    }else{
      firstAddress = pvDigitalIos.at(0)->addressRead();
    }
  }
  // Check status byte (register data bytes overlaps I/Os)
  if(pvStatusBytes.size() > 0){
    if(firstAddress >= 0){
      firstAddress = qMin(firstAddress, pvStatusBytes.at(0).first);
    }else{
      firstAddress = pvStatusBytes.at(0).first;
    }
  }

  return firstAddress;
}

int mdtDeviceModbusWagoModule::firstAddressWrite() const
{
  int firstAddress = -1;

  // Check in I/O's
  if(pvAnalogIos.size() > 0){
    Q_ASSERT(pvAnalogIos.at(0) != 0);
    firstAddress = pvAnalogIos.at(0)->addressWrite();
  }
  if(pvDigitalIos.size() > 0){
    Q_ASSERT(pvDigitalIos.at(0) != 0);
    if(firstAddress >= 0){
      firstAddress = qMin(firstAddress, pvDigitalIos.at(0)->addressWrite());
    }else{
      firstAddress = pvDigitalIos.at(0)->addressWrite();
    }
  }
  // Check control byte (register data bytes overlaps I/Os)
  if(pvControlBytes.size() > 0){
    if(firstAddress >= 0){
      firstAddress = qMin(firstAddress, pvControlBytes.at(0).first);
    }else{
      firstAddress = pvControlBytes.at(0).first;
    }
  }

  return firstAddress;
}

int mdtDeviceModbusWagoModule::lastAddressRead() const
{
  int lastAddress = -1;
  int lastIndex;

  // Check in I/O's
  lastIndex = pvAnalogIos.size() - 1;
  if(lastIndex >= 0){
    Q_ASSERT(pvAnalogIos.at(lastIndex) != 0);
    lastAddress = pvAnalogIos.at(lastIndex)->addressRead();
  }
  lastIndex = pvDigitalIos.size() - 1;
  if(lastIndex >= 0){
    Q_ASSERT(pvDigitalIos.at(lastIndex) != 0);
    lastAddress = qMax(lastAddress, pvDigitalIos.at(lastIndex)->addressRead());
  }
  // Check status byte (register data bytes overlaps I/Os)
  lastIndex = pvStatusBytes.size() - 1;
  if(lastIndex >= 0){
    lastAddress = qMax(lastAddress, pvStatusBytes.at(lastIndex).first);
  }

  return lastAddress;
}

int mdtDeviceModbusWagoModule::lastAddressWrite() const
{
  int lastAddress = -1;
  int lastIndex;

  // Check in I/O's
  lastIndex = pvAnalogIos.size() - 1;
  if(lastIndex >= 0){
    Q_ASSERT(pvAnalogIos.at(lastIndex) != 0);
    lastAddress = pvAnalogIos.at(lastIndex)->addressWrite();
  }
  lastIndex = pvDigitalIos.size() - 1;
  if(lastIndex >= 0){
    Q_ASSERT(pvDigitalIos.at(lastIndex) != 0);
    lastAddress = qMax(lastAddress, pvDigitalIos.at(lastIndex)->addressWrite());
  }
  // Check control byte (register data bytes overlaps I/Os)
  lastIndex = pvControlBytes.size() - 1;
  if(lastIndex >= 0){
    lastAddress = qMax(lastAddress, pvControlBytes.at(lastIndex).first);
  }

  return lastAddress;
}

int mdtDeviceModbusWagoModule::ioCount() const
{
  return (pvAnalogIos.size() + pvDigitalIos.size());
}

const QList<mdtAnalogIo*> & mdtDeviceModbusWagoModule::analogIos()
{
  return pvAnalogIos;
}

const QList<mdtDigitalIo*> & mdtDeviceModbusWagoModule::digitalIos()
{
  return pvDigitalIos;
}

mdtDeviceModbusWagoModule::type_t mdtDeviceModbusWagoModule::type() const
{
  return pvType;
}

/**
QString mdtDeviceModbusWagoModule::typeText() const {
}
*/

int mdtDeviceModbusWagoModule::partNumber() const
{
  return pvPartNumber;
}

QString mdtDeviceModbusWagoModule::partNumberText() const
{
  if(pvPartNumber <= 0){
    return QString();
  }
  return QString::number(pvPartNumber);
}

QString mdtDeviceModbusWagoModule::deviceName()
{
  Q_ASSERT(pvDevice != 0);

  return pvDevice->name();
}

void mdtDeviceModbusWagoModule::addAnalogIo(mdtAnalogIo *aio)
{
  Q_ASSERT(aio != 0);

  pvAnalogIos.append(aio);
}

void mdtDeviceModbusWagoModule::addDigitalIo(mdtDigitalIo *dio)
{
  Q_ASSERT(dio != 0);

  pvDigitalIos.append(dio);
}

void mdtDeviceModbusWagoModule::addRegisters(int channelCount, int firstAddressRead, int firstAddressWrite)
{
  int i;
  int currentAddressRead, currentAddressWrite;

  currentAddressRead = firstAddressRead;
  currentAddressWrite = firstAddressWrite;
  for(i = 0; i < channelCount; ++i){
    pvControlBytes.append(QPair<int, quint8>(currentAddressWrite, 0));
    pvStatusBytes.append(QPair<int, quint8>(currentAddressRead, 0));
    currentAddressWrite += 2;
    currentAddressRead += 2;
    ///pvRegisterDataWords.append(QPair<int, quint16>(currentAddressRead, 0)); /// \todo Gag avec adresses !!
    pvRegisterDataWords.append((quint16)0);
    ///++currentAddressRead;
  }
}

void mdtDeviceModbusWagoModule::clearRegisters()
{
  pvControlBytes.clear();
  pvStatusBytes.clear();
  pvRegisterDataWords.clear();
}

int mdtDeviceModbusWagoModule::channelCountRegisters() const
{
  Q_ASSERT(pvStatusBytes.size() == pvControlBytes.size());
  Q_ASSERT(pvRegisterDataWords.size() == pvControlBytes.size());

  return pvControlBytes.size();
}

quint8 mdtDeviceModbusWagoModule::statusByte(int channel) const
{
  Q_ASSERT((channel >= 0)&&(channel < pvStatusBytes.size()));

  return pvStatusBytes.at(channel).second;
}

void mdtDeviceModbusWagoModule::setControlByte(int channel, quint8 byte)
{
  Q_ASSERT((channel >= 0)&&(channel < pvControlBytes.size()));

  qDebug() << "setControlByte() - byte [hex]: " << hex << byte;
  pvControlBytes[channel].second = byte;
  qDebug() << "setControlByte() - pvControlBytes [hex]: " << hex << pvControlBytes;
}

quint8 mdtDeviceModbusWagoModule::controlByte(int channel) const
{
  Q_ASSERT((channel >= 0)&&(channel < pvControlBytes.size()));
 
  return pvControlBytes.at(channel).second;
}

void mdtDeviceModbusWagoModule::setRegisterDataWord(int channel, quint16 word)
{
  Q_ASSERT((channel >= 0)&&(channel < pvRegisterDataWords.size()));

  ///pvRegisterDataWords[channel].second = word;
  pvRegisterDataWords[channel] = word;
}

quint16 mdtDeviceModbusWagoModule::registerDataWord(int channel) const
{
  Q_ASSERT((channel >= 0)&&(channel < pvRegisterDataWords.size()));

  ///return pvRegisterDataWords.at(channel).second;
  return pvRegisterDataWords.at(channel);
}

void mdtDeviceModbusWagoModule::setRegisterDataBytes(int channel, quint8 byteD1, quint8 byteD0)
{
  Q_ASSERT((channel >= 0)&&(channel < pvRegisterDataWords.size()));

  quint16 word = (byteD0 | (byteD1 << 8));
  setRegisterDataWord(channel, word);
}

quint8 mdtDeviceModbusWagoModule::registerDataByteD1(int channel)
{
  Q_ASSERT((channel >= 0)&&(channel < pvRegisterDataWords.size()));

  ///return (pvRegisterDataWords.at(channel).second >> 8);
  return (pvRegisterDataWords.at(channel) >> 8);
}

quint8 mdtDeviceModbusWagoModule::registerDataByteD0(int channel)
{
  Q_ASSERT((channel >= 0)&&(channel < pvRegisterDataWords.size()));

  ///return (pvRegisterDataWords.at(channel).second & 0x00FF);
  return (pvRegisterDataWords.at(channel) & 0x00FF);
}

/// \todo Implement needDeviceSetupState
bool mdtDeviceModbusWagoModule::readRegisters(int firstChannel, int lastChannel, bool needDeviceSetupState)
{
  Q_ASSERT(pvDevice != 0);
  Q_ASSERT(firstChannel >= 0);
  Q_ASSERT(lastChannel >= 0);
  Q_ASSERT(lastChannel < pvStatusBytes.size());
  Q_ASSERT(lastChannel < pvRegisterDataWords.size());
  Q_ASSERT(firstChannel <= lastChannel);

  int startAddress;
  int registersCount;
  int channel, i;

  /**
  if(needDeviceSetupState && (pvDevice->currentState() != mdtPortManager::Setup)){
  }
  */
  // Read registers from device
  startAddress = pvStatusBytes.at(firstChannel).first;
  registersCount = pvStatusBytes.at(lastChannel).first + 1 - startAddress + 1;
  ///registersCount = pvRegisterDataWords.at(lastChannel).first - startAddress + 1;  /// \todo Change to status byte of last channel + 1......
  qDebug() << "Module - readRegisters(): startAddress: " << startAddress << " , qty: " << registersCount;
  if(!pvDevice->getRegisterValues(startAddress, registersCount)){
    mdtError e(MDT_DEVICE_ERROR, "Device: " + pvDevice->name() + " : unable to get register values from device", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return false;
  }
  qDebug() << "Module - readRegisters(): getRegisterValues() done - values [hex]: " << hex << pvDevice->registerValues();
  if(pvDevice->registerValues().size() != registersCount){
    mdtError e(MDT_DEVICE_ERROR, "Device: " + pvDevice->name() + " : received unpexpected count of register values from device", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return false;
  }
  // Update cache
  channel = firstChannel;
  for(i = 0; i < pvDevice->registerValues().size(); i += 2){
    Q_ASSERT((i + 1) < pvDevice->registerValues().size());
    Q_ASSERT(channel < pvStatusBytes.size());
    Q_ASSERT(channel < pvRegisterDataWords.size());
    pvStatusBytes[channel].second = pvDevice->registerValues().at(i) & 0x00FF;
    ///pvRegisterDataWords[channel].second = pvDevice->registerValues().at(i + 1);
    pvRegisterDataWords[channel] = pvDevice->registerValues().at(i + 1);
    ++channel;
  }
  /**
  Q_ASSERT(pvDevice->registerValues().size() == (lastChannel + 1));
  for(channel = firstChannel; channel <= lastChannel; channel += 2){
    pvStatusBytes[channel].second = pvDevice->registerValues().at(channel) & 0x00FF;
    pvRegisterDataWords[channel].second = pvDevice->registerValues().at(channel + 1);
  }
  */
  qDebug() << "Module - readRegisters(): status bytes [hex] : " << hex << pvStatusBytes;
  qDebug() << "Module - readRegisters(): register data [hex]: " << hex << pvRegisterDataWords;

  return true;
}

/// \todo Implement needDeviceSetupState
bool mdtDeviceModbusWagoModule::writeRegisters(int firstChannel, int lastChannel, bool needDeviceSetupState)
{
  Q_ASSERT(pvDevice != 0);
  Q_ASSERT(firstChannel >= 0);
  Q_ASSERT(lastChannel >= 0);
  Q_ASSERT(lastChannel < pvControlBytes.size());
  Q_ASSERT(lastChannel < pvRegisterDataWords.size());
  Q_ASSERT(firstChannel <= lastChannel);
  Q_ASSERT(pvRegisterDataWords.size() == pvControlBytes.size());

  int startAddress;
  int i;
  QList<int> values;

  /**
  if(needDeviceSetupState && (pvDevice->currentState() != mdtPortManager::Setup)){
  }
  */
  // Get startAddress and registers count
  startAddress = pvControlBytes.at(0).first;
  // Build values list
  for(i = 0; i < pvControlBytes.size(); ++i){
    values.append(pvControlBytes.at(i).second);
    ///values.append(pvRegisterDataWords.at(i).second);
    values.append(pvRegisterDataWords.at(i));
  }
  qDebug() << "Module - writeRegisters(): startAddress: " << startAddress << " , qty: " << values.size();
  qDebug() << "Module - writeRegisters(): values [hex]: " << hex << values;
  // Write registers to device
  if(!pvDevice->setRegisterValues(startAddress, values)){
    mdtError e(MDT_DEVICE_ERROR, "Device: " + pvDevice->name() + " : unable to set register values to device", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return false;
  }

  return true;
}

/// \todo Implement needDeviceSetupState
bool mdtDeviceModbusWagoModule::readStatusBytes(int firstChannel, int lastChannel, bool needDeviceSetupState)
{
  Q_ASSERT(pvDevice != 0);
  Q_ASSERT(firstChannel >= 0);
  Q_ASSERT(lastChannel >= 0);
  Q_ASSERT(lastChannel < pvStatusBytes.size());
  ///Q_ASSERT(lastChannel < pvRegisterDataWords.size());
  Q_ASSERT(firstChannel <= lastChannel);

  int startAddress;
  int registersCount;
  int channel;

  /**
  if(needDeviceSetupState && (pvDevice->currentState() != mdtPortManager::Setup)){
  }
  */
  // Read registers from device
  startAddress = pvStatusBytes.at(0).first;
  registersCount = pvStatusBytes.at(lastChannel).first - startAddress + 1;
  qDebug() << "Module - readStatusBytes(): startAddress: " << startAddress << " , qty: " << registersCount;
  if(!pvDevice->getRegisterValues(startAddress, registersCount)){
    mdtError e(MDT_DEVICE_ERROR, "Device: " + pvDevice->name() + " : unable to get register values from device", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return false;
  }
  if(pvDevice->registerValues().size() != registersCount){
    mdtError e(MDT_DEVICE_ERROR, "Device: " + pvDevice->name() + " : received unpexpected count of register values from device", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return false;
  }
  // Update cache
  Q_ASSERT(pvDevice->registerValues().size() == (lastChannel + 1));
  for(channel = firstChannel; channel <= lastChannel; channel += 2){
    pvStatusBytes[channel].second = pvDevice->registerValues().at(channel) & 0x00FF;
  }
  qDebug() << "Module - readStatusBytes(): status bytes : " << pvStatusBytes;

  return true;
}

/// \todo Implement needDeviceSetupState
bool mdtDeviceModbusWagoModule::writeControlBytes(int firstChannel, int lastChannel, bool needDeviceSetupState)
{
  Q_ASSERT(pvDevice != 0);
  Q_ASSERT(firstChannel >= 0);
  Q_ASSERT(lastChannel >= 0);
  Q_ASSERT(lastChannel < pvControlBytes.size());
  ///Q_ASSERT(lastChannel < pvRegisterDataWords.size());
  Q_ASSERT(firstChannel <= lastChannel);

  int startAddress;
  int channel;
  QList<int> values;

  /**
  if(needDeviceSetupState && (pvDevice->currentState() != mdtPortManager::Setup)){
  }
  */
  // Write each control byte to device
  for(channel = firstChannel; channel <= lastChannel; ++channel){
    values.append(pvControlBytes.at(channel).second);
    startAddress = pvControlBytes.at(channel).first;
    qDebug() << "writeControlBytes() : startAddress: " << startAddress << " , values [hex]: " << hex << values;
    if(!pvDevice->setRegisterValues(startAddress, values)){
      mdtError e(MDT_DEVICE_ERROR, "Device: " + pvDevice->name() + " : unable to set register values to device", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
      e.commit();
      return false;
    }
    values.clear();
  }

  return true;
}

void mdtDeviceModbusWagoModule::wait(int ms)
{
  Q_ASSERT(pvDevice != 0);

  pvDevice->wait(ms);
}

QVariant mdtDeviceModbusWagoModule::specialModuleValueMin()
{
  return QVariant();
}

QVariant mdtDeviceModbusWagoModule::specialModuleValueMax()
{
  return QVariant();
}

QVariant mdtDeviceModbusWagoModule::specialModuleValueBitsCount()
{
  return QVariant();
}

QVariant mdtDeviceModbusWagoModule::specialModuleValueLsbIndex()
{
  return QVariant();
}

QVariant mdtDeviceModbusWagoModule::specialModuleValueSigned()
{
  return QVariant();
}

QVariant mdtDeviceModbusWagoModule::specialModuleIsInput()
{
  return QVariant();
}

QVariant mdtDeviceModbusWagoModule::specialModuleIosCount()
{
  return QVariant();
}

/// \todo adapt
QVariant mdtDeviceModbusWagoModule::analogIoModuleValueMin(int partNumber) const
{
  switch(partNumber){
    case 452:
      return 0.0;
    case 454:
      return 4.0;
    case 457:
      return QVariant(-10.0);
    case 550:
      return QVariant(0.0);
    default:
      return QVariant();
  }
}

QVariant mdtDeviceModbusWagoModule::analogIoModuleValueMax(int partNumber) const
{
  switch(partNumber){
    case 452:
      return 20.0;
    case 454:
      return 20.0;
    case 457:
      return QVariant(10.0);
    case 550:
      return QVariant(10.0);
    default:
      return QVariant();
  }
}

int mdtDeviceModbusWagoModule::analogIoModuleValueBitsCount(int partNumber) const
{
  switch(partNumber){
    case 452:
      return 12;
    case 454:
      return 12;
    case 457:
      return 13;
    case 550:
      return 12;
    default:
      return -1;
  }
}

int mdtDeviceModbusWagoModule::analogIoModuleValueLsbIndex(int partNumber) const
{
  switch(partNumber){
    case 452:
      return 3;
    case 454:
      return 3;
    case 457:
      return 3;
    case 550:
      return 3;
    default:
      return -1;
  }
}

QVariant mdtDeviceModbusWagoModule::analogIoModuleValueSigned(int partNumber) const
{
  switch(partNumber){
    case 452:
      return false;
    case 454:
      return false;
    case 457:
      return true;
    case 550:
      return false;
    default:
      return QVariant();
  }
}

QVariant mdtDeviceModbusWagoModule::analogIoModuleIsInput(int partNumber) const
{
  switch(partNumber){
    case 452:
      return true;
    case 454:
      return true;
    case 457:
      return true;
    case 550:
      return false;
    default:
      return QVariant();
  }
}

int mdtDeviceModbusWagoModule::analogIoModuleIosCount(int partNumber) const
{
  switch(partNumber){
    case 452:
      return 2;
    case 454:
      return 2;
    case 457:
      return 4;
    case 550:
      return 2;
    default:
      return -1;
  }
}

QString mdtDeviceModbusWagoModule::analogIoModuleUnit(int partNumber) const
{
  switch(partNumber){
    case 452:
      return "mA";
    case 454:
      return "mA";
    case 457:
      return "V";
    case 550:
      return "V";
    default:
      return "";
  }
}

QVariant mdtDeviceModbusWagoModule::digitalIoModuleIsInput(quint16 word) const
{
  // Bit 0 is set for a input module
  if(word & 0x0001){
    // Should be a input, check bit 1
    if(word & 0x0002){
      return QVariant();
    }else{
      return true;
    }
  }else{
    // Should be a output module, check bit 1
    if(word & 0x0002){
      return false;
    }else{
      return QVariant();
    }
  }
}

int mdtDeviceModbusWagoModule::digitalIoModuleIosCount(quint16 word) const
{
  return ((word & 0x0F00) >> 8);
}

mdtAnalogIo *mdtDeviceModbusWagoModule::getNewAnalogIo(int partNumber) const
{
  mdtAnalogIo *aio;
  QVariant value;
  double min, max;
  int intValueBitsCount, intValueLsbIndex;
  bool intValueSigned;

  // Get I/O module's parameters
  value = analogIoModuleValueMin(partNumber);
  if(!value.isValid()){
    return 0;
  }
  min = value.toDouble();
  value = analogIoModuleValueMax(partNumber);
  if(!value.isValid()){
    return 0;
  }
  max = value.toDouble();
  intValueBitsCount = analogIoModuleValueBitsCount(partNumber);
  if(intValueBitsCount < 0){
    return 0;
  }
  intValueLsbIndex = analogIoModuleValueLsbIndex(partNumber);
  if(intValueLsbIndex < 0){
    return 0;
  }
  value = analogIoModuleValueSigned(partNumber);
  if(!value.isValid()){
    return 0;
  }
  intValueSigned = value.toBool();
  // Have all needed parameters, build I/O
  aio = new mdtAnalogIo;
  if(aio == 0){
    return 0;
  }
  aio->setRange(min, max, intValueBitsCount, intValueLsbIndex, intValueSigned);
  aio->setUnit("[" + analogIoModuleUnit(partNumber) + "]");
  aio->setLabel("Module: 750-" + QString::number(partNumber));

  return aio;
}

bool mdtDeviceModbusWagoModule::addAnalogIos(int partNumber)
{
  int iosCount;
  int i;
  bool isInput;
  QVariant var;
  mdtAnalogIo *aio;

  // Store part number
  pvPartNumber = partNumber;
  // check if module is input or output
  var = analogIoModuleIsInput(pvPartNumber);
  if(!var.isValid()){
    mdtError e(MDT_DEVICE_ERROR, "Cannot check if I/O module has inputs or a outputs (unknown part number: " + partNumberText() + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return false;
  }
  isInput = var.toBool();
  // Get I/Os count
  iosCount = analogIoModuleIosCount(pvPartNumber);
  if(iosCount < 0){
    mdtError e(MDT_DEVICE_ERROR, "Cannot get module's number of I/Os (unknown part number: " + partNumberText() + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return false;
  }
  // Add I/Os
  for(i = 0; i < iosCount; ++i){
    aio = getNewAnalogIo(pvPartNumber);
    if(aio == 0){
      mdtError e(MDT_DEVICE_ERROR, "Cannot get I/O module's parameters (unknown part number: " + partNumberText() + ")", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
      e.commit();
      return false;
    }
    pvAnalogIos.append(aio);
    if(isInput){
      pvType = AnalogInputs;
    }else{
      pvType = AnalogOutputs;
    }
  }

  return true;
}

bool mdtDeviceModbusWagoModule::addDigitalIos(quint16 word)
{
  int iosCount;
  int i;
  bool isInput;
  QVariant var;
  mdtDigitalIo *dio;

  // Check if module is input or output
  var = digitalIoModuleIsInput(word);
  if(!var.isValid()){
    mdtError e(MDT_DEVICE_ERROR, "Cannot check if I/O module has inputs or a outputs", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return false;
  }
  isInput = var.toBool();
  // Get I/Os count
  iosCount = digitalIoModuleIosCount(word);
  if(iosCount < 0){
    mdtError e(MDT_DEVICE_ERROR, "Cannot get module's number of I/Os", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return false;
  }
  // Add I/Os
  for(i=0; i<iosCount; i++){
    dio = new mdtDigitalIo;
    if(dio == 0){
      mdtError e(MDT_DEVICE_ERROR, "Unable to build a digital I/O (memory full ?)", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
      e.commit();
      return false;
    }
    pvDigitalIos.append(dio);
    if(isInput){
      pvType = DigitalInputs;
    }else{
      pvType = DigitalOutputs;
    }
  }

  return true;
}
