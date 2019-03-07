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
#include "CreateVehicleTypeClassTest.h"
#include "ResponseHandler.h"
#include "TestVehicleTypeClassRepository.h"
#include "Mdt/Railway/CreateVehicleTypeClass.h"
#include "Mdt/Railway/VehicleTypeClassData.h"
#include <memory>

using namespace Mdt::Railway;

using CreateVehicleTypeClassResponseHandler = ResponseHandler<CreateVehicleTypeClassRequest, CreateVehicleTypeClassResponse>;

/*
 * Tests
 */

void CreateVehicleTypeClassTest::createFromEmptyRepositoryTest()
{
  auto vehicleTypeClassRepository = std::make_shared<TestVehicleTypeClassRepository>();
  CreateVehicleTypeClass cvtc(vehicleTypeClassRepository);
  CreateVehicleTypeClassResponseHandler responseHandler;
  responseHandler.setUseCase(cvtc);
  CreateVehicleTypeClassRequest request;
  CreateVehicleTypeClassResponse response;

  /*
   * Initially, the repository is empty
   */
  QCOMPARE(vehicleTypeClassRepository->storageCount(), 0);
  /*
   * We create a vehicle type class
   */
  request.name = "RBDe 560 DO";
  request.alias = "DOMINO";
  request.transactionId = 22;
  QVERIFY(cvtc.execute(request));
  // Check the response
  response = responseHandler.response();
  QCOMPARE(response.id, VehicleTypeClassId(1));
  QCOMPARE(response.name, QString("RBDe 560 DO"));
  QCOMPARE(response.alias, QString("DOMINO"));
  QCOMPARE(response.transactionId, request.transactionId);
  // Check that the vehicle type class was stored
  QCOMPARE(vehicleTypeClassRepository->storageCount(), 1);
  const auto vehicle = vehicleTypeClassRepository->getById(response.id);
  QVERIFY(vehicle);
  QCOMPARE(vehicle->id(), VehicleTypeClassId(1));
  QCOMPARE(vehicle->name(), QString("RBDe 560 DO"));
  QCOMPARE(vehicle->alias(), QString("DOMINO"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  CreateVehicleTypeClassTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
