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
#include "AbstractControllerStateChainTest.h"
#include "Mdt/ItemEditor/AbstractControllerStateChain.h"
#include "Mdt/ItemEditor/AbstractControllerStatePermission.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include <QScopedPointer>

using namespace Mdt::ItemEditor;

/*
 * Init/Cleanup
 */

void AbstractControllerStateChainTest::initTestCase()
{
}

void AbstractControllerStateChainTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void AbstractControllerStateChainTest::stateChainTest()
{
  AbstractControllerStateChain sc;

  // From Visualizing state
  QCOMPARE(sc.dataEditionStartedState(ControllerState::Visualizing), ControllerState::Editing);
  QCOMPARE(sc.dataEditionDoneState(ControllerState::Visualizing), ControllerState::Visualizing);
  QCOMPARE(sc.submitDoneState(ControllerState::Visualizing), ControllerState::Visualizing);
  QCOMPARE(sc.revertDoneState(ControllerState::Visualizing), ControllerState::Visualizing);
  QCOMPARE(sc.insertStartedState(ControllerState::Visualizing), ControllerState::Inserting);
  QCOMPARE(sc.removeDoneState(ControllerState::Visualizing), ControllerState::Visualizing);
  // From Editing state
  QCOMPARE(sc.dataEditionStartedState(ControllerState::Editing), ControllerState::Editing);
  QCOMPARE(sc.dataEditionDoneState(ControllerState::Editing), ControllerState::Visualizing);
  QCOMPARE(sc.submitDoneState(ControllerState::Editing), ControllerState::Visualizing);
  QCOMPARE(sc.revertDoneState(ControllerState::Editing), ControllerState::Visualizing);
  QCOMPARE(sc.insertStartedState(ControllerState::Editing), ControllerState::Editing);
  QCOMPARE(sc.removeDoneState(ControllerState::Editing), ControllerState::Visualizing);
  // From Inserting state
  QCOMPARE(sc.dataEditionStartedState(ControllerState::Inserting), ControllerState::Inserting);
  QCOMPARE(sc.dataEditionDoneState(ControllerState::Inserting), ControllerState::Inserting);
  QCOMPARE(sc.submitDoneState(ControllerState::Inserting), ControllerState::Visualizing);
  QCOMPARE(sc.revertDoneState(ControllerState::Inserting), ControllerState::Inserting);
  QCOMPARE(sc.insertStartedState(ControllerState::Inserting), ControllerState::Inserting);
  QCOMPARE(sc.removeDoneState(ControllerState::Inserting), ControllerState::Visualizing);

  QFAIL("Not complete (Miss ParentEditing + ChildEditing states)");
}

void AbstractControllerStateChainTest::stateMachineTest()
{
  /*
   * Initial state
   */
  QScopedPointer<ControllerStateMachine> s( ControllerStateMachine::makeNew<AbstractControllerStateChain, AbstractControllerStatePermission>(nullptr) );
  QCOMPARE(s->currentState(), ControllerState::Visualizing);
  /*
   * Just check that each trigger calls the correct method in AbstractControllerStateChain
   */
  s->dataEditionStarted();
  QCOMPARE(s->currentState(), ControllerState::Editing);
  s->dataEditionDone();
  QCOMPARE(s->currentState(), ControllerState::Visualizing);
  s->dataEditionStarted();
  QCOMPARE(s->currentState(), ControllerState::Editing);
  s->submitDone();
  QCOMPARE(s->currentState(), ControllerState::Visualizing);
  s->dataEditionStarted();
  QCOMPARE(s->currentState(), ControllerState::Editing);
  s->revertDone();
  QCOMPARE(s->currentState(), ControllerState::Visualizing);
  s->insertStarted();
  QCOMPARE(s->currentState(), ControllerState::Inserting);
  s->submitDone();
  QCOMPARE(s->currentState(), ControllerState::Visualizing);
  s->dataEditionStarted();
  QCOMPARE(s->currentState(), ControllerState::Editing);
  s->removeDone();
  QCOMPARE(s->currentState(), ControllerState::Visualizing);

  QFAIL("Not complete (Miss ParentEditing + ChildEditing states)");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  AbstractControllerStateChainTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
