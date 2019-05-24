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
#include "UpdateQuery.h"
#include "Error.h"
#include <QSqlQuery>

namespace Mdt{ namespace Sql{

UpdateQuery::UpdateQuery(const Connection & connection, QObject *parent)
 : AbstractQuery(connection, parent)
{
}

void UpdateQuery::setTableName(const QString& name)
{
  Q_ASSERT(!name.trimmed().isEmpty());

  mStatement.setTableName(name);
}

void UpdateQuery::addValue(const FieldName& fieldName, const QVariant& value)
{
  Q_ASSERT(!fieldName.isNull());

  mStatement.addValue(fieldName, value);
}

void UpdateQuery::setConditions(const PrimaryKeyRecord& primaryKeyRecord)
{
  Q_ASSERT(!primaryKeyRecord.isNull());

  mStatement.setConditions(primaryKeyRecord);
}

bool UpdateQuery::execStatement(const UpdateStatement & statement)
{
  mStatement = statement;

  return exec();
}

bool UpdateQuery::exec()
{
  QSqlQuery query(database());

  if(!query.exec( mStatement.toSql(database()) )){
    QString msg = tr("Executing query to update '%1' failed.").arg(mStatement.tableName());
    const Mdt::ErrorCode code = Mdt::Sql::Error::errorCodeFromQSqlError(query.lastError(), query.driver());
    auto error = mdtErrorNewTQ(code, msg, Mdt::Error::Critical, this);
    error.stackError(mdtErrorFromQSqlQueryQ(query, this));
    setLastError(error);
    return false;
  }

  return true;
}

}} // namespace Mdt{ namespace Sql{
