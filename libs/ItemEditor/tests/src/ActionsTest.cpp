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
#include "TestTableModel.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/RowState.h"
#include "Mdt/ItemEditor/NavigationActions.h"
#include "Mdt/ItemEditor/InsertAction.h"
#include "Mdt/ItemEditor/ControllerState.h"
#include <QSignalSpy>
#include <QItemSelectionModel>
#include <QStringListModel>
#include <QTableView>
#include <QPointer>
#include <QAction>

#include <QDebug>

void ActionsTest::initTestCase()
{
}

void ActionsTest::cleanupTestCase()
{
}

/*
 * Tests
 */

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
