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
#include "mdtDeviceAddress.h"
#include "mdtModbusHwNodeId.h"
#include "mdtModbusTcpPortManager.h"
#include "mdtFrameCodecModbus.h"
#include "mdtDeviceIosSegment.h"
#include "mdtError.h"
#include <QByteArray>
#include <QString>
#include <QHash>
#include <QList>

/// \todo Provisoire !
///#include "mdtAlgorithms.h"

mdtDeviceModbus::mdtDeviceModbus(QObject *parent)
 : mdtMultiIoDevice(parent)
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

bool mdtDeviceModbus::connectToDevice()
{
  bool hasCompleteAddress;
  mdtDeviceAddress da = deviceAddress();

  // Check that port manager is not running
  if(!pvTcpPortManager->isClosed()){
    pvTcpPortManager->stop();
  }
  // Check that device address string refers to a TCPIP port
  if(deviceAddress().portType() != mdtDeviceAddress::PortType_t::TCPIP){
    pvLastError.setError(tr("Cannot connect to device with address string '") + deviceAddress().addressString() + \
                         tr("': port type '") + deviceAddress().portTypeStr() + tr("' not supported."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbus");
    pvLastError.commit();
    return false;
  }
  // Check that device address string refers to a MODBUS device
  if(deviceAddress().deviceType() != mdtDeviceAddress::DeviceType_t::MODBUS){
    pvLastError.setError(tr("Cannot connect to device with address string '") + deviceAddress().addressString() + \
                         tr("': device type '") + deviceAddress().deviceTypeStr() + tr("' not supported."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbus");
    pvLastError.commit();
    return false;
  }
  /*
   * Check if a complete device address was given.
   * If true, we have to get MODBUS HW node ID from device
   * and check that it matches expected one.
   * For other cases:
   *  - No specific MODBUS HW node ID is expected
   *  - Only MODBUS HW node ID is given, in witch case the check is done during seraching
   */
  hasCompleteAddress = ( (!da.tcpIpHostName().isEmpty()) && (da.tcpIpPort() > 0) && (!da.modbusHwNodeId().isNull()) );
  // If host and port was not given, search by MODBUS HW node ID
  if( (da.tcpIpHostName().isEmpty()) || (da.tcpIpPort() == 0) ){
    // Check that we not have a completely empty address
    if(da.modbusHwNodeId().isNull()){
      pvLastError.setError(tr("Cannot connect to device with address string '") + da.addressString() + \
                           tr("': at least host and port, or MODBUS hardware node ID is required."), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbus");
      pvLastError.commit();
      return false;
    }
    // Search device with expected MODBUS HW node ID
    auto foundDa = searchDeviceWithModbusHwNodeId(da.modbusHwNodeId(), 100, 502); /// \todo Fix scan timeout and port problem
    if(!foundDa.isValid()){
      return false;
    }
    pvTcpPortManager->setPortName(foundDa.tcpIpHostName() + ":" + QString::number(foundDa.tcpIpPort()) ); /// \todo Update once port management was fixed..
  }else{
    pvTcpPortManager->setPortName(da.tcpIpHostName() + ":" + QString::number(da.tcpIpPort()) ); /// \todo Update once port management was fixed..
  }
  // Connect to device
  if(!pvTcpPortManager->start()){
    pvLastError = pvTcpPortManager->lastError();
    return false;
  }
  // Check MODBUS HW node ID if required
  if(hasCompleteAddress){
    mdtModbusHwNodeId hwNodeId = da.modbusHwNodeId();
    mdtModbusHwNodeId deviceHwNodeId = pvTcpPortManager->getHardwareNodeAddress(hwNodeId.bitsCount(), hwNodeId.firstBit());
    if(deviceHwNodeId.isNull()){
      pvLastError = pvTcpPortManager->lastError();
      disconnectFromDevice();
      return false;
    }
    if(deviceHwNodeId.id() != hwNodeId.id()){
      pvLastError.setError(deviceIdString() + tr(" device's MODBUS hardware node ID does not match requested one. ") + \
                           tr("Requested: ") + QString::number(hwNodeId.id()) + \
                           tr(" , device:")  + QString::number(deviceHwNodeId.id()), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbus");
      pvLastError.commit();
      disconnectFromDevice();
      return false;
    }
  }

  return true;
}

bool mdtDeviceModbus::connectToDevice(const mdtModbusHwNodeId & hwNodeId, const QString & alias, int scanTimeout, int port)
{
//   mdtDeviceAddressList daList;
//   mdtModbusHwNodeIdList hwNodeIdList;

  // Check that port manager is not running
  if(!pvTcpPortManager->isClosed()){
    pvTcpPortManager->stop();
  }
  // Search device
  auto da = searchDeviceWithModbusHwNodeId(hwNodeId, scanTimeout, port);
  if(!da.isValid()){
    return false;
  }
  // Set device address and connect
  setDeviceAddress(da, alias);

  return connectToDevice();

//   // Get list of known devices that are reachable and check if node with requested HW ID exists
//   daList = pvTcpPortManager->scanFromKnownHostsFile(scanTimeout);
//   for(const auto & da : daList.internalVector()){
//     if(da.modbusHwNodeId().id() == hwNodeId.id()){
//       Q_ASSERT(!da.tcpIpHostName().isEmpty());  // Prevent recursiv calls
//       setDeviceAddress(da, alias);
//       return connectToDevice();
//     }
//   }
//   // Nothing found in cache, scan the network
//   hwNodeIdList.append(hwNodeId);
//   daList = pvTcpPortManager->scan(QNetworkInterface::allInterfaces(), hwNodeIdList, port, true, scanTimeout);
//   if(daList.isEmpty()){
//     pvLastError.setError(tr("Could not find a MODBUS/TCP device with hardware node ID ") + QString::number(hwNodeId.id()), mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbus");
//     pvLastError.commit();
//     return false;
//   }
//   Q_ASSERT(daList.size() > 0);
//   /*
//    * Save device addresses to known hosts cache file
//    * We ignore return value, if file cannot be saved,
//    * it's not a problem for all the rest
//    */
//   pvTcpPortManager->saveDeviceAddressList(daList);
//   // Should be ok , connect
//   Q_ASSERT(!daList.at(0).tcpIpHostName().isEmpty());  // Prevent recursiv calls
//   setDeviceAddress(daList.at(0), alias);
// 
//   return connectToDevice();
}

bool mdtDeviceModbus::detectIos()
{
  pvLastError.setError(deviceIdString() + tr("I/O detection not implemented in this base MODBUS class."), mdtError::Error);
  MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbus");
  pvLastError.commit();
  return false;
}

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

mdtDeviceAddress mdtDeviceModbus::searchDeviceWithModbusHwNodeId(const mdtModbusHwNodeId& hwNodeId, int scanTimeout, int port)
{
  Q_ASSERT(!hwNodeId.isNull());
  Q_ASSERT(pvTcpPortManager->isClosed());

  mdtDeviceAddressList daList;
  mdtModbusHwNodeIdList hwNodeIdList;

  // Get list of known devices that are reachable and check if node with requested HW ID exists
  daList = pvTcpPortManager->scanFromKnownHostsFile(scanTimeout);
  for(const auto & da : daList.internalVector()){
    if(da.modbusHwNodeId().id() == hwNodeId.id()){
      return da;
    }
  }
  // Nothing found in cache, scan the network
  hwNodeIdList.append(hwNodeId);
  daList = pvTcpPortManager->scan(QNetworkInterface::allInterfaces(), hwNodeIdList, port, true, scanTimeout);
  if(daList.isEmpty()){
    pvLastError.setError(tr("Could not find a MODBUS/TCP device with hardware node ID ") + QString::number(hwNodeId.id()), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceModbus");
    pvLastError.commit();
    return mdtDeviceAddress();
  }
  Q_ASSERT(daList.size() > 0);
  /*
   * Save device addresses to known hosts cache file
   * We ignore return value, if file cannot be saved,
   * it's not a problem for all the rest
   */
  pvTcpPortManager->saveDeviceAddressList(daList);

  return daList.at(0);
}

void mdtDeviceModbus::disconnectFromDeviceEvent()
{
  pvTcpPortManager->stop();
}
