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
#include "AsyncQueryConnection.h"
#include "AsyncQueryThread.h"
#include "Mdt/Container/VariantRecord.h"
#include <QMetaType>

namespace Mdt{ namespace Sql{

AsyncQueryConnection::AsyncQueryConnection(QObject *parent)
 : QObject(parent)
{
  // qRegisterMetaType<T>() can be called multiple time
  qRegisterMetaType<Mdt::Container::VariantRecord>();
}

AsyncQueryConnection::~AsyncQueryConnection()
{
  mThread.quit();
  mThread.wait();
}

bool AsyncQueryConnection::setup(const ConnectionParameters & parameters)
{
  mThread.quit();
  mThread.wait();

  auto *worker = new AsyncQueryThreadWorker;
  worker->moveToThread(&mThread);
  connect(&mThread, &QThread::finished, worker, &QObject::deleteLater);
  if(!worker->setup(parameters)){
    mSetupError = worker->setupError();
    return false;
  }
  connect(this, &AsyncQueryConnection::queryRequested, worker, &AsyncQueryThreadWorker::processQuery);
  connect(worker, &AsyncQueryThreadWorker::newRecordAvailable, this, &AsyncQueryConnection::newRecordAvailable);
  connect(worker, &AsyncQueryThreadWorker::errorOccured, this, &AsyncQueryConnection::errorOccured);
  mThread.start();

  return true;
}

std::unique_ptr<AsyncQuery> AsyncQueryConnection::createQuery()
{
  ++mQueryInstanceId;

  auto query = std::make_unique<AsyncQuery>(mQueryInstanceId);

  connect(query.get(), &AsyncQuery::queryRequested, this, &AsyncQueryConnection::queryRequested);
  connect(this, &AsyncQueryConnection::newRecordAvailable, query.get(), &AsyncQuery::setNewRecordAvailable);
  connect(this, &AsyncQueryConnection::errorOccured, query.get(), &AsyncQuery::setErrorOccured);

  return query;
}

}} // namespace Mdt{ namespace Sql{
