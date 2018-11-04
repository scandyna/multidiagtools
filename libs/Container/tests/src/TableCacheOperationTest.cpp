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
#include "TableCacheOperationTest.h"
#include "Mdt/Container/TableCacheOperationIndex.h"
#include "Mdt/Container/TableCacheOperationMap.h"
#include "Mdt/TestLib/CompareRowList.h"

using namespace Mdt::Container;

/*
 * Tests
 */

void TableCacheOperationTest::indexTest()
{
  TableCacheOperationIndex indexNull;
  QCOMPARE(indexNull.row(), -1);
  QCOMPARE(indexNull.column(), -1);
  QVERIFY(indexNull.isNull());
  QCOMPARE(indexNull.operation(), TableCacheOperation::None);

  TableCacheOperationIndex index0(0, TableCacheOperation::Insert);
  QCOMPARE(index0.row(), 0);
  QCOMPARE(index0.column(), -1);
  QVERIFY(!index0.isNull());
  QCOMPARE(index0.operation(), TableCacheOperation::Insert);

  TableCacheOperationIndex index10(1, 0, TableCacheOperation::Insert);
  QCOMPARE(index10.row(), 1);
  QCOMPARE(index10.column(), 0);
  QVERIFY(!index10.isNull());
  QCOMPARE(index10.operation(), TableCacheOperation::Insert);
}

void TableCacheOperationTest::operationFromExistingTest()
{
  QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::None, TableCacheOperation::None), TableCacheOperation::None);
  QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::None, TableCacheOperation::Insert), TableCacheOperation::Insert);
  QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::None, TableCacheOperation::Update), TableCacheOperation::Update);
  QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::None, TableCacheOperation::Delete), TableCacheOperation::Delete);
  ///QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::None, TableCacheOperation::InsertDelete), TableCacheOperation::Delete);
  ///QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Insert, TableCacheOperation::None), TableCacheOperation::None);
  QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Insert, TableCacheOperation::Insert), TableCacheOperation::Insert);
  QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Insert, TableCacheOperation::Update), TableCacheOperation::Insert);
  QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Insert, TableCacheOperation::Delete), TableCacheOperation::InsertDelete);
  ///QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Insert, TableCacheOperation::InsertDelete), TableCacheOperation::Delete);
  ///QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Update, TableCacheOperation::None), TableCacheOperation::None);
  ///QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Update, TableCacheOperation::Insert), TableCacheOperation::None);
  QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Update, TableCacheOperation::Update), TableCacheOperation::Update);
  QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Update, TableCacheOperation::Delete), TableCacheOperation::Delete);
  ///QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Update, TableCacheOperation::InsertDelete), TableCacheOperation::Delete);
  ///QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Delete, TableCacheOperation::None), TableCacheOperation::None);
  QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Delete, TableCacheOperation::Insert), TableCacheOperation::Delete);
  QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Delete, TableCacheOperation::Update), TableCacheOperation::Update);
  QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Delete, TableCacheOperation::Delete), TableCacheOperation::Delete);
  ///QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::Delete, TableCacheOperation::InsertDelete), TableCacheOperation::Delete);
  ///QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::InsertDelete, TableCacheOperation::None), TableCacheOperation::None);
  ///QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::InsertDelete, TableCacheOperation::Insert), TableCacheOperation::None);
  ///QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::InsertDelete, TableCacheOperation::Update), TableCacheOperation::None);
  ///QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::InsertDelete, TableCacheOperation::Delete), TableCacheOperation::None);
  ///QCOMPARE(TableCacheOperationMap::operationFromExisting(TableCacheOperation::InsertDelete, TableCacheOperation::InsertDelete), TableCacheOperation::None);

  QFAIL("Not complete");
}

