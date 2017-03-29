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
#include "WidgetMapperControllerTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/WidgetMapperController.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QSignalSpy>
#include <QStringListModel>
#include <QTableView>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>

#include <QDebug>

using namespace Mdt::ItemModel;
using namespace Mdt::ItemEditor;

void WidgetMapperControllerTest::initTestCase()
{
}

void WidgetMapperControllerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void WidgetMapperControllerTest::statePermissionTest()
{
  WidgetMapperController controller;
  auto *stateMachine = controller.controllerStateMachine();
  QVERIFY(stateMachine != nullptr);

  /*
   * Visualizing state
   */
  stateMachine->forceCurrentState(ControllerState::Visualizing);
  QVERIFY( stateMachine->canChangeCurrentRow());
  QVERIFY( stateMachine->isChangeCurrentRowActionEnabled());
  QVERIFY( stateMachine->canSubmit());
  QVERIFY(!stateMachine->isSubmitActionEnabled());
  QVERIFY(!stateMachine->canRevert());
  QVERIFY(!stateMachine->isRevertActionEnabled());
  QVERIFY( stateMachine->canInsert());
  QVERIFY( stateMachine->isInsertActionEnabled());
  QVERIFY( stateMachine->canRemove());
  QVERIFY( stateMachine->isRemoveActionEnabled());
  /*
   * Editing state
   *  - Model has cache: No
   *  - Controller has child: No
   */
  stateMachine->setModelHasCache(false);
  stateMachine->setHasChildController(false);
  stateMachine->forceCurrentState(ControllerState::Editing);
  QVERIFY(!stateMachine->canChangeCurrentRow());
  QVERIFY(!stateMachine->isChangeCurrentRowActionEnabled());
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
   *  - Model has cache: Yes
   *  - Controller has child: No
   */
  stateMachine->setModelHasCache(true);
  stateMachine->setHasChildController(false);
  stateMachine->forceCurrentState(ControllerState::Editing);
  QVERIFY(!stateMachine->canChangeCurrentRow());
  QVERIFY(!stateMachine->isChangeCurrentRowActionEnabled());
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
  QVERIFY( stateMachine->canSubmit());
  QVERIFY( stateMachine->isSubmitActionEnabled());
  QVERIFY( stateMachine->canRevert());
  QVERIFY( stateMachine->isRevertActionEnabled());
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
  QVERIFY(!stateMachine->canChangeCurrentRow());
  QVERIFY(!stateMachine->isChangeCurrentRowActionEnabled());
  QVERIFY( stateMachine->canSubmit());
  QVERIFY( stateMachine->isSubmitActionEnabled());
  QVERIFY( stateMachine->canRevert());
  QVERIFY( stateMachine->isRevertActionEnabled());
  QVERIFY(!stateMachine->canInsert());
  QVERIFY(!stateMachine->isInsertActionEnabled());
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
  QVERIFY( stateMachine->canSubmit());
  QVERIFY( stateMachine->isSubmitActionEnabled());
  QVERIFY( stateMachine->canRevert());
  QVERIFY( stateMachine->isRevertActionEnabled());
  QVERIFY(!stateMachine->canInsert());
  QVERIFY(!stateMachine->isInsertActionEnabled());
  QVERIFY(!stateMachine->canRemove());
  QVERIFY(!stateMachine->isRemoveActionEnabled());

  QFAIL("Not complete");

//   // Visualizing state
//   stateMachine->forceCurrentState(ControllerState::Visualizing);
//   QVERIFY( stateMachine->canChangeCurrentRow());
//   QVERIFY( stateMachine->isChangeCurrentRowActionEnabled());
//   QVERIFY( stateMachine->canSubmit());
//   QVERIFY(!stateMachine->isSubmitActionEnabled());
//   QVERIFY(!stateMachine->canRevert());
//   QVERIFY(!stateMachine->isRevertActionEnabled());
//   QVERIFY( stateMachine->canInsert());
//   QVERIFY( stateMachine->isInsertActionEnabled());
//   QVERIFY( stateMachine->canRemove());
//   QVERIFY( stateMachine->isRemoveActionEnabled());
//   // Editing state
//   stateMachine->forceCurrentState(ControllerState::Editing);
//   QVERIFY(!stateMachine->canChangeCurrentRow());
//   QVERIFY(!stateMachine->isChangeCurrentRowActionEnabled());
//   QVERIFY( stateMachine->canSubmit());
//   QVERIFY( stateMachine->isSubmitActionEnabled());
//   QVERIFY( stateMachine->canRevert());
//   QVERIFY( stateMachine->isRevertActionEnabled());
//   QVERIFY(!stateMachine->canInsert());
//   QVERIFY(!stateMachine->isInsertActionEnabled());
//   QVERIFY(!stateMachine->canRemove());
//   QVERIFY(!stateMachine->isRemoveActionEnabled());
//   // ParentEditing state
//   stateMachine->forceCurrentState(ControllerState::ParentEditing);
//   QVERIFY( stateMachine->canChangeCurrentRow());
//   QVERIFY( stateMachine->isChangeCurrentRowActionEnabled());
//   QVERIFY( stateMachine->canSubmit());
//   QVERIFY(!stateMachine->isSubmitActionEnabled());
//   QVERIFY( stateMachine->canRevert());
//   QVERIFY(!stateMachine->isRevertActionEnabled());
//   QVERIFY( stateMachine->canInsert());
//   QVERIFY( stateMachine->isInsertActionEnabled());
//   QVERIFY( stateMachine->canRemove());
//   QVERIFY( stateMachine->isRemoveActionEnabled());
//   // ChildEditing state
//   stateMachine->forceCurrentState(ControllerState::ChildEditing);
//   QVERIFY(!stateMachine->canChangeCurrentRow());
//   QVERIFY(!stateMachine->isChangeCurrentRowActionEnabled());
//   QVERIFY( stateMachine->canSubmit());
//   QVERIFY( stateMachine->isSubmitActionEnabled());
//   QVERIFY( stateMachine->canRevert());
//   QVERIFY( stateMachine->isRevertActionEnabled());
//   QVERIFY(!stateMachine->canInsert());
//   QVERIFY(!stateMachine->isInsertActionEnabled());
//   QVERIFY(!stateMachine->canRemove());
//   QVERIFY(!stateMachine->isRemoveActionEnabled());
//   // Inserting state
//   stateMachine->forceCurrentState(ControllerState::Inserting);
//   QVERIFY(!stateMachine->canChangeCurrentRow());
//   QVERIFY(!stateMachine->isChangeCurrentRowActionEnabled());
// //   QVERIFY(!stateMachine->canSubmit());
// //   QVERIFY(!stateMachine->isInsertActionEnabled());
//   QVERIFY(!stateMachine->canRevert());
//   QVERIFY(!stateMachine->isRevertActionEnabled());
// //   QVERIFY(!stateMachine->canInsert());
// //   QVERIFY(!stateMachine->isInsertActionEnabled());
//   QVERIFY( stateMachine->canRemove());
//   QVERIFY( stateMachine->isRemoveActionEnabled());

  // Select
//   QVERIFY(stateMachine->canSelect(ControllerState::Visualizing));
//   QVERIFY(!stateMachine->canSelect(ControllerState::Editing));
//   QVERIFY(!stateMachine->canSelect(ControllerState::Inserting));
//   QVERIFY(!stateMachine->canSelect(ControllerState::ChildEditing));
//   QVERIFY(!stateMachine->canSelect(ControllerState::ParentEditing));

//   QFAIL("Not complete - Check also Insert + Select");

//   WidgetMapperController controller;
//   auto permission = controller.controllerStatePermission();
// 
//   // Select
// //   QVERIFY(permission.canSelect(ControllerState::Visualizing));
// //   QVERIFY(!permission.canSelect(ControllerState::Editing));
// //   QVERIFY(!permission.canSelect(ControllerState::Inserting));
// //   QVERIFY(!permission.canSelect(ControllerState::ChildEditing));
// //   QVERIFY(!permission.canSelect(ControllerState::ParentEditing));
// 
//   // Current row change
//   QVERIFY( permission.canChangeCurrentRow(ControllerState::Visualizing));
//   QVERIFY( permission.isChangeCurrentRowActionEnabled(ControllerState::Visualizing));
//   QVERIFY(!permission.canChangeCurrentRow(ControllerState::Editing));
//   QVERIFY(!permission.isChangeCurrentRowActionEnabled(ControllerState::Editing));
//   QVERIFY(!permission.canChangeCurrentRow(ControllerState::Inserting));
//   QVERIFY(!permission.isChangeCurrentRowActionEnabled(ControllerState::Inserting));
//   QVERIFY(!permission.canChangeCurrentRow(ControllerState::ChildEditing));
//   QVERIFY(!permission.isChangeCurrentRowActionEnabled(ControllerState::ChildEditing));
//   QVERIFY( permission.canChangeCurrentRow(ControllerState::ParentEditing));
//   QVERIFY( permission.isChangeCurrentRowActionEnabled(ControllerState::ParentEditing));
//   // Insert
//   QVERIFY( permission.canInsert(ControllerState::Visualizing));
//   QVERIFY( permission.isInsertActionEnabled(ControllerState::Visualizing));
//   QVERIFY(!permission.canInsert(ControllerState::Editing));
//   QVERIFY(!permission.isInsertActionEnabled(ControllerState::Editing));
// //   QVERIFY(!permission.canInsert(ControllerState::Inserting));
// //   QVERIFY(!permission.isInsertActionEnabled(ControllerState::Inserting));
//   QVERIFY(!permission.canInsert(ControllerState::ChildEditing));
//   QVERIFY(!permission.isInsertActionEnabled(ControllerState::ChildEditing));
//   QVERIFY( permission.canInsert(ControllerState::ParentEditing));
//   QVERIFY( permission.isInsertActionEnabled(ControllerState::ParentEditing));
//   // Submit
//   QVERIFY( permission.canSubmit(ControllerState::Visualizing));
//   QVERIFY(!permission.isSubmitActionEnabled(ControllerState::Visualizing));
//   QVERIFY( permission.canSubmit(ControllerState::Editing));
//   QVERIFY( permission.isSubmitActionEnabled(ControllerState::Editing));
// //   QVERIFY(!permission.canSubmit(ControllerState::Inserting));
// //   QVERIFY(!permission.isInsertActionEnabled(ControllerState::Inserting));
//   QVERIFY( permission.canSubmit(ControllerState::ChildEditing));
//   QVERIFY( permission.isSubmitActionEnabled(ControllerState::ChildEditing));
//   QVERIFY( permission.canSubmit(ControllerState::ParentEditing));
//   QVERIFY(!permission.isSubmitActionEnabled(ControllerState::ParentEditing));
//   // Revert
//   QVERIFY(!permission.canRevert(ControllerState::Visualizing));
//   QVERIFY(!permission.isRevertActionEnabled(ControllerState::Visualizing));
//   QVERIFY( permission.canRevert(ControllerState::Editing));
//   QVERIFY( permission.isRevertActionEnabled(ControllerState::Editing));
//   QVERIFY(!permission.canRevert(ControllerState::Inserting));
//   QVERIFY(!permission.isRevertActionEnabled(ControllerState::Inserting));
//   QVERIFY( permission.canRevert(ControllerState::ChildEditing));
//   QVERIFY( permission.isRevertActionEnabled(ControllerState::ChildEditing));
//   QVERIFY( permission.canRevert(ControllerState::ParentEditing));
//   QVERIFY(!permission.isRevertActionEnabled(ControllerState::ParentEditing));
//   // Remove
//   QVERIFY( permission.canRemove(ControllerState::Visualizing));
//   QVERIFY( permission.isRemoveActionEnabled(ControllerState::Visualizing));
//   QVERIFY(!permission.canRemove(ControllerState::Editing));
//   QVERIFY(!permission.isRemoveActionEnabled(ControllerState::Editing));
//   QVERIFY( permission.canRemove(ControllerState::Inserting));
//   QVERIFY( permission.isRemoveActionEnabled(ControllerState::Inserting));
//   QVERIFY(!permission.canRemove(ControllerState::ChildEditing));
//   QVERIFY(!permission.isRemoveActionEnabled(ControllerState::ChildEditing));
//   QVERIFY( permission.canRemove(ControllerState::ParentEditing));
//   QVERIFY( permission.isRemoveActionEnabled(ControllerState::ParentEditing));
//   // Select
// //   QVERIFY(permission.canSelect(ControllerState::Visualizing));
// //   QVERIFY(!permission.canSelect(ControllerState::Editing));
// //   QVERIFY(!permission.canSelect(ControllerState::Inserting));
// //   QVERIFY(!permission.canSelect(ControllerState::ChildEditing));
// //   QVERIFY(!permission.canSelect(ControllerState::ParentEditing));
// 
//   QFAIL("Not complete - Check also Insert + Select");
}

