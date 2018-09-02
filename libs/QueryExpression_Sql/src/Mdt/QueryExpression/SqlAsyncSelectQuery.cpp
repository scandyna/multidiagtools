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
#include "SqlAsyncSelectQuery.h"
#include "SqlTransform.h"
#include "Mdt/QueryExpression/QuerySelectStatement.h"
#include <QVariant>

using Mdt::Sql::AsyncQuery;

namespace Mdt{ namespace QueryExpression{

SqlAsyncSelectQuery::SqlAsyncSelectQuery(std::unique_ptr<AsyncQuery> && query, QObject* parent)
 : BaseClass(parent),
   mQuery( std::move(query) )
{
  connect(mQuery.get(), &AsyncQuery::newRecordAvailable, this, &SqlAsyncSelectQuery::newRecordAvailable);
  connect(mQuery.get(), &AsyncQuery::errorOccured, this, &SqlAsyncSelectQuery::errorOccured);
}

void SqlAsyncSelectQuery::submitStatement(const SelectStatement& statement, int maxRows)
{
  Q_ASSERT(maxRows >= 0);

  mQuery->submitQuery( QVariant::fromValue( QuerySelectStatement(statement, maxRows) ) );
}

}} // namespace Mdt{ namespace QueryExpression{
