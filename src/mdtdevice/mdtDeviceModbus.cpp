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
  connect(pvTcpPortManager, SIGNAL(newTransactionDone(mdtPortTransaction*)), this, SLOT(decodeReadenFrame(mdtPortTransaction*)));
  connect(pvTcpPortManager, SIGNAL(stateChanged(int)), this, SLOT(setStateFromPortManager(int)));
  connect(pvTcpPortManager, SIGNAL(statusMessageChanged(const QString&, const QString&, int)), this, SIGNAL(statusMessageChanged(const QString&, const QString&, int)));
  timeout = pvTcpPortManager->config().readTimeout();
  if(pvTcpPortManager->config().writeTimeout() > timeout){
    timeout = pvTcpPortManager->config().writeTimeout();
  }
  setBackToReadyStateTimeout(2*timeout);
  pvTcpPortManager->notifyCurrentState();
}

mdtDeviceModbus::~mdtDeviceModbus()
{
  qDebug() << "mdtDeviceModbus::~mdtDeviceModbus() ...";
  pvTcpPortManager->stop();
  delete pvTcpPortManager;
  delete pvCodec;
  qDebug() << "mdtDeviceModbus::~mdtDeviceModbus() DONE";
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
  Q_ASSERT(pvTcpPortManager->isClosed());

  int i;

  for(i=0; i<scanResult.size(); i++){
    Q_ASSERT(scanResult.at(i) != 0);
    // Try to connect
    pvTcpPortManager->setPortInfo(*scanResult.at(i));
    if(!pvTcpPortManager->start()){
      continue;
    }
    // We are connected here, get the hardware node ID
    if(pvTcpPortManager->getHardwareNodeAddress(bitsCount, startFrom) == hardwareNodeId){
      return mdtAbstractPort::NoError;
    }else{
      pvTcpPortManager->stop();
      continue;
    }
  }

  return mdtAbstractPort::PortNotFound;
}

bool mdtDeviceModbus::getRegisterValues(int address, int n)
{
  return pvTcpPortManager->getRegisterValues(address, n);
}

const QList<int> &mdtDeviceModbus::registerValues() const
{
  return pvTcpPortManager->registerValues();
}

