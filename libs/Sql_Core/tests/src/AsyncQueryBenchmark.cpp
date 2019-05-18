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
#include "AsyncQueryBenchmark.h"
///#include "Mdt/Sql/AsyncQuery.h"
#include "Mdt/Sql/AsyncQueryConnection.h"
#include "Mdt/Sql/SelectQuery.h"
#include "Mdt/Sql/AsyncSelectQuery.h"
#include "Mdt/Sql/RecordUtils.h"
#include "Mdt/Sql/QueryExpressionSqlTransform.h"
#include "Mdt/Container/VariantRecord.h"
#include <QMetaMethod>
#include <QSqlQuery>
#include <QVector>
#include <vector>

using namespace Mdt::Sql;
using Mdt::Container::VariantRecord;

void AsyncQueryBenchmark::initTestCase()
{
//   qRegisterMetaType<Client>();

  QVERIFY(initDatabaseSqlite());
  QVERIFY(createClientTable());
  QVERIFY(initDatabaseSqliteAsync());
}

void AsyncQueryBenchmark::cleanupTestCase()
{
}

void AsyncQueryBenchmark::cleanup()
{
  QVERIFY(cleanupClientTable());
}

/*
 * Helpers
 */

// void setupReceiver(AsyncTestQueryReceiver & receiver, const std::unique_ptr<AsyncQuery> & query)
// {
//   QObject::connect(query.get(), &AsyncQuery::newRecordAvailable, &receiver, &AsyncTestQueryReceiver::storeNewRecord);
// }

/*
 * Benchmarks
 */

void AsyncQueryBenchmark::variantRecordFromSqlRecordBenchmark()
{
  using Mdt::Container::VariantRecord;

  const QSqlRecord sqlRecord = sqlRecordFromVariantRecord({"A",1,"Name A"});
  VariantRecord record;

  QBENCHMARK{
    record = variantRecordFromSqlRecord(sqlRecord);
  }

  QCOMPARE(record.columnCount(), sqlRecord.count());
}

void AsyncQueryBenchmark::isSignalConnectedBenchmark()
{
  bool isConnected = true;

  QBENCHMARK{
    static const QMetaMethod testSignalMethod = QMetaMethod::fromSignal(&AsyncQueryBenchmark::testSignal);
    isConnected = isSignalConnected(testSignalMethod);
  }

  QVERIFY(!isConnected);
}

void AsyncQueryBenchmark::selectQSqlQueryBenchmark()
{
  QFETCH(int, recordCount);
  QSqlDatabase db = database();
  const auto statement = selectAllClientStatement();
  VariantRecord record(2);
  std::vector<VariantRecord> result;
  result.reserve(recordCount);
  QSqlQuery query(db);
  query.setForwardOnly(true);

  QVERIFY(insertCountClients(recordCount));

  QBENCHMARK{
    result.clear();
    const QString sql = selectStatementToSql(statement, 0, db);
    QVERIFY(query.exec(sql));
    while(query.next()){
      record.setValue(0, query.value(0));
      record.setValue(1, query.value(1));
      result.push_back(record);
    }
  }
  QCOMPARE((int)result.size(), recordCount);
}

void AsyncQueryBenchmark::selectQSqlQueryBenchmark_data()
{
  prepareSelectBenchmarkData();
}

void AsyncQueryBenchmark::selectSqlQueryBenchmark()
{
  QFETCH(int, recordCount);
  const auto statement = selectAllClientStatement();
  VariantRecord record(2);
  std::vector<VariantRecord> result;
  result.reserve(recordCount);
  SelectQuery query(database());

  QVERIFY(insertCountClients(recordCount));

  QBENCHMARK{
    result.clear();
    QVERIFY(query.execStatement(statement));
    while(query.next()){
      record.setValue(0, query.value(0));
      record.setValue(1, query.value(1));
      result.push_back(record);
    }
  }
  QCOMPARE((int)result.size(), recordCount);
}

void AsyncQueryBenchmark::selectSqlQueryBenchmark_data()
{
  prepareSelectBenchmarkData();
}

void AsyncQueryBenchmark::selectAsyncSqlQuerySyncUsageNextBenchmark()
{
  QFETCH(int, recordCount);
  const auto statement = selectAllClientStatement();
  VariantRecord record(2);
  std::vector<VariantRecord> result;
  result.reserve(recordCount);
  AsyncSelectQuery query(asyncQueryConnection());

  QVERIFY(insertCountClients(recordCount));

  QBENCHMARK{
    result.clear();
    QVERIFY(query.execStatement(statement));
    while(query.next()){
      record.setValue(0, query.value(0));
      record.setValue(1, query.value(1));
      result.push_back(record);
    }
  }
  QCOMPARE((int)result.size(), recordCount);
}

void AsyncQueryBenchmark::selectAsyncSqlQuerySyncUsageNextBenchmark_data()
{
  prepareSelectBenchmarkData();
}

void AsyncQueryBenchmark::selectAsyncSqlQuerySyncUsageFetchRecordsBenchmark()
{
  QFETCH(int, recordCount);
  const auto statement = selectAllClientStatement();
  std::vector<VariantRecord> result;
  result.reserve(recordCount);
  AsyncSelectQuery query(asyncQueryConnection());

  QVERIFY(insertCountClients(recordCount));

  QBENCHMARK{
    result.clear();
    QVERIFY(query.execStatement(statement));
    while(query.fetchRecords()){
      const int n = query.fetchedRecordCount();
      for(int rec = 0; rec < n; ++rec){
        result.push_back( query.record(rec) );
      }
    }
  }
  QCOMPARE((int)result.size(), recordCount);
}

void AsyncQueryBenchmark::selectAsyncSqlQuerySyncUsageFetchRecordsBenchmark_data()
{
  prepareSelectBenchmarkData();
}

void AsyncQueryBenchmark::prepareSelectBenchmarkData()
{
  QTest::addColumn<int>("recordCount");

  QTest::newRow("1") << 1;
  QTest::newRow("10") << 10;
  QTest::newRow("100") << 100;
}

bool AsyncQueryBenchmark::insertCountClients(int n)
{
  Q_ASSERT(n > 0);

  QSqlDatabase db = database();

  db.transaction();
  for(int i = 1; i <= n; ++i){
    const QString name = QLatin1String("Name ") + QString::number(i);
    if(!insertClient(i, name)){
      return false;
    }
  }
  db.commit();

  return true;
}

Mdt::QueryExpression::SelectStatement AsyncQueryBenchmark::selectAllClientStatement() const
{
  Mdt::QueryExpression::SelectStatement statement;

  statement.setEntityName("Client_tbl");
  statement.addField("Id_PK");
  statement.addField("Name");

  return statement;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  AsyncQueryBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
