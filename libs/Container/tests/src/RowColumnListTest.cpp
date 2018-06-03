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
#include "RowColumnListTest.h"
#include "Mdt/Container/RowList.h"
#include "Mdt/Container/ColumnList.h"

using namespace Mdt::Container;

/*
 * Tests
 */

void RowColumnListTest::rowListTest()
{
  /*
   * Construct a empty list
   */
  RowList list1;
  QCOMPARE(list1.size(), 0);
  QVERIFY(list1.isEmpty());
  QVERIFY(list1.begin() == list1.end());
  QVERIFY(!list1.contains(0));
  QCOMPARE(list1.smallestRow(), -1);
  QCOMPARE(list1.greatestRow(), -1);
  /*
   * Construct list
   */
  RowList list2({0,3,2});
  QCOMPARE(list2.size(), 3);
  QVERIFY(!list2.isEmpty());
  QCOMPARE(list2.at(1), 3);
  QVERIFY(list2.begin() != list2.end());
  QCOMPARE(list2.smallestRow(), 0);
  QCOMPARE(list2.greatestRow(), 3);
  /*
   * Check contains
   */
  QVERIFY(list2.contains(0));
  QVERIFY(!list2.contains(1));
  QVERIFY(list2.contains(2));
  QVERIFY(list2.contains(3));
  QVERIFY(!list2.contains(4));
  /*
   * Clear
   */
  list2.clear();
  QCOMPARE(list2.size(), 0);
  QVERIFY(list2.isEmpty());
  /*
   * Append
   */
  list2.append(1);
  QCOMPARE(list2.size(), 1);
  QCOMPARE(list2.at(0), 1);
}

void RowColumnListTest::columnListTest()
{
  /*
   * Construct a empty list
   */
  ColumnList list1;
  QCOMPARE(list1.size(), 0);
  QVERIFY(list1.isEmpty());
  QVERIFY(list1.begin() == list1.end());
  QVERIFY(!list1.contains(0));
  QCOMPARE(list1.greatestColumn(), -1);
  /*
   * Construct list
   */
  ColumnList list2({0,3,2});
  QCOMPARE(list2.size(), 3);
  QVERIFY(!list2.isEmpty());
  QCOMPARE(list2.at(1), 3);
  QVERIFY(list2.begin() != list2.end());
  QCOMPARE(list2.greatestColumn(), 3);
  /*
   * Check contains
   */
  QVERIFY(list2.contains(0));
  QVERIFY(!list2.contains(1));
  QVERIFY(list2.contains(2));
  QVERIFY(list2.contains(3));
  QVERIFY(!list2.contains(4));
  /*
   * Clear
   */
  list2.clear();
  QCOMPARE(list2.size(), 0);
  QVERIFY(list2.isEmpty());
  /*
   * Append
   */
  list2.append(1);
  QCOMPARE(list2.size(), 1);
  QCOMPARE(list2.at(0), 1);
}

void RowColumnListTest::columnListQVariantTest()
{
  // Copy
  ColumnList list({1,2});
  QVariant v1;
  v1.setValue(list);
  QCOMPARE(v1.value<ColumnList>().size(), 2);
  QCOMPARE(v1.value<ColumnList>().at(0), 1);
  QCOMPARE(v1.value<ColumnList>().at(1), 2);
  // Move (maybe)
  QVariant v2;
  v2.setValue( ColumnList({3,4}) );
  QCOMPARE(v2.value<ColumnList>().size(), 2);
  QCOMPARE(v2.value<ColumnList>().at(0), 3);
  QCOMPARE(v2.value<ColumnList>().at(1), 4);
  // Copy variant
  QVariant v3 = v2;
  QCOMPARE(v3.value<ColumnList>().size(), 2);
  QCOMPARE(v3.value<ColumnList>().at(0), 3);
  QCOMPARE(v3.value<ColumnList>().at(1), 4);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
    RowColumnListTest test;

  return QTest::qExec(&test, argc, argv);
}
