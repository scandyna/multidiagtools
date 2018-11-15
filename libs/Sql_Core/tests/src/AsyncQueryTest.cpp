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
#include "AsyncQueryTest.h"
#include "Mdt/Sql/AsyncQuery.h"
#include "Mdt/Sql/AsyncQueryConnection.h"
#include "Mdt/Sql/AsyncQueryThreadWorker.h"

// #include <QDebug>

using namespace Mdt::Sql;

void AsyncQueryTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  QVERIFY(createClientTable());
}

void AsyncQueryTest::cleanupTestCase()
{
}

/*
 * Helpers
 */

void setupReceiver(AsyncTestQueryReceiver & receiver, const std::unique_ptr<AsyncQuery> & query)
{
  QObject::connect(query.get(), &AsyncQuery::newRecordAvailable, &receiver, &AsyncTestQueryReceiver::storeNewRecord);
}

/*
 * Tests/
 */

void AsyncQueryTest::generateConnectionNameTest()
{
  QFETCH(QStringList, existingNames);
  QFETCH(QString, expectedName);

  QCOMPARE(AsyncQueryThreadWorker::generateConnectionName(existingNames), expectedName);
}

void AsyncQueryTest::generateConnectionNameTest_data()
{
  QTest::addColumn<QStringList>("existingNames");
  QTest::addColumn<QString>("expectedName");

  QTest::newRow("1") << QStringList{}
                    << "MdtAsyncQueryConnection_1";

  QTest::newRow("1") << QStringList{"a"}
                    << "MdtAsyncQueryConnection_1";

  QTest::newRow("2") << QStringList{"MdtAsyncQueryConnection_1"}
                    << "MdtAsyncQueryConnection_2";

  QTest::newRow("3") << QStringList{"MdtAsyncQueryConnection_1","MdtAsyncQueryConnection_2"}
                    << "MdtAsyncQueryConnection_3";
}

void AsyncQueryTest::connectionSetupTest()
{
  QVERIFY(!QSqlDatabase::contains("MdtAsyncQueryConnection_1"));
  QVERIFY(!QSqlDatabase::contains("MdtAsyncQueryConnection_2"));
  QVERIFY(!QSqlDatabase::contains("MdtAsyncQueryConnection_3"));

  AsyncQueryConnection cnn1;
  QVERIFY(cnn1.setup(connectionParameters()));
  QVERIFY(QSqlDatabase::contains("MdtAsyncQueryConnection_1"));
  QVERIFY(!QSqlDatabase::contains("MdtAsyncQueryConnection_2"));
  QVERIFY(!QSqlDatabase::contains("MdtAsyncQueryConnection_3"));

  AsyncQueryConnection cnn2;
  QVERIFY(cnn2.setup(connectionParameters()));
  QVERIFY(QSqlDatabase::contains("MdtAsyncQueryConnection_1"));
  QVERIFY(QSqlDatabase::contains("MdtAsyncQueryConnection_2"));
  QVERIFY(!QSqlDatabase::contains("MdtAsyncQueryConnection_3"));

  QVERIFY(cnn2.setup(connectionParameters()));
  QVERIFY(QSqlDatabase::contains("MdtAsyncQueryConnection_1"));
  QVERIFY(QSqlDatabase::contains("MdtAsyncQueryConnection_2"));
  QVERIFY(!QSqlDatabase::contains("MdtAsyncQueryConnection_3"));
}

void AsyncQueryTest::connectionSetupFailTest()
{
  ConnectionParameters wrongDriverParameters("SOME_NON_AVAILABLE_DRIVER");
  AsyncQueryConnection cnn;

  QVERIFY(cnn.setupError().isNull());
  QVERIFY(!cnn.setup(wrongDriverParameters));
  QVERIFY(!cnn.setupError().isNull());
}

void AsyncQueryTest::createQueryTest()
{
  AsyncQueryConnection cnn;
  QVERIFY(cnn.setup(connectionParameters()));

  auto query1 = cnn.createQuery();
  QCOMPARE(query1->instanceId(), 1);

  auto query2 = cnn.createQuery();
  QCOMPARE(query2->instanceId(), 2);
}

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

void AsyncQueryTest::twoQueriesSelectTest()
{
  const int timeout = 1000;

  AsyncQueryConnection cnn;
  QVERIFY(cnn.setup(connectionParameters()));
  auto query1 = cnn.createQuery();
  AsyncTestQueryReceiver receiver1;
  setupReceiver(receiver1, query1);
  auto query2 = cnn.createQuery();
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

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  AsyncQueryTest test;

  return QTest::qExec(&test, argc, argv);
}
