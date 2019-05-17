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
#include "AsyncInsertQuery.h"

namespace Mdt{ namespace Sql{

AsyncInsertQuery::AsyncInsertQuery(const std::shared_ptr<AsyncQueryConnection> & connection, QObject *parent)
 : BaseClass(connection, parent)
{
  auto *cnnImpl = connectionImpl();
  Q_ASSERT(cnnImpl != nullptr);

  connect(cnnImpl, &AsyncQueryConnectionImpl::newIdInserted, this, &AsyncInsertQuery::setNewInsertedId);
}

void AsyncInsertQuery::submitStatement(const InsertStatement & statement)
{
  mIsSynchronous = false;
  connectionImpl()->submitInsertStatement(statement, instanceId());
}

bool AsyncInsertQuery::execStatement(const InsertStatement & statement)
{
  /*
   * Make sure that setNewInsertedId()
   * will store the comming id
   */
  mIsSynchronous = true;
  connectionImpl()->submitInsertStatement(statement, instanceId());

  const auto result = waitOperationFinished();
  if(!result){
    mIsSynchronous = false;
    return false;
  }

  return true;
}

void AsyncInsertQuery::setNewInsertedId(const QVariant & id, int iid)
{
  if(iid != instanceId()){
    return;
  }
  if(isSynchronous()){
    mLastInertId = id;
  }
  emit newIdInserted(id);
}

}} // namespace Mdt{ namespace Sql{
