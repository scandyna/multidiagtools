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
#include "mdtSqlDatabaseSchemaThread.h"
#include "mdtSqlDatabaseSchemaModel.h"
#include "mdtSqlSchemaTable.h"
#include "mdtAlgorithms.h"
#include <QSqlError>
#include <QSqlQuery>

#include <QDebug>

mdtSqlDatabaseSchemaThread::mdtSqlDatabaseSchemaThread(QObject* parent)
 : QThread(parent)
{
}

void mdtSqlDatabaseSchemaThread::createSchema(const mdtSqlDatabaseSchema & s, const QSqlDatabase & db)
{
  Q_ASSERT(!isRunning());

  pvSchema = s;
  Q_ASSERT(&pvSchema != &s);
  pvDatabaseInfo = db;
  start();
}

void mdtSqlDatabaseSchemaThread::run()
{
  QString connectionName;
  {
    auto db = createConnection();
    connectionName = db.connectionName();
    auto tables = pvSchema.tableList();

    qDebug() << "THD started...";

    // Check that we are successfully connected to database
    if(!db.isOpen()){
      return;
    }
    // Create tables
    for(auto & ts : tables){
      createTable(ts, db);
    }
    
    for(int i = 0; i <= 100; ++i){
      emit objectProgressChanged(mdtSqlDatabaseSchemaModel::Table, "", i);
      msleep(100);
    }
    for(int i = 0; i <= 100; ++i){
      emit objectProgressChanged(mdtSqlDatabaseSchemaModel::View, "", i);
      msleep(10);
    }

    qDebug() << "THD END";
    
    // Close database
    db.close();
  }
  QSqlDatabase::removeDatabase(connectionName);
}

QSqlDatabase mdtSqlDatabaseSchemaThread::createConnection()
{
  QString cnn = mdtAlgorithms::generateString(6, QSqlDatabase::connectionNames());
  QSqlDatabase db = QSqlDatabase::addDatabase(pvDatabaseInfo.driverName(), cnn);

  qDebug() << "Driver: " << pvDatabaseInfo.driverName();
  qDebug() << "DB: " << pvDatabaseInfo.databaseName();
  
  db.setDatabaseName(pvDatabaseInfo.databaseName());
  db.setHostName(pvDatabaseInfo.hostName());
  db.setPort(pvDatabaseInfo.port());
  db.setUserName(pvDatabaseInfo.userName());
  db.setPassword(pvDatabaseInfo.password());
  db.setConnectOptions(pvDatabaseInfo.connectOptions());
  if(!db.open()){
    QSqlError sqlError = db.lastError();
    mdtError error(tr("Connection to database") + " '" + db.databaseName() + "' " + tr("failed."), mdtError::Error);
    error.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(error, "mdtSqlDatabaseSchemaThread");
    error.commit();
    emit globalErrorOccured(error);
  }

  return db;
}

void mdtSqlDatabaseSchemaThread::createTable(mdtSqlSchemaTable & ts, const QSqlDatabase & db)
{
  QSqlQuery query(db);

  if(!ts.setDriverName(db.driverName())){
    emit errorOccured(mdtSqlDatabaseSchemaModel::Table, ts.tableName(), ts.lastError());
    return;
  }
  if(!query.exec(ts.sqlForCreateTable())){
    QSqlError sqlError = query.lastError();
    mdtError error(tr("Cannot create table '") + ts.tableName() + tr("'"), mdtError::Error);
    error.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(error, "mdtSqlDatabaseSchemaThread");
    error.commit();
    emit errorOccured(mdtSqlDatabaseSchemaModel::Table, ts.tableName(), error);
  }
}
