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
#include "DeleteQuery.h"
#include "Error.h"
#include <QSqlQuery>

namespace Mdt{ namespace Sql{

DeleteQuery::DeleteQuery(const QSqlDatabase& db)
 : AbstractQuery(db)
{
  Q_ASSERT(db.isValid());
}

void DeleteQuery::setTableName(const QString& name)
{
  Q_ASSERT(!name.trimmed().isEmpty());

  mStatement.setTableName(name);
}

void DeleteQuery::setConditions(const PrimaryKeyRecord& primaryKeyRecord)
{
  Q_ASSERT(!primaryKeyRecord.isNull());

  mStatement.setConditions(primaryKeyRecord);
}

bool DeleteQuery::exec()
{
  QSqlQuery query(database());

  if(!query.prepare( mStatement.toPrepareStatementSql(constDatabase()) )){
    QString msg = tr("Preparing query to delete in '%1' failed.").arg(mStatement.tableName());
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError(mdtErrorFromQSqlQueryQ(query, this));
    setLastError(error);
    return false;
  }
  const auto conditionsValues = mStatement.toConditionsValueList();
  for(const auto & value : conditionsValues){
    query.addBindValue(value);
  }
  if(!query.exec()){
    QString msg = tr("Executing query to delete in '%1' failed.").arg(mStatement.tableName());
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError(mdtErrorFromQSqlQueryQ(query, this));
    setLastError(error);
    return false;
  }

  return true;
}

void DeleteQuery::clear()
{
  mStatement.clear();
}

}} // namespace Mdt{ namespace Sql{
