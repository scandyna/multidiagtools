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
#include "StlTableProxyIteratorTest.h"
#include "Mdt/Entity/StlTableProxyIterator.h"
#include <QtGlobal>
#include <vector>
#include <algorithm>

#include <QDebug>

using namespace Mdt::Entity;

/*
 * Proxy implementation
 */

class DataAccessProxyImpl
{
 public:

  int rowCount() const
  {
    return data.size();
  }

  const int & constRecordAt(int row) const
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < rowCount());
    return data[row];
  }

  int & recordAt(int row)
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < rowCount());
    return data[row];
  }

  std::vector<int> data;
};

class SomeClass
{
 public:

  SomeClass(int v)
   : mValue(v)
  {
  }

  void setValue(int v)
  {
    mValue = v;
  }

  int value() const
  {
    return mValue;
  }

 private:

  int mValue = 0;
};

class SomeClassAccessProxyImpl
{
 public:

  int rowCount() const
  {
    return data.size();
  }

  const SomeClass & constRecordAt(int row) const
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < rowCount());
    return data[row];
  }

  SomeClass & recordAt(int row)
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < rowCount());
    return data[row];
  }

  std::vector<SomeClass> data;
};

/*
 * Tests
 */

template<typename Iterator>
void iteratorCommonRequirementTestImpl()
{
  using value_type = typename Iterator::value_type;

  value_type value;
  DataAccessProxyImpl impl;
  impl.data = {1,2};
  Iterator it(0, &impl);
  /*
   * Dereference
   */
  value = *it;
  QCOMPARE(value, 1);
  QCOMPARE(*it, 1);
  /*
   * Increment
   */
  ++it;
  QCOMPARE(*it, 2);
  /*
   * Copy construct
   */
  Iterator it1(it);
  QCOMPARE(*it1, 2);
  /*
   * Copy assign
   */
  Iterator it2(0, &impl);
  QCOMPARE(*it2, 1);
  it2 = it1;
  QCOMPARE(*it2, 2);
}

void StlTableProxyIteratorTest::iteratorRequirementTest()
{
  using const_iterator = StlTableProxyConstIterator<int, DataAccessProxyImpl>;
  using iterator = StlTableProxyIterator<int, DataAccessProxyImpl>;

  iteratorCommonRequirementTestImpl<const_iterator>();
  iteratorCommonRequirementTestImpl<iterator>();

  DataAccessProxyImpl impl;
  /*
   * Dereference iterator for write
   */
  impl.data = {1,2};
  iterator it(0, &impl);
  QCOMPARE(*it, 1);
  *it = 3;
  QCOMPARE(*it, 3);
  /*
   * Swap
   */
  impl.data = {1,2};
  iterator itA(0, &impl);
  iterator itB(1, &impl);
  QCOMPARE(*itA, 1);
  QCOMPARE(*itB, 2);
  std::swap(*itA, *itB);
  QCOMPARE(*itA, 2);
  QCOMPARE(*itB, 1);
  /*
   * Move
   */
  impl.data = {1,2};
  QCOMPARE(std::move(*iterator(0, &impl)), 1);
}

template<typename InputIterator>
void inputIteratorCommonRequirementTestImpl()
{
  DataAccessProxyImpl impl;
  impl.data = {1,2,3,4};
  /*
   * Increment
   */
  InputIterator it(0, &impl);
  QCOMPARE(*it, 1);
  ++it;
  QCOMPARE(*it, 2);
  it++;
  QCOMPARE(*it, 3);
  QCOMPARE(*it++, 3);
  QCOMPARE(*it, 4);
  /*
   * Equality comparison
   */
  InputIterator it1(0, &impl);
  InputIterator it2(0, &impl);
  QVERIFY(it1 == it2);
  QVERIFY(!(it1 != it2));
  ++it2;
  QVERIFY(!(it1 == it2));
  QVERIFY(it1 != it2);
  /*
   * Member access
   */
  using iterator3 = StlTableProxyIterator<SomeClass, SomeClassAccessProxyImpl>;
  SomeClassAccessProxyImpl impl3;
  impl3.data = {SomeClass(1)};
  iterator3 it3(0, &impl3);
  QCOMPARE(it3->value(), 1);
}

void StlTableProxyIteratorTest::inputIteratorRequirementTest()
{
  using const_iterator = StlTableProxyConstIterator<int, DataAccessProxyImpl>;
  using iterator = StlTableProxyIterator<int, DataAccessProxyImpl>;

  inputIteratorCommonRequirementTestImpl<const_iterator>();
  inputIteratorCommonRequirementTestImpl<iterator>();
}

void StlTableProxyIteratorTest::memberAccessTest()
{
  using const_iterator = StlTableProxyConstIterator<SomeClass, SomeClassAccessProxyImpl>;
  using iterator = StlTableProxyIterator<SomeClass, SomeClassAccessProxyImpl>;

  SomeClassAccessProxyImpl impl;
  impl.data = {SomeClass(1)};

  const_iterator cit(0, &impl);
  QCOMPARE(cit->value(), 1);

  iterator it(0, &impl);
  it->setValue(5);
  QCOMPARE(it->value(), 5);
}

