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
#include <QMetaMethod>

namespace Mdt{ namespace Sql{

AsyncSelectQuery::AsyncSelectQuery(const std::shared_ptr<AsyncQueryConnection> & connection, QObject *parent)
 : BaseClass(connection, parent)
{
  auto *cnnImpl = connectionImpl();
  Q_ASSERT(cnnImpl != nullptr);

  connect(cnnImpl, &AsyncQueryConnectionImpl::newRecordAvailable, this, &AsyncSelectQuery::onNewRecordAvailable);
  connect(cnnImpl, &AsyncQueryConnectionImpl::selectQueryFetchNextDone, this, &AsyncSelectQuery::onFetchNexDone);
}

Mdt::ExpectedResult AsyncSelectQuery::execStatement(const Mdt::QueryExpression::SelectStatement & statement)
{
  submitStatement(statement, false);

  const auto result = waitOperationFinished();
  if(result){
    mIsSynchronous = true;
  }

  return result;
}

bool AsyncSelectQuery::next()
{
  Q_ASSERT(isSynchronous());

  connectionImpl()->submitSelectQueryFetchNext(instanceId());
  if(!waitOperationFinished()){
    return false;
  }

  return mFetchNextResult;
}

void AsyncSelectQuery::submitStatement(const Mdt::QueryExpression::SelectStatement & statement)
{
  mIsSynchronous = false;
  submitStatement(statement, true);
}

void AsyncSelectQuery::onNewRecordAvailable(const Mdt::Container::VariantRecord & record, int iid)
{
  if(iid != instanceId()){
    return;
  }

  if(isSynchronous()){
    mCurrentRecord = record;
  }

  emit newRecordAvailable(record);
}

void AsyncSelectQuery::onFetchNexDone(bool result, int iid)
{
  if(iid == instanceId()){
    mFetchNextResult = result;
  }
}

void AsyncSelectQuery::submitStatement(const Mdt::QueryExpression::SelectStatement & statement, bool fetchRecords)
{
  connectionImpl()->submitSelectStatement(statement, instanceId(), fetchRecords);
}

}} // namespace Mdt{ namespace Sql{
