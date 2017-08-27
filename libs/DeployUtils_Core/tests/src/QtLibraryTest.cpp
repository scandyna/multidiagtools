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
#include "QtLibraryTest.h"
#include "Mdt/DeployUtils/QtLibrary.h"
#include "Mdt/PlainText/TestUtils.h"

using namespace Mdt::DeployUtils;
using namespace Mdt::PlainText;

#ifndef PREFIX_PATH
 #error "PREFIX_PATH missing"
#endif

void QtLibraryTest::initTestCase()
{
}

void QtLibraryTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void QtLibraryTest::getQtLibrariesTest()
{
  QFETCH(StringRecordList, libraries);
  QFETCH(StringRecordList, expectedQtLibraries);

  const auto librariesInfoList = libraryInfoListFromRecordList(libraries);
  const auto qtLibrariesInfoList = QtLibrary::getQtLibraries(librariesInfoList);
  const auto qtLibraries = recordListFromLibraryInfoList(qtLibrariesInfoList);
  QCOMPARE(qtLibraries, expectedQtLibraries);
}

void QtLibraryTest::getQtLibrariesTest_data()
{
  QTest::addColumn<StringRecordList>("libraries");
  QTest::addColumn<StringRecordList>("expectedQtLibraries");

  QTest::newRow("0")
    << StringRecordList{}
    << StringRecordList{};

  QTest::newRow("1")
    << StringRecordList{{"A","/lib/libA.so"}}
    << StringRecordList{};

  QTest::newRow("2")
    << StringRecordList{{"Qt5Core","/lib/libQt5Core.so"}}
    << StringRecordList{{"Qt5Core","/lib/libQt5Core.so"}};

  QTest::newRow("3")
    << StringRecordList{{"A","/lib/libA.so"},{"Qt5Core","/lib/libQt5Core.so"}}
    << StringRecordList{{"Qt5Core","/lib/libQt5Core.so"}};

}

void QtLibraryTest::qtModuleListTest()
{
  /*
   * Initial state
   */
  QtModuleList list;
  QCOMPARE(list.count(), 0);
  /*
   * Add modules
   */
  list.addModule(QtModule::Core);
  QCOMPARE(list.count(), 1);
  QCOMPARE(list.at(0), QtModule::Core);
  // A existing module must not be added
  list.addModule(QtModule::Core);
  QCOMPARE(list.count(), 1);
  // Unknown must not be added
  list.addModule(QtModule::Unknown);
  QCOMPARE(list.count(), 1);
  // Add a other module
  list.addModule(QtModule::Sql);
  QCOMPARE(list.count(), 2);
  QCOMPARE(list.at(0), QtModule::Core);
  QCOMPARE(list.at(1), QtModule::Sql);
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.count(), 0);
}

void QtLibraryTest::moduleTest()
{
  QFETCH(QString, qtLibrary);
  QFETCH(QtModule, expectedModule);

  LibraryInfo qtLibraryInfo;
  qtLibraryInfo.setLibraryName(qtLibrary);
  QCOMPARE(QtLibrary::module(qtLibraryInfo), expectedModule);
}

void QtLibraryTest::moduleTest_data()
{
  QTest::addColumn<QString>("qtLibrary");
  QTest::addColumn<QtModule>("expectedModule");

  QTest::newRow("0")
    << ""
    << QtModule::Unknown;

  QTest::newRow("A")
    << "A"
    << QtModule::Unknown;

  QTest::newRow("Qt5Core")
    << "Qt5Core"
    << QtModule::Core;

  QTest::newRow("Qt5Sql")
    << "Qt5Sql"
    << QtModule::Sql;
}

void QtLibraryTest::modulesTest()
{
  QFETCH(StringRecordList, qtLibraries);
  QFETCH(QtModuleList, expectedModules);

  const auto qtLibrariesInfoList = libraryInfoListFromRecordList(qtLibraries);
  const auto modules = QtLibrary::getModules(qtLibrariesInfoList);
  QCOMPARE(modules.count(), expectedModules.count());
  for(int i = 0; i < modules.count(); ++i){
    QCOMPARE(modules.at(i), expectedModules.at(i));
  }
}

void QtLibraryTest::modulesTest_data()
{
  QTest::addColumn<StringRecordList>("qtLibraries");
  QTest::addColumn<QtModuleList>("expectedModules");

  QTest::newRow("0")
    << StringRecordList{}
    << QtModuleList{};

  QTest::newRow("1")
    << StringRecordList{{"libA.so",""}}
    << QtModuleList{};

  QTest::newRow("2")
    << StringRecordList{{"libQt5Core.so",""}}
    << QtModuleList{QtModule::Core};

  QTest::newRow("3")
    << StringRecordList{{"libA.so",""},{"libQt5Core.so",""},{"libQt5Sql.so",""}}
    << QtModuleList{QtModule::Core, QtModule::Sql};
}

