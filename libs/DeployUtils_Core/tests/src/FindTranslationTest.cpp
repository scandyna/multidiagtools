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
#include "Mdt/DeployUtils/QtModule.h"
#include "Mdt/DeployUtils/QtModuleList.h"
#include <QString>
#include <QStringList>

#include <QDebug>

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
  QFETCH(QString, expectedTranslationPath);
  /*
   * Setup a fake root
   */
  QTemporaryDir root;
  QVERIFY(root.isValid());
  QVERIFY( createPathListInFakeRoot(root, existingPaths) );
  prependFakeRootToPathList(root, pathPrefixList);
  prependFakeRootToPath(root, expectedTranslationPath);
  QVERIFY( createQmFiles(expectedTranslationPath, "", {"qtbase"}, {"xx"}) );
  qDebug() << "Test: TR path: " << expectedTranslationPath;
  qDebug() << " - files: " << QDir(expectedTranslationPath).entryList();
  /*
   * Check
   */
  QCOMPARE( findQtTranslationsRoot(pathPrefixList), expectedTranslationPath );
}

void FindTranslationTest::findQtTranslationsRootTest_data()
{
  QTest::addColumn<PathList>("pathPrefixList");
  QTest::addColumn<QStringList>("existingPaths");
  QTest::addColumn<QString>("expectedTranslationPath");

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

  QTest::newRow("NonQt") << PathList{"/opt/nonqtroot","/opt/qtroot"}
                         << QStringList{"/opt/nonqtroot/translations","/opt/qtroot/translations"}
                         << "/opt/qtroot/translations";

}

void FindTranslationTest::getQmFileBaseNamesForQtModuleTest()
{
  QFETCH(QtModule, qtModule);
  QFETCH(QStringList, expectedQmFileBaseNames);

  QCOMPARE( getQmFileBaseNameListForQtModule(qtModule), expectedQmFileBaseNames );
}

void FindTranslationTest::getQmFileBaseNamesForQtModuleTest_data()
{
  QTest::addColumn<QtModule>("qtModule");
  QTest::addColumn<QStringList>("expectedQmFileBaseNames");

  QTest::newRow("Qt5Core") << QtModule::Core
                           << QStringList{"qt","qtbase"};
}

void FindTranslationTest::findQtTranslationsTest()
{
  QFETCH(QtModuleList, qtModules);
  QFETCH(QStringList, languageSuffixes);
  QFETCH(QStringList, expectedQmFiles);
  /*
   * Create a directory with Qt translatiosn files
   */
  QTemporaryDir qtRoot;
  QVERIFY(qtRoot.isValid());
  QVERIFY(createQmFiles(qtRoot, "translations", {"qt","qtbase","qtconfig"},{"en","fr","de","it"}));
  /*
   * Check
   */
  PathList pathPrefixList{"/"};
  prependFakeRootToPathList(qtRoot, pathPrefixList);
  const auto translations = findQtTranslations(qtModules, languageSuffixes, pathPrefixList);
  QVERIFY(translations);
  QCOMPARE((*translations).count(), expectedQmFiles.count());
  for(const auto & translation : *translations){
    QVERIFY(!translation.absoluteFilePath().isEmpty());
    QVERIFY(!translation.fullFileName().isEmpty());
    QVERIFY(expectedQmFiles.contains(translation.fullFileName()));
  }
}

void FindTranslationTest::findQtTranslationsTest_data()
{
  QTest::addColumn<QtModuleList>("qtModules");
  QTest::addColumn<QStringList>("languageSuffixes");
  QTest::addColumn<QStringList>("expectedQmFiles");

  QTest::newRow("1") << QtModuleList{QtModule::Core}
                     << QStringList{"en"}
                     << QStringList{"qt_en.qm","qtbase_en.qm"};

  QTest::newRow("2") << QtModuleList{QtModule::Core}
                     << QStringList{"en","fr"}
                     << QStringList{"qt_en.qm","qt_fr.qm","qtbase_en.qm","qtbase_fr.qm"};
}

