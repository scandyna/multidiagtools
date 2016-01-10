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

// void mdtSqlCopierTableMapping::setSourceType(int index, mdtSqlCopierFieldMapping::SourceType type)
// {
//   Q_ASSERT(index >= 0);
//   Q_ASSERT(index < pvFieldMappingList.size());
// 
//   auto & fm = pvFieldMappingList[index];
// 
//   fm.sourceType = type;
//   updateFieldMappingState(fm);
// }

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

void TableMapping::setSourceField(int index, const QString & fieldName)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFieldMappingList.size());

  auto & fm = pvFieldMappingList[index];

  // Update source field
  if(fieldName.isEmpty()){
    fm.clear();
  }else{
    setSourceFieldIndex(fm, fieldName);
  }
  updateFieldMappingState(fm);
  // Update table mapping state
  updateTableMappingState();
}

QString TableMapping::sourceTableFieldNameAt(int fieldIndex) const
{
  Q_ASSERT(fieldIndex >= 0);
  Q_ASSERT(fieldIndex < sourceTableFieldCount());

  return fetchSourceTableFieldNameAt(fieldIndex);
}

QString TableMapping::sourceFieldNameAtItem(int itemIndex) const
{
  Q_ASSERT(itemIndex >= 0);
  Q_ASSERT(itemIndex < pvItems.size());

  const int sourceFieldIndex = pvItems.at(itemIndex).sourceFieldIndex();
  if(sourceFieldIndex < 0){
    return QString();
  }
  Q_ASSERT(sourceFieldIndex < sourceTableFieldCount());

  return fetchSourceTableFieldNameAt(sourceFieldIndex);
}

QString TableMapping::sourceFieldTypeNameAtItem(int itemIndex) const
{
  Q_ASSERT(itemIndex >= 0);
  Q_ASSERT(itemIndex < pvItems.size());
  Q_ASSERT(pvItems.at(itemIndex).type() == TableMappingItem::FieldMappingType);

  const int sourceFieldIndex = pvItems.at(itemIndex).sourceFieldIndex();
  if(sourceFieldIndex < 0){
    return QString();
  }
  Q_ASSERT(sourceFieldIndex < sourceTableFieldCount());

  return fetchSourceTableFieldTypeNameAt(sourceFieldIndex);
}

TableMapping::FieldKeyType TableMapping::sourceFieldKeyTypeAtItem(int itemIndex) const
{
  Q_ASSERT(itemIndex >= 0);
  Q_ASSERT(itemIndex < pvItems.size());
  Q_ASSERT(pvItems.at(itemIndex).type() == TableMappingItem::FieldMappingType);

  const int sourceFieldIndex = pvItems.at(itemIndex).sourceFieldIndex();
  if(sourceFieldIndex < 0){
    return NotAKey;
  }
  Q_ASSERT(sourceFieldIndex < sourceTableFieldCount());

  return fetchSourceTableFieldKeyType(sourceFieldIndex);
}

QString TableMapping::sourceFieldName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());
  Q_ASSERT(pvFieldMappingList.at(index).sourceField.type() == mdt::sql::copier::SourceField::SourceFieldIndexType);

  int sourceFieldIndex = pvFieldMappingList.at(index).sourceField.fieldIndex();
  if(sourceFieldIndex < 0){
    return QString();
  }
  return fetchSourceTableFieldNameAt(sourceFieldIndex);
}

QString TableMapping::sourceFieldTypeName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());
  Q_ASSERT(pvFieldMappingList.at(index).sourceField.type() == mdt::sql::copier::SourceField::SourceFieldIndexType);

  int sourceFieldIndex = pvFieldMappingList.at(index).sourceField.fieldIndex();
  if(sourceFieldIndex < 0){
    return QString();
  }
//   return fetchSourceFieldTypeName(sourceFieldIndex);
  return fetchSourceTableFieldTypeNameAt(sourceFieldIndex);
}

TableMapping::FieldKeyType TableMapping::sourceFieldKeyType(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFieldMappingList.size());
  Q_ASSERT(pvFieldMappingList.at(index).sourceField.type() == mdt::sql::copier::SourceField::SourceFieldIndexType);

  const int sourceFieldIndex = fieldMappingAt(index).sourceField.fieldIndex();
  if(sourceFieldIndex < 0){
    return NotAKey;
  }
  Q_ASSERT(sourceFieldIndex < sourceTableFieldCount());
  return fetchSourceTableFieldKeyType(sourceFieldIndex);
}

void TableMapping::setSourceFixedValue(int index, const QVariant & value)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFieldMappingList.size());

  auto & fm = pvFieldMappingList[index];

  // Update source field
  fm.sourceField.setFixedValue(value);
  updateFieldMappingState(fm);
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
//   for(int i = 0; i < n; ++i){
//     mdtSqlCopierFieldMapping fm;
//     fm.destinationFieldIndex = i;
//     pvFieldMappingList.append(fm);
//   }
}

void TableMapping::clearFieldMapping()
{
//   pvFieldMappingList.clear();
  pvItems.clear();
  pvMappingState = MappingNotSet;
}

QString TableMapping::getSqlForSourceTableCount(const QSqlDatabase & db) const
{
  QString sql;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);

  sql = "SELECT COUNT(*) FROM " + driver->escapeIdentifier(sourceTableName(), QSqlDriver::TableName);

  return sql;
}

