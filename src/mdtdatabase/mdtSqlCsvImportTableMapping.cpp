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
#include "mdtSqlCsvImportTableMapping.h"
#include "mdtSqlCsvData.h"

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

void mdtSqlCsvImportTableMapping::setSourceField(int index, const QString & fieldName)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  auto fm = fieldMappingAt(index);
//   auto destinationDriverType = mdtSqlDriverType::typeFromName(pvDestinationDatabase.driverName());
// 
//   if(destinationDriverType == mdtSqlDriverType::Unknown){
//     return;
//   }
  if(fieldName.isEmpty()){
    fm.sourceFieldIndex = -1;
  }else{
    fm.sourceFieldIndex = sourceTable().fieldIndex(fieldName);
  }
  updateCsvSourceFormat(fm);
  updateFieldMappingState(fm/*, destinationDriverType*/);
  updateFieldMappingAt(index, fm);
  // Update table mapping state
  updateTableMappingState();
}

QString mdtSqlCsvImportTableMapping::sourceFieldName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  const int sourceFieldIndex = fieldMappingAt(index).sourceFieldIndex;
  Q_ASSERT(sourceFieldIndex < sourceTable().fieldCount());
  if(sourceFieldIndex < 0){
    return QString();
  }
  return sourceTable().fieldName(sourceFieldIndex);
}

QString mdtSqlCsvImportTableMapping::sourceFieldTypeName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < fieldCount());

  const int sourceFieldIndex = fieldMappingAt(index).sourceFieldIndex;
  Q_ASSERT(sourceFieldIndex < sourceTable().fieldCount());
  if(sourceFieldIndex < 0){
    return QString();
  }
  return sourceTable().fieldTypeName(sourceFieldIndex);
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

void mdtSqlCsvImportTableMapping::generateFieldMappingByName()
{

}

void mdtSqlCsvImportTableMapping::updateFieldMappingState(mdtSqlCopierFieldMapping & fm/*, mdtSqlDriverType::Type destinationDriverType*/)
{
//   Q_ASSERT(destinationDriverType != mdtSqlDriverType::Unknown);

  if(fm.isNull()){
    fm.mappingState = mdtSqlCopierFieldMapping::MappingNotSet;
    return;
  }
  Q_ASSERT(fm.sourceFieldIndex >= 0);
  Q_ASSERT(fm.sourceFieldIndex < sourceTable().fieldCount());
  Q_ASSERT(fm.destinationFieldIndex >= 0);
  Q_ASSERT(fm.destinationFieldIndex < pvDestinationTable.fieldCount());
  // Do checks regarding field types
  auto sourceFieldType = sourceTable().fieldType(fm.sourceFieldIndex);
  auto destinationFieldType = pvDestinationTable.field(fm.destinationFieldIndex).type();
  if(sourceFieldType == mdtSqlCsvData::csvFieldTypeFromMdtSqlFieldType(destinationFieldType)){
    fm.mappingState = mdtSqlCopierFieldMapping::MappingComplete;
  }else{
    fm.mappingState = mdtSqlCopierFieldMapping::MappingError;
  }
}

void mdtSqlCsvImportTableMapping::updateCsvSourceFormat(mdtSqlCopierFieldMapping & fm)
{
  Q_ASSERT(!fm.isNull());

  Q_ASSERT(fm.sourceFieldIndex >= 0);
  Q_ASSERT(fm.sourceFieldIndex < sourceTable().fieldCount());
  Q_ASSERT(fm.destinationFieldIndex >= 0);
  Q_ASSERT(fm.destinationFieldIndex < pvDestinationTable.fieldCount());

  auto destinationFieldFormat = pvDestinationTable.field(fm.destinationFieldIndex).type();
  auto csvFieldFormat = mdtSqlCsvData::csvFieldTypeFromMdtSqlFieldType(destinationFieldFormat);
  sourceTable().setFieldType(fm.sourceFieldIndex, csvFieldFormat);
}
