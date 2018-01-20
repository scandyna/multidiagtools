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
#include "StlContainerIteratorAdapterTest.h"
#include "Mdt/ItemModel/StlContainerIteratorAdapter.h"
#include <QVariantList>
#include <QList>
#include <QStringList>
#include <QVariantList>
#include <QVector>
#include <vector>
#include <list>

using namespace Mdt::ItemModel;

void StlContainerIteratorAdapterTest::initTestCase()
{
}

void StlContainerIteratorAdapterTest::cleanupTestCase()
{
}

/*
 * Tests
 */

template<typename Container>
void containerSizeTestImpl()
{
  StlContainerIteratorAdapter<Container> adapter;

  Container c0;
  QCOMPARE(adapter.containerSize(c0), 0);
  Container c1{1};
  QCOMPARE(adapter.containerSize(c1), 1);
  Container c3{1,2,3};
  QCOMPARE(adapter.containerSize(c3), 3);
}

void StlContainerIteratorAdapterTest::containerSizeTest()
{
  containerSizeTestImpl< std::vector<int> >();
  containerSizeTestImpl< QVector<int> >();
  containerSizeTestImpl< QList<int> >();
  containerSizeTestImpl< std::list<int> >();
}

template<typename Container>
void containerIsEmptyTestImpl()
{
  StlContainerIteratorAdapter<Container> adapter;

  Container c0;
  QVERIFY(adapter.containerIsEmpty(c0));
  Container c1{1};
  QVERIFY(!adapter.containerIsEmpty(c1));
}

void StlContainerIteratorAdapterTest::containerIsEmptyTest()
{
  containerIsEmptyTestImpl< std::vector<int> >();
  containerIsEmptyTestImpl< QVector<int> >();
  containerIsEmptyTestImpl< QList<int> >();
  containerIsEmptyTestImpl< std::list<int> >();
}

template<typename Container>
void valueAtIndexTestImpl()
{
  StlContainerIteratorAdapter<Container> adapter;

  Container c1{1};
  QCOMPARE(adapter.valueAtIndex(c1, 0), 1);

  Container c3{1,2,3};
  QCOMPARE(adapter.valueAtIndex(c3, 0), 1);
  QCOMPARE(adapter.valueAtIndex(c3, 1), 2);
  QCOMPARE(adapter.valueAtIndex(c3, 2), 3);
}

void StlContainerIteratorAdapterTest::valueAtIndexTest()
{
  valueAtIndexTestImpl< std::vector<int> >();
  valueAtIndexTestImpl< QVector<int> >();
  valueAtIndexTestImpl< QList<int> >();
  valueAtIndexTestImpl< std::list<int> >();
}

template<typename Container>
void setValueAtIndexTestImpl()
{
  StlContainerIteratorAdapter<Container> adapter;

  Container c1{1};
  QCOMPARE(adapter.valueAtIndex(c1, 0), 1);
  adapter.setValueAtIndex(c1, 0, 11);
  QCOMPARE(adapter.valueAtIndex(c1, 0), 11);

  Container c3{1,2,3};
  QCOMPARE(adapter.valueAtIndex(c3, 0), 1);
  QCOMPARE(adapter.valueAtIndex(c3, 1), 2);
  QCOMPARE(adapter.valueAtIndex(c3, 2), 3);
  adapter.setValueAtIndex(c3, 0, 11);
  adapter.setValueAtIndex(c3, 1, 12);
  adapter.setValueAtIndex(c3, 2, 13);
  QCOMPARE(adapter.valueAtIndex(c3, 0), 11);
  QCOMPARE(adapter.valueAtIndex(c3, 1), 12);
  QCOMPARE(adapter.valueAtIndex(c3, 2), 13);
}

void StlContainerIteratorAdapterTest::setValueAtIndexTest()
{
  setValueAtIndexTestImpl< std::vector<int> >();
  setValueAtIndexTestImpl< QVector<int> >();
  setValueAtIndexTestImpl< QList<int> >();
  setValueAtIndexTestImpl< std::list<int> >();
}

template<typename Container>
void initializeContainerTestImpl()
{
  StlContainerIteratorAdapter<Container> adapter;

  auto c1 = adapter.initializeContainer(1, 11);
  QCOMPARE(adapter.containerSize(c1), 1);
  QCOMPARE(adapter.valueAtIndex(c1, 0), 11);

  auto c3 = adapter.initializeContainer(3, 25);
  QCOMPARE(adapter.containerSize(c3), 3);
  QCOMPARE(adapter.valueAtIndex(c3, 0), 25);
  QCOMPARE(adapter.valueAtIndex(c3, 1), 25);
  QCOMPARE(adapter.valueAtIndex(c3, 2), 25);
}

void StlContainerIteratorAdapterTest::initializeContainerTest()
{
  initializeContainerTestImpl< std::vector<int> >();
  initializeContainerTestImpl< QVector<int> >();
  initializeContainerTestImpl< QList<int> >();
  initializeContainerTestImpl< std::list<int> >();
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);

  StlContainerIteratorAdapterTest test;

  return QTest::qExec(&test, argc, argv);
}
