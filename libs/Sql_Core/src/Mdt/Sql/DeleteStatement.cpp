/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "DeleteStatement.h"
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringBuilder>
#include <QSqlDriver>

namespace Mdt{ namespace Sql{

void DeleteStatement::setTableName(const QString& name)
{
  Q_ASSERT(!name.trimmed().isEmpty());

  mTableName = name;
}

void DeleteStatement::setConditions(const PrimaryKeyRecord& primaryKeyRecord)
{
  Q_ASSERT(!primaryKeyRecord.isNull());

  mPkrConditions = primaryKeyRecord;
}

void DeleteStatement::clear()
{
  mTableName.clear();
  mPkrConditions = PrimaryKeyRecord();
}

QString DeleteStatement::toPrepareStatementSql(const QSqlDatabase & db) const
{
  Q_ASSERT(db.isValid());

  QStringList fieldNames;

  QString sql = QLatin1String("DELETE FROM ") % escapeTableName(mTableName, db);
  if(!mPkrConditions.isNull()){
    sql += QLatin1String("\nWHERE ");
    fieldNames = mPkrConditions.toFieldNameList();
    for(int i = 0; i < fieldNames.size(); ++i){
      if(i > 0){
        sql += QLatin1String(" AND ");
      }
      sql += escapeFieldName( fieldNames.at(i), db ) % QLatin1String("=?");
    }
  }

  return sql;
}

QString DeleteStatement::escapeFieldName(const QString& fieldName, const QSqlDatabase& db)
{
  Q_ASSERT(db.driver() != nullptr);

  return db.driver()->escapeIdentifier(fieldName, QSqlDriver::FieldName);
}

QString DeleteStatement::escapeTableName(const QString& tableName, const QSqlDatabase& db)
{
  Q_ASSERT(db.driver() != nullptr);

  return db.driver()->escapeIdentifier(tableName, QSqlDriver::TableName);
}

}} // namespace Mdt{ namespace Sql{
