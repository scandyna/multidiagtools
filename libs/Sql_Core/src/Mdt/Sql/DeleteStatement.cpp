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
#include "DeleteStatement.h"
#include "StatementAlgorithm.h"
#include "QueryExpressionSqlTransform.h"
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

void DeleteStatement::setConditions(const PrimaryKeyRecord & primaryKeyRecord)
{
  Q_ASSERT(!primaryKeyRecord.isNull());

  mConditionsFilter = filterExpressionFromPrimaryKeyRecord(primaryKeyRecord);
}

void DeleteStatement::clear()
{
  mTableName.clear();
  mConditionsFilter.clear();
}

QString DeleteStatement::toSql(const QSqlDatabase & db) const
{
  Q_ASSERT(db.isValid());

  QString sql = QLatin1String("DELETE FROM ") % escapeTableName(mTableName, db);
  if(!mConditionsFilter.isNull()){
    sql += QLatin1String("\nWHERE ") % filterExpressionToSql(mConditionsFilter, db);
  }

  return sql;
}

}} // namespace Mdt{ namespace Sql{
