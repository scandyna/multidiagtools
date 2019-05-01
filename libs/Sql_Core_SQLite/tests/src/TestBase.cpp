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
#include "Mdt/Sql/SQLiteDatabase.h"
#include "Mdt/Sql/Schema/Table.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/InsertQuery.h"
#include "Mdt/QueryExpression/SelectStatement.h"
#include "Mdt/Sql/SelectQuery.h"
#include "Mdt/Sql/DeleteQuery.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

using namespace Mdt::Sql;

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
    QSqlDatabase db = database();
    db.close();
  }
}

bool TestBase::createClientTable()
{
  Q_ASSERT(isDatabaseOpen());

  using namespace Mdt::Sql::Schema;

  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(100);

  Table client;
  client.setTableName("Client_tbl");
  client.setAutoIncrementPrimaryKey("Id_PK");
  client.addField(Name);

  Driver driver(database());
  if(!driver.createTable(client)){
    qWarning() << "Could not create table '" << client.tableName() << "': " << driver.lastError().text();
    return false;
  }

  return true;
}

Client TestBase::getClient(int id)
{
  using namespace Mdt::QueryExpression;

  Q_ASSERT(isDatabaseOpen());
  Q_ASSERT(id > 0);

  Client client;
  QueryField id_Pk("Id_PK");

  SelectStatement stm;
  stm.setEntityName("Client_tbl");
  stm.addField(id_Pk);
  stm.addField("name");
  stm.setFilter(id_Pk == id);

  SelectQuery query(database());
  if(!query.execStatement(stm)){
    qWarning() << "Could not get client for id '" << id << "': " << query.lastError().text();
    return client;
  }

  const QSqlRecord record = query.fetchSingleRecord();
  if(record.isEmpty()){
    qWarning() << "Could not get client for id '" << id << "': " << query.lastError().text();
    return client;
  }
  Q_ASSERT(record.count() == 2);
  client.id = record.value(0).toInt();
  client.name = record.value(1).toString();

  return client;
}

bool TestBase::cleanupClientTable()
{
  Q_ASSERT(isDatabaseOpen());

  DeleteStatement statement;
  statement.setTableName("Client_tbl");

  DeleteQuery query(database());
  if(!query.execStatement(statement)){
    qWarning() << "Could not cleanup client table: " << query.lastError().text();
    return false;
  }

  return true;
}
