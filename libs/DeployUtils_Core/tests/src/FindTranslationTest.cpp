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
#include "FindTranslationTest.h"
#include "Mdt/DeployUtils/FindTranslation.h"
#include "Mdt/DeployUtils/PathList.h"
#include <QString>
#include <QStringList>

using namespace Mdt::DeployUtils;

void FindTranslationTest::initTestCase()
{
}

void FindTranslationTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void FindTranslationTest::findQtTranslationsRootTest()
{
  QFETCH(PathList, pathPrefixList);
  QFETCH(QStringList, existingPaths);
  QFETCH(QString, expectedTranslationRoot);
  /*
   * Setup a fake root
   */
  QTemporaryDir root;
  QVERIFY(root.isValid());
  QVERIFY( createPathListInFakeRoot(root, existingPaths) );
  prependFakeRootToPathList(root, pathPrefixList);
  prependFakeRootToPath(root, expectedTranslationRoot);
  /*
   * Check
   */
  QCOMPARE( findQtTranslationsRoot(pathPrefixList), expectedTranslationRoot );
}

void FindTranslationTest::findQtTranslationsRootTest_data()
{
  QTest::addColumn<PathList>("pathPrefixList");
  QTest::addColumn<QStringList>("existingPaths");
  QTest::addColumn<QString>("expectedTranslationRoot");

  QTest::newRow("OtherQt") << PathList{"/opt/qtroot"}
                           << QStringList{"/opt/qtroot/translations"}
                           << "/opt/qtroot/translations";

  QTest::newRow("MXE") << PathList{"/opt/mxeroot"}
                       << QStringList{"/opt/mxeroot/qt5/translations"}
                       << "/opt/mxeroot/qt5/translations";

  QTest::newRow("OtherQt,!sys") << PathList{"/opt/qtroot"}
                                << QStringList{"/opt/qtroot/translations","/usr/share/qt5/translations"}
                                << "/opt/qtroot/translations";

  QTest::newRow("OtherQt,..,!sys") << PathList{"/opt/qtroot/bin"}
                                   << QStringList{"/opt/qtroot/translations","/usr/share/qt5/translations"}
                                   << "/opt/qtroot/translations";

  QTest::newRow("OtherQt,..,!sys,2") << PathList{"/opt/qttoolroot/bin","/opt/qtroot/bin"}
                                     << QStringList{"/opt/qtroot/translations","/usr/share/qt5/translations"}
                                     << "/opt/qtroot/translations";

//   QTest::newRow("NonQt") << PathList{"/opt/nonqtroot","/opt/qtroot"}
//                          << QStringList{"/opt/nonqtroot/translations","/opt/qtroot/translations"}
//                          << "/opt/qtroot/translations";

}

void FindTranslationTest::findMdtTranslationsRootTest()
{
  QFETCH(PathList, pathPrefixList);
  QFETCH(QStringList, existingPaths);
  QFETCH(QString, expectedTranslationRoot);
  /*
   * Setup a fake root
   */
  QTemporaryDir root;
  QVERIFY(root.isValid());
  QVERIFY( createPathListInFakeRoot(root, existingPaths) );
  prependFakeRootToPathList(root, pathPrefixList);
  prependFakeRootToPath(root, expectedTranslationRoot);
  /*
   * Check
   */
  QCOMPARE( findMdtTranslationsRoot(pathPrefixList), expectedTranslationRoot );
}

void FindTranslationTest::findMdtTranslationsRootTest_data()
{
  QTest::addColumn<PathList>("pathPrefixList");
  QTest::addColumn<QStringList>("existingPaths");
  QTest::addColumn<QString>("expectedTranslationRoot");

  QTest::newRow("!sys") << PathList{"/opt/mdtroot"}
                        << QStringList{"/opt/mdtroot/translations"}
                        << "/opt/mdtroot/translations";

}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  FindTranslationTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
