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
#include "RowColumnRangeTest.h"
#include "Mdt/IndexRange/RowRange.h"
#include "Mdt/IndexRange/ColumnRange.h"

using namespace Mdt::IndexRange;

/*
 * Tests
 */

void RowColumnRangeTest::rowInitialStateTest()
{
  RowRange r;
  QCOMPARE(r.firstRow(), 0);
  QCOMPARE(r.lastRow(), 0);
  QCOMPARE(r.rowCount(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
}

void RowColumnRangeTest::rowSetFirstLastTest()
{
  RowRange r;
  /*
   * Set valid range
   */
  r.setFirstRow(10);
  r.setLastRow(20);
  QCOMPARE(r.firstRow(), 10);
  QCOMPARE(r.lastRow(), 20);
  QCOMPARE(r.rowCount(), 11);
  QVERIFY(r.containsRow(12));
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
}

void RowColumnRangeTest::rowSetFirstCountTest()
{
  RowRange r;
  /*
   * Set valid range
   */
  r.setFirstRow(10);
  r.setRowCount(20);
  QCOMPARE(r.firstRow(), 10);
  QCOMPARE(r.lastRow(), 29);
  QCOMPARE(r.rowCount(), 20);
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
}

void RowColumnRangeTest::rowClearTest()
{
  RowRange r;

  QVERIFY(r.isNull());
  r.setFirstRow(0);
  r.setRowCount(1);
  QVERIFY(!r.isNull());
  r.clear();
  QVERIFY(r.isNull());
}

void RowColumnRangeTest::columnInitialStateTest()
{
  ColumnRange r;
  QCOMPARE(r.firstColumn(), 0);
  QCOMPARE(r.lastColumn(), 0);
  QCOMPARE(r.columnCount(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
}

void RowColumnRangeTest::columnSetFirstLastTest()
{
  ColumnRange r;
  /*
   * Set valid range
   */
  r.setFirstColumn(10);
  r.setLastColumn(20);
  QCOMPARE(r.firstColumn(), 10);
  QCOMPARE(r.lastColumn(), 20);
  QCOMPARE(r.columnCount(), 11);
  QVERIFY(r.containsColumn(12));
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
}

void RowColumnRangeTest::columnSetFirstCountTest()
{
  ColumnRange r;
  /*
   * Set valid range
   */
  r.setFirstColumn(10);
  r.setColumnCount(20);
  QCOMPARE(r.firstColumn(), 10);
  QCOMPARE(r.lastColumn(), 29);
  QCOMPARE(r.columnCount(), 20);
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
}

void RowColumnRangeTest::columnClearTest()
{
  ColumnRange r;

  QVERIFY(r.isNull());
  r.setFirstColumn(0);
  r.setColumnCount(1);
  QVERIFY(!r.isNull());
  r.clear();
  QVERIFY(r.isNull());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  RowColumnRangeTest test;

  return QTest::qExec(&test, argc, argv);
}
