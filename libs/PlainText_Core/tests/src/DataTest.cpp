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
