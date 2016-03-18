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
#include "DatabaseCopyThread.h"
#include "DatabaseMappingModel.h"
#include "DatabaseCopierTableMapping.h"
#include "CopyHelper.h"
#include "mdt/sql/Database.h"
#include "mdt/sql/error/Error.h"
#include "mdtSqlTransaction.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

namespace mdt{ namespace sql{ namespace copier{

DatabaseCopyThread::DatabaseCopyThread(QObject *parent)
 : CopyThread(parent),
   pvModel(nullptr)
{
}

int DatabaseCopyThread::tableMappingCount() const
{
  if(pvModel == nullptr){
    return 0;
  }
  return pvModel->rowCount();
}

// void DatabaseCopyThread::startCopy(const DatabaseMapping & mapping)
// {
//   Q_ASSERT(!isRunning());
// 
// //   pvMapping = mapping;
//   resetCopyProgress();
//   start();
// }

void DatabaseCopyThread::startCopy(const DatabaseMappingModel *const model)
{
  Q_ASSERT(model != nullptr);

  pvModel = model;
  resetCopyProgress();
  start();
}

int64_t DatabaseCopyThread::calculateSourceTableSize(const DatabaseCopierTableMapping * const tm, const QSqlDatabase & sourceDatabase) const
{
  Q_ASSERT(tm != nullptr);

  QSqlQuery query(sourceDatabase);
  auto sql = CopyHelper::getSourceTableCountSql(tm, sourceDatabase);

  if(!query.exec(sql)){
    auto error = mdtErrorNewQ(tr("Executing query to get source table count failed"), mdtError::Warning, this);
    error.stackError(ErrorFromQSqlQuery(query));
    error.commit();
    return -1;
  }
  if(!query.next()){
    return -1;
  }

  return query.value(0).toLongLong();
}

void DatabaseCopyThread::calculateTableSizes(const QSqlDatabase & sourceDatabase)
{
  for(int i = 0; i < pvModel->rowCount(); ++i){
    if(pvAbort){
      return;
    }
    if(pvModel->isTableMappingSelected(i)){
      const auto tm = pvModel->tableMapping(i);
      Q_ASSERT(tm);
      auto size = calculateSourceTableSize(tm.get(), sourceDatabase);
      setTableSize(i, size);
    }
  }
//   for(int i = 0; i < pvMapping.tableMappingCount(); ++i){
//     if(pvAbort){
//       return;
//     }
//     const auto tm = pvMapping.tableMapping(i);
//     Q_ASSERT(tm);
//     if(tm->mappingState() == TableMapping::MappingComplete){
//       auto size = calculateSourceTableSize(tm.get(), sourceDatabase);
//       setTableSize(i, size);
//     }
//   }
  calculateTotalCopySize();
}

bool DatabaseCopyThread::copyTable(const DatabaseCopierTableMapping *const tm, int index,
                                   const QSqlDatabase & sourceDatabase, const QSqlDatabase & destinationDatabase)
{
  Q_ASSERT(tm != nullptr);

  QSqlQuery sourceQuery(sourceDatabase);
  QSqlQuery destinationQuery(destinationDatabase);
  mdtSqlTransaction transaction(destinationDatabase);
  QString sql;

  if(pvAbort){
    return true;
  }
  // Setup queries
  sourceQuery.setForwardOnly(true);
  destinationQuery.setForwardOnly(true);

  // Get values of all fields in source table
  sql = CopyHelper::getSourceTableSelectSql(tm, sourceDatabase);
  if(!sourceQuery.exec(sql)){
    auto error = mdtErrorNewQ(tr("Executing query to get source table records failed"), mdtError::Error, this);
    error.stackError(ErrorFromQSqlQuery(sourceQuery));
    error.commit();
    emit tableCopyErrorOccured(index, error);
    return false;
  }
  // Prepare destination table INSERT
  sql = CopyHelper::getDestinationInsertSql(tm, destinationDatabase);
  if(!destinationQuery.prepare(sql)){
    auto error = mdtErrorNewQ(tr("Preparing query for insertion in destination table failed"), mdtError::Error, this);
    error.stackError(ErrorFromQSqlQuery(destinationQuery));
    error.commit();
    emit tableCopyErrorOccured(index, error);
    return false;
  }
  // Beginn transaction
  if(!transaction.begin()){
    auto error = transaction.lastError();
    MDT_ERROR_SET_SRC_Q(error, this);
    error.commit();
    emit tableCopyErrorOccured(index, error);
    return false;
  }
  // Copy each record
  while(sourceQuery.next()){
    if(pvAbort){
      return true;
    }
    // If requiered, check if record allready exists in destination table
    if(!tm->uniqueInsertCriteria().isNull()){
      auto ret = CopyHelper::checkExistsInDestinationTable(sourceQuery.record(), tm, destinationDatabase);
      if(!ret){
        emit tableCopyErrorOccured(index, ret.error());
        return false;
      }
      if(ret.value()){
        continue;
      }
    }
    // Build destination record
    auto destinationRecord = CopyHelper::getDestinationRecord(sourceQuery.record(), tm, destinationDatabase);
    if(!destinationRecord){
      emit tableCopyErrorOccured(index, destinationRecord.error());
      return false;
    }
    // Bind values to destination query
    for(const auto & data : destinationRecord.value()){
      destinationQuery.addBindValue(data);
    }
    qDebug() << "Bound: " << destinationQuery.boundValues();
    // Execute
    if(!destinationQuery.exec()){
      auto error = mdtErrorNewQ(tr("Executing query for insertion in destination table failed"), mdtError::Error, this);
      error.stackError(ErrorFromQSqlQuery(destinationQuery));
      error.commit();
      emit tableCopyErrorOccured(index, error);
      return false;
    }
    // Update copy progress
    incrementTableCopyProgress(index);
  }
  // Commit transaction
  if(!transaction.commit()){
    auto error = transaction.lastError();
    MDT_ERROR_SET_SRC_Q(error, this);
    error.commit();
    emit tableCopyErrorOccured(index, error);
    return false;
  }
  // Notify end of table copy
  notifyTableCopyDone(index);

  return true;
}

void DatabaseCopyThread::run()
{
  using mdt::sql::Database;
  using mdt::sql::copier::TableMapping;

  QString sourceConnectionName;
  QString destinationConnectionName;
  {
    auto sourceDatabase = createConnection(pvModel->sourceDatabase());
    sourceConnectionName = sourceDatabase.connectionName();
    QSqlDatabase destinationDatabase;
    if(Database::isSameDatabase(sourceDatabase, pvModel->destinationDatabase())){
      destinationDatabase = sourceDatabase;
    }else{
      destinationDatabase = createConnection(pvModel->destinationDatabase());
    }
    destinationConnectionName = destinationDatabase.connectionName();

    // Check that we are successfully connected to both databases
    if(!sourceDatabase.isOpen()){
      return;
    }
    if(!destinationDatabase.isOpen()){
      return;
    }
    pvAbort = false;
    // Calculate copy size
    calculateTableSizes(sourceDatabase);
    // Copy each table that is selected
    for(int i = 0; i < pvModel->rowCount(); ++i){
      if(pvAbort){
        break;
      }
      if(pvModel->isTableMappingSelected(i)){
        const auto tm = pvModel->tableMapping(i);
        Q_ASSERT(tm);
        copyTable(tm.get(), i, sourceDatabase, destinationDatabase);
      }
    }
//     for(int i = 0; i < pvMapping.tableMappingCount(); ++i){
//       if(pvAbort){
//         break;
//       }
//       const auto tm = pvMapping.tableMapping(i);
//       Q_ASSERT(tm);
//       if(tm->mappingState() == TableMapping::MappingComplete){
//         copyTable(tm.get(), i, sourceDatabase, destinationDatabase);
//       }
//     }
  }
  QSqlDatabase::removeDatabase(sourceConnectionName);
  QSqlDatabase::removeDatabase(destinationConnectionName);
}

}}} // namespace mdt{ namespace sql{ namespace copier{
