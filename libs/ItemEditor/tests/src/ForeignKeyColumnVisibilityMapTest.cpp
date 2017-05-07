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

/// \brief aybe add isColumnHidden() in ForeignKeyColumnVisibilityMap

void ForeignKeyColumnVisibilityMapTest::setForeignKeyHiddenTest()
{
  QFAIL("Not complete");
}

void ForeignKeyColumnVisibilityMapTest::setAllForeignKeysHiddenTest()
{
  /*
   * Check that flag can be set before adding the foreign keys
   */
  
  /*
   * Change flag
   */
  


  QFAIL("Not complete");
}

void ForeignKeyColumnVisibilityMapTest::columnsToShowAndHideTest()
{
  /*
   * Initial state
   */
  ForeignKeyColumnVisibilityMap map;
  QCOMPARE( toQList(map.getColumnsToShow()), QList<int>() );
  QCOMPARE( toQList(map.getColumnsToHide()), QList<int>() );
  /*
   * Set initial list
   */
  ForeignKeyList fkList1;
  fkList1.addForeignKey("FE10", ForeignKey{1,0});
  fkList1.addForeignKey("FE12", ForeignKey{1,2});
  map.setForeignKeyList(fkList1);
  map.setAllForeignKeysHidden(true);
  QCOMPARE( toQList(map.getColumnsToShow()), QList<int>() );
  QCOMPARE( toSortedQList(map.getColumnsToHide()), QList<int>({0,1,2}) );
  /*
   * Set a other list
   */
  ForeignKeyList fkList2;
  fkList2.addForeignKey("FE23", ForeignKey{2,3});
  fkList2.addForeignKey("FE24", ForeignKey{2,4});
  map.setForeignKeyList(fkList2);
  map.setAllForeignKeysHidden(true);
  QCOMPARE( toSortedQList(map.getColumnsToShow()), QList<int>({0,1}) );
  QCOMPARE( toSortedQList(map.getColumnsToHide()), QList<int>({3,4}) );
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

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
