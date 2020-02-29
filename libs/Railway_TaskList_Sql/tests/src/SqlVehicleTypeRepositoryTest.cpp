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
#include "Mdt/Railway/TaskList/TestLib/VehicleTypeRepositoryTestBase.h"
#include "Mdt/Railway/TaskList/VehicleType.h"
#include <QObject>
#include <QLatin1String>
#include <memory>
#include <vector>

/// \todo Just for sandbox
#include "Mdt/Railway/TaskList/VehicleType_p.h"

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

void SqlVehicleTypeRepositoryTest::sandbox()
{
  VehicleTypeDataStruct data{1,"N","A","MS"};

  VehicleType v = vehicleTypeFromDataStruct(data);

  qDebug() << "v: " << v.name() << ", " << v.alias();
  
  QFAIL("Just a sandbox");
}

void SqlVehicleTypeRepositoryTest::addGetUpdateRemoveTest()
{
  SqlVehicleTypeRepository repository(database());

  Mdt::Railway::TaskList::TestLib::addGetUpdateRemoveTest(repository);

//   /*
//    * Add
//    */
// 
//   VehicleType rbde1("RBDe 560 DO", "DOMINO", "1");
//   const auto rbde1Id = repository.add(rbde1);
//   QVERIFY(rbde1Id);
//   QVERIFY(!rbde1Id->isNull());
// 
//   auto vehicle = repository.getById(*rbde1Id);
//   QVERIFY(vehicle);
//   QCOMPARE(vehicle->name(), QLatin1String("RBDe 560 DO"));
//   QCOMPARE(vehicle->alias(), QLatin1String("DOMINO"));
//   QCOMPARE(vehicle->manufacturerSerie(), QLatin1String("1"));
// 
//   /*
//    * Add more
//    * Will validate update and remove
//    */
// 
//   VehicleType dtz1("RABe 514", "DTZ", "1");
//   VehicleType icn1("RABDe 500", "ICN", "1");
// 
//   const auto dtz1Id = repository.add(dtz1);
//   const auto icn1Id = repository.add(icn1);
//   QVERIFY(dtz1Id);
//   QVERIFY(icn1Id);
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
//   QCOMPARE(vehicle->alias(), QLatin1String("DOMINO"));
// 
//   vehicle = repository.getById(*dtz1Id);
//   QVERIFY(vehicle);
//   QCOMPARE(vehicle->alias(), QLatin1String("DTZ"));
// 
//   vehicle = repository.getById(*icn1Id);
//   QVERIFY(vehicle);
//   QCOMPARE(vehicle->alias(), QLatin1String("ICN"));
// 
//   /*
//    * Remove
//    */
// 
//   QVERIFY(repository.remove(*dtz1Id));
// 
//   vehicle = repository.getById(*rbde1Id);
//   QVERIFY(vehicle);
//   QCOMPARE(vehicle->alias(), QLatin1String("DOMINO"));
// 
//   vehicle = repository.getById(*dtz1Id);
//   QVERIFY(!vehicle);
// 
//   vehicle = repository.getById(*icn1Id);
//   QVERIFY(vehicle);
//   QCOMPARE(vehicle->alias(), QLatin1String("ICN"));
// 
//   QVERIFY(repository.removeAll());
// 
//   vehicle = repository.getById(*rbde1Id);
//   QVERIFY(!vehicle);
// 
//   vehicle = repository.getById(*dtz1Id);
//   QVERIFY(!vehicle);
// 
//   vehicle = repository.getById(*icn1Id);
//   QVERIFY(!vehicle);
}

void SqlVehicleTypeRepositoryTest::getAllAsyncTest()
{
  SqlVehicleTypeRepository repository(database());

  Mdt::Railway::TaskList::TestLib::getAllAsyncTest(repository);
}

void SqlVehicleTypeRepositoryTest::getAllAsyncTest_data()
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
  SqlVehicleTypeRepositoryTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
