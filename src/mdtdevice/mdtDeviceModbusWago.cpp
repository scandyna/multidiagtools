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
#include "mdtDeviceModbusWago.h"
#include "mdtDeviceInfo.h"
#include "mdtModbusTcpPortManager.h"
#include "mdtFrameCodecModbus.h"
#include "mdtPortInfo.h"
#include "mdtError.h"
#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"
#include "mdtDeviceIos.h"
#include <QByteArray>
#include <QString>
#include <QHash>
#include <QNetworkInterface>

#include <QDebug>

mdtDeviceModbusWago::mdtDeviceModbusWago(QObject *parent)
 : mdtDeviceModbus(parent)
{
  setAnalogOutputAddressOffset(0x0200);
  setDigitalOutputAddressOffset(0x0200);
}

mdtDeviceModbusWago::~mdtDeviceModbusWago()
{
}

mdtAbstractPort::error_t mdtDeviceModbusWago::connectToDevice(const mdtDeviceInfo &devInfo)
{
  QList<mdtPortInfo*> portInfoList;
  int i;

  // Check that port manager is not running
  if(pvTcpPortManager->isRunning()){
    qDebug() << "mdtDeviceModbusWago::connectToDevice() : stopping ...";
    pvTcpPortManager->stop();
  }
  // Scan looking in chache file first
  portInfoList = pvTcpPortManager->scan(pvTcpPortManager->readScanResult());
  for(i=0; i<portInfoList.size(); i++){
    Q_ASSERT(portInfoList.at(i) != 0);
    // Try to connect
    pvTcpPortManager->setPortInfo(*portInfoList.at(i));
    if(!pvTcpPortManager->openPort()){
      continue;
    }
    if(!pvTcpPortManager->start()){
      pvTcpPortManager->closePort();
      continue;
    }
    qDebug() << "mdtDeviceModbusWago::connectToDevice() : Running ...";
    // Connected, check if device is a Wago 750
    if(isWago750()){
      qDeleteAll(portInfoList);
      qDebug() << "mdtDeviceModbusWago::connectToDevice() : Is a Wago 750";
      return mdtAbstractPort::NoError;
    }
    pvTcpPortManager->closePort();
  }
  qDeleteAll(portInfoList);
  portInfoList.clear();
  pvTcpPortManager->closePort();
  // Scan network
  portInfoList = pvTcpPortManager->scan(QNetworkInterface::allInterfaces(), 502, 100);
  for(i=0; i<portInfoList.size(); i++){
    Q_ASSERT(portInfoList.at(i) != 0);
    // Try to connect
    qDebug() << "mdtDeviceModbusWago::connectToDevice() : Trying " << portInfoList.at(i)->portName() << " ...";
    pvTcpPortManager->setPortInfo(*portInfoList.at(i));
    if(!pvTcpPortManager->openPort()){
      continue;
    }
    if(!pvTcpPortManager->start()){
      pvTcpPortManager->closePort();
      continue;
    }
    qDebug() << "mdtDeviceModbusWago::connectToDevice() : Running ...";
    // Connected, check if device is a Wago 750
    if(isWago750()){
      pvTcpPortManager->saveScanResult(portInfoList);
      qDeleteAll(portInfoList);
      qDebug() << "mdtDeviceModbusWago::connectToDevice() : Is a Wago 750";
      return mdtAbstractPort::NoError;
    }
    pvTcpPortManager->closePort();
  }
  qDeleteAll(portInfoList);
  pvTcpPortManager->closePort();

  return mdtAbstractPort::PortNotFound;
}