void FindTranslationTest::findMdtTranslationsRootTest()
{
  QFETCH(PathList, pathPrefixList);
  QFETCH(QStringList, existingPaths);
  QFETCH(QString, expectedTranslationPath);
  /*
   * Setup a fake root
   */
  QTemporaryDir root;
  QVERIFY(root.isValid());
  QVERIFY( createPathListInFakeRoot(root, existingPaths) );
  prependFakeRootToPathList(root, pathPrefixList);
  prependFakeRootToPath(root, expectedTranslationPath);
  QVERIFY( createQmFiles(expectedTranslationPath, "", {"Error_Core"}, {"xx"}) );
  /*
   * Check
   */
  QCOMPARE( findMdtTranslationsRoot(pathPrefixList), expectedTranslationPath );
}

void FindTranslationTest::findMdtTranslationsRootTest_data()
{
  QTest::addColumn<PathList>("pathPrefixList");
  QTest::addColumn<QStringList>("existingPaths");
  QTest::addColumn<QString>("expectedTranslationPath");

  QTest::newRow("!sys") << PathList{"/opt/mdtroot"}
                        << QStringList{"/opt/mdtroot/translations"}
                        << "/opt/mdtroot/translations";

  QTest::newRow("NonMdt") << PathList{"/opt/nonmdtroot","/opt/mdtroot"}
                          << QStringList{"/opt/nonmdtroot/translations","/opt/mdtroot/translations"}
                          << "/opt/mdtroot/translations";
}

void FindTranslationTest::findMdtTranslationsTest()
{
  QFETCH(QStringList, mdtLibrariesNames);
  QFETCH(QStringList, languageSuffixes);
  QFETCH(QStringList, expectedQmFiles);
  /*
   * Create a directory with Mdt translatiosn files
   */
  QTemporaryDir mdtRoot;
  QVERIFY(mdtRoot.isValid());
  QVERIFY(createQmFiles(mdtRoot, "translations", {"Application_Core","Application_Core_Single","Application_Widgets","DeployUtils_Core","Error_Core"},{"en","fr","de","it"}));
  /*
   * Create a list of library infos based on names
   */
  const auto mdtLibraries = buildLibraryInfoList("/tmp", mdtLibrariesNames);
  /*
   * Check
   */
  PathList pathPrefixList{"/"};
  prependFakeRootToPathList(mdtRoot, pathPrefixList);
  const auto translations = findMdtTranslations(mdtLibraries, languageSuffixes, pathPrefixList);
  QVERIFY(translations);
  QCOMPARE((*translations).count(), expectedQmFiles.count());
  for(const auto & translation : *translations){
    QVERIFY(!translation.absoluteFilePath().isEmpty());
    QVERIFY(!translation.fullFileName().isEmpty());
    QVERIFY(expectedQmFiles.contains(translation.fullFileName()));
  }
}

void FindTranslationTest::findMdtTranslationsTest_data()
{
  QTest::addColumn<QStringList>("mdtLibrariesNames");
  QTest::addColumn<QStringList>("languageSuffixes");
  QTest::addColumn<QStringList>("expectedQmFiles");

  QTest::newRow("1") << QStringList{"libMdt0Application_Core.so"}
                     << QStringList{"en"}
                     << QStringList{"Application_Core_en.qm"};
}

/*
 * Helpers
 */

bool FindTranslationTest::createQmFiles(const QString& directoryPath, const QString & subDirectory, const QStringList& baseNames, const QStringList& languageSuffixes)
{
  for(const auto & baseName : baseNames){
    for(const auto & languageSuffixe : languageSuffixes){
      const auto fileName = baseName + "_" + languageSuffixe + ".qm";
      const auto filePath = QDir::cleanPath( directoryPath + "/" + subDirectory + "/" + fileName );
      if(!createFile(filePath)){
        return false;
      }
    }
  }
  return true;
}

bool FindTranslationTest::createQmFiles(const QTemporaryDir& directory, const QString & subDirectory, const QStringList& baseNames, const QStringList& languageSuffixes)
{
  return createQmFiles(directory.path(), subDirectory, baseNames, languageSuffixes);
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
