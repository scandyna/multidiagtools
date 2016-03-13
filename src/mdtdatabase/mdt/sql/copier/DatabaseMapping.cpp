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
#include "DatabaseMapping.h"
#include "mdt/sql/Database.h"
#include <QLatin1String>

namespace mdt{ namespace sql{ namespace copier{

mdtExpected<bool> DatabaseMapping::setSourceDatabase(const QSqlDatabase &db)
{
  clearMapping();
  pvSourceDatabase = db;

  return resetMapping();
}

mdtExpected<bool> DatabaseMapping::setDestinationDatabase(const QSqlDatabase &db)
{
  clearMapping();
  pvDestinationDatabase = db;

  return resetMapping();
}

mdtExpected<bool> DatabaseMapping::resetMapping()
{
  using mdt::sql::Database;

  QStringList destinationTableNameList = Database::getTables(pvDestinationDatabase, Database::Tables);

  pvTableMappingList.clear();
  pvTableMappingList.reserve(destinationTableNameList.size());
  destinationTableNameList.sort();
  for(const auto & destinationTableName : destinationTableNameList){
    auto tm = std::make_shared<mdt::sql::copier::DatabaseCopierTableMapping>();
    if(pvSourceDatabase.isOpen()){
      tm->setSourceDatabase(pvSourceDatabase);
    }
    if(!tm->setDestinationTable(destinationTableName, pvDestinationDatabase)){
      clearMapping();
      return tm->lastError();
    }
    pvTableMappingList.append(tm);
  }

  return true;
}

mdtExpected<bool> DatabaseMapping::generateTableMappingByName()
{
  using mdt::sql::Database;

  mdtExpected<bool> ret = resetMapping();
  if(!ret){
    return ret;
  }
  const auto sourceTableNameList = Database::getTables(pvSourceDatabase, Database::Tables);
  for(auto & tm : pvTableMappingList){
    const auto destinationTableName = tm->destinationTableName();
    if(sourceTableNameList.contains(destinationTableName)){
      /// \todo We ignore errors, the mapping state will indicate failures
      tm->setSourceTable(destinationTableName);
      tm->generateFieldMappingByName();
    }
  }

  return true;

}

void DatabaseMapping::clearMapping()
{
  pvTableMappingList.clear();
//   pvSourceDatabase = QSqlDatabase();
//   pvDestinationDatabase = QSqlDatabase();
}

}}} // namespace mdt{ namespace sql{ namespace copier{

