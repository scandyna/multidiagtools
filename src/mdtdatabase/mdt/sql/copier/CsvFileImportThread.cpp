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
#include "CsvFileImportThread.h"
#include "CsvFileImportMappingModel.h"
#include "CsvFileImportTableMapping.h"
#include "CopyHelper.h"
#include "mdtCsvFileParser.h"
#include "mdtCsvData.h"
#include "mdt/csv/RecordFormatter.h"
#include "mdt/sql/error/Error.h"
#include "mdtSqlTransaction.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <memory>

#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

CsvFileImportThread::CsvFileImportThread(QObject *parent)
 : CopyThread(parent),
   pvModel(nullptr)
{
}

int CsvFileImportThread::tableMappingCount() const
{
  if(pvModel == nullptr){
    return 0;
  }
  return pvModel->rowCount();
}

void CsvFileImportThread::startCopy(const CsvFileImportMappingModel *const model)
{
  Q_ASSERT(model != nullptr);

  pvModel = model;
  resetCopyProgress();
  start();
}

bool CsvFileImportThread::importFile(const CsvFileImportTableMapping *const tm, int index, const QSqlDatabase & destinationDatabase)
{
  Q_ASSERT(tm != nullptr);
  Q_ASSERT(tm->sourceCsvSettings().isValid());
  Q_ASSERT(pvModel != nullptr);

  qDebug() << "Importing " << tm->sourceTableName() << " to " << tm->destinationTableName();

  mdtCsvFileParser csvParser;
  mdt::csv::RecordFormatter formatter;
  QSqlQuery destinationQuery(destinationDatabase);
  mdtSqlTransaction transaction(destinationDatabase);
  QString sql;

  if(pvAbort){
    return true;
  }
  // Open CSV file
  csvParser.setCsvSettings(tm->sourceCsvSettings());
  if(!csvParser.openFile(tm->sourceFileInfo(), tm->sourceFileEncoding())){
    auto error = mdtErrorNewQ(tr("Open CSV file failed"), mdtError::Error, this);
    error.stackError(csvParser.lastError());
    error.commit();
    emit tableCopyErrorOccured(index, error);
    return false;
  }
  // Setup CSV formatter
  formatter.setFormat(tm->sourceRecordFormat());
  // Setup query
  destinationQuery.setForwardOnly(true);

  // Read CSV header
  auto csvHeader = csvParser.readLine();
  if(!csvHeader){
    auto error = mdtErrorNewQ(tr("Reading header in CSV file failed"), mdtError::Error, this);
    error.stackError(csvParser.lastError());
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
  while(!csvParser.atEnd()){
    if(pvAbort){
      return true;
    }
    // Read a line in CSV file
    auto csvRecordExp = csvParser.readLine();
    if(!csvRecordExp){
      auto error = mdtErrorNewQ(tr("Reading a line in CSV file failed"), mdtError::Error, this);
      error.stackError(csvParser.lastError());
      error.commit();
      emit tableCopyErrorOccured(index, error);
      return false;
    }
    auto csvRecord = csvRecordExp.value();
    // Format CSV line
    if(!formatter.formatRecord(csvRecord)){
      auto error = mdtErrorNewQ(tr("Formatting a record from CSV file failed"), mdtError::Error, this);
      error.stackError(formatter.lastError());
      error.commit();
      emit tableCopyErrorOccured(index, error);
      return false;
    }
    // If requiered, check if record allready exists in destination table
    if(!tm->uniqueInsertCriteria().isNull()){
      auto ret = CopyHelper::checkExistsInDestinationTable(csvRecord, tm, destinationDatabase);
      if(!ret){
        emit tableCopyErrorOccured(index, ret.error());
        return false;
      }
      if(ret.value()){
        continue;
      }
    }
    
    ///qDebug() << "Source record: " << csvRecord.columnDataList;
    
    // Build destination record
    auto destinationRecord = CopyHelper::getDestinationRecord(csvRecord, tm, destinationDatabase);
    if(!destinationRecord){
      emit tableCopyErrorOccured(index, destinationRecord.error());
      ///return false;
      continue;
    }
    // Bind values to destination query
    for(const auto & data : destinationRecord.value()){
      destinationQuery.addBindValue(data);
    }
    // Execute
    if(!destinationQuery.exec()){
      auto error = mdtErrorNewQ(tr("Executing query for insertion in destination table failed"), mdtError::Error, this);
      error.stackError(ErrorFromQSqlQuery(destinationQuery));
      error.commit();
      emit tableCopyErrorOccured(index, error);
      return false;
    }
    
    qDebug() << "Inserted, SQL: " << destinationQuery.executedQuery();
    qDebug() << " -> bound values: " << destinationQuery.boundValues();
    
    // Update copy progress
///    incrementTableCopyProgress(index);
  }
  // Commit transaction
  if(!transaction.commit()){
    auto error = transaction.lastError();
    MDT_ERROR_SET_SRC_Q(error, this);
    error.commit();
    emit tableCopyErrorOccured(index, error);
    return false;
  }
  
  qDebug() << "Commit";
  
  // Notify end of table copy
  notifyTableCopyDone(index);

  return true;
}

void CsvFileImportThread::run()
{
  Q_ASSERT(pvModel != nullptr);

  QString destinationConnectionName;
  {
    auto destinationDatabase = createConnection(pvModel->destinationDatabase());

    // Check that we are successfully connected to database
    if(!destinationDatabase.isOpen()){
      return;
    }
    destinationConnectionName = destinationDatabase.connectionName();
    pvAbort = false;
    // Calculate copy size
    ///calculateTableSizes(sourceDatabase);
    // Copy each table that is selected
    for(int i = 0; i < pvModel->rowCount(); ++i){
      if(pvAbort){
        break;
      }
      if(pvModel->isTableMappingSelected(i)){
        const auto tm = pvModel->tableMapping(i);
        Q_ASSERT(tm);
        importFile(tm.get(), i, destinationDatabase);
      }
    }

  }
  QSqlDatabase::removeDatabase(destinationConnectionName);
}

}}} // namespace mdt{ namespace sql{ namespace copier{
