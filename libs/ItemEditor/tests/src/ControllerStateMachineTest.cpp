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
#include "ControllerStateMachineTest.h"
// #include "ControllerStateChangedSignalSpy.h"
#include "Mdt/ItemEditor/AbstractControllerStatePermission.h"
#include "Mdt/ItemEditor/AbstractControllerStateTable.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include <QSignalSpy>
#include <QScopedPointer>

using namespace Mdt::ItemEditor;

/*
 * Init/Cleanup
 */

void ControllerStateMachineTest::initTestCase()
{
}

void ControllerStateMachineTest::cleanupTestCase()
{
}

/*
 * Test classes
 */

class PermissionTestClass : public AbstractControllerStatePermission
{
 public:

  bool canChangeCurrentRow(const AbstractControllerStateTable &) const override
  {
    return true;
  }

  bool canInsert(const AbstractControllerStateTable &) const override
  {
    return false;
  }
};

/*
 * Tests
 */

void ControllerStateMachineTest::stateTableRowTest()
{
  ControllerStateTableRow row(ControllerState::Visualizing, ControllerEvent::DataEditionStarted, ControllerState::Editing);
  QCOMPARE(row.state(), ControllerState::Visualizing);
  QCOMPARE(row.event(), ControllerEvent::DataEditionStarted);
  QCOMPARE(row.targetState(), ControllerState::Editing);
  QVERIFY(row.canHandleEvent(ControllerEvent::DataEditionStarted));
  QVERIFY(!row.canHandleEvent(ControllerEvent::SubmitDone));
}

void ControllerStateMachineTest::abstractControllerStateTableForceStateTest()
{
  AbstractControllerStateTable table;
  table.createTable();
  QCOMPARE(table.currentState(), ControllerState::Visualizing);
  table.forceCurrentState(ControllerState::Editing);
  QCOMPARE(table.currentState(), ControllerState::Editing);
  table.forceCurrentState(ControllerState::Inserting);
  QCOMPARE(table.currentState(), ControllerState::Inserting);
  table.forceCurrentState(ControllerState::Visualizing);
  QCOMPARE(table.currentState(), ControllerState::Visualizing);
  table.forceCurrentState(ControllerState::ParentEditing);
  QCOMPARE(table.currentState(), ControllerState::ParentEditing);
  table.forceCurrentState(ControllerState::ChildEditing);
  QCOMPARE(table.currentState(), ControllerState::ChildEditing);
}

void ControllerStateMachineTest::abstractControllerStateTableTransitionTest()
{
  /*
   * Initial state
   */
  AbstractControllerStateTable table;
  table.createTable();
  // Visualizing - Editing
  QCOMPARE(table.currentState(), ControllerState::Visualizing);
  table.setEvent(ControllerEvent::DataEditionStarted);
  QCOMPARE(table.currentState(), ControllerState::Editing);
  table.setEvent(ControllerEvent::DataEditionDone);
  QCOMPARE(table.currentState(), ControllerState::Visualizing);
  table.setEvent(ControllerEvent::DataEditionStarted);
  QCOMPARE(table.currentState(), ControllerState::Editing);
  table.setEvent(ControllerEvent::SubmitDone);
  QCOMPARE(table.currentState(), ControllerState::Visualizing);
  table.setEvent(ControllerEvent::DataEditionStarted);
  QCOMPARE(table.currentState(), ControllerState::Editing);
  table.setEvent(ControllerEvent::RevertDone);
  QCOMPARE(table.currentState(), ControllerState::Visualizing);
  // Visualizing - Inserting
  table.setEvent(ControllerEvent::InsertStarted);
  QCOMPARE(table.currentState(), ControllerState::Inserting);
  table.setEvent(ControllerEvent::SubmitDone);
  QCOMPARE(table.currentState(), ControllerState::Visualizing);
  table.setEvent(ControllerEvent::InsertStarted);
  QCOMPARE(table.currentState(), ControllerState::Inserting);
  table.setEvent(ControllerEvent::RemoveDone);
  QCOMPARE(table.currentState(), ControllerState::Visualizing);
  /*
   * Complete test is done below (using ControllerStateMachine)
   */
}

