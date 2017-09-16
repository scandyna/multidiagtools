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
#include "TableViewControllerTest.h"
#include "RowStateChangedSpy.h"
#include "Mdt/ItemEditor/TableViewController.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include "Mdt/ItemEditor/ControllerState.h"
#include "Mdt/ItemEditor/ControllerEvent.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QStringList>
#include <QSignalSpy>
#include <QStringListModel>
#include <QTableView>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>

using namespace Mdt::ItemModel;
using namespace Mdt::ItemEditor;

void TableViewControllerTest::initTestCase()
{
}

void TableViewControllerTest::cleanupTestCase()
{
}

/*
 * This is a complete test for TableViewController,
 * which is implemented using TableViewControllerImplementation.
 *
 * TableViewControllerImplementation is also tested in TableViewControllerImplementationTest,
 * but event handling is not complete.
 */

void TableViewControllerTest::statePermissionTest()
{
  TableViewController controller;
  auto *stateMachine = controller.controllerStateMachine();
  QVERIFY(stateMachine != nullptr);

  /*
   * Visualizing state
   */
  stateMachine->forceCurrentState(ControllerState::Visualizing);
  QVERIFY( stateMachine->canChangeCurrentRow());
  QVERIFY( stateMachine->isChangeCurrentRowActionEnabled());
  QVERIFY( stateMachine->canEdit());
  QVERIFY( stateMachine->canSubmit());
  QVERIFY(!stateMachine->isSubmitActionEnabled());
  QVERIFY(!stateMachine->canRevert());
  QVERIFY(!stateMachine->isRevertActionEnabled());
  QVERIFY( stateMachine->canInsert());
  QVERIFY( stateMachine->isInsertActionEnabled());
  QVERIFY( stateMachine->canRemove());
  QVERIFY( stateMachine->isRemoveActionEnabled());
  /*
   * EditingItem state
   */
  stateMachine->forceCurrentState(ControllerState::EditingItem);
  QVERIFY(!stateMachine->canChangeCurrentRow());
  QVERIFY(!stateMachine->isChangeCurrentRowActionEnabled());
  QVERIFY( stateMachine->canEdit());
  QVERIFY( stateMachine->canSubmit());
  QVERIFY( stateMachine->isSubmitActionEnabled());
  QVERIFY(!stateMachine->canRevert());
  QVERIFY( stateMachine->isRevertActionEnabled());
  QVERIFY(!stateMachine->canInsert());
  QVERIFY(!stateMachine->isInsertActionEnabled());
  QVERIFY(!stateMachine->canRemove());
  QVERIFY(!stateMachine->isRemoveActionEnabled());
  /*
   * Editing state
   *  - Model has cache: No
   *  - Controller has child: No
   */
  stateMachine->setModelHasCache(false);
  stateMachine->setHasChildController(false);
  stateMachine->forceCurrentState(ControllerState::Editing);
  QVERIFY( stateMachine->canChangeCurrentRow());
  QVERIFY( stateMachine->isChangeCurrentRowActionEnabled());
  QVERIFY( stateMachine->canEdit());
  QVERIFY( stateMachine->canSubmit());
  QVERIFY( stateMachine->isSubmitActionEnabled());
  QVERIFY(!stateMachine->canRevert());
  QVERIFY(!stateMachine->isRevertActionEnabled());
  QVERIFY(!stateMachine->canInsert());
  QVERIFY(!stateMachine->isInsertActionEnabled());
  QVERIFY(!stateMachine->canRemove());
  QVERIFY(!stateMachine->isRemoveActionEnabled());
  /*
   * Editing state
   *  - Model has cache: Yes
   *  - Controller has child: No
   */
  stateMachine->setModelHasCache(true);
  stateMachine->setHasChildController(false);
  stateMachine->forceCurrentState(ControllerState::Editing);
  QVERIFY( stateMachine->canChangeCurrentRow());
  QVERIFY( stateMachine->isChangeCurrentRowActionEnabled());
  QVERIFY( stateMachine->canEdit());
  QVERIFY( stateMachine->canSubmit());
  QVERIFY( stateMachine->isSubmitActionEnabled());
  QVERIFY( stateMachine->canRevert());
  QVERIFY( stateMachine->isRevertActionEnabled());
  QVERIFY(!stateMachine->canInsert());
  QVERIFY(!stateMachine->isInsertActionEnabled());
  QVERIFY(!stateMachine->canRemove());
  QVERIFY(!stateMachine->isRemoveActionEnabled());
  /*
   * Editing state
   *  - Model has cache: No
   *  - Controller has child: Yes
   */
  stateMachine->setModelHasCache(false);
  stateMachine->setHasChildController(true);
  stateMachine->forceCurrentState(ControllerState::Editing);
  QVERIFY(!stateMachine->canChangeCurrentRow());
  QVERIFY(!stateMachine->isChangeCurrentRowActionEnabled());
  QVERIFY( stateMachine->canEdit());
  QVERIFY( stateMachine->canSubmit());
  QVERIFY( stateMachine->isSubmitActionEnabled());
  QVERIFY(!stateMachine->canRevert());
  QVERIFY(!stateMachine->isRevertActionEnabled());
  QVERIFY(!stateMachine->canInsert());
  QVERIFY(!stateMachine->isInsertActionEnabled());
  QVERIFY(!stateMachine->canRemove());
  QVERIFY(!stateMachine->isRemoveActionEnabled());
  /*
   * Inserting state
   *  - Controller has child: No
   */
  stateMachine->setHasChildController(false);
  stateMachine->forceCurrentState(ControllerState::Inserting);
  QVERIFY( stateMachine->canChangeCurrentRow());
  QVERIFY( stateMachine->isChangeCurrentRowActionEnabled());
  QVERIFY( stateMachine->canEdit());
  QVERIFY( stateMachine->canSubmit());
  QVERIFY( stateMachine->isSubmitActionEnabled());
  QVERIFY( stateMachine->canRevert());
  QVERIFY( stateMachine->isRevertActionEnabled());
  QVERIFY( stateMachine->canInsert());
  QVERIFY( stateMachine->isInsertActionEnabled());
  QVERIFY(!stateMachine->canRemove());
  QVERIFY(!stateMachine->isRemoveActionEnabled());
  /*
   * Inserting state
   *  - Controller has child: Yes
   */
  stateMachine->setHasChildController(true);
  stateMachine->forceCurrentState(ControllerState::Inserting);
  QVERIFY(!stateMachine->canChangeCurrentRow());
  QVERIFY(!stateMachine->isChangeCurrentRowActionEnabled());
  QVERIFY( stateMachine->canEdit());
  QVERIFY( stateMachine->canSubmit());
  QVERIFY( stateMachine->isSubmitActionEnabled());
  QVERIFY( stateMachine->canRevert());
  QVERIFY( stateMachine->isRevertActionEnabled());
  QVERIFY(!stateMachine->canInsert());
  QVERIFY(!stateMachine->isInsertActionEnabled());
  QVERIFY(!stateMachine->canRemove());
  QVERIFY(!stateMachine->isRemoveActionEnabled());
  /*
   * ParentEditing state
   */
  stateMachine->forceCurrentState(ControllerState::ParentEditing);
  QVERIFY( stateMachine->canChangeCurrentRow());
  QVERIFY( stateMachine->isChangeCurrentRowActionEnabled());
  QVERIFY(!stateMachine->canEdit());
  QVERIFY( stateMachine->canSubmit());
  QVERIFY(!stateMachine->isSubmitActionEnabled());
  QVERIFY(!stateMachine->canRevert());
  QVERIFY(!stateMachine->isRevertActionEnabled());
  QVERIFY(!stateMachine->canInsert());
  QVERIFY(!stateMachine->isInsertActionEnabled());
  QVERIFY(!stateMachine->canRemove());
  QVERIFY(!stateMachine->isRemoveActionEnabled());
  /*
   * ChildEditing state
   */
  stateMachine->forceCurrentState(ControllerState::ChildEditing);
  QVERIFY(!stateMachine->canChangeCurrentRow());
  QVERIFY(!stateMachine->isChangeCurrentRowActionEnabled());
  QVERIFY(!stateMachine->canEdit());
  QVERIFY( stateMachine->canSubmit());
  QVERIFY( stateMachine->isSubmitActionEnabled());
  QVERIFY(!stateMachine->canRevert());
  QVERIFY(!stateMachine->isRevertActionEnabled());
  QVERIFY(!stateMachine->canInsert());
  QVERIFY(!stateMachine->isInsertActionEnabled());
  QVERIFY(!stateMachine->canRemove());
  QVERIFY(!stateMachine->isRemoveActionEnabled());
  /*
   * Disabled state
   */
  stateMachine->forceCurrentState(ControllerState::Disabled);
  QVERIFY(!stateMachine->canChangeCurrentRow());
  QVERIFY(!stateMachine->isChangeCurrentRowActionEnabled());
  QVERIFY(!stateMachine->canEdit());
  QVERIFY(!stateMachine->canSubmit());
  QVERIFY(!stateMachine->isSubmitActionEnabled());
  QVERIFY(!stateMachine->canRevert());
  QVERIFY(!stateMachine->isRevertActionEnabled());
  QVERIFY(!stateMachine->canInsert());
  QVERIFY(!stateMachine->isInsertActionEnabled());
  QVERIFY(!stateMachine->canRemove());
  QVERIFY(!stateMachine->isRemoveActionEnabled());
}

