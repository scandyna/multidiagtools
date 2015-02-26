/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
  pvFirstAiAddress = 0;
  pvFirstAoAddressRead = 0;
  pvFirstAoAddressWrite = 0;
  pvFirstDiAddress = 0;
  pvFirstDoAddressRead = 0;
  pvFirstDoAddressWrite = 0;
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
    qDeleteAll(pvAnalogInputs);
    qDeleteAll(pvAnalogOutputs);
    qDeleteAll(pvDigitalInputs);
    qDeleteAll(pvDigitalOutputs);
  }
  pvAnalogInputs.clear();
  pvAnalogOutputs.clear();
  pvDigitalInputs.clear();
  pvDigitalOutputs.clear();
  clearRegisters();
  pvFirstAiAddress = 0;
  pvFirstAoAddressRead = 0;
  pvFirstAoAddressWrite = 0;
  pvFirstDiAddress = 0;
  pvFirstDoAddressRead = 0;
  pvFirstDoAddressWrite = 0;
}

bool mdtDeviceModbusWagoModule::setupFromRegisterWord(quint16 word)
{
  bool ok;

  clear();
  // Check module type (analog or digital)
  if(word & 0x8000){
    ok = addDigitalIos(word);
  }else{
    ok = addAnalogIos(word);
  }
  if(!ok){
    return false;
  }
  // Check about Control/Status bytes
  /// \todo Implement

  return true;
}

void mdtDeviceModbusWagoModule::updateAddresses(int firstAiAddress, int firstAoAddressRead, int firstAoAddressWrite, int firstDiAddress, int firstDoAddressRead, int firstDoAddressWrite)
{
  Q_ASSERT(firstAiAddress >= 0);
  Q_ASSERT(firstAoAddressRead >= 0);
  Q_ASSERT(firstAoAddressWrite >= 0);
  Q_ASSERT(firstDiAddress >= 0);
  Q_ASSERT(firstDoAddressRead >= 0);
  Q_ASSERT(firstDoAddressWrite >= 0);

  int i;
  int currentAddress, currentAddressRead, currentAddressWrite;

  // Store first addresses
  pvFirstAiAddress = firstAiAddress;
  pvFirstAoAddressRead = firstAoAddressRead;
  pvFirstAoAddressWrite = firstAoAddressWrite;
  pvFirstDiAddress = firstDiAddress;
  pvFirstDoAddressRead = firstDoAddressRead;
  pvFirstDoAddressWrite = firstDoAddressWrite;
  // Address analog inputs
  currentAddress = pvFirstAiAddress;
  for(i = 0; i < pvAnalogInputs.size(); ++i){
    Q_ASSERT(pvAnalogInputs.at(i) != 0);
    pvAnalogInputs.at(i)->setAddress(currentAddress);
    ++currentAddress;
  }
  // Address analog outputs
  currentAddressRead = pvFirstAoAddressRead;
  currentAddressWrite = pvFirstAoAddressWrite;
  for(i = 0; i < pvAnalogOutputs.size(); ++i){
    Q_ASSERT(pvAnalogOutputs.at(i) != 0);
    pvAnalogOutputs.at(i)->setAddressRead(currentAddressRead);
    pvAnalogOutputs.at(i)->setAddressWrite(currentAddressWrite);
    ++currentAddressRead;
    ++currentAddressWrite;
  }
  // Address digital inputs
  currentAddress = pvFirstDiAddress;
  for(i = 0; i < pvDigitalInputs.size(); ++i){
    Q_ASSERT(pvDigitalInputs.at(i) != 0);
    pvDigitalInputs.at(i)->setAddress(currentAddress);
    ++currentAddress;
  }
  // Address digital outputs
  currentAddressRead = pvFirstDoAddressRead;
  currentAddressWrite = pvFirstDoAddressWrite;
  for(i = 0; i < pvDigitalOutputs.size(); ++i){
    Q_ASSERT(pvDigitalOutputs.at(i) != 0);
    pvDigitalOutputs.at(i)->setAddressRead(currentAddressRead);
    pvDigitalOutputs.at(i)->setAddressWrite(currentAddressWrite);
    ++currentAddressRead;
    ++currentAddressWrite;
  }
}

int mdtDeviceModbusWagoModule::firstAiAddress() const
{
  return pvFirstAiAddress;
}

