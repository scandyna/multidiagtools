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
#include "Mdt/Sql/AsyncQueryConnection.h"
#include <Mdt/Sql/SQLiteAsyncQueryConnection.h>
#include "Mdt/Sql/AsyncInsertQuery.h"
#include "Mdt/ErrorCode.h"
#include <memory>

// #include <QDebug>

using namespace Mdt::Sql;

/*
 * We assume that SQLite is compiled thread safe
 * (Compile SQLite without threads upport is optionnal: https://www.sqlite.org/threadsafe.html)
 *
 * The same database file will be accessed by 2 threads:
 *  - Main thread ( initDatabaseSqlite(), cleanupClientTable(), ... )
 *  - Separate thread ( initDatabaseSqliteAsync(), asyncQueryConnection(), ... )
 *
 * This way the tests have less code.
 * If this should cause problems,
 * the tests should be updated to open/close the 2 connections
 */

void AsyncQueryTest::initTestCase()
{
  QVERIFY(initDatabaseTemporaryFile());
  QVERIFY(initDatabaseSqlite());
  QVERIFY(createClientTable());
  QVERIFY(initDatabaseSqliteAsync());
}

void AsyncQueryTest::cleanupTestCase()
{
}

/*
 * Helpers
 */

void setupReceiver(AsyncTestQueryReceiver & receiver, AsyncInsertQuery & query)
{
  QObject::connect(&query, &AsyncInsertQuery::newIdInserted, &receiver, &AsyncTestQueryReceiver::setLastInsertId);
}

/*
 * Tests/
 */

/**
void AsyncQueryTest::connectionSetupFailTest()
{
  ConnectionParameters wrongDriverParameters("SOME_NON_AVAILABLE_DRIVER");
  AsyncQueryConnection cnn;

  QVERIFY(cnn.setupError().isNull());
  QVERIFY(!cnn.setup(wrongDriverParameters));
  QVERIFY(!cnn.setupError().isNull());
}
*/

/**
void AsyncQueryTest::simpleSelectTest()
{
  const int timeout = 1000;

  AsyncQueryConnection cnn;
  QVERIFY(cnn.setup(connectionParameters()));
  auto query = cnn.createQuery();
  AsyncTestQueryReceiver receiver;
  setupReceiver(receiver, query);

  QVERIFY(cleanupClientTable());
  QVERIFY(insertClient(1, "A"));

  query->submitQuery("SELECT Id_PK, Name FROM Client_tbl");
  QTRY_VERIFY_WITH_TIMEOUT(receiver.hasResult(), timeout);
  auto record = receiver.recordAt(0);
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.value(0), QVariant(1));
  QCOMPARE(record.value(1), QVariant("A"));

  QVERIFY(insertClient(2, "B"));
  receiver.clear();
  query->submitQuery("SELECT Id_PK, Name FROM Client_tbl");
  QTRY_COMPARE_WITH_TIMEOUT(receiver.recordCount(), 2, timeout);
  record = receiver.recordAt(0);
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.value(0), QVariant(1));
  QCOMPARE(record.value(1), QVariant("A"));
  record = receiver.recordAt(1);
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.value(0), QVariant(2));
  QCOMPARE(record.value(1), QVariant("B"));
}
*/

/**
void AsyncQueryTest::twoQueriesSelectTest()
{
  const int timeout = 1000;

  auto cnn = std::make_shared<AsyncQueryConnection>();
  QVERIFY(cnn->setup(connectionParameters()));
  auto query1 = cnn->createQuery();
  AsyncTestQueryReceiver receiver1;
  setupReceiver(receiver1, query1);
  auto query2 = cnn->createQuery();
  AsyncTestQueryReceiver receiver2;
  setupReceiver(receiver2, query2);

  QVERIFY(cleanupClientTable());
  QVERIFY(insertClient(1, "A"));
  QVERIFY(insertClient(2, "B"));

  query1->submitQuery("SELECT Id_PK, Name FROM Client_tbl WHERE Id_PK=1");
  query2->submitQuery("SELECT Id_PK, Name FROM Client_tbl WHERE Id_PK=2");

  QTRY_VERIFY_WITH_TIMEOUT(receiver1.hasResult(), timeout);
  QCOMPARE(receiver1.recordCount(), 1);
  auto record = receiver1.recordAt(0);
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.value(0), QVariant(1));
  QCOMPARE(record.value(1), QVariant("A"));

  QTRY_VERIFY_WITH_TIMEOUT(receiver2.hasResult(), timeout);
  QCOMPARE(receiver2.recordCount(), 1);
  record = receiver2.recordAt(0);
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.value(0), QVariant(2));
  QCOMPARE(record.value(1), QVariant("B"));
}
*/

