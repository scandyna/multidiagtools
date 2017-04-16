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
#include "WindowTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/ItemEditor/StandardWindow.h"
#include "Mdt/ItemEditor/TableViewController.h"
#include <QSignalSpy>
#include <QItemSelectionModel>
#include <QStringListModel>
#include <QTableView>
#include <QPointer>
#include <QLineEdit>
#include <QLabel>

using namespace Mdt::ItemModel;

/*
 * AbstractWindowTester implementation
 */

AbstractWindowTester::AbstractWindowTester(QWidget* parent)
 : AbstractWindow(parent)
{
}

NavigationActions* AbstractWindowTester::navigationActions()
{
  return AbstractWindow::navigationActions();
}

EditionActions* AbstractWindowTester::editionActions()
{
  return AbstractWindow::editionActions();
}

InsertAction* AbstractWindowTester::insertAction()
{
  return AbstractWindow::insertAction();
}

RemoveAction* AbstractWindowTester::removeAction()
{
  return AbstractWindow::removeAction();
}


/*
 * Init/cleanup
 */

void WindowTest::initTestCase()
{
}

void WindowTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void WindowTest::initActionsTest()
{
  AbstractWindowTester window;
  /*
   * First time a action getter is called,
   *  the action(s) is created
   */
  QVERIFY(window.navigationActions() != nullptr);
  QVERIFY(window.editionActions() != nullptr);
  QVERIFY(window.insertAction() != nullptr);
  QVERIFY(window.editionActions() != nullptr);
}

void WindowTest::setupControllerBeforeSetTest()
{
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(5, 1);
  model.populateColumn(0, {1,2,3,4,5});
  /*
   * Setup controller
   */
  TableViewController controller;
  QTableView view;
  controller.setModel(&model);
  controller.setView(&view);
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Setup window
   * Call each actions to init them
   * (only those that are initialized are later handled)
   */
  AbstractWindowTester window;
  auto *navigationActions = window.navigationActions();
  auto *editionActions = window.editionActions();
  auto *insertAction = window.insertAction();
  auto *removeAction = window.removeAction();
  window.setMainController(&controller);
  /*
   * Check that initial state was propagated to actions
   */
  QVERIFY(navigationActions->toNext()->isEnabled());
  QVERIFY(!editionActions->submitAction()->isEnabled());
  QVERIFY(insertAction->insertAction()->isEnabled());
  QVERIFY(removeAction->removeAction()->isEnabled());
}

void WindowTest::setupControllerAfterSetTest()
{
  TableViewController controller;
  QTableView view;
  /*
   * Setup window
   * Call each actions to init them
   * (only those that are initialized are latr handled)
   */
  AbstractWindowTester window;
  auto *navigationActions = window.navigationActions();
  auto *editionActions = window.editionActions();
  auto *insertAction = window.insertAction();
  auto *removeAction = window.removeAction();
  window.setMainController(&controller);
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(5, 1);
  model.populateColumn(0, {1,2,3,4,5});
  /*
   * Setup controller
   */
  controller.setModel(&model);
  controller.setView(&view);
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Check that initial state was propagated to actions
   */
  QVERIFY(navigationActions->toNext()->isEnabled());
  QVERIFY(!editionActions->submitAction()->isEnabled());
  QVERIFY(insertAction->insertAction()->isEnabled());
  QVERIFY(removeAction->removeAction()->isEnabled());
}

void WindowTest::standardWindowTest()
{
  StandardWindow window;

  window.show();

  /*
   * Play
   */
//   while(window.isVisible()){
//     QTest::qWait(500);
//   }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  WindowTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
