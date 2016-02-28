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
#include "mdtSqlDatabaseCopierThread.h"
#include "mdtSqlDatabaseCopierMappingModel.h"
#include "mdtSqlCopierDataMapping.h"
#include "mdtAlgorithms.h"
#include "mdtSqlTransaction.h"
#include "mdtError.h"
#include "mdt/sql/error/Error.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

//#include <QDebug>

using mdt::sql::copier::DatabaseCopierTableMapping;

mdtSqlDatabaseCopierThread::mdtSqlDatabaseCopierThread(QObject* parent)
 : QThread(parent),
   pvAbort(false)
{
}

void mdtSqlDatabaseCopierThread::copyData(const mdtSqlDatabaseCopierMapping & mapping)
{
  Q_ASSERT(!isRunning());

  pvMapping = mapping;
  Q_ASSERT(&pvMapping != &mapping);
  start();
}

void mdtSqlDatabaseCopierThread::abort()
{
  pvAbort = true;
}

void mdtSqlDatabaseCopierThread::run()
{
  QString sourceConnectionName;
  QString destinationConnectionName;
  {
    mdtProgressValue<int64_t> globalProgress;
    int64_t totalSize;
    auto sourceDatabase = createConnection(pvMapping.sourceDatabase());
    sourceConnectionName = sourceDatabase.connectionName();
    QSqlDatabase destinationDatabase;
    if(isSameDatabase(sourceDatabase, pvMapping.destinationDatabase())){
      destinationDatabase = sourceDatabase;
    }else{
      destinationDatabase = createConnection(pvMapping.destinationDatabase());
    }
    destinationConnectionName = destinationDatabase.connectionName();

    // Check that we are successfully connected to both databases
    if(!sourceDatabase.isOpen()){
      return;
    }
    if(!destinationDatabase.isOpen()){
      return;
    }
    /*
     * Init
     */
    pvAbort = false;
    // Calculate row count of each table that has a valid mapping
    emit globalProgressRangeChanged(0, 0);
    calculateTableSizes(sourceDatabase);
    totalSize = getTotalCopySize();
    if(totalSize > 0){
      globalProgress.setRange(0, totalSize);
    }
    emit globalProgressRangeChanged(0, 100);
    // Get table mapping list
    auto tableMappingList = pvMapping.tableMappingList();
    /*
     * Copy each table that has a valid mapping
     */
    for(int i = 0; i < tableMappingList.size(); ++i){
      auto tm = tableMappingList.at(i);
      Q_ASSERT(tm);
      if(tm->mappingState() == DatabaseCopierTableMapping::MappingComplete){
        /// \todo return value
        copyTable(tm, i, sourceDatabase, destinationDatabase, globalProgress);
      }
    }
  }
  QSqlDatabase::removeDatabase(sourceConnectionName);
  QSqlDatabase::removeDatabase(destinationConnectionName);
}

QSqlDatabase mdtSqlDatabaseCopierThread::createConnection(const QSqlDatabase & dbInfo)
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

bool mdtSqlDatabaseCopierThread::isSameDatabase(const QSqlDatabase& dbA, const QSqlDatabase& dbB) const
{
  /// \todo Currently, only SQLite version is implemented
  return (dbA.databaseName() == dbB.databaseName());
}

void mdtSqlDatabaseCopierThread::calculateTableSizes(const QSqlDatabase & sourceDatabase)
{
  QSqlQuery query(sourceDatabase);
  QString sql;
  auto tableMappingList = pvMapping.tableMappingList();

  pvTableSizeList.clear();
  pvTableSizeList.assign(tableMappingList.size(), -1);
  // Calculate table siz for each valid mapping
  for(int i = 0; i < tableMappingList.size(); ++i){
    auto tm = tableMappingList.at(i);
    if(tm->mappingState() == DatabaseCopierTableMapping::MappingComplete){
      sql = tm->getSqlForSourceTableCount(sourceDatabase);
      if( (query.exec(sql)) && (query.next()) ){
        pvTableSizeList[i] = query.value(0).toLongLong();
      }else{
        pvTableSizeList[i] = -1;
      }
    }
  }
}

int64_t mdtSqlDatabaseCopierThread::tableSize(int dbMappingModelRow) const
{
  Q_ASSERT(dbMappingModelRow >= 0);
  Q_ASSERT(dbMappingModelRow < (int)pvTableSizeList.size());

  return pvTableSizeList[dbMappingModelRow];
}

