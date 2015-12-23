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

mdtSqlCopierTableMapping::mdtSqlCopierTableMapping()
 : pvMappingState(MappingNotSet)
{
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
