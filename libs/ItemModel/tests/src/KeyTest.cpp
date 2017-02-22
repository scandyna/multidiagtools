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
#include "Mdt/ItemModel/RowList.h"
#include "Mdt/ItemModel/ColumnList.h"
#include "Mdt/ItemModel/PrimaryKey.h"
#include "Mdt/ItemModel/ForeignKey.h"
#include "Mdt/ItemModel/KeyData.h"
#include "Mdt/ItemModel/KeyRecord.h"
#include "Mdt/ItemModel/PrimaryKeyRecord.h"

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

void KeyTest::rowListTest()
{
  /*
   * Construct a empty list
   */
  RowList list1;
  QCOMPARE(list1.size(), 0);
  QVERIFY(list1.isEmpty());
  QVERIFY(list1.begin() == list1.end());
  QVERIFY(!list1.contains(0));
  QCOMPARE(list1.greatestRow(), -1);
  /*
   * Construct list
   */
  RowList list2({0,3,2});
  QCOMPARE(list2.size(), 3);
  QVERIFY(!list2.isEmpty());
  QCOMPARE(list2.at(1), 3);
  QVERIFY(list2.begin() != list2.end());
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

void KeyTest::columnListTest()
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

void KeyTest::keyDataTest()
{
  KeyData data(1, "A");
  QCOMPARE(data.column(), 1);
  QCOMPARE(data.data(), QVariant("A"));
}

void KeyTest::keyRecordTest()
{
  /*
   * Initial state
   */
  KeyRecord record;
  QCOMPARE(record.columnCount(), 0);
  QVERIFY(record.isEmpty());
  QVERIFY(record.dataForColumn(0).isNull());
  /*
   * Append
   */
  record.append(1, "A");
  QCOMPARE(record.columnCount(), 1);
  QVERIFY(!record.isEmpty());
  QCOMPARE(record.columnAt(0), 1);
  QCOMPARE(record.dataAt(0), QVariant("A"));
  QVERIFY(record.dataForColumn(0).isNull());
  QCOMPARE(record.dataForColumn(1), QVariant("A"));
  /*
   * Clear
   */
  record.clear();
  QCOMPARE(record.columnCount(), 0);
  QVERIFY(record.isEmpty());
}

void KeyTest::primaryKeyRecordTest()
{
  PrimaryKeyRecord record;
  QVERIFY(record.isEmpty());
  record.append(2, "B");
  QVERIFY(!record.isEmpty());
}

void KeyTest::foreignKeyRecordTest()
{
  QFAIL("Not complete");
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
