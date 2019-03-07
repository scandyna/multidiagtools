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
#include "SqlVehicleTypeRepositoryTest.h"
#include "Mdt/Railway/TaskList/SqlVehicleTypeRepository.h"
#include "Mdt/Railway/TaskList/VehicleType.h"
#include <QObject>
#include <memory>
#include <vector>

#include <QDebug>

using namespace Mdt::Railway::TaskList;

void SqlVehicleTypeRepositoryTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  QVERIFY(createTestSchema());
}

/*
 * Helpers
 */

/*
 * Tests
 */

void SqlVehicleTypeRepositoryTest::addGetTest()
{
  SqlVehicleTypeRepository repository;
  repository.setDatabase(database());

  VehicleType rbde1("RBDe 560 DO", "DOMINO", "1");
  const auto rbde1Id = repository.add(rbde1);
  QVERIFY(rbde1Id);
  QVERIFY(!rbde1Id->isNull());

  auto vehicle = repository.getById(*rbde1Id);
  QVERIFY(vehicle);
  QCOMPARE(vehicle->name(), QString("RBDe 560 DO"));
}

void SqlVehicleTypeRepositoryTest::getAllAsyncTest()
{
  SqlVehicleTypeRepository repository;
  repository.setDatabase(database());

  VehicleType rbde1("RBDe 560 DO", "DOMINO", "1");
  VehicleType rae514("RAe 514", "DTZ", "1");
  QVERIFY(repository.add(rbde1));
  QVERIFY(repository.add(rae514));

  

  QFAIL("Not complete");
}

/*
 * Helpers
 */

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SqlVehicleTypeRepositoryTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
