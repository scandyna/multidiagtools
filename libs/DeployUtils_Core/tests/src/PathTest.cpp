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
#include "Mdt/DeployUtils/SearchPathList.h"

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

void PathTest::pathListStringListTest()
{
  /*
   * Construct from a initializer list
   */
  PathList pl1{"a","b"};
  QCOMPARE(pl1.toStringList(), QStringList({"a","b"}));
  QVERIFY(pl1.containsPath("a"));
  QVERIFY(pl1.containsPath("b"));
  QVERIFY(!pl1.containsPath("c"));
  /*
   * Construct from an QStringList
   */
  auto pl2 = PathList::fromStringList({"c","d"});
  QCOMPARE(pl2.toStringList(), QStringList({"c","d"}));
}

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

  QTest::newRow("6")
    << PathList{"A"}
    << PathList{"B","C"}
    << PathList{"A","B","C"};
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

void PathTest::pathListPrependPathListTest()
{
  QFETCH(PathList, initialList);
  QFETCH(PathList, pathList);
  QFETCH(PathList, expectedList);

  PathList list = initialList;

  QCOMPARE(list.toStringList(), initialList.toStringList());
  list.prependPathList(pathList);
  QCOMPARE(list.toStringList(), expectedList.toStringList());
}

void PathTest::pathListPrependPathListTest_data()
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
    << PathList{"B","A"};

  QTest::newRow("6")
    << PathList{"A"}
    << PathList{"B","C"}
    << PathList{"B","C","A"};
}

void PathTest::searchPathListTest()
{
  /*
   * Initial state
   */
  SearchPathList spl;
  QCOMPARE(spl.toStringList(), QStringList({}));
  /*
   * Set a path prefix
   */
  spl.setIncludePathPrefixes(true);
  spl.setPathPrefixList({"/opt/liba"});
  QCOMPARE(spl.toStringList(), QStringList({"/opt/liba"}));
  /*
   * Set a path suffix
   */
  spl.setPathSuffixList({"bin"});
  QCOMPARE(spl.toStringList(), QStringList({"/opt/liba","/opt/liba/bin"}));
  /*
   * Prepend a path
   */
  spl.prependPath("/opt/appc");
  QCOMPARE(spl.toStringList(), QStringList({"/opt/appc","/opt/liba","/opt/liba/bin"}));
  /*
   * Append a path
   */
  spl.appendPath("/opt/appd");
  QCOMPARE(spl.toStringList(), QStringList({"/opt/appc","/opt/liba","/opt/liba/bin","/opt/appd"}));
  /*
   * Specify to not include path prefixes
   */
  spl.setIncludePathPrefixes(false);
  QCOMPARE(spl.toStringList(), QStringList({"/opt/appc","/opt/liba/bin","/opt/appd"}));
  /*
   * Clear
   */
  spl.clear();
  spl.setIncludePathPrefixes(true);
  QCOMPARE(spl.toStringList(), QStringList({}));
  /*
   * Set a list of path prefixes
   */
  spl.setPathPrefixList({"/opt/liba","/opt/libb"});
  QCOMPARE(spl.toStringList(), QStringList({"/opt/liba","/opt/libb"}));
  /*
   * Prepend a path
   */
  spl.prependPath("/opt/appc");
  QCOMPARE(spl.toStringList(), QStringList({"/opt/appc","/opt/liba","/opt/libb"}));
  /*
   * Set a list of path suffixes
   */
  spl.setPathSuffixList({"bin","lib"});
  QCOMPARE(spl.toStringList(), QStringList({"/opt/appc","/opt/liba","/opt/liba/bin","/opt/liba/lib","/opt/libb","/opt/libb/bin","/opt/libb/lib"}));
  /*
   * Clear
   * - By default, path prefixes are not included
   */
  spl.clear();
  QCOMPARE(spl.toStringList(), QStringList({}));
  /*
   * Set a list of path prefixes
   */
  spl.setPathPrefixList({"/opt/liba","/opt/libb"});
  QCOMPARE(spl.toStringList(), QStringList({}));
  /*
   * Set a list of path suffixes
   */
  spl.setPathSuffixList({"bin","lib"});
  QCOMPARE(spl.toStringList(), QStringList({"/opt/liba/bin","/opt/liba/lib","/opt/libb/bin","/opt/libb/lib"}));
  /*
   * Setup list of path prefixes and suffixes
   * (This is how QtLibrary uses it to find plugins)
   */
  spl.clear();
  spl.setIncludePathPrefixes(true);
  spl.setPathPrefixList( PathList{"/opt/tools/bin","/opt/qt/bin"} );
  spl.setPathSuffixList({"qt5",".."});
  QCOMPARE(spl.toStringList(), QStringList({"/opt/tools/bin","/opt/tools/bin/qt5","/opt/tools","/opt/qt/bin","/opt/qt/bin/qt5","/opt/qt"}));
  /*
   * When using .. suffix, check that order is preserved
   * (Bug discovered 20171101)
   */
  spl.clear();
  spl.setIncludePathPrefixes(true);
  spl.setPathPrefixList( PathList{"/opt/liba/bin","/opt/libb","/opt/liba/lib"} );
  spl.setPathSuffixList({".."});
  qDebug() << spl.toStringList();
  QCOMPARE(spl.toStringList(), QStringList({"/opt/liba/bin","/opt/liba","/opt/libb","/opt","/opt/liba/lib"}));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  PathTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
