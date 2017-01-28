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
#include "RowRangeTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemModel/RowRange.h"

using namespace Mdt::ItemModel;

void RowRangeTest::initTestCase()
{
}

void RowRangeTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void RowRangeTest::initialStateTest()
{
  RowRange r;
  QCOMPARE(r.firstRow(), -1);
  QCOMPARE(r.lastRow(), -1);
  QCOMPARE(r.rowCount(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
}

void RowRangeTest::setFirstLastTest()
{
  RowRange r;
  /*
   * Set valid range
   */
  // Range starting from 0
  r.setFirstRow(0);
  r.setLastRow(1);
  QCOMPARE(r.firstRow(), 0);
  QCOMPARE(r.lastRow(), 1);
  QCOMPARE(r.rowCount(), 2);
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
  // Range not starting from 0
  r.setFirstRow(10);
  r.setLastRow(20);
  QCOMPARE(r.firstRow(), 10);
  QCOMPARE(r.lastRow(), 20);
  QCOMPARE(r.rowCount(), 11);
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
  /*
   * Clear
   */
  r.clear();
  QCOMPARE(r.firstRow(), -1);
  QCOMPARE(r.lastRow(), -1);
  QCOMPARE(r.rowCount(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
  /*
   * Set invalid range
   */
  // First > last
  r.setFirstRow(1);
  r.setLastRow(0);
  QCOMPARE(r.firstRow(), 1);
  QCOMPARE(r.lastRow(), 0);
  QCOMPARE(r.rowCount(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
  // Negative row
  r.setFirstRow(-1);
  r.setLastRow(0);
  QCOMPARE(r.firstRow(), -1);
  QCOMPARE(r.lastRow(), 0);
  QCOMPARE(r.rowCount(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
}

void RowRangeTest::setFirstCountTest()
{
  RowRange r;
  /*
   * Set valid range
   */
  // Range starting from 0
  r.setFirstRow(0);
  r.setRowCount(1);
  QCOMPARE(r.firstRow(), 0);
  QCOMPARE(r.lastRow(), 0);
  QCOMPARE(r.rowCount(), 1);
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
  // Range not starting from 0
  r.setFirstRow(10);
  r.setRowCount(20);
  QCOMPARE(r.firstRow(), 10);
  QCOMPARE(r.lastRow(), 29);
  QCOMPARE(r.rowCount(), 20);
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
  /*
   * Set empty range
   */
  r.setFirstRow(0);
  r.setRowCount(0);
  QCOMPARE(r.firstRow(), 0);
  QCOMPARE(r.lastRow(), -1);
  QCOMPARE(r.rowCount(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
  /*
   * Set invalid range
   */
  // Negative row count
  r.setFirstRow(0);
  r.setRowCount(-1);
  QCOMPARE(r.firstRow(), 0);
  QCOMPARE(r.lastRow(), -2);
  QCOMPARE(r.rowCount(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
  // Negative first
  r.setFirstRow(-1);
  r.setRowCount(1);
  QCOMPARE(r.firstRow(), -1);
  QCOMPARE(r.lastRow(), -1);
  QCOMPARE(r.rowCount(), 0);
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
}

void RowRangeTest::setGetBenchmark()
{
  QBENCHMARK{
    RowRange r;
    r.setFirstRow(10);
    r.setRowCount(20);
    QCOMPARE(r.firstRow(), 10);
    QCOMPARE(r.lastRow(), 29);
    QCOMPARE(r.rowCount(), 20);
    QVERIFY(!r.isEmpty());
    QVERIFY(!r.isNull());
    QVERIFY(r.isValid());
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  RowRangeTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
