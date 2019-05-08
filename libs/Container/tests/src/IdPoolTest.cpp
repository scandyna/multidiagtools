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
#include "IdPoolTest.h"
#include "Mdt/Container/IdPool.h"

using namespace Mdt::Container;

/*
 * Tests
 */

void IdPoolTest::getAndReleaseTest()
{
  IdPool<int> pool;

  QCOMPARE(pool.getNextAvailableId(), 1);
  QCOMPARE(pool.getNextAvailableId(), 2);
  QCOMPARE(pool.getNextAvailableId(), 3);

  pool.releaseId(3);
  // Used Iid: [1,2]
  QCOMPARE(pool.getNextAvailableId(), 3);
  // Used Iid: [1,2,3]

  pool.releaseId(2);
  // Used Iid: [1,3]
  QCOMPARE(pool.getNextAvailableId(), 2);
  QCOMPARE(pool.getNextAvailableId(), 4);
  // Used Iid: [1,2,3,4]

  pool.releaseId(1);
  // Used Iid: [2,3,4]
  QCOMPARE(pool.getNextAvailableId(), 1);
  QCOMPARE(pool.getNextAvailableId(), 5);
  // Used Iid: [1,2,3,4,5]

  pool.releaseId(1);
  pool.releaseId(3);
  pool.releaseId(2);
  // Used Iid: [4,5]
  QCOMPARE(pool.getNextAvailableId(), 1);
  QCOMPARE(pool.getNextAvailableId(), 2);
  QCOMPARE(pool.getNextAvailableId(), 3);
  QCOMPARE(pool.getNextAvailableId(), 6);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
    IdPoolTest test;

  return QTest::qExec(&test, argc, argv);
}