void TableViewControllerTest::stateTableTest()
{
  TableViewController controller;
  auto *stateMachine = controller.controllerStateMachine();
  QVERIFY(stateMachine != nullptr);

  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  // Visualizing - EditingItem - Editing
  stateMachine->dataEditionStarted();
  QCOMPARE(stateMachine->currentState(), ControllerState::EditingItem);
  stateMachine->dataEditionDone();
  QCOMPARE(stateMachine->currentState(), ControllerState::Editing);
  stateMachine->dataEditionStarted();
  QCOMPARE(stateMachine->currentState(), ControllerState::EditingItem);
  stateMachine->dataEditionDone();
  QCOMPARE(stateMachine->currentState(), ControllerState::Editing);
  stateMachine->submitDone();
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  stateMachine->dataEditionStarted();
  QCOMPARE(stateMachine->currentState(), ControllerState::EditingItem);
  stateMachine->dataEditionDone();
  QCOMPARE(stateMachine->currentState(), ControllerState::Editing);
  stateMachine->revertDone();
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  // Visualizing - Inserting
  stateMachine->insertStarted();
  QCOMPARE(stateMachine->currentState(), ControllerState::Inserting);
  stateMachine->dataEditionStarted();
  QCOMPARE(stateMachine->currentState(), ControllerState::Inserting);
  stateMachine->dataEditionDone();
  QCOMPARE(stateMachine->currentState(), ControllerState::Inserting);
  stateMachine->submitDone();
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  stateMachine->insertStarted();
  QCOMPARE(stateMachine->currentState(), ControllerState::Inserting);
  stateMachine->revertDone();
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  // Visualizing - ParentEditing
  stateMachine->setEvent(ControllerEvent::EditionStartedFromParent);
  QCOMPARE(stateMachine->currentState(), ControllerState::ParentEditing);
  stateMachine->setEvent(ControllerEvent::EditionDoneFromParent);
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  // Visualizing - ChildEditing
  stateMachine->setEvent(ControllerEvent::EditionStartedFromChild);
  QCOMPARE(stateMachine->currentState(), ControllerState::ChildEditing);
  stateMachine->setEvent(ControllerEvent::EditionDoneFromChild);
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  // Visualizing - Disabled
  stateMachine->setEvent(ControllerEvent::DisableController);
  QCOMPARE(stateMachine->currentState(), ControllerState::Disabled);
  stateMachine->setEvent(ControllerEvent::EnableController);
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
}

