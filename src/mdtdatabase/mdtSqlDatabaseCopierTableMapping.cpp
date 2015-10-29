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
#include "mdtSqlDatabaseCopierTableMapping.h"
#include "mdtSqlField.h"
#include "mdtSqlDriverType.h"
#include <QSqlDriver>

//#include <QDebug>

mdtSqlDatabaseCopierTableMapping::mdtSqlDatabaseCopierTableMapping()
 : pvMappingState(MappingNotSet)
{
}

// bool mdtSqlDatabaseCopierTableMapping::setSourceDatabase(const QSqlDatabase & db)
// {
//   clearFieldMapping();
//   pvSourceTable.clear();
//   if(!pvSourceTable.setDriverName(db.driverName())){
//     pvLastError = pvSourceTable.lastError();
//     return false;
//   }
//   pvSourceDatabase = db;
// 
//   return true;
// }

// bool mdtSqlDatabaseCopierTableMapping::setDestinationDatabase(const QSqlDatabase & db)
// {
//   clearFieldMapping();
//   pvDestinationTable.clear();
//   if(!pvDestinationTable.setDriverName(db.driverName())){
//     pvLastError = pvDestinationTable.lastError();
//     return false;
//   }
//   pvDestinationDatabase = db;
// 
//   return true;
// }

bool mdtSqlDatabaseCopierTableMapping::setSourceTable(const QString & tableName, const QSqlDatabase & db)
{
  clearFieldMapping();
  if(!pvSourceTable.setupFromTable(tableName, db)){
    pvLastError = pvSourceTable.lastError();
    return false;
  }
  pvSourceDatabase = db;
  resetFieldMapping();

  return true;
}

bool mdtSqlDatabaseCopierTableMapping::setDestinationTable(const QString & tableName, const QSqlDatabase & db)
{
  clearFieldMapping();
  if(!pvDestinationTable.setupFromTable(tableName, db)){
    pvLastError = pvDestinationTable.lastError();
    return false;
  }
  pvDestinationDatabase = db;
  resetFieldMapping();

  return true;
}

void mdtSqlDatabaseCopierTableMapping::resetFieldMapping()
{
  int n = pvSourceTable.fieldCount();

  clearFieldMapping();
  for(int i = 0; i < n; ++i){
    mdtSqlCopierFieldMapping fm;
    fm.sourceFieldIndex = i;
    pvFieldMappingList.append(fm);
  }
}

void mdtSqlDatabaseCopierTableMapping::clearFieldMapping()
{
  pvFieldMappingList.clear();
  pvMappingState = MappingNotSet;
}

void mdtSqlDatabaseCopierTableMapping::generateFieldMappingByName()
{
  bool mismatchDetected = false;

  resetFieldMapping();
  for(auto & fm : pvFieldMappingList){
    // Get source field
    Q_ASSERT(fm.sourceFieldIndex >= 0);
    Q_ASSERT(fm.sourceFieldIndex < pvSourceTable.fieldCount());
    mdtSqlField sourceField = pvSourceTable.field(fm.sourceFieldIndex);
    // Get destination field index that matches source field name
    int destinationFieldIndex = pvDestinationTable.fieldIndex(sourceField.name());
    /// \todo If destination field exists, do some check here
    
    // Update field mapping
    fm.destinationFieldIndex = destinationFieldIndex;
  }
  if(mismatchDetected){
    pvMappingState = MappingPartial;
  }else{
    pvMappingState = MappingComplete;
  }
}

void mdtSqlDatabaseCopierTableMapping::setDestinationField(int index, const QString & fieldName)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFieldMappingList.size());

  auto fm = pvFieldMappingList.at(index);

  if(fieldName.isEmpty()){
    fm.destinationFieldIndex = -1;
  }else{
    fm.destinationFieldIndex = pvDestinationTable.fieldIndex(fieldName);
  }
  pvFieldMappingList[index] = fm;
  // Check if mapping is now complete and update state
  if(mappingIsCompete()){
    pvMappingState = MappingComplete;
  }else{
    pvMappingState = MappingPartial;
  }
}

