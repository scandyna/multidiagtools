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
#include "DataTest.h"
#include "Mdt/PlainText/Record.h"
#include "Mdt/PlainText/RecordList.h"
#include "Mdt/PlainText/StringRecord.h"
#include "Mdt/PlainText/StringRecordList.h"
#include <algorithm>
#include <iterator>

using namespace Mdt::PlainText;

void DataTest::initTestCase()
{
}

void DataTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void DataTest::recordTest()
{
  Record record;
  /*
   * Initial state
   */
  QCOMPARE(record.columnCount(), 0);
  /*
   * Resize
   */
  record.appendColumn();
  QCOMPARE(record.columnCount(), 1);
  record.setColumnCount(3);
  QCOMPARE(record.columnCount(), 3);
  /*
   * Set/get
   */
  record.setData(0, "A");
  QCOMPARE(record.value(0), QVariant("A"));
  QCOMPARE(record.data(0), QVariant("A"));
  /*
   * Clear
   */
  record.clear();
  QCOMPARE(record.columnCount(), 0);
  /*
   * Stream
   */
  record << "B" << 2;
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.data(0), QVariant("B"));
  QCOMPARE(record.data(1), QVariant(2));
  /*
   * Assign and initializer lists
   * (Mostly used by unit tests)
   */
  record = {"A",1, "B"};
  QCOMPARE(record.columnCount(), 3);
  QCOMPARE(record.data(0), QVariant("A"));
  QCOMPARE(record.data(1), QVariant(1));
  QCOMPARE(record.data(2), QVariant("B"));
}

void DataTest::recordListTest()
{
  RecordList list;
  /*
   * Initial state
   */
  QCOMPARE(list.rowCount(), 0);
  /*
   * Add/set/get
   */
  list.appendRecord({"A","B"});
  QCOMPARE(list.rowCount(), 1);
  QCOMPARE(list.columnCount(0), 2);
  QCOMPARE(list.data(0, 0), QVariant("A"));
  QCOMPARE(list.value(0, 0), QVariant("A"));
  QCOMPARE(list.data(0, 1), QVariant("B"));
  QCOMPARE(list.value(0, 1), QVariant("B"));
  list.setData(0, 0, "E");
  QCOMPARE(list.data(0, 0), QVariant("E"));
  QCOMPARE(list.data(0, 1), QVariant("B"));
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.rowCount(), 0);
  /*
   * Stream
   */
  list << Record{1,"A"} << Record{2,"B"};
  QCOMPARE(list.rowCount(), 2);
  QCOMPARE(list.data(0, 0), QVariant(1));
  QCOMPARE(list.data(0, 1), QVariant("A"));
  QCOMPARE(list.data(1, 0), QVariant(2));
  QCOMPARE(list.data(1, 1), QVariant("B"));
  /*
   * Assign and initializer lists
   * (Mostly used by unit tests)
   */
  list = {{3,"C"},{4,"D"}};
  QCOMPARE(list.rowCount(), 2);
  QCOMPARE(list.data(0, 0), QVariant(3));
  QCOMPARE(list.data(0, 1), QVariant("C"));
  QCOMPARE(list.data(1, 0), QVariant(4));
  QCOMPARE(list.data(1, 1), QVariant("D"));
  /*
   * Check STL iterators usage
   */
  const auto f1 = [](const Record & record){
    QVERIFY(record.columnCount() > 0);
  };
  std::for_each(list.begin(), list.end(), f1);
  RecordList list2;
  std::copy(list.cbegin(), list.cend(), std::back_inserter(list2));
  QCOMPARE(list2.rowCount(), list.rowCount());
}

void DataTest::stringRecordTest()
{
  StringRecord record;
  /*
   * Initial state
   */
  QCOMPARE(record.columnCount(), 0);
  /*
   * Resize
   */
  record.appendColumn();
  QCOMPARE(record.columnCount(), 1);
  record.setColumnCount(3);
  QCOMPARE(record.columnCount(), 3);
  /*
   * Set/get
   */
  record.setData(0, "A");
  QCOMPARE(record.value(0), QString("A"));
  QCOMPARE(record.data(0), QString("A"));
  /*
   * Clear
   */
  record.clear();
  QCOMPARE(record.columnCount(), 0);
  /*
   * Stream
   */
  record << "B" << "2";
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.data(0), QString("B"));
  QCOMPARE(record.data(1), QString("2"));
  /*
   * Assign and initializer lists
   * (Mostly used by unit tests)
   */
  record = {"A","1", "B"};
  QCOMPARE(record.columnCount(), 3);
  QCOMPARE(record.data(0), QString("A"));
  QCOMPARE(record.data(1), QString("1"));
  QCOMPARE(record.data(2), QString("B"));
  /*
   * Check STL iterators usage
   */
  std::sort(record.begin(), record.end());
  QCOMPARE(record.columnCount(), 3);
  QCOMPARE(record.data(0), QString("1"));
  QCOMPARE(record.data(1), QString("A"));
  QCOMPARE(record.data(2), QString("B"));
  const auto it1 = std::find(record.cbegin(), record.cend(), QString("1"));
  QVERIFY(it1 == record.cbegin());
}

void DataTest::stringRecordListTest()
{
  StringRecordList list;
  /*
   * Initial state
   */
  QCOMPARE(list.rowCount(), 0);
  /*
   * Add/set/get
   */
  list.appendRecord({"A","B"});
  QCOMPARE(list.rowCount(), 1);
  QCOMPARE(list.columnCount(0), 2);
  QCOMPARE(list.data(0, 0), QString("A"));
  QCOMPARE(list.value(0, 0), QString("A"));
  QCOMPARE(list.data(0, 1), QString("B"));
  QCOMPARE(list.value(0, 1), QString("B"));
  list.setData(0, 0, "E");
  QCOMPARE(list.data(0, 0), QString("E"));
  QCOMPARE(list.data(0, 1), QString("B"));
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.rowCount(), 0);
  /*
   * Stream
   */
  list << StringRecord{"1","A"} << StringRecord{"2","B"};
  QCOMPARE(list.rowCount(), 2);
  QCOMPARE(list.data(0, 0), QString("1"));
  QCOMPARE(list.data(0, 1), QString("A"));
  QCOMPARE(list.data(1, 0), QString("2"));
  QCOMPARE(list.data(1, 1), QString("B"));
  /*
   * Assign and initializer lists
   * (Mostly used by unit tests)
   */
  list = {{"3","C"},{"4","D"}};
  QCOMPARE(list.rowCount(), 2);
  QCOMPARE(list.data(0, 0), QString("3"));
  QCOMPARE(list.data(0, 1), QString("C"));
  QCOMPARE(list.data(1, 0), QString("4"));
  QCOMPARE(list.data(1, 1), QString("D"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  DataTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
