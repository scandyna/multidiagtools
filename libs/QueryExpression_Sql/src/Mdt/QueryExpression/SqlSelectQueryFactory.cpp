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
#include "SqlSelectQueryFactory.h"
#include "SqlSelectQuery.h"

namespace Mdt{ namespace QueryExpression{

void SqlSelectQueryFactory::setDatabase(const QSqlDatabase& db)
{
  Q_ASSERT(db.isValid());

  mDatabase = db;
}

SelectQuery SqlSelectQueryFactory::createSelectQuery() const
{
  auto query = SelectQuery::make<SqlSelectQuery>();
  auto & impl = query.impl<SqlSelectQuery>();
  impl.setDatabase(mDatabase);

  return query;
}

}} // namespace Mdt{ namespace QueryExpression{
