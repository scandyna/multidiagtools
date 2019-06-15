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
#include "AsyncSelectQueryThreadWorker.h"
#include "RecordUtils.h"
#include <QSqlRecord>
#include <utility>

namespace Mdt{ namespace Sql{

AsyncSelectQueryThreadWorker::AsyncSelectQueryThreadWorker(const Connection & connection, QObject *parent)
 : QObject(parent),
   mConnection(connection)
{
}

void AsyncSelectQueryThreadWorker::processStatement(const Mdt::QueryExpression::SelectStatement & statement, 
                                                          AsyncSelectQueryRecordFetching recordFetching, int instanceId)
{
  initQueryIfNot(instanceId);

  auto & query = getQuery(instanceId);

  if( !query.execStatement(statement) ){
    emit errorOccured(query.lastError(), instanceId);
    return;
  }

  switch(recordFetching){
    case AsyncSelectQueryRecordFetching::FetchNoRecord:
      notifyOperationPartiallyDone(instanceId);
      break;
    case AsyncSelectQueryRecordFetching::FetchSingleRecord:
      fetchSingleRecord(query, instanceId);
      setQueryDone(instanceId);
      break;
    case AsyncSelectQueryRecordFetching::FetchAllRecords:
      fetchAllRecords(query, instanceId);
      setQueryDone(instanceId);
      break;
  }
}

void AsyncSelectQueryThreadWorker::processFetchSingleRecord(int instanceId)
{
  Q_ASSERT(contiansQuery(instanceId));

  fetchSingleRecord( getQuery(instanceId), instanceId );
}

void AsyncSelectQueryThreadWorker::processFetchNextRecords(int maxRecords,  int instanceId)
{
  if(!contiansQuery(instanceId)){
    emit operationDone(AsyncQueryOperationType::FinalOperation, instanceId);
    return;
  }

  auto & query = getQuery(instanceId);
  bool isFinished = true;

  for(int rec = 0; rec < maxRecords; ++rec){
    isFinished = !query.next();
    if(isFinished){
      setQueryDone(instanceId);
      return;
    }
    emit newRecordAvailable(  variantRecordFromSqlRecord(query.record()), instanceId );
  }
  emit operationDone(AsyncQueryOperationType::IntermediateOperation, instanceId);
}

void AsyncSelectQueryThreadWorker::fetchSingleRecord(SelectQuery & query, int instanceId)
{
  const auto sqlRecord = query.fetchSingleRecord();
  if(sqlRecord.isEmpty()){
    const auto error = query.lastError();
    removeQuery(instanceId);
    emit errorOccured(error, instanceId);
    return;
  }
  emit newRecordAvailable( variantRecordFromSqlRecord(sqlRecord), instanceId );
  setQueryDone(instanceId);
}

void AsyncSelectQueryThreadWorker::fetchAllRecords(SelectQuery & query, int instanceId)
{
  while(query.next()){
    emit newRecordAvailable(  variantRecordFromSqlRecord(query.record()), instanceId );
  }
}

void AsyncSelectQueryThreadWorker::notifyOperationPartiallyDone(int instanceId)
{
  emit operationDone(AsyncQueryOperationType::IntermediateOperation, instanceId);
}

void AsyncSelectQueryThreadWorker::setQueryDone(int instanceId)
{
  removeQuery(instanceId);
  emit operationDone(AsyncQueryOperationType::FinalOperation, instanceId);
}

bool AsyncSelectQueryThreadWorker::contiansQuery(int instanceId) const
{
  return mQueryMap.find(instanceId) != mQueryMap.cend();
}

SelectQuery & AsyncSelectQueryThreadWorker::getQuery(int instanceId)
{
  Q_ASSERT(contiansQuery(instanceId));

  return mQueryMap.find(instanceId)->second;
}

void AsyncSelectQueryThreadWorker::initQueryIfNot(int instanceId)
{
  if(contiansQuery(instanceId)){
    return;
  }
  mQueryMap.emplace( instanceId, mConnection );
}

void AsyncSelectQueryThreadWorker::removeQuery(int instanceId)
{
  mQueryMap.erase(instanceId);
}

}} // namespace Mdt{ namespace Sql{
