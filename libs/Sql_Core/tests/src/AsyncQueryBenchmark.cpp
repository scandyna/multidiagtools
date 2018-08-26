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
#include "AsyncQueryBenchmark.h"
#include "Mdt/Sql/AsyncQuery.h"
#include "Mdt/Sql/AsyncQueryConnection.h"
#include "Mdt/Container/VariantRecord.h"
#include <QSqlQuery>
#include <vector>

using namespace Mdt::Sql;
using Mdt::Container::VariantRecord;

void AsyncQueryBenchmark::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  QVERIFY(createClientTable());
}

void AsyncQueryBenchmark::cleanupTestCase()
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
 * Benchmarks
 */

void AsyncQueryBenchmark::selectQuery()
{
  QVERIFY(cleanupClientTable());
  QVERIFY(insertClient(1, "A"));
  QVERIFY(insertClient(2, "B"));

  VariantRecord record(2);
  std::vector<VariantRecord> result;
  QSqlQuery query(database());

  QBENCHMARK{
    result.clear();
    QVERIFY(query.exec("SELECT Id_PK, Name FROM Client_tbl"));
    while(query.next()){
      record.setValue(0, query.value(0));
      record.setValue(1, query.value(1));
      result.push_back(record);
    }
  }
  QCOMPARE(result.size(), 2ul);
  QCOMPARE(result[0].value(0), QVariant(1));
  QCOMPARE(result[0].value(1), QVariant("A"));
  QCOMPARE(result[1].value(0), QVariant(2));
  QCOMPARE(result[1].value(1), QVariant("B"));
}

void AsyncQueryBenchmark::asyncSelectQuery()
{
  QVERIFY(cleanupClientTable());
  QVERIFY(insertClient(1, "A"));
  QVERIFY(insertClient(2, "B"));

  AsyncQueryConnection cnn;
  QVERIFY(cnn.setup(database().databaseName()));

  AsyncTestQueryReceiver receiver;
  auto query = cnn.createQuery();
  setupReceiver(receiver, query);
  QBENCHMARK{
    receiver.clear();
    query->submitQuery("SELECT Id_PK, Name FROM Client_tbl");
    while(receiver.recordCount() < 2)
    {
      QCoreApplication::processEvents();
    }
  }
  auto record = receiver.recordAt(0);
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.value(0), QVariant(1));
  QCOMPARE(record.value(1), QVariant("A"));
  record = receiver.recordAt(1);
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
  AsyncQueryBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
