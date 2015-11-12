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
#include "mdtSqlDatabaseCopierMapping.h"
#include <QLatin1String>

bool mdtSqlDatabaseCopierMapping::setSourceDatabase(const QSqlDatabase & db)
{
  clearTableMapping();
  pvSourceDatabase = db;

  return resetTableMapping();
}

bool mdtSqlDatabaseCopierMapping::setDestinationDatabase(const QSqlDatabase & db)
{
  clearTableMapping();
  pvDestinationDatabase = db;

  return resetTableMapping();
}

QStringList mdtSqlDatabaseCopierMapping::getAvailableSourceTableNameList() const
{
  return getTables(pvSourceDatabase);
}

QStringList mdtSqlDatabaseCopierMapping::getAvailableDestinationTableNameList() const
{
  return getTables(pvDestinationDatabase);
}

bool mdtSqlDatabaseCopierMapping::resetTableMapping()
{
  QStringList destinationTableNameList = getTables(pvDestinationDatabase);

  pvTableMappingList.clear();
  destinationTableNameList.sort();
  for(const auto & destinationTableName : destinationTableNameList){
    mdtSqlDatabaseCopierTableMapping tm;
    if(!tm.setDestinationTable(destinationTableName, pvDestinationDatabase)){
      clearTableMapping();
      pvLastError = tm.lastError();
      return false;
    }
//     if(!tm.setSourceTable(sourceTableName, pvSourceDatabase)){
//       clearTableMapping();
//       pvLastError = tm.lastError();
//       return false;
//     }
    pvTableMappingList.append(tm);
  }

  return true;
}

// bool mdtSqlDatabaseCopierMapping::resetTableMapping()
// {
//   QStringList sourceTableNameList = getTables(pvSourceDatabase);
// 
//   pvTableMappingList.clear();
//   sourceTableNameList.sort();
//   for(const auto & sourceTableName : sourceTableNameList){
//     mdtSqlDatabaseCopierTableMapping tm;
//     if(!tm.setSourceTable(sourceTableName, pvSourceDatabase)){
//       clearTableMapping();
//       pvLastError = tm.lastError();
//       return false;
//     }
//     pvTableMappingList.append(tm);
//   }
// 
//   return true;
// }

void mdtSqlDatabaseCopierMapping::clearTableMapping()
{
  pvTableMappingList.clear();
}

bool mdtSqlDatabaseCopierMapping::generateTableMappingByName()
{
  if(!resetTableMapping()){
    return false;
  }
  QStringList sourceTableNameList = getTables(pvSourceDatabase);
  for(auto & tm : pvTableMappingList){
    QString destinationTableName = tm.destinationTableName();
    if(sourceTableNameList.contains(destinationTableName)){
      /// \todo We ignore errors, the mapping state will indicate failures
      tm.setSourceTable(destinationTableName, pvSourceDatabase);
      tm.generateFieldMappingByName();
    }
  }

  return true;
}

// bool mdtSqlDatabaseCopierMapping::generateTableMappingByName()
// {
//   if(!resetTableMapping()){
//     return false;
//   }
//   QStringList destinationTableNameList = getTables(pvDestinationDatabase);
//   for(auto & tm : pvTableMappingList){
//     QString sourceTableName = tm.sourceTableName();
//     if(destinationTableNameList.contains(sourceTableName)){
//       /// \todo We ignore errors, the mapping state will indicate failures
//       tm.setDestinationTable(sourceTableName, pvDestinationDatabase);
//       tm.generateFieldMappingByName();
//     }
//   }
// 
//   return true;
// }

// QVector<mdtSqlDatabaseCopierTableMapping> mdtSqlDatabaseCopierMapping::getCompletedTableMappingList() const
// {
//   QVector<mdtSqlDatabaseCopierTableMapping> tmList;
// 
//   for(const auto & tm : pvTableMappingList){
//     if(tm.mappingState() == mdtSqlDatabaseCopierTableMapping::MappingComplete){
//       tmList.append(tm);
//     }
//   }
// 
//   return tmList;
// }

QStringList mdtSqlDatabaseCopierMapping::getTables(const QSqlDatabase & db) const
{
  auto driverType = mdtSqlDriverType::typeFromName(db.driverName());

  switch(driverType){
    case mdtSqlDriverType::SQLite:
      return getTablesSqlite(db);
    case mdtSqlDriverType::MariaDB:
    case mdtSqlDriverType::MySQL:
    case mdtSqlDriverType::Unknown:
      return db.tables(QSql::Tables);
  }

  return QStringList();
}

QStringList mdtSqlDatabaseCopierMapping::getTablesSqlite(const QSqlDatabase & db) const
{
  QStringList tables;

  for(const auto & tableName : db.tables(QSql::Tables)){
    if(!tableName.startsWith(QLatin1String("sqlite_"))){
      tables.append(tableName);
    }
  }

  return tables;
}
