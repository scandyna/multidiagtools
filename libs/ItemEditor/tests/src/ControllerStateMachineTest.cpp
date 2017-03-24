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
  /*
   * Contruct a null state machine
   */
//   ControllerStateMachine s0;
//   QVERIFY(s0.isNull());
//   QVERIFY(!s0.canChangeCurrentRow());
//   QVERIFY(!s0.canInsert());
  /*
   * Controller state machine with test classes implementations
   */
  QScopedPointer<ControllerStateMachine> s( ControllerStateMachine::makeNew<StateChainTestClass, PermissionTestClass>() );
//   auto *s = ControllerStateMachine::makeNew<StateChainTestClass, PermissionTestClass>();
//   QVERIFY(!s.isNull());
  QVERIFY(s->canChangeCurrentRow());
  QVERIFY(!s->canInsert());
}

// void ControllerStateMachineTest::copyTest()
// {
//   auto s1 = ControllerStateMachine::make<StateChainTestClass, PermissionTestClass>();
//   QVERIFY(s1.canChangeCurrentRow());
//   QVERIFY(!s1.canInsert());
//   /*
//    * Copy construct
//    */
//   auto s2 = s1;
//   QVERIFY(s2.canChangeCurrentRow());
//   QVERIFY(!s2.canInsert());
//   /*
//    * Copy assign
//    */
//   ControllerStateMachine s3;
//   // Make shure s3 is used before assign
//   QVERIFY(!s3.canChangeCurrentRow());
//   s3 = s1;
//   QVERIFY(s3.canChangeCurrentRow());
//   QVERIFY(!s3.canInsert());
//   /*
//    * Move construct
//    */
//   auto s4 = std::move( ControllerStateMachine::make<StateChainTestClass, PermissionTestClass>() );
//   QVERIFY(s4.canChangeCurrentRow());
//   QVERIFY(!s4.canInsert());
//   /*
//    * Move assign
//    */
//   ControllerStateMachine s5;
//   // Make shure s5 is used before assign
//   QVERIFY(!s5.canChangeCurrentRow());
//   s5 = std::move( ControllerStateMachine::make<StateChainTestClass, PermissionTestClass>() );
//   QVERIFY(s5.canChangeCurrentRow());
//   QVERIFY(!s5.canInsert());
// }

void ControllerStateMachineTest::currentStateChangedSignalTest()
{
//   auto stateMachine = ControllerStateMachine::make<StateChainTestClass, PermissionTestClass>();
  QScopedPointer<ControllerStateMachine> stateMachine( ControllerStateMachine::makeNew<StateChainTestClass, PermissionTestClass>() );
//   QSignalSpy stateChangedSpy(stateMachine.stateChain(), &AbstractControllerStateChain::currentStateChanged);
//   QVERIFY(stateChangedSpy.isValid());

  /*
   * Initial state
   */
  ///QCOMPARE(stateMachine.
  //stateMachine
  QFAIL("Not complete");
}

void ControllerStateMachineTest::getterBenschmark()
{
  QScopedPointer<ControllerStateMachine> s( ControllerStateMachine::makeNew<StateChainTestClass, PermissionTestClass>() );
//   auto s = ControllerStateMachine::make<StateChainTestClass, PermissionTestClass>();
  QBENCHMARK{
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
