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
#include <QMainWindow>
#include <QMenuBar>

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

  auto *lsm = new LanguageSelectionMenu(&window);
  window.menuBar()->addMenu(lsm);

  showWidgetAndWait(window);
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
