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
#include "mdtError.h"
#include <QByteArray>
#include <QString>
#include <QHash>
#include <QList>

mdtDeviceModbus::mdtDeviceModbus(QObject *parent)
 : mdtDevice(parent)
{
  pvTcpPortManager = new mdtModbusTcpPortManager;
  pvCodec = new mdtFrameCodecModbus;
  /// \note Provisoire !!
  connect(pvTcpPortManager, SIGNAL(newReadenFrame(int, QByteArray)), this, SLOT(decodeReadenFrames(int, QByteArray)));
  pvTcpPortManager->setNotifyNewReadenFrame(true);
  pvTcpPortManager->setPortName("192.168.1.102:502");
  pvTcpPortManager->openPort();
  pvTcpPortManager->start();
  setStateReady();
}

mdtDeviceModbus::~mdtDeviceModbus()
{
  delete pvTcpPortManager;
  delete pvCodec;
}

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

  int retVal;
  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadInputRegisters(0, pvIos->analogInputsCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  retVal = pvTcpPortManager->writeData(pdu);
  if(retVal < 0){
    setStateFromPortError(retVal);
  }

  return retVal;
}

QVariant mdtDeviceModbus::getAnalogOutputValue(int address, bool readDirectly)
{
  QVariant value;

  // Check range
  if(address < 0){
    return value;
  }
  /**
  if(address >= pvAnalogOutputs.size()){
    return value;
  }
  */
  /// \todo Implement query !

  ///value = pvAnalogOutputs.value(address);

  return value;
}

/*
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
*/

int mdtDeviceModbus::readAnalogOutputs()
{
  Q_ASSERT(pvIos != 0);

  int retVal;
  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadHoldingRegisters(pvAnalogOutputAddressOffset, pvIos->analogOutputsCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  retVal = pvTcpPortManager->writeData(pdu);
  if(retVal < 0){
    setStateFromPortError(retVal);
  }

  return retVal;
}

int mdtDeviceModbus::writeAnalogOutputValue(int address, int value, int confirmationTimeout)
{
  Q_ASSERT(pvIos != 0);

  int transactionId;
  QByteArray pdu;
  mdtAnalogIo *ao;
  QVariant replyValue;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeWriteSingleRegister(address + pvAnalogOutputAddressOffset, value);
  if(pdu.isEmpty()){
    qDebug() << "mdtDeviceModbus::writeAnalogOutputValue(): encoding failed";
    return -1;
  }
  // Send request
  transactionId = pvTcpPortManager->writeData(pdu);
  if(transactionId < 0){
    setStateFromPortError(transactionId);
    qDebug() << "mdtDeviceModbus::writeAnalogOutputValue(): write fail";
    return transactionId;
  }
  // Add to transactions list
  ao = pvIos->analogOutputAt(address);
  if(ao == 0){
    mdtError e(MDT_DEVICE_ERROR, "No analog output assigned to address " + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
    e.commit();
    return -1;
  }
  ///pvAoPendingTransactions.insert(transactionId, ao);
  ao->setEnabled(false);
  addTransaction(transactionId, ao);
  if(!waitTransactionDone(transactionId, 500)){
    qDebug() << "mdtDeviceModbus::writeAnalogOutputValue(): wait failed";
    ao->setValue(0.0, false, false);
  }
  ao->setEnabled(true);
  /**
  qDebug() << "mdtDeviceModbus::writeAnalogOutputValue() request sent, TID: " << transactionId;
  int i = 10;
  while(pvAoPendingTransactions.contains(transactionId)){
    if(i <= 0){
      qDebug() << "mdtDeviceModbus::writeAnalogOutputValue(): timeout";
      pvAoPendingTransactions.remove(transactionId);
      setStateBusy(100);
    }
    pvTcpPortManager->wait(50);
    i--;
    qDebug() << "mdtDeviceModbus::writeAnalogOutputValue(): waiting , transactions queue: " << pvAoPendingTransactions.keys();
  }
  */
  qDebug() << "mdtDeviceModbus::writeAnalogOutputValue(): transaction DONE";

  return transactionId;
}

QVariant mdtDeviceModbus::getDigitalInputState(int address, bool readDirectly)
{
  QVariant state;

  // Check range
  if(address < 0){
    return state;
  }
  /**
  if(address >= pvDigitalInputs.size()){
    return state;
  }
  */
  /// \todo Implement query !

  ///state = pvDigitalInputs.value(address);

  return state;
}

bool mdtDeviceModbus::readDigitalInputs()
{
  // Setup MODBUS PDU
  QByteArray pdu;
  int transactionId;
  ///pdu = pvCodec->encodeReadDiscreteInputs(0, pvDigitalInputs.size());
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

bool mdtDeviceModbus::setDigitalOutput(int address, bool state, bool writeDirectly)
{
  // Check range
  if(address < 0){
    return false;
  }
  /**
  if(address >= pvDigitalOutputs.size()){
    return false;
  }
  */
  ///pvDigitalOutputs[address] = state;
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
  /**
  if(address >= pvDigitalOutputs.size()){
    return state;
  }
  */
  /// \todo Implement query !

  ///state = pvDigitalOutputs.value(address);

  return state;
}

bool mdtDeviceModbus::writeDigitalOutputs()
{
  // Setup MODBUS PDU
  QByteArray pdu;
  int transactionId;
  ///pdu = pvCodec->encodeWriteMultipleCoils(0, pvDigitalOutputs.values());
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
  ///pdu = pvCodec->encodeReadCoils(0, pvDigitalOutputs.size());
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

void mdtDeviceModbus::decodeReadenFrames(int id, QByteArray pdu)
{
  Q_ASSERT(pvIos != 0);

  int fc;
  mdtAnalogIo *ao;

  // Decode readen frame and update I/O's
  fc = pvCodec->decode(pdu);
  qDebug() << "mdtDeviceModbus::decodeReadenFrames(): RX frame TID " << id;
  switch(fc){
    case 0x03:  // Read holding registers
      pvIos->updateAnalogOutputValues(pvCodec->values());
      break;
    case 0x04:  // Read input registers
      pvIos->updateAnalogInputValues(pvCodec->values());
      break;
    case 0x06:  // Write single register reply
      // See if we have a pending analog output request
      ao = pendingAioTransaction(id);
      if(ao != 0){
      ///if(pvAoPendingTransactions.contains(id)){
        ///ao = pvAoPendingTransactions.take(id);
        ///Q_ASSERT(ao != 0);
        // Check validitiy
        if(pvCodec->values().size() != 2){
          mdtError e(MDT_DEVICE_ERROR, "Received unexptected count of values from device", mdtError::Error);
          MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
          e.commit();
          ao->setValue(0.0, false, false);
          break;
        }
        if(!pvCodec->values().at(1).isValid()){
          mdtError e(MDT_DEVICE_ERROR, "Received invalid value from device", mdtError::Error);
          MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
          e.commit();
          ao->setValue(0.0, false, false);
          break;
        }
        // Update (G)UI
        ao->setValueInt(pvCodec->values().at(1).toInt(), true, false);
        qDebug() << "mdtDeviceModbus::decodeReadenFrames(): decode done for TID " << id;
      }
      break;

    default:
      /// \todo Handle errors !
      mdtError e(MDT_DEVICE_ERROR, "Received frame with unhandled function code (0x" + QString::number(fc, 16) + ")", mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
      e.commit();
  }
}

bool mdtDeviceModbus::queriesSequence()
{
  if(readAnalogInputs() < 0){
    return false;
  }

  return true;
}
