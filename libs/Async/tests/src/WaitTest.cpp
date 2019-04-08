/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "WaitTest.h"
#include "TestTask.h"
#include "Mdt/Async/Wait.h"
#include "Mdt/Async/WaitDonePredicate.h"

/*
 * Tests
 */

void WaitTest::waitPredicateTest()
{
  using namespace std::chrono_literals;

  /*
   * Construct and first use
   */

  Mdt::Async::WaitDonePredicate pred1;
  QVERIFY(!pred1.isFinished());
  QVERIFY(!pred1.hasTimedOut());

  pred1.setDone();
  QVERIFY(pred1.isFinished());
  QVERIFY(!pred1.hasTimedOut());

  pred1.reset(0ms);
  QTRY_VERIFY(pred1.isFinished());
  QVERIFY(pred1.hasTimedOut());

  Mdt::Async::WaitDonePredicate pred2;
  QVERIFY(!pred2.isFinished());
  QVERIFY(!pred2.hasTimedOut());

  pred2.reset(0ms);
  QTRY_VERIFY(pred2.isFinished());
  QVERIFY(pred2.hasTimedOut());

  pred2.setDone();
  QVERIFY(pred2.isFinished());
  QVERIFY(pred2.hasTimedOut());

  /*
   * Re-use: predicate can be hold by the user
   */

  pred2.reset(5000ms);
  QVERIFY(!pred2.isFinished());
  QVERIFY(!pred2.hasTimedOut());

  pred2.setDone();
  QVERIFY(pred2.isFinished());
  QVERIFY(!pred2.hasTimedOut());

  pred2.reset(200ms);
  QVERIFY(!pred2.isFinished());
  QVERIFY(!pred2.hasTimedOut());
  QTRY_VERIFY(pred2.isFinished());
  QVERIFY(pred2.hasTimedOut());
}

void WaitTest::waitTest()
{
  using namespace std::chrono_literals;
  using Mdt::Async::WaitDonePredicate;

  TestTask task;
  WaitDonePredicate pred;
  connect(&task, &TestTask::done, &pred, &WaitDonePredicate::setDone);

  task.setDoneIn(0ms);
  QVERIFY( Mdt::Async::wait(pred, 5s) );
  QVERIFY(pred.isFinished());
  QVERIFY(!pred.hasTimedOut());

  task.setDoneIn(50ms);
  QVERIFY( Mdt::Async::wait(pred, 5s) );
  QVERIFY(pred.isFinished());
  QVERIFY(!pred.hasTimedOut());

  task.setDoneIn(5s);
  QVERIFY( !Mdt::Async::wait(pred, 0ms) );
  QVERIFY(pred.isFinished());
  QVERIFY(pred.hasTimedOut());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  WaitTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
