/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "ActionsTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/RowState.h"
#include "Mdt/ItemEditor/AbstractActionContainer.h"
#include "Mdt/ItemEditor/NavigationActions.h"
#include "Mdt/ItemEditor/EditionActions.h"
#include "Mdt/ItemEditor/InsertAction.h"
#include "Mdt/ItemEditor/RemoveAction.h"
#include "Mdt/ItemEditor/ControllerState.h"
#include <QSignalSpy>
#include <QItemSelectionModel>
#include <QStringListModel>
#include <QTableView>
#include <QPointer>
#include <QAction>

#include <QDebug>

namespace ItemEditor = Mdt::ItemEditor;
using ItemEditor::RowState;
using ItemEditor::ControllerState;

void ActionsTest::initTestCase()
{
}

void ActionsTest::cleanupTestCase()
{
}

/*
 * ActionContainerTester implementation
 */

ActionContainerTester::ActionContainerTester(QObject* parent)
 : AbstractActionContainer(parent)
{
}

/*
 * Tests
 */

void ActionsTest::abstractActionContainerTest()
{
  RowState rs;
  /*
   * Initial state
   */
  ActionContainerTester act;
  QCOMPARE(act.rowCount(), 0);
  QCOMPARE(act.currentRow(), -1);
  QVERIFY(act.rowStateIsNull());
  QVERIFY(act.controllerState() == ControllerState::Visualizing);
  QCOMPARE(act.stateChangedCount(), 0);
  QVERIFY(!act.disableForced());
  /*
   * Check setters and getters
   */
  rs.setRowCount(20);
  rs.setCurrentRow(10);
  act.setRowState(rs);
  act.setControllerState(ControllerState::Editing);
  QCOMPARE(act.rowCount(), 20);
  QCOMPARE(act.currentRow(), 10);
  QVERIFY(!act.rowStateIsNull());
  QVERIFY(act.controllerState() == ControllerState::Editing);
  /*
   * Go back to initial state
   */
  rs.clear();
  act.setRowState(rs);
  act.setControllerState(ControllerState::Visualizing);
  act.resetStateChangedCount();
  QCOMPARE(act.rowCount(), 0);
  QCOMPARE(act.currentRow(), -1);
  QVERIFY(act.controllerState() == ControllerState::Visualizing);
  QCOMPARE(act.stateChangedCount(), 0);
  QVERIFY(!act.disableForced());
  /*
   * Check changing each property
   */
  rs.clear();
  // Change row count
  rs.setRowCount(1);
  act.setRowState(rs);
  QCOMPARE(act.stateChangedCount(), 1);
  // Change current row
  rs.setCurrentRow(0);
  act.setRowState(rs);
  QCOMPARE(act.stateChangedCount(), 2);
  // Change controller state
  act.setControllerState(ControllerState::Editing);
  QCOMPARE(act.stateChangedCount(), 3);
  /*
   * Go back to initial state
   */
  rs.clear();
  act.setRowState(rs);
  act.setControllerState(ControllerState::Visualizing);
  act.resetStateChangedCount();
  QCOMPARE(act.rowCount(), 0);
  QCOMPARE(act.currentRow(), -1);
  QVERIFY(act.controllerState() == ControllerState::Visualizing);
  QCOMPARE(act.stateChangedCount(), 0);
  QVERIFY(!act.disableForced());
  /*
   * Check changing a property multiple times
   */
  rs.clear();
  // Change row count
  rs.setRowCount(1);
  act.setRowState(rs);
  QCOMPARE(act.stateChangedCount(), 1);
  act.setRowState(rs);
  QCOMPARE(act.stateChangedCount(), 1);
  // Change current row
  rs.setCurrentRow(0);
  act.setRowState(rs);
  QCOMPARE(act.stateChangedCount(), 2);
  act.setRowState(rs);
  QCOMPARE(act.stateChangedCount(), 2);
  // Change controller state
  act.setControllerState(ControllerState::Editing);
  QCOMPARE(act.stateChangedCount(), 3);
  act.setControllerState(ControllerState::Editing);
  QCOMPARE(act.stateChangedCount(), 3);
  /*
   * Go back to initial state
   */
  rs.clear();
  act.setRowState(rs);
  act.setControllerState(ControllerState::Visualizing);
  act.resetStateChangedCount();
  QCOMPARE(act.rowCount(), 0);
  QCOMPARE(act.currentRow(), -1);
  QVERIFY(act.controllerState() == ControllerState::Visualizing);
  QCOMPARE(act.stateChangedCount(), 0);
  QVERIFY(!act.disableForced());
  /*
   * Force disabled
   */
  // Disable all actions
  act.setActionsDisabled(true);
  QVERIFY(act.disableForced());
  QCOMPARE(act.stateChangedCount(), 0);
  rs.clear();
  // Change row count - must not call updateEnableState()
  rs.setRowCount(1);
  act.setRowState(rs);
  QVERIFY(act.disableForced());
  QCOMPARE(act.stateChangedCount(), 0);
  // Change current row - must not call updateEnableState()
  rs.setCurrentRow(0);
  act.setRowState(rs);
  QVERIFY(act.disableForced());
  QCOMPARE(act.stateChangedCount(), 0);
  // Change controller state - must not call updateEnableState()
  act.setControllerState(ControllerState::Editing);
  QVERIFY(act.disableForced());
  QCOMPARE(act.stateChangedCount(), 0);
  // Clear forcing disabled - Must call updateEnableState()
  act.setActionsDisabled(false);
  QVERIFY(!act.disableForced());
  QCOMPARE(act.stateChangedCount(), 1);
}

