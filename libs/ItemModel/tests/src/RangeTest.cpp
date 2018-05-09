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
#include "RangeTest.h"
#include "Mdt/Application.h"
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
 * Tests
 */

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
