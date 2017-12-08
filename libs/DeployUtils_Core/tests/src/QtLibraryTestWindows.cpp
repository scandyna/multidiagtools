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
#include "QtLibraryTestWindows.h"
#include "Mdt/Application.h"
#include "Mdt/DeployUtils/QtLibrary.h"
#include "Mdt/FileSystem/PathList.h"
#include "Mdt/DeployUtils/BinaryDependencies.h"
#include "Mdt/DeployUtils/ObjdumpWrapper.h"
#include "Mdt/DeployUtils/LibraryInfo.h"
#include "Mdt/DeployUtils/LibraryName.h"
#include "Mdt/DeployUtils/QtLibrary.h"
#include <QString>
#include <QStringList>
#include <QTemporaryDir>
#include <QDir>

// #include <QDebug>

#ifndef PREFIX_PATH
 #error "PREFIX_PATH missing"
#endif

using namespace Mdt::DeployUtils;
using namespace Mdt::FileSystem;

void QtLibraryTestWindows::initTestCase()
{
}

void QtLibraryTestWindows::cleanupTestCase()
{
}

/*
 * Tests
 */

void QtLibraryTestWindows::findPluginsRootTest()
{
  QFETCH(PathList, pathPrefixList);
  QFETCH(QStringList, existingPaths);
  QFETCH(QString, expectedPlginsRoot);
  /*
   * Setup a fake root
   */
  QTemporaryDir root;
  QVERIFY(root.isValid());
  QVERIFY( createPathListInFakeRoot(root, existingPaths) );
  prependFakeRootToPathList(root, pathPrefixList);
  prependFakeRootToPath(root, expectedPlginsRoot);
  /*
   * Check
   */
  QCOMPARE( QtLibrary::findPluginsRoot(pathPrefixList), expectedPlginsRoot );
}

void QtLibraryTestWindows::findPluginsRootTest_data()
{
  QTest::addColumn<PathList>("pathPrefixList");
  QTest::addColumn<QStringList>("existingPaths");
  QTest::addColumn<QString>("expectedPlginsRoot");

  QTest::newRow("QtRoot") << PathList{"/opt/qtroot"}
                           << QStringList{"/opt/qtroot/plugins"}
                           << "/opt/qtroot/plugins";

  QTest::newRow("QtRoot") << PathList{"/opt/qtroot/bin"}
                           << QStringList{"/opt/qtroot/plugins"}
                           << "/opt/qtroot/plugins";
}

void QtLibraryTestWindows::findPluginsRootFromSysTest()
{
  const auto pluginsRoot = QtLibrary::findPluginsRoot(PathList{});
  QVERIFY( !pluginsRoot.contains("CMake/plugins", Qt::CaseInsensitive) );
}

void QtLibraryTestWindows::findPluginsDependenciesTest()
{
  ObjdumpWrapper objdump;
  if(objdump.findObjdump().isEmpty()){
    QFAIL("Could not find objdump executable");
  }

  const auto prefix = QString::fromLocal8Bit(PREFIX_PATH);
  PathList prefixPath = PathList::fromStringList( prefix.split(';', QString::SkipEmptyParts) );
  BinaryDependencies deps;
  /*
   * Prepare the test:
   * We need Qt5Gui in a library list.
   */
  QVERIFY(deps.findDependencies( QCoreApplication::applicationFilePath(), prefixPath));
  const auto exeDependencies = deps.dependencies();
#ifdef QT_NO_DEBUG
  QVERIFY(containsLibrary(exeDependencies, "Qt5Gui.dll"));
  QVERIFY(!containsLibrary(exeDependencies, "Qt5Guid.dll"));
#else
  QVERIFY(containsLibrary(exeDependencies, "Qt5Guid.dll"));
  QVERIFY(!containsLibrary(exeDependencies, "Qt5Gui.dll"));
#endif
  /*
   * Find Qt plugins and their dependencies for the executable of this unit test
   * We know that we depend at least on qwindows.dll, whitch also depends at least on Qt5Core
   * 1) Find Qt plugins for this executable and check:
   *  - In Debug build, qwindowsd.dll is listed, but not qwindows.dll
   *  - In Release build, qwindows.dll is listed, but not qwindowsd.dll
   * 2) Find dependencies of found Qt plugins and check:
   *  - In Debug build, Qt5Cored.dll is listed, but not Qt5Core.dll
   *  - In Release build, Qt5Core.dll is listed, but not Qt5Cored.dll
   * (This was a bug discovered 20171101)
   */
  QtLibrary qtLibrary;
  const auto qtLibraries = qtLibrary.getQtLibraries(exeDependencies);
  const auto qtPlugins = qtLibrary.findLibrariesPlugins(qtLibraries, prefixPath);
  const auto qtPluginsLibraries = qtPlugins.toLibraryInfoList();
#ifdef QT_NO_DEBUG
  QVERIFY(containsLibrary(qtPluginsLibraries, "qwindows.dll"));
  QVERIFY(!containsLibrary(qtPluginsLibraries, "qwindowsd.dll"));
#else
  QVERIFY(containsLibrary(qtPluginsLibraries, "qwindowsd.dll"));
  QVERIFY(!containsLibrary(qtPluginsLibraries, "qwindows.dll"));
#endif
  QVERIFY(deps.findDependencies(qtPluginsLibraries, prefixPath));
  const auto qtPluginsDependentLibraries = deps.dependencies();
#ifdef QT_NO_DEBUG
  QVERIFY(containsLibrary(qtPluginsDependentLibraries, "Qt5Core.dll"));
  QVERIFY(!containsLibrary(qtPluginsDependentLibraries, "Qt5Cored.dll"));
#else
  QVERIFY(containsLibrary(qtPluginsDependentLibraries, "Qt5Cored.dll"));
  QVERIFY(!containsLibrary(qtPluginsDependentLibraries, "Qt5Core.dll"));
#endif
}

/*
 * Helpers
 */

bool QtLibraryTestWindows::containsLibrary(const Mdt::DeployUtils::LibraryInfoList& libraries, const QString& libraryFullName)
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
  // For above test we must depend on Qt5Gui
  Mdt::Application app(argc, argv);
  QtLibraryTestWindows test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
