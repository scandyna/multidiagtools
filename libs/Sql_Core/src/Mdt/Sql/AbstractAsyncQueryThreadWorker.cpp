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
#include "AbstractAsyncQueryThreadWorker.h"
#include "Error.h"
#include "InsertQuery.h"
#include "AsyncSelectQueryThreadWorker.h"
#include "UpdateQuery.h"
#include "DeleteQuery.h"

namespace Mdt{ namespace Sql{

AbstractAsyncQueryThreadWorker::AbstractAsyncQueryThreadWorker()
 : QObject()
{
}

AbstractAsyncQueryThreadWorker::~AbstractAsyncQueryThreadWorker()
{
  close();
}

/*
 * close() can be called from destructor
 * The sequence is:
 *  - QThread::quit()
 *  - QThread::finished()
 *  - AbstractAsyncQueryThreadWorker::deleteLater()
 *  - AbstractAsyncQueryThreadWorker::~AbstractAsyncQueryThreadWorker()
 *
 * QThread::finished() is emitted just right before the thread finishes
 * deleteLater() is called just after the event loop exited
 *
 * Remember that no event loop is running anymore at this state
 */
void AbstractAsyncQueryThreadWorker::close()
{
  if(mConnectionName.isEmpty()){
    return;
  }
  mSelectQueryThreadWorker.reset();
  Connection::close(connection());
  Connection::remove(connection());
}

void AbstractAsyncQueryThreadWorker::processInsertStatement(const Mdt::Sql::InsertStatement & statement, int instanceId)
{
  InsertQuery query(connection());

  if(!query.execStatement(statement)){
    emit queryErrorOccured(query.lastError(), instanceId);
    return;
  }
  const auto id = query.lastInsertId();
  if(!id.isNull()){
    emit newIdInserted(id, instanceId);
  }
  emit queryOperationDone(AsyncQueryOperationType::FinalOperation, instanceId);
}

void AbstractAsyncQueryThreadWorker::processSelectStatement(const Mdt::QueryExpression::SelectStatement & statement,
                                                            Mdt::Sql::AsyncSelectQueryRecordFetching recordFetching, int instanceId)
{
  Q_ASSERT(mSelectQueryThreadWorker.get() != nullptr);

  mSelectQueryThreadWorker->processStatement(statement, recordFetching, instanceId);
}

void AbstractAsyncQueryThreadWorker::processSelectQueryFetchSingleRecord(int instanceId)
{
  Q_ASSERT(mSelectQueryThreadWorker.get() != nullptr);

  mSelectQueryThreadWorker->processFetchSingleRecord(instanceId);
}

void AbstractAsyncQueryThreadWorker::processSelectQueryFetchNextRecords(int maxRecords,  int instanceId)
{
  Q_ASSERT(mSelectQueryThreadWorker.get() != nullptr);

  mSelectQueryThreadWorker->processFetchNextRecords(maxRecords, instanceId);
}

void AbstractAsyncQueryThreadWorker::processUpdateStatement(const Mdt::Sql::UpdateStatement & statement, int instanceId)
{
  UpdateQuery query(connection());

  if(!query.execStatement(statement)){
    emit queryErrorOccured(query.lastError(), instanceId);
    return;
  }
  emit queryOperationDone(AsyncQueryOperationType::FinalOperation, instanceId);
}

void AbstractAsyncQueryThreadWorker::processDeleteStatement(const Mdt::Sql::DeleteStatement & statement, int instanceId)
{
  DeleteQuery query(connection());

  if(!query.execStatement(statement)){
    emit queryErrorOccured(query.lastError(), instanceId);
    return;
  }
  emit queryOperationDone(AsyncQueryOperationType::FinalOperation, instanceId);
}

void AbstractAsyncQueryThreadWorker::setConnection(const Connection & connection)
{
  mConnectionName = connection.name();
  initSelectQueryThreadWorker();
}

void AbstractAsyncQueryThreadWorker::initSelectQueryThreadWorker()
{
  Q_ASSERT(connection().isValid());

  mSelectQueryThreadWorker = std::make_unique<AsyncSelectQueryThreadWorker>(connection());
  connect(mSelectQueryThreadWorker.get(), &AsyncSelectQueryThreadWorker::newRecordAvailable,
          this, &AbstractAsyncQueryThreadWorker::newRecordAvailable);
  connect(mSelectQueryThreadWorker.get(), &AsyncSelectQueryThreadWorker::operationDone, this,
          &AbstractAsyncQueryThreadWorker::queryOperationDone);
  connect(mSelectQueryThreadWorker.get(), &AsyncSelectQueryThreadWorker::errorOccured,
          this, &AbstractAsyncQueryThreadWorker::queryErrorOccured);
}

}} // namespace Mdt{ namespace Sql{