int mdtDeviceModbusWagoModule::lastAiAddress() const
{
  if(pvAnalogInputs.isEmpty()){
    return pvFirstAiAddress;
  }else{
    return pvFirstAiAddress + pvAnalogInputs.size() - 1;
  }
}

int mdtDeviceModbusWagoModule::nextModuleFirstAiAddress() const
{
  return pvFirstAiAddress + pvAnalogInputs.size();
}

int mdtDeviceModbusWagoModule::firstAoAddressRead() const
{
  return pvFirstAoAddressRead;
}

int mdtDeviceModbusWagoModule::lastAoAddressRead() const
{
  if(pvAnalogOutputs.isEmpty()){
    return pvFirstAoAddressRead;
  }else{
    return pvFirstAoAddressRead + pvAnalogOutputs.size() - 1;
  }
}

int mdtDeviceModbusWagoModule::nextModuleFirstAoAddressRead() const
{
  return pvFirstAoAddressRead + pvAnalogOutputs.size();
}

int mdtDeviceModbusWagoModule::firstAoAddressWrite() const
{
  return pvFirstAoAddressWrite;
}

int mdtDeviceModbusWagoModule::lastAoAddressWrite() const
{
  if(pvAnalogOutputs.isEmpty()){
    return pvFirstAoAddressWrite;
  }else{
    return pvFirstAoAddressWrite + pvAnalogOutputs.size() - 1;
  }
}

int mdtDeviceModbusWagoModule::nextModuleFirstAoAddressWrite() const
{
  return pvFirstAoAddressWrite + pvAnalogOutputs.size();
}

int mdtDeviceModbusWagoModule::firstDiAddress() const
{
  return pvFirstDiAddress;
}

int mdtDeviceModbusWagoModule::lastDiAddress() const
{
  if(pvDigitalInputs.isEmpty()){
    return pvFirstDiAddress;
  }else{
    return pvFirstDiAddress + pvDigitalInputs.size() - 1;
  }
}

int mdtDeviceModbusWagoModule::nextModuleFirstDiAddress() const
{
  return pvFirstDiAddress + pvDigitalInputs.size();
}

int mdtDeviceModbusWagoModule::firstDoAddressRead() const
{
  return pvFirstDoAddressRead;
}

int mdtDeviceModbusWagoModule::lastDoAddressRead() const
{
  if(pvDigitalOutputs.isEmpty()){
    return pvFirstDoAddressRead;
  }else{
    return pvFirstDoAddressRead + pvDigitalOutputs.size() - 1;
  }
}

int mdtDeviceModbusWagoModule::nextModuleFirstDoAddressRead() const
{
  return pvFirstDoAddressRead + pvDigitalOutputs.size();
}

int mdtDeviceModbusWagoModule::firstDoAddressWrite() const
{
  return pvFirstDoAddressWrite;
}

int mdtDeviceModbusWagoModule::lastDoAddressWrite() const
{
  if(pvDigitalOutputs.isEmpty()){
    return pvFirstDoAddressWrite;
  }else{
    return pvFirstDoAddressWrite + pvDigitalOutputs.size() - 1;
  }
}

int mdtDeviceModbusWagoModule::nextModuleFirstDoAddressWrite() const
{
  return pvFirstDoAddressWrite + pvDigitalOutputs.size();
}

const QList<mdtAnalogIo*> & mdtDeviceModbusWagoModule::analogInputs()
{
  return pvAnalogInputs;
}

const QList<mdtAnalogIo*> & mdtDeviceModbusWagoModule::analogOutputs()
{
  return pvAnalogOutputs;
}

const QList<mdtDigitalIo*> & mdtDeviceModbusWagoModule::digitalInputs()
{
  return pvDigitalInputs;
}

const QList<mdtDigitalIo*> & mdtDeviceModbusWagoModule::digitalOutputs()
{
  return pvDigitalOutputs;
}

mdtDeviceModbusWagoModule::type_t mdtDeviceModbusWagoModule::type() const
{
  return pvType;
}

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

  ///return pvDevice->name();
  return "mdtDeviceModbusWagoModule::deviceName() , line 306, is to FIX";
}

void mdtDeviceModbusWagoModule::addAnalogInput(mdtAnalogIo *aio)
{
  Q_ASSERT(aio != 0);

  pvAnalogInputs.append(aio);
}

void mdtDeviceModbusWagoModule::addAnalogOutput(mdtAnalogIo *aio)
{
  Q_ASSERT(aio != 0);

  pvAnalogOutputs.append(aio);
}