void QtLibraryTest::modulesBenchmark()
{
  QFETCH(StringRecordList, qtLibraries);
  QFETCH(QtModuleList, expectedModules);

  const auto qtLibrariesInfoList = libraryInfoListFromRecordList(qtLibraries);
  QtModuleList modules;
  QBENCHMARK{
    modules = QtLibrary::getModules(qtLibrariesInfoList);
  }
  QCOMPARE(modules.count(), expectedModules.count());
  for(int i = 0; i < modules.count(); ++i){
    QCOMPARE(modules.at(i), expectedModules.at(i));
  }
}

void QtLibraryTest::modulesBenchmark_data()
{
  QTest::addColumn<StringRecordList>("qtLibraries");
  QTest::addColumn<QtModuleList>("expectedModules");

  QTest::newRow("1")
    << StringRecordList{{"libQt5Core.so",""}}
    << QtModuleList{QtModule::Core};

  QTest::newRow("10")
    << StringRecordList{{"libQt5Core.so",""},{"libA.so",""},{"libQt5Widgets.so",""},{"libQt5Gui.so",""},{"libQt5Sql.so",""},
                        {"libB.so",""},{"libQt5Multimedia.so",""},{"libC.so",""},{"libQt5Core.so",""},{"libQt5Qml.so",""}}
    << QtModuleList{QtModule::Core, QtModule::Widgets, QtModule::Gui, QtModule::Sql, QtModule::Multimedia, QtModule::Qml};
}

void QtLibraryTest::pluginsDirectoriesTest()
{
  QFETCH(QtModuleList, modules);
  QFETCH(QStringList, expectedDirectories);

  QCOMPARE(QtLibrary::getPluginsDirectories(modules), expectedDirectories);
}

void QtLibraryTest::pluginsDirectoriesTest_data()
{
  QTest::addColumn<QtModuleList>("modules");
  QTest::addColumn<QStringList>("expectedDirectories");

  QTest::newRow("0")
    << QtModuleList{}
    << QStringList{};

  QTest::newRow("1")
    << QtModuleList{QtModule::Unknown}
    << QStringList{};

  QTest::newRow("QtCore")
    << QtModuleList{QtModule::Core}
    << QStringList{"platforms"};

  QTest::newRow("QtGui")
    << QtModuleList{QtModule::Gui}
    << QStringList{"accessiblebridge","imageformats","pictureformats","platforminputcontexts","platformthemes"};

  QTest::newRow("QtWidgets")
    << QtModuleList{QtModule::Widgets}
    << QStringList{"accessible", "styles"};

  QTest::newRow("QtSql")
    << QtModuleList{QtModule::Sql}
    << QStringList{"sqldrivers"};
}

void QtLibraryTest::findLibrariesPluginsTest()
{
  QtLibrary qtLibrary;
  PathList searchFirstPrefixPaths = PathList::fromStringList( QString(PREFIX_PATH).split(';', QString::SkipEmptyParts) );
  LibraryInfo qtGui;
  LibraryInfoList qtLibraries;

  qDebug() << "Prefixes: " << searchFirstPrefixPaths.toStringList();

  qtGui.setLibraryPlatformName("Qt5Gui");
  qtLibraries.addLibrary(qtGui);
  const auto expPlugins = qtLibrary.findLibrariesPlugins(qtLibraries, searchFirstPrefixPaths);
  QVERIFY(expPlugins.hasValue());
  QVERIFY(!expPlugins.value().isEmpty());
}

/*
 * Helpers
 */

LibraryInfoList QtLibraryTest::libraryInfoListFromRecordList(const StringRecordList& recordList)
{
  LibraryInfoList libraries;

  for(const auto & record : recordList){
    Q_ASSERT(record.columnCount() == 2);
    LibraryInfo li;
    li.setLibraryName(record.data(0));
    li.setAbsoluteFilePath(record.data(1));
    libraries.addLibrary(li);
  }

  return libraries;
}

StringRecordList QtLibraryTest::recordListFromLibraryInfoList(const LibraryInfoList& libraries)
{
  StringRecordList recordList;

  for(const auto & library : libraries){
    StringRecord record;
    record.setColumnCount(2);
    record.setData(0, library.libraryName().name());
    record.setData(1, library.absoluteFilePath());
    recordList.appendRecord(record);
  }

  return recordList;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  QtLibraryTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
