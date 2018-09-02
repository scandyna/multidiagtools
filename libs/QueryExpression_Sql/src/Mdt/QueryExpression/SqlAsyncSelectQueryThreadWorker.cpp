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
#include "SqlAsyncSelectQueryThreadWorker.h"
#include "SqlTransform.h"
#include <QVariant>

namespace Mdt{ namespace QueryExpression{

SqlAsyncSelectQueryThreadWorker::SqlAsyncSelectQueryThreadWorker(QObject* parent)
 : BaseClass(parent)
{
}

void SqlAsyncSelectQueryThreadWorker::processQuery(const QVariant& query, int instanceId)
{
  if(query.canConvert<QuerySelectStatement>()){
    processSelectStatement( query.value<QuerySelectStatement>(), instanceId );
  }else{
    BaseClass::processQuery(query, instanceId);
  }
}

void SqlAsyncSelectQueryThreadWorker::processSelectStatement(const QuerySelectStatement& statement, int instanceId)
{
  const auto sql = selectStatementToSql(statement.statement(), database());

  processSqlSelectStatement(sql , instanceId);
}

}} // namespace Mdt{ namespace QueryExpression{
