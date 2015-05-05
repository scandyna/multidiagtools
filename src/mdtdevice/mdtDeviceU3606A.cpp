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

mdtValueDouble mdtDeviceU3606A::getMeasureValue(int timeout)
{
  mdtValueDouble x;
  QByteArray data;
  mdtCodecScpi codec;

  data = sendQuery("READ?\n", timeout);
  if(data.isEmpty()){
    return x;
  }
  x = codec.decodeValueDouble(data);
  if(x.isNull()){
    pvLastError = codec.lastError();
    return x;
  }

  return x;
}

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

bool mdtDeviceU3606A::setSourceRange(mdtDeviceU3606A::SourceRange_t range)
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
