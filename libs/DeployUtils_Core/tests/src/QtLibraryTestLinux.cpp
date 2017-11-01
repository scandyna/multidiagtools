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
#include "QtLibraryTestLinux.h"
#include "Mdt/DeployUtils/QtLibrary.h"
#include "Mdt/DeployUtils/PathList.h"
#include <QString>
#include <QStringList>
#include <QTemporaryDir>
#include <QDir>

// #include <QDebug>

using namespace Mdt::DeployUtils;

void QtLibraryTestLinux::initTestCase()
{
}

void QtLibraryTestLinux::cleanupTestCase()
{
}

/*
 * Tests
 */

void QtLibraryTestLinux::findPluginsRootTest()
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

void QtLibraryTestLinux::findPluginsRootTest_data()
{
  QTest::addColumn<PathList>("pathPrefixList");
  QTest::addColumn<QStringList>("existingPaths");
  QTest::addColumn<QString>("expectedPlginsRoot");

  QTest::newRow("OtherQt") << PathList{"/opt/qtroot"}
                           << QStringList{"/opt/qtroot/plugins"}
                           << "/opt/qtroot/plugins";

  QTest::newRow("MXE") << PathList{"/opt/mxeroot"}
                       << QStringList{"/opt/mxeroot/qt5/plugins"}
                       << "/opt/mxeroot/qt5/plugins";

//   QTest::newRow("sys") << PathList{}
//                        << QStringList{"/usr/lib/x86_64-linux-gnu/qt5/plugins"}
//                        << "/usr/lib/x86_64-linux-gnu/qt5/plugins";

  QTest::newRow("OtherQt,!sys") << PathList{"/opt/qtroot"}
                           << QStringList{"/opt/qtroot/plugins","/usr/lib/x86_64-linux-gnu/qt5/plugins"}
                           << "/opt/qtroot/plugins";

  QTest::newRow("OtherQt,..,!sys") << PathList{"/opt/qtroot/bin"}
                                   << QStringList{"/opt/qtroot/plugins","/usr/lib/x86_64-linux-gnu/qt5/plugins"}
                                   << "/opt/qtroot/plugins";

  QTest::newRow("OtherQt,..,!sys,2") << PathList{"/opt/qttoolroot/bin","/opt/qtroot/bin"}
                                     << QStringList{"/opt/qtroot/plugins","/usr/lib/x86_64-linux-gnu/qt5/plugins"}
                                     << "/opt/qtroot/plugins";

}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  QtLibraryTestLinux test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
