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
#include "mdtDeviceModbusWagoModule.h"
#include "mdtDeviceModbusWagoModuleRtd.h"
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
  if(!pvTcpPortManager->isClosed()){
    qDebug() << "mdtDeviceModbusWago::connectToDevice() : stopping ...";
    pvTcpPortManager->stop();
  }
  // Scan looking in chache file first
  portInfoList = pvTcpPortManager->scan(pvTcpPortManager->readScanResult());
  for(i=0; i<portInfoList.size(); i++){
    Q_ASSERT(portInfoList.at(i) != 0);
    // Try to connect
    pvTcpPortManager->setPortInfo(*portInfoList.at(i));
    if(!pvTcpPortManager->start()){
      continue;
    }
    qDebug() << "mdtDeviceModbusWago::connectToDevice() : Running ...";
    // Connected, check if device is a Wago 750
    if(isWago750()){
      qDeleteAll(portInfoList);
      qDebug() << "mdtDeviceModbusWago::connectToDevice() : Is a Wago 750";
      return mdtAbstractPort::NoError;
    }
    pvTcpPortManager->stop();
  }
  qDeleteAll(portInfoList);
  portInfoList.clear();
  pvTcpPortManager->stop();
  // Scan network
  portInfoList = pvTcpPortManager->scan(QNetworkInterface::allInterfaces(), 502, 100);
  for(i=0; i<portInfoList.size(); i++){
    Q_ASSERT(portInfoList.at(i) != 0);
    // Try to connect
    qDebug() << "mdtDeviceModbusWago::connectToDevice() : Trying " << portInfoList.at(i)->portName() << " ...";
    pvTcpPortManager->setPortInfo(*portInfoList.at(i));
    if(!pvTcpPortManager->start()){
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
    pvTcpPortManager->stop();
  }
  qDeleteAll(portInfoList);
  pvTcpPortManager->stop();

  return mdtAbstractPort::PortNotFound;
}

mdtAbstractPort::error_t mdtDeviceModbusWago::connectToDevice(const QList<mdtPortInfo*> &scanResult, int hardwareNodeId, int bitsCount, int startFrom)
{
  ///Q_ASSERT(!pvTcpPortManager->isRunning());

  int i;

  // Check that port manager is not running
  if(!pvTcpPortManager->isClosed()){
    qDebug() << "mdtDeviceModbusWago::connectToDevice() : stopping ...";
    pvTcpPortManager->stop();
  }
  for(i=0; i<scanResult.size(); i++){
    Q_ASSERT(scanResult.at(i) != 0);
    // Try to connect
    pvTcpPortManager->setPortInfo(*scanResult.at(i));
    if(!pvTcpPortManager->start()){
      continue;
    }
    // We are connected here, check if device is a Wago 750 fieldbus coupler
    if(!isWago750()){
      pvTcpPortManager->stop();
      continue;
    }
    // Get the hardware node ID
    if(pvTcpPortManager->getHardwareNodeAddress(bitsCount, startFrom) == hardwareNodeId){
      return mdtAbstractPort::NoError;
    }else{
      pvTcpPortManager->stop();
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

bool mdtDeviceModbusWago::detectIos(mdtDeviceIos *ios, const QMap<int, mdtDeviceModbusWagoModule*> specialModules)
{
  Q_ASSERT(ios != 0);

  int analogInputsCnt;
  int analogOutputsCnt;
  int digitalInputsCnt;
  int digitalOutputsCnt;
  int i;
  quint16 word;
  QVariant var;
  mdtDeviceModbusWagoModule *module;
  int aiAddress;
  int aoAddressRead, aoAddressWrite;
  int diAddress;
  int doAddressRead, doAddressWrite;

  // Clear current I/O setup
  ios->deleteIos();
  qDeleteAll(pvModules);
  pvModules.clear();
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
  // Setup each module and add them to container
  if(registerValues().size() < 2){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no I/O modules found", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  /// \todo Handle adrress mapping of bus coupler (can be a problem with > 256 I/O on 750-352 f.ex.)
  aiAddress = 0;
  aoAddressRead = 0x0200;
  aoAddressWrite = 0;
  diAddress = 0;
  doAddressRead = 0x0200;
  doAddressWrite = 0;
  for(i = 1; i < registerValues().size(); ++i){
    word = registerValues().at(i);
    if(word == 0){
      // We have detected all connected I/Os
      break;
    }
    qDebug() << "I/O setup word: " << word  << " ...";
    // Check if we have a special (manually configured) module
    module = specialModules.value(i);
    if(module != 0){
      // Some special modules needs to communicate with device, so we address a first time
      module->updateAddresses(aiAddress, aoAddressRead, aoAddressWrite, diAddress, doAddressRead, doAddressWrite);
      if(!module->setupFromRegisterWord(word)){
        // We must not free given module, simply set pointer Null
        module = 0;
      }
      // Re-address module
      module->updateAddresses(aiAddress, aoAddressRead, aoAddressWrite, diAddress, doAddressRead, doAddressWrite);
    }else{
      // Create a module object, setup and adrress it
      module = new mdtDeviceModbusWagoModule(false, this);
      if(!module->setupFromRegisterWord(word)){
        module->clear(true);
        delete module;
        module = 0;
      }
      module->updateAddresses(aiAddress, aoAddressRead, aoAddressWrite, diAddress, doAddressRead, doAddressWrite);
    }
    // If module is null, a problem occured - cleanup and fail
    if(module == 0){
      mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": setup failed on a module (setup word: " + QString::number(word) + ")", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
      e.commit();
      qDeleteAll(pvModules);
      pvModules.clear();
      ios->deleteIos();
      return false;
    }
    Q_ASSERT(module != 0);
    Q_ASSERT(module->type() != mdtDeviceModbusWagoModule::Unknown);
    // Add module's I/Os to container
    ios->addAnalogInputs(module->analogInputs());
    ios->addAnalogOutputs(module->analogOutputs());
    ios->addDigitalInputs(module->digitalInputs());
    ios->addDigitalOutputs(module->digitalOutputs());
    // Update current first addresses
    aiAddress = module->nextModuleFirstAiAddress();
    aoAddressRead = module->nextModuleFirstAoAddressRead();
    aoAddressWrite = module->nextModuleFirstAoAddressWrite();
    diAddress = module->nextModuleFirstDiAddress();
    doAddressRead = module->nextModuleFirstDoAddressRead();
    doAddressWrite = module->nextModuleFirstDoAddressWrite();
    // Add module to modules container
    pvModules.append(module);
  }
  // Check coherence between detected setup and I/Os count
  if(ios->analogInputsCount() != analogInputsCnt){
    qDeleteAll(pvModules);
    pvModules.clear();
    ios->deleteIos();
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": analog inputs count not coherent", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  if(ios->analogOutputsCount() != analogOutputsCnt){
    qDeleteAll(pvModules);
    pvModules.clear();
    ios->deleteIos();
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": analog outputs count not coherent", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  if(ios->digitalInputsCount() != digitalInputsCnt){
    qDeleteAll(pvModules);
    pvModules.clear();
    ios->deleteIos();
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": digital inputs count not coherent", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  if(ios->digitalOutputsCount() != digitalOutputsCnt){
    qDeleteAll(pvModules);
    pvModules.clear();
    ios->deleteIos();
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": digital outputs count not coherent", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  // Set a default short label for each I/O
  ios->setIosDefaultLabelShort();

  return true;
}
