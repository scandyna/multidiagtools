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
#include "InsertQuery.h"
#include "Schema/AutoIncrementPrimaryKey.h"
#include "Schema/Field.h"
#include "Schema/Table.h"
#include "Mdt/Sql/Error.h"
#include <QSqlRecord>
#include <QSqlDriver>
#include <QStringBuilder>
#include <QChar>

namespace Mdt{ namespace Sql{

InsertQuery::InsertQuery(const QSqlDatabase & db)
 : AbstractQuery(db),
   mQuery(db)
{
}

void InsertQuery::setTableName(const QString & name)
{
  mTableName = name;
}

void InsertQuery::setTable(const Schema::Table & table)
{
  mTableName = table.tableName();
}

void InsertQuery::addValue(const FieldName & fieldName, const QVariant & value)
{
  mFieldNameList << escapeFieldName(fieldName.toString());
  mValueList.append(value);
}

void InsertQuery::addValue(const Schema::Field & field, const QVariant & value)
{
  mFieldNameList << escapeFieldName(field.name());
  mValueList.append(value);
}

void InsertQuery::addValue(const Schema::AutoIncrementPrimaryKey& field, const QVariant& value)
{
  mFieldNameList << escapeFieldName(field.fieldName());
  mValueList.append(value);
}

bool InsertQuery::exec()
{
  const QString sql = getPrepareStatement();

  if(!mQuery.prepare(sql)){
    QString msg = tr("Preparing query for insertion into '%1' failed.\nSQL: %2")
                  .arg(mTableName, mQuery.executedQuery());
    const Mdt::ErrorCode code = Mdt::Sql::Error::errorCodeFromQSqlError(mQuery.lastError(), mQuery.driver());
    auto error = mdtErrorNewTQ(code, msg, Mdt::Error::Critical, this);
    error.stackError(mdtErrorFromQSqlQueryQ(mQuery, this));
    setLastError(error);
    return false;
  }
  for(const auto & value : mValueList){
    mQuery.addBindValue(value);
  }
  if(!mQuery.exec()){
    QString msg = tr("Executing query for insertion into '%1' failed.\nSQL: %2")
                  .arg(mTableName, mQuery.executedQuery());
    const Mdt::ErrorCode code = Mdt::Sql::Error::errorCodeFromQSqlError(mQuery.lastError(), mQuery.driver());
    auto error = mdtErrorNewTQ(code, msg, Mdt::Error::Critical, this);
    error.stackError(mdtErrorFromQSqlQueryQ(mQuery, this));
    setLastError(error);
    return false;
  }

  return true;
}

void InsertQuery::clear()
{
  mTableName.clear();
  mFieldNameList.clear();
  mValueList.clear();
}

QString InsertQuery::getPrepareStatement() const
{
  Q_ASSERT(mFieldNameList.size() == mValueList.size());

  QString sql;
  const int n = mValueList.size();
  QStringList placeHolders;

  placeHolders.reserve(n);
  for(int i = 0; i < n; ++i){
    placeHolders.append(QChar('?'));
  }
  // Build SQL
  sql = QStringLiteral("INSERT INTO ") % escapeTableName(mTableName) \
      % QStringLiteral(" (") % mFieldNameList.join(QChar(',')) \
      % QStringLiteral(") VALUES (") \
      % placeHolders.join(QChar(',')) \
      % QStringLiteral(")");
  
  return sql;
}

QString InsertQuery::escapeFieldName(const QString& fieldName) const
{
  Q_ASSERT(constDatabase().driver() != nullptr);
  return constDatabase().driver()->escapeIdentifier(fieldName, QSqlDriver::FieldName);
}

QString InsertQuery::escapeTableName(const QString& tableName) const
{
  Q_ASSERT(constDatabase().driver() != nullptr);
  return constDatabase().driver()->escapeIdentifier(tableName, QSqlDriver::TableName);
}

}} // namespace Mdt{ namespace Sql{
