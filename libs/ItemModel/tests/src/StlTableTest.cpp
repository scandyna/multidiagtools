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
#include "StlTableTest.h"
#include "Mdt/ItemModel/StlTable.h"
#include "Mdt/Container/StlContainerIteratorAdapter.h"
#include <QVariantList>
#include <QList>
#include <QStringList>
#include <QVariantList>
#include <QVector>
#include <vector>
#include <list>

using namespace Mdt::ItemModel;
using namespace Mdt::Container;

void StlTableTest::initTestCase()
{
}

void StlTableTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void StlTableTest::stateFulCompareContainerSizeStdVectorTest()
{
  using Container = std::vector<int>;
  using Adapter = StlContainerIteratorAdapter<Container>;

  Container v0;
  Container v1{1};
//   std::vector<int> v2{1,2};

  StateFulCompareContainerSize<Adapter> cmpA;
  QVERIFY(cmpA(v0));
  QVERIFY(cmpA(v0));
  QVERIFY(!cmpA(v1));
  QVERIFY(cmpA(v1));
  QVERIFY(!cmpA(v0));
  QVERIFY(!cmpA(v1));

  StateFulCompareContainerSize<Adapter> cmpB;
  QVERIFY(cmpB(v1));
  QVERIFY(cmpB(v1));
  QVERIFY(!cmpB(v0));
  QVERIFY(!cmpB(v1));
  QVERIFY(!cmpB(v0));
  QVERIFY(cmpB(v0));
}

void StlTableTest::eachRecordHasSameColumnCountStdVectorTest()
{
  using Table = std::vector< std::vector<int> >;
  using Adapter = StlContainerIteratorAdapter<Table::value_type>;

  Adapter adapter;

  Table t00;
  QVERIFY(eachRecordHasSameColumnCount(t00, adapter));
  Table t01{{},{1}};
  QVERIFY(!eachRecordHasSameColumnCount(t01, adapter));
  Table t10{{1},{}};
  QVERIFY(!eachRecordHasSameColumnCount(t10, adapter));
  Table t11{{1},{1}};
  QVERIFY(eachRecordHasSameColumnCount(t11, adapter));
}

void StlTableTest::eachRecordHasSameColumnCountQListTest()
{
  using Table = QList< QList<int> >;
  using Adapter = StlContainerIteratorAdapter<Table::value_type>;

  Adapter adapter;

  Table t00;
  QVERIFY(eachRecordHasSameColumnCount(t00, adapter));
  Table t01{{},{1}};
  QVERIFY(!eachRecordHasSameColumnCount(t01, adapter));
}

void StlTableTest::eachRecordHasSameColumnCountQVectorTest()
{
  using Table = QVector< QVector<int> >;
  using Adapter = StlContainerIteratorAdapter<Table::value_type>;

  Adapter adapter;

  Table t00;
  QVERIFY(eachRecordHasSameColumnCount(t00, adapter));
  Table t01{{},{1}};
  QVERIFY(!eachRecordHasSameColumnCount(t01, adapter));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
    StlTableTest test;

  return QTest::qExec(&test, argc, argv);
}