void mdtDeviceModbusWagoModule::addDigitalInput(mdtDigitalIo *dio)
{
  Q_ASSERT(dio != 0);

  pvDigitalInputs.append(dio);
}

void mdtDeviceModbusWagoModule::addDigitalOutput(mdtDigitalIo *dio)
{
  Q_ASSERT(dio != 0);

  pvDigitalOutputs.append(dio);
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
  /**
  if(!pvDevice->getRegisterValues(startAddress, registersCount)){
    mdtError e(MDT_DEVICE_ERROR, "Device: " + pvDevice->name() + " : unable to get register values from device", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return false;
  }
  */
  qDebug() << "Module - readRegisters(): getRegisterValues() done - values [hex]: " << hex << pvDevice->registerValues();
  /**
  if(pvDevice->registerValues().size() != registersCount){
    mdtError e(MDT_DEVICE_ERROR, "Device: " + pvDevice->name() + " : received unpexpected count of register values from device", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return false;
  }
  */
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
    /**
    mdtError e(MDT_DEVICE_ERROR, "Device: " + pvDevice->name() + " : unable to set register values to device", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return false;
    */
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
  /**
  if(!pvDevice->getRegisterValues(startAddress, registersCount)){
    mdtError e(MDT_DEVICE_ERROR, "Device: " + pvDevice->name() + " : unable to get register values from device", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return false;
  }
  */
  /**
  if(pvDevice->registerValues().size() != registersCount){
    mdtError e(MDT_DEVICE_ERROR, "Device: " + pvDevice->name() + " : received unpexpected count of register values from device", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return false;
  }
  */
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
    /**
    if(!pvDevice->setRegisterValues(startAddress, values)){
      mdtError e(MDT_DEVICE_ERROR, "Device: " + pvDevice->name() + " : unable to set register values to device", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
      e.commit();
      return false;
    }
    */
    values.clear();
  }

  return true;
}

void mdtDeviceModbusWagoModule::wait(int ms)
{
  Q_ASSERT(pvDevice != 0);

  pvDevice->wait(ms);
}

QVariant mdtDeviceModbusWagoModule::analogInputValueMin(int partNumber, int /*channel*/) const
{
  switch(partNumber){
    case 452:
      return 0.0;
    case 454:
      return 4.0;
    case 457:
      return -10.0;
    default:
      return QVariant();
  }
}

QVariant mdtDeviceModbusWagoModule::analogOutputValueMin(int partNumber, int /*channel*/) const
{
  switch(partNumber){
    case 550:
      return 0.0;
    case 552:
      return 0.0;
    case 556:
      return -10.0;
    default:
      return QVariant();
  }
}

QVariant mdtDeviceModbusWagoModule::analogInputValueMax(int partNumber, int /*channel*/) const
{
  switch(partNumber){
    case 452:
      return 20.0;
    case 454:
      return 20.0;
    case 457:
      return 10.0;
    default:
      return QVariant();
  }
}

QVariant mdtDeviceModbusWagoModule::analogOutputValueMax(int partNumber, int /*channel*/) const
{
  switch(partNumber){
    case 550:
      return 10.0;
    case 552:
      return 20.0;
    case 556:
      return 10.0;
    default:
      return QVariant();
  }
}

int mdtDeviceModbusWagoModule::analogInputValueBitsCount(int partNumber, int /*channel*/) const
{
  switch(partNumber){
    case 452:
      return 12;
    case 454:
      return 12;
    case 457:
      return 13;
    default:
      return -1;
  }
}

int mdtDeviceModbusWagoModule::analogOutputValueBitsCount(int partNumber, int /*channel*/) const
{
  switch(partNumber){
    case 550:
      return 12;
    case 552:
      return 12;
    case 556:
      return 13;
    default:
      return -1;
  }
}

int mdtDeviceModbusWagoModule::analogInputValueLsbIndex(int partNumber, int /*channel*/) const
{
  switch(partNumber){
    case 452:
      return 3;
    case 454:
      return 3;
    case 457:
      return 3;
    default:
      return -1;
  }
}

int mdtDeviceModbusWagoModule::analogOutputValueLsbIndex(int partNumber, int /*channel*/) const
{
  switch(partNumber){
    case 550:
      return 3;
    case 552:
      return 3;
    case 556:
      return 3;
    default:
      return -1;
  }
}

