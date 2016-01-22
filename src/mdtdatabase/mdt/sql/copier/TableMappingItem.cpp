/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "TableMappingItem.h"
#include "AbstractTableMappingItem.h"
#include "FieldMapping.h"
#include "FixedValue.h"
#include "UniqueInsertExpression.h"

namespace mdt{ namespace sql{ namespace copier{

TableMappingItem::TableMappingItem(TableMappingItem::Type type)
 : pvType(type)
{
  qDebug() << "C TableMappingItem::TableMappingItem() .";
  constructShared();
}

TableMappingItem::TableMappingItem(const TableMappingItem & other)
 : pvType(other.pvType),
   pvShared(other.pvShared)
{
  qDebug() << "CPY TableMappingItem::TableMappingItem(other) .";
}

TableMappingItem::~TableMappingItem()
{
  qDebug() << "D TableMappingItem::~TableMappingItem() .";
}

TableMappingItem& TableMappingItem::operator=(const TableMappingItem & other)
{
  qDebug() << "CPY TableMappingItem::operator=(other) .";
  if(&other != this){
    pvType = other.pvType;
    pvShared = other.pvShared;
  }
  return *this;
}

bool TableMappingItem::isNull() const
{
  Q_ASSERT(pvShared);
  return pvShared->isNull();
}

void TableMappingItem::clear()
{
  Q_ASSERT(pvShared);
  pvShared->clear();
}

void TableMappingItem::setFieldMapping(int sourceFieldIndex, int destinationFieldIndex)
{
  if(pvType != FieldMappingType){
    reset(FieldMappingType);
  }
  Q_ASSERT(pvShared);
  pvShared->setFieldMapping(sourceFieldIndex, destinationFieldIndex);
}

int TableMappingItem::sourceFieldIndex() const
{
  Q_ASSERT(pvShared);
  return pvShared->sourceFieldIndex();
}

// QVector<int> TableMappingItem::destinationFieldIndexList() const
FieldIndexList TableMappingItem::destinationFieldIndexList() const
{
  Q_ASSERT(pvShared);
  return pvShared->destinationFieldIndexList();
}

void TableMappingItem::setFixedValue(const QVariant& value, int destinationFieldIndex)
{
  if(pvType != FixedValueType){
    reset(FixedValueType);
  }
  Q_ASSERT(pvShared);
  pvShared->setFixedValue(value, destinationFieldIndex);
}

QVariant TableMappingItem::fixedValue() const
{
  Q_ASSERT(pvShared);
  return pvShared->fixedValue();
}

void TableMappingItem::setUniqueInsertExpression(const UniqueInsertExpression & exp)
{
  pvType = UniqueInsertExpressionType;
  pvShared = new UniqueInsertExpression(exp);
}

UniqueInsertExpression TableMappingItem::uniqueInsertExpression() const
{
  UniqueInsertExpression exp;

  if(pvType == UniqueInsertExpressionType){
    const auto *pexp = dynamic_cast<const UniqueInsertExpression*>(pvShared.constData());
    Q_ASSERT(pexp != nullptr);
    exp = *pexp;
  }else{
    exp.setDestinationFieldIndexList(destinationFieldIndexList());
  }

  return exp;
}

void TableMappingItem::setMappingState(TableMappingItemState state)
{
  Q_ASSERT(pvShared);
  pvShared->setMappingState(state);
}

TableMappingItemState TableMappingItem::mappingState() const
{
  Q_ASSERT(pvShared);
  return pvShared->mappingState();
}

void TableMappingItem::reset(TableMappingItem::Type type)
{
  Q_ASSERT(type != pvType);

  pvType = type;
  if(pvShared){
    pvShared.~QSharedDataPointer();
  }
  constructShared();
}

void TableMappingItem::constructShared()
{
  switch(pvType){
    case FieldMappingType:
      pvShared = new FieldMapping;
      break;
    case FixedValueType:
      pvShared = new FixedValue;
      break;
    case UniqueInsertExpressionType:
      pvShared = new UniqueInsertExpression;
      break;
  }
}

bool operator<(const TableMappingItem & a, const TableMappingItem & b)
{
  Q_ASSERT(a.pvShared);
  Q_ASSERT(b.pvShared);
  return (*a.pvShared < *b.pvShared);
}

}}} // namespace mdt{ namespace sql{ namespace copier{

/*
 * Clone template specialization:
 * We have a QSharedDataPointer<AbstractTableMappingItem>,
 * but we must return a new object using the right clone() function.
 */
template <>
mdt::sql::copier::AbstractTableMappingItem *QSharedDataPointer<mdt::sql::copier::AbstractTableMappingItem>::clone()
{
  return d->clone();
}
