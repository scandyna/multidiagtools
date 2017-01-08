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
  /*
   * Construct list
   */
  ColumnList list2({0,1,2});
  QCOMPARE(list2.size(), 3);
  QVERIFY(!list2.isEmpty());
  QVERIFY(list2.begin() != list2.end());

  QFAIL("Not complete");
}

void KeyTest::primaryKeyTest()
{
  /*
   * Construct a null primary key
   */
  PrimaryKey pk1;
  QVERIFY(pk1.isNull());
  /*
   * Construct a primary key
   */
  PrimaryKey pk2({0,3,1});
  QVERIFY(!pk2.isNull());

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