void TableViewControllerTest::setModelTest()
{
  QTableView tableView;
  TableViewController controller;
  RowStateChangedSpy rowStateSpy(controller);
  RowState rs;
  /*
   * Initial state
   */
  QVERIFY(controller.model() == nullptr);
  QVERIFY(controller.sourceModel() == nullptr);
  QVERIFY(controller.view() == nullptr);
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set model
   */
  VariantTableModel tableModel;
  tableModel.resize(3, 2);
  controller.setModel(&tableModel);
  QVERIFY(controller.model() == &tableModel);
  QVERIFY(controller.sourceModel() == &tableModel);
  // Check that row state was not signaled (no view is attached)
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Affect view to controller
   */
  controller.setView(&tableView);
  QCOMPARE(controller.view(), &tableView);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  /*
   * Change model
   */
  QStringListModel listModel(QStringList({"A","B"}));
  controller.setModel(&listModel);
  QVERIFY(controller.model() == &listModel);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), 0);
  /*
   * Play
   */
//   tableView.show();
//   while(tableView.isVisible()){
//     QTest::qWait(500);
//   }
}

void TableViewControllerTest::primaryKeyVisibilityTest()
{
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(1, 3);
  /*
   * Setup controller
   */
  QTableView view;
  TableViewController controller;
  controller.setModel(&model);
  controller.setView(&view);
  controller.setPrimaryKey({0,1});
  /*
   * By default, all columns are visible
   */
  QVERIFY(!view.isColumnHidden(0));
  QVERIFY(!view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
  /*
   * Hide primary key
   */
  controller.setPrimaryKeyHidden(true);
  QVERIFY( view.isColumnHidden(0));
  QVERIFY( view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
  /*
   * Change primary key
   */
  controller.setPrimaryKey({1});
  QVERIFY(!view.isColumnHidden(0));
  QVERIFY( view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
  /*
   * Show primary key
   */
  controller.setPrimaryKeyHidden(false);
  QVERIFY(!view.isColumnHidden(0));
  QVERIFY(!view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
}

void TableViewControllerTest::primaryKeyVisibilityChangeModelTest()
{
  /*
   * Setup controller
   */
  QTableView view;
  TableViewController controller;
  controller.setView(&view);
  controller.setPrimaryKey({0,1});
  controller.setPrimaryKeyHidden(true);
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(1, 3);
  controller.setModel(&model);
  /*
   * Check that primary key is hidden
   * (This did not work)
   */
  QVERIFY( view.isColumnHidden(0));
  QVERIFY( view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
}

void TableViewControllerTest::foreignKeyVisibilityTest()
{
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(1, 3);
  /*
   * Setup controller
   */
  QTableView view;
  TableViewController controller;
  controller.setModel(&model);
  controller.setView(&view);
  controller.addForeignKey("FE01", {0,1});
  /*
   * By default, all columns are visible
   */
  QVERIFY(!view.isColumnHidden(0));
  QVERIFY(!view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
  /*
   * Hide all foreign keys
   */
  controller.setAllForeignKeysHidden(true);
  QVERIFY( view.isColumnHidden(0));
  QVERIFY( view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
  /*
   * Add a foreign key
   */
  controller.addForeignKey("FE2", {2});
  QVERIFY( view.isColumnHidden(0));
  QVERIFY( view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
  /*
   * Show foreign keys
   */
  controller.setAllForeignKeysHidden(false);
  QVERIFY(!view.isColumnHidden(0));
  QVERIFY(!view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
  /*
   * Hide FE2
   */
  controller.setForeignKeyHidden("FE2", true);
  QVERIFY(!view.isColumnHidden(0));
  QVERIFY(!view.isColumnHidden(1));
  QVERIFY( view.isColumnHidden(2));
}

void TableViewControllerTest::currentRowChangeTest()
{
  QModelIndex index;
  QTableView viewA, viewB;
  TableViewController controller;
  RowStateChangedSpy rowStateSpy(controller);
  RowState rs;
  /*
   * Initial state
   */
  QVERIFY(controller.model() == nullptr);
  QVERIFY(controller.view() == nullptr);
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set model and view
   *  1) model
   *  2) view
   */
  VariantTableModel modelA;
  modelA.resize(1, 1);
  controller.setModel(&modelA);
  controller.setView(&viewA);
  QVERIFY(viewA.selectionModel() != nullptr);
  QCOMPARE(controller.rowCount(), 1);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), 0);
  // Check that view was updated
  QCOMPARE(viewA.currentIndex().row(), 0);
  /*
   * Populate model
   */
  modelA.populate(5, 3);
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 0);
  // Check that view was updated
  QCOMPARE(viewA.currentIndex().row(), 0);
  /*
   * Set model and view
   *  1) view
   *  2) model
   */
  VariantTableModel modelB;
  modelB.resize(1, 1);
  controller.setView(&viewB);
  controller.setModel(&modelB);
  QVERIFY(viewB.selectionModel() != nullptr);
  QCOMPARE(controller.rowCount(), 1);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), 0);
  // Check that view was updated
  QCOMPARE(viewB.currentIndex().row(), 0);
  /*
   * Populate model
   */
  modelB.populate(5, 3);
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 0);
  // Check that view was updated
  QCOMPARE(viewB.currentIndex().row(), 0);
  /*
   * Change current row from view
   */
  index = modelB.index(1, 0);
  QVERIFY(index.isValid());
  viewB.setCurrentIndex(index);
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 1);
  // Check that view was updated
  QCOMPARE(viewB.currentIndex().row(), 1);
  /*
   * Change current row from controller
   */
  QVERIFY(controller.setCurrentRow(2));
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 2);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 2);
  // Check that view was updated
  QCOMPARE(viewB.currentIndex().row(), 2);
  /*
   * Play
   */
