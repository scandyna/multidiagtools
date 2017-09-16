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
#include "ForeignKeyColumnVisibilityMapTest.h"
#include "Mdt/ItemEditor/ForeignKeyColumnVisibilityMap.h"
#include <algorithm>

using namespace Mdt::ItemModel;
using namespace Mdt::ItemEditor;

void ForeignKeyColumnVisibilityMapTest::initTestCase()
{
}

void ForeignKeyColumnVisibilityMapTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ForeignKeyColumnVisibilityMapTest::itemTest()
{
  ForeignKey fk1{1};
  ForeignKeyColumnVisibilityMapItem item1(fk1);
  QCOMPARE(item1.foreignKey().columnCount(), 1);
  QVERIFY(!item1.isForeignKeyHidden());
  item1.setForeignKeyHidden(true);
  QVERIFY(item1.isForeignKeyHidden());
}

void ForeignKeyColumnVisibilityMapTest::setForeignKeyListTest()
{
  ForeignKeyList fkList;
  /*
   * Initial state
   */
  ForeignKeyColumnVisibilityMap map;
  QVERIFY(map.getForeignKeyList().getForeignKeyReferencing("FE1").isNull());
  /*
   * Set initial list
   */
  ForeignKeyList fkList1;
  fkList1.addForeignKey("FE10", ForeignKey{1,0});
  fkList1.addForeignKey("FE12", ForeignKey{1,2});
  map.setForeignKeyList(fkList1);
  fkList = map.getForeignKeyList();
  QVERIFY(!fkList.getForeignKeyReferencing("FE10").isNull());
  QVERIFY(!fkList.getForeignKeyReferencing("FE12").isNull());
  /*
   * Set a other list
   */
  ForeignKeyList fkList2;
  fkList2.addForeignKey("FE20", ForeignKey{2,0});
  fkList2.addForeignKey("FE21", ForeignKey{2,1});
  map.setForeignKeyList(fkList2);
  fkList = map.getForeignKeyList();
  QVERIFY( fkList.getForeignKeyReferencing("FE10").isNull());
  QVERIFY( fkList.getForeignKeyReferencing("FE12").isNull());
  QVERIFY(!fkList.getForeignKeyReferencing("FE20").isNull());
  QVERIFY(!fkList.getForeignKeyReferencing("FE21").isNull());
  /*
   * Set a other list
   */
  ForeignKeyList fkList12;
  fkList12.addForeignKey("FE10", ForeignKey{1,0});
  fkList12.addForeignKey("FE20", ForeignKey{2,0});
  map.setForeignKeyList(fkList12);
  fkList = map.getForeignKeyList();
  QVERIFY(!fkList.getForeignKeyReferencing("FE10").isNull());
  QVERIFY( fkList.getForeignKeyReferencing("FE12").isNull());
  QVERIFY(!fkList.getForeignKeyReferencing("FE20").isNull());
  QVERIFY( fkList.getForeignKeyReferencing("FE21").isNull());
}

void ForeignKeyColumnVisibilityMapTest::setForeignKeyHiddenTest()
{
  ForeignKeyColumnVisibilityMap map;
  ForeignKeyList fkList1;
  /*
   * Set initial foreign key list
   */
  fkList1.addForeignKey("FE10", ForeignKey{1,0});
  fkList1.addForeignKey("FE12", ForeignKey{1,2});
  map.setForeignKeyList(fkList1);
  QVERIFY(!map.isForeignKeyHidden("FE10"));
  QVERIFY(!map.isForeignKeyHidden("FE12"));
  map.setForeignKeyHidden("FE10", true);
  QVERIFY( map.isForeignKeyHidden("FE10"));
  QVERIFY(!map.isForeignKeyHidden("FE12"));
  map.setForeignKeyHidden("FE12", true);
  QVERIFY( map.isForeignKeyHidden("FE10"));
  QVERIFY( map.isForeignKeyHidden("FE12"));
  map.setAllForeignKeysHidden(false);
  QVERIFY(!map.isForeignKeyHidden("FE10"));
  QVERIFY(!map.isForeignKeyHidden("FE12"));
  /*
   * Simulate controller adding a foreign key
   */
  // Change default flags for existing foreign keys
  map.setAllForeignKeysHidden(true);
  QVERIFY( map.isForeignKeyHidden("FE10"));
  QVERIFY( map.isForeignKeyHidden("FE12"));
  // Add foreign key
  fkList1.addForeignKey("FE3", ForeignKey{3});
  map.setForeignKeyList(fkList1);
  QVERIFY( map.isForeignKeyHidden("FE10"));
  QVERIFY( map.isForeignKeyHidden("FE12"));
  QVERIFY(!map.isForeignKeyHidden("FE3"));
}