void ControllerStateMachineTest::abstractControllerStatePermissionTest()
{
  AbstractControllerStatePermission permission;
  AbstractControllerStateTable table;

  table.createTable();
  // Visualizing state
  table.forceCurrentState(ControllerState::Visualizing);
  QVERIFY( permission.canChangeCurrentRow(table) );
  QVERIFY( permission.isChangeCurrentRowActionEnabled(table) );
  QVERIFY( permission.canInsert(table) );
  QVERIFY( permission.isInsertActionEnabled(table) );
  QVERIFY( permission.canEdit(table) );
  QVERIFY( permission.canSubmit(table) );
  QVERIFY(!permission.isSubmitActionEnabled(table) );
  QVERIFY(!permission.canRevert(table) );
  QVERIFY(!permission.isRevertActionEnabled(table) );
  QVERIFY( permission.canRemove(table) );
  QVERIFY( permission.isRemoveActionEnabled(table) );
  /*
   * Complete test is done below (using ControllerStateMachine)
   */
}

void ControllerStateMachineTest::constructTest()
{
  QScopedPointer<ControllerStateMachine> s( ControllerStateMachine::makeNew<AbstractControllerStateTable, PermissionTestClass>() );
  QCOMPARE(s->currentState(), ControllerState::Visualizing);
  QVERIFY(s->canChangeCurrentRow());
  QVERIFY(!s->canInsert());
}

/*
 * Here we check the complete state machine with implementations for AbstractController
 */
void ControllerStateMachineTest::transitionTest()
{
  /*
   * Initial state
   */
  QScopedPointer<ControllerStateMachine> stateMachine( ControllerStateMachine::makeNew<AbstractControllerStateTable, AbstractControllerStatePermission>() );
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  // Visualizing - Editing
  stateMachine->dataEditionStarted();
  QCOMPARE(stateMachine->currentState(), ControllerState::Editing);
  stateMachine->dataEditionDone();
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
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
  stateMachine->submitDone();
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  stateMachine->insertStarted();
  QCOMPARE(stateMachine->currentState(), ControllerState::Inserting);
  stateMachine->removeDone();
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  
  QFAIL("Not complete");
}

/*
 * Here we check permissions for all states with implementations for AbstractController
 */
void ControllerStateMachineTest::permissionTest()
{
  /*
   * Initial state
   */
  QScopedPointer<ControllerStateMachine> stateMachine( ControllerStateMachine::makeNew<AbstractControllerStateTable, AbstractControllerStatePermission>() );
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  /*
   * Visualizing state
   */
  QVERIFY( stateMachine->canChangeCurrentRow() );
  QVERIFY( stateMachine->canInsert() );
  QVERIFY( stateMachine->canEdit() );
  QVERIFY( stateMachine->canSubmit() );
  QVERIFY(!stateMachine->canRevert() );
  QVERIFY( stateMachine->canRemove() );
  //QVERIFY( stateMachine->canSelect() );
  /*
   * Editing state
   */
  stateMachine->forceCurrentState(ControllerState::Editing);
  QCOMPARE(stateMachine->currentState(), ControllerState::Editing);
  QVERIFY(!stateMachine->canChangeCurrentRow() );
  QVERIFY(!stateMachine->canInsert() );
  QVERIFY( stateMachine->canEdit() );
  QVERIFY( stateMachine->canSubmit() );
  QVERIFY( stateMachine->canRevert() );
  QVERIFY(!stateMachine->canRemove() );
  //QVERIFY( stateMachine->canSelect() );
  /*
   * Inserting state
   */
  stateMachine->forceCurrentState(ControllerState::Inserting);
  QCOMPARE(stateMachine->currentState(), ControllerState::Inserting);
  QVERIFY(!stateMachine->canChangeCurrentRow() );
  QVERIFY(!stateMachine->canInsert() );
  QVERIFY( stateMachine->canEdit() );
  QVERIFY( stateMachine->canSubmit() );
  QVERIFY(!stateMachine->canRevert() );
  QVERIFY( stateMachine->canRemove() );
  //QVERIFY( stateMachine->canSelect() );


  QFAIL("Not complete");
}

