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

#include <QDebug>

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
  Connection::close(connection());
  Connection::remove(connection());
}

void AbstractAsyncQueryThreadWorker::processInsertStatement(const Mdt::Sql::InsertStatement & statement, int instanceId)
{
  qDebug() << "THD: process insert statement for iid: " << instanceId;

  InsertQuery query(connection().database());

  if(!query.execStatement(statement)){
    qDebug() << "THD: query failed, iid: " << instanceId;
    query.lastError().commit();
    emit queryErrorOccured(query.lastError(), instanceId);
    return;
  }
  const auto id = query.lastInsertId();
  if(!id.isNull()){
    qDebug() << "THD: new id: " << id << " for iid: " << instanceId;
    emit newIdInserted(id, instanceId);
  }
  qDebug() << "THD: process insert statement done, iid: " << instanceId;
  emit queryDone(instanceId);
}

void AbstractAsyncQueryThreadWorker::setConnection(const Connection & connection)
{
  mConnectionName = connection.name();
}

}} // namespace Mdt{ namespace Sql{