// void ForeignKeyColumnVisibilityMapTest::setAllForeignKeysHiddenTest()
// {
//   /*
//    * Check that flag can be set before adding the foreign keys
//    */
//   ForeignKeyColumnVisibilityMap map;
//   
//   /*
//    * Change flag
//    */
//   
// 
// 
//   QFAIL("Not complete");
// }

void ForeignKeyColumnVisibilityMapTest::isColumnVisibleTest()
{
  ForeignKeyColumnVisibilityMap map;
  /*
   * Set initial list
   */
  ForeignKeyList fkList1;
  fkList1.addForeignKey("FE10", ForeignKey{1,0});
  fkList1.addForeignKey("FE12", ForeignKey{1,2});
  map.setForeignKeyList(fkList1);
  /*
   * By default, all columns are visible
   */
  QVERIFY( map.isColumnVisible(0) );
  QVERIFY( map.isColumnVisible(1) );
  QVERIFY( map.isColumnVisible(2) );
  /*
   * Change hidden for all foreign keys
   */
  map.setAllForeignKeysHidden(true);
  QVERIFY(!map.isColumnVisible(0) );
  QVERIFY(!map.isColumnVisible(1) );
  QVERIFY(!map.isColumnVisible(2) );
  map.setAllForeignKeysHidden(false);
  QVERIFY( map.isColumnVisible(0) );
  QVERIFY( map.isColumnVisible(1) );
  QVERIFY( map.isColumnVisible(2) );
  /*
   * Check conflicts
   * When a column is part of more than 1 foreign key,
   * it must be visible if at least 1 foreign key is visible.
   */
  map.setForeignKeyHidden("FE10", true);
  QVERIFY(!map.isColumnVisible(0) );
  QVERIFY( map.isColumnVisible(1) );
  QVERIFY( map.isColumnVisible(2) );
  map.setForeignKeyHidden("FE12", true);
  QVERIFY(!map.isColumnVisible(0) );
  QVERIFY(!map.isColumnVisible(1) );
  QVERIFY(!map.isColumnVisible(2) );
  map.setForeignKeyHidden("FE10", false);
  QVERIFY( map.isColumnVisible(0) );
  QVERIFY( map.isColumnVisible(1) );
  QVERIFY(!map.isColumnVisible(2) );
}

void ForeignKeyColumnVisibilityMapTest::columnsToShowAndHideTest()
{
  /*
   * Initial state
   */
  ForeignKeyColumnVisibilityMap map;
  QCOMPARE( toQList(map.columnsToShow()), QList<int>() );
  QCOMPARE( toQList(map.columnsToHide()), QList<int>() );
  /*
   * Set initial list
   */
  ForeignKeyList fkList1;
  fkList1.addForeignKey("FE10", ForeignKey{1,0});
  fkList1.addForeignKey("FE12", ForeignKey{1,2});
  map.setForeignKeyList(fkList1);
  // Check changing visibility flags for all foreign keys
  map.setAllForeignKeysHidden(true);
  QCOMPARE( toQList(map.columnsToShow()), QList<int>() );
  QCOMPARE( toSortedQList(map.columnsToHide()), QList<int>({0,1,2}) );
  map.setAllForeignKeysHidden(false);
  QCOMPARE( toSortedQList(map.columnsToShow()), QList<int>({0,1,2}) );
  QCOMPARE( toSortedQList(map.columnsToHide()), QList<int>() );
  /*
   * Check changing visibility for 1 foreign key + conflicts
   * When a column is part of more than 1 foreign key,
   * it must be visible if at least 1 foreign key is visible.
   */
  map.setForeignKeyHidden("FE10", true);
  QCOMPARE( toSortedQList(map.columnsToShow()), QList<int>() );  // All foreign keys are actually visible
  QCOMPARE( toSortedQList(map.columnsToHide()), QList<int>({0}) );
  /*
   * Set a other list
   */
  ForeignKeyList fkList2;
  fkList2.addForeignKey("FE23", ForeignKey{2,3});
  fkList2.addForeignKey("FE24", ForeignKey{2,4});
  map.setForeignKeyList(fkList2);
  map.setAllForeignKeysHidden(true);
  QCOMPARE( toSortedQList(map.columnsToShow()), QList<int>({0,1}) );
  QCOMPARE( toSortedQList(map.columnsToHide()), QList<int>({2,3,4}) );
  /*
   * After a call of columnsToShow(), list of previous columns must be cleared
   */
  map.setAllForeignKeysHidden(false);
  QCOMPARE( toSortedQList(map.columnsToShow()), QList<int>({2,3,4}) );
  QCOMPARE( toSortedQList(map.columnsToHide()), QList<int>() );
  map.setAllForeignKeysHidden(true);
  QCOMPARE( toSortedQList(map.columnsToShow()), QList<int>() );
  QCOMPARE( toSortedQList(map.columnsToHide()), QList<int>({2,3,4}) );
}

