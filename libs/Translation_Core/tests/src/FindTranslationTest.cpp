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
#include "Mdt/Translation/FindTranslation.h"
#include "Mdt/Translation/TranslationInfo.h"
#include "Mdt/Translation/TranslationInfoList.h"
#include "Mdt/FileSystem/PathList.h"
#include "Mdt/TestLib/FakeRoot.h"
#include "Mdt/TestLib/CreateFile.h"
#include <QString>
#include <QStringList>
#include <QTemporaryDir>

#include <QDebug>

using namespace Mdt::Translation;
using namespace Mdt::FileSystem;
using namespace Mdt::TestLib;

void FindTranslationTest::initTestCase()
{
}

void FindTranslationTest::cleanupTestCase()
{
}

/*
 * Global helpers
 */

void prependFakeRootToPathList(const QTemporaryDir & root, Mdt::FileSystem::PathList & pathList)
{
  auto tmp = pathList.toStringList();
  prependFakeRootToPathList(root, tmp);
  pathList = PathList::fromStringList(tmp);
}

/*
 * Tests
 */

void FindTranslationTest::isTranslationDirectoryTest()
{
  /*
   * Create a fake filesystem
   */
  QTemporaryDir root;
  QVERIFY(root.isValid());
  const auto nonTranslationDirectory = pathWithFakeRoot(root, "nonTranslationDirectory");
  const auto someTranslationDirectory = pathWithFakeRoot(root, "someTranslationDirectory");
  const auto qtTranslationDirectory = pathWithFakeRoot(root, "qtTranslationDirectory");
  QVERIFY(createFileInDirectory(someTranslationDirectory, "app_en.qm"));
  QVERIFY(createFileInDirectory(qtTranslationDirectory, "qtbase_en.qm"));
  /*
   * Check
   */
  QVERIFY( !isTranslationDirectory(nonTranslationDirectory) );
  QVERIFY( !isTranslationDirectory(nonTranslationDirectory, "app") );
  QVERIFY( isTranslationDirectory(someTranslationDirectory) );
  QVERIFY( isTranslationDirectory(qtTranslationDirectory, "qtbase") );
  QVERIFY( !isTranslationDirectory(qtTranslationDirectory, "app") );
}

void FindTranslationTest::findTranslationsRootTest()
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
  QVERIFY( createQmFiles(expectedTranslationPath, "", {"app"}, {"xx"}) );
  qDebug() << "Test: TR path: " << expectedTranslationPath;
  qDebug() << " - files: " << QDir(expectedTranslationPath).entryList();
  /*
   * Check
   */
  QCOMPARE( findTranslationsRoot(pathPrefixList), expectedTranslationPath );
}

void FindTranslationTest::findTranslationsRootTest_data()
{
  QTest::addColumn<PathList>("pathPrefixList");
  QTest::addColumn<QStringList>("existingPaths");
  QTest::addColumn<QString>("expectedTranslationPath");

  QTest::newRow(".") << PathList{"."}
                     << QStringList{"translations"}
                     << "translations";

  QTest::newRow("/opt/app") << PathList{"/opt/app"}
                            << QStringList{"/opt/app/translations"}
                            << "/opt/app/translations";

  QTest::newRow("/opt/app") << PathList{"/opt/otherApp","/opt/app"}
                            << QStringList{"/opt/otherApp/translations","/opt/app/translations"}
                            << "/opt/app/translations";
}

void FindTranslationTest::findTranslationsTest()
{
  /*
   * Create a fake filesystem
   */
  QTemporaryDir root;
  QVERIFY(root.isValid());
  const auto translationDirectory = pathWithFakeRoot(root, "translations");
  QVERIFY(createFileInDirectory(translationDirectory, "app_en.qm"));
  QVERIFY(createFileInDirectory(translationDirectory, "app_fr.qm"));
  /*
   * Check
   */
  PathList pathPrefixList{"."};
  prependFakeRootToPathList(root, pathPrefixList);
  const auto translations = findTranslations(pathPrefixList);
  QVERIFY(translations);
  QCOMPARE((*translations).count(), 2);
  QVERIFY(!(*translations).at(0).absoluteFilePath().isEmpty());
  QVERIFY(!(*translations).at(0).fullFileName().isEmpty());
  QCOMPARE((*translations).at(0).fullFileName(), QString("app_en.qm"));
  QVERIFY(!(*translations).at(1).absoluteFilePath().isEmpty());
  QVERIFY(!(*translations).at(1).fullFileName().isEmpty());
  QCOMPARE((*translations).at(1).fullFileName(), QString("app_fr.qm"));
}

/*
 * Member helpers
 */

bool FindTranslationTest::createQmFiles(const QString & directoryPath, const QString & subDirectory, const QStringList & baseNames, const QStringList & languageSuffixes)
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

bool FindTranslationTest::createQmFiles(const QTemporaryDir & directory, const QString & subDirectory, const QStringList & baseNames, const QStringList & languageSuffixes)
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
