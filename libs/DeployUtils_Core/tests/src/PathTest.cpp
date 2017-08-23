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

//   QStringList searchPaths;
//   /*
//    * Initial state
//    */
//   BinaryDependencies deps;
//   QCOMPARE( deps.getLibrarySearchFirstPathList(BinaryDependencies::IncludeBinaryFileDirectory).toStringList(), QStringList() );
//   QCOMPARE( deps.getLibrarySearchFirstPathList(BinaryDependencies::ExcludeBinaryFileDirectory).toStringList(), QStringList() );
//   /*
//    * Set binary file
//    */
//   QTemporaryFile binaryFile;
//   QVERIFY(binaryFile.open());
//   const auto binaryFileDirectory = QFileInfo(binaryFile).absoluteDir().absolutePath();
//   QVERIFY( deps.setBinaryFile(binaryFile.fileName()) );
//   searchPaths = deps.getLibrarySearchFirstPathList(BinaryDependencies::IncludeBinaryFileDirectory).toStringList();
//   QCOMPARE( searchPaths.size(), 1 );
//   QCOMPARE( searchPaths.at(0), binaryFileDirectory );
//   QCOMPARE( deps.getLibrarySearchFirstPathList(BinaryDependencies::ExcludeBinaryFileDirectory).toStringList(), QStringList() );
//   /*
//    * Set library search first paths
//    */
//   deps.setLibrarySearchFirstPathList( PathList({"/opt/liba","/opt/libb"}) );
//   searchPaths = deps.getLibrarySearchFirstPathList(BinaryDependencies::IncludeBinaryFileDirectory).toStringList();
//   QCOMPARE( searchPaths.size(), 3 );
//   QCOMPARE( searchPaths.at(0), binaryFileDirectory );
//   QCOMPARE( searchPaths.at(1), QString("/opt/liba") );
//   QCOMPARE( searchPaths.at(2), QString("/opt/libb") );
//   QCOMPARE( deps.getLibrarySearchFirstPathList(BinaryDependencies::ExcludeBinaryFileDirectory).toStringList(),
//             QStringList({"/opt/liba","/opt/libb"}) );
//   /*
//    * Add suffixes
//    */
//   deps.setLibrarySearchFirstPathSuffixList(QStringList({"bin","qt5/bin"}));
//   searchPaths = deps.getLibrarySearchFirstPathList(BinaryDependencies::IncludeBinaryFileDirectory).toStringList();
//   QCOMPARE( searchPaths.size(), 7 );
//   QCOMPARE( searchPaths.at(0), binaryFileDirectory );
//   QCOMPARE( searchPaths.at(1), QString("/opt/liba") );
//   QCOMPARE( searchPaths.at(2), QString("/opt/liba/bin") );
//   QCOMPARE( deps.getLibrarySearchFirstPathList(BinaryDependencies::ExcludeBinaryFileDirectory).toStringList(),
//             QStringList({"/opt/liba","/opt/liba/bin","/opt/liba/qt5/bin","/opt/libb","/opt/libb/bin","/opt/libb/qt5/bin"}) );

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
   * Clear
   */
  spl.clear();
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