// void ForeignKeyColumnVisibilityMapTest::columnsToShowAndHideAddForeignKeyTest()
// {
//   ForeignKeyColumnVisibilityMap map;
//   QCOMPARE( toQList(map.columnsToShow()), QList<int>() );
//   QCOMPARE( toQList(map.columnsToHide()), QList<int>() );
//   /*
//    * Set initial list
//    */
//   ForeignKeyList fkList1;
//   fkList1.addForeignKey("FE10", ForeignKey{1,0});
//   map.setForeignKeyList(fkList1);
//   // Check changing visibility flags for all foreign keys
//   map.setAllForeignKeysHidden(true);
//   QCOMPARE( toSortedQList(map.columnsToShow()), QList<int>() );
//   QCOMPARE( toSortedQList(map.columnsToHide()), QList<int>({0,1}) );
//   /*
//    * Simulate controller adding a foreign key
//    */
//   fkList1.addForeignKey("FE20", ForeignKey{2,0});
//   map.setForeignKeyList(fkList1);
//   // For now, columns part FE20 are visible
//   QCOMPARE( toSortedQList(map.columnsToShow()), QList<int>() );
//   QCOMPARE( toSortedQList(map.columnsToHide()), QList<int>({1}) );
//   // Hide all foreign keys
//   map.setAllForeignKeysHidden(true);
//   QCOMPARE( toSortedQList(map.columnsToShow()), QList<int>() );
//   QCOMPARE( toSortedQList(map.columnsToHide()), QList<int>({2}) );
//   /*
//    * Simulate controller disabling foreign keys support
//    */
//   map.setForeignKeyList(ForeignKeyList());
//   QCOMPARE( toSortedQList(map.columnsToShow()), QList<int>({0,1,2}) );
//   QCOMPARE( toSortedQList(map.columnsToHide()), QList<int>() );
// }

void ForeignKeyColumnVisibilityMapTest::setForeignKeyListBenchmark()
{
  ForeignKeyColumnVisibilityMap map;
  ForeignKeyList fkList1;
  fkList1.addForeignKey("FE10", ForeignKey{1,0});
  fkList1.addForeignKey("FE12", ForeignKey{1,2});
  QBENCHMARK{
    map.setForeignKeyList(fkList1);
  }
  const auto fkList = map.getForeignKeyList();
  QVERIFY(!fkList.getForeignKeyReferencing("FE10").isNull());
  QVERIFY(!fkList.getForeignKeyReferencing("FE12").isNull());
}

/*
 * Helpers
 */

QList<int> ForeignKeyColumnVisibilityMapTest::toQList(const ColumnList& list)
{
  QList<int> retList;

  std::copy(list.begin(), list.end(), std::back_inserter(retList));

  return retList;
}

QList<int> ForeignKeyColumnVisibilityMapTest::toSortedQList(const ColumnList& list)
{
  QList<int> retList = toQList(list);

  std::sort(retList.begin(), retList.end());

  return retList;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ForeignKeyColumnVisibilityMapTest test;

  return QTest::qExec(&test, argc, argv);
}