void WidgetMapperControllerTest::stateTableTest()
{
  WidgetMapperController controller;
  auto *stateMachine = controller.controllerStateMachine();
  QVERIFY(stateMachine != nullptr);

  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  // Visualizing - EditingItem - Editing
  stateMachine->dataEditionStarted();
  QCOMPARE(stateMachine->currentState(), ControllerState::Editing);
  stateMachine->dataEditionDone();
  QCOMPARE(stateMachine->currentState(), ControllerState::Editing);
  stateMachine->dataEditionStarted();
  QCOMPARE(stateMachine->currentState(), ControllerState::Editing);
  stateMachine->submitDone();
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  stateMachine->dataEditionStarted();
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

  QFAIL("Not complete");
}

void WidgetMapperControllerTest::setModelTest()
{
  VariantTableModel tableModel;
  QStringListModel listModel;
  QLineEdit editA, editB;
  WidgetMapperController controller;
  QModelIndex index;
  QSignalSpy rowStateSpy(&controller, &WidgetMapperController::rowStateChanged);
  QList<QVariant> spyItem;
  RowState rs;

  QVERIFY(rowStateSpy.isValid());
  /*
   * Initial state
   */
  QVERIFY(controller.model() == nullptr);
  QVERIFY(controller.widgetMapper() != nullptr);
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set a empty model
   */
  QCOMPARE(tableModel.rowCount(), 0);
  controller.setModel(&tableModel);
  QVERIFY(controller.model() == &tableModel);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Map widgets
   */
  controller.addMapping(&editA, 0);
  controller.addMapping(&editB, 1);
  QVERIFY(!editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  QVERIFY(editA.text().isEmpty());
  QVERIFY(editB.text().isEmpty());
  /*
   * Populate model with 1 column
   */
  tableModel.populate(3, 1);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  // Check mapped widgets data
  index = tableModel.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), tableModel.data(index).toString());
  QVERIFY(editB.text().isEmpty());
  /*
   * Populate model with 2 columns
   */
  tableModel.populate(3, 2);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = tableModel.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), tableModel.data(index).toString());
  index = tableModel.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), tableModel.data(index).toString());
  /*
   * Change to a empty model model
   */
  QCOMPARE(listModel.rowCount(), 0);
  controller.clearMapping();
  controller.setModel(&listModel);
  QVERIFY(controller.model() == &listModel);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  // Map widgets
  controller.addMapping(&editA, 0);
  controller.addMapping(&editB, 1);
  // Check mapped widgets enable state
  QVERIFY(!editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  // Check mapped widgets data
  QVERIFY(editA.text().isEmpty());
  QVERIFY(editB.text().isEmpty());
  /*
   * Populate previous model and set it to controller
   */
  tableModel.populate(3, 2);
  controller.clearMapping();
  controller.setModel(&tableModel);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  // Map widgets
  controller.addMapping(&editA, 0);
  controller.addMapping(&editB, 1);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = tableModel.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), tableModel.data(index).toString());
  index = tableModel.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), tableModel.data(index).toString());
  /*
   * Clear model
   */
  tableModel.clear();
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  // Check mapped widgets enable state
  QVERIFY(!editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  // Check mapped widgets data
  QVERIFY(editA.text().isEmpty());
  QVERIFY(editB.text().isEmpty());
}