void ActionsTest::navigationActionsTest()
{
  using Mdt::ItemEditor::NavigationActions;
  using Mdt::ItemEditor::RowState;
  using Mdt::ItemEditor::ControllerState;

  NavigationActions *actions = new NavigationActions(nullptr);
  QPointer<QAction> toFirst = actions->toFirst();
  QPointer<QAction> toPrevious = actions->toPrevious();
  QPointer<QAction> toNext = actions->toNext();
  QPointer<QAction> toLast = actions->toLast();
  RowState rs;
  /*
   * Initial state
   */
  QVERIFY(!toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(!toNext->isEnabled());
  QVERIFY(!toLast->isEnabled());
  /*
   * Check row changes
   */
  // N: 0 , current row: -1
  rs.setRowCount(0);
  rs.setCurrentRow(-1);
  actions->setRowState(rs);
  QVERIFY(!toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(!toNext->isEnabled());
  QVERIFY(!toLast->isEnabled());
  // N: 1 , current row: 0
  rs.setRowCount(1);
  rs.setCurrentRow(0);
  actions->setRowState(rs);
  QVERIFY(!toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(!toNext->isEnabled());
  QVERIFY(!toLast->isEnabled());
  // N: 2 , current row: 0
  rs.setRowCount(2);
  rs.setCurrentRow(0);
  actions->setRowState(rs);
  QVERIFY(!toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(toNext->isEnabled());
  QVERIFY(toLast->isEnabled());
  // N: 2 , current row: 1
  rs.setRowCount(2);
  rs.setCurrentRow(1);
  actions->setRowState(rs);
  QVERIFY(toFirst->isEnabled());
  QVERIFY(toPrevious->isEnabled());
  QVERIFY(!toNext->isEnabled());
  QVERIFY(!toLast->isEnabled());
  // N: 3 , current row: 0
  rs.setRowCount(3);
  rs.setCurrentRow(0);
  actions->setRowState(rs);
  QVERIFY(!toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(toNext->isEnabled());
  QVERIFY(toLast->isEnabled());
  // N: 3 , current row: 1
  rs.setRowCount(3);
  rs.setCurrentRow(1);
  actions->setRowState(rs);
  QVERIFY(toFirst->isEnabled());
  QVERIFY(toPrevious->isEnabled());
  QVERIFY(toNext->isEnabled());
  QVERIFY(toLast->isEnabled());
  // N: 3 , current row: 2
  rs.setRowCount(3);
  rs.setCurrentRow(2);
  actions->setRowState(rs);
  QVERIFY(toFirst->isEnabled());
  QVERIFY(toPrevious->isEnabled());
  QVERIFY(!toNext->isEnabled());
  QVERIFY(!toLast->isEnabled());
  /*
   * Check with some invalid row states
   */
  // N: 1 , current row: -1
  rs.setRowCount(1);
  rs.setCurrentRow(-1);
  actions->setRowState(rs);
  QVERIFY(toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(toNext->isEnabled());
  QVERIFY(toLast->isEnabled());
  // N: 2 , current row: -1
  rs.setRowCount(2);
  rs.setCurrentRow(-1);
  actions->setRowState(rs);
  QVERIFY(toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(toNext->isEnabled());
  QVERIFY(toLast->isEnabled());
  /*
   * Check in editing state
   */
  actions->setControllerState(ControllerState::Editing);
  QVERIFY(!toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(!toNext->isEnabled());
  QVERIFY(!toLast->isEnabled());
  // Set a valid row state
  rs.setRowCount(4);
  rs.setCurrentRow(0);
  actions->setRowState(rs);
  QVERIFY(!toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(!toNext->isEnabled());
  QVERIFY(!toLast->isEnabled());
  /*
   * Go to visualizing state
   */
  actions->setControllerState(ControllerState::Visualizing);
  QVERIFY(!toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(toNext->isEnabled());
  QVERIFY(toLast->isEnabled());
  // Set a valid row state
  rs.setRowCount(4);
  rs.setCurrentRow(1);
  actions->setRowState(rs);
  QVERIFY(toFirst->isEnabled());
  QVERIFY(toPrevious->isEnabled());
  QVERIFY(toNext->isEnabled());
  QVERIFY(toLast->isEnabled());
  /*
   * Clear
   */
  delete actions;
  QVERIFY(toFirst.isNull());
  QVERIFY(toPrevious.isNull());
  QVERIFY(toNext.isNull());
  QVERIFY(toLast.isNull());
}

void ActionsTest::editionActionsTest()
{
  using Mdt::ItemEditor::EditionActions;
  using Mdt::ItemEditor::RowState;
  using Mdt::ItemEditor::ControllerState;

  EditionActions *actions = new EditionActions(nullptr);
  QPointer<QAction> submitAction = actions->submitAction();
  QPointer<QAction> revertAction = actions->revertAction();
  RowState rs;
  /*
   * Initial state
   */
  QVERIFY(!submitAction->isEnabled());
  QVERIFY(!revertAction->isEnabled());
  /*
   * Check controller state
   */
  // Set a valid row state
  rs.setRowCount(2);
  rs.setCurrentRow(0);
  actions->setRowState(rs);
  // Visualizing state
  actions->setControllerState(ControllerState::Visualizing);
  QVERIFY(!submitAction->isEnabled());
  QVERIFY(!revertAction->isEnabled());
  // Editing state
  actions->setControllerState(ControllerState::Editing);
  QVERIFY(submitAction->isEnabled());
  QVERIFY(revertAction->isEnabled());
  /*
   * Check setting row sate in Editing state
   */
  actions->setControllerState(ControllerState::Editing);
  // Set valid row state
  rs.setRowCount(2);
  rs.setCurrentRow(0);
  actions->setRowState(rs);
  QVERIFY(submitAction->isEnabled());
  QVERIFY(revertAction->isEnabled());
  // Null row state
  rs.setRowCount(0);
  rs.setCurrentRow(-1);
  actions->setRowState(rs);
  QVERIFY(!submitAction->isEnabled());
  QVERIFY(!revertAction->isEnabled());
  /*
   * Check setting row sate in Visualizing state
   */
  actions->setControllerState(ControllerState::Visualizing);
  // Set valid row state
  rs.setRowCount(2);
  rs.setCurrentRow(0);
  actions->setRowState(rs);
  QVERIFY(!submitAction->isEnabled());
  QVERIFY(!revertAction->isEnabled());
  // Null row state
  rs.setRowCount(0);
  rs.setCurrentRow(-1);
  actions->setRowState(rs);
  QVERIFY(!submitAction->isEnabled());
  QVERIFY(!revertAction->isEnabled());
  /*
   * Clear
   */
  delete actions;
  QVERIFY(submitAction.isNull());
  QVERIFY(revertAction.isNull());
}

void ActionsTest::insertActionTest()
{
  using Mdt::ItemEditor::InsertAction;
  using Mdt::ItemEditor::ControllerState;

  auto *action = new InsertAction(nullptr);
  QPointer<QAction> insertAction = action->insertAction();
  /*
   * Initial state
   */
  /// \todo Define a initial state (also in controller) and enable
//   QVERIFY(!insertAction->isEnabled());
  /*
   * Change controller state
   */
  action->setControllerState(ControllerState::Visualizing);
  QVERIFY(insertAction->isEnabled());
  action->setControllerState(ControllerState::Editing);
  QVERIFY(!insertAction->isEnabled());
  /*
   * Clear
   */
  delete action;
  QVERIFY(insertAction.isNull());
}

void ActionsTest::removeActionTest()
{
  using ItemEditor::RemoveAction;
  using ItemEditor::RowState;
  using ItemEditor::ControllerState;

  RowState rs;
  auto *action = new RemoveAction(nullptr);
  QPointer<QAction> removeAction = action->removeAction();
  /*
   * Initial state
   */
  QVERIFY(!removeAction->isEnabled());
  /*
   * Check controller state
   */
  // Set a valid row state
  rs.setRowCount(2);
  rs.setCurrentRow(0);
  action->setRowState(rs);
  // Visualizing state
  action->setControllerState(ControllerState::Visualizing);
  QVERIFY(removeAction->isEnabled());
  // Editing state
  action->setControllerState(ControllerState::Editing);
  QVERIFY(!removeAction->isEnabled());
  /*
   * Check setting row sate in Editing state
   */
  action->setControllerState(ControllerState::Editing);
  // Set valid row state
  rs.setRowCount(2);
  rs.setCurrentRow(0);
  action->setRowState(rs);
  QVERIFY(!removeAction->isEnabled());
  // Null row state
  rs.setRowCount(0);
  rs.setCurrentRow(-1);
  action->setRowState(rs);
  QVERIFY(!removeAction->isEnabled());
  /*
   * Check setting row sate in Visualizing state
   */
  action->setControllerState(ControllerState::Visualizing);
  // Set valid row state
  rs.setRowCount(2);
  rs.setCurrentRow(0);
  action->setRowState(rs);
  QVERIFY(removeAction->isEnabled());
  // Null row state
  rs.setRowCount(0);
  rs.setCurrentRow(-1);
  action->setRowState(rs);
  QVERIFY(!removeAction->isEnabled());
  /*
   * Clear
   */
  delete action;
  QVERIFY(removeAction.isNull());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ActionsTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
