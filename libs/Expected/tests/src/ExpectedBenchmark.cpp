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
#include "ExpectedBenchmark.h"
#include "Mdt/CoreApplication.h"
#include "Mdt/Expected.h"

// #include <QDebug>

void ExpectedBenchmark::initTestCase()
{
}

void ExpectedBenchmark::cleanupTestCase()
{
}

/*
 * Benchmarks
 */

void ExpectedBenchmark::getIntBenchmark()
{
  int y;
  QBENCHMARK{
    const auto x = getValue(50);
    y = x;
  }
  QCOMPARE(y, 50);
}

void ExpectedBenchmark::getExpectedIntBenchmark()
{
  int y;
  QBENCHMARK{
    const auto x = getExpectedValue(50);
    y = *x;
  }
  QCOMPARE(y, 50);
}

void ExpectedBenchmark::getAndCheckExpectedIntBenchmark()
{
  int y;
  QBENCHMARK{
    const auto x = getExpectedValue(50);
    QVERIFY(x);
    y = *x;
  }
  QCOMPARE(y, 50);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ExpectedBenchmark test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
