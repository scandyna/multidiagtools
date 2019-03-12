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
#include "SelectQuery.h"
#include "QueryExpressionSqlTransform.h"
#include "Error.h"
#include <QSqlError>
#include <QSqlRecord>

namespace Mdt{ namespace Sql{

SelectQuery::SelectQuery(const QSqlDatabase& db)
 : AbstractQuery(db),
   mQuery(db)
{
  Q_ASSERT(db.isValid());
}

SelectQuery::SelectQuery(QObject* parent, const QSqlDatabase& db)
 : AbstractQuery(parent, db),
   mQuery(db)
{
  Q_ASSERT(db.isValid());
}

bool SelectQuery::execStatement(const Mdt::QueryExpression::SelectStatement & statement, int maxRows)
{
  mStatement = statement;

  const QString sql = selectStatementToSql(mStatement, maxRows, constDatabase());
  if(!mQuery.exec(sql)){
    QString msg = tr("Executing query to select from '%1' failed.").arg(mStatement.entity().name());
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError(mdtErrorFromQSqlQueryQ(mQuery, this));
    setLastError(error);
    return false;
  }

  return true;
}

int SelectQuery::fieldCount() const
{
  return mQuery.record().count();
}

QSqlRecord SelectQuery::fetchSingleRecord()
{
  QSqlRecord record;

  if(!mQuery.first()){
    QString msg = tr("Fetching a single record from '%1' failed because no record is available.").arg(mStatement.entity().name());
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    if(mQuery.lastError().isValid()){
      error.stackError(mdtErrorFromQSqlQueryQ(mQuery, this));
      setLastError(error);
    }
    return QSqlRecord();
  }
  record = mQuery.record();
  if(mQuery.next()){
    QString msg = tr("Fetching a single record from '%1' failed because more than 1 record is available.").arg(mStatement.entity().name());
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    return QSqlRecord();
  }

  return record;
}

}} // namespace Mdt{ namespace Sql{
