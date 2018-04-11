/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "JoinClauseSqlTransform.h"
#include "JoinClause.h"
#include "JoinClauseItemList.h"
#include "JoinClauseItemListSqlTransform.h"
#include <QSqlDatabase>
#include <QStringBuilder>
#include <QSqlDriver>

namespace Mdt{ namespace Sql{

QString JoinClauseSqlTransform::getSql(const JoinClause & joinClause, const QSqlDatabase & db)
{
  QString sql;
  const auto tableAlias = joinClause.tableAlias();
  const auto * driver = db.driver();
  Q_ASSERT(driver != nullptr);

  sql = driver->escapeIdentifier(joinClause.tableName(), QSqlDriver::TableName);
  if(!tableAlias.isEmpty()){
    sql += QStringLiteral(" ") % driver->escapeIdentifier(tableAlias, QSqlDriver::TableName);
  }
  if(!joinClause.itemList().isEmpty()){
    sql += QStringLiteral("\n") % JoinClauseItemListSqlTransform::getSql(joinClause.itemList(), db);
  }

  return sql;
}


}} // namespace Mdt{ namespace Sql{
