/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "CopyThread.h"
#include "mdtAlgorithms.h"
#include "mdt/sql/error/Error.h"

namespace mdt{ namespace sql{ namespace copier{

CopyThread::CopyThread(QObject *parent)
 : QThread(parent),
   pvAbort(false)
{
}

void CopyThread::abort()
{
  pvAbort = true;
}

// void CopyThread::resetTableSizeList()
// {
//   pvTableSizeList.assign(tableMappingCount(), -1);
//   
//   
// }

void CopyThread::resetTableCopyProgress()
{
  pvTableProgressList.assign(tableMappingCount(), mdtProgressValue<int64_t>());
}

void CopyThread::setTableSize(int index, int64_t size)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < (int)pvTableSizeList.size());
  pvTableSizeList[index] = size;
}

void CopyThread::incrementCopyProgress(int index, int64_t n)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < (int)pvTableSizeList.size());

  
}

QSqlDatabase CopyThread::createConnection(const QSqlDatabase & dbInfo)
{
  QSqlDatabase db;
  QString cnn = mdtAlgorithms::generateString(6, QSqlDatabase::connectionNames());
  db = QSqlDatabase::addDatabase(dbInfo.driverName(), cnn);

  db.setDatabaseName(dbInfo.databaseName());
  db.setHostName(dbInfo.hostName());
  db.setPort(dbInfo.port());
  db.setUserName(dbInfo.userName());
  db.setPassword(dbInfo.password());
  db.setConnectOptions(dbInfo.connectOptions());
  if(!db.open()){
    auto error = mdtErrorNewQ(tr("Connection to database") + " '" + db.databaseName() + "' " + tr("failed."), mdtError::Error, this);
    error.stackError(mdt::sql::error::fromQSqlError(db.lastError()));
    error.commit();
    emit globalErrorOccured(error);
  }

  return db;
}

}}} // namespace mdt{ namespace sql{ namespace copier{
