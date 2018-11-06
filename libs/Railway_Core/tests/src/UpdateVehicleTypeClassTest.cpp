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
#include "UpdateVehicleTypeClassTest.h"
#include "Mdt/Railway/UpdateVehicleTypeClass.h"
#include "Mdt/Railway/VehicleTypeClassData.h"
#include "Mdt/Railway/VehicleTypeClassRepository.h"
#include "Mdt/Entity/MemoryEntityRepository.h"
#include <memory>

using namespace Mdt::Railway;

/*
 * Test repository
 */

class TestVehicleTypeClassRepository : public VehicleTypeClassRepository, public Mdt::Entity::MemoryEntityRepository<VehicleTypeClassData, VehicleTypeClassId>
{
};

/*
 * Tests
 */

void UpdateVehicleTypeClassTest::updateTest()
{
  auto vehicleTypeClassRepository = std::make_shared<TestVehicleTypeClassRepository>();
  UpdateVehicleTypeClass updateVTC(vehicleTypeClassRepository);

  /*
   * First create a vehicle type classe
   */
  VehicleTypeClassData vehicle;
  vehicle.setId(1);
  vehicle.setName("RBDe");
  vehicle.setAlias("A");
  QVERIFY(vehicleTypeClassRepository->add(vehicle));
  QCOMPARE(vehicleTypeClassRepository->storageCount(), 1);

  /*
   * Update the vehicle type class
   */
  UpdateVehicleTypeClassRequest request;
  request.id = VehicleTypeClassId(1);
  request.name = "RBDe 560 DO";
  request.alias = "DOMINO";
  QVERIFY(updateVTC.execute(request));
  // Check the response
  auto response = updateVTC.response();
  QCOMPARE(response.id.value(), 1u);
  QCOMPARE(response.name, QString("RBDe 560 DO"));
  QCOMPARE(response.alias, QString("DOMINO"));
  // Check that the vehicle type class was updated in storage
  QCOMPARE(vehicleTypeClassRepository->storageCount(), 1);
  auto vtcExp = vehicleTypeClassRepository->getById(VehicleTypeClassId(1));
  QVERIFY(vtcExp);
  vehicle = *vtcExp;
  QCOMPARE(vehicle.id().value(), 1u);
  QCOMPARE(vehicle.name(), QString("RBDe 560 DO"));
  QCOMPARE(vehicle.alias(), QString("DOMINO"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  UpdateVehicleTypeClassTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
