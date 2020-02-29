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
#include "MemoryVehicleTypeRepositoryTest.h"
#include "MemoryVehicleTypeRepository.h"
#include "Mdt/Railway/TaskList/TestLib/VehicleTypeRepositoryTestBase.h"
#include "Mdt/Railway/TaskList/VehicleType.h"
#include <QObject>
#include <QLatin1String>
#include <memory>
#include <vector>

#include <QDebug>

using namespace Mdt::Railway::TaskList;

void MemoryVehicleTypeRepositoryTest::initTestCase()
{
}

/*
 * Helpers
 */

/*
 * Tests
 */

void MemoryVehicleTypeRepositoryTest::addGetUpdateRemoveTest()
{
  MemoryVehicleTypeRepository repository;

  Mdt::Railway::TaskList::TestLib::addGetUpdateRemoveTest(repository);

  /*
   * Add, get
   */

//   VehicleType rbde1("RBDe 560 DO", "DOMINO", "1");
//   const auto rbde1Id = repository.add(rbde1);
//   QVERIFY(rbde1Id);
//   QVERIFY(!rbde1Id->isNull());
// 
//   auto vehicle = repository.getById(*rbde1Id);
//   QVERIFY(vehicle);
//   QCOMPARE(vehicle->id(), *rbde1Id);
//   QCOMPARE(vehicle->name(), QLatin1String("RBDe 560 DO"));
//   QCOMPARE(vehicle->alias(), QLatin1String("DOMINO"));
//   QCOMPARE(vehicle->manufacturerSerie(), QLatin1String("1"));
// 
//   /*
//    * Add more
//    * Will validate update and remove
//    */
// 
//   VehicleType dtzRAe("RAe 514", "DTZ", "1");
//   VehicleType dtzRBe("RBe 514", "DTZ", "1");
// 
//   const auto dtzRAeId = repository.add(dtzRAe);
//   const auto dtzRBeId = repository.add(dtzRBe);
//   QVERIFY(dtzRAeId);
//   QVERIFY(dtzRBeId);
// 
//   /*
//    * Update
//    */
// 
//   vehicle = repository.getById(*rbde1Id);
//   QVERIFY(vehicle);
//   vehicle->setName("RBDe 560 MPZ");
//   QVERIFY(repository.update(*vehicle));
// 
//   vehicle = repository.getById(*rbde1Id);
//   QVERIFY(vehicle);
//   QCOMPARE(vehicle->name(), QLatin1String("RBDe 560 MPZ"));
//   QCOMPARE(vehicle->alias(), QLatin1String("DOMINO"));
//   QCOMPARE(vehicle->manufacturerSerie(), QLatin1String("1"));
// 
//   vehicle = repository.getById(*rbde1Id);
//   QVERIFY(vehicle);
//   QCOMPARE(vehicle->name(), QLatin1String("RBDe 560 MPZ"));
// 
//   vehicle = repository.getById(*dtzRAeId);
//   QVERIFY(vehicle);
//   QCOMPARE(vehicle->name(), QLatin1String("RAe 514"));
// 
//   vehicle = repository.getById(*dtzRBeId);
//   QVERIFY(vehicle);
//   QCOMPARE(vehicle->name(), QLatin1String("RBe 514"));
// 
//   /*
//    * Remove
//    */
// 
//   QVERIFY(repository.remove(*dtzRAeId));
// 
//   vehicle = repository.getById(*rbde1Id);
//   QVERIFY(vehicle);
//   QCOMPARE(vehicle->name(), QLatin1String("RBDe 560 MPZ"));
// 
//   vehicle = repository.getById(*dtzRAeId);
//   QVERIFY(!vehicle);
// 
//   vehicle = repository.getById(*dtzRBeId);
//   QVERIFY(vehicle);
//   QCOMPARE(vehicle->name(), QLatin1String("RBe 514"));
// 
//   QVERIFY(repository.removeAll());
// 
//   vehicle = repository.getById(*rbde1Id);
//   QVERIFY(!vehicle);
// 
//   vehicle = repository.getById(*dtzRAeId);
//   QVERIFY(!vehicle);
// 
//   vehicle = repository.getById(*dtzRBeId);
//   QVERIFY(!vehicle);
}

void MemoryVehicleTypeRepositoryTest::getAllAsyncTest()
{
  MemoryVehicleTypeRepository repository;

  Mdt::Railway::TaskList::TestLib::getAllAsyncTest(repository);

//   VehicleType rbde1("RBDe 560 DO", "DOMINO", "1");
//   VehicleType rae514("RAe 514", "DTZ", "1");
//   QVERIFY(repository.add(rbde1));
//   QVERIFY(repository.add(rae514));
// 
//   
// 
//   QFAIL("Not complete");
}

void MemoryVehicleTypeRepositoryTest::getAllAsyncTest_data()
{
  Mdt::Railway::TaskList::TestLib::getAllAsyncTestBuildDataSet();
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
  MemoryVehicleTypeRepositoryTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
