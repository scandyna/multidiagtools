/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "BinaryDependenciesTest.h"
#include "Mdt/DeployUtils/BinaryDependencies.h"
#include "Mdt/DeployUtils/PathList.h"
#include "Mdt/DeployUtils/Platform.h"
#include <QCoreApplication>
#include <QStringList>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDir>

// #include <QDebug>

#ifndef PREFIX_PATH
 #error "PREFIX_PATH missing"
#endif

using namespace Mdt::DeployUtils;

void BinaryDependenciesTest::initTestCase()
{
}

void BinaryDependenciesTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void BinaryDependenciesTest::searchPathListTest()
{
  QStringList searchPaths;
  /*
   * Initial state
   */
  BinaryDependencies deps;
  QCOMPARE( deps.getLibrarySearchFirstPathList(BinaryDependencies::IncludeBinaryFileDirectory).toStringList(), QStringList() );
  QCOMPARE( deps.getLibrarySearchFirstPathList(BinaryDependencies::ExcludeBinaryFileDirectory).toStringList(), QStringList() );
  /*
   * Set binary file
   */
  QTemporaryFile binaryFile;
  QVERIFY(binaryFile.open());
  const auto binaryFileDirectory = QFileInfo(binaryFile).absoluteDir().absolutePath();
  QVERIFY( deps.setBinaryFile(binaryFile.fileName()) );
  searchPaths = deps.getLibrarySearchFirstPathList(BinaryDependencies::IncludeBinaryFileDirectory).toStringList();
  QCOMPARE( searchPaths.size(), 1 );
  QCOMPARE( searchPaths.at(0), binaryFileDirectory );
  QCOMPARE( deps.getLibrarySearchFirstPathList(BinaryDependencies::ExcludeBinaryFileDirectory).toStringList(), QStringList() );
  /*
   * Set library search first paths
   */
  deps.setLibrarySearchFirstPathList( PathList({"/opt/liba","/opt/libb"}) );
  searchPaths = deps.getLibrarySearchFirstPathList(BinaryDependencies::IncludeBinaryFileDirectory).toStringList();
  QCOMPARE( searchPaths.size(), 3 );
  QCOMPARE( searchPaths.at(0), binaryFileDirectory );
  QCOMPARE( searchPaths.at(1), QString("/opt/liba") );
  QCOMPARE( searchPaths.at(2), QString("/opt/libb") );
  QCOMPARE( deps.getLibrarySearchFirstPathList(BinaryDependencies::ExcludeBinaryFileDirectory).toStringList(),
            QStringList({"/opt/liba","/opt/libb"}) );
  /*
   * Add suffixes
   */
  deps.setLibrarySearchFirstPathSuffixList(QStringList({"bin","qt5/bin"}));
  searchPaths = deps.getLibrarySearchFirstPathList(BinaryDependencies::IncludeBinaryFileDirectory).toStringList();
  QCOMPARE( searchPaths.size(), 7 );
  QCOMPARE( searchPaths.at(0), binaryFileDirectory );
  QCOMPARE( searchPaths.at(1), QString("/opt/liba") );
  QCOMPARE( searchPaths.at(2), QString("/opt/liba/bin") );
  QCOMPARE( deps.getLibrarySearchFirstPathList(BinaryDependencies::ExcludeBinaryFileDirectory).toStringList(),
            QStringList({"/opt/liba","/opt/liba/bin","/opt/liba/qt5/bin","/opt/libb","/opt/libb/bin","/opt/libb/qt5/bin"}) );
}

void BinaryDependenciesTest::runTest()
{
  const auto prefix = QString::fromLocal8Bit(PREFIX_PATH);
  PathList searchFirstPrefixPaths = PathList::fromStringList( prefix.split(';', QString::SkipEmptyParts) );
  BinaryDependencies deps;

  QVERIFY(deps.findDependencies( QCoreApplication::applicationFilePath(), searchFirstPrefixPaths));
  QVERIFY(!deps.dependencies().isEmpty());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  BinaryDependenciesTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