void WidgetMapperControllerTest::currentRowChangedTest()
{
  using Mdt::ItemEditor::RowState;

  VariantTableModel model;
  QLineEdit editA, editB;
  WidgetMapperController controller;
  QModelIndex index;
  QSignalSpy rowStateSpy(&controller, &WidgetMapperController::rowStateChanged);
  QList<QVariant> spyItem;
  RowState rs;

  QVERIFY(rowStateSpy.isValid());
  /*
   * Setup
   */
  model.populate(5, 2);
  controller.setModel(&model);
  controller.addMapping(&editA, 0);
  controller.addMapping(&editB, 1);
  rowStateSpy.clear();
  /*
   * Initial state
   */
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 0);
  QCOMPARE(rowStateSpy.count(), 0);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), model.data(index).toString());
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), model.data(index).toString());
  /*
   * Set current row
   */
  QVERIFY(controller.setCurrentRow(2));
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 2);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 2);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = model.index(2, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), model.data(index).toString());
  index = model.index(2, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), model.data(index).toString());
  /*
   * Go to first
   */
  controller.toFirst();
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 0);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), model.data(index).toString());
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), model.data(index).toString());
  /*
   * Go to next
   */
  controller.toNext();
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 1);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 1);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), model.data(index).toString());
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), model.data(index).toString());
  /*
   * Go to last
   */
  controller.toLast();
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 4);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 4);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = model.index(4, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), model.data(index).toString());
  index = model.index(4, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), model.data(index).toString());
  /*
   * Go to previous
   */
  controller.toPrevious();
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 3);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 3);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = model.index(3, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), model.data(index).toString());
  index = model.index(3, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), model.data(index).toString());
}

