/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtAlgorithms.h"
#include "mdtError.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QDebug>

mdtSqlDatabaseCopierThread::mdtSqlDatabaseCopierThread(QObject* parent)
 : QThread(parent)
{
}

void mdtSqlDatabaseCopierThread::copyData(const mdtSqlDatabaseCopierMapping & mapping)
{
  Q_ASSERT(!isRunning());

  pvMapping = mapping;
  Q_ASSERT(&pvMapping != &mapping);
  start();
}

void mdtSqlDatabaseCopierThread::run()
{
  QString sourceConnectionName;
  QString destinationConnectionName;
  {
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

    qDebug() << "THD started ...";

    // Get table mapping list
    auto tableMappingList = pvMapping.getCompletedTableMappingList();
    // Copy data of each table
    for(const auto & tm : tableMappingList){
      /// \todo return value
      copyTable(tm, sourceDatabase, destinationDatabase);
    }

    qDebug() << "THD end";
  }
  QSqlDatabase::removeDatabase(sourceConnectionName);
  QSqlDatabase::removeDatabase(destinationConnectionName);
}

QSqlDatabase mdtSqlDatabaseCopierThread::createConnection(const QSqlDatabase & dbInfo)
{
  QSqlDatabase db;
  QString cnn = mdtAlgorithms::generateString(6, QSqlDatabase::connectionNames());
  db = QSqlDatabase::addDatabase(dbInfo.driverName(), cnn);

  qDebug() << "Driver: " << db.driverName();
  qDebug() << "DB: " << dbInfo.databaseName();
  
  db.setDatabaseName(dbInfo.databaseName());
  db.setHostName(dbInfo.hostName());
  db.setPort(dbInfo.port());
  db.setUserName(dbInfo.userName());
  db.setPassword(dbInfo.password());
  db.setConnectOptions(dbInfo.connectOptions());
  if(!db.open()){
    QSqlError sqlError = db.lastError();
    mdtError error(tr("Connection to database") + " '" + db.databaseName() + "' " + tr("failed."), mdtError::Error);
    error.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(error, "mdtSqlDatabaseCopierThread");
    error.commit();
    ///emit globalErrorOccured(error);
  }

  return db;
}

bool mdtSqlDatabaseCopierThread::isSameDatabase(const QSqlDatabase& dbA, const QSqlDatabase& dbB) const
{
  /// \todo Currently, only SQLite version is implemented
  return (dbA.databaseName() == dbB.databaseName());
}

bool mdtSqlDatabaseCopierThread::copyTable(const mdtSqlDatabaseCopierTableMapping & tm,
                                           const QSqlDatabase & sourceDatabase, const QSqlDatabase& destinationDatabase)
{
  QSqlQuery sourceQuery(sourceDatabase);
  QSqlQuery destinationQuery(destinationDatabase);
  QString sql;

  sourceQuery.setForwardOnly(true);
  destinationQuery.setForwardOnly(true);

  // Get source table data
  sql = tm.getSqlForSourceTableSelect(sourceDatabase);
  if(!sourceQuery.exec(sql)){
    QSqlError sqlError = sourceQuery.lastError();
    mdtError error(tr("Cannot select data from  table '") + tm.sourceTableName() + tr("'"), mdtError::Error);
    error.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(error, "mdtSqlDatabaseCopierThread");
    error.commit();
    ///emit objectErrorOccured(mdtSqlDatabaseSchemaModel::Table, tableName, error);
    ///emit objectProgressChanged(mdtSqlDatabaseSchemaModel::Table, tableName, 0);
    return false;
  }
  // Copy each row
  while(sourceQuery.next()){
    // Prepare destination query for insertion
    sql = tm.getSqlForDestinationTableInsert(destinationDatabase);
    if(!destinationQuery.prepare(sql)){
      QSqlError sqlError = sourceQuery.lastError();
      mdtError error(tr("Cannot prepare stamenet for insertion into table '") + tm.destinationTableName() + tr("'"), mdtError::Error);
      error.setSystemError(sqlError.number(), sqlError.text());
      MDT_ERROR_SET_SRC(error, "mdtSqlDatabaseCopierThread");
      error.commit();
      ///emit objectErrorOccured(mdtSqlDatabaseSchemaModel::Table, tableName, error);
      ///emit objectProgressChanged(mdtSqlDatabaseSchemaModel::Table, tableName, 0);
      return false;
    }
    // Bind values for each field
    int fieldCount = sourceQuery.record().count();
    for(int i = 0; i < fieldCount; ++i){
      destinationQuery.bindValue(i, sourceQuery.value(i));
    }
    // Copy this row
    if(!destinationQuery.exec()){
      QSqlError sqlError = sourceQuery.lastError();
      mdtError error(tr("Cannot execute query for insertion into table '") + tm.destinationTableName() + tr("'"), mdtError::Error);
      error.setSystemError(sqlError.number(), sqlError.text());
      MDT_ERROR_SET_SRC(error, "mdtSqlDatabaseCopierThread");
      error.commit();
      ///emit objectErrorOccured(mdtSqlDatabaseSchemaModel::Table, tableName, error);
      ///emit objectProgressChanged(mdtSqlDatabaseSchemaModel::Table, tableName, 0);
      return false;
    }
  }

  return true;
}
