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
#include "mdtFrameCodecScpiU3606A.h"
#include "mdtError.h"
#include <cfloat>

#include <QDebug>

mdtFrameCodecScpiU3606A::mdtFrameCodecScpiU3606A()
{
  pvMeasureType = MT_UNKNOW;
}

bool mdtFrameCodecScpiU3606A::decodeConfigure(const QByteArray &data)
{
  QString str;
  QVariant value;

  // Clear previous results
  pvValues.clear();
  pvMeasureType = MT_UNKNOW;
  pvMeasureRange.clear();
  pvResolution.clear();

  // Extract <function> and <parameters>
  if(!decodeFunctionParameters(data)){
    return false;
  }
  Q_ASSERT(pvValues.size() > 0);
  Q_ASSERT(pvValues.at(0).type() == QVariant::String);
  // Decode <function>
  pvNodes = pvValues.at(0).toString().split(':', QString::SkipEmptyParts);
  if(pvNodes.size() < 1){
    mdtError e("Function part contains no data", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpiU3606A");
    e.commit();
    return false;
  }
  // Get "first level" type (voltage, current, resistance, ...) and set default sub type (DC, AC, ...)
  str = pvNodes.at(0);
  if(str.left(4) == "VOLT"){
    pvMeasureType = MT_VOLTAGE_DC;
  }else if(str.left(4) == "CURR"){
    pvMeasureType = MT_CURRENT_DC;
  }else if(str.left(3) == "RES"){
    pvMeasureType = MT_RESISTANCE;
  }else if(str.left(4) == "CONT"){
    pvMeasureType = MT_CONTINUITY;
  }else if(str.left(4) == "LRES"){
    pvMeasureType = MT_LRESISTANCE;
  }else if(str.left(3) == "CAP"){
    pvMeasureType = MT_CAPACITANCE;
  }else if(str.left(4) == "DIOD"){
    pvMeasureType = MT_DIODE;
  }else if(str.left(4) == "FREQ"){
    pvMeasureType = MT_FREQUENCY;
  }else if(str.left(4) == "PWID"){
    pvMeasureType = MT_PWIDTH;
  }else if(str.left(4) == "DCYC"){
    pvMeasureType = MT_DCYCLE;
  }else{
    pvMeasureType = MT_UNKNOW;
  }
  // Get optional sub types (AC)
  if(pvNodes.size() > 1){
    str = pvNodes.at(1);
    switch(pvMeasureType){
      case MT_VOLTAGE_DC:
        if(str.left(2) == "AC"){
          pvMeasureType = MT_VOLTAGE_AC;
        }
        break;
      case MT_CURRENT_DC:
        if(str.left(2) == "AC"){
          pvMeasureType = MT_CURRENT_AC;
        }
      default:
        // No other case to check
        break;
    }
  }
  // Get <parameters>
  if(pvValues.size() == 3){
    pvMeasureRange = pvValues.at(1).value<mdtValue>();
    pvResolution = pvValues.at(2).value<mdtValue>();
  }
  /**
  if(pvValues.size() == 3){
    value = pvValues.at(1);
    qDebug() << "mdtFrameCodecScpiU3606A::decodeConfigure() - value: " << value;
    if(value.convert(QVariant::Double)){
      pvMeasureRange = checkFloatingValueValidity(value.toDouble());
    }
    value = pvValues.at(2);
    if(value.convert(QVariant::Double)){
      pvResolution = checkFloatingValueValidity(value.toDouble());
    }
  }
  */

  return true;
}

mdtFrameCodecScpiU3606A::measure_type_t mdtFrameCodecScpiU3606A::measureType() const
{
  return pvMeasureType;
}

QString mdtFrameCodecScpiU3606A::measureUnit() const
{
  switch(pvMeasureType){
    case MT_UNKNOW:
      return "";
    case MT_VOLTAGE_DC:
    case MT_VOLTAGE_AC:
      return "V";
    case MT_CURRENT_DC:
    case MT_CURRENT_AC:
      return "A";
    case MT_RESISTANCE:
    case MT_CONTINUITY:
    case MT_LRESISTANCE:
      return "Ohm";
    case MT_CAPACITANCE:
      return "uF";
    case MT_DIODE:
      return "V";
    case MT_FREQUENCY:
      return "Hz";
    case MT_PWIDTH:
    case MT_DCYCLE:
      return "s";
  }
  return "";
}

mdtValue mdtFrameCodecScpiU3606A::measureRange() const
{
  return pvMeasureRange;
}

mdtValue mdtFrameCodecScpiU3606A::resolution() const
{
  return pvResolution;
}
