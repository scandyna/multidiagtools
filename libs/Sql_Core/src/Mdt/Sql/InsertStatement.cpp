/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "InsertStatement.h"
#include <QStringList>
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringBuilder>
#include <QSqlDriver>
#include <QChar>

namespace Mdt{ namespace Sql{

void InsertStatement::setTableName(const QString& name)
{
  Q_ASSERT(!name.trimmed().isEmpty());

  mTableName = name;
}

void InsertStatement::addValue(const FieldName & fieldName, const QVariant & value)
{
  Q_ASSERT(!fieldName.isNull());
  Q_ASSERT(!mFieldValueMap.containsFieldName(fieldName.toString()));

  mFieldValueMap.addValue(fieldName, value);
}

void InsertStatement::clear()
{
  mTableName.clear();
  mFieldValueMap.clear();
}

QString InsertStatement::toPrepareStatementSql(const QSqlDatabase & db) const
{
  Q_ASSERT(db.isValid());

  QString sql;

  QStringList fieldNameList = mFieldValueMap.toFieldNameList();
  for(auto & fieldName : fieldNameList){
    fieldName = escapeFieldName(fieldName, db);
  }

  const int n = mFieldValueMap.size();
  QStringList placeHolders;
  placeHolders.reserve(n);
  for(int i = 0; i < n; ++i){
    placeHolders.append(QChar('?'));
  }
  // Build SQL
  sql = QLatin1String("INSERT INTO ") % escapeTableName(mTableName, db) \
      % QLatin1String(" (") % fieldNameList.join(QChar(',')) \
      % QLatin1String(") VALUES (") \
      % placeHolders.join(QChar(',')) \
      % QLatin1Char(')');

  return sql;
}

QString InsertStatement::escapeFieldName(const QString& fieldName, const QSqlDatabase& db)
{
  Q_ASSERT(db.driver() != nullptr);

  return db.driver()->escapeIdentifier(fieldName, QSqlDriver::FieldName);
}

QString InsertStatement::escapeTableName(const QString& tableName, const QSqlDatabase& db)
{
  Q_ASSERT(db.driver() != nullptr);

  return db.driver()->escapeIdentifier(tableName, QSqlDriver::TableName);
}

}} // namespace Mdt{ namespace Sql{
