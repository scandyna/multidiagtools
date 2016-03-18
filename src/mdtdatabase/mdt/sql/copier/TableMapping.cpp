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
#include "TableMapping.h"
#include "TableMappingItemState.h"
#include "TableMappingEditHelper.h"
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QStringList>

//#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

TableMapping::TableMapping()
 : pvMappingState(MappingNotSet)
{
}

void TableMapping::setUniqueInsertCriteria(const UniqueInsertCriteria &c)
{
  pvUniqueInsertCriteria = c;
}

QVector<int> TableMapping::getItemsToRemoveIndexList(const FieldIndexList& itemDfiList) const
{
  return TableMappingEditHelper::getItemsToRemoveIndexList(itemDfiList, pvItems, true);
}

void TableMapping::insertItem(TableMappingItem item)
{
  Q_ASSERT(!item.destinationFieldIndexList().isEmpty());
  Q_ASSERT(item.destinationFieldIndexList().at(0) >= 0);

  updateMappingItemState(item);
  TableMappingEditHelper::insertItem(item, pvItems);
  updateTableMappingState();
}

QStringList TableMapping::getSourceTableFieldNameList() const
{
  return fetchSourceTableFieldNameList();
}

QStringList TableMapping::getDestinationTableFieldNameList() const
{
  return fetchDestinationTableFieldNameList();
}

void TableMapping::setSourceFieldAtItem(int itemIndex, const QString & fieldName)
{
  Q_ASSERT(itemIndex >= 0);
  Q_ASSERT(itemIndex < pvItems.size());

  auto & item = pvItems[itemIndex];
  Q_ASSERT(item.destinationFieldIndexList().count() == 1);
  const int sourceFieldIndex = fetchSourceTableFieldIndexOf(fieldName);
  const int destinationFieldIndex = item.destinationFieldIndexList().at(0);

  item.setFieldMapping(sourceFieldIndex, destinationFieldIndex);
  // Update item mapping state
  updateMappingItemState(item);
  // Update table mapping state
  updateTableMappingState();
}

QString TableMapping::sourceTableFieldNameAt(int fieldIndex) const
{
  Q_ASSERT(fieldIndex >= 0);
  Q_ASSERT(fieldIndex < sourceTableFieldCount());

  return fetchSourceTableFieldNameAt(fieldIndex);
}

QStringList TableMapping::sourceFieldNameListAtItem(int itemIndex) const
{
  Q_ASSERT(itemIndex >= 0);
  Q_ASSERT(itemIndex < pvItems.size());

  QStringList fieldNameList;

  const auto fieldIndexList = pvItems.at(itemIndex).sourceFieldIndexList();
  for(const auto & fieldIndex : fieldIndexList){
    Q_ASSERT(fieldIndex >= 0);
    Q_ASSERT(fieldIndex < sourceTableFieldCount());
    fieldNameList.append(fetchSourceTableFieldNameAt(fieldIndex));
  }

  return fieldNameList;
}

QStringList TableMapping::sourceFieldTypeNameListAtItem(int itemIndex) const
{
  Q_ASSERT(itemIndex >= 0);
  Q_ASSERT(itemIndex < pvItems.size());
  Q_ASSERT(pvItems.at(itemIndex).type() == TableMappingItem::FieldMappingType);

  QStringList fieldTypeNameList;

  const auto fieldIndexList = pvItems.at(itemIndex).sourceFieldIndexList();
  for(const auto & fieldIndex : fieldIndexList){
    Q_ASSERT(fieldIndex >= 0);
    Q_ASSERT(fieldIndex < sourceTableFieldCount());
    fieldTypeNameList.append(fetchSourceTableFieldTypeNameAt(fieldIndex));
  }

  return fieldTypeNameList;
}

QVector<TableMapping::FieldKeyType> TableMapping::sourceFieldKeyTypeListAtItem(int itemIndex) const
{
  Q_ASSERT(itemIndex >= 0);
  Q_ASSERT(itemIndex < pvItems.size());
  Q_ASSERT(pvItems.at(itemIndex).type() == TableMappingItem::FieldMappingType);

  QVector<FieldKeyType> fieldKeyTypeList;

  const auto fieldIndexList = pvItems.at(itemIndex).sourceFieldIndexList();
  for(const auto & fieldIndex : fieldIndexList){
    Q_ASSERT(fieldIndex >= 0);
    Q_ASSERT(fieldIndex < sourceTableFieldCount());
    fieldKeyTypeList.append(fetchSourceTableFieldKeyType(fieldIndex));
  }

  return fieldKeyTypeList;
}

void TableMapping::setSourceFixedValueAtItem(int itemIndex, const QVariant & value)
{
  Q_ASSERT(itemIndex >= 0);
  Q_ASSERT(itemIndex < pvItems.size());

  auto & item = pvItems[itemIndex];
  Q_ASSERT(item.destinationFieldIndexList().count() == 1);
  const int destinationFieldIndex = item.destinationFieldIndexList().at(0);

  item.setFixedValue(value, destinationFieldIndex);
  // Update item mapping state
  updateMappingItemState(item);
  // Update table mapping state
  updateTableMappingState();
}

QString TableMapping::destinationTableFieldNameAt(int fieldIndex) const
{
  Q_ASSERT(fieldIndex >= 0);
  Q_ASSERT(fieldIndex < destinationTableFieldCount());

  return fetchDestinationTableFieldNameAt(fieldIndex);
}

