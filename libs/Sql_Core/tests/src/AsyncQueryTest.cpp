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
#include "Mdt/Sql/AsyncQueryConnectionImpl.h"
#include "Mdt/Sql/SQLiteAsyncQueryConnection.h"
#include "Mdt/Sql/AsyncInsertQuery.h"
#include "Mdt/Sql/AsyncSelectQueryThreadWorker.h"
#include "Mdt/Sql/AsyncSelectQuery.h"
#include "Mdt/Sql/AsyncUpdateQuery.h"
#include "Mdt/Sql/AsyncDeleteQuery.h"
#include "Mdt/Container/VariantRecord.h"
#include "Mdt/Container/Vector.h"
#include "Mdt/ErrorCode.h"
#include <memory>

// #include <QDebug>

using namespace Mdt::Sql;

/*
 * We assume that SQLite is compiled thread safe
 * (Compile SQLite without threads support is optionnal: https://www.sqlite.org/threadsafe.html)
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

void AsyncQueryTest::cleanup()
{
  QVERIFY(cleanupClientTable());
}

/*
 * Helpers
 */

void setupReceiverBase(AsyncTestQueryReceiver & receiver, AsyncQueryBase & query)
{
  QObject::connect(&query, &AsyncQueryBase::doneSuccessfully, &receiver, &AsyncTestQueryReceiver::setQueryDone);
  QObject::connect(&query, &AsyncQueryBase::errorOccurred, &receiver, &AsyncTestQueryReceiver::setLastError);
}

void setupReceiver(AsyncTestQueryReceiver & receiver, AsyncInsertQuery & query)
{
  setupReceiverBase(receiver, query);
  QObject::connect(&query, &AsyncInsertQuery::newIdInserted, &receiver, &AsyncTestQueryReceiver::setLastInsertId);
}

void setupReceiver(AsyncTestQueryReceiver & receiver, AsyncSelectQuery & query)
{
  setupReceiverBase(receiver, query);
  QObject::connect(&query, &AsyncSelectQuery::newRecordAvailable, &receiver, &AsyncTestQueryReceiver::storeNewRecord);
}

void setupReceiver(AsyncTestQueryReceiver & receiver, AsyncUpdateQuery & query)
{
  setupReceiverBase(receiver, query);
}

void setupReceiver(AsyncTestQueryReceiver & receiver, AsyncDeleteQuery & query)
{
  setupReceiverBase(receiver, query);
}

class AsyncTestQuery : public AsyncQueryBase
{
 public:

  explicit AsyncTestQuery(const std::shared_ptr<AsyncQueryConnection> & connection)
   : AsyncQueryBase(connection)
  {
  }

  int instanceId() const
  {
    return AsyncQueryBase::instanceId();
  }
};

/*
 * Tests/
 */

void AsyncQueryTest::connectionIidTest()
{
  AsyncQueryConnectionImpl connection;

  QCOMPARE(connection.getNextAvailableInstanceId(), 1);
  QCOMPARE(connection.getNextAvailableInstanceId(), 2);
  QCOMPARE(connection.getNextAvailableInstanceId(), 3);

  connection.releaseInstanceId(3);
  // Used Iid: [1,2]
  QCOMPARE(connection.getNextAvailableInstanceId(), 3);
  // Used Iid: [1,2,3]

  connection.releaseInstanceId(2);
  // Used Iid: [1,3]
  QCOMPARE(connection.getNextAvailableInstanceId(), 2);
  QCOMPARE(connection.getNextAvailableInstanceId(), 4);
  // Used Iid: [1,2,3,4]

  connection.releaseInstanceId(1);
  // Used Iid: [2,3,4]
  QCOMPARE(connection.getNextAvailableInstanceId(), 1);
  QCOMPARE(connection.getNextAvailableInstanceId(), 5);
  // Used Iid: [1,2,3,4,5]

  connection.releaseInstanceId(1);
  connection.releaseInstanceId(3);
  connection.releaseInstanceId(2);
  // Used Iid: [4,5]
  QCOMPARE(connection.getNextAvailableInstanceId(), 1);
  QCOMPARE(connection.getNextAvailableInstanceId(), 2);
  QCOMPARE(connection.getNextAvailableInstanceId(), 3);
  QCOMPARE(connection.getNextAvailableInstanceId(), 6);
}

