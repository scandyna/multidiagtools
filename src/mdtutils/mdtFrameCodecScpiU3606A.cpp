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
#include "mdtFrameCodecScpiU3606A.h"
#include "mdtError.h"
#include <cfloat>

mdtFrameCodecScpiU3606A::mdtFrameCodecScpiU3606A()
{
  pvMeasureType = MT_UNKNOW;
}

/**
bool mdtFrameCodecScpiU3606A::decodeConfFunc(QByteArray &frame)
{
  QVariant value;
  QString str;
  QStringList nodeValues;
  int i;

  // Clear previous results
  pvValues.clear();
  pvMeasureType = MT_UNKNOW;
  pvRange.clear();
  pvResolution.clear();

  // Store raw data in local QString constainer
  pvAsciiData = frame;

  // Remove white spaces at begin and end
  trim();
  // remove EofSeq
  if(!removeEofSeq()){
    return false;
  }

  // Case of no data
  if(pvAsciiData.size() < 1){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Frame contains no data" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpiU3606A");
    e.commit();
    return false;
  }

  // Build the nodes "tree"
  pvNodes = pvAsciiData.split(":");

  // If we have 1 node, it can be a measure type with values
  // For ex: VOLT range,resolution
  if(pvNodes.size() == 1){
    return decodeNodeValues(pvNodes.at(0));
  }
  // Travel the tree and get values of all nodes
  for(i=0; i<pvNodes.size(); i++){
    if(!decodeNodeValues(pvNodes.at(i))){
      return false;
    }
  }

  return true;
}
*/

bool mdtFrameCodecScpiU3606A::decodeConfigure(const QByteArray &data)
{
  QString str;
  QVariant value;

  // Clear previous results
  pvValues.clear();
  pvMeasureType = MT_UNKNOW;
  pvRange.clear();
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
    mdtError e(MDT_FRAME_DECODE_ERROR, "Function part contains no data", mdtError::Error);
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
    value = pvValues.at(1);
    if(value.convert(QVariant::Double)){
      pvRange = checkFloatingValueValidity(value.toDouble());
    }
    value = pvValues.at(2);
    if(value.convert(QVariant::Double)){
      pvResolution = checkFloatingValueValidity(value.toDouble());
    }
  }

  return true;
}

mdtFrameCodecScpiU3606A::measure_type_t mdtFrameCodecScpiU3606A::measureType() const
{
  return pvMeasureType;
}

QVariant mdtFrameCodecScpiU3606A::range() const
{
  return pvRange;
}

QVariant mdtFrameCodecScpiU3606A::resolution() const
{
  return pvResolution;
}

/**
bool mdtFrameCodecScpiU3606A::decodeNodeValues(QString data)
{
  QStringList nodeValues;

  // Case of VOLT
  if(data.left(4) == "VOLT"){
    pvMeasureType = MT_VOLTAGE_DC;
    data = data.remove(0, 4);
  // Case of CURRent
  }else if(data.left(4) == "CURR"){
    pvMeasureType = MT_CURRENT_DC;
    data = data.remove(0, 4);
  // Case of DC
  }else if(data.left(2) == "DC"){
    data = data.remove(0, 2);
  // Case of AC
  }else if(data.left(2) == "AC"){
    // Select the correct measure type
    switch(pvMeasureType){
      case MT_VOLTAGE_DC:
        pvMeasureType = MT_VOLTAGE_AC;
        break;
      case MT_CURRENT_DC:
        pvMeasureType = MT_CURRENT_AC;
        break;
      default:
        // Something goes wrong
        pvMeasureType = MT_UNKNOW;
        mdtError e(MDT_FRAME_DECODE_ERROR, "Unknow measure type" , mdtError::Warning);
        MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpiU3606A");
        e.commit();
        return false;
    }
  // Unknow measure type - Simply add to values
  }else{
    pvValues << data;
    return true;
  }

  // Continue parsing, it's possible to find range and resolution
  data = data.remove(0, 2);
  data = data.trimmed();
  // case of no range
  if(data.size() < 1){
    return true;
  }
  nodeValues = data.split(",");
  // We must have a couple range,resolution
  if(nodeValues.size() != 2){
    pvMeasureType = MT_UNKNOW;
    mdtError e(MDT_FRAME_DECODE_ERROR, "Range,resolution values not found" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpiU3606A");
    e.commit();
    return false;
  }
  ///pvRange = convertToDouble(nodeValues.at(0));
  pvRange = decodeSingleValueDouble(nodeValues.at(0).toAscii()).toDouble();
  ///pvResolution = convertToDouble(nodeValues.at(1));
  pvResolution = decodeSingleValueDouble(nodeValues.at(1).toAscii()).toDouble();

  return true;
}
*/
