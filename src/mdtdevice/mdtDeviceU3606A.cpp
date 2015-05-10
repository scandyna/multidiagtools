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
#include <QByteArray>
#include <QList>
#include <QTimer>

#include <QDebug>

mdtDeviceU3606A::mdtDeviceU3606A(QObject *parent)
 : mdtDeviceScpi(parent),
   pvContinousMeasurementActive(false)
{
}

mdtDeviceU3606A::~mdtDeviceU3606A()
{
}

bool mdtDeviceU3606A::reset(bool rst, bool cls)
{
  if(rst){
    if(!sendCommand("*RST\n")){
      return false;
    }
    wait(1);
  }
  if(cls){
    if(!sendCommand("*CLS\n")){
      return false;
    }
    wait(1);
  }
  return true;
}

bool mdtDeviceU3606A::setupVoltageDcMeasure(mdtDeviceU3606A::Range_t range, mdtDeviceU3606A::Resolution_t resolution)
{
  QByteArray rangeStr;
  QByteArray resolutionStr;
  QByteArray command;

  // Set range part
  switch(range){
    case Range_t::Auto:
      rangeStr = "AUTO";
      break;
    case Range_t::Min:
      rangeStr = "MIN";
      break;
    case Range_t::Max:
      rangeStr = "MAX";
      break;
    case Range_t::Range20m:
      rangeStr = "0.02";
      break;
    case Range_t::Range100m:
      rangeStr = "0.1";
      break;
    case Range_t::Range1:
      rangeStr = "1";
      break;
    case Range_t::Range10:
      rangeStr = "10";
      break;
    case Range_t::Range100:
      rangeStr = "100";
      break;
    case Range_t::Range1k:
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
    case Resolution_t::Min:
      resolutionStr = "MIN";
      break;
    case Resolution_t::Max:
      resolutionStr = "MAX";
      break;
  }
  // Send command to device
  command = "CONF:VOLT:DC " + rangeStr + "," + resolutionStr;
  return sendCommand(command);
}

bool mdtDeviceU3606A::setupVoltageAcMeasure(mdtDeviceU3606A::Range_t range, mdtDeviceU3606A::Resolution_t resolution)
{
  QByteArray rangeStr;
  QByteArray resolutionStr;
  QByteArray command;

  // Set range part
  switch(range){
    case Range_t::Auto:
      rangeStr = "AUTO";
      break;
    case Range_t::Min:
      rangeStr = "MIN";
      break;
    case Range_t::Max:
      rangeStr = "MAX";
      break;
    case Range_t::Range100m:
      rangeStr = "0.1";
      break;
    case Range_t::Range1:
      rangeStr = "1";
      break;
    case Range_t::Range10:
      rangeStr = "10";
      break;
    case Range_t::Range100:
      rangeStr = "100";
      break;
    case Range_t::Range750:
      rangeStr = "750";
      break;
    default:
      pvLastError.setError(deviceIdString() + tr("requested range is not supported for AC voltage measurement, AUTO will be used."), mdtError::Warning);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceU3606A");
      pvLastError.commit();
      rangeStr = "AUTO";
  }
  // Set resolution part
  switch(resolution){
    case Resolution_t::Min:
      resolutionStr = "MIN";
      break;
    case Resolution_t::Max:
      resolutionStr = "MAX";
      break;
  }
  // Send command to device
  command = "CONF:VOLT:AC " + rangeStr + "," + resolutionStr;
  return sendCommand(command);
}

bool mdtDeviceU3606A::setupResistanceMeasure(mdtDeviceU3606A::Range_t range, mdtDeviceU3606A::Resolution_t resolution)
{
  QByteArray rangeStr;
  QByteArray resolutionStr;
  QByteArray command;

  // Set range part
  switch(range){
    case Range_t::Auto:
      rangeStr = "AUTO";
      break;
    case Range_t::Min:
      rangeStr = "MIN";
      break;
    case Range_t::Max:
      rangeStr = "MAX";
      break;
    case Range_t::Range100:
      rangeStr = "100";
      break;
    case Range_t::Range1k:
      rangeStr = "1000";
      break;
    case Range_t::Range10k:
      rangeStr = "10000";
      break;
    case Range_t::Range100k:
      rangeStr = "100000";
      break;
    case Range_t::Range1M:
      rangeStr = "1000000";
      break;
    case Range_t::Range10M:
      rangeStr = "10000000";
      break;
    case Range_t::Range100M:
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
    case Resolution_t::Min:
      resolutionStr = "MIN";
      break;
    case Resolution_t::Max:
      resolutionStr = "MAX";
      break;
  }
  // Send command to device
  command = "CONF:RES " + rangeStr + "," + resolutionStr;
  return sendCommand(command);
}

