/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtDeviceU3606A.h"
#include "mdtError.h"
#include "mdtAnalogIo.h"
#include "mdtDeviceIos.h"
#include "mdtUsbtmcPortManager.h"
#include <QByteArray>
#include <QList>

#include <QDebug>

mdtDeviceU3606A::mdtDeviceU3606A(QObject *parent)
 : mdtDeviceScpi(parent)
{
  mdtAnalogIo *aio;

  pvCodec = new mdtFrameCodecScpiU3606A;
  // Setup port manager
  portManager()->config().setReadFrameSize(512);
  portManager()->config().setReadQueueSize(10);
  portManager()->config().setReadTimeout(30000);
  portManager()->config().setWriteFrameSize(512);
  portManager()->config().setWriteQueueSize(1);
  connect(pvUsbtmcPortManager, SIGNAL(newTransactionDone(mdtPortTransaction*)), this, SLOT(decodeReadenFrame(mdtPortTransaction*)));
  // Setup I/Os
  aio = new mdtAnalogIo;
  aio->setAddress(0);
  aio->setLabelShort("MEASURE");
  addInput(aio);
  aio = new mdtAnalogIo;
  aio->setAddress(1);
  aio->setLabelShort("SENSEU");
  addInput(aio);
  aio = new mdtAnalogIo;
  aio->setAddress(2);
  aio->setLabelShort("SENSEI");
  addInput(aio);
  aio = new mdtAnalogIo;
  aio->setAddress(0);
  aio->setLabelShort("SOURCE");
  addOutput(aio);
}

mdtDeviceU3606A::~mdtDeviceU3606A()
{
  delete pvCodec;
}

mdtAbstractPort::error_t mdtDeviceU3606A::connectToDevice(const mdtDeviceInfo &devInfo)
{
  mdtDeviceInfo device;

  // Setup device info
  device = devInfo;
  device.setVendorId(0x0957);
  device.setProductId(0x4d18);

  return mdtDeviceScpi::connectToDevice(device);
}

