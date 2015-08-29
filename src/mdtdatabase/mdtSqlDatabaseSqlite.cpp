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
#include "mdtSqlDatabaseSqlite.h"
#include "mdtSqlDriverType.h"
#include <QString>

mdtSqlDatabaseSqlite::mdtSqlDatabaseSqlite()
{
}

mdtSqlDatabaseSqlite::mdtSqlDatabaseSqlite(const QSqlDatabase & db)
 : pvDatabase(db)
{
  Q_ASSERT(mdtSqlDriverType::typeFromName(db.driverName()) == mdtSqlDriverType::SQLite);
}

void mdtSqlDatabaseSqlite::clear()
{
  /// \todo close
  pvDatabase = QSqlDatabase();
}

void mdtSqlDatabaseSqlite::setDatabase(const QSqlDatabase & db)
{
  Q_ASSERT(mdtSqlDriverType::typeFromName(db.driverName()) == mdtSqlDriverType::SQLite);
  pvDatabase = db;
}

QStringList mdtSqlDatabaseSqlite::getConnectionNames()
{
  QStringList connectionNames;

  for(const auto & cnnName : QSqlDatabase::connectionNames()){
    QSqlDatabase db = QSqlDatabase::database(cnnName, false);
    if(mdtSqlDriverType::typeFromName(db.driverName()) == mdtSqlDriverType::SQLite){
      connectionNames.append(cnnName);
    }
  }

  return connectionNames;
}
