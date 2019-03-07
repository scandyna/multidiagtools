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
#include "UpdateVehicleTypeTest.h"
#include "ResponseHandler.h"
#include "Mdt/Railway/UpdateVehicleType.h"
#include "Mdt/Railway/CreateVehicleType.h"
#include "Mdt/Railway/CreateVehicleTypeClass.h"
#include "Mdt/Railway/VehicleTypeData.h"
#include <QString>

using namespace Mdt::Railway;

using UpdateVehicleTypeResponseHandler = ResponseHandler<UpdateVehicleTypeRequest, UpdateVehicleTypeResponse>;

/*
 * Tests
 */

UpdateVehicleTypeTest::UpdateVehicleTypeTest()
 : QObject(nullptr),
   mVehicleTypeClassRepository( std::make_shared<TestVehicleTypeClassRepository>() ),
   mVehicleTypeRepository( std::make_shared<TestVehicleTypeRepository>() )
{
}

void UpdateVehicleTypeTest::simpleUpdateTest()
{
  VehicleTypeId vehicleTypeId;
  UpdateVehicleType uvt(mVehicleTypeRepository);
  UpdateVehicleTypeResponseHandler responseHandler;
  responseHandler.setUseCase(uvt);

  /*
   * Create a vehicle type and check our storage
   */
  vehicleTypeId = createVehicleTypeAndClass("RBDe 560 DO", "DOMINO", "1");
  QVERIFY(!vehicleTypeId.isNull());
  QCOMPARE(mVehicleTypeRepository->storageCount(), 1);
  auto vehicleType = mVehicleTypeRepository->getById(vehicleTypeId);
  QVERIFY(vehicleType);
  QCOMPARE(vehicleType->id(), vehicleTypeId);
  QCOMPARE(vehicleType->manufacturerSerie(), QString("1"));
  QVERIFY(!vehicleType->vehicleTypeClassId().isNull());
  /*
   * Update and check
   */
  UpdateVehicleTypeRequest request;
  request.vehicleTypeId = vehicleTypeId;
  request.vehicleTypeClassId = vehicleType->vehicleTypeClassId();
  request.taskId = 5;
  request.manufacturerSerie = "2";
  QVERIFY(uvt.execute(request));
  // Check response
  const auto response = responseHandler.response();
  QCOMPARE(response.vehicleTypeId, request.vehicleTypeId);
  QCOMPARE(response.vehicleTypeClassId, request.vehicleTypeClassId);
  QCOMPARE(response.manufacturerSerie, request.manufacturerSerie);
  QCOMPARE(response.taskId, request.taskId);
  // Check storage
  vehicleType = mVehicleTypeRepository->getById(vehicleTypeId);
  QVERIFY(vehicleType);
  QCOMPARE(vehicleType->id(), vehicleTypeId);
  QCOMPARE(vehicleType->manufacturerSerie(), QString("2"));
  QVERIFY(!vehicleType->vehicleTypeClassId().isNull());
}

/*
 * Helpers
 */

VehicleTypeClassId UpdateVehicleTypeTest::createVehicleTypeClass(const CreateVehicleTypeClassRequest & request)
{
  CreateVehicleTypeClass cvtc(mVehicleTypeClassRepository);
  cvtc.execute(request);

  return cvtc.response().id;
}

VehicleTypeId UpdateVehicleTypeTest::createVehicleType(const CreateVehicleTypeRequest & request)
{
  using CreateVehicleTypeResponseHandler = ResponseHandler<CreateVehicleTypeRequest, CreateVehicleTypeResponse>;

  CreateVehicleType cvt(mVehicleTypeRepository);
  CreateVehicleTypeResponseHandler responseHandler;

  responseHandler.setUseCase(cvt);
  cvt.execute(request);

  return responseHandler.response().vehicleTypeId;
}

VehicleTypeId UpdateVehicleTypeTest::createVehicleTypeAndClass(const QString & name, const QString & alias, const QString & manufacturerSerie)
{
  CreateVehicleTypeClassRequest createVehicleTypeClassRequest;
  CreateVehicleTypeRequest createVehicleTypeRequest;

  createVehicleTypeClassRequest.name = name;
  createVehicleTypeClassRequest.alias = alias;

  createVehicleTypeRequest.vehicleTypeClassId = createVehicleTypeClass(createVehicleTypeClassRequest);
  createVehicleTypeRequest.manufacturerSerie = manufacturerSerie;

  return createVehicleType(createVehicleTypeRequest);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  UpdateVehicleTypeTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