void AsyncQueryTest::queryIidTest()
{
  /*
   * Check that the destructor releases the Iid
   */

  auto connection = std::make_shared<SQLiteAsyncQueryConnection>();
  QVERIFY( connection->open(sqliteConnectionParameters()) );

  {
    AsyncTestQuery query(connection);
    QCOMPARE(query.instanceId(), 1);
  }
  {
    AsyncTestQuery query(connection);
    QCOMPARE(query.instanceId(), 1);
  }
}

void AsyncQueryTest::insertAsync()
{
  Client client;
  InsertStatement statement;
  AsyncInsertQuery query(asyncQueryConnection());
  AsyncTestQueryReceiver receiver;
  setupReceiver(receiver, query);

  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Id_PK"), 1);
  statement.addValue(FieldName("Name"), "Name 1");
  QVERIFY(!query.isSynchronous());
  query.submitStatement(statement);
  QVERIFY(!query.isSynchronous());
  // Check that newIdInserted() signal is emitted
  QTRY_VERIFY(receiver.hasLastInsertId());
  QCOMPARE(receiver.lastInsertId(), QVariant(1));

  statement.clear();
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Name"), "Name 2");
  receiver.clear();
  query.submitStatement(statement);
  // Check that done() signal is emitted
  QTRY_VERIFY(receiver.isFinished());
  QVERIFY(receiver.isQueryDone());
  client = getClient(receiver.lastInsertId().toInt());
  QCOMPARE(client.name, QLatin1String("Name 2"));
}

void AsyncQueryTest::insertSync()
{
  Client client;
  InsertStatement statement;
  AsyncInsertQuery query(asyncQueryConnection());

  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Id_PK"), 1);
  statement.addValue(FieldName("Name"), "Name 1");
  QVERIFY(!query.isSynchronous());
  QVERIFY(query.execStatement(statement));
  QVERIFY(query.isSynchronous());
  QCOMPARE(query.lastInsertId(), QVariant(1));
  client = getClient(1);
  QCOMPARE(client.name, QLatin1String("Name 1"));

  statement.clear();
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Name"), "Name 2");
  QVERIFY(query.execStatement(statement));
  client = getClient( query.lastInsertId().toInt());
  QCOMPARE(client.name, QLatin1String("Name 2"));
}

void AsyncQueryTest::insertError()
{
  Client client;
  Mdt::Expected<QVariant> id;
  InsertStatement statement;
  AsyncInsertQuery query(asyncQueryConnection());

  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Id_PK"), 1);
  statement.addValue(FieldName("Name"), "Name 1");
  QVERIFY(query.execStatement(statement));
  QCOMPARE(query.lastInsertId(), QVariant(1));

  // Unique constraint error
  QVERIFY(!query.execStatement(statement));
  QVERIFY(query.lastError().isError(Mdt::ErrorCode::UniqueConstraintError));
}

void AsyncQueryTest::insertMultipleQueries()
{
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

void AsyncQueryTest::updateAsyncTest()
{
  Client client;
  QueryField id("Id_PK");
  UpdateStatement statement;
  AsyncUpdateQuery query(asyncQueryConnection());
  AsyncTestQueryReceiver receiver;
  setupReceiver(receiver, query);

  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));

  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Name"), "Name 1 U");
  statement.setConditions(id == 1);
  query.submitStatement(statement);
  // Check that doneSuccessfully() signal is emitted
  QTRY_VERIFY(receiver.isFinished());
  QVERIFY(receiver.isQueryDone());
  QVERIFY(!receiver.hasError());
  client = getClient(1);
  QCOMPARE(client.name, QLatin1String("Name 1 U"));
  client = getClient(2);
  QCOMPARE(client.name, QLatin1String("Name 2"));
}

void AsyncQueryTest::updateSyncTest()
{
  Client client;
  QueryField id("Id_PK");
  UpdateStatement statement;
  AsyncUpdateQuery query(asyncQueryConnection());

  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));

  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Name"), "Name 1 U");
  statement.setConditions(id == 1);
  QVERIFY(query.execStatement(statement));

  client = getClient(1);
  QCOMPARE(client.name, QLatin1String("Name 1 U"));
  client = getClient(2);
  QCOMPARE(client.name, QLatin1String("Name 2"));
}