/*
 * Here we check actions enable state for all states with implementations for AbstractController
 */
void ControllerStateMachineTest::actionEnableStateTest()
{
  /*
   * Initial state
   */
  QScopedPointer<ControllerStateMachine> stateMachine( ControllerStateMachine::makeNew<AbstractControllerStateTable, AbstractControllerStatePermission>() );
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  /*
   * Visualizing state
   */
  QVERIFY( stateMachine->isChangeCurrentRowActionEnabled() );
  QVERIFY( stateMachine->isInsertActionEnabled() );
  QVERIFY(!stateMachine->isSubmitActionEnabled() );
  QVERIFY(!stateMachine->isRevertActionEnabled() );
  QVERIFY( stateMachine->isRemoveActionEnabled() );
  //QVERIFY( stateMachine->isSelectActionEnabled() );
  /*
   * Editing state
   */
  stateMachine->forceCurrentState(ControllerState::Editing);
  QCOMPARE(stateMachine->currentState(), ControllerState::Editing);
  QVERIFY(!stateMachine->isChangeCurrentRowActionEnabled() );
  QVERIFY(!stateMachine->isInsertActionEnabled() );
  QVERIFY( stateMachine->isSubmitActionEnabled() );
  QVERIFY( stateMachine->isRevertActionEnabled() );
  QVERIFY(!stateMachine->isRemoveActionEnabled() );
  //QVERIFY( stateMachine->isSelectActionEnabled() );
  /*
   * Inserting state
   */
  stateMachine->forceCurrentState(ControllerState::Inserting);
  QCOMPARE(stateMachine->currentState(), ControllerState::Inserting);
  QVERIFY(!stateMachine->isChangeCurrentRowActionEnabled() );
  QVERIFY(!stateMachine->isInsertActionEnabled() );
  QVERIFY( stateMachine->isSubmitActionEnabled() );
  QVERIFY(!stateMachine->isRevertActionEnabled() );
  QVERIFY( stateMachine->isRemoveActionEnabled() );
  //QVERIFY( stateMachine->isSelectActionEnabled() );


  QFAIL("Not complete");
}

void ControllerStateMachineTest::currentStateChangedSignalTest()
{
  QScopedPointer<ControllerStateMachine> stateMachine( ControllerStateMachine::makeNew<AbstractControllerStateTable, PermissionTestClass>() );
  QSignalSpy stateChangedSpy(stateMachine.data(), &ControllerStateMachine::currentStateChanged);
  QVERIFY(stateChangedSpy.isValid());
  /*
   * Initial state
   */
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  QCOMPARE(stateChangedSpy.count(), 0);
  // Change state
  stateMachine->forceCurrentState(ControllerState::Editing);
  QCOMPARE(stateMachine->currentState(), ControllerState::Editing);
  QCOMPARE(stateChangedSpy.count(), 1);
  stateChangedSpy.clear();
  // Set same state again
  stateMachine->forceCurrentState(ControllerState::Editing);
  QCOMPARE(stateChangedSpy.count(), 0);
}

void ControllerStateMachineTest::getterBenschmark()
{
  QScopedPointer<ControllerStateMachine> s( ControllerStateMachine::makeNew<AbstractControllerStateTable, PermissionTestClass>() );

  QBENCHMARK{
    QCOMPARE(s->currentState(), ControllerState::Visualizing);
    QVERIFY(s->canChangeCurrentRow());
    QVERIFY(!s->canInsert());
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ControllerStateMachineTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
