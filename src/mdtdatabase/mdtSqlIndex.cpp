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
#include "mdtSqlIndex.h"
#include <QSqlDatabase>
#include <QSqlDriver>

void mdtSqlIndex::clear()
{
  mdtSqlIndexBase::clear();
  pvIsUnique = false;
  pvTableName.clear();
}

QString mdtSqlIndex::getSqlForDrop(const QSqlDatabase& db, const QString & databaseName) const
{
  QString sql;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);

  sql = "DROP INDEX IF EXISTS ";
  if(!databaseName.isEmpty()){
    sql += driver->escapeIdentifier(databaseName, QSqlDriver::TableName) + ".";
  }
  sql += driver->escapeIdentifier(name(), QSqlDriver::TableName);

  return sql;
}

QString mdtSqlIndex::getSqlForCreate(const QSqlDatabase & db, const QString & databaseName) const
{
  QString sql;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);
  int lastFieldIndex = pvFields.size() - 1;

  if(lastFieldIndex < 0){
    return sql;
  }
  if(pvIsUnique){
    sql = "CREATE UNIQUE INDEX ";
  }else{
    sql = "CREATE INDEX ";
  }
  if(!databaseName.isEmpty()){
    sql += driver->escapeIdentifier(databaseName, QSqlDriver::TableName) + ".";
  }
  sql += driver->escapeIdentifier(name(), QSqlDriver::TableName);
  sql += " ON " + driver->escapeIdentifier(pvTableName, QSqlDriver::TableName) + " (";
  for(int i = 0; i < lastFieldIndex; ++i){
    sql += driver->escapeIdentifier(pvFields.at(i), QSqlDriver::FieldName) + ",";
  }
  sql += driver->escapeIdentifier(pvFields.at(lastFieldIndex), QSqlDriver::FieldName) + ")";

  return sql;
}
