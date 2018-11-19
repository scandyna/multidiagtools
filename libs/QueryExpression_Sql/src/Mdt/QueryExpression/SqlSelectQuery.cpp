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
#include "SqlSelectQuery.h"
#include "SqlTransform.h"
#include "Mdt/Sql/Error.h"
#include <QSqlRecord>

// #include <QDebug>

namespace Mdt{ namespace QueryExpression{

// SqlSelectQuery::SqlSelectQuery(const QSqlDatabase& db)
//  : AbstractSelectQuery(db)
// {
//   Q_ASSERT(db.isValid());
// }

// SqlSelectQuery::SqlSelectQuery(QObject* parent, const QSqlDatabase& db)
//  : AbstractSelectQuery(parent, db)
// {
//   Q_ASSERT(db.isValid());
// }

SqlSelectQuery::SqlSelectQuery(QObject* parent)
 : AbstractSelectQuery(parent)
{
}

void SqlSelectQuery::setDatabase(const QSqlDatabase& db)
{
  Q_ASSERT(db.isValid());

  mQuery = QSqlQuery(db);
  mDb = db;
}


bool SqlSelectQuery::exec(const SelectStatement & statement, int maxRows)
{
  Q_ASSERT(maxRows >= 0);

  const auto sql = selectStatementToSql(statement, maxRows, mDb);

  if(!mQuery.exec(sql)){
    const auto msg = tr("Failed to execute query, SQL: %1 .")
                     .arg(sql);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError( mdtErrorFromQSqlQueryQ(mQuery, this) );
    setLastError(error);
    return false;
  }

  return true;
}

int SqlSelectQuery::fieldCount() const
{
  return mQuery.record().count();
}

QVariant SqlSelectQuery::value(int fieldIndex) const
{
  return mQuery.value(fieldIndex);
}

int SqlSelectQuery::fieldIndexImpl(const EntityAndField & field) const
{
  const auto sqlFieldName = field.fieldAliasOrName();

  return mQuery.record().indexOf(sqlFieldName);
}

bool SqlSelectQuery::fetchNext()
{
  return mQuery.next();
}

}} // namespace Mdt{ namespace QueryExpression{
