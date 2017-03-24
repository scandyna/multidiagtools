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
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include "Mdt/ItemEditor/AbstractControllerStateChain.h"
#include "Mdt/ItemEditor/AbstractControllerStatePermission.h"
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

class StateChainTestClass : public AbstractControllerStateChain
{
 public:

  ControllerState dataEditionStartedState(ControllerState currentState) const override
  {
    return ControllerState::Editing;
  }
};

/*
 * Tests
 */

void ControllerStateMachineTest::constructTest()
{
  QScopedPointer<ControllerStateMachine> s( ControllerStateMachine::makeNew<StateChainTestClass, PermissionTestClass>() );
  QCOMPARE(s->currentState(), ControllerState::Visualizing);
  QVERIFY(s->canChangeCurrentRow());
  QVERIFY(!s->canInsert());
}

void ControllerStateMachineTest::currentStateChangedSignalTest()
{
  QScopedPointer<ControllerStateMachine> stateMachine( ControllerStateMachine::makeNew<StateChainTestClass, PermissionTestClass>() );
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
  QScopedPointer<ControllerStateMachine> s( ControllerStateMachine::makeNew<StateChainTestClass, PermissionTestClass>() );

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
