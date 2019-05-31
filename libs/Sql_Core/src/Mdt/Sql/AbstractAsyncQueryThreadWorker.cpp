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
#include "SelectQuery.h"
#include "UpdateQuery.h"
#include "DeleteQuery.h"
#include "RecordUtils.h"
#include <QSqlRecord>

// #include <QDebug>

namespace Mdt{ namespace Sql{

AbstractAsyncQueryThreadWorker::AbstractAsyncQueryThreadWorker()
 : QObject()
{
}

AbstractAsyncQueryThreadWorker::~AbstractAsyncQueryThreadWorker()
{
  mSelectQuery.reset();
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

void AbstractAsyncQueryThreadWorker::processSelectStatement(const Mdt::QueryExpression::SelectStatement & statement, int instanceId, bool fetchRecords)
{
  initSelectQueryIfNot();

  if(!mSelectQuery->execStatement(statement)){
    emit queryErrorOccured(mSelectQuery->lastError(), instanceId);
    return;
  }
  if(!fetchRecords){
    emit queryOperationDone(AsyncQueryOperationType::IntermediateOperation, instanceId);
    return;
  }
  while(mSelectQuery->next()){
    emit newRecordAvailable(  variantRecordFromSqlRecord(mSelectQuery->record()), instanceId );
  }
  mSelectQuery.reset();
  emit queryOperationDone(AsyncQueryOperationType::FinalOperation, instanceId);
}

void AbstractAsyncQueryThreadWorker::processGetSingleRecordSelectStatement(const Mdt::QueryExpression::SelectStatement & statement, int instanceId)
{
  initSelectQueryIfNot();

  if(!mSelectQuery->execStatement(statement)){
    emit queryErrorOccured(mSelectQuery->lastError(), instanceId);
    return;
  }
  const auto sqlRecord = mSelectQuery->fetchSingleRecord();
  if(sqlRecord.isEmpty()){
    const auto error = mSelectQuery->lastError();
    mSelectQuery.reset();
    emit queryErrorOccured(error, instanceId);
    return;
  }
  emit newRecordAvailable( variantRecordFromSqlRecord(sqlRecord), instanceId );
  mSelectQuery.reset();
  emit queryOperationDone(AsyncQueryOperationType::FinalOperation, instanceId);
}

void AbstractAsyncQueryThreadWorker::processSelectQueryFetchNextRecords(int maxRecords,  int instanceId)
{
  if(!mSelectQuery){
    emit queryOperationDone(AsyncQueryOperationType::FinalOperation, instanceId);
    return;
  }

  bool isFinished = true;

  for(int rec = 0; rec < maxRecords; ++rec){
    isFinished = !mSelectQuery->next();
    if(isFinished){
      mSelectQuery.reset();
      emit queryOperationDone(AsyncQueryOperationType::FinalOperation, instanceId);
      return;
    }
    emit newRecordAvailable(  variantRecordFromSqlRecord(mSelectQuery->record()), instanceId );
  }
  emit queryOperationDone(AsyncQueryOperationType::IntermediateOperation, instanceId);
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
}

void AbstractAsyncQueryThreadWorker::initSelectQueryIfNot()
{
  if(!mSelectQuery){
    mSelectQuery = std::make_unique<SelectQuery>( connection() );
  }
}

}} // namespace Mdt{ namespace Sql{
