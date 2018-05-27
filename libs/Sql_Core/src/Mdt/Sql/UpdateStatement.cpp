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
#include "UpdateStatement.h"
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringBuilder>
#include <QSqlDriver>

namespace Mdt{ namespace Sql{

void UpdateStatement::setTableName(const QString& name)
{
  Q_ASSERT(!name.trimmed().isEmpty());

  mTableName = name;
}

void UpdateStatement::addValue(const FieldName& fieldName, const QVariant& value)
{
  Q_ASSERT(!fieldName.isNull());
  Q_ASSERT(!mFieldValueMap.containsFieldName(fieldName.toString()));

  mFieldValueMap.addValue(fieldName, value);
}

void UpdateStatement::setConditions(const PrimaryKeyRecord& primaryKeyRecord)
{
  Q_ASSERT(!primaryKeyRecord.isNull());

  mPkrConditions = primaryKeyRecord;
}

void UpdateStatement::clear()
{
  mTableName.clear();
  mFieldValueMap.clear();
  mPkrConditions = PrimaryKeyRecord();
}

QString UpdateStatement::toPrepareStatementSql(const QSqlDatabase & db) const
{
  Q_ASSERT(db.isValid());

  QStringList fieldNames;

  QString sql = QLatin1String("UPDATE ") % escapeTableName(mTableName, db) \
              % QLatin1String("\nSET ");
  fieldNames = mFieldValueMap.toFieldNameList();
  for(int i = 0; i < fieldNames.size(); ++i){
    sql += escapeFieldName( fieldNames.at(i), db ) % QLatin1String("=?");
    if(i < (fieldNames.size()-1)){
      sql += QLatin1Char(',');
    }
  }
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

QString UpdateStatement::escapeFieldName(const QString& fieldName, const QSqlDatabase& db)
{
  Q_ASSERT(db.driver() != nullptr);

  return db.driver()->escapeIdentifier(fieldName, QSqlDriver::FieldName);
}

QString UpdateStatement::escapeTableName(const QString& tableName, const QSqlDatabase& db)
{
  Q_ASSERT(db.driver() != nullptr);

  return db.driver()->escapeIdentifier(tableName, QSqlDriver::TableName);
}

}} // namespace Mdt{ namespace Sql{
