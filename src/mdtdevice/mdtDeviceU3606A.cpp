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
#include "mdtDeviceU3606A.h"
#include "mdtError.h"
#include "mdtAnalogIo.h"
#include "mdtDeviceIos.h"
#include <QByteArray>
#include <QList>

#include <QDebug>

mdtDeviceU3606A::mdtDeviceU3606A(QObject *parent)
 : mdtDeviceScpi(parent)
{
  ///pvCodec = new mdtFrameCodecScpiU3606A;  /// \todo Use new codec
}

mdtDeviceU3606A::~mdtDeviceU3606A()
{
  ///delete pvCodec;
}

bool mdtDeviceU3606A::setupVoltageDcMeasure(mdtDeviceU3606A::range_t range, mdtDeviceU3606A::resolution_t resolution)
{
  QByteArray rangeStr;
  QByteArray resolutionStr;
  QByteArray command;

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
      pvLastError.setError(deviceIdString() + tr("requested range is not supported for DC voltage measurement, AUTO will be used."), mdtError::Warning);
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
  return sendCommand(command);
}

bool mdtDeviceU3606A::setupResistanceMeasure(mdtDeviceU3606A::range_t range, mdtDeviceU3606A::resolution_t resolution)
{
  QByteArray rangeStr;
  QByteArray resolutionStr;
  QByteArray command;

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
      pvLastError.setError(deviceIdString() + tr("requested range is not supported for resistance measurement, AUTO will be used."), mdtError::Warning);
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
  return sendCommand(command);
}

bool mdtDeviceU3606A::setupLowResistanceMeasure(mdtDeviceU3606A::range_t range, mdtDeviceU3606A::resolution_t resolution)
{
  QByteArray rangeStr;
  QByteArray resolutionStr;
  QByteArray command;

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
      pvLastError.setError(deviceIdString() + tr("Requested range is not supported for low resistance measurement, AUTO will be used."), mdtError::Warning);
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
  return sendCommand(command);
}

mdtDeviceU3606A::MeasureType_t mdtDeviceU3606A::getMeasureConfiguration()
{
  QByteArray data;
  mdtCodecScpi codec;
  mdtScpiFunctionFarameter fp;

  // Get configuration from device
  data = sendQuery("CONF?\n");
  if(data.isEmpty()){
    return MeasureType_t::Unknown;
  }
  // Deduce current device configuration
  fp = codec.decodeFunctionParameters(data);
  switch(fp.fundamentalMeasurementCommand()){
    case mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Unknown:
      return MeasureType_t::Unknown;
    case mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Voltage:
      switch(fp.simpleMeasurementCommand()){
        case mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Dc:
          return MeasureType_t::VoltageDc;
        case mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Ac:
          return MeasureType_t::VoltageAc;
        default:
          return MeasureType_t::Unknown;
      }
      break;
    case mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Current:
      switch(fp.simpleMeasurementCommand()){
        case mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Dc:
          return MeasureType_t::CurrentDc;
        case mdtScpiFunctionFarameter::SimpleMeasurementCommand_t::Ac:
          return MeasureType_t::CurrentAc;
        default:
          return MeasureType_t::Unknown;
      }
      break;
    case mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Resistance:
      return MeasureType_t::Resistance;
    case mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t::Fresistance:
      return MeasureType_t::Lresistance;
  }
}


// mdtFrameCodecScpiU3606A::measure_type_t mdtDeviceU3606A::getMeasureConfiguration()
// {
//   int bTag;
//   mdtPortTransaction *transaction;
//   mdtUsbtmcPortManager *upm;
// 
//   // We need a specific port manager
//   upm = dynamic_cast<mdtUsbtmcPortManager*>(portManager());
//   Q_ASSERT(upm != 0);
//   // Send query
//   bTag = upm->sendData("CONF?");
//   if(bTag < 0){
//     return mdtFrameCodecScpiU3606A::MT_UNKNOW;
//   }
//   // Setup transaction
//   transaction = getNewTransaction();
//   transaction->setType(mdtFrameCodecScpi::QT_CONF);
//   transaction->setQueryReplyMode(true);
//   // Send read request - transaction is restored by sendReadRequest() on failure
//   bTag = upm->sendReadRequest(transaction);
//   if(bTag < 0){
//     return mdtFrameCodecScpiU3606A::MT_UNKNOW;
//   }
//   // Wait on response - transaction are restored automatically during waitTransactionDone() on failure
//   if(!waitTransactionDone(bTag)){
//     return mdtFrameCodecScpiU3606A::MT_UNKNOW;
//   }
// 
//   return pvCodec->measureType();
// }

mdtValue mdtDeviceU3606A::getMeasureValue()
{
  ///return getAnalogInputValue(0, true, true);
}

bool mdtDeviceU3606A::setOutputState(bool state)
{
  QByteArray cmd = "OUTP:STAT ";

  if(state){
    cmd += "1";
  }else{
    cmd += "0";
  }

  return sendCommand(cmd);
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

  return sendCommand(cmd);
}

bool mdtDeviceU3606A::setSourceVoltageLimit(double v)
{
  QByteArray cmd = "SOUR:VOLT:LIM ";

  cmd += QByteArray::number(v);

  return sendCommand(cmd);
}

bool mdtDeviceU3606A::setSourceCurrent(double x)
{
  QByteArray cmd = "SOUR:CURR ";

  cmd += QByteArray::number(x);

  return sendCommand(cmd);
}

// void mdtDeviceU3606A::onStateChanged(int state)
// {
//   qDebug() << "mdtDeviceU3606A::onStateChanged() ...";
// }
