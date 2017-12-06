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
#include "QtToolWrapperTest.h"
#include "Mdt/FileSystem/PathList.h"
#include "Mdt/DeployUtils/QtToolExecutableWrapper.h"
#include <QString>
#include <QStringList>

using namespace Mdt::DeployUtils;
using namespace Mdt::FileSystem;

void QtToolWrapperTest::initTestCase()
{
}

void QtToolWrapperTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void QtToolWrapperTest::findQtBinPathTest()
{
  QFETCH(PathList, pathPrefixList);
  QFETCH(QStringList, existingPaths);
  QFETCH(QString, expectedQtBinPath);
  /*
   * Setup a fake root
   */
  QTemporaryDir root;
  QVERIFY(root.isValid());
  QVERIFY( createPathListInFakeRoot(root, existingPaths) );
  prependFakeRootToPathList(root, pathPrefixList);
  if(!expectedQtBinPath.isEmpty()){
    prependFakeRootToPath(root, expectedQtBinPath);
    QVERIFY( createFileInDirectory(expectedQtBinPath, "lconvert") );
  }
  qDebug() << " - files: " << QDir(expectedQtBinPath).entryList();
  /*
   * Check
   */
  qDebug() << "expectedQtBinPath: " << expectedQtBinPath;
  QtToolExecutableWrapper wrapper;
  if(expectedQtBinPath.isEmpty()){
    QVERIFY( !wrapper.findQtBinPath(pathPrefixList) );
    QVERIFY( wrapper.qtBinPath().isEmpty() );
  }else{
    QVERIFY( wrapper.findQtBinPath(pathPrefixList) );
    QCOMPARE( wrapper.qtBinPath(), expectedQtBinPath );
  }
  // Check state change coherance
  QVERIFY( !wrapper.findQtBinPath(PathList()) );
  QVERIFY( wrapper.qtBinPath().isEmpty() );
}

void QtToolWrapperTest::findQtBinPathTest_data()
{
  QTest::addColumn<PathList>("pathPrefixList");
  QTest::addColumn<QStringList>("existingPaths");
  QTest::addColumn<QString>("expectedQtBinPath");

  QTest::newRow("NonSysQt") << PathList{"/opt/qtroot"}
                            << QStringList{"/opt/qtroot/bin"}
                            << "/opt/qtroot/bin";

  QTest::newRow("MXE") << PathList{"/opt/mxeroot"}
                       << QStringList{"/opt/mxeroot/qt5/bin"}
                       << "/opt/mxeroot/qt5/bin";

  QTest::newRow("OthetLib") << PathList{"/opt/mdtroot","/opt/qtroot"}
                            << QStringList{"/opt/mdtroot/bin","/opt/qtroot/bin"}
                            << "/opt/qtroot/bin";

  QTest::newRow("SysQt") << PathList{"/opt/mdtroot"}
                            << QStringList{"/opt/mdtroot/bin"}
                            << "";
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  QtToolWrapperTest test;

//   app.debugEnvironment();

  return QTest::qExec(&test, argc, argv);
}