void AsyncQueryTest::updateMultipleQueriesTest()
{
  Client client;
  QueryField id("Id_PK");
  UpdateStatement statement;
  AsyncUpdateQuery query1(asyncQueryConnection());
  AsyncTestQueryReceiver receiver1;
  setupReceiver(receiver1, query1);
  AsyncUpdateQuery query2(asyncQueryConnection());
  AsyncTestQueryReceiver receiver2;
  setupReceiver(receiver2, query2);

  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));

  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Name"), "Name 1 U");
  statement.setConditions(id == 1);
  query1.submitStatement(statement);

  statement.clear();
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Name"), "Name 2 U");
  statement.setConditions(id == 2);
  query2.submitStatement(statement);

  QTRY_VERIFY(receiver1.isQueryDone());
  QTRY_VERIFY(receiver2.isQueryDone());

  client = getClient(1);
  QCOMPARE(client.name, QLatin1String("Name 1 U"));
  client = getClient(2);
  QCOMPARE(client.name, QLatin1String("Name 2 U"));
}

void AsyncQueryTest::deleteAsyncTest()
{
  QueryField id("Id_PK");
  DeleteStatement statement;
  AsyncDeleteQuery query(asyncQueryConnection());
  AsyncTestQueryReceiver receiver;
  setupReceiver(receiver, query);

  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));

  statement.setTableName("Client_tbl");
  statement.setConditions(id == 1);
  query.submitStatement(statement);
  // Check that doneSuccessfully() signal is emitted
  QTRY_VERIFY(receiver.isFinished());
  QVERIFY(receiver.isQueryDone());
  QVERIFY(!receiver.hasError());
  QVERIFY(!clientExists(1));
  QVERIFY(clientExists(2));
}

void AsyncQueryTest::deleteSyncTest()
{
  QueryField id("Id_PK");
  DeleteStatement statement;
  AsyncDeleteQuery query(asyncQueryConnection());

  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));

  statement.setTableName("Client_tbl");
  statement.setConditions(id == 1);
  QVERIFY(query.execStatement(statement));
  QVERIFY(!clientExists(1));
  QVERIFY(clientExists(2));
}

void AsyncQueryTest::deleteMultipleQueriesTest()
{
  QueryField id("Id_PK");
  DeleteStatement statement;
  AsyncDeleteQuery query1(asyncQueryConnection());
  AsyncTestQueryReceiver receiver1;
  setupReceiver(receiver1, query1);
  AsyncDeleteQuery query2(asyncQueryConnection());
  AsyncTestQueryReceiver receiver2;
  setupReceiver(receiver2, query2);

  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));
  QVERIFY(insertClient(3, "Name 3"));

  statement.setTableName("Client_tbl");

  statement.setConditions(id == 1);
  query1.submitStatement(statement);

  statement.setConditions(id == 2);
  query2.submitStatement(statement);

  QTRY_VERIFY(receiver1.isQueryDone());
  QTRY_VERIFY(receiver2.isQueryDone());
  QVERIFY(!clientExists(1));
  QVERIFY(!clientExists(2));
  QVERIFY(clientExists(3));
}

void AsyncQueryTest::selectThreadWorkerFetchAllTest()
{
  using Mdt::Container::VariantRecord;

  QVERIFY(insertClient(11, "Name 11"));
  QVERIFY(insertClient(12, "Name 12"));

  QueryField id("Id_PK");
  Mdt::QueryExpression::SelectStatement statement;
  AsyncSelectQueryThreadWorker tw(connection());
  Mdt::Container::Vector<VariantRecord> recordList;
  Mdt::Error error;

  const auto addRecordToList = [&recordList](const VariantRecord & rec, int instanceId){
    QCOMPARE(instanceId, 1);
    recordList.append(rec);
  };
  connect(&tw, &AsyncSelectQueryThreadWorker::newRecordAvailable, addRecordToList);

  const auto setError = [&error](const Mdt::Error & err, int instanceId){
    QCOMPARE(instanceId, 1);
    error = err;
  };
  connect(&tw, &AsyncSelectQueryThreadWorker::errorOccured, setError);

  statement.setEntityName("Client_tbl");
  statement.addField(id);
  statement.addField("Name");

  tw.processStatement(statement, AsyncSelectQueryRecordFetching::FetchAllRecords, 1);
  QCOMPARE(recordList.size(), 2);
  QCOMPARE(recordList.at(0).columnCount(), 2);
  QCOMPARE(recordList.at(0).value(0).toInt(), 11);
  QCOMPARE(recordList.at(0).value(1).toString(), QLatin1String("Name 11"));
  QCOMPARE(recordList.at(1).columnCount(), 2);
  QCOMPARE(recordList.at(1).value(0).toInt(), 12);
  QCOMPARE(recordList.at(1).value(1).toString(), QLatin1String("Name 12"));
  QVERIFY(error.isNull());
}

