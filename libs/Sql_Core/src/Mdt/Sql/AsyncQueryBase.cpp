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
#include "AsyncQueryBase.h"
#include "Mdt/Async/Wait.h"
#include "Mdt/ErrorCode.h"
#include <QString>

namespace Mdt{ namespace Sql{

AsyncQueryBase::AsyncQueryBase(const std::shared_ptr<AsyncQueryConnection> & connection, QObject *parent)
 : QObject(parent),
   mConnection(connection)
{
  Q_ASSERT(mConnection.get() != nullptr);

  auto *cnnImpl = connectionImpl();
  Q_ASSERT(cnnImpl != nullptr);

  mInstanceId = cnnImpl->nextInstanceId();
  connect(cnnImpl, &AsyncQueryConnectionImpl::queryDone, this, &AsyncQueryBase::setDone);
  connect(cnnImpl, &AsyncQueryConnectionImpl::queryErrorOccured, this, &AsyncQueryBase::setError);
}

void AsyncQueryBase::setWaitTimeout(std::chrono::milliseconds timeout)
{
  Q_ASSERT(timeout >= std::chrono::milliseconds(0));

  mWaitTimeout = timeout;
}

Mdt::ExpectedResult AsyncQueryBase::waitFinished()
{
  if( !Mdt::Async::wait(mWaitPredicate, mWaitTimeout) ){
    const QString msg = tr("Query timed out");
    auto error = mdtErrorNewTQ(Mdt::ErrorCode::TimeoutError, msg, Mdt::Error::Critical, this);
    /// \todo Should cancel the query..
    return error;
  }
  if(mWaitPredicate.hasError()){
    Q_ASSERT(!mLastError.isNull());
    return mLastError;
  }

  return Mdt::ExpectedResultOk();
}

void AsyncQueryBase::setDone(int instanceId)
{
  if(instanceId == mInstanceId){
    mWaitPredicate.setDone();
    emit done();
  }
}

void AsyncQueryBase::setError(const Mdt::Error & error, int instanceId)
{
  Q_ASSERT(!error.isNull());

  if(instanceId == mInstanceId){
    mLastError = error;
    mWaitPredicate.setErrorOccured();
    emit errorOccurred(error);
  }
}

}} // namespace Mdt{ namespace Sql{
