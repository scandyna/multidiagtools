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
#include "StringIteratorTest.h"
#include "Mdt/PlainText/StringConstIterator.h"

using namespace Mdt::PlainText;

void StringIteratorTest::initTestCase()
{
}

void StringIteratorTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void StringIteratorTest::constIteratorTest()
{
  QString str;
  /*
   * Constructs and assignements
   */
  str = "ABCDEF";
  // Direct assignement
  StringConstIterator it(str.cbegin());
  QCOMPARE(*it, wchar_t('A'));
  // Default constructed
  StringConstIterator first, last;
  // Assignement
  first = str.cbegin();
  QCOMPARE(*first, wchar_t('A'));
  last = first;
  QCOMPARE(*last, wchar_t('A'));
  /*
   * Increment
   */
  it = str.cbegin();
  QCOMPARE(*it, wchar_t('A'));
  // Pre-increment
  ++it;
  QCOMPARE(*it, wchar_t('B'));
  // Post-increment
  QCOMPARE(*it++, wchar_t('B'));
  QCOMPARE(*it, wchar_t('C'));
  /*
   * Decrement
   */
  it = str.cbegin();
  QCOMPARE(*it, wchar_t('A'));
  ++it;
  ++it;
  QCOMPARE(*it, wchar_t('C'));
  // Pre-decrement
  --it;
  QCOMPARE(*it, wchar_t('B'));
  // Post-decrement
  QCOMPARE(*it--, wchar_t('B'));
  QCOMPARE(*it, wchar_t('A'));
  /*
   * Increment and decrement by n
   */
  it = str.cbegin();
  QCOMPARE(*it, wchar_t('A'));
  it += 2;
  QCOMPARE(*it, wchar_t('C'));
  it -= 2;
  QCOMPARE(*it, wchar_t('A'));
  first = it;
  it = first + 2;
  QCOMPARE(*it, wchar_t('C'));
  first = it - 1;
  QCOMPARE(*first, wchar_t('B'));
  /*
   * Index based access
   */
  it = str.cbegin();
  QCOMPARE(it[0], wchar_t('A'));
  QCOMPARE(it[1], wchar_t('B'));
  QCOMPARE(it[2], wchar_t('C'));
  /*
   * Comparisons
   */
  first = str.cbegin();
  it = first;
  QVERIFY(it == first);
  QVERIFY(!(it != first));
  QVERIFY(!(it < first));
  QVERIFY(it <= first);
  QVERIFY(!(it > first));
  QVERIFY(it >= first);
  // Increment it and check
  ++it;
  QVERIFY(!(it == first));
  QVERIFY(it != first);
  QVERIFY(!(it < first));
  QVERIFY(!(it <= first));
  QVERIFY(it > first);
  QVERIFY(it >= first);
}

void StringIteratorTest::constIteratorBenchmark()
{
  QString source = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz";
  std::vector<wchar_t> destination;

  destination.reserve(source.size());
  QBENCHMARK
  {
    destination.clear();
    StringConstIterator first(source.cbegin());
    StringConstIterator last(source.cend());
    while(first != last){
      destination.push_back(*first);
      ++first;
    }
  }
  QCOMPARE((int)destination.size(), source.size());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  StringIteratorTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
