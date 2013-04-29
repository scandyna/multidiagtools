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
  int timeout;

  pvTcpPortManager = new mdtModbusTcpPortManager;
  pvCodec = new mdtFrameCodecModbus;
  connect(pvTcpPortManager, SIGNAL(newReadenFrame(mdtPortTransaction)), this, SLOT(decodeReadenFrame(mdtPortTransaction)));
  ///connect(pvTcpPortManager, SIGNAL(errorStateChanged(int, const QString&, const QString&)), this, SLOT(setStateFromPortError(int, const QString&, const QString&)));
  connect(pvTcpPortManager, SIGNAL(stateChanged(int)), this, SLOT(setStateFromPortManager(int)));
  
  ///connect(pvTcpPortManager, SIGNAL(statusMessageChanged(const QString&, int)), this, SIGNAL(statusMessageChanged(const QString&, int)));
  connect(pvTcpPortManager, SIGNAL(statusMessageChanged(const QString&, const QString&, int)), this, SIGNAL(statusMessageChanged(const QString&, const QString&, int)));
  pvTcpPortManager->config().setReadTimeout(10000);
  timeout = pvTcpPortManager->config().readTimeout();
  if(pvTcpPortManager->config().writeTimeout() > timeout){
    timeout = pvTcpPortManager->config().writeTimeout();
  }
  setBackToReadyStateTimeout(2*timeout);
  /// \note Provisoire !!
  ///pvTcpPortManager->setPortName("192.168.1.110:502");
  ///pvTcpPortManager->setPortName("192.168.1.103:502");
  ///connect(this, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
  ///pvTcpPortManager->openPort();
  ///pvTcpPortManager->start();
  ///setStateReady();
}

mdtDeviceModbus::~mdtDeviceModbus()
{
  delete pvTcpPortManager;
  delete pvCodec;
}

mdtPortManager *mdtDeviceModbus::portManager()
{
  return pvTcpPortManager;
}

mdtModbusTcpPortManager *mdtDeviceModbus::modbusTcpPortManager()
{
  return pvTcpPortManager;
}

mdtAbstractPort::error_t mdtDeviceModbus::connectToDevice(const QList<mdtPortInfo*> &scanResult, int hardwareNodeId, int bitsCount, int startFrom)
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
    // We are connected here, get the hardware node ID
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

bool mdtDeviceModbus::getRegisterValues(int address, int n)
{
  return pvTcpPortManager->getRegisterValues(address, n);
  /**
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
    pvRegisterValues.append(pvCodec->values().at(i).toInt());
  }

  return true;
  */
}

const QList<int> &mdtDeviceModbus::registerValues() const
{
  return pvTcpPortManager->registerValues();
}

