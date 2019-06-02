/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "Mdt/Container/StlContainer.h"
#include "Mdt/Container/Vector.h"
#include <QVariantList>
#include <QList>
#include <QStringList>
#include <QVariantList>
#include <QVector>
#include <vector>
#include <list>

// #include <QDebug>

using namespace Mdt::Container;

void StlContainerTest::initTestCase()
{
}

void StlContainerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

template<typename Container>
void initializeTestImpl()
{
  auto c1 = initializeContainer<Container>(1, 10);
  QCOMPARE(containerSize(c1), 1);
  QCOMPARE(constValueAtIndex(c1, 0), 10);

  const auto c2 = initializeContainer<Container>(2, 20);
  QCOMPARE(containerSize(c2), 2);
  QCOMPARE(constValueAtIndex(c2, 0), 20);
  QCOMPARE(constValueAtIndex(c2, 1), 20);
}

void StlContainerTest::initializeTest()
{
  initializeTestImpl< std::vector<int> >();
  initializeTestImpl< std::list<int> >();
  initializeTestImpl< QVector<int> >();
  initializeTestImpl< QList<int> >();
}

void StlContainerTest::initializeQStringListTest()
{
  auto c1 = initializeContainer<QStringList>(1, "A");
  QCOMPARE(containerSize(c1), 1);
  QCOMPARE(constValueAtIndex(c1, 0), QString("A"));

  auto c2 = initializeContainer<QStringList>(2, "B");
  QCOMPARE(containerSize(c2), 2);
  QCOMPARE(constValueAtIndex(c2, 0), QString("B"));
  QCOMPARE(constValueAtIndex(c2, 1), QString("B"));
}

template<typename Container>
void sizeTestImpl()
{
  Container c0;
  QCOMPARE(containerSize(c0), 0);
  Container c1{1};
  QCOMPARE(containerSize(c1), 1);
  Container c2{1,2};
  QCOMPARE(containerSize(c2), 2);
}

void StlContainerTest::sizeTest()
{
  sizeTestImpl< std::vector<int> >();
  sizeTestImpl< std::list<int> >();
  sizeTestImpl< QVector<int> >();
  sizeTestImpl< QList<int> >();
}

template<typename Container>
void isEmptyTestImpl()
{
  Container c0;
  QVERIFY(containerIsEmpty(c0));
  Container c1{1};
  QVERIFY(!containerIsEmpty(c1));
  Container c2{1,2};
  QVERIFY(!containerIsEmpty(c2));
}

void StlContainerTest::isEmptyTest()
{
  isEmptyTestImpl< std::vector<int> >();
  isEmptyTestImpl< QList<int> >();
  isEmptyTestImpl< QVector<int> >();
}

template<typename Container>
void constIteratorAtIndexTestImpl()
{
  Container c{1,2,3};

  QCOMPARE(*constIteratorAtIndex(c, 0), 1);
  QCOMPARE(*constIteratorAtIndex(c, 1), 2);
  QCOMPARE(*constIteratorAtIndex(c, 2), 3);
}

void StlContainerTest::constIteratorAtIndexTest()
{
  constIteratorAtIndexTestImpl< std::vector<int> >();
  constIteratorAtIndexTestImpl< std::list<int> >();
  constIteratorAtIndexTestImpl< QVector<int> >();
  constIteratorAtIndexTestImpl< QList<int> >();
}

template<typename Container>
void iteratorAtIndexTestImpl()
{
  Container c{1,2};

  QCOMPARE(*iteratorAtIndex(c, 0), 1);
  QCOMPARE(*iteratorAtIndex(c, 1), 2);
  *iteratorAtIndex(c, 0) = 10;
  *iteratorAtIndex(c, 1) = 20;
  QCOMPARE(*iteratorAtIndex(c, 0), 10);
  QCOMPARE(*iteratorAtIndex(c, 1), 20);
}

void StlContainerTest::iteratorAtIndexTest()
{
  iteratorAtIndexTestImpl< std::vector<int> >();
  iteratorAtIndexTestImpl< std::list<int> >();
  iteratorAtIndexTestImpl< QVector<int> >();
  iteratorAtIndexTestImpl< QList<int> >();
}

template<typename Container>
void constValueAtIndexTestImpl()
{
  Container c{1,2,3};

  QCOMPARE(constValueAtIndex(c, 0), 1);
  QCOMPARE(constValueAtIndex(c, 1), 2);
  QCOMPARE(constValueAtIndex(c, 2), 3);
}

void StlContainerTest::constValueAtIndexTest()
{
  constValueAtIndexTestImpl< std::vector<int> >();
  constValueAtIndexTestImpl< std::list<int> >();
  constValueAtIndexTestImpl< QVector<int> >();
  constValueAtIndexTestImpl< QList<int> >();
}

template<typename Container>
void callInsertTestImpl()
{
  Container container;
  QCOMPARE(containerSize(container), 0);

  auto it = callInsert(container, container.begin(), 1, 10);
  QCOMPARE(containerSize(container), 1);
  QCOMPARE(constValueAtIndex(container, 0), 10);
  QVERIFY(it == container.begin());

  it = callInsert(container, std::next(container.begin(), 1), 1, 20);
  QCOMPARE(containerSize(container), 2);
  QCOMPARE(constValueAtIndex(container, 0), 10);
  QCOMPARE(constValueAtIndex(container, 1), 20);
  QVERIFY(it == std::next(container.begin(), 1));
}