void AsyncQueryTest::selectThreadWorkerFetchSingleRecordTest()
{
  using Mdt::Container::VariantRecord;

  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));

  QueryField id("Id_PK");
  Mdt::QueryExpression::SelectStatement statement;
  AsyncSelectQueryThreadWorker tw(connection());
  VariantRecord record;
  Mdt::Error error;

  const auto setRecord = [&record](const VariantRecord & rec, int instanceId){
    QCOMPARE(instanceId, 1);
    record = rec;
  };
  connect(&tw, &AsyncSelectQueryThreadWorker::newRecordAvailable, setRecord);

  const auto setError = [&error](const Mdt::Error & err, int instanceId){
    QCOMPARE(instanceId, 1);
    error = err;
  };
  connect(&tw, &AsyncSelectQueryThreadWorker::errorOccured, setError);

  statement.setEntityName("Client_tbl");
  statement.addField(id);
  statement.addField("Name");

  statement.setFilter(id == 2);
  tw.processStatement(statement, AsyncSelectQueryRecordFetching::FetchNoRecord, 1);
  QVERIFY(record.isEmpty());
  QVERIFY(error.isNull());
  tw.processFetchSingleRecord(1);
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.value(0).toInt(), 2);
  QCOMPARE(record.value(1).toString(), QLatin1String("Name 2"));
  QVERIFY(error.isNull());

  record.clear();
  statement.setFilter(id == 15);
  tw.processStatement(statement, AsyncSelectQueryRecordFetching::FetchNoRecord, 1);
  tw.processFetchSingleRecord(1);
  QVERIFY(record.isEmpty());
  QVERIFY(error.isError(Mdt::ErrorCode::NotFound));

  record.clear();
  error.clear();
  statement.setFilter(id == 1);
  tw.processStatement(statement, AsyncSelectQueryRecordFetching::FetchSingleRecord, 1);
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.value(0).toInt(), 1);
  QCOMPARE(record.value(1).toString(), QLatin1String("Name 1"));
  QVERIFY(error.isNull());
}

