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
  if(!getRegisterValues(0x2011, 1)){
    return false;
  }
  if(pvRegisterValues.size() != 1){
    return false;
  }
  return (pvRegisterValues.at(0) == 750);
}

void mdtDeviceModbusWago::sandBox()
{
  /**
  int transactionId;
  mdtPortTransaction *transaction;
  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadInputRegisters(0x2011, 1);
  if(pdu.isEmpty()){
    return;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  // Send query
  transaction->setQueryReplyMode(true);
  transactionId = pvTcpPortManager->writeData(pdu, transaction);
  if(transactionId < 0){
    setStateFromPortError(transactionId);
    return;
  }
  // Wait on result (use device's defined timeout)
  if(!pvTcpPortManager->waitOnFrame(transactionId)){
    return;
  }
  if(pvCodec->decode(pvTcpPortManager->readenFrame(transactionId)) < 0){
    return;
  }
  */
  ///getRegisterValues(0x2011, 1);
  ///qDebug() << "mdtDeviceModbusWago::sandBox() , values: " << pvRegisterValues.at(0);
  ///qDebug() << "mdtDeviceModbusWago::sandBox() , Nb of analog outputs: " << pvCodec->values().at(0).toInt() / 16;
  qDebug() << "mdtDeviceModbusWago::sandBox() , is Wago 750: " << isWago750();
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
