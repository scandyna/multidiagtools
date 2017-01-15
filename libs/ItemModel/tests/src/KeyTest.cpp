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
#include "KeyTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemModel/ColumnList.h"
#include "Mdt/ItemModel/PrimaryKey.h"
#include "Mdt/ItemModel/ForeignKey.h"
#include "Mdt/ItemModel/RelationKey.h"

using namespace Mdt::ItemModel;

void KeyTest::initTestCase()
{
}

void KeyTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void KeyTest::columnListTest()
{
  /*
   * Construct a empty list
   */
  ColumnList list1;
  QCOMPARE(list1.size(), 0);
  QVERIFY(list1.isEmpty());
  QVERIFY(list1.begin() == list1.end());
  QCOMPARE(list1.greatestColumn(), -1);
  /*
   * Construct list
   */
  ColumnList list2({0,3,2});
  QCOMPARE(list2.size(), 3);
  QVERIFY(!list2.isEmpty());
  QVERIFY(list2.begin() != list2.end());
  QCOMPARE(list2.greatestColumn(), 3);
  /*
   * Clear
   */
  list2.clear();
  QCOMPARE(list2.size(), 0);
  QVERIFY(list2.isEmpty());
}

void KeyTest::primaryKeyTest()
{
  /*
   * Construct a null primary key
   */
  PrimaryKey pk1;
  QCOMPARE(pk1.columnCount(), 0);
  QVERIFY(pk1.isNull());
  QCOMPARE(pk1.greatestColumn(), -1);
  QVERIFY(pk1.begin() == pk1.end());
  /*
   * Construct a primary key
   */
  PrimaryKey pk2({0,4,1});
  QCOMPARE(pk2.columnCount(), 3);
  QVERIFY(!pk2.isNull());
  QCOMPARE(pk2.greatestColumn(), 4);
  QVERIFY(pk2.begin() != pk2.end());
  /*
   * Clear
   */
  pk2.clear();
  QVERIFY(pk2.isNull());
}

void KeyTest::foreignKeyTest()
{
  /*
   * Contruct a null foreign key
   */
  ForeignKey fk1;
  QCOMPARE(fk1.columnCount(), 0);
  QVERIFY(fk1.isNull());
  QCOMPARE(fk1.greatestColumn(), -1);
  QVERIFY(fk1.begin() == fk1.end());
  /*
   * Contruct a foreign key
   */
  ForeignKey fk2({1,3});
  QCOMPARE(fk2.columnCount(), 2);
  QVERIFY(!fk2.isNull());
  QCOMPARE(fk2.greatestColumn(), 3);
  QVERIFY(fk2.begin() != fk2.end());
  /*
   * Clear
   */
  fk2.clear();
  QVERIFY(fk2.isNull());
}

void KeyTest::columnPairTest()
{
  RelationColumnPair p(1, 2);
  QCOMPARE(p.parentModelColumn(), 1);
  QCOMPARE(p.childModelColumn(), 2);
}

void KeyTest::relationKeyTest()
{
  /*
   * Initial state
   */
  RelationKey key;
  QVERIFY(key.isNull());
  QVERIFY(key.begin() == key.end());
  /*
   * Add column pair
   */
  key.addColumnPair(1, 2);
  QVERIFY(!key.isNull());
  /*
   * Iterate
   */
  for(const auto cp : key){
    QCOMPARE(cp.parentModelColumn(), 1);
    QCOMPARE(cp.childModelColumn(), 2);
  }
  /*
   * Set key
   */
  PrimaryKey pk({3,4});
  ForeignKey fk({5,6});
  key.setKey(pk, fk);
  // Check first pair
  auto it = key.begin();
  QVERIFY(it != key.end());
  QCOMPARE(it->parentModelColumn(), 3);
  QCOMPARE(it->childModelColumn(), 5);
  // Check second pair
  ++it;
  QVERIFY(it != key.end());
  QCOMPARE(it->parentModelColumn(), 4);
  QCOMPARE(it->childModelColumn(), 6);
  /*
   * Clear
   */
  key.clear();
  QVERIFY(key.isNull());
  QVERIFY(key.begin() == key.end());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  KeyTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
