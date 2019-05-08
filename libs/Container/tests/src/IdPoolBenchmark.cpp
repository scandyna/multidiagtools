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
#include "IdPoolBenchmark.h"
#include "Mdt/Container/IdPool.h"

using namespace Mdt::Container;

/*
 * Benchmarks
 */

void IdPoolBenchmark::getAndRelease5Benchmark()
{
  IdPool<int> pool;
  int id1, id2, id3, id4, id5;

  QBENCHMARK{
    id1 = pool.getNextAvailableId();
    id2 = pool.getNextAvailableId();
    id3 = pool.getNextAvailableId();
    id4 = pool.getNextAvailableId();
    id5 = pool.getNextAvailableId();
    pool.releaseId(id3);
    pool.releaseId(id1);
    pool.releaseId(id5);
    pool.releaseId(id2);
    pool.releaseId(id4);
  }

  QCOMPARE(pool.getNextAvailableId(), 1);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
    IdPoolBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