QString mdtSqlDatabaseCopierTableMapping::sourceFieldName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFieldMappingList.size());

  int sourceFieldIndex = pvFieldMappingList.at(index).sourceFieldIndex;
  Q_ASSERT(sourceFieldIndex < pvSourceTable.fieldCount());
  if(sourceFieldIndex < 0){
    return QString();
  }
  return pvSourceTable.fieldName(sourceFieldIndex);
}

QString mdtSqlDatabaseCopierTableMapping::sourceFieldTypeName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFieldMappingList.size());

  int sourceFieldIndex = pvFieldMappingList.at(index).sourceFieldIndex;
  Q_ASSERT(sourceFieldIndex < pvSourceTable.fieldCount());
  if(sourceFieldIndex < 0){
    return QString();
  }
  return pvSourceTable.fieldTypeName(sourceFieldIndex, mdtSqlDriverType::typeFromName(pvSourceDatabase.driverName()));
}

QString mdtSqlDatabaseCopierTableMapping::destinationFieldName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFieldMappingList.size());

  int destinationFieldIndex = pvFieldMappingList.at(index).destinationFieldIndex;
  Q_ASSERT(destinationFieldIndex < pvDestinationTable.fieldCount());
  if(destinationFieldIndex < 0){
    return QString();
  }
  return pvDestinationTable.fieldName(destinationFieldIndex);
}

QString mdtSqlDatabaseCopierTableMapping::destinationFieldTypeName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFieldMappingList.size());

  int destinationFieldIndex = pvFieldMappingList.at(index).destinationFieldIndex;
  Q_ASSERT(destinationFieldIndex < pvDestinationTable.fieldCount());
  if(destinationFieldIndex < 0){
    return QString();
  }
  return pvDestinationTable.fieldTypeName(destinationFieldIndex, mdtSqlDriverType::typeFromName(pvDestinationDatabase.driverName()));
}

bool mdtSqlDatabaseCopierTableMapping::mappingIsCompete()
{
  // Check if both tables are set
  if( pvSourceTable.tableName().isEmpty() || pvDestinationTable.tableName().isEmpty() ){
    return false;
  }
  // Check field mapping
  for(const auto & fm : pvFieldMappingList){
    // Check if source and destination is mapped
    if(fm.isNull()){
      return false;
    }
    /// \todo Type and other checks
  }

  return true;
}

QString mdtSqlDatabaseCopierTableMapping::getSqlForSourceTableSelect(const QSqlDatabase & db) const
{
  QString sql;
  QStringList fields;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);
  int lastIndex;

  // Build list of mapped fields
  for(const auto & fm : pvFieldMappingList){
    if(fm.destinationFieldIndex >= 0){
      Q_ASSERT(fm.sourceFieldIndex >= 0);
      Q_ASSERT(fm.sourceFieldIndex < pvSourceTable.fieldCount());
      fields.append(pvSourceTable.fieldName(fm.sourceFieldIndex));
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
  sql += " FROM " + driver->escapeIdentifier(pvSourceTable.tableName(), QSqlDriver::TableName);

  return sql;
}

QString mdtSqlDatabaseCopierTableMapping::getSqlForDestinationTableInsert(const QSqlDatabase & db) const
{
  QString sql;
  QStringList fields;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);
  int lastIndex;

  // Build list of mapped fields
  for(const auto & fm : pvFieldMappingList){
    if(fm.destinationFieldIndex >= 0){
      Q_ASSERT(fm.destinationFieldIndex < pvDestinationTable.fieldCount());
      fields.append(pvDestinationTable.fieldName(fm.destinationFieldIndex));
    }
  }
  lastIndex = fields.size() - 1;
  // If no mapping was set, simply return a empty statement
  if(lastIndex < 0){
    return sql;
  }
  // Build statement
  sql = "INSERT INTO " + driver->escapeIdentifier(pvDestinationTable.tableName(), QSqlDriver::TableName) + " (";
  for(int i = 0; i < lastIndex; ++i){
    sql += driver->escapeIdentifier(fields.at(i), QSqlDriver::FieldName) + ",";
  }
  sql += driver->escapeIdentifier(fields.at(lastIndex), QSqlDriver::FieldName);
  sql += ") VALUES (";
  for(int i = 0; i < lastIndex; ++i){
    sql += "?,";
  }
  sql += "?)";

  return sql;
}
