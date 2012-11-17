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
  connect(pvTcpPortManager, SIGNAL(newReadenFrame(int, QByteArray)), this, SLOT(decodeReadenFrame(int, QByteArray)));
  pvTcpPortManager->setNotifyNewReadenFrame(true);
  ///pvTcpPortManager->setPortName("192.168.1.102:502");
  pvTcpPortManager->setPortName("192.168.1.103:502");
  pvTcpPortManager->openPort();
  pvTcpPortManager->start();
  setStateReady();
}

mdtDeviceModbus::~mdtDeviceModbus()
{
  delete pvTcpPortManager;
  delete pvCodec;
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
  qDebug() << "mdtDeviceModbus::decodeReadenFrames(): RX frame TID " << id;
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
      qDebug() << "decodeReadenFrame: FC 5 ...";
      if(dout != 0){
        // Check validitiy
        if(pvCodec->values().size() != 2){
          mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received unexptected count of states from device", mdtError::Error);
          MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
          e.commit();
          dout->setOn(QVariant(), false);
          break;
        }
        qDebug() << "Pending FC 5 : " << pvCodec->values().at(1);
        dout->setOn(pvCodec->values().at(1), false);
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
          break;
        }
        if(!pvCodec->values().at(1).isValid()){
          mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received invalid value from device", mdtError::Error);
          MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
          e.commit();
          ao->setValue(0.0, false, false);
          break;
        }
        // Update (G)UI
        ao->setValue(pvCodec->values().at(1), false);
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
      /// \todo Handle errors !
      mdtError e(MDT_DEVICE_ERROR, "Received frame with unhandled function code (0x" + QString::number(fc, 16) + ")", mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtDeviceModbus");
      e.commit();
  }
}

bool mdtDeviceModbus::queriesSequence()
{
  if(getAnalogInputs(0) < 0){
    return false;
  }

  return true;
}

int mdtDeviceModbus::readAnalogInput(int address)
{
  Q_ASSERT(pvIos != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadInputRegisters(address, 1);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu);
}

int mdtDeviceModbus::readAnalogInputs()
{
  Q_ASSERT(pvIos != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadInputRegisters(0, pvIos->analogInputsCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu);
}

int mdtDeviceModbus::readAnalogOutput(int address)
{
  Q_ASSERT(pvIos != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadHoldingRegisters(address + pvAnalogOutputAddressOffset, 1);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu);
}

int mdtDeviceModbus::readAnalogOutputs()
{
  Q_ASSERT(pvIos != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadHoldingRegisters(pvAnalogOutputAddressOffset, pvIos->analogOutputsCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu);
}

int mdtDeviceModbus::writeAnalogOutput(int address, int value)
{
  Q_ASSERT(pvIos != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeWriteSingleRegister(address + pvAnalogOutputAddressOffset, value);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu);
}

int mdtDeviceModbus::writeAnalogOutputs()
{
  Q_ASSERT(pvIos != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeWriteMultipleRegisters(pvAnalogOutputAddressOffset, pvIos->analogOutputsValuesInt());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu);
}

int mdtDeviceModbus::readDigitalInput(int address)
{
  Q_ASSERT(pvIos != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadDiscreteInputs(address+pvDigitalOutputAddressOffset, 1);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu);
}

int mdtDeviceModbus::readDigitalInputs()
{
  Q_ASSERT(pvIos != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadDiscreteInputs(pvDigitalOutputAddressOffset, pvIos->digitalInputsCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu);
}

int mdtDeviceModbus::readDigitalOutput(int address)
{
  Q_ASSERT(pvIos != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadCoils(address+pvDigitalOutputAddressOffset, 1);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu);
}

int mdtDeviceModbus::readDigitalOutputs()
{
  Q_ASSERT(pvIos != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeReadCoils(pvDigitalOutputAddressOffset, pvIos->digitalInputsCount());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu);
}

int mdtDeviceModbus::writeDigitalOutput(int address, bool state)
{
  Q_ASSERT(pvIos != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeWriteSingleCoil(address+pvDigitalOutputAddressOffset, state);
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu);
}

int mdtDeviceModbus::writeDigitalOutputs()
{
  Q_ASSERT(pvIos != 0);

  QByteArray pdu;

  // Setup MODBUS PDU
  pdu = pvCodec->encodeWriteMultipleCoils(pvDigitalOutputAddressOffset, pvIos->digitalOutputsStates());
  if(pdu.isEmpty()){
    return -1;
  }
  // Send request
  return pvTcpPortManager->writeData(pdu);
}
