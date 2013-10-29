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
#include "mdtFieldMapItem.h"

//#include <QDebug>

mdtFieldMapItem::mdtFieldMapItem()
{
  pvFieldIndex = -1;
  pvSourceFieldIndex = -1;
  pvSourceFieldDataStartOffset = -1;
  pvSourceFieldDataEndOffset = -1;
  pvDataType = QVariant::Invalid;
}

mdtFieldMapItem::~mdtFieldMapItem()
{
}

void mdtFieldMapItem::setFieldIndex(int value)
{
  pvFieldIndex = value;
}

int mdtFieldMapItem::fieldIndex() const
{
  return pvFieldIndex;
}

void mdtFieldMapItem::setFieldName(const QString &value)
{
  pvFieldName = value;
}

QString mdtFieldMapItem::fieldName() const
{
  return pvFieldName;
}

void mdtFieldMapItem::setFieldDisplayText(const QString &value)
{
  pvFieldDisplayText = value;
}

QString mdtFieldMapItem::fieldDisplayText() const
{
  return pvFieldDisplayText;
}

void mdtFieldMapItem::setSourceFieldIndex(int value)
{
  pvSourceFieldIndex = value;
}

int mdtFieldMapItem::sourceFieldIndex() const
{
  return pvSourceFieldIndex;
}

void mdtFieldMapItem::setSourceFieldName(const QString &value)
{
  pvSourceFieldName = value;
}

QString mdtFieldMapItem::sourceFieldName() const
{
  return pvSourceFieldName;
}

void mdtFieldMapItem::setSourceFieldDataStartOffset(int offset)
{
  pvSourceFieldDataStartOffset = offset;
}

int mdtFieldMapItem::sourceFieldDataStartOffset() const
{
  return pvSourceFieldDataStartOffset;
}

void mdtFieldMapItem::setSourceFieldDataEndOffset(int offset)
{
  pvSourceFieldDataEndOffset = offset;
}

int mdtFieldMapItem::sourceFieldDataEndOffset() const
{
  return pvSourceFieldDataEndOffset;
}

void mdtFieldMapItem::setDataType(QVariant::Type type)
{
  pvDataType = type;
}

QVariant::Type mdtFieldMapItem::dataType() const
{
  return pvDataType;
}

QVariant mdtFieldMapItem::destinationData(const QStringList &sourceRow) const
{
  ///QString src;
  ///QVariant data;

  if(pvSourceFieldIndex < 0){
    return QVariant();
  }
  if(pvSourceFieldIndex >= sourceRow.size()){
    return QVariant();
  }

  return destinationData(sourceRow.at(pvSourceFieldIndex));

  /**
  if((pvSourceFieldDataStartOffset < 0)&&(pvSourceFieldDataEndOffset < 0)){
    src = sourceRow.at(pvSourceFieldIndex);
    data = src;
    data.convert(pvDataType);
    return data;
  }
  if(pvSourceFieldDataStartOffset < 0){
    src = sourceRow.at(pvSourceFieldIndex).left(pvSourceFieldDataEndOffset + 1);
    data = src;
    data.convert(pvDataType);
    return data;
  }
  if(pvSourceFieldDataEndOffset < 0){
    src = sourceRow.at(pvSourceFieldIndex);
    src = src.right(src.size() - pvSourceFieldDataStartOffset);
    data = src;
    data.convert(pvDataType);
    return data;
  }
  src = sourceRow.at(pvSourceFieldIndex);
  src = src.mid(pvSourceFieldDataStartOffset, pvSourceFieldDataEndOffset - pvSourceFieldDataStartOffset + 1);
  data = src;
  data.convert(pvDataType);

  return data;
  */
}

QVariant mdtFieldMapItem::destinationData(const QString &sourceData) const
{
  QVariant data;

  if((pvSourceFieldDataStartOffset < 0)&&(pvSourceFieldDataEndOffset < 0)){
    data = sourceData;
    data.convert(pvDataType);
    return data;
  }
  if(pvSourceFieldDataStartOffset < 0){
    data = sourceData.left(pvSourceFieldDataEndOffset + 1);
    data.convert(pvDataType);
    return data;
  }
  if(pvSourceFieldDataEndOffset < 0){
    data = sourceData.mid(pvSourceFieldDataStartOffset, -1);
    data.convert(pvDataType);
    return data;
  }
  data = sourceData.mid(pvSourceFieldDataStartOffset, pvSourceFieldDataEndOffset - pvSourceFieldDataStartOffset + 1);
  data.convert(pvDataType);

  return data;
}