void WidgetMapperControllerTest::editTest()
{
  using Mdt::ItemEditor::ControllerState;

  
  
  QFETCH(QWidget*, editor0);

  VariantTableModel model;
  QLineEdit editA, editB;
  WidgetMapperController controller;
  QModelIndex index;
  QSignalSpy stateSpy(&controller, &WidgetMapperController::controllerStateChanged);
  QList<QVariant> spyItem;
  QString originalText;

  QVERIFY(stateSpy.isValid());
  /*
   * Setup
   */
  model.populate(5, 2);
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setData(index, QVariant());
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  model.setData(index, QVariant());
  controller.setModel(&model);
  controller.addMapping(&editA, 0);
  controller.addMapping(&editB, 1);
  /*
   * Initial state
   */
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Check that we can change current row
   */
  QVERIFY(controller.setCurrentRow(1));
  QCOMPARE(controller.currentRow(), 1);
  /*
   * Begin editing
   */
  QVERIFY(editA.text() != QString("A"));
  QTest::keyClicks(&editA, "A");
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  spyItem = stateSpy.takeFirst();
  QVERIFY(spyItem.at(0).value<ControllerState>() == ControllerState::Editing);
  // Check that we cannot change current row
  QVERIFY(!controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 1);
  /*
   * End edition by submit
   */
  QVERIFY(controller.submit());
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  spyItem = stateSpy.takeFirst();
  QVERIFY(spyItem.at(0).value<ControllerState>() == ControllerState::Visualizing);
  QCOMPARE(controller.currentRow(), 1);
  // Check that model was updated
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("A"));
  // Check that widget data was updated
  QCOMPARE(editA.text(), QString("A"));
  // Check that we can change current row
  QVERIFY(controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Begin editing
   */
  originalText = editA.text();
  QVERIFY(originalText != QString("B"));
  QTest::keyClicks(&editA, "B");
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  spyItem = stateSpy.takeFirst();
  QVERIFY(spyItem.at(0).value<ControllerState>() == ControllerState::Editing);
  // Check that we cannot change current row
  QVERIFY(!controller.setCurrentRow(1));
  QCOMPARE(controller.currentRow(), 0);
  /*
   * End edition by revert
   */
  controller.revert();
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  spyItem = stateSpy.takeFirst();
  QVERIFY(spyItem.at(0).value<ControllerState>() == ControllerState::Visualizing);
  QCOMPARE(controller.currentRow(), 0);
  // Check that model was not updated
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index).toString(), originalText);
  // Check that widget data was updated
  QCOMPARE(editA.text(), originalText);
  // Check that we can change current row
  QVERIFY(controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Cleanup
   */
  delete editor0;
}

