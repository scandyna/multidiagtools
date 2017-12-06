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
#include "LanguageSelectionMenuTest.h"
#include "Mdt/Translation/LanguageSelectionMenu.h"
#include "Mdt/Translation/LanguageCodeList.h"
#include "Mdt/Translation/LanguageSelectionActionList.h"
#include <QMainWindow>
#include <QMenuBar>
#include <QWidget>
#include <QString>
#include <QAction>
#include <QList>

#include <QToolBar>
#include "Mdt/Translation/FindTranslation.h"
#include "Mdt/Translation/TranslationInfoList.h"
#include "Mdt/FileSystem/PathList.h"
using namespace Mdt::FileSystem;

using namespace Mdt::Translation;

void LanguageSelectionMenuTest::initTestCase()
{
}

void LanguageSelectionMenuTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void LanguageSelectionMenuTest::sandbox()
{
  QMainWindow window;
  const auto translations = findTranslations(PathList{"/home/philippe/opt/qt/Qt5/5.9.1/gcc_64/"});
  QVERIFY(translations);
  const auto languages = (*translations).toLanguageCodeList();
  LanguageSelectionActionList actions;
  actions.createActions(languages, &window);
  auto *lsm = new LanguageSelectionMenu(&window);
  lsm->setAvailableLanguages(actions);
  window.menuBar()->addMenu(lsm);

  auto *ltb = new QToolBar(&window);
  ltb->addActions(actions.actions());
  window.addToolBar(ltb);

  showWidgetAndWait(window);
}

void LanguageSelectionMenuTest::setAvailableLaguagesTest()
{
  LanguageCodeList languages;
  LanguageSelectionMenu menu;

  languages.addLanguageCode(LanguageCode("en"));
  languages.addLanguageCode(LanguageCode("fr"));
  languages.addLanguageCode(LanguageCode("de"));
  ///menu.setAvailableLanguages(languages);
  QCOMPARE(menu.actions().count(), 3);
  QCOMPARE(menu.actions().at(0)->text(), QString("English"));
  QCOMPARE(menu.actions().at(0)->text(), QString("French"));
  QCOMPARE(menu.actions().at(0)->text(), QString("German"));

  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  LanguageSelectionMenuTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
