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
#include "PathTest.h"
#include "Mdt/DeployUtils/PathList.h"

using namespace Mdt::DeployUtils;

void PathTest::initTestCase()
{
}

void PathTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void PathTest::pathListAppendTest()
{
  QFETCH(PathList, initialList);
  QFETCH(QString, path);
  QFETCH(PathList, expectedList);

  PathList list = initialList;

  QCOMPARE(list.toStringList(), initialList.toStringList());
  list.appendPath(path);
  QCOMPARE(list.toStringList(), expectedList.toStringList());
}

void PathTest::pathListAppendTest_data()
{
  QTest::addColumn<PathList>("initialList");
  QTest::addColumn<QString>("path");
  QTest::addColumn<PathList>("expectedList");

  PathList initialList;
  PathList expectedList;

  initialList = {};
  expectedList = {"1"};
  QTest::newRow("{},1,{1}") << initialList << "1" << expectedList;

  initialList = {"A"};
  expectedList = {"A","B"};
  QTest::newRow("{A},B,{A,B}") << initialList << "B" << expectedList;

  initialList = {"A"};
  expectedList = {"A"};
  QTest::newRow("{A},A,{A}") << initialList << "A" << expectedList;

  initialList = {"A","B","C"};
  expectedList = {"A","C","B"};
  QTest::newRow("{A,B,C},B,{A,C,B}") << initialList << "B" << expectedList;
}

void PathTest::pathListAppendPathListTest()
{
  QFETCH(PathList, initialList);
  QFETCH(PathList, pathList);
  QFETCH(PathList, expectedList);

  PathList list = initialList;

  QCOMPARE(list.toStringList(), initialList.toStringList());
  list.appendPathList(pathList);
  QCOMPARE(list.toStringList(), expectedList.toStringList());
}

void PathTest::pathListAppendPathListTest_data()
{
  QTest::addColumn<PathList>("initialList");
  QTest::addColumn<PathList>("pathList");
  QTest::addColumn<PathList>("expectedList");

  QTest::newRow("0")
    << PathList{}
    << PathList{}
    << PathList{};

  QTest::newRow("1")
    << PathList{"A"}
    << PathList{}
    << PathList{"A"};

  QTest::newRow("2")
    << PathList{"A"}
    << PathList{""}
    << PathList{"A"};

  QTest::newRow("3")
    << PathList{}
    << PathList{"A"}
    << PathList{"A"};

  QTest::newRow("4")
    << PathList{"A"}
    << PathList{"A"}
    << PathList{"A"};

  QTest::newRow("5")
    << PathList{"A"}
    << PathList{"B"}
    << PathList{"A","B"};
}

void PathTest::pathListPrependTest()
{
  QFETCH(PathList, initialList);
  QFETCH(QString, path);
  QFETCH(PathList, expectedList);

  PathList list = initialList;

  QCOMPARE(list.toStringList(), initialList.toStringList());
  list.prependPath(path);
  QCOMPARE(list.toStringList(), expectedList.toStringList());
}

void PathTest::pathListPrependTest_data()
{
  QTest::addColumn<PathList>("initialList");
  QTest::addColumn<QString>("path");
  QTest::addColumn<PathList>("expectedList");

  PathList initialList;
  PathList expectedList;

  initialList = {};
  expectedList = {"1"};
  QTest::newRow("{},1,{1}") << initialList << "1" << expectedList;

  initialList = {"A"};
  expectedList = {"B","A"};
  QTest::newRow("{A},B,{B,A}") << initialList << "B" << expectedList;

  initialList = {"A"};
  expectedList = {"A"};
  QTest::newRow("{A},A,{A}") << initialList << "A" << expectedList;

  initialList = {"A","B","C"};
  expectedList = {"B","A","C"};
  QTest::newRow("{A,B,C},B,{B,A,C}") << initialList << "B" << expectedList;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  PathTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
