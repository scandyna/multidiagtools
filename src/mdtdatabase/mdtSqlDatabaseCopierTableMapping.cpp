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
#include "mdtSqlFieldType.h"
#include "mdtSqlPrimaryKey.h"
#include <QSqlDriver>
#include <QSqlRecord>

//#include <QDebug>

// mdtSqlDatabaseCopierTableMapping::mdtSqlDatabaseCopierTableMapping(const mdtSqlDatabaseCopierTableMapping & other)
//  : mdtSqlCopierTableMapping(other),
//    pvSourceDatabase(other.pvSourceDatabase),
//    pvSourceTable(other.pvSourceTable),
//    pvDestinationDatabase(other.pvDestinationDatabase),
//    pvDestinationTable(other.pvDestinationTable)
// {
// }

// mdtSqlDatabaseCopierTableMapping& mdtSqlDatabaseCopierTableMapping::operator=(const mdtSqlDatabaseCopierTableMapping & other)
// {
//   pvSourceDatabase = other.pvSourceDatabase;
//   pvSourceTable = other.pvSourceTable;
//   pvDestinationDatabase = other.pvDestinationDatabase;
//   pvDestinationTable = other.pvDestinationTable;
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

void mdtSqlDatabaseCopierTableMapping::generateFieldMappingByName()
{
//   auto sourceDriverType = mdtSqlDriverType::typeFromName(pvSourceDatabase.driverName());
//   auto destinationDriverType = mdtSqlDriverType::typeFromName(pvDestinationDatabase.driverName());

  resetFieldMapping();
//   if(sourceDriverType == mdtSqlDriverType::Unknown){
//     return;
//   }
//   if(destinationDriverType == mdtSqlDriverType::Unknown){
//     return;
//   }
  for(auto & fm : fieldMappingList()){
    // Get source field
    Q_ASSERT(fm.destinationFieldIndex >= 0);
    Q_ASSERT(fm.destinationFieldIndex < pvDestinationTable.fieldCount());
    mdtSqlField destinationField = pvDestinationTable.field(fm.destinationFieldIndex);
    // Get source field index that matches destination field name
    fm.sourceFieldIndex = pvSourceTable.fieldIndex(destinationField.name());
    updateFieldMappingState(fm);
  }
  // Update table mapping state
  updateTableMappingState();
}

QString mdtSqlDatabaseCopierTableMapping::destinationFieldName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  int destinationFieldIndex = fieldMappingAt(index).destinationFieldIndex;
  Q_ASSERT(destinationFieldIndex < pvDestinationTable.fieldCount());
  if(destinationFieldIndex < 0){
    return QString();
  }
  return pvDestinationTable.fieldName(destinationFieldIndex);
}

QString mdtSqlDatabaseCopierTableMapping::destinationFieldTypeName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  int destinationFieldIndex = fieldMappingAt(index).destinationFieldIndex;
  Q_ASSERT(destinationFieldIndex < pvDestinationTable.fieldCount());
  if(destinationFieldIndex < 0){
    return QString();
  }
  return pvDestinationTable.fieldTypeName(destinationFieldIndex, mdtSqlDriverType::typeFromName(pvDestinationDatabase.driverName()));
}

mdtSqlCopierTableMapping::FieldKeyType mdtSqlDatabaseCopierTableMapping::destinationFieldKeyType(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  const int destinationFieldIndex = fieldMappingAt(index).destinationFieldIndex;
  Q_ASSERT(destinationFieldIndex < pvDestinationTable.fieldCount());
  if(destinationFieldIndex < 0){
    return NotAKey;
  }
  if(pvDestinationTable.isFieldPartOfPrimaryKey(destinationFieldIndex)){
    return PrimaryKey;
  }
  return NotAKey;
}

void mdtSqlDatabaseCopierTableMapping::updateSourceField(mdtSqlCopierFieldMapping & fm, const QString & sourceFieldName)
{
  auto sourceDriverType = mdtSqlDriverType::typeFromName(pvSourceDatabase.driverName());
  auto destinationDriverType = mdtSqlDriverType::typeFromName(pvDestinationDatabase.driverName());

  if(sourceDriverType == mdtSqlDriverType::Unknown){
    return;
  }
  if(destinationDriverType == mdtSqlDriverType::Unknown){
    return;
  }
  if(sourceFieldName.isEmpty()){
    fm.sourceFieldIndex = -1;
  }else{
    fm.sourceFieldIndex = pvSourceTable.fieldIndex(sourceFieldName);
  }
}

QString mdtSqlDatabaseCopierTableMapping::fetchSourceFieldName(int sourceFieldIndex) const
{
  Q_ASSERT(sourceFieldIndex >= 0);
  Q_ASSERT(sourceFieldIndex < pvSourceTable.fieldCount());
  return pvSourceTable.fieldName(sourceFieldIndex);
}

QString mdtSqlDatabaseCopierTableMapping::fetchSourceFieldTypeName(int sourceFieldIndex) const
{
  Q_ASSERT(sourceFieldIndex >= 0);
  Q_ASSERT(sourceFieldIndex < pvSourceTable.fieldCount());
  return pvSourceTable.fieldTypeName(sourceFieldIndex, mdtSqlDriverType::typeFromName(pvSourceDatabase.driverName()));
}

mdtSqlCopierTableMapping::FieldKeyType mdtSqlDatabaseCopierTableMapping::fetchSourceFieldKeyType(int sourceFieldIndex) const
{
  Q_ASSERT(sourceFieldIndex >= 0);
  Q_ASSERT(sourceFieldIndex < pvSourceTable.fieldCount());

  if(pvSourceTable.isFieldPartOfPrimaryKey(sourceFieldIndex)){
    return PrimaryKey;
  }
  return NotAKey;
}

bool mdtSqlDatabaseCopierTableMapping::areFieldsCompatible(int sourceFieldIndex, int destinationFieldIndex) const
{
  Q_ASSERT(sourceFieldIndex < pvSourceTable.fieldCount());
  Q_ASSERT(destinationFieldIndex < pvDestinationTable.fieldCount());

  // Get driver types needed for fetching field informations
  auto sourceDriverType = mdtSqlDriverType::typeFromName(pvSourceDatabase.driverName());
  auto destinationDriverType = mdtSqlDriverType::typeFromName(pvDestinationDatabase.driverName());
  // Ww cannot check anything without a known DB driver
  if(sourceDriverType == mdtSqlDriverType::Unknown){
    return false;
  }
  if(destinationDriverType == mdtSqlDriverType::Unknown){
    return false;
  }
  // Do checks regarding field types
  auto sourceFieldType = pvSourceTable.field(sourceFieldIndex).getFieldType(sourceDriverType);
  auto destinationFieldType = pvDestinationTable.field(destinationFieldIndex).getFieldType(destinationDriverType);
  if(sourceFieldType == destinationFieldType){
    /// \todo Add length checking
    return true;
  }

  return false;
}