void StlTableProxyIteratorTest::outputIteratorRequirementTest()
{
  using iterator = StlTableProxyIterator<int, DataAccessProxyImpl>;

  DataAccessProxyImpl impl;
  impl.data = {1,2};

  iterator it(0, &impl);
  *it = 10;
  QCOMPARE(impl.data[0], 10);
  *it++ = 11;
  QCOMPARE(impl.data[0], 11);
}

template<typename ForwardIterator>
void forwardIteratorCommonRequirementTestImpl()
{
  DataAccessProxyImpl impl;
  impl.data = {1,2};
  /*
   * Default construct
   */
  ForwardIterator itNull;
  ForwardIterator it(0, &impl);
  QVERIFY(it != itNull);
  /*
   * Multi-pass
   */
  ForwardIterator it1(0, &impl);
  ForwardIterator it2 = it1;
  QVERIFY(it1 == it2);
  QCOMPARE(*it1, 1);
  QCOMPARE(*it2, 1);
  ++it2;
  QCOMPARE(*it1, 1);
  QCOMPARE(*it2, 2);
}

void StlTableProxyIteratorTest::forwardIteratorRequirementTest()
{
  using const_iterator = StlTableProxyConstIterator<int, DataAccessProxyImpl>;
  using iterator = StlTableProxyIterator<int, DataAccessProxyImpl>;

  forwardIteratorCommonRequirementTestImpl<const_iterator>();
  forwardIteratorCommonRequirementTestImpl<iterator>();
}

template<typename BidirectionalIterator>
void bidirectionalIteratorCommonRequirementTestImpl()
{
  DataAccessProxyImpl impl;
  impl.data = {1,2,3,4};
  /*
   * Decrement
   */
  BidirectionalIterator it(3, &impl);
  QCOMPARE(*it, 4);
  --it;
  QCOMPARE(*it, 3);
  it--;
  QCOMPARE(*it, 2);
  QCOMPARE(*it--, 2);
  QCOMPARE(*it, 1);
}

void StlTableProxyIteratorTest::bidirectionalIteratorRequirementTest()
{
  using const_iterator = StlTableProxyConstIterator<int, DataAccessProxyImpl>;
  using iterator = StlTableProxyIterator<int, DataAccessProxyImpl>;

  bidirectionalIteratorCommonRequirementTestImpl<const_iterator>();
  bidirectionalIteratorCommonRequirementTestImpl<iterator>();
}

template<typename RandomAccessIterator>
void randomAccessIteratorCommonRequirementTestImpl()
{
  DataAccessProxyImpl impl;
  impl.data = {1,2,3,4};

  RandomAccessIterator it(1, &impl);
  QCOMPARE(*it, 2);
  it += 2;
  QCOMPARE(*it, 4);
  it += -1;
  QCOMPARE(*it, 3);
  auto it1 = it + 1;
  QCOMPARE(*it1, 4);
  QCOMPARE(*it, 3);
  it1 = -1 + it;
  QCOMPARE(*it1, 2);
  QCOMPARE(*it, 3);
  it -= 1;
  QCOMPARE(*it, 2);
  it1 = it - 1;
  QCOMPARE(*it1, 1);
  QCOMPARE(*it, 2);
  QCOMPARE(it - it1, 1);
  /*
   * Comparison
   */
  RandomAccessIterator it2(0, &impl);
  it1 = it2;
  QVERIFY(!(it1 < it2));
  QVERIFY(it1 <= it2);
  QVERIFY(!(it1 > it2));
  QVERIFY(it1 >= it2);
  ++it2;
  QVERIFY(it1 < it2);
  QVERIFY(it1 <= it2);
  QVERIFY(it2 > it1);
  QVERIFY(it2 >= it1);
}

void StlTableProxyIteratorTest::randomAccessIteratorRequirementTest()
{
  using const_iterator = StlTableProxyConstIterator<int, DataAccessProxyImpl>;
  using iterator = StlTableProxyIterator<int, DataAccessProxyImpl>;

  randomAccessIteratorCommonRequirementTestImpl<const_iterator>();
  randomAccessIteratorCommonRequirementTestImpl<iterator>();
}

template<typename Iterator>
void stdFindTestImpl()
{
  DataAccessProxyImpl impl;
  impl.data = {1,2,3,4};

  Iterator begin(0, &impl);
  Iterator end(4, &impl);

  auto it = std::find(begin, end, 2);
  QCOMPARE(*it, 2);
  it = std::find(begin, end, 56);
  QVERIFY(it == end);
}

void StlTableProxyIteratorTest::stdFindTest()
{
  using const_iterator = StlTableProxyConstIterator<int, DataAccessProxyImpl>;
  using iterator = StlTableProxyIterator<int, DataAccessProxyImpl>;

  stdFindTestImpl<const_iterator>();
  stdFindTestImpl<iterator>();
}

void StlTableProxyIteratorTest::stdSortTest()
{
  using iterator = StlTableProxyIterator<int, DataAccessProxyImpl>;

  DataAccessProxyImpl impl;
  impl.data = {4,3,2,1};

  iterator begin(0, &impl);
  iterator end(4, &impl);

  std::sort(begin, end);
  QCOMPARE(impl.data[0], 1);
  QCOMPARE(impl.data[1], 2);
  QCOMPARE(impl.data[2], 3);
  QCOMPARE(impl.data[3], 4);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  StlTableProxyIteratorTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
