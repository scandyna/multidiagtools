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
#include "mdtCodecScpi.h"
#include "mdtAlgorithms.h"
#include <QObject>
#include <QList>
#include <QStringList>
#include <limits>

mdtScpiFunctionFarameter::FundamentalMeasurementCommand_t mdtScpiFunctionFarameter::fundamentalMeasurementCommand() const
{
  QList<QByteArray> nodes = functionRaw.split(':');

  if(nodes.isEmpty()){
    return FundamentalMeasurementCommand_t::Unknown;
  }
  QByteArray fmc = nodes.at(0).trimmed().toUpper();
  if((fmc == "VOLT")||(fmc == "VOLTAGE")){
    return FundamentalMeasurementCommand_t::Voltage;
  }
  if((fmc == "CURR")||(fmc == "CURRENT")){
    return FundamentalMeasurementCommand_t::Current;
  }
  if((fmc == "POW")||(fmc == "POWER")){
    return FundamentalMeasurementCommand_t::Current;
  }
  if((fmc == "RES")||(fmc == "RESISTANCE")){
    return FundamentalMeasurementCommand_t::Current;
  }
  if((fmc == "FRES")||(fmc == "FRESISTANCE")){
    return FundamentalMeasurementCommand_t::Current;
  }
  if((fmc == "TEMP")||(fmc == "TEMPERATURE")){
    return FundamentalMeasurementCommand_t::Current;
  }

  return FundamentalMeasurementCommand_t::Unknown;
}

mdtScpiFunctionFarameter::SimpleMeasurementCommand_t mdtScpiFunctionFarameter::simpleMeasurementCommand() const
{
  QList<QByteArray> nodes = functionRaw.split(':');
  QByteArray smc;

  if(nodes.isEmpty()){
    return SimpleMeasurementCommand_t::Unknown;
  }
  // DC is optionnal
  if(nodes.size() == 1){
    if(nodes.at(0).trimmed().isEmpty()){
      return SimpleMeasurementCommand_t::Unknown;
    }
    return SimpleMeasurementCommand_t::Dc;
  }
  Q_ASSERT(nodes.size() >= 2);
  smc = nodes.at(1).trimmed().toUpper();
  if(smc == "AC"){
    return SimpleMeasurementCommand_t::Ac;
  }
  if(smc == "DC"){
    return SimpleMeasurementCommand_t::Dc;
  }
  if((smc == "FREQ")||(smc == "FREQUENCY")){
    if(nodes.size() > 2){
      QByteArray ba = nodes.at(2).trimmed().toUpper();
      if((ba == "BURS")||(ba == "BURST")){
        return SimpleMeasurementCommand_t::FrequencyBurst;
      }
      if(ba == "PRF"){
        return SimpleMeasurementCommand_t::FrequencyPrf;
      }
    }else{
      return SimpleMeasurementCommand_t::Frequency;
    }
  }
  if((smc == "PER")||(smc == "PERIOD")){
    return SimpleMeasurementCommand_t::Period;
  }
  if((smc == "PHA")||(smc == "PHASE")){
    return SimpleMeasurementCommand_t::Phase;
  }

  return SimpleMeasurementCommand_t::Unknown;
}

