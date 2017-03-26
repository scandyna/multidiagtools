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
// #include "Mdt/ItemEditor/AbstractControllerStateChain.h"
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

  bool canChangeCurrentRow(ControllerState) const override
  {
    return true;
  }

  bool canInsert(ControllerState) const override
  {
    return false;
  }
};

// class StateChainTestClass : public AbstractControllerStateChain
// {
//  public:
// 
//   ControllerState dataEditionStartedState(ControllerState currentState) const override
//   {
//     return ControllerState::Editing;
//   }
// };

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

  QFAIL("Not complete");
}

void ControllerStateMachineTest::abstractControllerStateTablePermissionTest()
{
  AbstractControllerStateTable table;

  table.createTable();
  // Visualizing state
  QCOMPARE(table.currentState(), ControllerState::Visualizing);
  QVERIFY( table.canHandleEvent(ControllerEvent::DataEditionStarted));
  QVERIFY(!table.canHandleEvent(ControllerEvent::DataEditionDone));
  QVERIFY(!table.canHandleEvent(ControllerEvent::SubmitDone));
  QVERIFY(!table.canHandleEvent(ControllerEvent::RevertDone));
  QVERIFY( table.canHandleEvent(ControllerEvent::InsertStarted));
  QVERIFY( table.canHandleEvent(ControllerEvent::RemoveDone));
  
  QFAIL("Not complete");
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
  QVERIFY( permission.canRevert(table) );
  QVERIFY(!permission.isRevertActionEnabled(table) );
  QVERIFY( permission.canRemove(table) );
  QVERIFY( permission.isRemoveActionEnabled(table) );
  
  QFAIL("Not complete");
}

void ControllerStateMachineTest::constructTest()
{
  QScopedPointer<ControllerStateMachine> s( ControllerStateMachine::makeNew<AbstractControllerStateTable, PermissionTestClass>() );
  QCOMPARE(s->currentState(), ControllerState::Visualizing);
  QVERIFY(s->canChangeCurrentRow());
  QVERIFY(!s->canInsert());
}

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
  
  QFAIL("Not complete");
}

void ControllerStateMachineTest::permissionTest()
{
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
