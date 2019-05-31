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
  // qRegisterMetaType<T>() can be called multiple time
  qRegisterMetaType<InsertStatement>();
  qRegisterMetaType<UpdateStatement>();
  qRegisterMetaType<DeleteStatement>();
  qRegisterMetaType<Mdt::QueryExpression::SelectStatement>();
  qRegisterMetaType<Mdt::Container::VariantRecord>();
  qRegisterMetaType<AsyncQueryOperationType>();
}

AsyncQueryConnectionImpl::~AsyncQueryConnectionImpl()
{
  close();
}

bool AsyncQueryConnectionImpl::waitOpen()
{
  using namespace std::chrono_literals;

  if( !Mdt::Async::wait(mWaitOpenPredicate, 5000ms) ){
    const QString msg = tr("Wait connection open timed out");
    mLastOpenError = mdtErrorNewTQ(Mdt::ErrorCode::TimeoutError, msg, Mdt::Error::Critical, this);
    return false;
  }

  if(mWaitOpenPredicate.hasError()){
    return false;
  }

  return true;
}

void AsyncQueryConnectionImpl::submitInsertStatement(const InsertStatement & statement, int instanceId)
{
  emit insertStatementSubmitted(statement, instanceId);
}

void AsyncQueryConnectionImpl::submitSelectStatement(const Mdt::QueryExpression::SelectStatement & statement, int instanceId, bool fetchRecords)
{
  emit selectStatementSubmitted(statement, instanceId, fetchRecords);
}

void AsyncQueryConnectionImpl::submitGetSingleRecordSelectStatement(const Mdt::QueryExpression::SelectStatement & statement, int instanceId)
{
  emit getSingleRecordSelectStatementSubmitted(statement, instanceId);
}

void AsyncQueryConnectionImpl::submitSelectQueryFetchNextRecords(int maxRecords, int instanceId)
{
  emit selectQueryFetchNextRecordsSubmitted(maxRecords, instanceId);
}

void AsyncQueryConnectionImpl::submitUpdateStatement(const UpdateStatement & statement, int instanceId)
{
  emit updateStatementSubmitted(statement, instanceId);
}

void AsyncQueryConnectionImpl::submitDeleteStatement(const DeleteStatement & statement, int instanceId)
{
  emit deleteStatementSubmitted(statement, instanceId);
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
  Q_ASSERT(!error.isNull());

  mLastOpenError = error;
  mWaitOpenPredicate.setErrorOccured();
}

}} // namespace Mdt{ namespace Sql{
