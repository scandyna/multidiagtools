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
#include "Mdt/Container/TableCache.h"
#include "Mdt/TestLib/CompareRowList.h"

using namespace Mdt::Container;
using namespace Mdt::IndexRange;

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
  tc2.setRecordAt(0, 10);
  QCOMPARE(tc2.constRecordAt(0), 10);
  QCOMPARE(tc2.constRecordAt(1), 2);
  QCOMPARE(tc2.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(tc2.operationAtRow(1), TableCacheOperation::None);
  tc2.refRecordAtForUpdate(1) = 20;
  QCOMPARE(tc2.constRecordAt(0), 10);
  QCOMPARE(tc2.constRecordAt(1), 20);
  QCOMPARE(tc2.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(tc2.operationAtRow(1), TableCacheOperation::Update);
  tc2.clear();
  QVERIFY(tc2.isEmpty());
}

void TableCacheTest::refRecordAtTest()
{
  TableCache<int> tc{1,2};

  QCOMPARE(tc.rowCount(), 2);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList());

  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 2);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList());

  tc.refRecordAt(0) = 10;
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList());
  QCOMPARE(tc.constRecordAt(0), 10);
  QCOMPARE(tc.constRecordAt(1), 2);

  tc.refRecordAtForUpdate(0) = 100;
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList({0}));
  QCOMPARE(tc.constRecordAt(0), 100);
  QCOMPARE(tc.constRecordAt(1), 2);
}

void TableCacheTest::insertRecordsTest()
{
  TableCache<int> tc;
  QCOMPARE(tc.rowCount(), 0);
  QVERIFY(tc.committedRows().isNull());

  tc.insertRecords(0, 1, 10);
  QCOMPARE(tc.rowCount(), 1);
  QCOMPARE(tc.constRecordAt(0), 10);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList({0}));
  QVERIFY(tc.committedRows().isNull());

  tc.appendRecord(20);
  QCOMPARE(tc.rowCount(), 2);
  QCOMPARE(tc.constRecordAt(0), 10);
  QCOMPARE(tc.constRecordAt(1), 20);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList({0,1}));
  QVERIFY(tc.committedRows().isNull());

  tc.commitChanges();
  QCOMPARE(tc.rowCount(), 2);
  QCOMPARE(tc.constRecordAt(0), 10);
  QCOMPARE(tc.constRecordAt(1), 20);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList());
  QVERIFY(!tc.committedRows().isNull());
  QCOMPARE(tc.committedRows().firstRow(), 0);
  QCOMPARE(tc.committedRows().lastRow(), 1);
}

void TableCacheTest::insertRecordsFromStorageTest()
{
  TableCache<int> tc;
  QCOMPARE(tc.rowCount(), 0);

  tc.appendRecordFromStorage(10);
  QCOMPARE(tc.rowCount(), 1);
  QCOMPARE(tc.constRecordAt(0), 10);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
}

void TableCacheTest::removeRecordsTest()
{
  TableCache<int> tc;
  QCOMPARE(tc.rowCount(), 0);

  tc.appendRecordFromStorage(1);
  tc.appendRecordFromStorage(2);
  tc.appendRecordFromStorage(3);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 2);
  QCOMPARE(tc.constRecordAt(2), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QVERIFY(tc.committedRows().isNull());

  tc.removeRecords(1, 1);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 2);
  QCOMPARE(tc.constRecordAt(2), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::Delete);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList({1}));
  QVERIFY(tc.committedRows().isNull());

  tc.commitChanges();
  QCOMPARE(tc.rowCount(), 2);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QVERIFY(!tc.committedRows().isNull());
  QCOMPARE(tc.committedRows().firstRow(), 1);
  QCOMPARE(tc.committedRows().lastRow(), 1);
}

void TableCacheTest::removeRecordsFromStorageTest()
{

  QFAIL("Not complete");
}

void TableCacheTest::insertRemoveSetTest()
{

  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
    TableCacheTest test;

  return QTest::qExec(&test, argc, argv);
}
