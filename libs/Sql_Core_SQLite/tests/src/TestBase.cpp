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
#include "TestBase.h"
// #include "Schema/TestSchema.h"
// #include "Schema/Client.h"
#include "Mdt/Sql/SQLiteConnectionParameters.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/InsertQuery.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

using Mdt::Sql::SQLiteConnectionParameters;

bool TestBase::initDatabaseSqlite()
{
  SQLiteConnectionParameters parameters;

  // Create a database
  if(!mTempFile.open()){
    qWarning() << "Could not open file " << mTempFile.fileName();
    return false;
  }
  mTempFile.close();
  parameters.setDatabaseFile(mTempFile.fileName());
  mConnectionParameters = parameters.toConnectionParameters();

  // Get database instance
  qDebug() << "Add MDTQSQLITE database...";
  mDatabase = QSqlDatabase::addDatabase("MDTQSQLITE");
  if(!mDatabase.isValid()){
    qWarning() << "MDTQSQLITE driver is not available";
    return false;
  }
  mDatabase.setConnectOptions(QLatin1String("QSQLITE_USE_EXTENDED_RESULT_CODES"));
  mConnectionParameters.setupDatabase(mDatabase);
  if(!mDatabase.open()){
    qWarning() << "Could not open database, error: " << mDatabase.lastError();
    return false;
  }

  return true;
}

QSqlDatabase TestBase::database() const
{
  return mDatabase;
}

Mdt::Sql::ConnectionParameters TestBase::connectionParameters() const
{
  return mConnectionParameters;
}

// bool TestBase::createClientTable()
// {
//   Mdt::Sql::Schema::Driver driver(mDatabase);
//   Q_ASSERT(driver.isValid());
// 
//   if(!driver.createTable( Schema::Client() )){
//     qWarning() << "Could not create client table, error: " << driver.lastError().text();
//     return false;
//   }
// 
//   return true;
// }

// bool TestBase::insertClient(int id, const QString& name)
// {
//   Schema::Client client;
//   Mdt::Sql::InsertQuery query(mDatabase);
//   query.setTable(client);
//   query.addValue(client.Id_PK(), id);
//   query.addValue(client.Name(), name);
//   if(!query.exec()){
//     qWarning() << "Insert client failed: " << query.lastError().text();
//     return false;
//   }
//   return true;
// }

// bool TestBase::cleanupClientTable()
// {
//   QSqlQuery query(mDatabase);
//   if(!query.exec("DELETE FROM Client_tbl")){
//     qWarning() << "Cleanup Client_tbl failed: " << query.lastError().text();
//     return false;
//   }
//   return true;
// }

// bool TestBase::createTestSchema()
// {
//   Mdt::Sql::Schema::Driver driver(mDatabase);
//   Q_ASSERT(driver.isValid());
// 
//   if(!driver.createSchema(Schema::TestSchema())){
//     qWarning() << "Could not create test schema, error: " << driver.lastError().text();
//     return false;
//   }
// 
//   return true;
// }
