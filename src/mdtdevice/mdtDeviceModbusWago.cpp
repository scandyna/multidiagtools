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
  if(pvRegisterValues.size() != 1){
    return false;
  }
  mdtError e2(MDT_DEVICE_ERROR, "Device " + name() + ": device is from Wago", mdtError::Info);
  MDT_ERROR_SET_SRC(e2, "mdtDeviceModbusWago");
  e2.commit();

  return (pvRegisterValues.at(0) == 750);
}

int mdtDeviceModbusWago::analogOutputsCount()
{

  if(!getRegisterValues(0x1022, 1)){
    return -1;
  }
  if(pvRegisterValues.size() != 1){
    return -1;
  }
  return (pvRegisterValues.at(0) / 16);
}

int mdtDeviceModbusWago::analogInputsCount()
{
  if(!getRegisterValues(0x1023, 1)){
    return -1;
  }
  if(pvRegisterValues.size() != 1){
    return -1;
  }
  return (pvRegisterValues.at(0) / 16);
}

int mdtDeviceModbusWago::digitalOutputsCount()
{
  if(!getRegisterValues(0x1024, 1)){
    return -1;
  }
  if(pvRegisterValues.size() != 1){
    return -1;
  }
  return pvRegisterValues.at(0);
}

int mdtDeviceModbusWago::digitalInputsCount()
{
  if(!getRegisterValues(0x1025, 1)){
    return -1;
  }
  if(pvRegisterValues.size() != 1){
    return -1;
  }
  return pvRegisterValues.at(0);
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
  mdtAnalogIo *aio;
  ///mdtAnalogIo *ao;
  mdtDigitalIo *di;
  mdtDigitalIo *dout;
  QVariant var;
  bool isInput;

  // Clear current I/O setup
  ios->deleteIos();
  // Get modules configuration
  /// \bug If number of requested modules is > 62, this hangs up with 750-352 filedbus (possibly a bug in mdtLib..)
  if(!getRegisterValues(0x2030, 62)){
    qDebug() << "Cannot get I/O config";
    return false;
  }
  qDebug() << "I/O config: " << pvRegisterValues;
  // Setup the Fieldbus Coupler.
  if(pvRegisterValues.size() < 1){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no field bus coupler found", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  word = pvRegisterValues.at(0);
  if((word < 1)||(word > 999)){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": field bus coupler part number is wrong (expected: value from 0 to 999)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  qDebug() << "Found filebus part number 750 -" << word;
  // Setup I/O modules
  if(pvRegisterValues.size() < 2){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no I/O modules found", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  for(i=1; i<pvRegisterValues.size(); i++){
    word = pvRegisterValues.at(i);
    if(word == 0){
      // We have detected all connected I/Os
      break;
    }
    // Check module type (analog or digital)
    if(word & 0x8000){
      qDebug() << "Module[" << i << "]: digital I/O";
    }else{
      qDebug() << "Module[" << i << "]: analog I/O";
      // Check if it's a input or output module
      var = analogIoModuleIsInput(word);
      if(!var.isValid()){
        mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": I/O detection failed for a analog I/O (cannot check if it is a input or a output)", mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
        e.commit();
        return false;
      }
      isInput = var.toBool();
      aio = getNewAnalogIo(word);
      if(aio == 0){
        mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": I/O detection failed for a analog I/O (cannot get encode/decode parameters)", mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
        e.commit();
        return false;
      }
      if(isInput){
        qDebug() << " -> Input";
        ios->addAnalogInput(aio);
      }else{
        qDebug() << " -> Output";
      }
      
    }
    
    
    ///qDebug() << " -> " << hex << (pvRegisterValues.at(i) >> 8) << "," << (pvRegisterValues.at(i) & 0xFF);
    /**
    if(i > 0){
      if(word & 0x8000){
        qDebug() << " -> Digital I/O module";
      }else{
        qDebug() << " -> Analog I/O module, range: " << analogIoValueMin(word) << " to " << analogIoValueMax(word) << " , resolution: " << analogIoValueBitsCount(pvRegisterValues.at(i)) \
          << " , signed: " << analogIoValueSigned(word) << " , is input: " << analogIoIsInput(word);
      }
    }
    */
  }
  return true;
  
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
  
  
  // Setup
  ios->deleteIos();
  for(i=0; i<analogInputsCnt; i++){
    ///ai = new mdtAnalogIo;
    ///ai->setAddress(i);
    ///ai->setLabelShort("AI" + QString::number(i+1));
  }
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

  return aio;
}

bool mdtDeviceModbusWago::addAnalogIos(QList<mdtAnalogIo*> analogInputs, QList<mdtAnalogIo*> analogOutputs, int partNumber) const
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

bool mdtDeviceModbusWago::getRegisterValues(int address, int n)
{
  Q_ASSERT(address >= 0);
  Q_ASSERT(n > 0);

  int transactionId;
  mdtPortTransaction *transaction;
  QByteArray pdu;
  int i;

  // Clear previous results
  pvRegisterValues.clear();
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
    setStateFromPortError(transactionId);
    /// \todo restore transaction ???
    return false;
  }
  // Wait on result (use device's defined timeout)
  if(!pvTcpPortManager->waitOnFrame(transactionId)){
    return false;
  }
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
    pvRegisterValues.append(pvCodec->values().at(i).toInt());
  }

  return true;
}