void StlContainerTest::callInsertTest()
{
  callInsertTestImpl< std::vector<int> >();
  callInsertTestImpl< std::list<int> >();
  callInsertTestImpl< QVector<int> >();
  callInsertTestImpl< QList<int> >();
}

template<typename Container>
void callEraseTestImpl()
{
  Container container{1,2,3};
  QCOMPARE(containerSize(container), 3);

  /*
   * |1|2|3|  -->  |1|3|
   *    f l           i
   */
  auto i = callErase( container, std::next(container.begin(), 1), std::next(container.begin(), 2) );
  QCOMPARE(containerSize(container), 2);
  QCOMPARE(constValueAtIndex(container, 0), 1);
  QCOMPARE(constValueAtIndex(container, 1), 3);
  QVERIFY( i == std::next(container.begin(), 1) );

  /*
   * |1|3|  -->  |1|
   *    i l         i
   */
  i = callErase( container, i, container.end() );
  QCOMPARE(containerSize(container), 1);
  QCOMPARE(constValueAtIndex(container, 0), 1);
  QVERIFY( i == container.end() );
  /*
   * |1|  -->  |
   *  f l       i
   */
  i = callErase( container, container.begin(), container.end() );
  QCOMPARE(containerSize(container), 0);
  QVERIFY( i == container.end() );
}

void StlContainerTest::callEraseTest()
{
  callEraseTestImpl< std::vector<int> >();
  callEraseTestImpl< std::list<int> >();
  callEraseTestImpl< QVector<int> >();
  callEraseTestImpl< QList<int> >();
}

template<typename Container>
void appendPrependInsertToContainerTestImpl()
{
  /*
   * Initial state
   */
  Container container;
  QCOMPARE(containerSize(container), 0);
  /*
   * Append a element
   * ---
   * |1|
   * ---
   */
  appendToContainer(container, 1);
  QCOMPARE(containerSize(container), 1);
  QCOMPARE(constValueAtIndex(container, 0), 1);
  /*
   * Prepend a element
   * -------
   * |-1| 1|
   * -------
   */
  prependToContainer(container, -1);
  QCOMPARE(containerSize(container), 2);
  QCOMPARE(constValueAtIndex(container, 0), -1);
  QCOMPARE(constValueAtIndex(container, 1), 1);
  /*
   * Insert 2 elements before '1'
   *
   * Before:
   * -------
   * |-1| 1|
   * -------
   *    ^
   * After:
   * -------------
   * |-1| 5| 5| 1|
   * -------------
   */
  insertToContainer(container, 1, 2, 5);
  QCOMPARE(containerSize(container), 4);
  QCOMPARE(constValueAtIndex(container, 0), -1);
  QCOMPARE(constValueAtIndex(container, 1), 5);
  QCOMPARE(constValueAtIndex(container, 2), 5);
  QCOMPARE(constValueAtIndex(container, 3), 1);
}

void StlContainerTest::appendPrependInsertToContainerTest()
{
  appendPrependInsertToContainerTestImpl< std::vector<int> >();
  appendPrependInsertToContainerTestImpl< Mdt::Container::Vector<int> >();
  appendPrependInsertToContainerTestImpl< QList<int> >();
  appendPrependInsertToContainerTestImpl< QVector<int> >();
  appendPrependInsertToContainerTestImpl< std::list<int> >();
}

template<typename Container>
void removeFromContainerTestImpl()
{
  /*
   * Initial state
   * -------------
   * |0|1|2|3|4|5|
   * -------------
   */
  Container container{0,1,2,3,4,5};
  QCOMPARE(containerSize(container), 6);
  /*
   * Remove 3 and 4
   * ---------
   * |0|1|2|5|
   * ---------
   */
  removeFromContainer(container, 3, 2);
  QCOMPARE(containerSize(container), 4);
  QCOMPARE(constValueAtIndex(container, 0), 0);
  QCOMPARE(constValueAtIndex(container, 1), 1);
  QCOMPARE(constValueAtIndex(container, 2), 2);
  QCOMPARE(constValueAtIndex(container, 3), 5);
  /*
   * Remove first element
   * -------
   * |1|2|5|
   * -------
   */
  removeFirstFromContainer(container);
  QCOMPARE(containerSize(container), 3);
  QCOMPARE(constValueAtIndex(container, 0), 1);
  QCOMPARE(constValueAtIndex(container, 1), 2);
  QCOMPARE(constValueAtIndex(container, 2), 5);
  /*
   * Remove last element
   * -----
   * |1|2|
   * -----
   */
  removeLastFromContainer(container);
  QCOMPARE(containerSize(container), 2);
  QCOMPARE(constValueAtIndex(container, 0), 1);
  QCOMPARE(constValueAtIndex(container, 1), 2);
  /*
   * Remove the 2 last elements
   */
  removeFromContainer(container, 0, 2);
  QCOMPARE(containerSize(container), 0);
}

void StlContainerTest::removeFromContainerTest()
{
  removeFromContainerTestImpl< std::vector<int> >();
  removeFromContainerTestImpl< QList<int> >();
  removeFromContainerTestImpl< QVector<int> >();
  removeFromContainerTestImpl< std::list<int> >();
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