void AsyncQueryTest::selectThreadWorkerFetchSingleRecordMultipleQueriesTest()
{
  using Mdt::Container::VariantRecord;

  QVERIFY(insertClient(11, "Name 11"));
  QVERIFY(insertClient(12, "Name 12"));

  QueryField id("Id_PK");
  Mdt::QueryExpression::SelectStatement statement;
  AsyncSelectQueryThreadWorker tw(connection());
  VariantRecord record1, record2;
  Mdt::Error error1, error2;

  const auto setRecord1 = [&record1](const VariantRecord & rec, int instanceId){
    if(instanceId == 1){
      record1 = rec;
    }
  };
  const auto setRecord2 = [&record2](const VariantRecord & rec, int instanceId){
    if(instanceId == 2){
      record2 = rec;
    }
  };
  connect(&tw, &AsyncSelectQueryThreadWorker::newRecordAvailable, setRecord1);
  connect(&tw, &AsyncSelectQueryThreadWorker::newRecordAvailable, setRecord2);

  const auto setError1 = [&error1](const Mdt::Error & err, int instanceId){
    if(instanceId == 1){
      error1 = err;
    }
  };
  const auto setError2 = [&error2](const Mdt::Error & err, int instanceId){
    if(instanceId == 2){
      error2 = err;
    }
  };
  connect(&tw, &AsyncSelectQueryThreadWorker::errorOccured, setError1);
  connect(&tw, &AsyncSelectQueryThreadWorker::errorOccured, setError2);

  statement.setEntityName("Client_tbl");
  statement.addField(id);
  statement.addField("Name");

  statement.setFilter(id == 11);
  tw.processStatement(statement, AsyncSelectQueryRecordFetching::FetchNoRecord, 1);
  statement.setFilter(id == 12);
  tw.processStatement(statement, AsyncSelectQueryRecordFetching::FetchNoRecord, 2);
  QVERIFY(record1.isEmpty());
  QVERIFY(record2.isEmpty());
  QVERIFY(error1.isNull());
  QVERIFY(error2.isNull());

  tw.processFetchSingleRecord(1);
  QCOMPARE(record1.columnCount(), 2);
  QCOMPARE(record1.value(0).toInt(), 11);
  QCOMPARE(record1.value(1).toString(), QLatin1String("Name 11"));
  QVERIFY(record2.isEmpty());
  QVERIFY(error1.isNull());
  QVERIFY(error2.isNull());

  tw.processFetchSingleRecord(2);
  QCOMPARE(record1.columnCount(), 2);
  QCOMPARE(record1.value(0).toInt(), 11);
  QCOMPARE(record1.value(1).toString(), QLatin1String("Name 11"));
  QCOMPARE(record2.columnCount(), 2);
  QCOMPARE(record2.value(0).toInt(), 12);
  QCOMPARE(record2.value(1).toString(), QLatin1String("Name 12"));
  QVERIFY(error1.isNull());
  QVERIFY(error2.isNull());
}

void AsyncQueryTest::selectThreadWorkerFetchNextRecordsTest()
{
  using Mdt::Container::VariantRecord;

  QVERIFY(insertClient(11, "Name 11"));
  QVERIFY(insertClient(12, "Name 12"));
  QVERIFY(insertClient(13, "Name 13"));

  QueryField id("Id_PK");
  Mdt::QueryExpression::SelectStatement statement;
  AsyncSelectQueryThreadWorker tw(connection());
  Mdt::Container::Vector<VariantRecord> recordList;
  Mdt::Error error;

  const auto addRecordToList = [&recordList](const VariantRecord & rec, int instanceId){
    QCOMPARE(instanceId, 1);
    recordList.append(rec);
  };
  connect(&tw, &AsyncSelectQueryThreadWorker::newRecordAvailable, addRecordToList);

  const auto setError = [&error](const Mdt::Error & err, int instanceId){
    QCOMPARE(instanceId, 1);
    error = err;
  };
  connect(&tw, &AsyncSelectQueryThreadWorker::errorOccured, setError);

  statement.setEntityName("Client_tbl");
  statement.addField(id);
  statement.addField("Name");

  tw.processStatement(statement, AsyncSelectQueryRecordFetching::FetchNoRecord, 1);
  QVERIFY(recordList.isEmpty());
  QVERIFY(error.isNull());

  tw.processFetchNextRecords(2, 1);
  QCOMPARE(recordList.size(), 2);

  tw.processFetchNextRecords(2, 1);
  QCOMPARE(recordList.size(), 3);

  tw.processFetchNextRecords(2, 1);
  QCOMPARE(recordList.size(), 3);

  QCOMPARE(recordList.at(0).columnCount(), 2);
  QCOMPARE(recordList.at(0).value(0).toInt(), 11);
  QCOMPARE(recordList.at(0).value(1).toString(), QLatin1String("Name 11"));
  QCOMPARE(recordList.at(1).columnCount(), 2);
  QCOMPARE(recordList.at(1).value(0).toInt(), 12);
  QCOMPARE(recordList.at(1).value(1).toString(), QLatin1String("Name 12"));
  QCOMPARE(recordList.at(2).columnCount(), 2);
  QCOMPARE(recordList.at(2).value(0).toInt(), 13);
  QCOMPARE(recordList.at(2).value(1).toString(), QLatin1String("Name 13"));
  QVERIFY(error.isNull());
}

