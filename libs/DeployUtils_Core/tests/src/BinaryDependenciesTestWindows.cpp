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
#include "BinaryDependenciesTestWindows.h"
#include "Mdt/DeployUtils/BinaryDependencies.h"
#include "Mdt/FileSystem/PathList.h"
#include "Mdt/DeployUtils/ObjdumpWrapper.h"
#include "Mdt/DeployUtils/LibraryInfo.h"
#include "Mdt/DeployUtils/LibraryName.h"
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
using namespace Mdt::FileSystem;

void BinaryDependenciesTestWindows::initTestCase()
{
}

void BinaryDependenciesTestWindows::cleanupTestCase()
{
}

/*
 * Tests
 */

void BinaryDependenciesTestWindows::runTest()
{
  ObjdumpWrapper objdump;
  if(objdump.findObjdump().isEmpty()){
    QFAIL("Could not find objdump executable");
  }

  const auto prefix = QString::fromLocal8Bit(PREFIX_PATH);
  PathList prefixPath = PathList::fromStringList( prefix.split(';', QString::SkipEmptyParts) );
  BinaryDependencies deps;
  LibraryInfoList dependencies;
  /*
   * Find dependencies for the executable of this unit test
   * We know that we depend at least on Qt5Core
   * Check that:
   *  - In Debug build, Qt5Cored.dll is listed, but not Qt5Core.dll
   *  - In Release build, Qt5Core.dll is listed, but not Qt5Cored.dll
   * (This was a bug discovered 20171029)
   */
  QVERIFY(deps.findDependencies( QCoreApplication::applicationFilePath(), prefixPath));
  dependencies = deps.dependencies();
  QVERIFY(!dependencies.isEmpty());
#ifdef QT_NO_DEBUG
  QVERIFY(!containsLibrary(dependencies, "Qt5Cored.dll"));
  QVERIFY(containsLibrary(dependencies, "Qt5Core.dll"));
#else
  QVERIFY(!containsLibrary(dependencies, "Qt5Core.dll"));
  QVERIFY(containsLibrary(dependencies, "Qt5Cored.dll"));
#endif
}

/*
 * Helpers
 */

bool BinaryDependenciesTestWindows::containsLibrary(const LibraryInfoList & libraries, const QString & libraryFullName)
{
  for(const auto & library : libraries){
    if( QString::compare( library.libraryName().fullName(), libraryFullName, Qt::CaseInsensitive ) == 0 ){
      return true;
    }
  }
  return false;
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  BinaryDependenciesTestWindows test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