void WidgetMapperControllerTest::editTest_data()
{
  QTest::addColumn<QWidget*>("editor0");

  QTest::newRow("QLineEdit") << (static_cast<QWidget*>(new QLineEdit));
  QTest::newRow("QSpinBox") << (static_cast<QWidget*>(new QSpinBox));

  auto *cb = new QComboBox;
  ///cb->setEditable(true);
  cb->addItem("A");
  cb->addItem("B");
  QTest::newRow("QComboBox") << (static_cast<QWidget*>(cb));
}

void WidgetMapperControllerTest::insertTest()
{
  VariantTableModel model;
  model.resize(0, 1);
  QLineEdit editor0;
  WidgetMapperController controller;
  QModelIndex index;

  /*
   * Initial state
   */
  QVERIFY(controller.insertLocation() == WidgetMapperController::InsertAtBeginning);
  /*
   * Setup
   */
  controller.setModel(&model);
  controller.addMapping(&editor0, 0);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  QVERIFY(!editor0.isEnabled());
  QVERIFY(editor0.text().isEmpty());
  /*
   * Insert at beginning
   */
  QVERIFY(controller.insert());
  QCOMPARE(controller.rowCount(), 1);
  QCOMPARE(controller.currentRow(), 0);
  QVERIFY(editor0.isEnabled());
  QVERIFY(editor0.text().isEmpty());
  editor0.setText("B");
  QVERIFY(controller.submit());
  /*
   * Insert at beginning
   */
  QVERIFY(controller.insert());
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  QVERIFY(editor0.isEnabled());
  QVERIFY(editor0.text().isEmpty());
  editor0.setText("A");
  QVERIFY(controller.submit());
  /*
   * Insert at end
   */
  controller.setInsertLocation(WidgetMapperController::InsertAtEnd);
  QVERIFY(controller.insertLocation() == WidgetMapperController::InsertAtEnd);
  QVERIFY(controller.insert());
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 2);
  QVERIFY(editor0.isEnabled());
  QVERIFY(editor0.text().isEmpty());
  editor0.setText("C");
  QVERIFY(controller.submit());
  /*
   * Check data in model
   */
  QCOMPARE(model.data(0, 0), QVariant("A"));
  QCOMPARE(model.data(1, 0), QVariant("B"));
  QCOMPARE(model.data(2, 0), QVariant("C"));
}

