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
#include "mdtSqlTableCopierMapping.h"
#include <QSqlField>

bool mdtSqlTableCopierMapping::setSourceDatabase(const QSqlDatabase & db)
{
  clearFieldMapping();
  pvSourceTable.clear();
  if(!pvSourceTable.setDriverName(db.driverName())){
    pvLastError = pvSourceTable.lastError();
    return false;
  }
  pvSourceDatabase = db;

  return true;
}

bool mdtSqlTableCopierMapping::setDestinationDatabase(const QSqlDatabase & db)
{
  clearFieldMapping();
  pvDestinationTable.clear();
  if(!pvDestinationTable.setDriverName(db.driverName())){
    pvLastError = pvDestinationTable.lastError();
    return false;
  }
  pvDestinationDatabase = db;

  return true;
}

bool mdtSqlTableCopierMapping::setSourceTable(const QString & tableName)
{
  clearFieldMapping();
  if(!pvSourceTable.setupFromTable(tableName, pvSourceDatabase)){
    pvLastError = pvSourceTable.lastError();
    return false;
  }
  resetFieldMapping();

  return true;
}

bool mdtSqlTableCopierMapping::setDestinationTable(const QString & tableName)
{
  clearFieldMapping();
  if(!pvDestinationTable.setupFromTable(tableName, pvDestinationDatabase)){
    pvLastError = pvDestinationTable.lastError();
    return false;
  }
  resetFieldMapping();

  return true;
}

// void mdtSqlTableCopierMapping::addFieldMapping(const mdtSqlCopierFieldMapping& fm)
// {
//   Q_ASSERT(fm.sourceFieldIndex >= 0);
//   Q_ASSERT(fm.sourceFieldIndex < pvSourceTable.fieldCount());
//   Q_ASSERT(fm.destinationFieldIndex >= 0);
//   Q_ASSERT(fm.destinationFieldIndex < pvDestinationTable.fieldCount());
// 
//   pvFieldMappingList.append(fm);
// }

// void mdtSqlTableCopierMapping::addFieldMapping(int sourceFieldIndex, const QString & destinationFieldName)
// {
//   Q_ASSERT(sourceFieldIndex >= 0);
//   Q_ASSERT(sourceFieldIndex < pvSourceTable.fieldCount());
// 
//   int destinationFieldIndex = pvDestinationTable.fieldIndex(destinationFieldName);
//   Q_ASSERT(destinationFieldIndex >= 0);
//   Q_ASSERT(destinationFieldIndex < pvDestinationTable.fieldCount());
//   mdtSqlCopierFieldMapping fm;
//   fm.sourceFieldIndex = sourceFieldIndex;
//   fm.destinationFieldIndex = destinationFieldIndex;
//   pvFieldMappingList.append(fm);
// }

void mdtSqlTableCopierMapping::resetFieldMapping()
{
  int n = pvSourceTable.fieldCount();

  pvFieldMappingList.clear();
  for(int i = 0; i < n; ++i){
    mdtSqlCopierFieldMapping fm;
    fm.sourceFieldIndex = i;
    pvFieldMappingList.append(fm);
  }
}

void mdtSqlTableCopierMapping::clearFieldMapping()
{
  pvFieldMappingList.clear();
}

void mdtSqlTableCopierMapping::generateFieldMappingByName()
{
  resetFieldMapping();
  for(auto & fm : pvFieldMappingList){
    // Get source field
    Q_ASSERT(fm.sourceFieldIndex >= 0);
    Q_ASSERT(fm.sourceFieldIndex < pvSourceTable.fieldCount());
    QSqlField sourceField = pvSourceTable.field(fm.sourceFieldIndex);
    // Get destination field index that matches source field name
    int destinationFieldIndex = pvDestinationTable.fieldIndex(sourceField.name());
    /// \todo If destination field exists, do some check here
    
    // Update field mapping
    fm.destinationFieldIndex = destinationFieldIndex;
  }
}

void mdtSqlTableCopierMapping::setDestinationField(int index, const QString & fieldName)
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
}
