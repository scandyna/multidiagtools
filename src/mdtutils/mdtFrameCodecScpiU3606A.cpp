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

/**
QVariant mdtFrameCodecScpiU3606A::decodeSingleValueDouble(QByteArray &frame)
{
  QVariant value;

  // Clear previous results
  pvValues.clear();

  // Store raw data in local QString constainer
  pvAsciiData = frame;

  // Remove white spaces at begin and end
  trim();
  // remove EofSeq
  if(!removeEofSeq()){
    return value;
  }

  // Case of no data
  if(pvAsciiData.size() < 1){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Frame contains no data" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpiU3606A");
    e.commit();
    return value;
  }

  // Convert value
  value = pvAsciiData;
  value = convertToDouble(value);
  if(value.isValid()){
    pvValues << value;
  }

  return value;
}
*/

mdtFrameCodecScpiU3606A::measure_type_t mdtFrameCodecScpiU3606A::measureType()
{
  return pvMeasureType;
}

QVariant mdtFrameCodecScpiU3606A::range()
{
  return pvRange;
}

QVariant mdtFrameCodecScpiU3606A::resolution()
{
  return pvResolution;
}

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

/// \todo Implement NaN (see SCPI99, 7.2.1.5)
/**
QVariant mdtFrameCodecScpiU3606A::convertToDouble(QVariant value)
{
  bool ok = false;
  double x;

  x = value.toDouble(&ok);
  if(!ok){
    value.clear();
    return value;
  }
  // Check limit
  if(x <= (-9.9e37 + DBL_EPSILON)){
    value.clear();
    return value;
  }
  if(x >= (9.9e37 - DBL_EPSILON)){
    value.clear();
    return value;
  }
  // Store value and return
  value = x;

  return value;
}
*/