QVariant mdtDeviceModbusWagoModule::analogInputValueSigned(int partNumber, int /*channel*/) const
{
  switch(partNumber){
    case 452:
      return false;
    case 454:
      return false;
    case 457:
      return true;
    default:
      return QVariant();
  }
}

QVariant mdtDeviceModbusWagoModule::analogOutputValueSigned(int partNumber, int /*channel*/) const
{
  switch(partNumber){
    case 550:
      return false;
    case 552:
      return false;
    case 556:
      return true;
    default:
      return QVariant();
  }
}

QVariant mdtDeviceModbusWagoModule::analogInputValueScaledFromMinToMax(int partNumber, int channel) const
{
  switch(partNumber){
    case 452:
      return true;
    case 454:
      return true;
    case 457:
      return true;
    default:
      return QVariant();
  }
}

QVariant mdtDeviceModbusWagoModule::analogOutputValueScaledFromMinToMax(int partNumber, int channel) const
{
  switch(partNumber){
    case 550:
      return true;
    case 552:
      return true;
    case 556:
      return true;
    default:
      return QVariant();
  }
}

QVariant mdtDeviceModbusWagoModule::analogInputValueConversionFactor(int partNumber, int channel) const
{
  switch(partNumber){
    case 452:
      return 1.0;
    case 454:
      return 1.0;
    case 457:
      return 1.0;
    default:
      return QVariant();
  }
}

QVariant mdtDeviceModbusWagoModule::analogOutputValueConversionFactor(int partNumber, int channel) const
{
  switch(partNumber){
    case 550:
      return 1.0;
    case 552:
      return 1.0;
    case 556:
      return 1.0;
    default:
      return QVariant();
  }
}

int mdtDeviceModbusWagoModule::analogInputsCount(int partNumber) const
{
  switch(partNumber){
    case 452:
      return 2;
    case 454:
      return 2;
    case 457:
      return 4;
    case 550:
      return 0;
    default:
      return -1;
  }
}

int mdtDeviceModbusWagoModule::analogOutputsCount(int partNumber) const
{
  switch(partNumber){
    case 452:
      return 0;
    case 454:
      return 0;
    case 457:
      return 0;
    case 550:
      return 2;
    case 552:
      return 2;
    case 556:
      return 2;
    default:
      return -1;
  }
}

QString mdtDeviceModbusWagoModule::analogInputUnit(int partNumber, int /*channel*/) const
{
  switch(partNumber){
    case 452:
      return "mA";
    case 454:
      return "mA";
    case 457:
      return "V";
    default:
      return "";
  }
}

QString mdtDeviceModbusWagoModule::analogOutputUnit(int partNumber, int /*channel*/) const
{
  switch(partNumber){
    case 550:
      return "V";
    case 552:
      return "mA";
    default:
    case 556:
      return "V";
      return "";
  }
}

int mdtDeviceModbusWagoModule::digitalInputsProcessImageSize(quint16 word) const
{
  // Bit 0 is set for a input module
  if(word & 1){
    return ((word & 0x0F00) >> 8);
  }else{
    return 0;
  }
}

int mdtDeviceModbusWagoModule::digitalInputsCount(quint16 word) const
{
  return digitalInputsProcessImageSize(word);
}

int mdtDeviceModbusWagoModule::digitalOutputsProcessImageSize(quint16 word) const
{
  // Bit 0 is set for a input module
  if(word & 1){
    return 0;
  }else{
    return ((word & 0x0F00) >> 8);
  }
}

int mdtDeviceModbusWagoModule::digitalOutputsCount(quint16 word) const
{
  return digitalOutputsProcessImageSize(word);
}