/// \todo Update to mdtValue
void mdtDeviceModbus::decodeReadenFrame(mdtPortTransaction transaction)
{
  ///Q_ASSERT(pvIos != 0);
  if(pvIos == 0){
    return;
  }

  int fc;

  // Decode readen frame and update I/O's
  fc = pvCodec->decode(transaction.data());
  switch(fc){
    case 0x01:  // Read coils
      // Check if we have just one or all outputs to update
      if(transaction.forMultipleIos()){
        pvIos->updateDigitalOutputStates(pvCodec->values());
      }else{
        // Update digital input if present and decoded value if ok
        if(transaction.digitalIo() != 0){
          if(pvCodec->values().size() == 1){
            ///transaction.digitalIo()->setOn(pvCodec->values().at(0), false);
            transaction.digitalIo()->setValue(pvCodec->values().at(0).toBool(), false);
          }else{
            ///transaction.digitalIo()->setOn(QVariant(), false);
            transaction.digitalIo()->setValue(mdtValue(), false);
          }
        }
      }
      break;
    case 0x02:  // Read discrete inputs
      ///qDebug() << "mdtDeviceModbus::decodeReadenFrames(): RX 0x02, read discrete inputs frame TID " << id << "\n-> Values: " << pvCodec->values();
      // Check if we have just one or all inputs to update
      if(transaction.forMultipleIos()){
        pvIos->updateDigitalInputStates(pvCodec->values());
      }else{
        // Update digital input if present and decoded value is ok
        if(transaction.digitalIo() != 0){
          if(pvCodec->values().size() == 1){
            ///transaction.digitalIo()->setOn(pvCodec->values().at(0), false);
            transaction.digitalIo()->setValue(pvCodec->values().at(0).toBool(), false);
          }else{
            ///transaction.digitalIo()->setOn(QVariant(), false);
            transaction.digitalIo()->setValue(mdtValue(), false);
          }
        }
      }
      break;
    case 0x03:  // Read holding registers
      // Check if we have just one or all outputs to update
      if(transaction.forMultipleIos()){
        pvIos->updateAnalogOutputValues(pvCodec->values());
      }else{
        // Update analog output if present and decoded value is ok
        if(transaction.analogIo() != 0){
          if(pvCodec->values().size() == 1){
            transaction.analogIo()->setValue(pvCodec->values().at(0).toDouble(), false);
          }else{
            ///transaction.analogIo()->setValue(QVariant(), false);
            transaction.analogIo()->setValue(mdtValue(), false);
          }
        }
      }
      break;
    case 0x04:  // Read input registers
      // Check if we have just one or all inputs to update
      if(transaction.forMultipleIos()){
        pvIos->updateAnalogInputValues(pvCodec->values());
      }else{
        // Update analog input if present and decoded value is Ok
        if(transaction.analogIo() != 0){
          if(pvCodec->values().size() == 1){
            transaction.analogIo()->setValue(pvCodec->values().at(0).toDouble(), false);
          }else{
            transaction.analogIo()->setValue(mdtValue(), false);
          }
        }
      }
      break;
    case 0x05:  // Write single coil reply
      if(transaction.digitalIo() != 0){
        // Check validitiy
        if(pvCodec->values().size() != 2){
          mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received unexptected count of states from device", mdtError::Error);
          MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
          e.commit();
          ///transaction.digitalIo()->setOn(QVariant(), false);
          transaction.digitalIo()->setValue(mdtValue(), false);
          break;
        }
        ///transaction.digitalIo()->setOn(pvCodec->values().at(1), false);
        transaction.digitalIo()->setValue(pvCodec->values().at(1).toBool(), false);
        transaction.digitalIo()->setEnabled(true);
      }
      break;
    case 0x06:  // Write single register reply
      if(transaction.analogIo() != 0){
        // Check validitiy
        if(pvCodec->values().size() != 2){
          mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received unexptected count of values from device", mdtError::Error);
          MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
          e.commit();
          ///transaction.analogIo()->setValue(0.0, false, false);
          transaction.analogIo()->setValue(mdtValue(), false);
          transaction.analogIo()->setEnabled(true);
          break;
        }
        if(!pvCodec->values().at(1).isValid()){
          mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received invalid value from device", mdtError::Error);
          MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
          e.commit();
          ///transaction.analogIo()->setValue(0.0, false, false);
          transaction.analogIo()->setValue(mdtValue(), false);
          transaction.analogIo()->setEnabled(true);
          break;
        }
        // Update (G)UI
        transaction.analogIo()->setValue(pvCodec->values().at(1).toDouble(), false);
        transaction.analogIo()->setEnabled(true);
      }
      break;
    case 0x10:  // Write multiple registers reply
      // Check validitiy
      if(pvCodec->values().size() != 2){
        mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received unexptected count of values from device", mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
        e.commit();
        pvIos->setAnalogOutputsValue(QVariant());
        break;
      }
      pvIos->setAnalogOutputsEnabled(true);
      break;
    default:
      /// \todo Handle errors !
      mdtError e(MDT_DEVICE_ERROR, "Received frame with unhandled function code (0x" + QString::number(fc, 16) + ")", mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
      e.commit();
  }
}

void mdtDeviceModbus::onStateChanged(int state)
{
  if(pvIos == 0){
    return;
  }
  if(state == Ready){
    if(getAnalogInputs(500) < 0){
      return;
    }
    if(getAnalogOutputs(500) < 0){
      return;
    }
    if(getDigitalInputs(500) < 0){
      return;
    }
    if(getDigitalOutputs(500) < 0){
      return;
    }
  }
}

bool mdtDeviceModbus::queriesSequence()
{
  if(getAnalogInputs(0) < 0){
    return false;
  }
  if(getDigitalInputs(0) < 0){
    return false;
  }

  return true;
}

int mdtDeviceModbus::readAnalogInput(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadInputRegisters(transaction->address(), 1);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu, transaction);
}

