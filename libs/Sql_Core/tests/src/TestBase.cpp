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
#include "TestBase.h"
#include "Schema/TestSchema.h"
#include "Schema/Client.h"
#include "Mdt/Sql/SQLiteConnectionParameters.h"
#include "Mdt/Sql/SQLiteDatabase.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/InsertQuery.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>

using Mdt::Sql::SQLiteConnectionParameters;
using Mdt::Sql::SQLiteDatabase;

TestBase::TestBase()
 : QObject()
{
}

TestBase::~TestBase()
{
  closeDatabase();
}

bool TestBase::initDatabaseTemporaryFile()
{
  if(!mTempFile.open()){
    qWarning() << "Could not open file " << mTempFile.fileName();
    return false;
  }
  mTempFile.close();
  mConnectionParameters.setDatabaseFile(mTempFile.fileName());

  return true;
}

bool TestBase::initDatabaseSqlite()
{
  const auto connection = SQLiteDatabase::addConnection();
  if(!connection){
    qWarning() << "Could not add a connection to as SQLite database: " << connection.error().text();
    return false;
  }
  mConnectionName = connection->name();

  if(!initDatabaseTemporaryFile()){
    return false;
  }

  SQLiteDatabase db(*connection, mConnectionParameters);
  const auto result = db.open();
  if(!result){
    qWarning() << "Could not open database, error: " << result.error().text();
    return false;
  }

  return true;
}

bool TestBase::openDatabaseIfNot()
{
  if(mConnectionName.isEmpty()){
    return initDatabaseSqlite();
  }

  SQLiteDatabase db(connection(), mConnectionParameters);
  const auto result = db.open();
  if(!result){
    qWarning() << "Could not open database, error: " << result.error().text();
    return false;
  }

  return true;
}

void TestBase::closeDatabase()
{
  if(!mConnectionName.isEmpty()){
    Mdt::Sql::Connection::close(connection());
  }
}

bool TestBase::createClientTable()
{
  Q_ASSERT(isDatabaseOpen());

  Mdt::Sql::Schema::Driver driver(database());
  Q_ASSERT(driver.isValid());

  if(!driver.createTable( Schema::Client() )){
    qWarning() << "Could not create client table, error: " << driver.lastError().text();
    return false;
  }

  return true;
}

bool TestBase::insertClient(int id, const QString& name)
{
  Q_ASSERT(isDatabaseOpen());

  Schema::Client client;
  Mdt::Sql::InsertQuery query(database());
  query.setTable(client);
  query.addValue(client.Id_PK(), id);
  query.addValue(client.Name(), name);
  if(!query.exec()){
    qWarning() << "Insert client failed: " << query.lastError().text();
    return false;
  }
  return true;
}

Client TestBase::getClient(int id)
{
  Q_ASSERT(isDatabaseOpen());

  Client client;
  QSqlQuery query(database());

  const QString sql = "SELECT Id_PK, Name FROM Client_tbl WHERE Id_PK=" + QString::number(id);
  if(!query.exec(sql)){
    qWarning() << "Get client with id " << id << " failed: " << query.lastError().text();
    return client;
  }
  if(!query.next()){
    qWarning() << "Get client with id " << id << " failed (not found)";
    return client;
  }
  Q_ASSERT(query.record().count() == 2);
  client.id = query.value(0).toInt();
  client.name = query.value(1).toString();

  return client;
}

bool TestBase::cleanupClientTable()
{
  Q_ASSERT(isDatabaseOpen());

  QSqlQuery query(database());
  if(!query.exec("DELETE FROM Client_tbl")){
    qWarning() << "Cleanup Client_tbl failed: " << query.lastError().text();
    return false;
  }
  return true;
}

bool TestBase::createTestSchema()
{
  Q_ASSERT(isDatabaseOpen());

  Mdt::Sql::Schema::Driver driver(database());
  Q_ASSERT(driver.isValid());

  if(!driver.createSchema(Schema::TestSchema())){
    qWarning() << "Could not create test schema, error: " << driver.lastError().text();
    return false;
  }

  return true;
}
