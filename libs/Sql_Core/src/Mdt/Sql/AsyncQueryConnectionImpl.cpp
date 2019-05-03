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
#include "AsyncQueryConnectionImpl.h"
#include "Mdt/Async/WaitDonePredicateWithError.h"
#include "Mdt/Async/Wait.h"
#include "Mdt/ErrorCode.h"
#include <QMetaType>
#include <QString>

namespace Mdt{ namespace Sql{

AsyncQueryConnectionImpl::AsyncQueryConnectionImpl(QObject* parent)
 : QObject(parent)
{
//   using Mdt::Async::WaitDonePredicate;

  // qRegisterMetaType<T>() can be called multiple time
  qRegisterMetaType<InsertStatement>();

//   connect(&mThread, &QThread::finished, &mWaitClosedPredicate, &WaitDonePredicate::setDone);
}

AsyncQueryConnectionImpl::~AsyncQueryConnectionImpl()
{
  close();
}

Mdt::ExpectedResult AsyncQueryConnectionImpl::waitOpen()
{
  using namespace std::chrono_literals;

  if( !Mdt::Async::wait(mWaitOpenPredicate, 5000ms) ){
    const QString msg = tr("Wait connection open timed out");
    mLastOpenError = mdtErrorNewTQ(Mdt::ErrorCode::TimeoutError, msg, Mdt::Error::Critical, this);
    return mLastOpenError;
  }

  if(mWaitOpenPredicate.hasError()){
    return mLastOpenError;
  }

  return Mdt::ExpectedResultOk();
}

void AsyncQueryConnectionImpl::submitInsertStatement(const InsertStatement & statement, int instanceId)
{
  emit insertStatementSubmitted(statement, instanceId);
}

void AsyncQueryConnectionImpl::submitClose()
{
  mThread.quit();
}

void AsyncQueryConnectionImpl::close()
{
  submitClose();
  mThread.wait();
}

void AsyncQueryConnectionImpl::setOpenDone()
{
  mWaitOpenPredicate.setDone();
}

void AsyncQueryConnectionImpl::setOpenError(const Mdt::Error & error)
{
  mLastOpenError = error;
  mWaitOpenPredicate.setErrorOccured();
}

}} // namespace Mdt{ namespace Sql{
