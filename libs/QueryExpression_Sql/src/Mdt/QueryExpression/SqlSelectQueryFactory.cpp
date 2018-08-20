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

namespace Mdt{ namespace QueryExpression{

void SqlSelectQueryFactory::setDatabase(const QSqlDatabase& db)
{
  Q_ASSERT(db.isValid());

  mDatabase = db;
}

std::unique_ptr<AbstractSelectQuery> SqlSelectQueryFactory::createSelectQuery() const
{
  auto query = std::make_unique<SqlSelectQuery>();

  query->setDatabase(mDatabase);

  return std::move(query);
}

CachedSelectQuery SqlSelectQueryFactory::createCachedSelectQuery() const
{
  auto query = CachedSelectQuery::make<SqlSelectQuery>();
  setupQueryImpl(query.impl<SqlSelectQuery>());

  return query;
}

void SqlSelectQueryFactory::setupQueryImpl(SqlSelectQuery& queryImpl) const
{
  queryImpl.setDatabase(mDatabase);
}

}} // namespace Mdt{ namespace QueryExpression{
