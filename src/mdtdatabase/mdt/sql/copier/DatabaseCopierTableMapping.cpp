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
#include "DatabaseCopierTableMapping.h"
#include "mdtSqlField.h"
#include "mdtSqlFieldType.h"
#include "mdtSqlPrimaryKey.h"
#include <QSqlDriver>
#include <QSqlRecord>

//#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

void DatabaseCopierTableMapping::setSourceDatabase(const QSqlDatabase & db)
{
  Q_ASSERT(db.isOpen());

  clearFieldMapping();
  pvSourceDatabase = db;
  resetFieldMapping();
}

bool DatabaseCopierTableMapping::setSourceTable(const QString & tableName)
{
  Q_ASSERT(pvSourceDatabase.isOpen());

  clearFieldMapping();
  if(!pvSourceTable.setupFromTable(tableName, pvSourceDatabase)){
    pvLastError = pvSourceTable.lastError();
    return false;
  }
  resetFieldMapping();

  return true;
}

bool DatabaseCopierTableMapping::setSourceTable(const QString & tableName, const QSqlDatabase & db)
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

bool DatabaseCopierTableMapping::setDestinationTable(const QString & tableName, const QSqlDatabase & db)
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

QString DatabaseCopierTableMapping::fetchSourceTableFieldTypeNameAt(int fieldIndex) const
{
  return pvSourceTable.fieldTypeName(fieldIndex, mdtSqlDriverType::typeFromName(pvSourceDatabase.driverName()));
}

TableMapping::FieldKeyType DatabaseCopierTableMapping::fetchSourceTableFieldKeyType(int fieldIndex) const
{
  if(pvSourceTable.isFieldPartOfPrimaryKey(fieldIndex)){
    return PrimaryKey;
  }
  return NotAKey;
}

QString DatabaseCopierTableMapping::fetchDestinationTableFieldTypeNameAt(int fieldIndex) const
{
  return pvDestinationTable.fieldTypeName(fieldIndex, mdtSqlDriverType::typeFromName(pvDestinationDatabase.driverName()));
}

TableMapping::FieldKeyType DatabaseCopierTableMapping::fetchDestinationTableFieldKeyType(int fieldIndex) const
{
  if(pvDestinationTable.isFieldPartOfPrimaryKey(fieldIndex)){
    return PrimaryKey;
  }
  return NotAKey;
}

bool DatabaseCopierTableMapping::areFieldsCompatible(int sourceFieldIndex, int destinationFieldIndex) const
{
  Q_ASSERT(sourceFieldIndex < pvSourceTable.fieldCount());
  Q_ASSERT(destinationFieldIndex < pvDestinationTable.fieldCount());

  // Get driver types needed for fetching field informations
  auto sourceDriverType = mdtSqlDriverType::typeFromName(pvSourceDatabase.driverName());
  auto destinationDriverType = mdtSqlDriverType::typeFromName(pvDestinationDatabase.driverName());
  // We cannot check anything without a known DB driver
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

}}} // namespace mdt{ namespace sql{ namespace copier{