void AsyncQueryTest::insertAsync()
{
  QVERIFY(cleanupClientTable());

  Client client;
  InsertStatement statement;
  AsyncInsertQuery query(asyncQueryConnection());
  AsyncTestQueryReceiver receiver;
  setupReceiver(receiver, query);

  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Id_PK"), 1);
  statement.addValue(FieldName("Name"), "Name 1");
  query.submitStatement(statement);
  QTRY_VERIFY(receiver.hasLastInsertId());
  QCOMPARE(receiver.lastInsertId(), QVariant(1));

  statement.clear();
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Name"), "Name 2");
  receiver.clear();
  query.submitStatement(statement);
  QTRY_VERIFY(receiver.hasLastInsertId());
  client = getClient(receiver.lastInsertId().toInt());
  QCOMPARE(client.name, QLatin1String("Name 2"));
}

void AsyncQueryTest::insertSync()
{
  QVERIFY(cleanupClientTable());

  Client client;
  Mdt::Expected<QVariant> id;
  InsertStatement statement;
  AsyncInsertQuery query(asyncQueryConnection());

  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Id_PK"), 1);
  statement.addValue(FieldName("Name"), "Name 1");
  id = query.execStatement(statement);
  QVERIFY(id);
  QCOMPARE(*id, QVariant(1));
  client = getClient(id->toInt());
  QCOMPARE(client.name, QLatin1String("Name 1"));

  statement.clear();
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Name"), "Name 2");
  id = query.execStatement(statement);
  QVERIFY(id);
  client = getClient(id->toInt());
  QCOMPARE(client.name, QLatin1String("Name 2"));
}

void AsyncQueryTest::insertError()
{
  QVERIFY(cleanupClientTable());

  Client client;
  Mdt::Expected<QVariant> id;
  InsertStatement statement;
  AsyncInsertQuery query(asyncQueryConnection());

  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Id_PK"), 1);
  statement.addValue(FieldName("Name"), "Name 1");
  id = query.execStatement(statement);
  QVERIFY(id);
  QCOMPARE(*id, QVariant(1));

  // Unique constraint error
  id = query.execStatement(statement);
  QVERIFY(!id);
  QVERIFY(id.error().isError(Mdt::ErrorCode::UniqueConstraintError));
}

void AsyncQueryTest::insertMultipleQueries()
{
  QVERIFY(cleanupClientTable());

  Client client;
  InsertStatement statement;
  AsyncInsertQuery query1(asyncQueryConnection());
  AsyncTestQueryReceiver receiver1;
  setupReceiver(receiver1, query1);
  AsyncInsertQuery query2(asyncQueryConnection());
  AsyncTestQueryReceiver receiver2;
  setupReceiver(receiver2, query2);

  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Id_PK"), 1);
  statement.addValue(FieldName("Name"), "Name 1");
  query1.submitStatement(statement);

  statement.clear();
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Id_PK"), 2);
  statement.addValue(FieldName("Name"), "Name 2");
  query2.submitStatement(statement);

  QTRY_VERIFY(receiver1.hasLastInsertId());
  QCOMPARE(receiver1.lastInsertId(), QVariant(1));

  QTRY_VERIFY(receiver2.hasLastInsertId());
  QCOMPARE(receiver2.lastInsertId(), QVariant(2));
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
