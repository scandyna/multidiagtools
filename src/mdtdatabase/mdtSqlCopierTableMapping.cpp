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

  auto fm = pvFieldMappingList.at(index);

  // Update source field
  updateSourceField(fm, fieldName);
  pvFieldMappingList[index] = fm;
  // Update table mapping state
  updateTableMappingState();
}

void mdtSqlCopierTableMapping::setSourceFixedValue(int index, const QVariant & value)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFieldMappingList.size());
  Q_ASSERT(pvFieldMappingList.at(index).sourceType == mdtSqlCopierFieldMapping::FixedValue);
  pvFieldMappingList[index].sourceFixedValue = value;
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
    if(fm.sourceFieldIndex >= 0){
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
  QStringList fields;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);
  int lastIndex;

  // Build list of mapped fields
  for(int i = 0; i < fieldCount(); ++i){
    const auto fm = fieldMappingAt(i);
    if(fm.sourceFieldIndex >= 0){
      fields.append(destinationFieldName(i));
    }
  }

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
    sql += "?,";
  }
  sql += "?)";

  return sql;
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
