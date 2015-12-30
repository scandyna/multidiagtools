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
#include "mdtSqlCopierTableMapping.h"
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QStringList>

//#include <QDebug>

mdtSqlCopierTableMapping::mdtSqlCopierTableMapping()
 : pvMappingState(MappingNotSet)
{
}

void mdtSqlCopierTableMapping::setSourceType(int index, mdtSqlCopierFieldMapping::SourceType type)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFieldMappingList.size());
  pvFieldMappingList[index].sourceType = type;
}

void mdtSqlCopierTableMapping::setSourceField(int index, const QString & fieldName)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFieldMappingList.size());
  Q_ASSERT(pvFieldMappingList.at(index).sourceType == mdtSqlCopierFieldMapping::Field);

  auto & fm = pvFieldMappingList[index];

  // Update source field
  updateSourceField(fm, fieldName);
  updateFieldMappingState(fm);
  // Update table mapping state
  updateTableMappingState();
}

QString mdtSqlCopierTableMapping::sourceFieldName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());
  Q_ASSERT(pvFieldMappingList.at(index).sourceType == mdtSqlCopierFieldMapping::Field);

  int sourceFieldIndex = pvFieldMappingList.at(index).sourceFieldIndex;
  if(sourceFieldIndex < 0){
    return QString();
  }
  return fetchSourceFieldName(sourceFieldIndex);
}

QString mdtSqlCopierTableMapping::sourceFieldTypeName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());
  Q_ASSERT(pvFieldMappingList.at(index).sourceType == mdtSqlCopierFieldMapping::Field);

  int sourceFieldIndex = pvFieldMappingList.at(index).sourceFieldIndex;
  if(sourceFieldIndex < 0){
    return QString();
  }
  return fetchSourceFieldTypeName(sourceFieldIndex);
}

void mdtSqlCopierTableMapping::setSourceFixedValue(int index, const QVariant & value)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFieldMappingList.size());
  Q_ASSERT(pvFieldMappingList.at(index).sourceType == mdtSqlCopierFieldMapping::FixedValue);

  auto & fm = pvFieldMappingList[index];

  // Update source field
  fm.sourceFixedValue = value;
  updateFieldMappingState(fm);
  // Update table mapping state
  updateTableMappingState();
}

void mdtSqlCopierTableMapping::resetFieldMapping()
{
  int n = destinationTableFieldCount();

  clearFieldMapping();
  for(int i = 0; i < n; ++i){
    mdtSqlCopierFieldMapping fm;
    fm.destinationFieldIndex = i;
    pvFieldMappingList.append(fm);
  }
}

void mdtSqlCopierTableMapping::clearFieldMapping()
{
  pvFieldMappingList.clear();
  pvMappingState = MappingNotSet;
}

QString mdtSqlCopierTableMapping::getSqlForSourceTableCount(const QSqlDatabase & db) const
{
  QString sql;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);

  sql = "SELECT COUNT(*) FROM " + driver->escapeIdentifier(sourceTableName(), QSqlDriver::TableName);

  return sql;
}

QString mdtSqlCopierTableMapping::getSqlForSourceTableSelect(const QSqlDatabase & db) const
{
  QString sql;
  QStringList fields;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);
  int lastIndex;

  // Build list of mapped fields
  for(int i = 0; i < fieldCount(); ++i){
    const auto fm = fieldMappingAt(i);
    if( (fm.sourceFieldIndex >= 0) && (fm.sourceType == mdtSqlCopierFieldMapping::Field) ){
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

QString mdtSqlCopierTableMapping::getSqlForDestinationTableInsert(const QSqlDatabase & db) const
{
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
      if(fm.sourceType == mdtSqlCopierFieldMapping::Field){
        values.append("?");
      }else{
        values.append("'" + fm.sourceFixedValue.toString() + "'");
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

void mdtSqlCopierTableMapping::updateFieldMappingState(mdtSqlCopierFieldMapping & fm)
{
  // Check about null mapping
  if(fm.isNull()){
    fm.mappingState = mdtSqlCopierFieldMapping::MappingNotSet;
    return;
  }
  // Check for case of fixed value source
  if(fm.sourceType == mdtSqlCopierFieldMapping::FixedValue){
    if(fm.sourceFixedValue.isNull()){
      fm.mappingState = mdtSqlCopierFieldMapping::MappingError;
    }else{
      fm.mappingState = mdtSqlCopierFieldMapping::MappingComplete;
    }
    return;
  }
  // We have a field source, do checks
  Q_ASSERT(fm.sourceType == mdtSqlCopierFieldMapping::Field);
  const int sourceFieldIndex = fm.sourceFieldIndex;
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

void mdtSqlCopierTableMapping::updateTableMappingState()
{
  // Check if both tables are set
  if( sourceTableName().isEmpty() || destinationTableName().isEmpty() ){
    pvMappingState = MappingNotSet;
    return;
  }
  // Check state of each field mapping and deduce table mapping state
  for(const auto & fm : pvFieldMappingList){
    switch(fm.mappingState){
      case mdtSqlCopierFieldMapping::MappingError:
        pvMappingState = MappingError;
        return;
      case mdtSqlCopierFieldMapping::MappingNotSet:
      case mdtSqlCopierFieldMapping::MappingPartial:
        pvMappingState = MappingPartial;
        return;
      case mdtSqlCopierFieldMapping::MappingComplete:
        break;
    }
  }
  // All checks successfully passed
  pvMappingState = MappingComplete;
}
