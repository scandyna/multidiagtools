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
#include "AsyncQueryTest.h"
#include "Mdt/Sql/SQLiteAsyncQueryConnection.h"
#include "Mdt/Sql/SQLiteAsyncQueryThreadWorker.h"
#include "Mdt/Sql/AsyncInsertQuery.h"
#include "Mdt/Sql/InsertStatement.h"
#include <memory>

#include <QDebug>

using namespace Mdt::Sql;

void AsyncQueryTest::initTestCase()
{
  QVERIFY(initDatabaseTemporaryFile());
  QVERIFY(openDatabaseIfNot());
  QVERIFY(createClientTable());
  closeDatabase();
}

void AsyncQueryTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void AsyncQueryTest::threadWorkerOpenCloseTest()
{
  const int initialConnectionCount = QSqlDatabase::connectionNames().count();

  SQLiteAsyncQueryThreadWorker worker(connectionParameters());

  QVERIFY(!worker.isDatabaseOpen());
  QVERIFY(worker.open());
  QVERIFY(worker.isDatabaseOpen());
  worker.close();
  QVERIFY(!worker.isDatabaseOpen());
  QCOMPARE(QSqlDatabase::connectionNames().count(), initialConnectionCount);
}

void AsyncQueryTest::connectionOpenCloseTest()
{
  const int initialConnectionCount = QSqlDatabase::connectionNames().count();

  auto connection = std::make_shared<SQLiteAsyncQueryConnection>();

  QVERIFY(connection->open(connectionParameters()));
  connection->close();
  QCOMPARE(QSqlDatabase::connectionNames().count(), initialConnectionCount);

  {
    auto connection2 = std::make_shared<SQLiteAsyncQueryConnection>();
    QVERIFY(connection2->open(connectionParameters()));
  }
  QCOMPARE(QSqlDatabase::connectionNames().count(), initialConnectionCount);
}

void AsyncQueryTest::connectionOpenErrorTest()
{
  SQLiteAsyncQueryConnection connection;
  SQLiteConnectionParameters parameters;

  parameters.setDatabaseFile("/Some/Non/Existing/db");
  QVERIFY(connection.lastError().isNull());
  QVERIFY(!connection.open(parameters));
  QVERIFY(!connection.lastError().isNull());
}

void AsyncQueryTest::connectionOpenConnectionNameTest()
{
  const QString namePrefix = "AsyncConnection";

  QVERIFY(!QSqlDatabase::contains("AsyncConnection_1"));
  QVERIFY(!QSqlDatabase::contains("AsyncConnection_2"));
  QVERIFY(!QSqlDatabase::contains("AsyncConnection_3"));

  SQLiteAsyncQueryConnection connection1;
  QVERIFY(connection1.open(connectionParameters(), namePrefix));
  QVERIFY(QSqlDatabase::contains("AsyncConnection_1"));
  QVERIFY(!QSqlDatabase::contains("AsyncConnection_2"));
  QVERIFY(!QSqlDatabase::contains("AsyncConnection_3"));

  SQLiteAsyncQueryConnection connection2;
  QVERIFY(connection2.open(connectionParameters(), namePrefix));
  QVERIFY(QSqlDatabase::contains("AsyncConnection_1"));
  QVERIFY(QSqlDatabase::contains("AsyncConnection_2"));
  QVERIFY(!QSqlDatabase::contains("AsyncConnection_3"));

  SQLiteAsyncQueryConnection connection3;
  QVERIFY(connection3.open(connectionParameters(), namePrefix));
  QVERIFY(QSqlDatabase::contains("AsyncConnection_1"));
  QVERIFY(QSqlDatabase::contains("AsyncConnection_2"));
  QVERIFY(QSqlDatabase::contains("AsyncConnection_3"));

  connection1.close();
  QVERIFY(!QSqlDatabase::contains("AsyncConnection_1"));
  QVERIFY(QSqlDatabase::contains("AsyncConnection_2"));
  QVERIFY(QSqlDatabase::contains("AsyncConnection_3"));

  connection2.close();
  QVERIFY(!QSqlDatabase::contains("AsyncConnection_1"));
  QVERIFY(!QSqlDatabase::contains("AsyncConnection_2"));
  QVERIFY(QSqlDatabase::contains("AsyncConnection_3"));

  connection3.close();
  QVERIFY(!QSqlDatabase::contains("AsyncConnection_1"));
  QVERIFY(!QSqlDatabase::contains("AsyncConnection_2"));
  QVERIFY(!QSqlDatabase::contains("AsyncConnection_3"));
}

void AsyncQueryTest::connectionUsageTest()
{
  auto connection = std::make_shared<SQLiteAsyncQueryConnection>();
  QVERIFY(connection->open(connectionParameters()));

  InsertStatement statement;
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Id_PK"), 2);
  statement.addValue(FieldName("Name"), "Name 2");

  AsyncInsertQuery query(connection);
  QVERIFY(query.execStatement(statement));

  connection->close();

  QVERIFY(openDatabaseIfNot());
  Client client = getClient(2);
  QCOMPARE(client.id, 2);
  QCOMPARE(client.name, QLatin1String("Name 2"));
  closeDatabase();
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  AsyncQueryTest test;

  return QTest::qExec(&test, argc, argv);
}