bool mdtDeviceU3606A::setupLowResistanceMeasure(mdtDeviceU3606A::Range_t range, mdtDeviceU3606A::Resolution_t resolution)
{
  QByteArray rangeStr;
  QByteArray resolutionStr;
  QByteArray command;

  // Set range part
  switch(range){
    case Range_t::Auto:
      rangeStr = "AUTO";
      break;
    case Range_t::Min:
      rangeStr = "MIN";
      break;
    case Range_t::Max:
      rangeStr = "MAX";
      break;
    case Range_t::Range100m:
      rangeStr = "0.1";
      break;
    case Range_t::Range1:
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
    case Resolution_t::Min:
      resolutionStr = "MIN";
      break;
    case Resolution_t::Max:
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

// mdtValueDouble mdtDeviceU3606A::getMeasureValue(int timeout)
// {
//   mdtValueDouble x;
//   QByteArray data;
//   mdtCodecScpi codec;
// 
//   data = sendQuery("READ?\n", timeout);
//   if(data.isEmpty()){
//     return x;
//   }
//   x = codec.decodeValueDouble(data);
//   if(x.isNull()){
//     pvLastError = codec.lastError();
//     return x;
//   }
// 
//   return x;
// }

void mdtDeviceU3606A::startContinuousMeasurement(int interval)
{
  Q_ASSERT(interval >= 0);

  // Setup device in free-run mode
  if(!sendCommand("INIT:CONT 1\n")){
    return;
  }
  // Update flags and fetch device a first time
  pvContinousMeasurementActive = true;
  pvContinousMeasurementIntervall = interval;
  sendFetchQuery();
}

void mdtDeviceU3606A::stopContinuousMeasurement()
{
  pvContinousMeasurementActive = false;
  sendCommand("INIT:CONT 0\n");
}

bool mdtDeviceU3606A::setSourceOutputState(bool state)
{
  QByteArray cmd = "OUTP:STAT ";

  if(state){
    cmd += "1";
  }else{
    cmd += "0";
  }

  return sendCommand(cmd);
}

mdtValueBool mdtDeviceU3606A::getSourceOutputState()
{
  mdtValueBool state;
  QByteArray data;
  mdtCodecScpi codec;

  data = sendQuery("OUTP:STAT?\n");
  if(data.isEmpty()){
    return state;
  }
  state = codec.decodeValueBool(data);
  if(state.isNull()){
    pvLastError = codec.lastError();
    return state;
  }

  return state;
}

bool mdtDeviceU3606A::setSourceVoltageRange(mdtDeviceU3606A::SourceRange_t range)
{
  QByteArray cmd = "SOUR:VOLT:RANG ";

  switch(range){
    case SourceRange_t::S1_30V1A:
      cmd += "30";
      break;
    case SourceRange_t::S2_8V3A:
      cmd += "8";
      break;
  }

  return sendCommand(cmd);
}

bool mdtDeviceU3606A::setSourceCurrentRange(mdtDeviceU3606A::SourceRange_t range)
{
  QByteArray cmd = "SOUR:CURR:RANG ";

  switch(range){
    case SourceRange_t::S1_30V1A:
      cmd += "1";
      break;
    case SourceRange_t::S2_8V3A:
      cmd += "3";
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

// mdtValueDouble mdtDeviceU3606A::getSourceVoltageLimit()
// {
//   mdtValueDouble x;
//   QByteArray data;
//   mdtCodecScpi codec;
// 
//   data = sendQuery("SOUR:VOLT:LIM?\n");
//   if(data.isEmpty()){
//     return x;
//   }
//   x = codec.decodeValueDouble(data);
//   if(x.isNull()){
//     pvLastError = codec.lastError();
//     return x;
//   }
// 
//   return x;
// }

bool mdtDeviceU3606A::setSourceCurrentLimit(double x)
{
  QByteArray cmd = "SOUR:CURR:LIM ";

  cmd += QByteArray::number(x);

  return sendCommand(cmd);
}

// mdtValueDouble mdtDeviceU3606A::getSourceCurrentLimit()
// {
//   mdtValueDouble x;
//   QByteArray data;
//   mdtCodecScpi codec;
// 
//   data = sendQuery("SOUR:CURR:LIM?\n");
//   if(data.isEmpty()){
//     return x;
//   }
//   x = codec.decodeValueDouble(data);
//   if(x.isNull()){
//     pvLastError = codec.lastError();
//     return x;
//   }
// 
//   return x;
// }

bool mdtDeviceU3606A::setSourceVoltageProtection(double ovp)
{
  Q_ASSERT(mdtValueDouble(ovp) >= mdtValueDouble(0.0));
  Q_ASSERT(mdtValueDouble(ovp) <= mdtValueDouble(33.0));

  QByteArray cmd = "SOUR:VOLT:PROT ";

  cmd += QByteArray::number(ovp);

  return sendCommand(cmd);
}

// mdtValueDouble mdtDeviceU3606A::getSourceVoltageProtection()
// {
//   mdtValueDouble x;
//   QByteArray data;
//   mdtCodecScpi codec;
// 
//   data = sendQuery("SOUR:VOLT:PROT?\n");
//   if(data.isEmpty()){
//     return x;
//   }
//   x = codec.decodeValueDouble(data);
//   if(x.isNull()){
//     pvLastError = codec.lastError();
//     return x;
//   }
// 
//   return x;
// }

bool mdtDeviceU3606A::setSourceCurrentProtection(double ocp)
{
  Q_ASSERT(mdtValueDouble(ocp) >= mdtValueDouble(0.0));
  Q_ASSERT(mdtValueDouble(ocp) <= mdtValueDouble(3.3));

  QByteArray cmd = "SOUR:CURR:PROT ";

  cmd += QByteArray::number(ocp);

  return sendCommand(cmd);
}

bool mdtDeviceU3606A::setSourceVoltage(double x)
{
  QByteArray cmd = "SOUR:VOLT ";

  cmd += QByteArray::number(x);

  return sendCommand(cmd);
}

// mdtValueDouble mdtDeviceU3606A::getSourceSenseVoltage()
// {
//   mdtValueDouble x;
//   QByteArray data;
//   mdtCodecScpi codec;
// 
//   data = sendQuery("SENS:VOLT?\n");
//   if(data.isEmpty()){
//     return x;
//   }
//   x = codec.decodeValueDouble(data);
//   if(x.isNull()){
//     pvLastError = codec.lastError();
//     return x;
//   }
// 
//   return x;
// }

bool mdtDeviceU3606A::setSourceCurrent(double x)
{
  QByteArray cmd = "SOUR:CURR ";

  cmd += QByteArray::number(x);

  return sendCommand(cmd);
}

// mdtValueDouble mdtDeviceU3606A::getSourceSenseCurrent()
// {
//   mdtValueDouble x;
//   QByteArray data;
//   mdtCodecScpi codec;
// 
//   data = sendQuery("SENS:CURR?\n");
//   if(data.isEmpty()){
//     return x;
//   }
//   x = codec.decodeValueDouble(data);
//   if(x.isNull()){
//     pvLastError = codec.lastError();
//     return x;
//   }
// 
//   return x;
// }

bool mdtDeviceU3606A::setupSourceCurrent(const mdtValueDouble & current, const mdtValueDouble & voltageLimit, const mdtValueDouble & ovp)
{
  Q_ASSERT(!current.isNull());

  SourceRange_t range;

  // Disable source output (required for range setup)
  if(!setSourceOutputState(false)){
    return false;
  }
  wait(1);
  // Setup source range regarding current
  if(current <= 1.0){
    range = SourceRange_t::S1_30V1A;
  }else{
    range = SourceRange_t::S2_8V3A;
  }
  if(!setSourceCurrentRange(range)){
    return false;
  }
  wait(1);
  // Set voltage limit
  if(!voltageLimit.isNull()){
    if(!setSourceVoltageLimit(voltageLimit.value())){
      return false;
    }
    wait(1);
  }
  // Set OVP
  if(!ovp.isNull()){
    if(!setSourceVoltageProtection(ovp.value())){
      return false;
    }
    wait(1);
  }
  // Set current
  if(!setSourceCurrent(current.value())){
    return false;
  }
  wait(1);
  // We not re-enable source output

  return true;
}

bool mdtDeviceU3606A::setupSourceVoltage(const mdtValueDouble & voltage, const mdtValueDouble & currentLimit, const mdtValueDouble & ocp)
{
  Q_ASSERT(!voltage.isNull());

  SourceRange_t range;

  // Disable source output (required for range setup)
  if(!setSourceOutputState(false)){
    return false;
  }
  wait(1);
  // Setup source range regarding voltage
  if(voltage <= 8.0){
    range = SourceRange_t::S2_8V3A;
  }else{
    range = SourceRange_t::S1_30V1A;
  }
  if(!setSourceVoltageRange(range)){
    return false;
  }
  wait(1);
  // Set current limit
  if(!currentLimit.isNull()){
    if(!setSourceCurrentLimit(currentLimit.value())){
      return false;
    }
    wait(1);
  }
  // Set OCP
  if(!ocp.isNull()){
    if(!setSourceCurrentProtection(ocp.value())){
      return false;
    }
    wait(1);
  }
  // Set voltage
  if(!setSourceVoltage(voltage.value())){
    return false;
  }
  wait(1);
  // We not re-enable source output

  return true;
}

void mdtDeviceU3606A::sendFetchQuery()
{
  mdtValueDouble x;
  QByteArray data;
  mdtCodecScpi codec;

  // Check if continous mesurement is active
  if(!pvContinousMeasurementActive){
    return;
  }
  // Check if we are ready to process
  /**
  if(currentState() != State_t::Ready){
    stopContinuousMeasurement();
    return;
  }
  */
  // Query device
  data = sendQuery("FETC?\n", 30000);
  qDebug() << "****** FETC returned " << data;
  if(data.isEmpty()){
    stopContinuousMeasurement();
    return;
  }
  x = codec.decodeValueDouble(data);
  if(x.isNull()){
    pvLastError = codec.lastError();
    stopContinuousMeasurement();
    return;
  }
  // Signal new value
  emit measureValueReceived(x);
  // If we are active and ready, re-arm timer for next measurement
  /**
  if(currentState() != State_t::Ready){
    stopContinuousMeasurement();
    return;
  }
  */
  if(pvContinousMeasurementActive){
    QTimer::singleShot(pvContinousMeasurementIntervall, this, SLOT(sendFetchQuery()));
  }
}
