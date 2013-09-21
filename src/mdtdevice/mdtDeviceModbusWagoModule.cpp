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
#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"
#include "mdtError.h"

mdtDeviceModbusWagoModule::mdtDeviceModbusWagoModule(bool autoDeleteIos)
{
  pvAutoDeleteIos = autoDeleteIos;
  pvType = Unknown;
  pvPartNumber = 0;
}

mdtDeviceModbusWagoModule::~mdtDeviceModbusWagoModule()
{
  clear();
}

void mdtDeviceModbusWagoModule::clear()
{
  pvType = Unknown;
  pvPartNumber = 0;
  if(pvAutoDeleteIos){
    qDeleteAll(pvAnalogIos);
    qDeleteAll(pvDigitalIos);
  }
  pvAnalogIos.clear();
  pvDigitalIos.clear();
  pvStatusBytes.clear();
  pvControlBytes.clear();
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
    firstAddress = qMin(firstAddress, pvStatusBytes.at(0).first);
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
    firstAddress = qMin(firstAddress, pvControlBytes.at(0).first);
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

quint8 mdtDeviceModbusWagoModule::statusByte(int channel) const
{
}

void mdtDeviceModbusWagoModule::setControlByte(int channel, quint8 byte)
{
}

quint8 mdtDeviceModbusWagoModule::controlByte(int channel) const
{
}

void mdtDeviceModbusWagoModule::setRegisterDataWord(int channel, quint16 word)
{
}

quint16 mdtDeviceModbusWagoModule::registerDataWord(int channel) const
{
}

void mdtDeviceModbusWagoModule::setRegisterDataBytes(int channel, quint8 byteD1, quint8 byteD0)
{
}

quint8 mdtDeviceModbusWagoModule::registerDataByteD1(int channel)
{
}

quint8 mdtDeviceModbusWagoModule::registerDataByteD0(int channel)
{
}

bool mdtDeviceModbusWagoModule::readRegisters(int firstChannel, int lastChannel, bool needDeviceSetupState)
{
}

bool mdtDeviceModbusWagoModule::writeRegisters(int firstChannel, int lastChannel, bool needDeviceSetupState)
{
}

bool mdtDeviceModbusWagoModule::readStatusBytes(int firstChannel, int lastChannel, bool needDeviceSetupState)
{
}

bool mdtDeviceModbusWagoModule::writeControlBytes(int firstChannel, int lastChannel, bool needDeviceSetupState)
{
}

QVariant mdtDeviceModbusWagoModule::analogIoModuleValueMin(int partNumber) const
{
  switch(partNumber){
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
