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
#include "mdtFrameCodecAscii.h"
#include "mdtError.h"
#include <QString>

mdtFrameCodecAscii::mdtFrameCodecAscii()
{
  pvEofSeq = "";
}

mdtFrameCodecAscii::~mdtFrameCodecAscii()
{
}

void mdtFrameCodecAscii::setEofSeq(QByteArray &eofSeq)
{
  QString str;

  str = eofSeq;
  // Remove chars that are considered as whitespace (QChar::isSpace())
  // else, the trim() function will remove it. See QString::trimmed() for details
  str.remove('\t');
  str.remove('\n');
  str.remove('\v');
  str.remove('\f');

//   pvEofSeq = str.toAscii();
  pvEofSeq = str.toLocal8Bit();
}

void mdtFrameCodecAscii::trim()
{
  pvAsciiData = pvAsciiData.trimmed();
}

void mdtFrameCodecAscii::setCaseUpper()
{
  pvAsciiData = pvAsciiData.toUpper();
}

QVariant mdtFrameCodecAscii::convertData(const QString &data) const
{
  QVariant value;

  // Empty string is invalid
  if(data.isEmpty()){
    return QVariant();
  }
  // Store data and check possible conversions
  value = data;
  if(value.canConvert(QVariant::Date)){
    if(value.convert(QVariant::Date)){
      return value;
    }
  }
  value = data;
  if(value.canConvert(QVariant::Int)){
    if(value.convert(QVariant::Int)){
      return value;
    }
  }
  value = data;
  if(value.canConvert(QVariant::Double)){
    if(value.convert(QVariant::Double)){
      return value;
    }
  }
  // As defulat, we let the string format
  value = data;

  return value;
}

bool mdtFrameCodecAscii::clean()
{
  // Remove white spaces at beginning and end
  pvAsciiData = pvAsciiData.trimmed();
  // Remove EOF
  if(!removeEofSeq()){
    return false;
  }
  // Check that data are available
  if(pvAsciiData.size() < 1){
    mdtError e = mdtErrorNew("Frame contains no data (after clean)", mdtError::Error, "mdtFrameCodecAscii");
    e.commit();
    return false;
  }

  return true;
}

bool mdtFrameCodecAscii::removeEofSeq()
{
  // If EofSeq was not set, simply return
  if(pvEofSeq.size() < 1){
    return true;
  }
  // Check data length
  if(pvAsciiData.size() <= pvEofSeq.size()){
    mdtError e = mdtErrorNew("Frame is smaller than End Of Frame Sequence" , mdtError::Error, "mdtFrameCodecAscii");
    e.commit();
    return false;
  }
  // Remove the end of frame sequence
  pvAsciiData.remove(pvAsciiData.size()-pvEofSeq.size(), pvEofSeq.size());

  return true;
}
