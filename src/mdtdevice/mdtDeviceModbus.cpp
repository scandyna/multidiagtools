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
#include "mdtDeviceModbus.h"
#include "mdtModbusTcpPortManager.h"
#include "mdtFrameCodecModbus.h"
#include "mdtDeviceIosSegment.h"
#include "mdtError.h"
#include <QByteArray>
#include <QString>
#include <QHash>
#include <QList>

/// \todo Provisoire !
#include "mdtAlgorithms.h"

mdtDeviceModbus::mdtDeviceModbus(QObject *parent)
 : mdtMultiIoDevice(parent)
{
  int timeout;

  /**
  pvHardwareNodeIdBitsCount = 0;
  pvHardwareNodeIdBitsStartFrom = 0;
  */
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

/**
void mdtDeviceModbus::setHardwareNodeId(int hwNodeId, int bitsCount, int startFrom)
{
  pvHardwareNodeId = hwNodeId;
  pvHardwareNodeIdBitsCount = bitsCount;
  pvHardwareNodeIdBitsStartFrom = startFrom;
}
*/

/**
void mdtDeviceModbus::clearHardwareNodeId()
{
  pvHardwareNodeId.clear();
  pvHardwareNodeIdBitsCount = 0;
  pvHardwareNodeIdBitsStartFrom = 0;
}

QVariant mdtDeviceModbus::hardwareNodeId() const
{
  return pvHardwareNodeId;
}
*/

// mdtAbstractPort::error_t mdtDeviceModbus::connectToDevice(const mdtPortInfo & portInfo)
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
//   return mdtAbstractPort::NoError;
// }

/**
mdtAbstractPort::error_t mdtDeviceModbus::connectToDevice(const mdtDeviceInfo &devInfo)
{
  QList<mdtPortInfo*> portInfoList;
  int i;

  // Check that port manager is not running
  if(!pvTcpPortManager->isClosed()){
    pvTcpPortManager->stop();
  }
  // Scan looking in chache file first
  ///portInfoList = pvTcpPortManager->scan(pvTcpPortManager->readScanResult());
  for(i = 0; i < portInfoList.size(); ++i){
    Q_ASSERT(portInfoList.at(i) != 0);
    // Try to connect
    if(connectToDevice(*portInfoList.at(i)) == mdtAbstractPort::NoError){
      qDeleteAll(portInfoList);
      return mdtAbstractPort::NoError;
    }
    pvTcpPortManager->stop();
  }
  qDeleteAll(portInfoList);
  portInfoList.clear();
  pvTcpPortManager->stop();
  // Scan network
  ///portInfoList = pvTcpPortManager->scan(QNetworkInterface::allInterfaces(), 502, 100);
  for(i=0; i<portInfoList.size(); i++){
    Q_ASSERT(portInfoList.at(i) != 0);
    // Try to connect
    if(connectToDevice(*portInfoList.at(i)) == mdtAbstractPort::NoError){
      pvTcpPortManager->saveScanResult(portInfoList);
      qDeleteAll(portInfoList);
      return mdtAbstractPort::NoError;
    }
    pvTcpPortManager->stop();
  }
  qDeleteAll(portInfoList);
  pvTcpPortManager->stop();

  return mdtAbstractPort::PortNotFound;
}
*/

// mdtAbstractPort::error_t mdtDeviceModbus::connectToDevice(const QList<mdtPortInfo*> &scanResult, int hardwareNodeId, int bitsCount, int startFrom)
// {
//   int i;
//   mdtAbstractPort::error_t portError;
// 
//   // Check that port manager is not running
//   if(!pvTcpPortManager->isClosed()){
//     pvTcpPortManager->stop();
//   }
//   // Search device with requested hardwareNodeId
//   for(i=0; i<scanResult.size(); i++){
//     Q_ASSERT(scanResult.at(i) != 0);
//     // Try to connect
//     portError = connectToDevice(*scanResult.at(i));
//     if(portError != mdtAbstractPort::NoError){
//       continue;
//     }
//     /**
//     pvTcpPortManager->setPortInfo(*scanResult.at(i));
//     if(!pvTcpPortManager->start()){
//       continue;
//     }
//     */
//     // We are connected here, get the hardware node ID
//     if(pvTcpPortManager->getHardwareNodeAddress(bitsCount, startFrom) == hardwareNodeId){
//       return mdtAbstractPort::NoError;
//     }else{
//       pvTcpPortManager->stop();
//       ///continue;
//     }
//   }
// 
//   return mdtAbstractPort::PortNotFound;
// }

// mdtAbstractPort::error_t mdtDeviceModbus::connectToDevice(const QList<int> & existingHwNodeIdList)
// {
//   QList<int> _existingHwNodeIdList;
//   QList<mdtPortInfo*> portInfoList;
//   mdtAbstractPort::error_t portError;
// 
//   // Check that a hardware node id was set
//   /**
//   if(!pvHardwareNodeId.isValid()){
//     pvLastError.setError(tr("Hardware node ID was not set."), mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbus");
//     pvLastError.commit();
//     return mdtAbstractPort::SetupError;
//   }
//   */
//   // Setup _existingHwNodeIdList
//   /**
//   _existingHwNodeIdList = existingHwNodeIdList;
//   if(!_existingHwNodeIdList.contains(pvHardwareNodeId.toInt())){
//     _existingHwNodeIdList.append(pvHardwareNodeId.toInt());
//   }
//   */
//   // Check that port manager is not running
//   if(!pvTcpPortManager->isClosed()){
//     pvTcpPortManager->stop();
//   }
//   // Scan looking in chache file first
//   ///portInfoList = pvTcpPortManager->scan(pvTcpPortManager->readScanResult(), 100, _existingHwNodeIdList, pvHardwareNodeIdBitsCount, pvHardwareNodeIdBitsStartFrom);
//   // Try to connect
//   ///portError = connectToDevice(portInfoList, pvHardwareNodeId.toInt(), pvHardwareNodeIdBitsCount, pvHardwareNodeIdBitsStartFrom);
//   qDeleteAll(portInfoList);
//   portInfoList.clear();
//   if(portError == mdtAbstractPort::NoError){
//     return mdtAbstractPort::NoError;
//   }
//   // Expected device not found, scan the network
//   ///portInfoList = pvTcpPortManager->scan(QNetworkInterface::allInterfaces(), 502, 100, true, _existingHwNodeIdList, pvHardwareNodeIdBitsCount, pvHardwareNodeIdBitsStartFrom);
//   // Try to connect
//   ///portError = connectToDevice(portInfoList, pvHardwareNodeId.toInt(), pvHardwareNodeIdBitsCount, pvHardwareNodeIdBitsStartFrom);
//   if(portError == mdtAbstractPort::NoError){
//     pvTcpPortManager->saveScanResult(portInfoList);
//   }
//   qDeleteAll(portInfoList);
//   portInfoList.clear();
// 
//   return portError;
// }

/**
mdtAbstractPort::error_t mdtDeviceModbus::connectToDevice()
{
  qDebug() << "mdtDeviceModbus::connectToDevice() (without argument) is not implemented yet, sorry :-/";
  return mdtAbstractPort::UnhandledError;
}
*/

bool mdtDeviceModbus::getRegisterValues(int address, int n)
{
  return pvTcpPortManager->getRegisterValues(address, n);
}

const QList<int> &mdtDeviceModbus::registerValues() const
{
  return pvTcpPortManager->registerValues();
}

bool mdtDeviceModbus::setRegisterValues(int startAddress, QList<int> &values)
{
  return pvTcpPortManager->setRegisterValues(startAddress, values);
}

void mdtDeviceModbus::decodeReadenFrame(mdtPortTransaction *transaction)
{
  Q_ASSERT(transaction != 0);
  if(ios() == 0){
    return;
  }

  int fc;
  QVariant var;

  /// \todo 1) Add a flag for special register data in mdtPortTransaction, 2) Don't update I/O if this flag is set
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
        ios()->updateDigitalOutputValues(pvCodec->values(), transaction->address(), mdtDeviceIos::Read , transaction->ioCount(), true);
        ios()->setDigitalOutputsEnabled(true);
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
        ios()->updateDigitalInputValues(pvCodec->values(), transaction->address(), transaction->ioCount(), true);
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
        ios()->updateAnalogOutputValues(pvCodec->values(), transaction->address(), mdtDeviceIos::Read, transaction->ioCount(), true);
        ios()->setAnalogOutputsEnabled(true);
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
        ios()->updateAnalogInputValues(pvCodec->values(), transaction->address(), transaction->ioCount(), true);
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
            ios()->setDigitalOutputsValue(mdtValue());
          }
        }else{
          ios()->setDigitalOutputsValue(mdtValue());
        }
      }else{
        ios()->setDigitalOutputsValue(mdtValue());
      }
      ios()->setDigitalOutputsEnabled(true);
    case 0x10:  // Write multiple registers reply
      // Check validitiy and update
      if(pvCodec->values().size() == 2){
        // Start address
        var = pvCodec->values().at(0);
        if((var.isValid())&&(var.type() == QVariant::Int)&&(var.toInt() == transaction->address())){
          // Qty of outputs set
          var = pvCodec->values().at(1);
          if((!var.isValid())||(var.type() != QVariant::Int)||(var.toInt() != transaction->ioCount())){
            ios()->setAnalogOutputsValue(mdtValue());
          }
        }else{
          ios()->setAnalogOutputsValue(mdtValue());
        }
      }else{
        ios()->setAnalogOutputsValue(mdtValue());
      }
      ios()->setAnalogOutputsEnabled(true);
      break;
    default:
      /// \todo Handle errors !
      pvLastError.setError("Received frame with unhandled function code (0x" + QString::number(fc, 16) + ")", mdtError::Warning);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbus");
      pvLastError.commit();
  }
}

