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
#include "RemoveVehicleTypeClassTest.h"
#include "Mdt/Railway/RemoveVehicleTypeClass.h"
#include "Mdt/Railway/VehicleTypeClassData.h"
#include "ResponseHandler.h"
#include "TestVehicleTypeClassRepository.h"
#include <memory>

using namespace Mdt::Railway;

using RemoveVehicleTypeClassResponseHandler = ResponseHandler<RemoveVehicleTypeClassRequest, RemoveVehicleTypeClassResponse>;

/*
 * Tests
 */

void RemoveVehicleTypeClassTest::basicTest()
{
  auto vehicleTypeClassRepository = std::make_shared<TestVehicleTypeClassRepository>();
  RemoveVehicleTypeClass removeVTC(vehicleTypeClassRepository);
  RemoveVehicleTypeClassResponseHandler responseHandler;
  responseHandler.setUseCase(removeVTC);

  /*
   * First create vehicle type classes
   */
  VehicleTypeClassData vehicle;
  vehicle.setId(VehicleTypeClassId(1));
  vehicle.setName("RBDe 560 DO");
  vehicle.setAlias("DOMINO");
  const auto rbdeVid = vehicleTypeClassRepository->add(vehicle);
  QVERIFY(rbdeVid);
  vehicle.setId(VehicleTypeClassId(2));
  vehicle.setName("ABt DO");
  vehicle.setAlias("DOMINO");
  QVERIFY(vehicleTypeClassRepository->add(vehicle));
  QCOMPARE(vehicleTypeClassRepository->storageCount(), 2);

  /*
   * Remove the first vehicle type class
   */
  RemoveVehicleTypeClassRequest request;
  request.id = *rbdeVid;
  request.taskId = 10;
  QVERIFY(removeVTC.execute(request));
  // Check response
  auto response = responseHandler.response();
  QCOMPARE(response.id, request.id);
  QCOMPARE(response.taskId, request.taskId);
  // Check storage
  QCOMPARE(vehicleTypeClassRepository->storageCount(), 1);
  auto vehicleFromRepo = vehicleTypeClassRepository->getById(*rbdeVid);
  QVERIFY(vehicleFromRepo);
  QVERIFY(vehicleFromRepo->id().isNull());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  RemoveVehicleTypeClassTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
