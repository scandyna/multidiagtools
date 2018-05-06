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
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({0}));

  map.insertRecords(2, 2);
  QCOMPARE(map.indexCount(), 3);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::Insert);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({0,2,3}));

  map.clear();
  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getRowsToInsertIntoStorage(), RowList({}));
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

  map.commitChanges();
  QCOMPARE(map.indexCount(), 5);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
  QCOMPARE(map.getFirstCommittedRow(), 0);
  QCOMPARE(map.getLastCommittedRow(), 4);

  map.clear();
  QCOMPARE(map.indexCount(), 0);
  QCOMPARE(map.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(3), TableCacheOperation::None);
  QCOMPARE(map.operationAtRow(4), TableCacheOperation::None);
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