mdtAnalogIo *mdtDeviceModbusWagoModule::getNewAnalogInput(int partNumber, int channel) const
{
  mdtAnalogIo *aio;
  QVariant value;
  double min, max;
  int intValueBitsCount, intValueLsbIndex;
  bool intValueSigned;
  bool scaleFromMinToMax;
  double conversionFactor;

  // Get I/O module's parameters
  value = analogInputValueMin(partNumber, channel);
  if(value.isNull()){
    mdtError e(MDT_DEVICE_ERROR, "Cannot get analog input range minimum value (unknown part number: " + partNumberText() + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return 0;
  }
  min = value.toDouble();
  value = analogInputValueMax(partNumber, channel);
  if(!value.isValid()){
    mdtError e(MDT_DEVICE_ERROR, "Cannot get analog input range maximum value (unknown part number: " + partNumberText() + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return 0;
  }
  max = value.toDouble();
  intValueBitsCount = analogInputValueBitsCount(partNumber, channel);
  if(intValueBitsCount < 0){
    mdtError e(MDT_DEVICE_ERROR, "Cannot get analog input value bits count (unknown part number: " + partNumberText() + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return 0;
  }
  intValueLsbIndex = analogInputValueLsbIndex(partNumber, channel);
  if(intValueLsbIndex < 0){
    mdtError e(MDT_DEVICE_ERROR, "Cannot get analog input value LSB index (unknown part number: " + partNumberText() + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return 0;
  }
  value = analogInputValueSigned(partNumber, channel);
  if(!value.isValid()){
    mdtError e(MDT_DEVICE_ERROR, "Cannot check if analog input value is signed (unknown part number: " + partNumberText() + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return 0;
  }
  intValueSigned = value.toBool();
  value = analogInputValueScaledFromMinToMax(partNumber, channel);
  if(!value.isValid()){
    mdtError e(MDT_DEVICE_ERROR, "Cannot check if analog input value is scaled from minimum to maximum (unknown part number: " + partNumberText() + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return 0;
  }
  scaleFromMinToMax = value.toBool();
  value = analogInputValueConversionFactor(partNumber, channel);
  if(!value.isValid()){
    mdtError e(MDT_DEVICE_ERROR, "Cannot get analog input conversion factor (unknown part number: " + partNumberText() + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return 0;
  }
  conversionFactor = value.toDouble();
  // Have all needed parameters, build I/O
  aio = new mdtAnalogIo;
  if(aio == 0){
    mdtError e(MDT_DEVICE_ERROR, "Memory allocation failed during analog input creation (unknown part number: " + partNumberText() + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWagoModule");
    e.commit();
    return 0;
  }
  aio->setRange(min, max, intValueBitsCount, intValueLsbIndex, intValueSigned, scaleFromMinToMax, conversionFactor);
  aio->setUnit("[" + analogInputUnit(partNumber, channel) + "]");
  aio->setLabel("Module: 750-" + QString::number(partNumber));

  return aio;
}

mdtAnalogIo *mdtDeviceModbusWagoModule::getNewAnalogOutput(int partNumber, int channel)
{
  mdtAnalogIo *aio;
  QVariant value;
  double min, max;
  int intValueBitsCount, intValueLsbIndex;
  bool intValueSigned;
  bool scaleFromMinToMax;
  double conversionFactor;

  // Get I/O module's parameters
  value = analogOutputValueMin(partNumber, channel);
  if(!value.isValid()){
    pvLastError.setError(tr("Cannot get analog output range minimum value (unknown part number: ") + partNumberText() + tr(")"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
    pvLastError.commit();
    return 0;
  }
  min = value.toDouble();
  value = analogOutputValueMax(partNumber, channel);
  if(!value.isValid()){
    pvLastError.setError(tr("Cannot get analog output range maximum value (unknown part number: ") + partNumberText() + tr(")"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
    pvLastError.commit();
    return 0;
  }
  max = value.toDouble();
  intValueBitsCount = analogOutputValueBitsCount(partNumber, channel);
  if(intValueBitsCount < 0){
    pvLastError.setError(tr("Cannot get analog output value bits count (unknown part number: ") + partNumberText() + tr(")"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
    pvLastError.commit();
    return 0;
  }
  intValueLsbIndex = analogOutputValueLsbIndex(partNumber, channel);
  if(intValueLsbIndex < 0){
    pvLastError.setError(tr("Cannot get analog output value LSB index (unknown part number: ") + partNumberText() + tr(")"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
    pvLastError.commit();
    return 0;
  }
  value = analogOutputValueSigned(partNumber, channel);
  if(!value.isValid()){
    pvLastError.setError(tr("Cannot check if analog output value is signed (unknown part number: ") + partNumberText() + tr(")"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
    pvLastError.commit();
    return 0;
  }
  intValueSigned = value.toBool();
  value = analogOutputValueScaledFromMinToMax(partNumber, channel);
  if(!value.isValid()){
    pvLastError.setError(tr("Cannot check if analog output value is scaled from minimum to maximum (unknown part number: ") + partNumberText() + tr(")"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
    pvLastError.commit();
    return 0;
  }
  scaleFromMinToMax = value.toBool();
  value = analogOutputValueConversionFactor(partNumber, channel);
  if(!value.isValid()){
    pvLastError.setError(tr("Cannot get analog output conversion factor (unknown part number: ") + partNumberText() + tr(")"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
    pvLastError.commit();
    return 0;
  }
  conversionFactor = value.toDouble();
  // Have all needed parameters, build I/O
  aio = new mdtAnalogIo;
  if(aio == 0){
    pvLastError.setError(tr("Memory allocation failed during analog output creation (unknown part number: ") + partNumberText() + tr(")"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
    pvLastError.commit();
    return 0;
  }
  aio->setRange(min, max, intValueBitsCount, intValueLsbIndex, intValueSigned, scaleFromMinToMax, conversionFactor);
  aio->setUnit("[" + analogOutputUnit(partNumber, channel) + "]");
  aio->setLabel("Module: 750-" + QString::number(partNumber));
  
  qDebug() << "Adding analog output " << aio->label();

  return aio;
}

bool mdtDeviceModbusWagoModule::addAnalogIos(int partNumber)
{
  int iosCount;
  int i;
  mdtAnalogIo *aio;

  // Store part number
  pvPartNumber = partNumber;
  // Add module's analog inputs
  iosCount = analogInputsCount(partNumber);
  for(i = 0; i < iosCount; ++i){
    aio = getNewAnalogInput(partNumber, i);
    if(aio == 0){
      pvLastError.setError(tr("Cannot get I/O module's parameters (part number: ") + partNumberText() + tr(")"), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
      pvLastError.commit();
      clear(true);
      return false;
    }
    pvAnalogInputs.append(aio);
  }
  // Add module's analog outputs
  iosCount = analogOutputsCount(partNumber);
  for(i = 0; i < iosCount; ++i){
    aio = getNewAnalogOutput(partNumber, i);
    if(aio == 0){
      pvLastError.setError(tr("Cannot get I/O module's parameters (part number: ") + partNumberText() + tr(")"), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
      pvLastError.commit();
      clear(true);
      return false;
    }
    pvAnalogOutputs.append(aio);
  }
  // If both analog inputs count and analog outputs count are 0, module is not recognized
  if( (pvAnalogInputs.size() == 0) && (pvAnalogOutputs.size() == 0) ){
    pvLastError.setError(tr("Cannot get analog I/O's count for module with part number: ") + partNumberText(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
    pvLastError.commit();
    clear(true);
    return false;
  }
  // Set module type
  if( (pvAnalogInputs.size() > 0) && (pvAnalogOutputs.size() > 0) ){
    pvLastError.setError(tr("Analog module with inputs and outputs is not supported (part number: ") + partNumberText() + tr(")"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
    pvLastError.commit();
    clear(true);
    return false;
  }else if(pvAnalogInputs.size() > 0){
    pvType = AnalogInputs;
  }else{
    pvType = AnalogOutputs;
  }

  return true;
}

bool mdtDeviceModbusWagoModule::addDigitalIos(quint16 word)
{
  int iosCount;
  int i;
  mdtDigitalIo *dio;

  // Add digital inputs
  iosCount = digitalInputsCount(word);
  if(iosCount < 0){
    pvLastError.setError(tr("Cannot get module's number of digital inputs."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
    pvLastError.commit();
    return false;
  }
  for(i = 0; i < iosCount; ++i){
    dio = new mdtDigitalIo;
    if(dio == 0){
      pvLastError.setError(tr("Unable to build a digital I/O (memory full ?)"), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
      pvLastError.commit();
      return false;
    }
    pvDigitalInputs.append(dio);
  }
  // Add digital outputs
  iosCount = digitalOutputsCount(word);
  if(iosCount < 0){
    pvLastError.setError(tr("Cannot get module's number of digital outputs"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
    pvLastError.commit();
    return false;
  }
  for(i = 0; i < iosCount; ++i){
    dio = new mdtDigitalIo;
    if(dio == 0){
      pvLastError.setError(tr("Unable to build a digital I/O (memory full ?)"), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWagoModule");
      pvLastError.commit();
      clear(true);
      return false;
    }
    pvDigitalOutputs.append(dio);
  }
  // Set module type
  if((pvDigitalInputs.size() > 0)&&(pvDigitalOutputs.size() > 0)){
    pvType = DigitalInputsOutputs;
  }else if(pvDigitalInputs.size() > 0){
    pvType = DigitalInputs;
  }else{
    pvType = DigitalOutputs;
  }

  return true;
}
