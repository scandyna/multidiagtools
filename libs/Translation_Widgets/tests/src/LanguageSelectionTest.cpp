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
#include <QWidget>
#include <QAction>
#include <QString>

using namespace Mdt::Translation;

void LanguageSelectionTest::initTestCase()
{
}

void LanguageSelectionTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void LanguageSelectionTest::findTranslationsTest()
{
  /*
   * We have a directory 'translations' in our build tree
   */
  LanguageSelection ls;
  QVERIFY(ls.findTranslations());
}

void LanguageSelectionTest::createLanguageSelectionActionsTest()
{
  QWidget parent;
  LanguageSelection ls;
  QVERIFY(ls.languageSelectionActions().isEmpty());
  QVERIFY(ls.findTranslations());
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
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  LanguageSelectionTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
