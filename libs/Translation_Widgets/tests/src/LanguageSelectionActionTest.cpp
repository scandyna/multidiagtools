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
#include "LanguageSelectionActionTest.h"
#include "Mdt/Translation/LanguageSelectionActionList.h"
#include "Mdt/Translation/LanguageCodeList.h"
#include <QWidget>
#include <QAction>
#include <QString>

using namespace Mdt::Translation;

void LanguageSelectionActionTest::initTestCase()
{
}

void LanguageSelectionActionTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void LanguageSelectionActionTest::createActionsTest()
{
  LanguageSelectionActionList list;
  QCOMPARE(list.actions().count(), 0);

  QWidget parent;
  LanguageCodeList languages;
  languages.addLanguageCode(LanguageCode("en"));
  languages.addLanguageCode(LanguageCode("fr"));
  list.createActions(languages, &parent);
  QCOMPARE(list.actions().count(), 2);
  QCOMPARE(list.actions().at(0)->text(), QString("English"));
  QCOMPARE(list.actions().at(1)->text(), QString("French"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  LanguageSelectionActionTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
