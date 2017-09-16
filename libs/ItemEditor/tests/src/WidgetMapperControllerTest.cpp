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
#include "RowStateChangedSpy.h"
#include "Mdt/ItemEditor/WidgetMapperController.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include "Mdt/ItemEditor/ControllerState.h"
#include "Mdt/ItemEditor/ControllerEvent.h"
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
#include <QScopedPointer>

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
   * Editing state
   *  - Model has cache: No
   *  - Controller has child: No
   */
  stateMachine->setModelHasCache(false);
  stateMachine->setHasChildController(false);
  stateMachine->forceCurrentState(ControllerState::Editing);
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
   * Editing state
   *  - Model has cache: Yes
   *  - Controller has child: No
   */
  stateMachine->setModelHasCache(true);
  stateMachine->setHasChildController(false);
  stateMachine->forceCurrentState(ControllerState::Editing);
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

void WidgetMapperControllerTest::setModelTest()
{
  VariantTableModel tableModel;
  QStringListModel listModel;
  QLineEdit editA, editB;
  WidgetMapperController controller;
  RowStateChangedSpy rowStateSpy(controller);
  RowState rs;
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
  QCOMPARE(rowStateSpy.count(), 0);
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
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  // Check mapped widgets data
  QCOMPARE(editA.text(), tableModel.data(0, 0).toString());
  QVERIFY(editB.text().isEmpty());
  /*
   * Populate model with 2 columns
   */
  tableModel.populate(3, 2);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  QCOMPARE(rowStateSpy.count(), 0);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  QCOMPARE(editA.text(), tableModel.data(0, 0).toString());
  QCOMPARE(editB.text(), tableModel.data(0, 1).toString());
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
  rs = rowStateSpy.takeRowState();
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
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  // Map widgets
  controller.addMapping(&editA, 0);
  controller.addMapping(&editB, 1);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  QCOMPARE(editA.text(), tableModel.data(0, 0).toString());
  QCOMPARE(editB.text(), tableModel.data(0, 1).toString());
  /*
   * Clear model
   */
  tableModel.clear();
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
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
  VariantTableModel model;
  QLineEdit editA, editB;
  QString originalText;
  /*
   * Setup controller
   */
  WidgetMapperController controller;
  QVERIFY(controller.controllerStateMachine() != nullptr);
  QSignalSpy stateSpy(controller.controllerStateMachine(), &ControllerStateMachine::currentStateChanged);
  QVERIFY(stateSpy.isValid());
  /*
   * Setup
   */
  model.resize(5, 2);
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
  stateSpy.clear();
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
  stateSpy.clear();
  QCOMPARE(controller.currentRow(), 1);
  // Check that model was updated
  QCOMPARE(getModelData(model, 1, 0), QVariant("A"));
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
  stateSpy.clear();
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
  stateSpy.clear();
  QCOMPARE(controller.currentRow(), 0);
  // Check that model was not updated
  QCOMPARE(getModelData(model, 0, 0).toString(), originalText);
  // Check that widget data was updated
  QCOMPARE(editA.text(), originalText);
  // Check that we can change current row
  QVERIFY(controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 0);
}

void WidgetMapperControllerTest::statePermissionModelFlagsTest()
{
  VariantTableModel model;
  QLineEdit editA;
  WidgetMapperController controller;
  auto *stateMachine = controller.controllerStateMachine();
  QVERIFY(stateMachine != nullptr);
  /*
   * Setup
   */
  model.resize(2, 1);
  controller.setModel(&model);
  controller.addMapping(&editA, 0);
  /*
   * Initial state
   */
  controller.setCurrentRow(0);
  QCOMPARE(controller.currentRow(), 0);
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  QCOMPARE(controller.currentRow(), 0);
  QVERIFY(!editA.isReadOnly());
  /*
   * Change controller state without changing current row
   */
  stateMachine->forceCurrentState(ControllerState::ChildEditing);
  QVERIFY(editA.isReadOnly());
  stateMachine->forceCurrentState(ControllerState::Visualizing);
  QVERIFY(!editA.isReadOnly());
  /*
   * Check that underlaying model flags are respected
   * -> Those flags are only updated when changing current row
   */
  // Set flags for row 0 and 1
  model.setItemEditable(0, 0, true);
  model.setItemEditable(1, 0, false);
  // Change to row 1 and check
  controller.setCurrentRow(1);
  QVERIFY(editA.isReadOnly());
  // Change to row 0 and check
  controller.setCurrentRow(0);
  QVERIFY(!editA.isReadOnly());
}

void WidgetMapperControllerTest::insertEditTest()
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

void WidgetMapperControllerTest::insertEditRevertTest()
{

  QFAIL("Not complete");
}

void WidgetMapperControllerTest::insertFromModelTest()
{
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
  QVERIFY(controller.submit());
  QCOMPARE(controller.controllerState(), ControllerState::Visualizing);
  QVERIFY(controller.insert());
  QCOMPARE(controller.controllerState(), ControllerState::Inserting);
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Begin editing
   */
  editor0.setText("ABCD");
  QCOMPARE(controller.controllerState(), ControllerState::Inserting);
  /*
   * Insert at end in model
   */
  QVERIFY(model.insertRow(2));
  QCOMPARE(controller.controllerState(), ControllerState::Inserting);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * End
   */
  QVERIFY(controller.submit());
  QCOMPARE(getModelData(model, 0, 0), QVariant("ABCD"));
//   index = model.index(0, 0);
//   QVERIFY(index.isValid());
//   QCOMPARE(model.data(index, Qt::DisplayRole).toString(), QString("ABCD"));
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

  return QTest::qExec(&test, argc, argv);
}
