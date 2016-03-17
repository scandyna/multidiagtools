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

#include <QDebug>

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

void CopyThread::resetCopyProgress()
{
  pvTableSizeList.assign(tableMappingCount(), -1);
  pvTableProgressList.assign(tableMappingCount(), mdtProgressValue<int64_t>());
  pvGlobalProgress.setValue(0);
  emit globalProgressRangeChanged(0, 0);
}

void CopyThread::setTableSize(int index, int64_t size)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < (int)pvTableSizeList.size());
  Q_ASSERT(index < (int)pvTableProgressList.size());

  pvTableSizeList[index] = size;
  if(size > 0){
    pvTableProgressList[index].setRange(0, size);
  }
}

int64_t CopyThread::calculateTotalCopySize()
{
  Q_ASSERT(pvTableSizeList.size() == pvTableProgressList.size());

  int64_t totalSize = 0;

  for(size_t i = 0; i < pvTableSizeList.size(); ++i){
    auto size = pvTableSizeList[i];
    if(size > 0){
      totalSize += size;
    }
  }
  if(totalSize > 0){
    pvGlobalProgress.setRange(0, totalSize);
    emit globalProgressRangeChanged(0, 100);
  }

  return totalSize;
}

void CopyThread::incrementTableCopyProgress(int index, int64_t n)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < (int)pvTableProgressList.size());

  // Increment table copy progress
  auto & tableProgress = pvTableProgressList[index];
  tableProgress.increment(n);
  // Increment global progress and notify if timeout
  pvGlobalProgress.increment(n);
  if(pvGlobalProgress.isTimeToNotify()){
    // Notify progress of each table
    for(size_t i = 0; i < pvTableProgressList.size(); ++i){
      auto & tcp = pvTableProgressList[i];
      emit tableCopyProgressChanged(i, tcp.scaledValue());
    }
    // Notify global progress
    emit globalProgressValueChanged(pvGlobalProgress.scaledValue());
  }
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
