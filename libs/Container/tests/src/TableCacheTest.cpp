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

void TableCacheTest::setRecordAtTest()
{
  TableCache<int> tc{1,2,3};
  QCOMPARE(tc.rowCount(), 3);
  QVERIFY(!tc.isEmpty());
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 2);
  QCOMPARE(tc.constRecordAt(2), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList());

  tc.setRecordAt(0, 10);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 10);
  QCOMPARE(tc.constRecordAt(1), 2);
  QCOMPARE(tc.constRecordAt(2), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList({0}));
  QVERIFY(tc.committedRows().isNull());

  tc.recordAt(2) = 30;
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 10);
  QCOMPARE(tc.constRecordAt(1), 2);
  QCOMPARE(tc.constRecordAt(2), 30);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList({0}));
  QVERIFY(tc.committedRows().isNull());

  tc.setRecordAtUpdated(2);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 10);
  QCOMPARE(tc.constRecordAt(1), 2);
  QCOMPARE(tc.constRecordAt(2), 30);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::Update);
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList({0,2}));
  QVERIFY(tc.committedRows().isNull());

  tc.commitChanges();
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 10);
  QCOMPARE(tc.constRecordAt(1), 2);
  QCOMPARE(tc.constRecordAt(2), 30);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList());
  QVERIFY(!tc.committedRows().isNull());
  QCOMPARE(tc.committedRows().firstRow(), 0);
  QCOMPARE(tc.committedRows().lastRow(), 2);

  tc.clear();
  QCOMPARE(tc.rowCount(), 0);
  QVERIFY(tc.isEmpty());
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

  tc.insertRecords(0, 1, 100);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 100);
  QCOMPARE(tc.constRecordAt(1), 10);
  QCOMPARE(tc.constRecordAt(2), 20);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList({1,2,0}));
  QVERIFY(tc.committedRows().isNull());

  tc.commitChanges();
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 100);
  QCOMPARE(tc.constRecordAt(1), 10);
  QCOMPARE(tc.constRecordAt(2), 20);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList());
  QVERIFY(!tc.committedRows().isNull());
  QCOMPARE(tc.committedRows().firstRow(), 0);
  QCOMPARE(tc.committedRows().lastRow(), 2);
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

  tc.removeRecords(0, 1);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 2);
  QCOMPARE(tc.constRecordAt(2), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::Delete);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::Delete);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList({1,0}));
  QVERIFY(tc.committedRows().isNull());

  tc.commitChanges();
  QCOMPARE(tc.rowCount(), 1);
  QCOMPARE(tc.constRecordAt(0), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QVERIFY(tc.committedRows().isNull());
}

