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
#include "CsvFileImportMapping.h"
#include "mdt/sql/Database.h"

namespace mdt{ namespace sql{ namespace copier{

bool CsvFileImportMapping::setDestinationDatabase(const QSqlDatabase & db)
{
  pvTableMappingList.clear();
  pvDestinationDatabase = db;

  return resetMapping();
}

bool CsvFileImportMapping::resetMapping()
{
  using mdt::sql::Database;

  QStringList destinationTableNameList = Database::getTables(pvDestinationDatabase, Database::Tables);

  pvTableMappingList.clear();
  pvTableMappingList.reserve(destinationTableNameList.size());
  destinationTableNameList.sort();
  for(const auto & destinationTableName : destinationTableNameList){
    auto tm = std::make_shared<CsvFileImportTableMapping>();
    tm->setDestinationDatabase(pvDestinationDatabase);
    if(!tm->setDestinationTable(destinationTableName)){
      pvTableMappingList.clear();
      pvLastError = tm->lastError();
      return false;
    }
    pvTableMappingList.append(tm);
  }

  return true;
}

}}} // namespace mdt{ namespace sql{ namespace copier{
