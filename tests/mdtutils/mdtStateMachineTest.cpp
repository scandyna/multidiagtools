/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtStateMachineTest.h"
#include "mdtState.h"
#include "mdtStateMachine.h"
#include "mdtApplication.h"
#include <QTest>
#include <QString>
#include <QTimer>
#include <QVector>

#include <atomic>

#include <QDebug>

void mdtStateMachineTest::sandbox()
{
  std::atomic<int> i;

  i = 25;
  qDebug() << "i: " << i;
  qDebug() << "i: " << i.load();
}


void mdtStateMachineTest::stateTest()
{
  mdtState s(1);

  // Check initial values
  QCOMPARE(s.id(), 1);
  QCOMPARE(s.text(), QString());
  QCOMPARE(s.notifyEnteredToUi(), false);
  // Set/Get
  s.setId(25);
  QCOMPARE(s.id(), 25);
  s.setText("State 1");
  QCOMPARE(s.text(), QString("State 1"));
  s.setNotifyEnteredToUi(true);
  QCOMPARE(s.notifyEnteredToUi(), true);
}

void mdtStateMachineTest::stateMachineTest()
{
  mdtStateMachine sm;
  mdtState *s1, *s2;
  QTimer event;
  QVector<int> states;

  event.setSingleShot(true);

  // Build a simple OFF-ON-OFF-... like state machine
  s1 = new mdtState(1);
  s2 = new mdtState(2);
  s1->addTransition(&event, SIGNAL(timeout()), s2);
  s2->addTransition(&event, SIGNAL(timeout()), s1);
  sm.addState(s1);
  sm.addState(s2);
  sm.setInitialState(s1);
  // Check initial state
  QCOMPARE(sm.currentState(), -1);
  // Start
  sm.start(true);
  QCOMPARE(sm.currentState(), 1);
  // Stop
  sm.stop(true);
  // Start
  sm.start(true);
  QCOMPARE(sm.currentState(), 1);
  // Check s1->s2 transition
  event.start(100);
  QVERIFY(sm.waitOnState(2));
  QCOMPARE(sm.currentState(), 2);
  // Check s2->s1 transition
  event.start(100);
  QVERIFY(sm.waitOnState(1));
  QCOMPARE(sm.currentState(), 1);
  // Check timeout
  QVERIFY(!sm.waitOnState(55, 100));
  QVERIFY(sm.waitOnState(1));
  QCOMPARE(sm.currentState(), 1);
  QVERIFY(sm.waitOnState(1, 100));
  QCOMPARE(sm.currentState(), 1);
  // Check s1->s2 transition with timeout
  event.start(100);
  QVERIFY(sm.waitOnState(2, 200));
  QCOMPARE(sm.currentState(), 2);
  // Check s2->s1 transition
  event.start(100);
  QVERIFY(sm.waitOnState(1, 200));
  QCOMPARE(sm.currentState(), 1);
  // Check that waiting on s2 fails
  QVERIFY(!sm.waitOnState(2, 100));
  // Check waitOnOneState
  states.clear();
  states << 1 << 2;
  QVERIFY(sm.waitOnOneState(states, 50));
  // Go back to s2
  event.start(50);
  QVERIFY(sm.waitOnOneState(states, 100));
}

void mdtStateMachineTest::subMachineTest()
{
  mdtStateMachine sm;
  mdtState *s1, *s11, *s12;
  QTimer event;

  event.setSingleShot(true);

  // Build state machine with global state s1 und child states
  s1 = new mdtState(1);
  s11 = new mdtState(11, s1);
  s12 = new mdtState(12, s1);
  s11->addTransition(&event, SIGNAL(timeout()), s12);
  s12->addTransition(&event, SIGNAL(timeout()), s11);
  s1->setInitialState(s11);
  sm.addState(s1);
  sm.setInitialState(s1);
  // Check initial state
  QCOMPARE(sm.currentState(), -1);
  // Start
  sm.start(true);
  QCOMPARE(sm.currentState(), 11);
  // Check s11 -> s12 transition
  event.start(100);
  QVERIFY(sm.waitOnState(12, 100));
  QCOMPARE(sm.currentState(), 12);
  // Check s12 -> s11 transition
  event.start(100);
  QVERIFY(sm.waitOnState(11, 100));
  QCOMPARE(sm.currentState(), 11);
}


/* Main */

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtStateMachineTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