void AsyncQueryTest::selectThreadWorkerFetchNextRecordsMultipleQueriesTest()
{
  using Mdt::Container::VariantRecord;

  QVERIFY(insertClient(11, "Name 11"));
  QVERIFY(insertClient(12, "Name 12"));

  QueryField id("Id_PK");
  Mdt::QueryExpression::SelectStatement statement;
  AsyncSelectQueryThreadWorker tw(connection());
  Mdt::Container::Vector<VariantRecord> recordList1, recordList2;
  Mdt::Error error1, error2;

  const auto addRecordToList1 = [&recordList1](const VariantRecord & rec, int instanceId){
    if(instanceId == 1){
      recordList1.append(rec);
    }
  };
  const auto addRecordToList2 = [&recordList2](const VariantRecord & rec, int instanceId){
    if(instanceId == 2){
      recordList2.append(rec);
    }
  };
  connect(&tw, &AsyncSelectQueryThreadWorker::newRecordAvailable, addRecordToList1);
  connect(&tw, &AsyncSelectQueryThreadWorker::newRecordAvailable, addRecordToList2);

  const auto setError1 = [&error1](const Mdt::Error & err, int instanceId){
    if(instanceId == 1){
      error1 = err;
    }
  };
  const auto setError2 = [&error2](const Mdt::Error & err, int instanceId){
    if(instanceId == 2){
      error2 = err;
    }
  };
  connect(&tw, &AsyncSelectQueryThreadWorker::errorOccured, setError1);
  connect(&tw, &AsyncSelectQueryThreadWorker::errorOccured, setError2);

  statement.setEntityName("Client_tbl");
  statement.addField(id);
  statement.addField("Name");

  tw.processStatement(statement, AsyncSelectQueryRecordFetching::FetchNoRecord, 1);

  statement.setFilter(id == 12);
  tw.processStatement(statement, AsyncSelectQueryRecordFetching::FetchNoRecord, 2);

  QVERIFY(recordList1.isEmpty());
  QVERIFY(recordList2.isEmpty());
  QVERIFY(error1.isNull());
  QVERIFY(error2.isNull());

  tw.processFetchNextRecords(50, 1);
  QCOMPARE(recordList1.size(), 2);
  QCOMPARE(recordList1.at(0).columnCount(), 2);
  QCOMPARE(recordList1.at(0).value(0).toInt(), 11);
  QCOMPARE(recordList1.at(0).value(1).toString(), QLatin1String("Name 11"));
  QCOMPARE(recordList1.at(1).columnCount(), 2);
  QCOMPARE(recordList1.at(1).value(0).toInt(), 12);
  QCOMPARE(recordList1.at(1).value(1).toString(), QLatin1String("Name 12"));
  QVERIFY(recordList2.isEmpty());
  QVERIFY(error1.isNull());
  QVERIFY(error2.isNull());

  tw.processFetchNextRecords(50, 2);
  QCOMPARE(recordList1.size(), 2);
  QCOMPARE(recordList2.size(), 1);
  QCOMPARE(recordList2.at(0).columnCount(), 2);
  QCOMPARE(recordList2.at(0).value(0).toInt(), 12);
  QCOMPARE(recordList2.at(0).value(1).toString(), QLatin1String("Name 12"));
  QVERIFY(error1.isNull());
  QVERIFY(error2.isNull());
}

void AsyncQueryTest::selectAsyncTest()
{
  using Mdt::QueryExpression::SelectStatement;

  QueryField id("Id_PK");
  SelectStatement statement;
  AsyncSelectQuery query(asyncQueryConnection());
  AsyncTestQueryReceiver receiver;
  setupReceiver(receiver, query);

  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));

  statement.setEntityName("Client_tbl");
  statement.addField(id);
  statement.addField("Name");

  QVERIFY(!query.isSynchronous());
  query.submitStatementAndFetchAll(statement);
  QVERIFY(!query.isSynchronous());
  // Check that doneSuccessfully() signal is emitted
  QTRY_VERIFY(receiver.isFinished());
  QVERIFY(receiver.isQueryDone());
  QVERIFY(!receiver.hasError());
  QCOMPARE(receiver.recordCount(), 2);
  QCOMPARE(receiver.recordAt(0).value(0), QVariant(1));
  QCOMPARE(receiver.recordAt(0).value(1), QVariant("Name 1"));
  QCOMPARE(receiver.recordAt(1).value(0), QVariant(2));
  QCOMPARE(receiver.recordAt(1).value(1), QVariant("Name 2"));

  receiver.clear();
  statement.setFilter(id == 1);
  query.submitStatementAndFetchAll(statement);
  QTRY_VERIFY(receiver.isFinished());
  QVERIFY(receiver.isQueryDone());
  QCOMPARE(receiver.recordCount(), 1);
  QCOMPARE(receiver.recordAt(0).value(0), QVariant(1));
  QCOMPARE(receiver.recordAt(0).value(1), QVariant("Name 1"));
}