void TableCacheTest::removeRecordInCacheTest()
{
  TableCache<int> tc;
  QCOMPARE(tc.rowCount(), 0);

  tc.appendRecord(1);
  tc.appendRecordFromStorage(2);
  tc.appendRecord(3);
  tc.appendRecordFromStorage(4);
  QCOMPARE(tc.rowCount(), 4);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 2);
  QCOMPARE(tc.constRecordAt(2), 3);
  QCOMPARE(tc.constRecordAt(3), 4);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(tc.getRowsToDeleteInCacheOnly(), RowList());
  QVERIFY(tc.committedRows().isNull());

  tc.removeRecordInCache(3);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 2);
  QCOMPARE(tc.constRecordAt(2), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(tc.getRowsToDeleteInCacheOnly(), RowList());
  QVERIFY(tc.committedRows().isNull());

  tc.removeRecordInCache(0);
  QCOMPARE(tc.rowCount(), 2);
  QCOMPARE(tc.constRecordAt(0), 2);
  QCOMPARE(tc.constRecordAt(1), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(tc.getRowsToDeleteInCacheOnly(), RowList());
  QVERIFY(tc.committedRows().isNull());

  tc.commitChanges();
  QCOMPARE(tc.rowCount(), 2);
  QCOMPARE(tc.constRecordAt(0), 2);
  QCOMPARE(tc.constRecordAt(1), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(tc.getRowsToDeleteInCacheOnly(), RowList());
  QVERIFY(!tc.committedRows().isNull());
  QCOMPARE(tc.committedRows().firstRow(), 1);
  QCOMPARE(tc.committedRows().lastRow(), 1);
}

void TableCacheTest::insertRemoveRecordsTest()
{
  TableCache<int> tc;
  QCOMPARE(tc.rowCount(), 0);

  tc.appendRecordFromStorage(1);
  tc.appendRecord(2);
  tc.appendRecordFromStorage(3);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 2);
  QCOMPARE(tc.constRecordAt(2), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QVERIFY(tc.committedRows().isNull());

  tc.removeRecords(1, 2);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 2);
  QCOMPARE(tc.constRecordAt(2), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::InsertDelete);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::Delete);
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList({2}));
  QVERIFY(tc.committedRows().isNull());

  tc.commitChanges();
  QCOMPARE(tc.rowCount(), 1);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QVERIFY(tc.committedRows().isNull());

  tc.appendRecord(20);
  tc.appendRecord(30);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 20);
  QCOMPARE(tc.constRecordAt(2), 30);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList({1,2}));
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList());
  QCOMPARE(tc.getRowsToDeleteInCacheOnly(), RowList());
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QVERIFY(tc.committedRows().isNull());

  tc.removeRecords(1, 1);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 20);
  QCOMPARE(tc.constRecordAt(2), 30);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::InsertDelete);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList({2}));
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList());
  QCOMPARE(tc.getRowsToDeleteInCacheOnly(), RowList({1}));
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QVERIFY(tc.committedRows().isNull());

  tc.commitChanges();
  QCOMPARE(tc.rowCount(), 2);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 30);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList());
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList());
  QCOMPARE(tc.getRowsToDeleteInCacheOnly(), RowList());
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QVERIFY(!tc.committedRows().isNull());
  QCOMPARE(tc.committedRows().firstRow(), 1);
  QCOMPARE(tc.committedRows().lastRow(), 1);
}

void TableCacheTest::insertSetRemoveTest()
{
  TableCache<int> tc;
  QCOMPARE(tc.rowCount(), 0);

  tc.appendRecordFromStorage(1);
  tc.appendRecord(2);
  tc.appendRecord(3);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 2);
  QCOMPARE(tc.constRecordAt(2), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList({1,2}));
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList());
  QCOMPARE(tc.getRowsToDeleteInCacheOnly(), RowList());
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QVERIFY(tc.committedRows().isNull());

  tc.setRecordAt(1, 22);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 22);
  QCOMPARE(tc.constRecordAt(2), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList({1,2}));
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList());
  QCOMPARE(tc.getRowsToDeleteInCacheOnly(), RowList());
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QVERIFY(tc.committedRows().isNull());

  tc.removeRecords(1, 1);
  QCOMPARE(tc.rowCount(), 3);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 22);
  QCOMPARE(tc.constRecordAt(2), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::InsertDelete);
  QCOMPARE(tc.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList({2}));
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList());
  QCOMPARE(tc.getRowsToDeleteInCacheOnly(), RowList({1}));
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QVERIFY(tc.committedRows().isNull());

  tc.commitChanges();
  QCOMPARE(tc.rowCount(), 2);
  QCOMPARE(tc.constRecordAt(0), 1);
  QCOMPARE(tc.constRecordAt(1), 3);
  QCOMPARE(tc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(tc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(tc.getRowsToInsertIntoStorage(), RowList());
  QCOMPARE(tc.getRowsToUpdateInStorage(), RowList());
  QCOMPARE(tc.getRowsToDeleteInCacheOnly(), RowList());
  QCOMPARE(tc.getRowsToDeleteInStorage(), RowList());
  QVERIFY(!tc.committedRows().isNull());
  QCOMPARE(tc.committedRows().firstRow(), 1);
  QCOMPARE(tc.committedRows().lastRow(), 1);
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