//   viewB.show();
//   while(viewB.isVisible()){
//     QTest::qWait(500);
//   }
}

void TableViewControllerTest::viewCurrentRowChangeBenchmark()
{
  VariantTableModel model;
  QModelIndex index;
  QTableView view;

  view.setModel(&model);
  model.populate(5, 3);
  view.show();

  index = model.index(0, 0);
  QVERIFY(index.isValid());
  view.setCurrentIndex(index);
  QCOMPARE(view.currentIndex().row(), 0);

  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QBENCHMARK{
    view.setCurrentIndex(index);
    QCOMPARE(view.currentIndex().row(), 1);
  }
}

void TableViewControllerTest::controllerCurrentRowChangeBenchmark()
{
  VariantTableModel model;
  QModelIndex index;
  QTableView view;
  TableViewController controller;

  controller.setModel(&model);
  controller.setView(&view);
  model.populate(5, 3);
  view.show();

  QCOMPARE(controller.currentRow(), 0);
  QCOMPARE(view.currentIndex().row(), 0);

  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QBENCHMARK{
    view.setCurrentIndex(index);
    QCOMPARE(controller.currentRow(), 1);
  }
}

void TableViewControllerTest::editTest()
{
  VariantTableModel model;
  QTableView view;
  /*
   * Setup controller
   */
  TableViewController controller;
  QVERIFY(controller.controllerStateMachine() != nullptr);
  QSignalSpy stateSpy(controller.controllerStateMachine(), &ControllerStateMachine::currentStateChanged);
  QVERIFY(stateSpy.isValid());
  /*
   * Setup model and view
   */
  controller.setModel(&model);
  controller.setView(&view);
  model.populate(3, 2);
  view.show();
  QCOMPARE(controller.controllerState(), ControllerState::Visualizing);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Check that we can change current row
   */
  QVERIFY(controller.setCurrentRow(1));
  QCOMPARE(controller.currentRow(), 1);
  /*
   * Begin editing
   */
  beginEditing(view, 1, 0, BeginEditTrigger::DoubleClick);
  QCOMPARE(controller.controllerState(), ControllerState::EditingItem);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  stateSpy.clear();
  // Check that we cannot change current row
  QVERIFY(!controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 1);
  /*
   * Edit some text
   */
  editText(view, 1, 0, "ABCD");
  QCOMPARE(controller.controllerState(), ControllerState::EditingItem);
  QCOMPARE(stateSpy.count(), 0);
  /*
   * End edition (by user)
   */
  endEditing(view, 1, 0, EndEditTrigger::EnterKeyClick);
  QCOMPARE(controller.controllerState(), ControllerState::Editing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  stateSpy.clear();
  QCOMPARE(controller.currentRow(), 1);
  // Check that we can change current row
  QVERIFY(controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 0);
  // Check that model was updated
  QCOMPARE(getModelData(model, 1, 0), QVariant("ABCD"));
  /*
   * Begin editing
   */
  beginEditing(view, 1, 0, BeginEditTrigger::DoubleClick);
  QCOMPARE(controller.controllerState(), ControllerState::EditingItem);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  stateSpy.clear();
  // Check that we cannot change current row
  QVERIFY(!controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 1);
  /*
   * Edit some text
   */
  editText(view, 1, 0, "1234");
  QCOMPARE(controller.controllerState(), ControllerState::EditingItem);
  QCOMPARE(stateSpy.count(), 0);
  /*
   * Call submit from controller
   */
  QVERIFY(controller.submit());
  QCOMPARE(controller.controllerState(), ControllerState::Visualizing);
  // Check that new state was signaled (EditingItem -> Editing -> Visualizing = 2 state changes)
  QCOMPARE(stateSpy.count(), 2);
  stateSpy.clear();
  QCOMPARE(controller.currentRow(), 1);
  // Check that we can change current row
  QVERIFY(controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 0);
  // Check that model was updated
  QCOMPARE(getModelData(model, 1, 0), QVariant("1234"));
  /*
   * Begin editing
   */
  beginEditing(view, 1, 0, BeginEditTrigger::DoubleClick);
  QCOMPARE(controller.controllerState(), ControllerState::EditingItem);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  stateSpy.clear();
  // Check that we cannot change current row
  QVERIFY(!controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 1);
  /*
   * Edit some text
   */
  editText(view, 1, 0, "EFGH");
  QCOMPARE(controller.controllerState(), ControllerState::EditingItem);
  QCOMPARE(stateSpy.count(), 0);
  /*
   * Call revert from controller
   */
  controller.revert();
  QCOMPARE(controller.controllerState(), ControllerState::Editing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  stateSpy.clear();
  QCOMPARE(controller.currentRow(), 1);
  // Check that model was not updated
  QCOMPARE(getModelData(model, 1, 0), QVariant("1234"));
  /*
   * Submit
   */
  QVERIFY(controller.submit());
  QCOMPARE(controller.controllerState(), ControllerState::Visualizing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  stateSpy.clear();
  QCOMPARE(controller.currentRow(), 1);
  // Check that we can change current row
  QVERIFY(controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 0);
  // Check that model was not updated
  QCOMPARE(getModelData(model, 1, 0), QVariant("1234"));
  /*
   * Play
   */
//   while(view.isVisible()){
//     QTest::qWait(500);
//   }
}

void TableViewControllerTest::insertEditTest()
{
  QStringListModel model;
  QModelIndex index;
  QTableView view;
  TableViewController controller;

  /*
   * Initial state
   */
  QVERIFY(controller.insertLocation() == TableViewController::InsertAtBeginning);
  /*
   * Setup
   */
  controller.setModel(&model);
  controller.setView(&view);
  QCOMPARE(controller.controllerState(), ControllerState::Visualizing);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  view.show();
  /*
   * Insert at beginning
   */
  QVERIFY(controller.insert());
  QCOMPARE(controller.controllerState(), ControllerState::Inserting);
  QCOMPARE(controller.rowCount(), 1);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Edit new inserted row
   */
  index = view.model()->index(0, 0);
  QVERIFY(index.isValid());
  beginEditing(view, index, BeginEditTrigger::F2KeyClick);
  QCOMPARE(controller.controllerState(), ControllerState::Inserting);
  editText(view, index, "A");
  endEditing(view, index, EndEditTrigger::EnterKeyClick);
  QCOMPARE(controller.controllerState(), ControllerState::Inserting);
  QVERIFY(controller.submit());
  QCOMPARE(controller.controllerState(), ControllerState::Visualizing);
  QCOMPARE(controller.rowCount(), 1);
  QCOMPARE(controller.currentRow(), 0);
  QCOMPARE(getModelData(model, 0, 0), QVariant("A"));
  /*
   * Insert at end
   */
  controller.setInsertLocation(TableViewController::InsertAtEnd);
  QVERIFY(controller.insertLocation() == TableViewController::InsertAtEnd);
  QVERIFY(controller.insert());
  QCOMPARE(controller.controllerState(), ControllerState::Inserting);
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 1);
  /*
   * Edit new inserted row
   */
  edit(view, 1, 0, "B", BeginEditTrigger::DoubleClick, EndEditTrigger::EnterKeyClick);
  QCOMPARE(controller.controllerState(), ControllerState::Inserting);
  QVERIFY(controller.submit());
  QCOMPARE(controller.controllerState(), ControllerState::Visualizing);
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant("B"));
  /*
   * Play
   */
//   view.show();
//   while(view.isVisible()){
//     QTest::qWait(500);
//   }
}

void TableViewControllerTest::insertEditRevertTest()
{
  QStringListModel model;
  QModelIndex index;
  QTableView view;
  TableViewController controller;

  /*
   * Initial state
   */
  QVERIFY(controller.insertLocation() == TableViewController::InsertAtBeginning);
  /*
   * Setup
   */
  model.setStringList(QStringList({"A","B"}));
  controller.setModel(&model);
  controller.setView(&view);
  QCOMPARE(controller.controllerState(), ControllerState::Visualizing);
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  view.show();
  /*
   * Insert
   */
  QVERIFY(controller.insert());
  QCOMPARE(controller.controllerState(), ControllerState::Inserting);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Edit inserted row
   */
  beginEditing(view, 0, 0, BeginEditTrigger::DoubleClick);
  QCOMPARE(controller.controllerState(), ControllerState::Inserting);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Revert
   */
  controller.revert();
  QCOMPARE(controller.controllerState(), ControllerState::Visualizing);
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  QCOMPARE(getModelData(model, 0, 0), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant("B"));
}

void TableViewControllerTest::editRemoveTest()
{
  QStringListModel model({"A","B"});
  QTableView view;
  TableViewController controller;

  /*
   * Setup
   */
  controller.setModel(&model);
  controller.setView(&view);
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  view.show();
  /*
   * Edit and check that remove fails
   */
   // Begin editing
  beginEditing(view, 0, 0, BeginEditTrigger::DoubleClick);
  QCOMPARE(controller.controllerState(), ControllerState::EditingItem);
  editText(view, 0, 0, "ABCD");
  // Try to remove
  QVERIFY(!controller.remove());
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  QCOMPARE(controller.controllerState(), ControllerState::EditingItem);
  // End editing and check
  endEditing(view, 0, 0, EndEditTrigger::EnterKeyClick);
  QCOMPARE(controller.controllerState(), ControllerState::Editing);
  QVERIFY(controller.submit());
  QCOMPARE(controller.controllerState(), ControllerState::Visualizing);
  QCOMPARE(getModelData(model, 0, 0), QVariant("ABCD"));
  /*
   * Remove
   */
  QVERIFY(controller.remove());
  QCOMPARE(controller.rowCount(), 1);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Remove
   */
  QVERIFY(controller.remove());
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  /*
   * Remove
   */
  QVERIFY(!controller.remove());
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  /*
   * Play
   */
//   view.show();
//   while(view.isVisible()){
//     QTest::qWait(500);
//   }
}

void TableViewControllerTest::editRemoveFromModelTest()
{
  QStringListModel model({"A","B","C","D"});
  QTableView view;
  TableViewController controller;

  /*
   * Setup
   */
  controller.setModel(&model);
  controller.setView(&view);
  QCOMPARE(controller.rowCount(), 4);
  QCOMPARE(controller.currentRow(), 0);
  view.show();
  /*
   * Edit row 0 , remove last row
   */
   // Begin editing
  beginEditing(view, 0, 0, BeginEditTrigger::DoubleClick);
  QCOMPARE(controller.controllerState(), ControllerState::EditingItem);
  editText(view, 0, 0, "1234");
  // Remove last row
  QVERIFY(model.removeRow(3));
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  QCOMPARE(controller.controllerState(), ControllerState::EditingItem);
  // End editing and check
  endEditing(view, 0, 0, EndEditTrigger::EnterKeyClick);
  QCOMPARE(controller.controllerState(), ControllerState::Editing);
  QVERIFY(controller.submit());
  QCOMPARE(controller.controllerState(), ControllerState::Visualizing);
  QCOMPARE(getModelData(model, 0, 0), QVariant("1234"));
  /*
   * Go to row 1 , remove row 0
   */
  // Go to row 1
  QVERIFY(controller.setCurrentRow(1));
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 1);
  // Remove row 0 , and check that controller goes to row 0
  QVERIFY(model.removeRow(0));
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Remove current row (row 0)
   */
  QVERIFY(model.removeRow(0));
  QCOMPARE(controller.rowCount(), 1);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Remove last row
   */
  QVERIFY(model.removeRow(0));
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  /*
   * Play
   */
//   view.show();
//   while(view.isVisible()){
//     QTest::qWait(500);
//   }
}

