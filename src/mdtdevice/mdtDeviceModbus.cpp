/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtDeviceModbus.h"
#include "mdtModbusTcpPortManager.h"
#include "mdtFrameCodecModbus.h"
#include <QByteArray>
#include <QHash>
#include <QList>

mdtDeviceModbus::mdtDeviceModbus(QObject *parent)
 : mdtDevice(parent)
{
  pvTcpPortManager = new mdtModbusTcpPortManager;
  pvCodec = new mdtFrameCodecModbus;
  /// \note Provisoire !!
  connect(pvTcpPortManager, SIGNAL(newReadenFrame()), this, SLOT(decodeReadenFrames()));
  pvTcpPortManager->setPortName("192.168.1.102:502");
  pvTcpPortManager->openPort();
  pvTcpPortManager->start();
}

mdtDeviceModbus::~mdtDeviceModbus()
{
  delete pvTcpPortManager;
  delete pvCodec;
}

/**
void mdtDeviceModbus::setupAnalogInputs(int count)
{
  Q_ASSERT((count >= 1)&&(count <= 125));

  int i;

  pvAnalogInputs.clear();
  for(i=0; i<count; i++){
    pvAnalogInputs.insert(i, 0);
  }
}
*/

QVariant mdtDeviceModbus::getAnalogInputValue(int address, bool readDirectly)
{
  QVariant value;

  // Check range
  if(address < 0){
    return value;
  }
  /**
  if(address >= pvAnalogInputs.size()){
    return value;
  }
  value = pvAnalogInputs.value(address);
  */

  /// \todo Implement query !

  return value;
}

int mdtDeviceModbus::readAnalogInputs()
{
  Q_ASSERT(pvIos != 0);

  // Setup MODBUS PDU
  QByteArray pdu;
  ///int transactionId;
  ///pdu = pvCodec->encodeReadInputRegisters(0, pvAnalogInputs.size());
  pdu = pvCodec->encodeReadInputRegisters(0, pvIos->analogInputsCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu);

  /**
  transactionId = pvTcpPortManager->writeData(pdu);
  if(transactionId < 0){
    return false;
  }
  */
  /// \note Provisoire !
  /**
  pvTcpPortManager->waitReadenFrame();
  pdu = pvTcpPortManager->readenFrame(transactionId);
  qDebug() << "DEV RD reponse: " << pvCodec->decode(pdu);
  qDebug() << "DEV RD value: " << pvCodec->values();
  */
}

void mdtDeviceModbus::setupAnalogOutputs(int count)
{
  Q_ASSERT((count >= 1)&&(count <= 123));

  int i;

  pvAnalogOutputs.clear();
  for(i=0; i<count; i++){
    pvAnalogOutputs.insert(i, 0);
  }
}

bool mdtDeviceModbus::setAnalogOutput(int address, int value, bool writeDirectly)
{
  // Check range
  if(address < 0){
    return false;
  }
  if(address >= pvAnalogOutputs.size()){
    return false;
  }
  pvAnalogOutputs[address] = value;
  /// \todo Implement query !
  if(writeDirectly){
    // Setup MODBUS PDU
    QByteArray pdu;
    int transactionId;
    pdu = pvCodec->encodeWriteSingleRegister(address, value);
    if(pdu.isEmpty()){
      return false;
    }
    // Send request
    transactionId = pvTcpPortManager->writeData(pdu);
    if(transactionId < 0){
      return false;
    }
    /// \note Provisoire !
    pvTcpPortManager->waitReadenFrame();
    pdu = pvTcpPortManager->readenFrame(transactionId);
    qDebug() << "DEV RD reponse: " << pvCodec->decode(pdu);
    qDebug() << "DEV RD value: " << pvCodec->values();
  }

  return true;
}

QVariant mdtDeviceModbus::getAnalogOutputValue(int address, bool readDirectly)
{
  QVariant value;

  // Check range
  if(address < 0){
    return value;
  }
  if(address >= pvAnalogOutputs.size()){
    return value;
  }
  /// \todo Implement query !

  value = pvAnalogOutputs.value(address);

  return value;
}

bool mdtDeviceModbus::writeAnalogOutputs()
{
  // Setup MODBUS PDU
  QByteArray pdu;
  int transactionId;
  pdu = pvCodec->encodeWriteMultipleRegisters(0, pvAnalogOutputs.values());
  if(pdu.isEmpty()){
    return false;
  }
  // Send request
  transactionId = pvTcpPortManager->writeData(pdu);
  if(transactionId < 0){
    return false;
  }
  /// \note Provisoire !
  pvTcpPortManager->waitReadenFrame();
  pdu = pvTcpPortManager->readenFrame(transactionId);
  qDebug() << "DEV RD reponse: " << pvCodec->decode(pdu);
  qDebug() << "DEV RD value: " << pvCodec->values();

  return true;
}

bool mdtDeviceModbus::readAnalogOutputs()
{
  // Setup MODBUS PDU
  QByteArray pdu;
  int transactionId;
  pdu = pvCodec->encodeReadHoldingRegisters(0, pvAnalogOutputs.size());
  if(pdu.isEmpty()){
    return false;
  }
  // Send request
  transactionId = pvTcpPortManager->writeData(pdu);
  if(transactionId < 0){
    return false;
  }
  /// \note Provisoire !
  pvTcpPortManager->waitReadenFrame();
  pdu = pvTcpPortManager->readenFrame(transactionId);
  qDebug() << "DEV RD reponse: " << pvCodec->decode(pdu);
  qDebug() << "DEV RD value: " << pvCodec->values();

  return true;
}

