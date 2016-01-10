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
#include "mdtSqlCsvImportTableMapping.h"
#include "mdtSqlCsvData.h"

//#include <QDebug>

using mdt::sql::copier::TableMapping;

bool mdtSqlCsvImportTableMapping::setDestinationTable(const QString & tableName, const QSqlDatabase & db)
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

QString mdtSqlCsvImportTableMapping::destinationFieldName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  const int destinationFieldIndex = fieldMappingAt(index).destinationFieldIndex;
  Q_ASSERT(destinationFieldIndex < pvDestinationTable.fieldCount());
  if(destinationFieldIndex < 0){
    return QString();
  }
  return pvDestinationTable.fieldName(destinationFieldIndex);
}

QString mdtSqlCsvImportTableMapping::destinationFieldTypeName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  const int destinationFieldIndex = fieldMappingAt(index).destinationFieldIndex;
  Q_ASSERT(destinationFieldIndex < pvDestinationTable.fieldCount());
  if(destinationFieldIndex < 0){
    return QString();
  }
  return pvDestinationTable.fieldTypeName(destinationFieldIndex, mdtSqlDriverType::typeFromName(pvDestinationDatabase.driverName()));
}

// void mdtSqlCsvImportTableMapping::generateFieldMappingByName()
// {
//   auto destinationDriverType = mdtSqlDriverType::typeFromName(pvDestinationDatabase.driverName());
// 
//   resetFieldMapping();
//   if(destinationDriverType == mdtSqlDriverType::Unknown){
//     return;
//   }
//   for(auto & fm : fieldMappingList()){
//     // Get source field
//     Q_ASSERT(fm.destinationFieldIndex >= 0);
//     Q_ASSERT(fm.destinationFieldIndex < pvDestinationTable.fieldCount());
//     mdtSqlField destinationField = pvDestinationTable.field(fm.destinationFieldIndex);
//     // Get source field index that matches destination field name
//     fm.sourceField.setFieldIndex(sourceTable().fieldIndex(destinationField.name()));
//     updateCsvSourceFormat(fm);
//     updateFieldMappingState(fm);
//   }
//   // Update table mapping state
//   updateTableMappingState();
// }

void mdtSqlCsvImportTableMapping::setSourceFieldIndex(mdtSqlCopierFieldMapping & fm, const QString & sourceFieldName)
{
  Q_ASSERT(!sourceFieldName.isEmpty());

  const int index = sourceTable().fieldIndex(sourceFieldName);
  fm.sourceField.setFieldIndex(index);
  updateCsvSourceFormat(fm);
}

QString mdtSqlCsvImportTableMapping::fetchDestinationTableFieldTypeNameAt(int fieldIndex) const
{
  return pvDestinationTable.fieldTypeName(fieldIndex, mdtSqlDriverType::typeFromName(pvDestinationDatabase.driverName()));
}

TableMapping::FieldKeyType mdtSqlCsvImportTableMapping::fetchDestinationTableFieldKeyType(int fieldIndex) const
{
  if(pvDestinationTable.isFieldPartOfPrimaryKey(fieldIndex)){
    return PrimaryKey;
  }
  return NotAKey;
}

bool mdtSqlCsvImportTableMapping::areFieldsCompatible(int sourceFieldIndex, int destinationFieldIndex) const
{
  Q_ASSERT(sourceFieldIndex < sourceTable().fieldCount());
  Q_ASSERT(destinationFieldIndex < pvDestinationTable.fieldCount());

  // Do checks regarding field types
  auto sourceFieldType = sourceTable().fieldType(sourceFieldIndex);
  auto destinationFieldType = pvDestinationTable.field(destinationFieldIndex).type();
  if(sourceFieldType == mdtSqlCsvData::csvFieldTypeFromMdtSqlFieldType(destinationFieldType)){
    return true;
  }

  return false;
}

void mdtSqlCsvImportTableMapping::updateCsvSourceFormat(mdtSqlCopierFieldMapping & fm)
{
  if(fm.isNull()){
    return;
  }
  Q_ASSERT(fm.sourceField.fieldIndex() >= 0);
  Q_ASSERT(fm.sourceField.fieldIndex() < sourceTable().fieldCount());
  Q_ASSERT(fm.destinationFieldIndex >= 0);
  Q_ASSERT(fm.destinationFieldIndex < pvDestinationTable.fieldCount());

  auto destinationFieldFormat = pvDestinationTable.field(fm.destinationFieldIndex).type();
  auto csvFieldFormat = mdtSqlCsvData::csvFieldTypeFromMdtSqlFieldType(destinationFieldFormat);
  sourceTable().setFieldType(fm.sourceField.fieldIndex(), csvFieldFormat);
}
