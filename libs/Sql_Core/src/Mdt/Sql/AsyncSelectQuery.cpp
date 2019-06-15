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
#include "AsyncSelectQuery.h"

namespace Mdt{ namespace Sql{

AsyncSelectQuery::AsyncSelectQuery(const std::shared_ptr<AsyncQueryConnection> & connection, QObject *parent)
 : BaseClass(connection, parent)
{
  auto *cnnImpl = connectionImpl();
  Q_ASSERT(cnnImpl != nullptr);

  connect(cnnImpl, &AsyncQueryConnectionImpl::newRecordAvailable, this, &AsyncSelectQuery::onNewRecordAvailable);
}

bool AsyncSelectQuery::execStatement(const Mdt::QueryExpression::SelectStatement & statement)
{
  /*
   * Make sure that onNewRecordAvailable()
   * will store the first comming records
   */
  mIsSynchronous = true;
  submitStatement(statement, AsyncSelectQueryRecordFetching::FetchNoRecord);

  const auto result = waitOperationFinished();
  if(result){
    mIsSynchronous = true;
  }else{
    mIsSynchronous = false;
  }

  return result;
}

bool AsyncSelectQuery::next()
{
  Q_ASSERT(isSynchronous());

  return fetchRecords(1);
}

bool AsyncSelectQuery::fetchRecords(int maxRecords)
{
  Q_ASSERT(maxRecords > 0);
  Q_ASSERT(isSynchronous());

  mCurrentRecords.clear();
  connectionImpl()->submitSelectQueryFetchNextRecords(maxRecords, instanceId());
  if(!waitOperationFinished()){
    return false;
  }

  return fetchedRecordCount() > 0;
}

Mdt::Container::VariantRecord AsyncSelectQuery::fetchSingleRecord()
{
  Q_ASSERT(isSynchronous());

  mCurrentRecords.clear();
  connectionImpl()->submitSelectQueryFetchSingleRecord(instanceId());
  if(!waitOperationFinished()){
    return Mdt::Container::VariantRecord();
  }
  Q_ASSERT(mCurrentRecords.size() == 1);

  return mCurrentRecords[0];
}

void AsyncSelectQuery::submitStatementAndFetchAll(const Mdt::QueryExpression::SelectStatement & statement)
{
  mIsSynchronous = false;
  mCurrentRecords.clear();
  submitStatement(statement, AsyncSelectQueryRecordFetching::FetchAllRecords);
}

void AsyncSelectQuery::submitGetSingleRecordStatement(const Mdt::QueryExpression::SelectStatement & statement)
{
  mIsSynchronous = false;
  mCurrentRecords.clear();
  submitStatement(statement, AsyncSelectQueryRecordFetching::FetchSingleRecord);
}

void AsyncSelectQuery::onNewRecordAvailable(const Mdt::Container::VariantRecord & record, int iid)
{
  if(iid != instanceId()){
    return;
  }

  if(isSynchronous()){
    mCurrentRecords.push_back(record);
  }

  emit newRecordAvailable(record);
}

void AsyncSelectQuery::submitStatement(const Mdt::QueryExpression::SelectStatement & statement, AsyncSelectQueryRecordFetching recordFetching)
{
  connectionImpl()->submitSelectStatement(statement, recordFetching, instanceId());
}

}} // namespace Mdt{ namespace Sql{
