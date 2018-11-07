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
#include "VectorTest.h"
#include "Mdt/Container/Vector.h"
#include <QString>
#include <string>
#include <algorithm>

// #include <QDebug>

using namespace Mdt::Container;

/*
 * Tests
 */

template<typename T>
void constructAndGetTest(const T & defaultValue)
{
  Vector<T> v0;
  QCOMPARE(v0.size(), 0);
  QVERIFY(v0.isEmpty());

  Vector<T> v1(1, defaultValue);
  QCOMPARE(v1.size(), 1);
  QVERIFY(!v1.isEmpty());
  QCOMPARE(v1.at(0), defaultValue);
}

void VectorTest::constructAndGet()
{
  constructAndGetTest<int>(0);
  constructAndGetTest<std::string>(std::string("A"));
  constructAndGetTest<QString>(QString("Q"));
}

void VectorTest::initializerList()
{
  Vector<int> vi{1,2,3};
  QCOMPARE(vi.size(), 3);
  QCOMPARE(vi.at(0), 1);
  QCOMPARE(vi.at(1), 2);
  QCOMPARE(vi.at(2), 3);

  Vector<std::string> vs{"A","B","C"};
  QCOMPARE(vs.size(), 3);
  QCOMPARE(vs.at(0), std::string("A"));
  QCOMPARE(vs.at(1), std::string("B"));
  QCOMPARE(vs.at(2), std::string("C"));
}

void VectorTest::findTest()
{
  Vector<int> vi{1,2,3};
  const auto viIt = std::find(vi.cbegin(), vi.cend(), 2);
  QCOMPARE(*viIt, 2);

  Vector<std::string> vs{"A","B","C"};
  const auto vsIt = std::find(vs.cbegin(), vs.cend(), std::string("B"));
  QCOMPARE(*vsIt, std::string("B"));
}

void VectorTest::addTest()
{
  Vector<int> vi;

  int i1 = 1;
  vi.append(i1);
  vi.append(2);
  int i3 = 3;
  vi.push_back(i3);
  vi.push_back(4);
  QCOMPARE(vi.size(), 4);
  QCOMPARE(vi.at(0), 1);
  QCOMPARE(vi.at(1), 2);
  QCOMPARE(vi.at(2), 3);
  QCOMPARE(vi.at(3), 4);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  VectorTest test;

  return QTest::qExec(&test, argc, argv);
}
