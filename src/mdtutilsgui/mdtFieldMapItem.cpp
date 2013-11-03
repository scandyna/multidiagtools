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
#include <QSqlField>

//#include <QDebug>

mdtFieldMapItem::mdtFieldMapItem()
{
  pvSourceFieldDataStartOffset = -1;
  pvSourceFieldDataEndOffset = -1;
}

mdtFieldMapItem::~mdtFieldMapItem()
{
}

void mdtFieldMapItem::setDestinationFieldIndex(int value)
{
  pvDestinationField.setIndex(value);
}

int mdtFieldMapItem::destinationFieldIndex() const
{
  return pvDestinationField.index();
}

void mdtFieldMapItem::setDestinationField(const mdtFieldMapField &field)
{
  pvDestinationField = field;
}

mdtFieldMapField mdtFieldMapItem::destinationField() const
{
  return pvDestinationField;
}

void mdtFieldMapItem::setDestinationFieldName(const QString &value)
{
  pvDestinationField.setName(value);
}

QString mdtFieldMapItem::destinationFieldName() const
{
  return pvDestinationField.name();
}

void mdtFieldMapItem::setDestinationFieldDisplayText(const QString &value)
{
  pvDestinationField.setDisplayText(value);
}

QString mdtFieldMapItem::destinationFieldDisplayText() const
{
  return pvDestinationField.displayText();
}

void mdtFieldMapItem::setSourceFieldIndex(int value)
{
  pvSourceField.setIndex(value);
}

int mdtFieldMapItem::sourceFieldIndex() const
{
  return pvSourceField.index();
}

void mdtFieldMapItem::setSourceField(const mdtFieldMapField &field)
{
  pvSourceField = field;
}

mdtFieldMapField mdtFieldMapItem::sourceField() const
{
  return pvSourceField;
}

void mdtFieldMapItem::setSourceFieldName(const QString &value)
{
  pvSourceField.setName(value);
}

QString mdtFieldMapItem::sourceFieldName() const
{
  return pvSourceField.name();
}

void mdtFieldMapItem::setSourceFieldDisplayText(const QString &text)
{
  pvSourceField.setDisplayText(text);
}

QString mdtFieldMapItem::sourceFieldDisplayText() const
{
  return pvSourceField.displayText();
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
  pvDestinationField.sqlField().setType(type);
}

QVariant::Type mdtFieldMapItem::dataType() const
{
  return pvDestinationField.sqlField().type();
}

QVariant mdtFieldMapItem::destinationData(const QStringList &sourceRow) const
{
  if(pvSourceField.index() < 0){
    return QVariant();
  }
  if(pvSourceField.index() >= sourceRow.size()){
    return QVariant();
  }

  return destinationData(sourceRow.at(pvSourceField.index()));
}

QVariant mdtFieldMapItem::destinationData(const QString &sourceData) const
{
  QVariant data;

  if((pvSourceFieldDataStartOffset < 0)&&(pvSourceFieldDataEndOffset < 0)){
    data = sourceData;
    data.convert(pvDestinationField.sqlField().type());
    return data;
  }
  if(pvSourceFieldDataStartOffset < 0){
    data = sourceData.left(pvSourceFieldDataEndOffset + 1);
    data.convert(pvDestinationField.sqlField().type());
    return data;
  }
  if(pvSourceFieldDataEndOffset < 0){
    data = sourceData.mid(pvSourceFieldDataStartOffset, -1);
    data.convert(pvDestinationField.sqlField().type());
    return data;
  }
  data = sourceData.mid(pvSourceFieldDataStartOffset, pvSourceFieldDataEndOffset - pvSourceFieldDataStartOffset + 1);
  data.convert(pvDestinationField.sqlField().type());

  return data;
}
