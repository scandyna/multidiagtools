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
#include "LanguageSelectionTest.h"
#include "Mdt/Translation/LanguageSelection.h"
#include "Mdt/Translation/LanguageCodeList.h"
#include "Mdt/FileSystem/PathList.h"
#include "Mdt/TestLib/CreateFile.h"
#include "Mdt/TestLib/FakeRoot.h"
#include <QTemporaryDir>
#include <QWidget>
#include <QAction>
#include <QString>

using namespace Mdt::Translation;
using namespace Mdt::FileSystem;
using namespace Mdt::TestLib;

void LanguageSelectionTest::initTestCase()
{
}

void LanguageSelectionTest::cleanupTestCase()
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

void LanguageSelectionTest::findTranslationsTest()
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
  LanguageSelection ls;
  QVERIFY(ls.findTranslations(pathPrefixList));
}

void LanguageSelectionTest::createLanguageSelectionActionsTest()
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
  QWidget parent;
  LanguageSelection ls;
  QVERIFY(ls.languageSelectionActions().isEmpty());
  QVERIFY(ls.findTranslations(pathPrefixList));
  ls.createLanguageSelectionActions(&parent);
  const auto actions = ls.languageSelectionActions();
  QVERIFY(!actions.isEmpty());
  for(const auto *action : actions){
    QVERIFY(action != nullptr);
    QVERIFY(!action->text().isEmpty());
    QVERIFY(action->parent() == &parent);
  }
  QVERIFY(!ls.findTranslations());
}

/*
 * Member helpers
 */

bool LanguageSelectionTest::createQmFiles(const QString& directoryPath, const QString& subDirectory, const QStringList& baseNames, const QStringList& languageSuffixes)
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

bool LanguageSelectionTest::createQmFiles(const QTemporaryDir& directory, const QString& subDirectory, const QStringList& baseNames, const QStringList& languageSuffixes)
{
  return createQmFiles(directory.path(), subDirectory, baseNames, languageSuffixes);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  LanguageSelectionTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