int mdtDeviceU3606A::setupVoltageDcMeasure ( mdtDeviceU3606A::range_t range, mdtDeviceU3606A::resolution_t resolution )
{
  QByteArray rangeStr;
  QByteArray resolutionStr;
  QByteArray command;
  int retVal;

  // Set range part
  switch(range){
    case RangeAuto:
      rangeStr = "AUTO";
      break;
    case RangeMin:
      rangeStr = "MIN";
      break;
    case RangeMax:
      rangeStr = "MAX";
      break;
    case Range20m:
      rangeStr = "0.02";
      break;
    case Range100m:
      rangeStr = "0.1";
      break;
    case Range1:
      rangeStr = "1";
      break;
    case Range10:
      rangeStr = "10";
      break;
    case Range100:
      rangeStr = "100";
      break;
    case Range1k:
      rangeStr = "1000";
      break;
    default:
      pvLastError.setError(tr("Requested range is not supported for DC voltage measurement, AUTO will be used."), mdtError::Warning);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceU3606A");
      pvLastError.commit();
      rangeStr = "AUTO";
  }
  // Set resolution part
  switch(resolution){
    case ResolutionMin:
      resolutionStr = "MIN";
      break;
    case ResolutionMax:
      resolutionStr = "MAX";
      break;
  }
  // Send command to device
  command = "CONF:VOLT:DC " + rangeStr + "," + resolutionStr;
  retVal = sendCommand(command);
  if(retVal < 0){
    pvLastError.setError(tr("Device") + " " + name() + ": " + tr("Command send failed"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceU3606A");
    pvLastError.commit();
  }

  return retVal;
}

int mdtDeviceU3606A::setupResistanceMeasure(mdtDeviceU3606A::range_t range, mdtDeviceU3606A::resolution_t resolution)
{
  QByteArray rangeStr;
  QByteArray resolutionStr;
  QByteArray command;
  int retVal;

  // Set range part
  switch(range){
    case RangeAuto:
      rangeStr = "AUTO";
      break;
    case RangeMin:
      rangeStr = "MIN";
      break;
    case RangeMax:
      rangeStr = "MAX";
      break;
    case Range100:
      rangeStr = "100";
      break;
    case Range1k:
      rangeStr = "1000";
      break;
    case Range10k:
      rangeStr = "10000";
      break;
    case Range100k:
      rangeStr = "100000";
      break;
    case Range1M:
      rangeStr = "1000000";
      break;
    case Range10M:
      rangeStr = "10000000";
      break;
    case Range100M:
      rangeStr = "100000000";
      break;
    default:
      pvLastError.setError(tr("Requested range is not supported for resistance measurement, AUTO will be used."), mdtError::Warning);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceU3606A");
      pvLastError.commit();
      rangeStr = "AUTO";
  }
  // Set resolution part
  switch(resolution){
    case ResolutionMin:
      resolutionStr = "MIN";
      break;
    case ResolutionMax:
      resolutionStr = "MAX";
      break;
  }
  // Send command to device
  command = "CONF:RES " + rangeStr + "," + resolutionStr;
  retVal = sendCommand(command);
  if(retVal < 0){
    pvLastError.setError(tr("Device") + " " + name() + ": " + tr("Command send failed"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceU3606A");
    pvLastError.commit();
  }

  return retVal;
}

int mdtDeviceU3606A::setupLowResistanceMeasure(mdtDeviceU3606A::range_t range, mdtDeviceU3606A::resolution_t resolution)
{
  QByteArray rangeStr;
  QByteArray resolutionStr;
  QByteArray command;
  int retVal;

  // Set range part
  switch(range){
    case RangeAuto:
      rangeStr = "AUTO";
      break;
    case RangeMin:
      rangeStr = "MIN";
      break;
    case RangeMax:
      rangeStr = "MAX";
      break;
    case Range100m:
      rangeStr = "0.1";
      break;
    case Range1:
      rangeStr = "1";
      break;
    default:
      pvLastError.setError(tr("Requested range is not supported for low resistance measurement, AUTO will be used."), mdtError::Warning);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceU3606A");
      pvLastError.commit();
      rangeStr = "AUTO";
  }
  // Set resolution part
  switch(resolution){
    case ResolutionMin:
      resolutionStr = "MIN";
      break;
    case ResolutionMax:
      resolutionStr = "MAX";
      break;
  }
  // Send command to device
  command = "CONF:LRES " + rangeStr + "," + resolutionStr;
  retVal = sendCommand(command);
  if(retVal < 0){
    pvLastError.setError(tr("Device") + " " + name() + ": " + tr("Command send failed"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceU3606A");
    pvLastError.commit();
  }

  return retVal;
}

mdtFrameCodecScpiU3606A::measure_type_t mdtDeviceU3606A::getMeasureConfiguration()
{
  int bTag;
  mdtPortTransaction *transaction;
  mdtUsbtmcPortManager *upm;

  // We need a specific port manager
  upm = dynamic_cast<mdtUsbtmcPortManager*>(portManager());
  Q_ASSERT(upm != 0);
  // Send query
  bTag = upm->sendData("CONF?");
  if(bTag < 0){
    return mdtFrameCodecScpiU3606A::MT_UNKNOW;
  }
  // Setup transaction
  transaction = getNewTransaction();
  transaction->setType(mdtFrameCodecScpi::QT_CONF);
  transaction->setQueryReplyMode(true);
  // Send read request - transaction is restored by sendReadRequest() on failure
  bTag = upm->sendReadRequest(transaction);
  if(bTag < 0){
    return mdtFrameCodecScpiU3606A::MT_UNKNOW;
  }
  // Wait on response - transaction are restored automatically during waitTransactionDone() on failure
  if(!waitTransactionDone(bTag)){
    return mdtFrameCodecScpiU3606A::MT_UNKNOW;
  }

  return pvCodec->measureType();
}

mdtValue mdtDeviceU3606A::getMeasureValue()
{
  return getAnalogInputValue(0, true, true);
}

bool mdtDeviceU3606A::setOutputState ( bool state )
{
  QByteArray cmd = "OUTP:STAT ";

  if(state){
    cmd += "1";
  }else{
    cmd += "0";
  }
  if(sendCommand(cmd) < 0){
    return false;
  }

  return true;
}

bool mdtDeviceU3606A::setSourceRange( mdtDeviceU3606A::sourceRange_t range )
{
  QByteArray cmd = "SOUR:VOLT:RANG ";

  switch(range){
    case S1_30V1A:
      cmd += "30";
      break;
    case S2_8V3A:
      cmd += "8";
      break;
  }
  if(sendCommand(cmd) < 0){
    return false;
  }

  return true;
}

bool mdtDeviceU3606A::setSourceVoltageLimit ( double v )
{
  QByteArray cmd = "SOUR:VOLT:LIM ";

  cmd += QByteArray::number(v);
  if(sendCommand(cmd) < 0){
    return false;
  }

  return true;
}

bool mdtDeviceU3606A::setSourceCurrent ( double x )
{
  QByteArray cmd = "SOUR:CURR ";

  cmd += QByteArray::number(x);
  if(sendCommand(cmd) < 0){
    return false;
  }

  return true;
}

void mdtDeviceU3606A::onStateChanged(int state)
{
  qDebug() << "mdtDeviceU3606A::onStateChanged() ...";
}

void mdtDeviceU3606A::decodeReadenFrame(mdtPortTransaction *transaction)
{
  Q_ASSERT(transaction != 0);

  bool ok;

  ///qDebug() << "mdtDeviceU3606A::decodeReadenFrame() , ID: " << transaction->id();

  switch(transaction->type()){
    case mdtFrameCodecScpi::QT_VALUE:
      if(transaction->analogIo() == 0){
        lastErrorW().setError(tr("Device ") + name() + tr(": received value response from device but no I/O was affected to query"), mdtError::Warning);
        MDT_ERROR_SET_SRC(lastErrorW(), "mdtDeviceU3606A");
        lastErrorW().commit();
      }else{
        ok = pvCodec->decodeValues(transaction->data());
        if(ok && (pvCodec->values().size() == 1)){
          transaction->analogIo()->setValue(pvCodec->values().at(0).value<mdtValue>(), false);
        }else{
          transaction->analogIo()->setValue(mdtValue(), false);
        }
      }
      break;
    case mdtFrameCodecScpi::QT_SINGLE_VALUE_FLT:
      if(transaction->analogIo() == 0){
        lastErrorW().setError(tr("Device ") + name() + tr(": received value response from device but no I/O was affected to query"), mdtError::Warning);
        MDT_ERROR_SET_SRC(lastErrorW(), "mdtDeviceU3606A");
        lastErrorW().commit();
      }else{
        qDebug() << "decodeReadenFrame() - data: " << transaction->data();
        transaction->analogIo()->setValue(pvCodec->decodeSingleValueDouble(transaction->data()), false);
        qDebug() << "decodeReadenFrame() - decoded data: " << pvCodec->decodeSingleValueDouble(transaction->data());
        qDebug() << "decodeReadenFrame() - decoded data: " << transaction->analogIo()->value().valueDouble();
      }
      break;
    case mdtFrameCodecScpi::QT_ERR:
      ok = pvCodec->decodeError(transaction->data());
      if(!ok){
        lastErrorW().setError(tr("Device ") + name() + tr(": SYST:ERR? reply could not be decoded"), mdtError::Warning);
        MDT_ERROR_SET_SRC(lastErrorW(), "mdtDeviceU3606A");
        lastErrorW().commit();
        break;
      }
      handleDeviceError(pvCodec->values());
      break;
    case mdtFrameCodecScpi::QT_CONF:
      if(!pvCodec->decodeConfigure(transaction->data())){
        lastErrorW().setError(tr("Device ") + name() + tr(": CONF? reply could not be decoded"), mdtError::Warning);
        MDT_ERROR_SET_SRC(lastErrorW(), "mdtDeviceU3606A");
        lastErrorW().commit();
        break;
      }
      updateMeasureSetup();
      break;
    default:
      lastErrorW().setError(tr("Device ") + name() + tr(": received unsupported response from device (type: ") + QString::number(transaction->type()) + tr(")"), mdtError::Warning);
      MDT_ERROR_SET_SRC(lastErrorW(), "mdtDeviceU3606A");
      lastErrorW().commit();
  }
}

int mdtDeviceU3606A::readAnalogInput(mdtPortTransaction *transaction)
{
  Q_ASSERT(ios() != 0);
  Q_ASSERT(transaction != 0);

  int bTag;
  mdtAnalogIo *aio;

  aio = transaction->analogIo();
  Q_ASSERT(aio != 0);
  switch(aio->addressRead()){
    case 0: // Measure
      transaction->setData("READ?");
      break;
    case 1: // Sense U (voltage)
      transaction->setData("SENS:VOLT?");
      break;
    case 2: // Sense I (current)
      transaction->setData("SENS:CURR?");
      break;
    default:
      lastErrorW().setError(tr("Requested to read a unknown analog input. Address: ") + QString::number(aio->addressRead()), mdtError::Error);
      MDT_ERROR_SET_SRC(lastErrorW(), "mdtDeviceU3606A");
      lastErrorW().commit();
      return mdtAbstractPort::UnhandledError;
  }
  // Send query
  bTag = pvUsbtmcPortManager->sendData(transaction);
  if(bTag < 0){
    return bTag;
  }
  // Remember query type.
  transaction->setType(mdtFrameCodecScpi::QT_SINGLE_VALUE_FLT);
  // Send read request
  bTag = pvUsbtmcPortManager->sendReadRequest(transaction);

  return bTag;
}

bool mdtDeviceU3606A::queriesSequence()
{
  getAnalogInputValue(0, true, true);

  return true;
}

void mdtDeviceU3606A::updateMeasureSetup()
{
  Q_ASSERT(ios() != 0);

  mdtAnalogIo *aio;

  // Get the measure input
  aio = ios()->analogInputAtAddress(0);
  Q_ASSERT(aio != 0);
  // Check validity and update
  if(pvCodec->measureRange().isValid()){
    aio->setRange(0.0, pvCodec->measureRange().valueDouble(), 32);
  }else{
    aio->setRange(0.0, 0.5, 32);
  }
  aio->setUnit(pvCodec->measureUnit());
}

/**
void mdtDeviceU3606A::handleDeviceSpecificError()
{
}
*/