mdtAbstractPort::error_t mdtDeviceModbusWago::connectToDevice(const QList<mdtPortInfo*> &scanResult, int hardwareNodeId, int bitsCount, int startFrom)
{
  Q_ASSERT(!pvTcpPortManager->isRunning());

  int i;

  for(i=0; i<scanResult.size(); i++){
    Q_ASSERT(scanResult.at(i) != 0);
    // Try to connect
    pvTcpPortManager->setPortInfo(*scanResult.at(i));
    if(!pvTcpPortManager->openPort()){
      continue;
    }
    if(!pvTcpPortManager->start()){
      pvTcpPortManager->closePort();
      continue;
    }
    // We are connected here, check if device is a Wago 750 fieldbus coupler
    if(!isWago750()){
      pvTcpPortManager->stop();
      pvTcpPortManager->closePort();
      continue;
    }
    // Get the hardware node ID
    if(pvTcpPortManager->getHardwareNodeAddress(bitsCount, startFrom) == hardwareNodeId){
      return mdtAbstractPort::NoError;
    }else{
      pvTcpPortManager->stop();
      pvTcpPortManager->closePort();
      continue;
    }
  }

  return mdtAbstractPort::PortNotFound;
}

bool mdtDeviceModbusWago::isWago750()
{
  // If device is not from Wago, server can return a error (Something like: invalid address range)
  // We put a information into the log, so that we know that we are scanning.
  mdtError e1(MDT_DEVICE_ERROR, "Device " + name() + ": checking if device is from Wago ...", mdtError::Info);
  MDT_ERROR_SET_SRC(e1, "mdtDeviceModbusWago");
  e1.commit();

  if(!getRegisterValues(0x2011, 1)){
    return false;
  }
  if(registerValues().size() != 1){
    return false;
  }
  mdtError e2(MDT_DEVICE_ERROR, "Device " + name() + ": device is from Wago", mdtError::Info);
  MDT_ERROR_SET_SRC(e2, "mdtDeviceModbusWago");
  e2.commit();

  return (registerValues().at(0) == 750);
}

int mdtDeviceModbusWago::analogOutputsCount()
{
  if(!getRegisterValues(0x1022, 1)){
    return -1;
  }
  if(registerValues().size() != 1){
    return -1;
  }
  return (registerValues().at(0) / 16);
}

int mdtDeviceModbusWago::analogInputsCount()
{
  if(!getRegisterValues(0x1023, 1)){
    return -1;
  }
  if(registerValues().size() != 1){
    return -1;
  }
  return (registerValues().at(0) / 16);
}

int mdtDeviceModbusWago::digitalOutputsCount()
{
  if(!getRegisterValues(0x1024, 1)){
    return -1;
  }
  if(registerValues().size() != 1){
    return -1;
  }
  return registerValues().at(0);
}

int mdtDeviceModbusWago::digitalInputsCount()
{
  if(!getRegisterValues(0x1025, 1)){
    return -1;
  }
  if(registerValues().size() != 1){
    return -1;
  }
  return registerValues().at(0);
}