void mdtDeviceModbus::setupDigitalInputs(int count)
{
  Q_ASSERT((count >= 1)&&(count <= 2000));

  int i;

  pvDigitalInputs.clear();
  for(i=0; i<count; i++){
    pvDigitalInputs.insert(i, false);
  }
}

QVariant mdtDeviceModbus::getDigitalInputState(int address, bool readDirectly)
{
  QVariant state;

  // Check range
  if(address < 0){
    return state;
  }
  if(address >= pvDigitalInputs.size()){
    return state;
  }
  /// \todo Implement query !

  state = pvDigitalInputs.value(address);

  return state;
}

bool mdtDeviceModbus::readDigitalInputs()
{
  // Setup MODBUS PDU
  QByteArray pdu;
  int transactionId;
  pdu = pvCodec->encodeReadDiscreteInputs(0, pvDigitalInputs.size());
  if(pdu.isEmpty()){
    return false;
  }
  // Send request
  transactionId = pvTcpPortManager->writeData(pdu);
  if(transactionId < 0){
    return false;
  }
  /// \note Provisoire !
  pvTcpPortManager->waitReadenFrame();
  pdu = pvTcpPortManager->readenFrame(transactionId);
  qDebug() << "DEV RD reponse: " << pvCodec->decode(pdu);
  qDebug() << "DEV RD value: " << pvCodec->values();

  return true;
}

void mdtDeviceModbus::setupDigitalOutputs(int count)
{
  Q_ASSERT((count >= 1)&&(count <= 1968));

  int i;

  pvDigitalOutputs.clear();
  for(i=0; i<count; i++){
    pvDigitalOutputs.insert(i, false);
  }
}

bool mdtDeviceModbus::setDigitalOutput(int address, bool state, bool writeDirectly)
{
  // Check range
  if(address < 0){
    return false;
  }
  if(address >= pvDigitalOutputs.size()){
    return false;
  }
  pvDigitalOutputs[address] = state;
  /// \todo Implement query !
  if(writeDirectly){
    // Setup MODBUS PDU
    QByteArray pdu;
    int transactionId;
    pdu = pvCodec->encodeWriteSingleCoil(address, state);
    if(pdu.isEmpty()){
      return false;
    }
    // Send request
    transactionId = pvTcpPortManager->writeData(pdu);
    if(transactionId < 0){
      return false;
    }
    /// \note Provisoire !
    /**
    pdu = pvCodec->encodeReadCoils(address+0x0200, 5);
    if(!pvTcpPortManager->writeData(pdu, 2)){
      return false;
    }
    */
    pvTcpPortManager->waitReadenFrame();
    pdu = pvTcpPortManager->readenFrame(transactionId);
    qDebug() << "DEV RD reponse: " << pvCodec->decode(pdu);
    /**
    if(pvTcpPortManager->readenFrames().size() < 1){
      pvTcpPortManager->waitReadenFrame();
    }
    if(pvTcpPortManager->readenFrames().size() > 0){
      pdu = pvTcpPortManager->readenFrames().take(2);
      qDebug() << "DEV RD reponse: " << pvCodec->decode(pdu);
      qDebug() << "DEV RD states: " << pvCodec->values();
    }
    */
  }

  return true;
}

QVariant mdtDeviceModbus::getDigitalOutputState(int address, bool readDirectly)
{
  QVariant state;

  // Check range
  if(address < 0){
    return state;
  }
  if(address >= pvDigitalOutputs.size()){
    return state;
  }
  /// \todo Implement query !

  state = pvDigitalOutputs.value(address);

  return state;
}

bool mdtDeviceModbus::writeDigitalOutputs()
{
  // Setup MODBUS PDU
  QByteArray pdu;
  int transactionId;
  pdu = pvCodec->encodeWriteMultipleCoils(0, pvDigitalOutputs.values());
  if(pdu.isEmpty()){
    return false;
  }
  // Send request
  transactionId = pvTcpPortManager->writeData(pdu);
  if(transactionId < 0){
    return false;
  }
  /// \note Provisoire !
  pvTcpPortManager->waitReadenFrame();
  pdu = pvTcpPortManager->readenFrame(transactionId);
  qDebug() << "DEV RD reponse: " << pvCodec->decode(pdu);
  qDebug() << "DEV RD value: " << pvCodec->values();

  return true;
}

bool mdtDeviceModbus::readDigitalOutputs()
{
  // Setup MODBUS PDU
  QByteArray pdu;
  int transactionId;
  pdu = pvCodec->encodeReadCoils(0, pvDigitalOutputs.size());
  if(pdu.isEmpty()){
    return false;
  }
  // Send request
  transactionId = pvTcpPortManager->writeData(pdu);
  if(transactionId < 0){
    return false;
  }
  /// \note Provisoire !
  pvTcpPortManager->waitReadenFrame();
  pdu = pvTcpPortManager->readenFrame(transactionId);
  qDebug() << "DEV RD reponse: " << pvCodec->decode(pdu);
  qDebug() << "DEV RD value: " << pvCodec->values();

  return true;
}

void mdtDeviceModbus::decodeReadenFrames()
{
  Q_ASSERT(pvIos != 0);

  QByteArray pdu;
  int fc;
  qDebug() << "mdtDeviceModbus::decodeReadenFrames() ...";

  // Decode each readen frame and update I/O's
  pvReadenFrames = pvTcpPortManager->readenFrames();
  while(pvReadenFrames.size() > 0){
    pdu = pvReadenFrames.takeFirst();
    fc = pvCodec->decode(pdu);
    switch(fc){
      case 0x04:  // Read input registers
        pvIos->updateAnalogInputValues(pvCodec->values());
        break;
      default:
        /// \todo Handle errors !
        qDebug() << "mdtDeviceModbus::decodeReadenFrames() unknow frame format";
    }
  }
}