void TableCacheOperationTest::insertRecordsTest()
{
  TableCacheOperationMap map;
  QCOMPARE(map.indexCount(), 0);
  QVERIFY(map.isEmpty());
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({}));

  map.insertRecords(0, 1);
  QCOMPARE(map.indexCount(), 1);
  QVERIFY(!map.isEmpty());
  QCOMPARE(map.indexAt(0).row(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({0}));

  map.insertRecords(2, 2);
  QCOMPARE(map.indexCount(), 3);
  QCOMPARE(map.indexAt(0).row(), 0);
  QCOMPARE(map.indexAt(1).row(), 2);
  QCOMPARE(map.indexAt(2).row(), 3);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({0,2,3}));

  map.insertRecords(0, 1);
  QCOMPARE(map.indexCount(), 4);
  QCOMPARE(map.indexAt(0).row(), 1);
  QCOMPARE(map.indexAt(1).row(), 3);
  QCOMPARE(map.indexAt(2).row(), 4);
  QCOMPARE(map.indexAt(3).row(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::Insert);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({1,3,4,0}));

  map.clear();
  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({}));

  map.insertRecords(0, 1);
  QCOMPARE(map.indexCount(), 1);
  QCOMPARE(map.indexAt(0).row(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({0}));

  map.insertRecords(0, 2);
  QCOMPARE(map.indexCount(), 3);
  QCOMPARE(map.indexAt(0).row(), 2);
  QCOMPARE(map.indexAt(1).row(), 0);
  QCOMPARE(map.indexAt(2).row(), 1);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({2,0,1}));
}

void TableCacheOperationTest::insertCommitChangesTest()
{
  TableCacheOperationMap map;

  map.insertRecords(0, 5);
  QCOMPARE(map.indexCount(), 5);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::Insert);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({0,1,2,3,4}));
  QVERIFY(map.committedRows().isNull());

  map.commitChanges();
  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QVERIFY(!map.committedRows().isNull());
  QCOMPARE(map.committedRows().firstRow(), 0);
  QCOMPARE(map.committedRows().lastRow(), 4);
//   QCOMPARE(map.getFirstCommittedRow(), 0);
//   QCOMPARE(map.getLastCommittedRow(), 4);

  map.clear();
  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QVERIFY(map.committedRows().isNull());
}

void TableCacheOperationTest::setOperationAtRowTest()
{
  TableCacheOperationMap map;

  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);

  map.setOperationAtRow(1, TableCacheOperation::Update);
  QCOMPARE(map.indexCount(), 1);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Update);

  map.setOperationAtRow(0, TableCacheOperation::Delete);
  QCOMPARE(map.indexCount(), 2);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Delete);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Update);

  map.setOperationAtRow(0, TableCacheOperation::Update);
  QCOMPARE(map.indexCount(), 2);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Update);
}

void TableCacheOperationTest::removeOperationAtRowTest()
{
  TableCacheOperationMap map;

  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);

  map.setOperationAtRow(0, TableCacheOperation::Update);
  map.setOperationAtRow(1, TableCacheOperation::Delete);
  map.setOperationAtRow(2, TableCacheOperation::Insert);
  QCOMPARE(map.indexCount(), 3);
  QCOMPARE(map.indexAt(0).row(), 0);
  QCOMPARE(map.indexAt(1).row(), 1);
  QCOMPARE(map.indexAt(2).row(), 2);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Delete);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Insert);

  map.removeOperationAtRow(2);
  QCOMPARE(map.indexCount(), 2);
  QCOMPARE(map.indexAt(0).row(), 0);
  QCOMPARE(map.indexAt(1).row(), 1);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Delete);

  map.removeOperationAtRow(0);
  QCOMPARE(map.indexCount(), 1);
  QCOMPARE(map.indexAt(0).row(), 1);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Delete);
}