void WidgetMapperControllerTest::insertFromModelTest()
{
  using Mdt::ItemEditor::ControllerState;

  QStringListModel model;
  QLineEdit editor0;
  WidgetMapperController controller;
  QModelIndex index;

  /*
   * Initial state
   */
  QVERIFY(controller.insertLocation() == WidgetMapperController::InsertAtBeginning);
  /*
   * Setup
   */
  controller.setModel(&model);
  controller.addMapping(&editor0, 0);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  /*
   * Insert at beginning
   */
  QVERIFY(controller.insert());
  QVERIFY(controller.insert());
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Begin editing
   */
  editor0.setText("ABCD");
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  /*
   * Insert at end in model
   */
  QVERIFY(model.insertRow(2));
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * End
   */
  QVERIFY(controller.submit());
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole).toString(), QString("ABCD"));
}

void WidgetMapperControllerTest::removeTest()
{
  QStringListModel model;
  QLineEdit editor0;
  WidgetMapperController controller;
  QModelIndex index;

  /*
   * Setup
   */
  model.setStringList({"A","B"});
  controller.setModel(&model);
  controller.addMapping(&editor0, 0);
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Edit and check that remove fails
   */
  // Begin editing
  editor0.setText("1234");
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // Try to remove
  QVERIFY(!controller.remove());
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // End editing
  QVERIFY(controller.submit());
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole).toString(), QString("1234"));
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
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
//   editor0.show();
//   while(editor0.isVisible()){
//     QTest::qWait(500);
//   }
}

void WidgetMapperControllerTest::removeFromModelTest()
{
  QStringListModel model;
  QLineEdit editor0;
  WidgetMapperController controller;
  QModelIndex index;

  /*
   * Setup
   */
  model.setStringList({"A","B","C","D"});
  controller.setModel(&model);
  controller.addMapping(&editor0, 0);
  QCOMPARE(controller.rowCount(), 4);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Edit row 0 , remove last row
   */
  // Begin editing
  editor0.setText("1234");
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // Remove last row
  QVERIFY(model.removeRow(3));
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // End editing and check
  // End editing
  QVERIFY(controller.submit());
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole).toString(), QString("1234"));
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
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
//   editor0.show();
//   while(editor0.isVisible()){
//     QTest::qWait(500);
//   }
}

void WidgetMapperControllerTest::filterTest()
{
  QFAIL("Not complete");
}

void WidgetMapperControllerTest::sortTest()
{
  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  WidgetMapperControllerTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
