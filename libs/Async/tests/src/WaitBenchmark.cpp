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
#include "WaitBenchmark.h"
#include <QTimer>

/*
 * Helpers
 */

class TestTimeout : public QObject
{
 public:

  bool hasTimedOut() const noexcept
  {
    return mTimedOut;
  }

 public slots:

  void setTimedOut()
  {
    mTimedOut = true;
  }

 private:

  bool mTimedOut = false;
};

/*
 * Benchmarks
 */

void WaitBenchmark::connectDisconnect()
{
  TestTimeout t;
  QTimer timer;
  timer.setSingleShot(true);

  QBENCHMARK{
    connect(&timer, &QTimer::timeout, &t, &TestTimeout::setTimedOut);
    disconnect(&timer, &QTimer::timeout, &t, &TestTimeout::setTimedOut);
  }
}

void WaitBenchmark::qObjectNewDelete()
{
  QBENCHMARK{
    QObject *obj = nullptr;
    obj = new QTimer;
    QVERIFY(obj != nullptr);
    delete obj;
  }
}

void WaitBenchmark::qTimerStaticSingleShot()
{
  TestTimeout t;

  QVERIFY(!t.hasTimedOut());
  QBENCHMARK{
    QTimer::singleShot(0, &t, &TestTimeout::setTimedOut);
    QTRY_VERIFY(t.hasTimedOut());
  }
}

void WaitBenchmark::qTimerObjectSingleShot()
{
  TestTimeout t;
  QTimer timer;
  timer.setSingleShot(true);
  connect(&timer, &QTimer::timeout, &t, &TestTimeout::setTimedOut);

  QVERIFY(!t.hasTimedOut());
  QBENCHMARK{
    timer.start(0);
    QTRY_VERIFY(t.hasTimedOut());
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  WaitBenchmark test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
