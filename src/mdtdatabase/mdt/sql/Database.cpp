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
#include "Database.h"
#include "mdtSqlDriverType.h"
#include <QLatin1String>

namespace mdt{ namespace sql{

QStringList Database::getTables(const QSqlDatabase & db, TableTypes types)
{
  QStringList tables;

  /*
   * If all tables where requested,
   *  just use QSqlDatabase implementation
   */
  if(types == AllTables){
    tables = db.tables(QSql::AllTables);
    return tables;
  }
  /*
   * Now we have special case if tables or system tables where requested
   */
  if( (types & Tables) || (types & SystemTables) ){
    auto driverType = mdtSqlDriverType::typeFromName(db.driverName());
    switch(driverType){
      case mdtSqlDriverType::SQLite:
        tables = getTablesSqlite(db, types & Tables, types & SystemTables);
        break;
      case mdtSqlDriverType::MariaDB:
      case mdtSqlDriverType::MySQL:
      case mdtSqlDriverType::Unknown:
        if(types & Tables){
          tables = db.tables(QSql::Tables);
        }
        if(types & SystemTables){
          tables += db.tables(QSql::SystemTables);
        }
        break;
    }
  }
  /*
   * Add views if requested
   */
  if(types & Views){
    tables += db.tables(QSql::Views);
  }
//   auto driverType = mdtSqlDriverType::typeFromName(db.driverName());
// 
//   switch(driverType){
//     case mdtSqlDriverType::SQLite:
//       tables = getTablesSqlite(db, types);
//       break;
//     case mdtSqlDriverType::MariaDB:
//     case mdtSqlDriverType::MySQL:
//     case mdtSqlDriverType::Unknown:
//       if(types & AllTables){
//         tables = db.tables(QSql::AllTables);
//       }else{
//         if(types & Tables){
//           tables += db.tables(QSql::Tables);
//         }
//         if(types & SystemTables){
//           tables += db.tables(QSql::SystemTables);
//         }
//         if(types & Views){
//           tables += db.tables(QSql::Views);
//         }
//       }
//       break;
//   }

  return tables;
}

QStringList Database::getTablesSqlite(const QSqlDatabase & db, TableTypes types)
{
  QStringList tables;

  if(types == Views){
    tables = db.tables(QSql::Views);
  }else{
    /*
     * QSqlDatabase::tables(QSql::Tables) returns also
     *  tyble that we consider as system tables (f.ex. sqlite_sequence).
     * QSqlDatabase::tables(QSql::SystemTables) does not return
     *  tables like sqlite_sequence.
     */
    const auto allTables = db.tables(QSql::Tables) + db.tables(QSql::SystemTables);
    for(const auto & table : allTables){
      
    }
    /*
     * If views are also requested, add them here (or not?)
     */
    
  }

  return tables;
}

QStringList Database::getTablesSqlite(const QSqlDatabase & db, bool userTables, bool systemTables)
{
  QStringList tables;

  /*
   * QSqlDatabase::tables(QSql::Tables) returns also
   *  tyble that we consider as system tables (f.ex. sqlite_sequence).
   * QSqlDatabase::tables(QSql::SystemTables) does not return
   *  tables like sqlite_sequence.
   */
  const auto allTables = db.tables(QSql::Tables) + db.tables(QSql::SystemTables);
  for(const auto & table : allTables){
    if(table.startsWith(QLatin1String("sqlite_"))){
      if(systemTables){
        tables.append(table);
      }
    }else{
      if(userTables){
        tables.append(table);
      }
    }
  }

  return tables;
}

}} // namespace mdt{ namespace sql{
