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
#include "InsertQuery.h"
#include "Schema/AutoIncrementPrimaryKey.h"
#include "Schema/Field.h"
#include "Schema/Table.h"
#include "Mdt/Sql/Error.h"
#include <QChar>

namespace Mdt{ namespace Sql{

InsertQuery::InsertQuery(const Connection & connection, QObject *parent)
 : AbstractQuery(connection, parent),
   mQuery( connection.database() )
{
}

void InsertQuery::setTableName(const QString & name)
{
  mStatement.setTableName(name);
}

void InsertQuery::setTable(const Schema::Table & table)
{
  setTableName(table.tableName());
}

void InsertQuery::addValue(const FieldName & fieldName, const QVariant & value)
{
  Q_ASSERT(!fieldName.isNull());
  Q_ASSERT(!mStatement.containsFieldName(fieldName.toString()));

  mStatement.addValue(fieldName, value);
}

void InsertQuery::addValue(const Schema::Field & field, const QVariant & value)
{
  Q_ASSERT(!field.isNull());
  Q_ASSERT(!mStatement.containsFieldName(field.name()));

  addValue( FieldName(field.name()), value );
}

void InsertQuery::addValue(const Schema::AutoIncrementPrimaryKey& field, const QVariant& value)
{
  Q_ASSERT(!field.isNull());
  Q_ASSERT(!mStatement.containsFieldName(field.fieldName()));

  addValue( FieldName(field.fieldName()), value );
}

bool InsertQuery::execStatement(const InsertStatement & statement)
{
  mStatement = statement;

  return exec();
}

bool InsertQuery::exec()
{
  const QString sql = mStatement.toPrepareStatementSql(database());

  if(!mQuery.prepare(sql)){
    QString msg = tr("Preparing query for insertion into '%1' failed.\nSQL: %2")
                  .arg(mStatement.tableName(), mQuery.executedQuery());
    const Mdt::ErrorCode code = Mdt::Sql::Error::errorCodeFromQSqlError(mQuery.lastError(), mQuery.driver());
    auto error = mdtErrorNewTQ(code, msg, Mdt::Error::Critical, this);
    error.stackError(mdtErrorFromQSqlQueryQ(mQuery, this));
    setLastError(error);
    return false;
  }
  const auto valueList = mStatement.toValueList();
  for(const auto & value : valueList){
    mQuery.addBindValue(value);
  }
  if(!mQuery.exec()){
    QString msg = tr("Executing query for insertion into '%1' failed.\nSQL: %2")
                  .arg(mStatement.tableName(), mQuery.executedQuery());
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
  mStatement.clear();
  mQuery.clear();
}

}} // namespace Mdt{ namespace Sql{