void TableCacheOperationTest::shiftRowsInMapTest()
{
  TableCacheOperationMap map;
  RowList rowList;

  map.setOperationAtRow(0, TableCacheOperation::Update);
  map.setOperationAtRow(1, TableCacheOperation::Delete);
  map.setOperationAtRow(2, TableCacheOperation::Insert);
  QCOMPARE(map.indexCount(), 3);
  QCOMPARE(map.indexAt(0).row(), 0);
  QCOMPARE(map.indexAt(1).row(), 1);
  QCOMPARE(map.indexAt(2).row(), 2);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Delete);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Insert);

  rowList = map.shiftRowsInMap(1, 2);
  QCOMPARE(rowList, RowList({3,4}));
  QCOMPARE(map.indexCount(), 3);
  QCOMPARE(map.indexAt(0).row(), 0);
  QCOMPARE(map.indexAt(1).row(), 3);
  QCOMPARE(map.indexAt(2).row(), 4);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::Delete);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::Insert);

  rowList = map.shiftRowsInMap(3, -1);
  QCOMPARE(rowList, RowList({2,3}));
  QCOMPARE(map.indexCount(), 3);
  QCOMPARE(map.indexAt(0).row(), 0);
  QCOMPARE(map.indexAt(1).row(), 2);
  QCOMPARE(map.indexAt(2).row(), 3);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Delete);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
}

void TableCacheOperationTest::getRowsToUpdateInStorageTest()
{
  TableCacheOperationMap map;

  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.getRowsToUpdateInStorage(), RowList());

  map.setOperationAtRow(0, TableCacheOperation::Update);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.getRowsToUpdateInStorage(), RowList({0}));

  map.setOperationAtRow(2, TableCacheOperation::Update);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Update);
  QCOMPARE(map.getRowsToUpdateInStorage(), RowList({0,2}));
}

void TableCacheOperationTest::removeRecordsTest()
{
  TableCacheOperationMap map;

  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInCache(), RowList());

  map.removeRecords(1, 2);
  QCOMPARE(map.indexCount(), 2);
  QCOMPARE(map.indexAt(0).row(), 1);
  QCOMPARE(map.indexAt(1).row(), 2);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Delete);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Delete);
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList({2,1}));
  QCOMPARE(map.getRowsToDeleteInCache(), RowList({2,1}));
  QVERIFY(map.committedRows().isNull());

  map.commitChanges();
  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInCache(), RowList());
  QVERIFY(map.committedRows().isNull());

  map.removeRecords(1, 1);
  QCOMPARE(map.indexCount(), 1);
  QCOMPARE(map.indexAt(0).row(), 1);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Delete);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList({1}));
  QCOMPARE(map.getRowsToDeleteInCache(), RowList({1}));
  QVERIFY(map.committedRows().isNull());

  map.removeRecords(3, 2);
  QCOMPARE(map.indexCount(), 3);
  QCOMPARE(map.indexAt(0).row(), 1);
  QCOMPARE(map.indexAt(1).row(), 3);
  QCOMPARE(map.indexAt(2).row(), 4);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Delete);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::Delete);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::Delete);
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList({4,3,1}));
  QCOMPARE(map.getRowsToDeleteInCache(), RowList({4,3,1}));
  QVERIFY(map.committedRows().isNull());

  map.removeRecords(0, 1);
  QCOMPARE(map.indexCount(), 4);
  QCOMPARE(map.indexAt(0).row(), 1);
  QCOMPARE(map.indexAt(1).row(), 3);
  QCOMPARE(map.indexAt(2).row(), 4);
  QCOMPARE(map.indexAt(3).row(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Delete);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Delete);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::Delete);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::Delete);
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList({4,3,1,0}));
  QCOMPARE(map.getRowsToDeleteInCache(), RowList({4,3,1,0}));
  QVERIFY(map.committedRows().isNull());
}

