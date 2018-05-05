/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "RangeTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemModel/Range.h"
#include "Mdt/ItemModel/RowRange.h"
#include "Mdt/ItemModel/ColumnRange.h"
#include "Mdt/ItemModel/VariantTableModel.h"

using namespace Mdt::ItemModel;

void RangeTest::initTestCase()
{
}

void RangeTest::cleanupTestCase()
{
}

/*
 * Range test class
 */
class RangeTestClass : public Range
{
public:
  constexpr void setFirst(int f) noexcept { Range::setFirst(f); }
  constexpr int first() const noexcept { return Range::first(); }
  constexpr void setLast(int f) noexcept { Range::setLast(f); }
  constexpr int last() const noexcept { return Range::last(); }
  constexpr void setCount(int c) noexcept { Range::setCount(c); }
  constexpr int count() const noexcept { return Range::count(); }
  constexpr int contains(int e) const noexcept { return Range::contains(e); }
};

/*
 * Tests
 */

void RangeTest::initialStateTest()
{
  RangeTestClass r;
  QCOMPARE(r.first(), -1);
  QCOMPARE(r.last(), -1);
  QCOMPARE(r.count(), 0);
  QVERIFY(!r.contains(0));
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
}

void RangeTest::setFirstLastTest()
{
  RangeTestClass r;
  /*
   * Set valid range
   */
  // Range starting from 0
  r.setFirst(0);
  r.setLast(1);
  QCOMPARE(r.first(), 0);
  QCOMPARE(r.last(), 1);
  QCOMPARE(r.count(), 2);
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
  // Range not starting from 0
  r.setFirst(10);
  r.setLast(20);
  QCOMPARE(r.first(), 10);
  QCOMPARE(r.last(), 20);
  QCOMPARE(r.count(), 11);
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
  /*
   * Clear
   */
  r.clear();
  QCOMPARE(r.first(), -1);
  QCOMPARE(r.last(), -1);
  QCOMPARE(r.count(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
  /*
   * Set invalid range
   */
  // First > last
  r.setFirst(1);
  r.setLast(0);
  QCOMPARE(r.first(), 1);
  QCOMPARE(r.last(), 0);
  QCOMPARE(r.count(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
  // Negative row
  r.setFirst(-1);
  r.setLast(0);
  QCOMPARE(r.first(), -1);
  QCOMPARE(r.last(), 0);
  QCOMPARE(r.count(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
}

void RangeTest::setFirstCountTest()
{
  RangeTestClass r;
  /*
   * Set valid range
   */
  // Range starting from 0
  r.setFirst(0);
  r.setCount(1);
  QCOMPARE(r.first(), 0);
  QCOMPARE(r.last(), 0);
  QCOMPARE(r.count(), 1);
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
  // Range not starting from 0
  r.setFirst(10);
  r.setCount(20);
  QCOMPARE(r.first(), 10);
  QCOMPARE(r.last(), 29);
  QCOMPARE(r.count(), 20);
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
  /*
   * Set empty range
   */
  r.setFirst(0);
  r.setCount(0);
  QCOMPARE(r.first(), 0);
  QCOMPARE(r.last(), -1);
  QCOMPARE(r.count(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
  /*
   * Set invalid range
   */
  // Negative row count
  r.setFirst(0);
  r.setCount(-1);
  QCOMPARE(r.first(), 0);
  QCOMPARE(r.last(), -2);
  QCOMPARE(r.count(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
  // Negative first
  r.setFirst(-1);
  r.setCount(1);
  QCOMPARE(r.first(), -1);
  QCOMPARE(r.last(), -1);
  QCOMPARE(r.count(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
}

void RangeTest::containsTest()
{
  RangeTestClass r;
  /*
   * Set valid range
   */
  // 1 element range
  r.setFirst(0);
  r.setLast(0);
  QVERIFY(r.contains(0));
  QVERIFY(!r.contains(1));
  // 2 elements range
  r.setFirst(0);
  r.setLast(1);
  QVERIFY(r.contains(0));
  QVERIFY(r.contains(1));
  QVERIFY(!r.contains(2));
  // > 2 elements range starting from > 0
  r.setFirst(10);
  r.setLast(15);
  QVERIFY(!r.contains(9));
  QVERIFY(r.contains(10));
  QVERIFY(r.contains(11));
  QVERIFY(r.contains(12));
  QVERIFY(r.contains(13));
  QVERIFY(r.contains(14));
  QVERIFY(r.contains(15));
  QVERIFY(!r.contains(16));
}

void RangeTest::setGetBenchmark()
{
  QBENCHMARK{
    RangeTestClass r;
    r.setFirst(10);
    r.setCount(20);
    QCOMPARE(r.first(), 10);
    QCOMPARE(r.last(), 29);
    QCOMPARE(r.count(), 20);
    QVERIFY(r.contains(10));
    QVERIFY(r.contains(15));
    QVERIFY(!r.isEmpty());
    QVERIFY(!r.isNull());
    QVERIFY(r.isValid());
  }
}

void RangeTest::rowInitialStateTest()
{
  RowRange r;
  QCOMPARE(r.firstRow(), -1);
  QCOMPARE(r.lastRow(), -1);
  QCOMPARE(r.rowCount(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
}

void RangeTest::rowSetFirstLastTest()
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

void RangeTest::rowSetFirstCountTest()
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

void RangeTest::rowSetFirstLastIndexTest()
{
  RowRange r;
  VariantTableModel model;
  model.resize(3, 2);
  /*
   * Set valid range
   */
  r.setFirstIndex( model.index(1, 0) );
  r.setLastIndex( model.index(2, 1) );
  QCOMPARE(r.firstRow(), 1);
  QCOMPARE(r.lastRow(), 2);
  QCOMPARE(r.rowCount(), 2);
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
}

void RangeTest::columnInitialStateTest()
{
  ColumnRange r;
  QCOMPARE(r.firstColumn(), -1);
  QCOMPARE(r.lastColumn(), -1);
  QCOMPARE(r.columnCount(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
}

void RangeTest::columnSetFirstLastTest()
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

void RangeTest::columnSetFirstCountTest()
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

void RangeTest::columnSetFirstLastIndexTest()
{
  ColumnRange r;
  VariantTableModel model;
  model.resize(3, 2);
  /*
   * Set valid range
   */
  r.setFirstIndex( model.index(1, 0) );
  r.setLastIndex( model.index(2, 1) );
  QCOMPARE(r.firstColumn(), 0);
  QCOMPARE(r.lastColumn(), 1);
  QCOMPARE(r.columnCount(), 2);
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  RangeTest test;

  return QTest::qExec(&test, argc, argv);
}