int64_t mdtSqlDatabaseCopierThread::getTotalCopySize() const
{
  int64_t totalSize = 0;

  for(const auto size : pvTableSizeList){
    if(size > 0){
      totalSize += size;
    }
  }

  return totalSize;
}

bool mdtSqlDatabaseCopierThread::copyTable(const std::shared_ptr<DatabaseCopierTableMapping> & tm, int dbMappingModelRow,
                                           const QSqlDatabase & sourceDatabase, const QSqlDatabase& destinationDatabase,
                                           mdtProgressValue<int64_t> & globalProgress)
{
  Q_ASSERT(tm);

  QSqlQuery sourceQuery(sourceDatabase);
  QSqlQuery destinationQuery(destinationDatabase);
  mdtSqlTransaction transaction(destinationDatabase);
  QString sql;
  mdtProgressValue<int64_t> progress;
  auto totalRows = tableSize(dbMappingModelRow);
  mdtSqlCopierDataMapping dataMapping;

  if(pvAbort){
    return true;
  }
  // Setup queries
  sourceQuery.setForwardOnly(true);
  destinationQuery.setForwardOnly(true);
  // Setup progress
  if(totalRows > 0){
    progress.setRange(0, totalRows);
  }
  // Get source table data
  sql = tm->getSqlForSourceTableSelect(sourceDatabase);
  if(!sourceQuery.exec(sql)){
    auto error = mdtErrorNewQ(tr("Cannot select data from  table '") + tm->sourceTableName() + tr("'"), mdtError::Error, this);
    error.stackError(mdt::sql::error::fromQSqlError(sourceQuery.lastError()));
    error.commit();
    emit tableCopyErrorOccured(dbMappingModelRow, error);
    ///emit objectProgressChanged(mdtSqlDatabaseSchemaModel::Table, tableName, 0);
    return false;
  }
  // Beginn transaction
  if(!transaction.begin()){
    mdtError error = transaction.lastError();
    MDT_ERROR_SET_SRC(error, "mdtSqlDatabaseCopierThread");
    error.commit();
    emit tableCopyErrorOccured(dbMappingModelRow, error);
    return false;
  }
  // Copy each row
  while(sourceQuery.next()){
    if(pvAbort){
      return true;
    }
    // Prepare destination query for insertion
    sql = tm->getSqlForDestinationTableInsert(destinationDatabase);
    if(!destinationQuery.prepare(sql)){
      auto error = mdtErrorNewQ(tr("Cannot prepare stamenet for insertion into table '") + tm->destinationTableName() + tr("'"), mdtError::Error, this);
      error.stackError(mdt::sql::error::fromQSqlError(sourceQuery.lastError()));
      error.commit();
      emit tableCopyErrorOccured(dbMappingModelRow, error);
      ///emit objectProgressChanged(mdtSqlDatabaseSchemaModel::Table, tableName, 0);
      return false;
    }
    // Bind values for each field
    dataMapping.setSourceRecord(sourceQuery.record(), tm);
    for(const auto & value : dataMapping.destinationRecord){
      destinationQuery.addBindValue(value);
    }
    // Copy this row
    if(!destinationQuery.exec()){
      auto error = mdtErrorNewQ(tr("Cannot execute query for insertion into table '") + tm->destinationTableName() + tr("'"), mdtError::Error, this);
      error.stackError(mdt::sql::error::fromQSqlError(destinationQuery.lastError()));
      error.commit();
      emit tableCopyErrorOccured(dbMappingModelRow, error);
      ///emit objectProgressChanged(mdtSqlDatabaseSchemaModel::Table, tableName, 0);
      return false;
    }
    // Update progress
    ++globalProgress;
    ++progress;
    if(progress.isTimeToNotify()){
      emit tableCopyProgressChanged(dbMappingModelRow, progress.scaledValue());
      emit globalProgressValueChanged(globalProgress.scaledValue());
    }
  }
  // Commit transaction
  if(!transaction.commit()){
    mdtError error = transaction.lastError();
    MDT_ERROR_SET_SRC(error, "mdtSqlDatabaseCopierThread");
    error.commit();
    emit tableCopyErrorOccured(dbMappingModelRow, error);
    return false;
  }
  // Notify end
  emit tableCopyStatusChanged(dbMappingModelRow, mdtSqlDatabaseCopierMappingModel::CopyStatusOk);
  emit tableCopyProgressChanged(dbMappingModelRow, progress.scaledValue());
  emit globalProgressValueChanged(globalProgress.scaledValue());

  return true;
}