QStringList TableMapping::destinationFieldNameListAtItem(int itemIndex) const
{
  Q_ASSERT(itemIndex >= 0);
  Q_ASSERT(itemIndex < pvItems.size());

  QStringList fieldNameList;

  auto item = pvItems.at(itemIndex);
  const auto fieldIndexList = item.destinationFieldIndexList();
  for(const auto & fieldIndex : fieldIndexList){
    Q_ASSERT(fieldIndex >= 0);
    Q_ASSERT(fieldIndex < destinationTableFieldCount());
    fieldNameList.append(fetchDestinationTableFieldNameAt(fieldIndex));
  }

  return fieldNameList;
}

QStringList TableMapping::destinationFieldTypeNameListAtItem(int itemIndex) const
{
  Q_ASSERT(itemIndex >= 0);
  Q_ASSERT(itemIndex < pvItems.size());

  QStringList fieldNameList;

  auto item = pvItems.at(itemIndex);
  const auto fieldIndexList = item.destinationFieldIndexList();
  for(const auto & fieldIndex : fieldIndexList){
    Q_ASSERT(fieldIndex >= 0);
    Q_ASSERT(fieldIndex < destinationTableFieldCount());
    fieldNameList.append(fetchDestinationTableFieldTypeNameAt(fieldIndex));
  }

  return fieldNameList;
}

QVector<TableMapping::FieldKeyType> TableMapping::destinationFieldKeyTypeListAtItem(int itemIndex) const
{
  Q_ASSERT(itemIndex >= 0);
  Q_ASSERT(itemIndex < pvItems.size());

  QVector<FieldKeyType> fieldKeyTypeList;

  auto item = pvItems.at(itemIndex);
  const auto fieldIndexList = item.destinationFieldIndexList();
  for(const auto & fieldIndex : fieldIndexList){
    Q_ASSERT(fieldIndex >= 0);
    Q_ASSERT(fieldIndex < destinationTableFieldCount());
    fieldKeyTypeList.append(fetchDestinationTableFieldKeyType(fieldIndex));
  }

  return fieldKeyTypeList;
}

void TableMapping::generateFieldMappingByName()
{
  resetFieldMapping();
  for(auto & item : pvItems){
    Q_ASSERT(item.destinationFieldIndexList().count() == 1);
    // Get destination field index
    const int destinationFieldIndex = item.destinationFieldIndexList().at(0);
    Q_ASSERT(destinationFieldIndex >= 0);
    Q_ASSERT(destinationFieldIndex < destinationTableFieldCount());
    // Get current destination field name
    const QString destinationFieldName = fetchDestinationTableFieldNameAt(destinationFieldIndex);
    // Get source field index that matches destination field name
    const int sourceFieldIndex = fetchSourceTableFieldIndexOf(destinationFieldName);
    // Set item and update its state
    item.setFieldMapping(sourceFieldIndex, destinationFieldIndex);
    updateMappingItemState(item);
  }
  updateTableMappingState();
}

void TableMapping::resetFieldMapping()
{
  int n = destinationTableFieldCount();

  clearFieldMapping();
  pvItems.reserve(n);
  for(int i = 0; i < n; ++i){
    TableMappingItem item(TableMappingItem::FieldMappingType);
    item.setFieldMapping(-1, i);
    pvItems.append(item);
  }
}

void TableMapping::clearFieldMapping()
{
  pvItems.clear();
  pvMappingState = MappingNotSet;
}

void TableMapping::updateMappingItemState(TableMappingItem& item)
{
  // Check about null mapping
  if(item.isNull()){
    item.setMappingState(TableMappingItemState::MappingNotSet);
    return;
  }
  // Check for case of fixed value source
  if(item.type() == TableMappingItem::FixedValueType){
    if(item.fixedValue().isNull()){
      item.setMappingState(TableMappingItemState::MappingError);
    }else{
      item.setMappingState(TableMappingItemState::MappingComplete);
    }
    return;
  }
  // Check case of field mapping
  if(item.type() == TableMappingItem::FieldMappingType){
    Q_ASSERT(item.sourceFieldIndexList().count() == 1);
    Q_ASSERT(item.destinationFieldIndexList().count() == 1);
    const int sourceFieldIndex = item.sourceFieldIndexList().at(0);
    const int destinationFieldIndex = item.destinationFieldIndexList().at(0);
    Q_ASSERT(sourceFieldIndex >= 0);
    Q_ASSERT(destinationFieldIndex >= 0);
    // Check field compatibility
    if(!areFieldsCompatible(sourceFieldIndex, destinationFieldIndex)){
      item.setMappingState(TableMappingItemState::MappingError);
      return;
    }
  }
  /// Expressions ...
  
  // Done
  item.setMappingState(TableMappingItemState::MappingComplete);
}

void TableMapping::updateTableMappingState()
{
  // Check if both tables are set
  if( sourceTableName().isEmpty() || destinationTableName().isEmpty() ){
    pvMappingState = MappingNotSet;
    return;
  }
  // Check state of each mapping item and deduce table mapping state
  for(const auto & item : pvItems){
    switch(item.mappingState()){
      case TableMappingItemState::MappingError:
        pvMappingState = MappingError;
        return;
      case TableMappingItemState::MappingNotSet:
      case TableMappingItemState::MappingPartial:
        pvMappingState = MappingPartial;
        return;
      case TableMappingItemState::MappingComplete:
        break;
    }
  }
  // All checks successfully passed
  pvMappingState = MappingComplete;
}

}}} // namespace mdt{ namespace sql{ namespace copier{
