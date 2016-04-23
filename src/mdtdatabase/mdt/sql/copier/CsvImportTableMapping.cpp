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
#include "CsvImportTableMapping.h"
#include "mdtSqlCsvData.h"
#include "FieldTypeMapCheck.h"
#include "mdt/csv/FieldType.h"

//#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

bool CsvImportTableMapping::setSourceCsvSettings(const mdtCsvParserSettings & settings)
{
  Q_ASSERT(settings.isValid());

  pvSourceCsvSettings = settings;
  if(sourceCsvIsSet()){
    return parseSourceHeader();
  }
  return true;
}

bool CsvImportTableMapping::setDestinationTable(const QString & tableName, const QSqlDatabase & db)
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

QStringList CsvImportTableMapping::fetchSourceTableFieldNameList() const
{
  QStringList fieldNames;

  for(const auto & var : pvSourceHeader.columnDataList){
    fieldNames.append(var.toString());
  }

  return fieldNames;
}

QString CsvImportTableMapping::fetchDestinationTableFieldTypeNameAt(int fieldIndex) const
{
  return pvDestinationTable.fieldTypeName(fieldIndex, mdtSqlDriverType::typeFromName(pvDestinationDatabase.driverName()));
}

QString CsvImportTableMapping::fetchSourceTableFieldTypeNameAt(int fieldIndex) const
{
  Q_ASSERT(fieldIndex >= 0);
  Q_ASSERT(fieldIndex < pvSourceFormat.fieldCount());
  return mdt::csv::FieldType::nameFromType(pvSourceFormat.fieldType(fieldIndex));
}

TableMapping::FieldKeyType CsvImportTableMapping::fetchDestinationTableFieldKeyType(int fieldIndex) const
{
  if(pvDestinationTable.isFieldPartOfPrimaryKey(fieldIndex)){
    return PrimaryKey;
  }
  return NotAKey;
}

bool CsvImportTableMapping::areFieldsCompatible(int sourceFieldIndex, int destinationFieldIndex) const
{
  Q_ASSERT(sourceFieldIndex >= 0);
  Q_ASSERT(sourceFieldIndex < pvSourceFormat.fieldCount());
  Q_ASSERT(destinationFieldIndex >= 0);
  Q_ASSERT(destinationFieldIndex < pvDestinationTable.fieldCount());

  auto sourceFieldType = pvSourceFormat.fieldType(sourceFieldIndex);
  auto destinationFieldType = pvDestinationTable.field(destinationFieldIndex).getFieldType(mdtSqlDriverType::typeFromName(pvDestinationDatabase.driverName()));

  return FieldTypeMapCheck::canCopy(sourceFieldType, destinationFieldType);
}

/// Keep until new version was written
// void mdtSqlCsvImportTableMapping::updateCsvSourceFormat(mdtSqlCopierFieldMapping & fm)
// {
//   if(fm.isNull()){
//     return;
//   }
//   Q_ASSERT(fm.sourceField.fieldIndex() >= 0);
//   Q_ASSERT(fm.sourceField.fieldIndex() < sourceTable().fieldCount());
//   Q_ASSERT(fm.destinationFieldIndex >= 0);
//   Q_ASSERT(fm.destinationFieldIndex < pvDestinationTable.fieldCount());
// 
//   auto destinationFieldFormat = pvDestinationTable.field(fm.destinationFieldIndex).type();
//   auto csvFieldFormat = mdtSqlCsvData::csvFieldTypeFromMdtSqlFieldType(destinationFieldFormat);
//   sourceTable().setFieldType(fm.sourceField.fieldIndex(), csvFieldFormat);
// }

}}} // namespace mdt{ namespace sql{ namespace copier{
