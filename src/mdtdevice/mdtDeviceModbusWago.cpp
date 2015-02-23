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
  qDeleteAll(pvModules);
}


// mdtAbstractPort::error_t mdtDeviceModbusWago::connectToDevice(const mdtPortInfo & portInfo)
// {
//   // Check that port manager is not running
//   if(!pvTcpPortManager->isClosed()){
//     pvTcpPortManager->stop();
//   }
//   // Try to connect
//   pvTcpPortManager->setPortInfo(portInfo);
//   if(!pvTcpPortManager->start()){
//     return mdtAbstractPort::PortNotFound;
//   }
//   // Connected - check if device is a Wago 750 fieldbus coupler
//   if(!isWago750()){
//     pvTcpPortManager->stop();
//     return mdtAbstractPort::PortNotFound;
//   }
//   qDebug() << "mdtDeviceModbusWago::connectToDevice() - found WAGO device at " << portInfo.portName();
//   return mdtAbstractPort::NoError;
// }

/**
mdtAbstractPort::error_t mdtDeviceModbusWago::connectToDevice(const mdtDeviceInfo &devInfo)
{
  return mdtDeviceModbus::connectToDevice(devInfo);
}
*/

// mdtAbstractPort::error_t mdtDeviceModbusWago::connectToDevice(const QList<mdtPortInfo*> &scanResult, int hardwareNodeId, int bitsCount, int startFrom)
// {
//   return mdtDeviceModbus::connectToDevice(scanResult, hardwareNodeId, bitsCount, startFrom);
// }

// mdtAbstractPort::error_t mdtDeviceModbusWago::connectToDevice(const QList<int> & existingHwNodeIdList)
// {
//   return mdtDeviceModbus::connectToDevice(existingHwNodeIdList);
// }

bool mdtDeviceModbusWago::isWago750()
{
  // If device is not from Wago, server can return a error (Something like: invalid address range)
  // We put a information into the log, so that we know that we are scanning.
  mdtError e1(MDT_DEVICE_ERROR, deviceIdString() + ": checking if device is from Wago ...", mdtError::Info);
  MDT_ERROR_SET_SRC(e1, "mdtDeviceModbusWago");
  e1.commit();

  if(!getRegisterValues(0x2011, 1)){
    return false;
  }
  if(registerValues().size() != 1){
    return false;
  }
  mdtError e2(MDT_DEVICE_ERROR, deviceIdString() + ": device is from Wago", mdtError::Info);
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

bool mdtDeviceModbusWago::detectIos(const QMap<int, mdtDeviceModbusWagoModule*> specialModules)
{
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
  deleteIos();
  qDeleteAll(pvModules);
  pvModules.clear();
  // Detect I/O's count
  analogInputsCnt = analogInputsCount();
  if(analogInputsCnt < 0){
    pvLastError.setError(deviceIdString() + ": error occured during analog inputs count detection", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWago");
    pvLastError.commit();
    return false;
  }
  analogOutputsCnt = analogOutputsCount();
  if(analogOutputsCnt < 0){
    pvLastError.setError(deviceIdString() + ": error occured during analog outputs count detection", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWago");
    pvLastError.commit();
    return false;
  }
  digitalInputsCnt = digitalInputsCount();
  if(digitalInputsCnt < 0){
    pvLastError.setError(deviceIdString() + ": error occured during digital inputs count detection", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWago");
    pvLastError.commit();
    return false;
  }
  digitalOutputsCnt = digitalOutputsCount();
  if(digitalOutputsCnt < 0){
    pvLastError.setError(deviceIdString() + ": error occured during digital outputs count detection", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWago");
    pvLastError.commit();
    return false;
  }
  // Get modules configuration
  /// \bug If number of requested modules is > 62, this hangs up with 750-352 filedbus (possibly a bug in mdtLib..)
  if(!getRegisterValues(0x2030, 62)){
    qDebug() << "Cannot get I/O config";
    return false;
  }
  ///qDebug() << "I/O config: " << registerValues();
  // Setup the Fieldbus Coupler.
  if(registerValues().size() < 1){
    pvLastError.setError(deviceIdString() + ": no field bus coupler found", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWago");
    pvLastError.commit();
    return false;
  }
  word = registerValues().at(0);
  if((word < 1)||(word > 999)){
    pvLastError.setError(deviceIdString() + ": field bus coupler part number is wrong (expected: value from 1 to 999)", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWago");
    pvLastError.commit();
    return false;
  }
  qDebug() << "Found filedbus part number 750 -" << word;
  // Setup each module and add them to container
  if(registerValues().size() < 2){
    pvLastError.setError(deviceIdString() + ": no I/O modules found", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWago");
    pvLastError.commit();
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
    ///qDebug() << "I/O setup word: " << word  << " ...";
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
      /// \todo Provisoire !!!!
      if(word == 464){
        module = new mdtDeviceModbusWagoModuleRtd(false, this);
      }else{
        module = new mdtDeviceModbusWagoModule(false, this);
      }
      if(!module->setupFromRegisterWord(word)){
        module->clear(true);
        delete module;
        module = 0;
      }
      module->updateAddresses(aiAddress, aoAddressRead, aoAddressWrite, diAddress, doAddressRead, doAddressWrite);
    }
    // If module is null, a problem occured - cleanup and fail
    if(module == 0){
      pvLastError.setError(deviceIdString() + ": setup failed on a module (setup word: " + QString::number(word) + ")", mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWago");
      pvLastError.commit();
      qDeleteAll(pvModules);
      pvModules.clear();
      deleteIos();
      return false;
    }
    Q_ASSERT(module != 0);
    Q_ASSERT(module->type() != mdtDeviceModbusWagoModule::Unknown);
    ///qDebug() << "Module pos. " << i << ": word: " << word << ", firs AI address: " << module->firstAiAddress() << " , IN count: " << module->analogInputs().size();
    // Add module's I/Os to container
    addInputs(module->analogInputs());
    addOutputs(module->analogOutputs());
    addInputs(module->digitalInputs());
    addOutputs(module->digitalOutputs());
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
  if(ios()->analogInputsCount() != analogInputsCnt){
    qDeleteAll(pvModules);
    pvModules.clear();
    deleteIos();
    pvLastError.setError(deviceIdString() + ": analog inputs count not coherent", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWago");
    pvLastError.commit();
    return false;
  }
  if(ios()->analogOutputsCount() != analogOutputsCnt){
    qDeleteAll(pvModules);
    pvModules.clear();
    deleteIos();
    pvLastError.setError(deviceIdString() + ": analog outputs count not coherent", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWago");
    pvLastError.commit();
    return false;
  }
  if(ios()->digitalInputsCount() != digitalInputsCnt){
    qDeleteAll(pvModules);
    pvModules.clear();
    deleteIos();
    pvLastError.setError(deviceIdString() + ": digital inputs count not coherent", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWago");
    pvLastError.commit();
    return false;
  }
  if(ios()->digitalOutputsCount() != digitalOutputsCnt){
    qDeleteAll(pvModules);
    pvModules.clear();
    deleteIos();
    pvLastError.setError(deviceIdString() + ": digital outputs count not coherent", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbusWago");
    pvLastError.commit();
    return false;
  }
  // Set a default short label for each I/O
  ios()->setIosDefaultLabelShort();

  return true;
}
