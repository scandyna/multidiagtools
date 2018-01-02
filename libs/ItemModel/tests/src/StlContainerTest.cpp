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
#include "StlContainerTest.h"
#include "Mdt/ItemModel/StlContainer.h"
#include <QVariantList>
#include <QList>
#include <QStringList>
#include <QVariantList>
#include <QVector>
#include <vector>
#include <list>

using namespace Mdt::ItemModel;

void StlContainerTest::initTestCase()
{
}

void StlContainerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void StlContainerTest::sizeStdVectorTest()
{
  std::vector<int> v0;
  QCOMPARE(containerSize(v0), 0);
  std::vector<int> v1{1};
  QCOMPARE(containerSize(v1), 1);
  std::vector<int> v2{1,2,3,4,5};
  QCOMPARE(containerSize(v2), 5);
}

void StlContainerTest::sizeStdListTest()
{
  std::list<int> l1{1};
  QCOMPARE(containerSize(l1), 1);
}

void StlContainerTest::sizeQListTest()
{
  QList<int> l1{1};
  QCOMPARE(containerSize(l1), 1);
  QStringList sl{"A","B"};
  QCOMPARE(containerSize(sl), 2);
  QVariantList vl{1, "A",3.0};
  QCOMPARE(containerSize(vl), 3);
}

void StlContainerTest::sizeQVectorTest()
{
  QVector<int> v1{1};
  QCOMPARE(containerSize(v1), 1);
}

void StlContainerTest::stateFulCompareContainerSizeStdVectorTest()
{
  std::vector<int> v0;
  std::vector<int> v1{1};
//   std::vector<int> v2{1,2};

  StateFulCompareContainerSize cmpA;
  QVERIFY(cmpA(v0));
  QVERIFY(cmpA(v0));
  QVERIFY(!cmpA(v1));
  QVERIFY(cmpA(v1));
  QVERIFY(!cmpA(v0));
  QVERIFY(!cmpA(v1));

  StateFulCompareContainerSize cmpB;
  QVERIFY(cmpB(v1));
  QVERIFY(cmpB(v1));
  QVERIFY(!cmpB(v0));
  QVERIFY(!cmpB(v1));
  QVERIFY(!cmpB(v0));
  QVERIFY(cmpB(v0));
}

void StlContainerTest::eachRecordHasSameColumnCountStdVectorTest()
{
  using Table = std::vector< std::vector<int> >;

  Table t00;
  QVERIFY(eachRecordHasSameColumnCount(t00));
  Table t01{{},{1}};
  QVERIFY(!eachRecordHasSameColumnCount(t01));
  Table t10{{1},{}};
  QVERIFY(!eachRecordHasSameColumnCount(t10));
  Table t11{{1},{1}};
  QVERIFY(eachRecordHasSameColumnCount(t11));
}

void StlContainerTest::eachRecordHasSameColumnCountQListTest()
{
  using Table = QList< QList<int> >;

  Table t00;
  QVERIFY(eachRecordHasSameColumnCount(t00));
  Table t01{{},{1}};
  QVERIFY(!eachRecordHasSameColumnCount(t01));
}

void StlContainerTest::eachRecordHasSameColumnCountQVectorTest()
{
  using Table = QVector< QVector<int> >;

  Table t00;
  QVERIFY(eachRecordHasSameColumnCount(t00));
  Table t01{{},{1}};
  QVERIFY(!eachRecordHasSameColumnCount(t01));
}

void StlContainerTest::constIteratorAtIndexStdVectorTest()
{
  std::vector<int> v{1,2,3};

  QCOMPARE(*constIteratorAtIndex(v, 0), 1);
  QCOMPARE(*constIteratorAtIndex(v, 1), 2);
  QCOMPARE(*constIteratorAtIndex(v, 2), 3);
}

void StlContainerTest::iteratorAtIndexStdVectorTest()
{
  std::vector<int> v{0,0};

  QCOMPARE(*iteratorAtIndex(v, 0), 0);
  QCOMPARE(*iteratorAtIndex(v, 1), 0);
  *iteratorAtIndex(v, 0) = 10;
  *iteratorAtIndex(v, 1) = 20;
  QCOMPARE(*iteratorAtIndex(v, 0), 10);
  QCOMPARE(*iteratorAtIndex(v, 1), 20);
}

void StlContainerTest::constIteratorAtRowColumnStdVectorTest()
{
  using Table = std::vector< std::vector<int> >;

  Table t32{{10,11},{20,21},{30,31}};
  QCOMPARE(*constIteratorAtRowColumn(t32, 0, 0), 10);
  QCOMPARE(*constIteratorAtRowColumn(t32, 0, 1), 11);
  QCOMPARE(*constIteratorAtRowColumn(t32, 1, 0), 20);
  QCOMPARE(*constIteratorAtRowColumn(t32, 1, 1), 21);
  QCOMPARE(*constIteratorAtRowColumn(t32, 2, 0), 30);
  QCOMPARE(*constIteratorAtRowColumn(t32, 2, 1), 31);
}

void StlContainerTest::iteratorAtRowColumnStdVectorTest()
{
  using Table = std::vector< std::vector<int> >;

  Table t32{{0,0},{0,0},{0,0}};
  QCOMPARE(*iteratorAtRowColumn(t32, 0, 0), 0);
  QCOMPARE(*iteratorAtRowColumn(t32, 0, 1), 0);
  QCOMPARE(*iteratorAtRowColumn(t32, 1, 0), 0);
  QCOMPARE(*iteratorAtRowColumn(t32, 1, 1), 0);
  QCOMPARE(*iteratorAtRowColumn(t32, 2, 0), 0);
  QCOMPARE(*iteratorAtRowColumn(t32, 2, 1), 0);
  *iteratorAtRowColumn(t32, 0, 0) = 10;
  *iteratorAtRowColumn(t32, 0, 1) = 11;
  *iteratorAtRowColumn(t32, 1, 0) = 20;
  *iteratorAtRowColumn(t32, 1, 1) = 21;
  *iteratorAtRowColumn(t32, 2, 0) = 30;
  *iteratorAtRowColumn(t32, 2, 1) = 31;
  QCOMPARE(*iteratorAtRowColumn(t32, 0, 0), 10);
  QCOMPARE(*iteratorAtRowColumn(t32, 0, 1), 11);
  QCOMPARE(*iteratorAtRowColumn(t32, 1, 0), 20);
  QCOMPARE(*iteratorAtRowColumn(t32, 1, 1), 21);
  QCOMPARE(*iteratorAtRowColumn(t32, 2, 0), 30);
  QCOMPARE(*iteratorAtRowColumn(t32, 2, 1), 31);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
    StlContainerTest test;

  return QTest::qExec(&test, argc, argv);
}
