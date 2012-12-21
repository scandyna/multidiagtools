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
  int timeout;

  pvTcpPortManager = new mdtModbusTcpPortManager;
  pvCodec = new mdtFrameCodecModbus;
  ///connect(pvTcpPortManager, SIGNAL(newReadenFrame(int, QByteArray)), this, SLOT(decodeReadenFrame(int, QByteArray)));
  connect(pvTcpPortManager, SIGNAL(newReadenFrame(mdtPortTransaction)), this, SLOT(decodeReadenFrame(mdtPortTransaction)));
  connect(pvTcpPortManager, SIGNAL(errorStateChanged(int)), this, SLOT(setStateFromPortError(int)));
  timeout = pvTcpPortManager->config().readTimeout();
  if(pvTcpPortManager->config().writeTimeout() > timeout){
    timeout = pvTcpPortManager->config().writeTimeout();
  }
  setBackToReadyStateTimeout(2*timeout);
  ///pvTcpPortManager->setNotifyNewReadenFrame(true);
  /// \note Provisoire !!
  pvTcpPortManager->setPortName("192.168.1.102:502");
  ///pvTcpPortManager->setPortName("192.168.1.103:502");
  connect(this, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
  pvTcpPortManager->openPort();
  pvTcpPortManager->start();
  setStateReady();
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

void mdtDeviceModbus::decodeReadenFrame(int id, QByteArray pdu)
{
  Q_ASSERT(pvIos != 0);

  int fc;
  mdtAnalogIo *ai;
  mdtAnalogIo *ao;
  mdtDigitalIo *di;
  mdtDigitalIo *dout;

  // Decode readen frame and update I/O's
  fc = pvCodec->decode(pdu);
  qDebug() << "mdtDeviceModbus::decodeReadenFrames(id, data): RX frame TID " << id;
  switch(fc){
    case 0x01:  // Read coils
      dout = pendingDioTransaction(id);
      pendingIoTransaction(id);
      // Check if we have just one or all outputs to update
      if((dout != 0)&&(pvCodec->values().size() == 1)){
        dout->setOn(pvCodec->values().at(0), false);
      }else{
        pvIos->updateDigitalOutputStates(pvCodec->values());
      }
      break;
    case 0x02:  // Read discrete inputs
      ///qDebug() << "mdtDeviceModbus::decodeReadenFrames(): RX 0x02, read discrete inputs frame TID " << id << "\n-> Values: " << pvCodec->values();
      di = pendingDioTransaction(id);
      pendingIoTransaction(id);
      // Check if we have just one or all inputs to update
      if((di != 0)&&(pvCodec->values().size() == 1)){
        di->setOn(pvCodec->values().at(0), false);
      }else{
        pvIos->updateDigitalInputStates(pvCodec->values());
      }
      break;
    case 0x03:  // Read holding registers
      ao = pendingAioTransaction(id);
      pendingIoTransaction(id);
      // Check if we have just one or all outputs to update
      if((ao != 0)&&(pvCodec->values().size() == 1)){
        ao->setValue(pvCodec->values().at(0), false);
      }else{
        pvIos->updateAnalogOutputValues(pvCodec->values());
      }
      break;
    case 0x04:  // Read input registers
      ai = pendingAioTransaction(id);
      pendingIoTransaction(id);
      // Check if we have just one or all inputs to update
      if((ai != 0)&&(pvCodec->values().size() == 1)){
        ai->setValue(pvCodec->values().at(0), false);
      }else{
        pvIos->updateAnalogInputValues(pvCodec->values());
      }
      break;
    case 0x05:  // Write single coil reply
      dout = pendingDioTransaction(id);
      if(dout != 0){
        // Check validitiy
        if(pvCodec->values().size() != 2){
          mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received unexptected count of states from device", mdtError::Error);
          MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
          e.commit();
          dout->setOn(QVariant(), false);
          dout->setEnabled(true);
          break;
        }
        dout->setOn(pvCodec->values().at(1), false);
        dout->setEnabled(true);
      }
      break;
    case 0x06:  // Write single register reply
      // See if we have a pending analog output request
      ao = pendingAioTransaction(id);
      if(ao != 0){
        // Check validitiy
        if(pvCodec->values().size() != 2){
          mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received unexptected count of values from device", mdtError::Error);
          MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
          e.commit();
          ao->setValue(0.0, false, false);
          ao->setEnabled(true);
          break;
        }
        if(!pvCodec->values().at(1).isValid()){
          mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received invalid value from device", mdtError::Error);
          MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
          e.commit();
          ao->setValue(0.0, false, false);
          ao->setEnabled(true);
          break;
        }
        // Update (G)UI
        ao->setValue(pvCodec->values().at(1), false);
        ao->setEnabled(true);
      }
      break;
    case 0x10:  // Write multiple registers reply
      pendingIoTransaction(id);
      // Check validitiy
      if(pvCodec->values().size() != 2){
        mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received unexptected count of values from device", mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
        e.commit();
        pvIos->setAnalogOutputsValue(QVariant());
        break;
      }
      break;
    default:
      // Remove pending transactions
      pendingIoTransaction(id);
      pendingAioTransaction(id);
      pendingDioTransaction(id);
      /// \todo Handle errors !
      mdtError e(MDT_DEVICE_ERROR, "Received frame with unhandled function code (0x" + QString::number(fc, 16) + ")", mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
      e.commit();
  }
}

void mdtDeviceModbus::decodeReadenFrame(mdtPortTransaction transaction)
{
  Q_ASSERT(pvIos != 0);

  int fc;

  // Decode readen frame and update I/O's
  fc = pvCodec->decode(transaction.data());
  qDebug() << "mdtDeviceModbus::decodeReadenFrames(): RX frame TID " << transaction.id();
  switch(fc){
    case 0x01:  // Read coils
      // Check if we have just one or all outputs to update
      if(transaction.forMultipleIos()){
        pvIos->updateDigitalOutputStates(pvCodec->values());
      }else{
        // Update digital input if present and decoded value is ok
        if(transaction.digitalIo() != 0){
          if(pvCodec->values().size() == 1){
            transaction.digitalIo()->setOn(pvCodec->values().at(0), false);
          }else{
            transaction.digitalIo()->setOn(QVariant(), false);
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
            transaction.digitalIo()->setOn(pvCodec->values().at(0), false);
          }else{
            transaction.digitalIo()->setOn(QVariant(), false);
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
            transaction.analogIo()->setValue(pvCodec->values().at(0), false);
          }else{
            transaction.analogIo()->setValue(QVariant(), false);
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
            transaction.analogIo()->setValue(pvCodec->values().at(0), false);
          }else{
            transaction.analogIo()->setValue(QVariant(), false);
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
          transaction.digitalIo()->setOn(QVariant(), false);
          break;
        }
        transaction.digitalIo()->setOn(pvCodec->values().at(1), false);
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
          transaction.analogIo()->setValue(0.0, false, false);
          transaction.analogIo()->setEnabled(true);
          break;
        }
        if(!pvCodec->values().at(1).isValid()){
          mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received invalid value from device", mdtError::Error);
          MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
          e.commit();
          transaction.analogIo()->setValue(0.0, false, false);
          transaction.analogIo()->setEnabled(true);
          break;
        }
        // Update (G)UI
        transaction.analogIo()->setValue(pvCodec->values().at(1), false);
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

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadInputRegisters(0, pvIos->analogInputsCount());
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
  pdu = pvCodec->encodeReadHoldingRegisters(transaction->address() + pvAnalogOutputAddressOffset, 1);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu, transaction);
}

int mdtDeviceModbus::readAnalogOutputs(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadHoldingRegisters(pvAnalogOutputAddressOffset, pvIos->analogOutputsCount());
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
  pdu = pvCodec->encodeWriteSingleRegister(transaction->address() + pvAnalogOutputAddressOffset, value);
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
  pdu = pvCodec->encodeWriteMultipleRegisters(pvAnalogOutputAddressOffset, pvIos->analogOutputsValuesInt());
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
  pdu = pvCodec->encodeReadDiscreteInputs(transaction->address()+pvDigitalOutputAddressOffset, 1);
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
  pdu = pvCodec->encodeReadDiscreteInputs(0, pvIos->digitalInputsCount());
  ///pdu = pvCodec->encodeReadDiscreteInputs(0x0000, 16);
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
  pdu = pvCodec->encodeReadCoils(transaction->address()+pvDigitalOutputAddressOffset, 1);
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
  pdu = pvCodec->encodeReadCoils(pvDigitalOutputAddressOffset, pvIos->digitalOutputsCount());
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
  pdu = pvCodec->encodeWriteSingleCoil(transaction->address()+pvDigitalOutputAddressOffset, state);
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
  pdu = pvCodec->encodeWriteMultipleCoils(0, pvIos->digitalOutputsStates());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu, transaction);
}