void mdtDeviceModbus::setStateFromPortManager ( int portManagerState )
{
  switch((mdtPortManager::state_t)portManagerState){
    case mdtPortManager::PortClosed:
      ///setStatePortClosed();
      setCurrentState(State_t::Disconnected);
      break;
    case mdtPortManager::Disconnected:
      ///setStateDisconnected();
      setCurrentState(State_t::Disconnected);
      break;
    case mdtPortManager::Connecting:
      ///setStateConnecting();
      setCurrentState(State_t::Connecting);
      break;
    case mdtPortManager::Ready:
      ///setStateReady();
      setCurrentState(State_t::Ready);
      break;
    case mdtPortManager::Busy:
      //setStateBusy();
      //break;
    case mdtPortManager::PortError:
      ///setStateError();
      setCurrentState(State_t::Error);
      break;
    case mdtPortManager::Stopped:
    case mdtPortManager::Starting:
    case mdtPortManager::Stopping:
    case mdtPortManager::Running:
    case mdtPortManager::PortReady:
    case mdtPortManager::Connected:
      ///emit stateChanged(portManagerState);
      setCurrentState(State_t::Error);
      break;
  }
}

bool mdtDeviceModbus::queriesSequence()
{
  ///qDebug() << "mdtDeviceModbus::queriesSequence() ...";
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
  Q_ASSERT(ios() != 0);
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
  Q_ASSERT(ios() != 0);
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
  Q_ASSERT(ios() != 0);
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
  Q_ASSERT(ios() != 0);
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
  Q_ASSERT(ios() != 0);
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

int mdtDeviceModbus::writeAnalogOutputs(mdtPortTransaction *transaction, mdtDeviceIosSegment *segment)
{
  Q_ASSERT(ios() != 0);
  Q_ASSERT(transaction != 0);
  Q_ASSERT(segment != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  ///pdu = pvCodec->encodeWriteMultipleRegisters(transaction->address(), ios()->analogOutputsValuesIntByAddressWrite());
  pdu = pvCodec->encodeWriteMultipleRegisters(segment->startAddressWrite(), segment->valuesInt());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  transaction->setData(pdu);
  return pvTcpPortManager->sendData(transaction);
}

int mdtDeviceModbus::readDigitalInput(mdtPortTransaction *transaction)
{
  Q_ASSERT(ios() != 0);
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
  Q_ASSERT(ios() != 0);
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
  Q_ASSERT(ios() != 0);
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
  Q_ASSERT(ios() != 0);
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
  Q_ASSERT(ios() != 0);
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

int mdtDeviceModbus::writeDigitalOutputs(mdtPortTransaction *transaction, mdtDeviceIosSegment *segment)
{
  Q_ASSERT(ios() != 0);
  Q_ASSERT(transaction != 0);
  Q_ASSERT(segment != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  ///pdu = pvCodec->encodeWriteMultipleCoils(transaction->address(), ios()->digitalOutputsStatesByAddressWrite());
  pdu = pvCodec->encodeWriteMultipleCoils(segment->startAddressWrite(), segment->valuesBool());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  transaction->setData(pdu);
  return pvTcpPortManager->sendData(transaction);
}

void mdtDeviceModbus::disconnectFromDeviceEvent()
{
  pvTcpPortManager->stop();
}
