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
#include "SqlAsyncSelectQueryFactory.h"
#include "SqlAsyncSelectQuery.h"
#include "SqlAsyncSelectQueryThreadWorker.h"

namespace Mdt{ namespace QueryExpression{

bool SqlAsyncSelectQueryFactory::setup(const Sql::ConnectionParameters & parameters)
{
  return mConnection.setup(parameters, new SqlAsyncSelectQueryThreadWorker);
}

std::unique_ptr<AbstractAsyncSelectQuery> SqlAsyncSelectQueryFactory::createSelectQuery()
{
  auto query = std::make_unique<SqlAsyncSelectQuery>( std::move(mConnection.createQuery()) );

  return std::move(query);
}

}} // namespace Mdt{ namespace QueryExpression{
