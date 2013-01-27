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
#include "mdtFrameCodecScpi.h"
#include "mdtError.h"
#include "mdtAlgorithms.h"
#include <cfloat>
#include <QList>

#include <QDebug>

mdtFrameCodecScpi::mdtFrameCodecScpi()
{
  /*
   * Values returned by device
   *  when a measure fails
   *  (OL or other)
   * See SCPI-99, chap. 7.2.1
   */
  pvInfinity = 9.9e37;
  pvNinfinity = -9.9e37;
  pvNan = 9.91e37;
}

mdtFrameCodecScpi::~mdtFrameCodecScpi()
{
}

bool mdtFrameCodecScpi::decodeValues(const QByteArray &data, QString sep)
{
  int i;
  QVariant value;
  ///double fltValue;

  // Clear previous results
  pvValues.clear();
  pvNodes.clear();

  // Store raw data in local QString constainer
  pvAsciiData = data;

  // Remove begin/end spaces and EOF
  if(!clean()){
    return false;
  }

  // Extract nodes
  pvNodes = pvAsciiData.split(sep);
  for(i=0; i<pvNodes.size(); i++){
    // We ignore empty nodes
    if(pvNodes.at(i).isEmpty()){
      continue;
    }
    // Convert node
    value = convertData(pvNodes.at(i));
    // Check limits if type is double
    if(value.type() == QVariant::Double){
      /**
      fltValue = value.toDouble();
      // Check about Nan
      if(qAbs(fltValue - pvNan) <= FLT_EPSILON){
        value.clear();
      }else{
        // Check about -OL and OL
        if((fltValue <= (pvNinfinity+FLT_EPSILON))||(fltValue >= (pvInfinity-FLT_EPSILON))){
          value.clear();
        }
      }
      */
      value = checkFloatingValueValidity(value.toDouble());
    }
    // Add converted value
    pvValues.append(value);
  }

  return true;
}

QVariant mdtFrameCodecScpi::decodeSingleValueDouble(const QByteArray &data)
{
  QVariant value;

  // Store raw data in local QString constainer
  pvAsciiData = data;
  // Remove white spaces at begin and end
  trim();
  // remove EofSeq
  if(!removeEofSeq()){
    return value;
  }
  // Case of no data
  if(pvAsciiData.size() < 1){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Frame contains no data" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return value;
  }
  // Convert and check
  value = pvAsciiData;
  if(value.canConvert(QVariant::Double)){
    if(value.convert(QVariant::Double)){
      return checkFloatingValueValidity(value.toDouble());
    }
  }

  return QVariant();
}

QVariant mdtFrameCodecScpi::checkFloatingValueValidity(double x) const
{
  // Check about NaN
  if(qAbs(x - pvNan) <= FLT_EPSILON){
    return QVariant();
  }
  // Check about -OL and OL
  if((x <= (pvNinfinity+FLT_EPSILON))||(x >= (pvInfinity-FLT_EPSILON))){
    return QVariant();
  }

  return x;
}

