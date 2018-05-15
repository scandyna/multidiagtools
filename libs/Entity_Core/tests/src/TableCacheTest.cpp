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
#include "TableCacheTest.h"
#include "Mdt/Entity/TableCache.h"
#include "Mdt/TestLib/CompareRowList.h"

using namespace Mdt::Entity;
using Mdt::Container::RowList;
using Mdt::Container::TableCacheOperation;

/*
 * Tests
 */

void TableCacheTest::constructTest()
{
  TableCache<int> tc0;
  QCOMPARE(tc0.rowCount(), 0);
  QVERIFY(tc0.isEmpty());

  TableCache<int> tc2{1,2};
  QCOMPARE(tc2.rowCount(), 2);
  QVERIFY(!tc2.isEmpty());
}

void TableCacheTest::setGetTest()
{
  TableCache<int> tc2{1,2};
  QCOMPARE(tc2.rowCount(), 2);
  QVERIFY(!tc2.isEmpty());
  QCOMPARE(tc2.constRecordAt(0), 1);
  QCOMPARE(tc2.constRecordAt(1), 2);
  QCOMPARE(tc2.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc2.operationAtRow(1), TableCacheOperation::None);
  tc2.recordAt(0) = 10;
  QCOMPARE(tc2.constRecordAt(0), 10);
  QCOMPARE(tc2.constRecordAt(1), 2);
  QCOMPARE(tc2.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(tc2.operationAtRow(1), TableCacheOperation::None);
  tc2.clear();
  QVERIFY(tc2.isEmpty());
}

void TableCacheTest::insertRemoveTest()
{
  TableCache<int> tc;
  QCOMPARE(tc.rowCount(), 0);

  tc.insertRecords(0, 1, 10, TableCacheMarking::MarkOperation);
  QCOMPARE(tc.rowCount(), 1);
  QCOMPARE(tc.constRecordAt(0), 10);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList({0}));

  tc.appendRecord(20, TableCacheMarking::MarkOperation);
  QCOMPARE(tc.rowCount(), 2);
  QCOMPARE(tc.constRecordAt(0), 10);
  QCOMPARE(tc.constRecordAt(1), 20);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList({0,1}));

  tc.insertRecords(2, 1, 30, TableCacheMarking::NotMarkOperation);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 10);
  QCOMPARE(tc.constRecordAt(1), 20);
  QCOMPARE(tc.constRecordAt(2), 30);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList({0,1}));

  tc.appendRecord(40, TableCacheMarking::NotMarkOperation);
  QCOMPARE(tc.rowCount(), 4);
  QCOMPARE(tc.constRecordAt(0), 10);
  QCOMPARE(tc.constRecordAt(1), 20);
  QCOMPARE(tc.constRecordAt(2), 30);
  QCOMPARE(tc.constRecordAt(3), 40);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList({0,1}));

  tc.removeRecords(0, 1);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 20);

  QFAIL("Not complete (miss Delete OP)");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  TableCacheTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
