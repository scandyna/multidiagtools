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
#include "CreateVehicleTypeTest.h"
#include "TestVehicleTypeClassRepository.h"
#include "TestVehicleTypeRepository.h"
#include "ResponseHandler.h"
#include "Mdt/Error.h"
#include "Mdt/Railway/VehicleTypeClassId.h"
#include "Mdt/Railway/CreateVehicleType.h"
#include "Mdt/Railway/CreateVehicleTypeClass.h"
#include "Mdt/Railway/VehicleTypeRepository.h"
#include "Mdt/Entity/IntegralUniqueIdTemplate.h"
#include "Mdt/Entity/PrimaryKeyRecord.h"
#include "Mdt/Entity/MemoryEntityRepository.h"
#include <QStringList>
#include <QObject>
#include <memory>

#include <QDebug>

using namespace Mdt::Railway;

/*
 * Helpers
 */

VehicleTypeClassId createVehicleTypeClass(const QString & name, const QString & alias)
{
  auto repository = std::make_shared<TestVehicleTypeClassRepository>();
  CreateVehicleTypeClass cvtc(repository);
  CreateVehicleTypeClassRequest request;

  request.name = name;
  request.alias = alias;
  cvtc.execute(request);

  return cvtc.response().id;
}

using CreateVehicleTypeResponseHandler = ResponseHandler<CreateVehicleTypeRequest, CreateVehicleTypeResponse>;

/*
 * tests
 */

void CreateVehicleTypeTest::createFromEmptyRepository()
{
  auto repository = std::make_shared<TestVehicleTypeRepository>();
  CreateVehicleType cvt(repository);
  CreateVehicleTypeRequest request;
  CreateVehicleTypeResponseHandler responseHandler;

  CreateVehicleTypeResponse response;
  responseHandler.setUseCase(cvt);

  /*
   * Initially, the repository is empty
   */
  QCOMPARE(repository->storageCount(), 0);
  /*
   * We create a vehicle type
   */
  const VehicleTypeClassId vehicleTypeClassId = createVehicleTypeClass("RBDe 560 DO", "DOMINO");
  QVERIFY(!vehicleTypeClassId.isNull());
  request.vehicleTypeClassId = vehicleTypeClassId;
  request.manufacturerSerie = "1";
  request.transactionId = 5;
  QVERIFY(cvt.execute(request));
  // Check the response
  response = responseHandler.response();
  QCOMPARE(response.vehicleTypeId, VehicleTypeId(1));
  QCOMPARE(response.vehicleTypeClassId, vehicleTypeClassId);
  QCOMPARE(response.manufacturerSerie, QString("1"));
  QCOMPARE(response.vehicleTypeClassName, QString("RBDe 560 DO"));
  QCOMPARE(response.vehicleTypeClassAlias, QString("DOMINO"));
  QCOMPARE(response.transactionId, 5);
  // Check that the vehicle type was stored
  QCOMPARE(repository->storageCount(), 1);
  const auto vehicleType = repository->getById(response.vehicleTypeId);
  QVERIFY(vehicleType);
  QCOMPARE(vehicleType->id(), response.vehicleTypeId);
  QCOMPARE(vehicleType->vehicleTypeClassId(), vehicleTypeClassId);
  QCOMPARE(vehicleType->manufacturerSerie(), QString("1"));
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  CreateVehicleTypeTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