void AsyncQueryTest::selectAsyncSingleRecordTest()
{
  using Mdt::QueryExpression::SelectStatement;

  QueryField id("Id_PK");
  SelectStatement statement;
  AsyncSelectQuery query(asyncQueryConnection());
  AsyncTestQueryReceiver receiver;
  setupReceiver(receiver, query);

  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));

  statement.setEntityName("Client_tbl");
  statement.addField(id);
  statement.addField("Name");

  statement.setFilter(id == 1);
  query.submitGetSingleRecordStatement(statement);
  QVERIFY(!query.isSynchronous());
  QTRY_VERIFY(receiver.isFinished());
  QVERIFY(receiver.isQueryDone());
  QCOMPARE(receiver.recordCount(), 1);
  QCOMPARE(receiver.recordAt(0).value(0), QVariant(1));
  QCOMPARE(receiver.recordAt(0).value(1), QVariant("Name 1"));

  receiver.clear();
  statement.setFilter(id == 5);
  query.submitGetSingleRecordStatement(statement);
  QTRY_VERIFY(receiver.isFinished());
  QVERIFY(receiver.lastError().isError(Mdt::ErrorCode::NotFound));
  QCOMPARE(receiver.recordCount(), 0);
}

void AsyncQueryTest::selectSyncTest()
{
  using Mdt::QueryExpression::SelectStatement;

  QueryField id("Id_PK");
  SelectStatement statement;
  AsyncSelectQuery query(asyncQueryConnection());

  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));

  statement.setEntityName("Client_tbl");
  statement.addField(id);
  statement.addField("Name");

  QVERIFY(!query.isSynchronous());
  QVERIFY(query.execStatement(statement));
  QVERIFY(query.isSynchronous());
  QVERIFY(query.next());
  QCOMPARE(query.fieldCount(), 2);
  QCOMPARE(query.value(0).toInt(), 1);
  QCOMPARE(query.value(1).toString(), QLatin1String("Name 1"));
  QVERIFY(query.next());
  QCOMPARE(query.fieldCount(), 2);
  QCOMPARE(query.value(0).toInt(), 2);
  QCOMPARE(query.value(1).toString(), QLatin1String("Name 2"));
  QVERIFY(!query.next());

  QVERIFY(query.execStatement(statement));
  QVERIFY(query.isSynchronous());
  QVERIFY(query.fetchRecords());
  QCOMPARE(query.fetchedRecordCount(), 2);
  QCOMPARE(query.record(0).value(1).toString(), QLatin1String("Name 1"));
  QCOMPARE(query.record(1).value(1).toString(), QLatin1String("Name 2"));
  QVERIFY(!query.fetchRecords());
  QCOMPARE(query.fetchedRecordCount(), 0);
}

void AsyncQueryTest::selectSyncSingleRecordTest()
{
  using Mdt::QueryExpression::SelectStatement;

  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));

  QueryField id("Id_PK");
  SelectStatement statement;
  AsyncSelectQuery query(asyncQueryConnection());
  Mdt::Container::VariantRecord record;

  statement.setEntityName("Client_tbl");
  statement.addField(id);
  statement.addField("Name");

  QVERIFY(query.execStatement(statement));
  QVERIFY(query.lastError().isNull());
  record = query.fetchSingleRecord();
  QVERIFY(record.isEmpty());
  QVERIFY(!query.lastError().isNull());

  statement.setFilter(id == 1);
  QVERIFY(query.execStatement(statement));
  record = query.fetchSingleRecord();
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.value(0).toInt(), 1);
  QCOMPARE(record.value(1).toString(), QLatin1String("Name 1"));

  statement.setFilter(id == 5);
  QVERIFY(query.execStatement(statement));
  record = query.fetchSingleRecord();
  QVERIFY(record.isEmpty());
  QVERIFY(query.lastError().isError(Mdt::ErrorCode::NotFound));
}

