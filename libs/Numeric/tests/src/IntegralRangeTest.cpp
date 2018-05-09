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
#include "IntegralRangeTest.h"
#include "Mdt/Numeric/IntegralRange.h"

using namespace Mdt::Numeric;

/*
 * Tests
 */

template<typename T>
void initialStateTestImpl()
{
  IntegralRange<T> r;
  QCOMPARE(r.first(), static_cast<T>(0));
  QCOMPARE(r.last(), static_cast<T>(0));
  QCOMPARE(r.count(), static_cast<T>(0));
  QVERIFY(!r.contains(0));
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
}

void IntegralRangeTest::initialStateTest()
{
  initialStateTestImpl<int>();
  initialStateTestImpl<unsigned long>();
}

template<typename T>
void setFirstLastTestImpl()
{
  IntegralRange<T> r;
  /*
   * Set valid range
   */
  // Range starting from 0
  r.setFirst(0);
  r.setLast(1);
  QCOMPARE(r.first(), static_cast<T>(0));
  QCOMPARE(r.last(), static_cast<T>(1));
  QCOMPARE(r.count(), static_cast<T>(2));
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
  // Range not starting from 0
  r.setFirst(10);
  r.setLast(20);
  QCOMPARE(r.first(), static_cast<T>(10));
  QCOMPARE(r.last(), static_cast<T>(20));
  QCOMPARE(r.count(), static_cast<T>(11));
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
  /*
   * Clear
   */
  r.clear();
  QCOMPARE(r.first(), static_cast<T>(0));
  QCOMPARE(r.last(), static_cast<T>(0));
  QCOMPARE(r.count(), static_cast<T>(0));
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
  /*
   * Set invalid range
   */
  // First > last
  r.setFirst(1);
  r.setLast(0);
  QCOMPARE(r.first(), static_cast<T>(1));
  QCOMPARE(r.last(), static_cast<T>(0));
  QCOMPARE(r.count(), static_cast<T>(0));
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
}

void IntegralRangeTest::setFirstLastTest()
{
  setFirstLastTestImpl<int>();
  setFirstLastTestImpl<unsigned long>();
}

template<typename T>
void setFirstCountTestImpl()
{
  IntegralRange<T> r;
  /*
   * Set valid range
   */
  // Range starting from 0
  r.setFirst(0);
  r.setCount(1);
  QCOMPARE(r.first(), static_cast<T>(0));
  QCOMPARE(r.last(), static_cast<T>(0));
  QCOMPARE(r.count(), static_cast<T>(1));
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
  // Range not starting from 0
  r.setFirst(10);
  r.setCount(20);
  QCOMPARE(r.first(), static_cast<T>(10));
  QCOMPARE(r.last(), static_cast<T>(29));
  QCOMPARE(r.count(), static_cast<T>(20));
  QVERIFY(!r.isEmpty());
  QVERIFY(!r.isNull());
  QVERIFY(r.isValid());
  /*
   * Set empty range
   */
  r.setFirst(0);
  r.setCount(0);
  QCOMPARE(r.first(), static_cast<T>(0));
  QCOMPARE(r.last(), static_cast<T>(0));
  QCOMPARE(r.count(), static_cast<T>(0));
  QVERIFY(r.isEmpty());
  QVERIFY(r.isNull());
  QVERIFY(!r.isValid());
}

void IntegralRangeTest::setFirstCountTest()
{
  setFirstCountTestImpl<int>();
  setFirstCountTestImpl<unsigned long>();
}

template<typename T>
void containsTestImpl()
{
  IntegralRange<T> r;
  // 1 element range
  r.setFirst(0);
  r.setLast(0);
  QVERIFY(r.contains(0));
  QVERIFY(!r.contains(1));
  // 2 elements range
  r.setFirst(0);
  r.setLast(1);
  QVERIFY(r.contains(0));
  QVERIFY(r.contains(1));
  QVERIFY(!r.contains(2));
  // > 2 elements range starting from > 0
  r.setFirst(10);
  r.setLast(15);
  QVERIFY(!r.contains(9));
  QVERIFY(r.contains(10));
  QVERIFY(r.contains(11));
  QVERIFY(r.contains(12));
  QVERIFY(r.contains(13));
  QVERIFY(r.contains(14));
  QVERIFY(r.contains(15));
  QVERIFY(!r.contains(16));
}

void IntegralRangeTest::containsTest()
{
  containsTestImpl<int>();
  containsTestImpl<unsigned int>();
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  IntegralRangeTest test;

  return QTest::qExec(&test, argc, argv);
}
