/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "IntegralRangeBenchmark.h"
#include "Mdt/Numeric/IntegralRange.h"

using namespace Mdt::Numeric;

/*
 * Benchmarks
 */

void IntegralRangeBenchmark::setFirstLastBenchmark()
{
  IntegralRange<int> r;

  QBENCHMARK{
    r.setFirst(10);
    r.setLast(20);
  }
  QCOMPARE(r.first(), 10);
  QCOMPARE(r.last(), 20);
}

void IntegralRangeBenchmark::containsBenchmark()
{
  IntegralRange<int> r;

  r.setFirst(10);
  r.setLast(20);
  bool contains;
  QBENCHMARK{
    contains = r.contains(15);
  }
  QVERIFY(contains);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  IntegralRangeBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