bool mdtFrameCodecScpi::decodeFunctionParameters(const QByteArray &data)
{
  int i;
  QVariant value;

  // Clear previous results
  pvValues.clear();
  pvNodes.clear();
  // Store raw data in local QString constainer
  pvAsciiData = data;
  // Remove white spaces at begin and end
  trim();
  // remove EofSeq
  if(!removeEofSeq()){
    return false;
  }
  // Case of no data
  if(pvAsciiData.size() < 1){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Frame contains no data" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // Separate <function> <parameters>
  pvNodes = pvAsciiData.split(' ', QString::SkipEmptyParts);
  if(pvNodes.size() < 1){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Unexpected amount of space separated elements (expected min. 1)" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // Ad function to result
  pvValues.append(pvNodes.at(0));
  if(pvValues.at(0).type() != QVariant::String){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Function is not a string" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // Separate parameters
  if(pvNodes.size() > 1){
    if(pvNodes.size() != 2){
      mdtError e(MDT_FRAME_DECODE_ERROR, "Unexpected amount of space separated elements (expected 2)" , mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
      e.commit();
      return false;
    }
    pvNodes = pvNodes.at(1).split(',', QString::SkipEmptyParts);
    for(i=0; i<pvNodes.size(); i++){
      // Convert node
      value = convertData(pvNodes.at(i));
      // Check limits if type is double
      if(value.type() == QVariant::Double){
        value = checkFloatingValueValidity(value.toDouble());
      }
      // Add converted value
      pvValues.append(value);
    }
  }

  return true;
}

bool mdtFrameCodecScpi::decodeIdn(const QByteArray &data)
{
  QVariant value;
  int i;

  // Clear previous results
  pvValues.clear();
  pvNodes.clear();
  // Store raw data in local QString constainer
  pvAsciiData = data;
  // Remove begin/end spaces and EOF
  if(!clean()){
    return false;
  }
  // Build the values list
  pvNodes = pvAsciiData.split(",");
  // Store result in data list
  for(i=0; i<pvNodes.size(); i++){
    pvValues << pvNodes.at(i);
  }

  return true;
}

bool mdtFrameCodecScpi::decodeScpiVersion(const QByteArray &data)
{
  QVariant value;

  // Clear previous results
  pvValues.clear();
  pvNodes.clear();
  // Store raw data in local QString constainer
  pvAsciiData = data;
  // Remove begin/end spaces and EOF
  if(!clean()){
    return false;
  }
  // Separate YYYY and V
  pvNodes = pvAsciiData.split('.', QString::SkipEmptyParts);
  if(pvNodes.size() != 2){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Unexpected amount of space separated elements (expected 2)" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // Convert and store version
  value = pvNodes.at(0);
  if(!value.convert(QVariant::Int)){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Version has wrong format (expected YYYY.V)" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  pvValues.append(value);
  value = pvNodes.at(1);
  if(!value.convert(QVariant::Int)){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Version has wrong format (expected YYYY.V)" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  pvValues.append(value);

  return true;
}

bool mdtFrameCodecScpi::decodeError(const QByteArray &data)
{
  QVariant value;
  bool ok = false;
  int i;

  // Clear previous results
  pvValues.clear();
  pvNodes.clear();

  // Store raw data in local QString constainer
  pvAsciiData = data;

  // Remove begin/end spaces and EOF
  if(!clean()){
    return false;
  }

  // Build the values list
  pvNodes = mdtAlgorithms::splitString(pvAsciiData, ",", "\"");
  // Store result in data list
  if(pvNodes.size() != 2){
    mdtError e(MDT_FRAME_DECODE_ERROR, "data contains not an error" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // Error code
  value = pvNodes.at(0);
  value = value.toInt(&ok);
  if(!ok){
    value.clear();
  }
  pvValues << value;
  // Error texts
  pvNodes = pvNodes.at(1).split(";");
  for(i=0; i<pvNodes.size(); i++){
    pvValues << pvNodes.at(i);
  }

  return true;
}

bool mdtFrameCodecScpi::decodeIEEEblock(const QByteArray &data, mdtFrameCodecScpi::waveform_format_t format)
{
  int headerSize;
  int dataSize;
  bool ok = false;

  // Clear previous results
  pvValues.clear();
  // If string length is < 3: shure: not an IEEE block
  if(data.size() < 3){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Frame is not a IEEE block (length < 2)" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // If first char is not a #, we have not a IEEE block
  if(data.at(0) != '#'){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Frame is not a IEEE block (beginns not with #)" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // Get the header length
  headerSize = data.mid(1, 1).toInt(&ok);
  if(!ok){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Cannot get header length in IEEE block" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // Check that enough bytes are available
  if(data.size() < (headerSize+2)){
    mdtError e(MDT_FRAME_DECODE_ERROR, "IEEE block contains no data" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // Get data length
  dataSize = data.mid(2, headerSize).toInt(&ok);
  if(!ok){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Cannot get data length in IEEE block" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  if(dataSize < 1){
    mdtError e(MDT_FRAME_DECODE_ERROR, "IEEE block contains no data (info from header)" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // Calculate real received data size
  if(data.at(data.size()-1) == '\n'){
    dataSize = data.size()-headerSize-3;
  }else{
    dataSize = data.size()-headerSize-2;
  }
  if(dataSize < 1){
    mdtError e(MDT_FRAME_DECODE_ERROR, "IEEE block contains no data (info from header)" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // Decode data part
  if(format == BYTE){
    return decodeIEEEdataByte(data.mid(headerSize+2, dataSize));
  }
  if(format == ASCII){
    return decodeIEEEdataAscii(data.mid(headerSize+2, dataSize));
  }
  mdtError e(MDT_FRAME_DECODE_ERROR, "Data format not implemented yet, sorry" , mdtError::Warning);
  MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
  e.commit();
  return false;
}

bool mdtFrameCodecScpi::decodeIEEEdataAscii(const QByteArray &data)
{
  QList<QByteArray> items;
  int i;
  bool ok;
  QVariant value;

  items = data.split(',');
  ///qDebug() << "Data: " << items;
  // Convert each item
  for(i=0; i<items.size(); i++){
    value = items.at(i).toDouble(&ok);
    if(!ok){
      value.clear();
    }
    pvValues.append(value);
  }
  ///qDebug() << "Values: " << pvValues;

  return true;
}

bool mdtFrameCodecScpi::decodeIEEEdataByte(const QByteArray &data)
{
  int i;
  ///int len = data.size()-1;  // Last item is the term char

  // Check data length
  /*
  if(len < 1){
    mdtError e(MDT_FRAME_DECODE_ERROR, "IEEE block contains no data" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  */
  for(i=0; i<data.size(); i++){
    pvValues.append((int)data.at(i));
    ///qDebug() << "data[" << i << "]: " << (qint8)data.at(i) << " ,  0x" << hex << (qint8)data.at(i) << " , flt: " << (double)data.at(i)/127.0;
  }
  
  return true;
}