void AsyncQueryTest::selectMultipleQueriesTest()
{
  using Mdt::QueryExpression::SelectStatement;

  QueryField id("Id_PK");
  SelectStatement statement;
  AsyncSelectQuery query1(asyncQueryConnection());
  AsyncTestQueryReceiver receiver1;
  setupReceiver(receiver1, query1);
  AsyncSelectQuery query2(asyncQueryConnection());
  AsyncTestQueryReceiver receiver2;
  setupReceiver(receiver2, query2);

  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));

  statement.setEntityName("Client_tbl");
  statement.addField(id);
  statement.addField("Name");

  statement.setFilter(id == 1);
  query1.submitStatementAndFetchAll(statement);
  statement.setFilter(id == 2);
  query2.submitStatementAndFetchAll(statement);

  QTRY_VERIFY(receiver1.isFinished());
  QTRY_VERIFY(receiver2.isFinished());

  QCOMPARE(receiver1.recordCount(), 1);
  QCOMPARE(receiver1.recordAt(0).value(0), QVariant(1));
  QCOMPARE(receiver1.recordAt(0).value(1), QVariant("Name 1"));

  QCOMPARE(receiver2.recordCount(), 1);
  QCOMPARE(receiver2.recordAt(0).value(0), QVariant(2));
  QCOMPARE(receiver2.recordAt(0).value(1), QVariant("Name 2"));
}

void AsyncQueryTest::selectSyncSingleRecordMultipleQueriesTest()
{
  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));
  QVERIFY(insertClient(3, "Name 3"));

  QueryField id("Id_PK");
  Mdt::QueryExpression::SelectStatement statement;
  AsyncSelectQuery query1(asyncQueryConnection());
  AsyncSelectQuery query2(asyncQueryConnection());
  Mdt::Container::VariantRecord record;

  statement.setEntityName("Client_tbl");
  statement.addField(id);
  statement.addField("Name");

  statement.setFilter(id == 1);
  QVERIFY(query1.execStatement(statement));

  statement.setFilter(id == 2);
  QVERIFY(query2.execStatement(statement));

  record = query1.fetchSingleRecord();
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.value(0).toInt(), 1);
  QCOMPARE(record.value(1).toString(), QLatin1String("Name 1"));

  record = query2.fetchSingleRecord();
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.value(0).toInt(), 2);
  QCOMPARE(record.value(1).toString(), QLatin1String("Name 2"));
}

void AsyncQueryTest::selectSyncMultipleQueriesTest()
{
  using Mdt::QueryExpression::SelectStatement;

  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));
  QVERIFY(insertClient(3, "Name 3"));
  QVERIFY(insertClient(4, "Name 4"));
  QVERIFY(insertClient(5, "Name 5"));

  QueryField id("Id_PK");
  SelectStatement statement;
  AsyncSelectQuery query1(asyncQueryConnection());
  AsyncSelectQuery query2(asyncQueryConnection());

  statement.setEntityName("Client_tbl");
  statement.addField(id);
  statement.addField("Name");

  QVERIFY(query1.execStatement(statement));
  QVERIFY(query2.execStatement(statement));

  QVERIFY(query1.fetchRecords());
  QVERIFY(query2.fetchRecords());

  QCOMPARE(query1.fetchedRecordCount(), 5);
  QCOMPARE(query2.fetchedRecordCount(), 5);

  statement.setFilter(id == 1);
  QVERIFY(query1.execStatement(statement));
  statement.setFilter(id == 2);
  QVERIFY(query2.execStatement(statement));

  QVERIFY(query1.fetchRecords());
  QVERIFY(query2.fetchRecords());

  QCOMPARE(query1.fetchedRecordCount(), 1);
  QCOMPARE(query1.record(0).value(0).toInt(), 1);

  QCOMPARE(query2.fetchedRecordCount(), 1);
  QCOMPARE(query2.record(0).value(0).toInt(), 2);
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