bool mdtDeviceModbusWago::detectIos(mdtDeviceIos *ios)
{
  Q_ASSERT(ios != 0);

  int analogInputsCnt;
  int analogOutputsCnt;
  int digitalInputsCnt;
  int digitalOutputsCnt;
  ///int modulesCnt;
  int i;
  quint16 word;
  QList<mdtAnalogIo*> analogInputs;
  QList<mdtAnalogIo*> analogOutputs;
  QList<mdtDigitalIo*> digitalInputs;
  QList<mdtDigitalIo*> digitalOutputs;
  mdtAnalogIo *aio;
  ///mdtAnalogIo *ao;
  mdtDigitalIo *dio;
  ///mdtDigitalIo *dout;
  QVariant var;
  ///bool isInput;

  // Clear current I/O setup
  ios->deleteIos();
  // Detect I/O's count
  analogInputsCnt = analogInputsCount();
  if(analogInputsCnt < 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": error occured during analog inputs count detection", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  analogOutputsCnt = analogOutputsCount();
  if(analogOutputsCnt < 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": error occured during analog outputs count detection", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  digitalInputsCnt = digitalInputsCount();
  if(digitalInputsCnt < 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": error occured during digital inputs count detection", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  digitalOutputsCnt = digitalOutputsCount();
  if(digitalOutputsCnt < 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": error occured during digital outputs count detection", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  // Get modules configuration
  /// \bug If number of requested modules is > 62, this hangs up with 750-352 filedbus (possibly a bug in mdtLib..)
  if(!getRegisterValues(0x2030, 62)){
    qDebug() << "Cannot get I/O config";
    return false;
  }
  qDebug() << "I/O config: " << registerValues();
  // Setup the Fieldbus Coupler.
  if(registerValues().size() < 1){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no field bus coupler found", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  word = registerValues().at(0);
  if((word < 1)||(word > 999)){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": field bus coupler part number is wrong (expected: value from 1 to 999)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  qDebug() << "Found filedbus part number 750 -" << word;
  // Get I/O modules setup
  if(registerValues().size() < 2){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no I/O modules found", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  for(i=1; i<registerValues().size(); i++){
    word = registerValues().at(i);
    if(word == 0){
      // We have detected all connected I/Os
      break;
    }
    // Check module type (analog or digital)
    if(word & 0x8000){
      qDebug() << "Module[" << i << "]: digital I/O";
      addDigitalIos(digitalInputs, digitalOutputs, word);
    }else{
      qDebug() << "Module[" << i << "]: analog I/O";
      addAnalogIos(analogInputs, analogOutputs, word);
    }
  }
  // Check coherence between detected setup and I/Os count
  if(analogInputs.size() != analogInputsCnt){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": analog inputs count not coherent", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    qDeleteAll(analogInputs);
    qDeleteAll(analogOutputs);
    qDeleteAll(digitalInputs);
    qDeleteAll(digitalOutputs);
    return false;
  }
  if(analogOutputs.size() != analogOutputsCnt){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": analog outputs count not coherent", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    qDeleteAll(analogInputs);
    qDeleteAll(analogOutputs);
    qDeleteAll(digitalInputs);
    qDeleteAll(digitalOutputs);
    return false;
  }
  if(digitalInputs.size() != digitalInputsCnt){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": digital inputs count not coherent", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    qDeleteAll(analogInputs);
    qDeleteAll(analogOutputs);
    qDeleteAll(digitalInputs);
    qDeleteAll(digitalOutputs);
    return false;
  }
  if(digitalOutputs.size() != digitalOutputsCnt){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": digital outputs count not coherent", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    qDeleteAll(analogInputs);
    qDeleteAll(analogOutputs);
    qDeleteAll(digitalInputs);
    qDeleteAll(digitalOutputs);
    return false;
  }
  // Add I/Os to container
  for(i=0; i<analogInputs.size(); i++){
    aio = analogInputs.at(i);
    Q_ASSERT(aio != 0);
    aio->setAddress(i);
    aio->setLabelShort("AI" + QString::number(i+1));
    ios->addAnalogInput(aio);
  }
  for(i=0; i<analogOutputs.size(); i++){
    aio = analogOutputs.at(i);
    Q_ASSERT(aio != 0);
    aio->setAddress(i);
    aio->setLabelShort("AO" + QString::number(i+1));
    ios->addAnalogOutput(aio);
  }
  for(i=0; i<digitalInputs.size(); i++){
    dio = digitalInputs.at(i);
    Q_ASSERT(dio != 0);
    dio->setAddress(i);
    dio->setLabelShort("DI" + QString::number(i+1));
    ios->addDigitalInput(dio);
  }
  for(i=0; i<digitalOutputs.size(); i++){
    dio = digitalOutputs.at(i);
    Q_ASSERT(dio != 0);
    dio->setAddress(i);
    dio->setLabelShort("DO" + QString::number(i+1));
    ios->addDigitalOutput(dio);
  }

  return true;
}

QVariant mdtDeviceModbusWago::analogIoModuleValueMin(int partNumber) const
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

QVariant mdtDeviceModbusWago::analogIoModuleValueMax(int partNumber) const
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

int mdtDeviceModbusWago::analogIoModuleValueBitsCount(int partNumber) const
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

int mdtDeviceModbusWago::analogIoModuleValueLsbIndex(int partNumber) const
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

QVariant mdtDeviceModbusWago::analogIoModuleValueSigned(int partNumber) const
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

QVariant mdtDeviceModbusWago::analogIoModuleIsInput(int partNumber) const
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

int mdtDeviceModbusWago::analogIoModuleIosCount(int partNumber) const
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

QString mdtDeviceModbusWago::analogIoModuleUnit(int partNumber) const
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

QVariant mdtDeviceModbusWago::digitalIoModuleIsInput(quint16 word) const
{
  // Bit 0 is set for a input module
  if(word & 0x0001){
    // Should be a input, check bit 1
    if(word & 0x002){
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

int mdtDeviceModbusWago::digitalIoModuleIosCount(quint16 word) const
{
  return ((word & 0x0F00) >> 8);
}

mdtAnalogIo *mdtDeviceModbusWago::getNewAnalogIo(int partNumber) const
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

bool mdtDeviceModbusWago::addAnalogIos(QList<mdtAnalogIo*> &analogInputs, QList<mdtAnalogIo*> &analogOutputs, int partNumber) const
{
  int iosCount;
  int i;
  bool isInput;
  QVariant var;
  mdtAnalogIo *aio;

  // check if module is input or output
  var = analogIoModuleIsInput(partNumber);
  if(!var.isValid()){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": Cannot check if I/O module has inputs or a outputs (unknown part number: " + QString::number(partNumber) + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  isInput = var.toBool();
  // Get I/Os count
  iosCount = analogIoModuleIosCount(partNumber);
  if(iosCount < 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": Cannot get module's number of I/Os (unknown part number: " + QString::number(partNumber) + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  // Add I/Os
  for(i=0; i<iosCount; i++){
    aio = getNewAnalogIo(partNumber);
    if(aio == 0){
      mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": Cannot get I/O module's parameters (unknown part number: " + QString::number(partNumber) + ")", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
      e.commit();
      return false;
    }
    if(isInput){
      analogInputs.append(aio);
    }else{
      analogOutputs.append(aio);
    }
  }

  return true;
}

bool mdtDeviceModbusWago::addDigitalIos(QList<mdtDigitalIo*> &digitalInputs, QList<mdtDigitalIo*> &digitalOutputs, quint16 word) const
{
  int iosCount;
  int i;
  bool isInput;
  QVariant var;
  mdtDigitalIo *dio;

  // Check if module is input or output
  var = digitalIoModuleIsInput(word);
  if(!var.isValid()){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": Cannot check if I/O module has inputs or a outputs", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  isInput = var.toBool();
  // Get I/Os count
  iosCount = digitalIoModuleIosCount(word);
  if(iosCount < 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": Cannot get module's number of I/Os", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  // Add I/Os
  for(i=0; i<iosCount; i++){
    dio = new mdtDigitalIo;
    if(dio == 0){
      mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": unable to build a digital I/O (memory full ?)", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
      e.commit();
      return false;
    }
    if(isInput){
      digitalInputs.append(dio);
    }else{
      digitalOutputs.append(dio);
    }
  }

  return true;
}

/**
bool mdtDeviceModbusWago::getRegisterValues(int address, int n)
{
  Q_ASSERT(address >= 0);
  Q_ASSERT(n > 0);

  int transactionId;
  mdtPortTransaction *transaction;
  QByteArray pdu;
  int i;

  // Clear previous results
  registerValues().clear();
  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadInputRegisters(address, n);
  if(pdu.isEmpty()){
    return false;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  // Send query
  transaction->setQueryReplyMode(true);
  transactionId = pvTcpPortManager->writeData(pdu, transaction);
  if(transactionId < 0){
    restoreTransaction(transaction);
    return false;
  }
  // Wait on result (use device's defined timeout)
  if(!pvTcpPortManager->waitOnFrame(transactionId)){
    restoreTransaction(transaction);
    return false;
  }
  // At this state, transaction will be restored by readenFrame()
  if(pvCodec->decode(pvTcpPortManager->readenFrame(transactionId)) < 0){
    return false;
  }
  // Store values
  if(pvCodec->values().size() != n){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received unexptected count of values", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  for(i=0; i<n; i++){
    registerValues().append(pvCodec->values().at(i).toInt());
  }

  return true;
}
*/