void mdtDeviceModbus::decodeReadenFrame(mdtPortTransaction *transaction)
{
  Q_ASSERT(transaction != 0);
  if(pvIos == 0){
    return;
  }

  int fc;
  QVariant var;

  // Decode readen frame and update I/O's
  fc = pvCodec->decode(transaction->data());
  switch(fc){
    case 0x01:  // Read coils
      // Check if we have a answer for a single I/O or multiple I/Os
      if((transaction->ioCount() == 1)&&(transaction->digitalIo() != 0)){
        // If one DO was requested, 1 byte is returned with just first bit set
        if(pvCodec->values().size() == 8){
          var = pvCodec->values().at(0);
          if((var.isValid())&&(var.type() == QVariant::Bool)){
            transaction->digitalIo()->setValue(var.toBool(), false);
          }else{
            transaction->digitalIo()->setValue(mdtValue(), false);
          }
        }else{
          transaction->digitalIo()->setValue(mdtValue(), false);
        }
        transaction->digitalIo()->setEnabled(true);
      }else{
        // Transaction contains first I/O address (for read access) and qty of I/Os
        pvIos->updateDigitalOutputValues(pvCodec->values(), transaction->address(), transaction->ioCount());
        pvIos->setDigitalOutputsEnabled(true);
      }
      break;
    case 0x02:  // Read discrete inputs
      // Check if we have a answer for a single I/O or multiple I/Os
      if((transaction->ioCount() == 1)&&(transaction->digitalIo() != 0)){
        // If one DI was requested, 1 byte is returned with just first bit set
        if(pvCodec->values().size() == 8){
          var = pvCodec->values().at(0);
          if((var.isValid())&&(var.type() == QVariant::Bool)){
            transaction->digitalIo()->setValue(var.toBool(), false);
          }else{
            transaction->digitalIo()->setValue(mdtValue(), false);
          }
        }else{
          transaction->digitalIo()->setValue(mdtValue(), false);
        }
      }else{
        // Transaction contains first I/O address and qty of I/Os
        pvIos->updateDigitalInputValues(pvCodec->values(), transaction->address(), transaction->ioCount());
      }
      break;
    case 0x03:  // Read holding registers
      // Check if we have a answer for a single I/O or multiple I/Os
      if((transaction->ioCount() == 1)&&(transaction->analogIo() != 0)){
        // Single I/O reply
        if(pvCodec->values().size() == 1){
          var = pvCodec->values().at(0);
          if((var.isValid())&&(var.type() == QVariant::Int)){
            // Codec only get raw data, so let I/O do needed conversions
            transaction->analogIo()->setValue(var.toInt(), false);
          }else{
            transaction->analogIo()->setValue(mdtValue(), false);
          }
        }else{
          transaction->analogIo()->setValue(mdtValue(), false);
        }
        transaction->analogIo()->setEnabled(true);
      }else{
        // Multiple I/O reply
        pvIos->updateAnalogOutputValues(pvCodec->values(), transaction->address(), transaction->ioCount());
        pvIos->setAnalogOutputsEnabled(true);
      }
      break;
    case 0x04:  // Read input registers
      // Check if we have a answer for a single I/O or multiple I/Os
      if((transaction->ioCount() == 1)&&(transaction->analogIo() != 0)){
        // Single I/O reply
        if(pvCodec->values().size() == 1){
          var = pvCodec->values().at(0);
          if((var.isValid())&&(var.type() == QVariant::Int)){
            // Codec only get raw data, so let I/O do needed conversions
            transaction->analogIo()->setValue(var.toInt(), false);
          }else{
            transaction->analogIo()->setValue(mdtValue(), false);
          }
        }else{
          transaction->analogIo()->setValue(mdtValue(), false);
        }
      }else{
        // Multiple I/O reply
        pvIos->updateAnalogInputValues(pvCodec->values(), transaction->address(), transaction->ioCount());
      }
      break;
    case 0x05:  // Write single coil reply
      if(transaction->digitalIo() != 0){
        // Check validity and update
        if(pvCodec->values().size() == 2){
          // Address
          var = pvCodec->values().at(0);
          if((var.isValid())&&(var.type() == QVariant::Int)&&(var.toInt() == transaction->digitalIo()->addressWrite())){
            // Value
            var = pvCodec->values().at(1);
            if((var.isValid())&&(var.type() == QVariant::Bool)){
              transaction->digitalIo()->setValue(var.toBool(), false);
            }else{
              transaction->digitalIo()->setValue(mdtValue(), false);
            }
          }else{
            transaction->digitalIo()->setValue(mdtValue(), false);
          }
        }else{
          transaction->digitalIo()->setValue(mdtValue(), false);
        }
        transaction->digitalIo()->setEnabled(true);
      }
      break;
    case 0x06:  // Write single register reply
      if(transaction->analogIo() != 0){
        // Check validity and update
        if(pvCodec->values().size() == 2){
          // Address
          var = pvCodec->values().at(0);
          if((var.isValid())&&(var.type() == QVariant::Int)&&(var.toInt() == transaction->analogIo()->addressWrite())){
            // Value
            var = pvCodec->values().at(1);
            if((var.isValid())&&(var.type() == QVariant::Int)){
              // Update (G)UI - Codec only get raw data, so let I/O do needed conversions
              transaction->analogIo()->setValue(var.toInt(), false);
            }
          }else{
            transaction->analogIo()->setValue(mdtValue(), false);
          }
        }else{
          transaction->analogIo()->setValue(mdtValue(), false);
        }
        transaction->analogIo()->setEnabled(true);
      }
      break;
    case 0x0F:  // Write multiple coils
      // Check validitiy and update
      if(pvCodec->values().size() == 2){
        // Start address
        var = pvCodec->values().at(0);
        if((var.isValid())&&(var.type() == QVariant::Int)&&(var.toInt() == transaction->address())){
          // Qty of outputs set
          var = pvCodec->values().at(1);
          if((!var.isValid())||(var.type() != QVariant::Int)||(var.toInt() != transaction->ioCount())){
            pvIos->setDigitalOutputsValue(mdtValue());
          }
        }else{
          pvIos->setDigitalOutputsValue(mdtValue());
        }
      }else{
        pvIos->setDigitalOutputsValue(mdtValue());
      }
      pvIos->setDigitalOutputsEnabled(true);
    case 0x10:  // Write multiple registers reply
      // Check validitiy and update
      if(pvCodec->values().size() == 2){
        // Start address
        var = pvCodec->values().at(0);
        if((var.isValid())&&(var.type() == QVariant::Int)&&(var.toInt() == transaction->address())){
          // Qty of outputs set
          var = pvCodec->values().at(1);
          if((!var.isValid())||(var.type() != QVariant::Int)||(var.toInt() != transaction->ioCount())){
            pvIos->setAnalogOutputsValue(mdtValue());
          }
        }else{
          pvIos->setAnalogOutputsValue(mdtValue());
        }
      }else{
        pvIos->setAnalogOutputsValue(mdtValue());
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

bool mdtDeviceModbus::queriesSequence()
{
  if(getAnalogInputs(true) < 0){
    return false;
  }
  if(getDigitalInputs(true) < 0){
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
  transaction->setData(pdu);
  return pvTcpPortManager->sendData(transaction);
}

int mdtDeviceModbus::readAnalogInputs(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadInputRegisters(transaction->address(), transaction->ioCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  transaction->setData(pdu);
  return pvTcpPortManager->sendData(transaction);
}

int mdtDeviceModbus::readAnalogOutput(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadHoldingRegisters(transaction->address(), 1);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  transaction->setData(pdu);
  return pvTcpPortManager->sendData(transaction);
}

int mdtDeviceModbus::readAnalogOutputs(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadHoldingRegisters(transaction->address(), transaction->ioCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  transaction->setData(pdu);
  return pvTcpPortManager->sendData(transaction);
}

int mdtDeviceModbus::writeAnalogOutput(int value, mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeWriteSingleRegister(transaction->address(), value);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  transaction->setData(pdu);
  return pvTcpPortManager->sendData(transaction);
}

int mdtDeviceModbus::writeAnalogOutputs(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeWriteMultipleRegisters(transaction->address(), pvIos->analogOutputsValuesIntByAddressWrite());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  transaction->setData(pdu);
  return pvTcpPortManager->sendData(transaction);
}

int mdtDeviceModbus::readDigitalInput(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadDiscreteInputs(transaction->address(), 1);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  transaction->setData(pdu);
  return pvTcpPortManager->sendData(transaction);
}

int mdtDeviceModbus::readDigitalInputs(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadDiscreteInputs(transaction->address(), transaction->ioCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  transaction->setData(pdu);
  return pvTcpPortManager->sendData(transaction);
}

int mdtDeviceModbus::readDigitalOutput(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadCoils(transaction->address(), 1);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  transaction->setData(pdu);
  return pvTcpPortManager->sendData(transaction);
}

int mdtDeviceModbus::readDigitalOutputs(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadCoils(transaction->address(), transaction->ioCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  transaction->setData(pdu);
  return pvTcpPortManager->sendData(transaction);
}

int mdtDeviceModbus::writeDigitalOutput(bool state, mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeWriteSingleCoil(transaction->address(), state);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  transaction->setData(pdu);
  return pvTcpPortManager->sendData(transaction);
}

int mdtDeviceModbus::writeDigitalOutputs(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeWriteMultipleCoils(transaction->address(), pvIos->digitalOutputsStatesByAddressWrite());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  transaction->setData(pdu);
  return pvTcpPortManager->sendData(transaction);
}