void TableViewControllerTest::filterAndEditTest()
{
  VariantTableModel model;
  QTableView view;
  TableViewController controller;
  /*
   * Setup
   */
  model.resize(3, 2);
  model.populateColumn(0, {1,2,3});
  model.populateColumn(1, {"A","B","C"});
  controller.setModel(&model);
  controller.setView(&view);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Check enable/disable filter
   */
  // Enable filter
  QVERIFY(!controller.isFilterEnabled());
  controller.setFilterEnabled(true);
  QVERIFY(controller.isFilterEnabled());
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  // Disable filter
  controller.setFilterEnabled(false);
  QVERIFY(!controller.isFilterEnabled());
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  // Enable filter
  controller.setFilterEnabled(true);
  QVERIFY(controller.isFilterEnabled());
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Apply a filter
   */
  controller.setFilter( FilterColumn(0) > 1 );
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Disable filter
   */
  controller.setFilterEnabled(false);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Apply a filter
   *  -> Must also enable filter
   */
  controller.setFilter( FilterColumn(0) == 2 );
  QCOMPARE(controller.rowCount(), 1);
  QCOMPARE(controller.currentRow(), 0);
  /**
   * \todo
   * Try while editing
   */

  /*
   * Play
   */
//   view.show();
//   while(view.isVisible()){
//     QTest::qWait(500);
//   }

  QFAIL("Not complete");
}

void TableViewControllerTest::sortAndEditTest()
{
  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  TableViewControllerTest test;

  return QTest::qExec(&test, argc, argv);
}
