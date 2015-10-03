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
#include "mdtSqlPrimaryKey.h"
#include "mdtSqlDriverType.h"
#include <QSqlDatabase>
#include <QSqlDriver>

QString mdtSqlPrimaryKey::getSql(const QSqlDatabase & db) const
{
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);

  switch(mdtSqlDriverType::typeFromName(db.driverName())){
    case mdtSqlDriverType::SQLite:
      return getSqlSqlite(driver);
    case mdtSqlDriverType::MariaDB:
    case mdtSqlDriverType::MySQL:
    case mdtSqlDriverType::Unknown:
      break;
  }

  return QString();
}

QString mdtSqlPrimaryKey::getSqlSqlite(const QSqlDriver*const driver) const
{
  Q_ASSERT(driver != nullptr);

  QString sql;
  int lastFieldIndex = pvFields.size() - 1;

  if(lastFieldIndex < 0){
    return sql;
  }
  sql = "PRIMARY KEY (";
  for(int i = 0; i < lastFieldIndex; ++i){
    sql += driver->escapeIdentifier(pvFields.at(i).name(), QSqlDriver::FieldName) + ",";
  }
  sql += driver->escapeIdentifier(pvFields.at(lastFieldIndex).name(), QSqlDriver::FieldName) + ")";

  return sql;
}
