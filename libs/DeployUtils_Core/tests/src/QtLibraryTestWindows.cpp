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
#include "Mdt/DeployUtils/QtLibrary.h"
#include "Mdt/DeployUtils/PathList.h"
#include <QString>
#include <QStringList>
#include <QTemporaryDir>
#include <QDir>

// #include <QDebug>

using namespace Mdt::DeployUtils;

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

}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  QtLibraryTestWindows test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