int mdtDeviceModbus::readAnalogInputs(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadInputRegisters(pvIos->analogInputsFirstAddress(), pvIos->analogInputsCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu, transaction);
}

int mdtDeviceModbus::readAnalogOutput(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  ///pdu = pvCodec->encodeReadHoldingRegisters(transaction->address() + pvAnalogOutputAddressOffset, 1);
  pdu = pvCodec->encodeReadHoldingRegisters(transaction->address(), 1);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu, transaction);
}

int mdtDeviceModbus::readAnalogOutputs(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  ///pdu = pvCodec->encodeReadHoldingRegisters(pvAnalogOutputAddressOffset, pvIos->analogOutputsCount());
  pdu = pvCodec->encodeReadHoldingRegisters(pvIos->analogOutputsFirstAddressRead(), pvIos->analogOutputsCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu, transaction);
}

int mdtDeviceModbus::writeAnalogOutput(int value, mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  ///pdu = pvCodec->encodeWriteSingleRegister(transaction->address() + pvAnalogOutputAddressOffset, value);
  pdu = pvCodec->encodeWriteSingleRegister(transaction->address(), value);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu, transaction);
}

int mdtDeviceModbus::writeAnalogOutputs(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  ///pdu = pvCodec->encodeWriteMultipleRegisters(pvAnalogOutputAddressOffset, pvIos->analogOutputsValuesInt());
  pdu = pvCodec->encodeWriteMultipleRegisters(pvIos->analogOutputsFirstAddressWrite(), pvIos->analogOutputsValuesIntByAddressWrite());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu, transaction);
}

int mdtDeviceModbus::readDigitalInput(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  ///pdu = pvCodec->encodeReadDiscreteInputs(transaction->address()+pvDigitalOutputAddressOffset, 1);
  pdu = pvCodec->encodeReadDiscreteInputs(transaction->address(), 1);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu, transaction);
}

int mdtDeviceModbus::readDigitalInputs(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  ///pdu = pvCodec->encodeReadDiscreteInputs(pvDigitalOutputAddressOffset, pvIos->digitalInputsCount());
  ///pdu = pvCodec->encodeReadDiscreteInputs(0, pvIos->digitalInputsCount());
  pdu = pvCodec->encodeReadDiscreteInputs(pvIos->digitalInputsFirstAddress(), pvIos->digitalInputsCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu, transaction);
}

int mdtDeviceModbus::readDigitalOutput(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  ///pdu = pvCodec->encodeReadCoils(transaction->address()+pvDigitalOutputAddressOffset, 1);
  pdu = pvCodec->encodeReadCoils(transaction->address(), 1);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu, transaction);
}

int mdtDeviceModbus::readDigitalOutputs(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  ///pdu = pvCodec->encodeReadCoils(pvDigitalOutputAddressOffset, pvIos->digitalOutputsCount());
  pdu = pvCodec->encodeReadCoils(pvIos->digitalOutputsFirstAddressRead(), pvIos->digitalOutputsCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu, transaction);
}

int mdtDeviceModbus::writeDigitalOutput(bool state, mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  ///pdu = pvCodec->encodeWriteSingleCoil(transaction->address()+pvDigitalOutputAddressOffset, state);
  pdu = pvCodec->encodeWriteSingleCoil(transaction->address(), state);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu, transaction);
}

int mdtDeviceModbus::writeDigitalOutputs(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  ///qDebug() << "mdtDeviceModbus::writeDigitalOutputs(): states: " << pvIos->digitalOutputsStates();
  ///pdu = pvCodec->encodeWriteMultipleCoils(pvDigitalOutputAddressOffset, pvIos->digitalOutputsStates());
  ///pdu = pvCodec->encodeWriteMultipleCoils(0, pvIos->digitalOutputsStates());
  pdu = pvCodec->encodeWriteMultipleCoils(pvIos->digitalOutputsFirstAddressWrite(), pvIos->digitalOutputsStatesByAddressWrite());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu, transaction);
}