void TableCacheOperationTest::cancelRemoveRecordsTest()
{
  TableCacheOperationMap map;

  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInCache(), RowList());

  map.removeRecords(1, 2);
  QCOMPARE(map.indexCount(), 2);
  QCOMPARE(map.indexAt(0).row(), 1);
  QCOMPARE(map.indexAt(1).row(), 2);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Delete);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Delete);
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList({2,1}));
  QCOMPARE(map.getRowsToDeleteInCache(), RowList({2,1}));
  QVERIFY(map.committedRows().isNull());

  map.cancelRemoveRecords(1, 1);
  QCOMPARE(map.indexCount(), 1);
  QCOMPARE(map.indexAt(0).row(), 2);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Delete);
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList({2}));
  QCOMPARE(map.getRowsToDeleteInCache(), RowList({2}));
  QVERIFY(map.committedRows().isNull());

  map.insertRecords(0, 1);
  QCOMPARE(map.indexCount(), 2);
  QCOMPARE(map.indexAt(0).row(), 3);
  QCOMPARE(map.indexAt(1).row(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::Delete);
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList({3}));
  QCOMPARE(map.getRowsToDeleteInCache(), RowList({3}));
  QVERIFY(map.committedRows().isNull());

  map.cancelRemoveRecords(0, 1);
  QCOMPARE(map.indexCount(), 2);
  QCOMPARE(map.indexAt(0).row(), 3);
  QCOMPARE(map.indexAt(1).row(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::Delete);
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList({3}));
  QCOMPARE(map.getRowsToDeleteInCache(), RowList({3}));
  QVERIFY(map.committedRows().isNull());

  map.commitChanges();
  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInCache(), RowList());
  QVERIFY(!map.committedRows().isNull());
  QCOMPARE(map.committedRows().firstRow(), 0);
  QCOMPARE(map.committedRows().lastRow(), 0);
}

void TableCacheOperationTest::insertRemoveRecordsTest()
{
  TableCacheOperationMap map;

  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInCacheOnly(), RowList());
  QCOMPARE(map.getRowsToDeleteInCache(), RowList());
  QVERIFY(map.committedRows().isNull());

  map.insertRecords(1, 3);
  QCOMPARE(map.indexCount(), 3);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({1,2,3}));
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInCacheOnly(), RowList());
  QCOMPARE(map.getRowsToDeleteInCache(), RowList());
  QVERIFY(map.committedRows().isNull());

  map.removeRecords(2, 3);
  QCOMPARE(map.indexCount(), 4);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::InsertDelete);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::InsertDelete);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::Delete);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({1}));
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList({4}));
  QCOMPARE(map.getRowsToDeleteInCacheOnly(), RowList({3,2}));
  QCOMPARE(map.getRowsToDeleteInCache(), RowList({4,3,2}));
  QVERIFY(map.committedRows().isNull());

  map.commitChanges();
  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInCacheOnly(), RowList());
  QCOMPARE(map.getRowsToDeleteInCache(), RowList());
  QVERIFY(!map.committedRows().isNull());
  QCOMPARE(map.committedRows().firstRow(), 1);
  QCOMPARE(map.committedRows().lastRow(), 1);
}

void TableCacheOperationTest::insertSetRemoveTest()
{
  TableCacheOperationMap map;
  QCOMPARE(map.indexCount(), 0);

  map.insertRecords(0, 3);
  QCOMPARE(map.indexCount(), 3);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({0,1,2}));
  QCOMPARE(map.getRowsToUpdateInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInCacheOnly(), RowList());
//   QCOMPARE(map.getRowsToDeleteInCache(), RowList());
  QVERIFY(map.committedRows().isNull());

  map.setOperationAtRow(1, TableCacheOperation::Update);
  QCOMPARE(map.indexCount(), 3);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({0,1,2}));
  QCOMPARE(map.getRowsToUpdateInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInCacheOnly(), RowList());
  QVERIFY(map.committedRows().isNull());

  map.removeRecords(1, 1);
  QCOMPARE(map.indexCount(), 3);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::InsertDelete);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({0,2}));
  QCOMPARE(map.getRowsToUpdateInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInCacheOnly(), RowList({1}));
  QVERIFY(map.committedRows().isNull());

  map.removeOperationAtRow(1);
  map.shiftRowsInMap(1, -1);
  map.commitChanges();
  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList());
  QCOMPARE(map.getRowsToUpdateInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInStorage(), RowList());
  QCOMPARE(map.getRowsToDeleteInCacheOnly(), RowList());
  QVERIFY(!map.committedRows().isNull());
  QCOMPARE(map.committedRows().firstRow(), 0);
  QCOMPARE(map.committedRows().lastRow(), 1);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
    TableCacheOperationTest test;

  return QTest::qExec(&test, argc, argv);
}