mdtScpiIdnResponse mdtCodecScpi::decodeIdn(const QByteArray & data)
{
  mdtScpiIdnResponse r;
  QByteArray str = data.trimmed();

  QList<QByteArray> fields = str.split(',');
  if(fields.size() != 4){
    pvLastError.setError(QObject::tr("data") + " '" + data + "' " + QObject::tr("contains not a *IDN? response."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtCodecScpi");
    pvLastError.commit();
    return r;
  }
  r.manufacturer = fields.at(0);
  r.model = fields.at(1);
  r.serial = fields.at(2);
  r.firmwareLevel = fields.at(3);

  return r;
}

mdtValueBool mdtCodecScpi::decodeValueBool(const QByteArray & data)
{
  QByteArray str = data.trimmed();

  if(str == "0"){
    return false;
  }else if(str == "1"){
    return true;
  }else{
    pvLastError.setError(QObject::tr("data") + " '" + data + "' " + QObject::tr("contains not a boolean value."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtCodecScpi");
    pvLastError.commit();
    return mdtValueBool();
  }
}

mdtValueInt mdtCodecScpi::decodeValueInt(const QByteArray & data)
{
  QByteArray str = data.trimmed();
  int v;
  bool ok;

  if(str.size() > 2){
    if(str.left(2) == "0+"){
      str.remove(0, 2);
    }
  }
  v = str.toInt(&ok);
  if(!ok){
    pvLastError.setError(QObject::tr("data") + " '" + data + "' " + QObject::tr("contains not a integer value."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtCodecScpi");
    pvLastError.commit();
    return mdtValueInt();
  }

  return v;
}

mdtValueDouble mdtCodecScpi::decodeValueDouble(const QByteArray & data)
{
  mdtValueDouble x, y;
  QByteArray str = data.trimmed();
  bool ok;

  // Remove 0+ prefix, if present
  if(str.size() > 2){
    if(str.left(2) == "0+"){
      str.remove(0, 2);
    }
  }
  // Convert
  x = str.toDouble(&ok);
  if(!ok){
    pvLastError.setError(QObject::tr("data") + " '" + data + "' " + QObject::tr("contains not a decimal value."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtCodecScpi");
    pvLastError.commit();
    return mdtValueDouble();
  }
  // Check about NaN
  y.setValue(9.91e37);
  if(x == y){
    x.setValue(std::numeric_limits<double>::quiet_NaN());
    return x;
  }
  // Check about OL flag
  y.setValue(9.9e37);
  if(x >= y){
    x.setValue(9.9e37, true);
    return x;
  }
  y.setValue(-9.9e37);
  if(x <= y){
    x.setValue(-9.9e37, true);
    return x;
  }

  return x;
}

mdtError mdtCodecScpi::decodeDeviceError(const QByteArray& data, const QString & errorText)
{
  mdtError error;
  QByteArray str = data.trimmed();
  int code;
  bool ok;

  // Set text
  if(errorText.isEmpty()){
    error.setError(QObject::tr("Device returned a error"), mdtError::Error);
  }else{
    error.setError(errorText, mdtError::Error);
  }
  // Parse error
  QStringList fields = mdtAlgorithms::splitString(str, ",", "\"");
  if(fields.size() != 2){
    pvLastError.setError(QObject::tr("data") + " '" + data + "' " + QObject::tr("contains not a error."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtCodecScpi");
    pvLastError.commit();
    return mdtError();
  }
  code = fields.at(0).toInt(&ok);
  if(!ok){
    pvLastError.setError(QObject::tr("data") + " '" + data + "' " + QObject::tr("contains not a valid error (code is not a number)."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtCodecScpi");
    pvLastError.commit();
    return mdtError();
  }
  // Fill error
  auto sysError = mdtErrorNewT(int, code, fields.at(1).trimmed(), mdtError::Error, "mdtCodecScpi");
  error.stackError(sysError);

  return error;
}

mdtScpiFunctionFarameter mdtCodecScpi::decodeFunctionParameters(const QByteArray& data)
{
  mdtScpiFunctionFarameter fp;
  QByteArray str = data.trimmed();
  int idx;

  if(str.isEmpty()){
    pvLastError.setError(QObject::tr("data") + " '" + data + "' " + QObject::tr("contains not a valid <function> <parameter> (is empty)."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtCodecScpi");
    pvLastError.commit();
    return fp;
  }
  // Search first space
  idx = str.indexOf(' ');
  if(idx >= 0){
    fp.functionRaw = str.left(idx + 1).trimmed();
    fp.parametersRaw = str.mid(idx).trimmed();
  }else{
    fp.functionRaw = str;
  }
  // Decode possible parameters as double values
  QList<QByteArray> nodes = fp.parametersRaw.split(',');
  mdtValueDouble x;
  for(auto & item : nodes){
    x = decodeValueDouble(item.trimmed());  /// \todo Will generate errors in log if we have channels list (@a, @b, ...)
    if(x.isNull()){
      break;
    }
    fp.parametersDouble.append(x.value());
  }

  return fp;
}