QString TableMapping::getSqlForSourceTableSelect(const QSqlDatabase & db) const
{
  using mdt::sql::copier::SourceField;

  QString sql;
  QStringList fields;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);
  int lastIndex;

  // Build list of mapped fields
  for(int i = 0; i < fieldCount(); ++i){
    const auto fm = fieldMappingAt(i);
    if( (!fm.sourceField.isNull()) && (fm.sourceField.type() == SourceField::SourceFieldIndexType) ){
      fields.append(sourceFieldName(i));
    }
  }
  lastIndex = fields.size() - 1;
  // If no mapping was set, simply return a empty statement
  if(lastIndex < 0){
    return sql;
  }
  // Build statement
  sql = "SELECT ";
  for(int i = 0; i < lastIndex; ++i){
    sql += driver->escapeIdentifier(fields.at(i), QSqlDriver::FieldName) + ",";
  }
  sql += driver->escapeIdentifier(fields.at(lastIndex), QSqlDriver::FieldName);
  sql += " FROM " + driver->escapeIdentifier(sourceTableName(), QSqlDriver::TableName);

  return sql;
}

QString TableMapping::getSqlForDestinationTableInsert(const QSqlDatabase & db) const
{
  using mdt::sql::copier::SourceField;

  QString sql;
  QVector<QString> fields;
  QVector<QString> values;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);
  int lastIndex;

  // Build list of fields and values
  for(int i = 0; i < fieldCount(); ++i){
    const auto fm = fieldMappingAt(i);
    if(!fm.isNull()){
      fields.append(destinationFieldName(i));
      if(fm.sourceField.type() == SourceField::SourceFieldIndexType){
        /// \todo We could create a helper in SourceField class that returns the correct string
        values.append("?");
      }else{
        values.append("'" + fm.sourceField.fixedValue().toString() + "'");
      }
    }
  }
  Q_ASSERT(fields.size() == values.size());

  lastIndex = fields.size() - 1;
  // If no mapping was set, simply return a empty statement
  if(lastIndex < 0){
    return sql;
  }
  // Build statement
  sql = "INSERT INTO " + driver->escapeIdentifier(destinationTableName(), QSqlDriver::TableName) + " (";
  for(int i = 0; i < lastIndex; ++i){
    sql += driver->escapeIdentifier(fields.at(i), QSqlDriver::FieldName) + ",";
  }
  sql += driver->escapeIdentifier(fields.at(lastIndex), QSqlDriver::FieldName);
  sql += ") VALUES (";
  for(int i = 0; i < lastIndex; ++i){
    sql += values.at(i) + ",";
  }
  sql += values.at(lastIndex) + ")";

  return sql;
}

void TableMapping::updateFieldMappingState(mdtSqlCopierFieldMapping & fm)
{
  using mdt::sql::copier::SourceField;

  // Check about null mapping
  if(fm.isNull()){
    fm.mappingState = mdtSqlCopierFieldMapping::MappingNotSet;
    return;
  }
  // Check for case of fixed value source
  if(fm.sourceField.type() == SourceField::SourceFixedValueType){
    if(fm.sourceField.isNull()){
      fm.mappingState = mdtSqlCopierFieldMapping::MappingError;
    }else{
      fm.mappingState = mdtSqlCopierFieldMapping::MappingComplete;
    }
    return;
  }
  // We have a field source, do checks
  Q_ASSERT(fm.sourceField.type() == SourceField::SourceFieldIndexType);
  const int sourceFieldIndex = fm.sourceField.fieldIndex();
  const int destinationFieldIndex = fm.destinationFieldIndex;
  Q_ASSERT(sourceFieldIndex >= 0);
  Q_ASSERT(destinationFieldIndex >= 0);
  // Check field compatibility
  if(!areFieldsCompatible(sourceFieldIndex, destinationFieldIndex)){
    fm.mappingState = mdtSqlCopierFieldMapping::MappingError;
    return;
  }
  // Done for this field
  fm.mappingState = mdtSqlCopierFieldMapping::MappingComplete;
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
    Q_ASSERT(item.destinationFieldIndexList().count() == 1);
    const int sourceFieldIndex = item.sourceFieldIndex();
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

// void TableMapping::updateTableMappingState()
// {
//   // Check if both tables are set
//   if( sourceTableName().isEmpty() || destinationTableName().isEmpty() ){
//     pvMappingState = MappingNotSet;
//     return;
//   }
//   // Check state of each field mapping and deduce table mapping state
//   for(const auto & fm : pvFieldMappingList){
//     switch(fm.mappingState){
//       case mdtSqlCopierFieldMapping::MappingError:
//         pvMappingState = MappingError;
//         return;
//       case mdtSqlCopierFieldMapping::MappingNotSet:
//       case mdtSqlCopierFieldMapping::MappingPartial:
//         pvMappingState = MappingPartial;
//         return;
//       case mdtSqlCopierFieldMapping::MappingComplete:
//         break;
//     }
//   }
//   // All checks successfully passed
//   pvMappingState = MappingComplete;
// }

}}} // namespace mdt{ namespace sql{ namespace copier{
