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
#include "Mdt/Railway/CreateVehicleTypeClass.h"
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
 * Helper classes
 */

class ResponseHandler : public QObject
{
 public:

  CreateVehicleTypeClassResponse response() const
  {
    return mResponse;
  }

  Mdt::Error error() const
  {
    return mError;
  }

 public slots:

  void setResponse(const CreateVehicleTypeClassResponse & response)
  {
    mResponse = response;
  }

  void setError(const Mdt::Error & error)
  {
    mError = error;
  }

 private:

  CreateVehicleTypeClassResponse mResponse;
  Mdt::Error mError;
};

void setupResponseHandler(const ResponseHandler & handler, const CreateVehicleTypeClass & cvtc)
{
  QObject::connect(&cvtc, &CreateVehicleTypeClass::succeed, &handler, &ResponseHandler::setResponse);
  QObject::connect(&cvtc, &CreateVehicleTypeClass::failed, &handler, &ResponseHandler::setError);
}

/*
 * Tests
 */

void CreateVehicleTypeClassTest::createFromEmptyRepositoryTest()
{
  auto vehicleTypeClassRepository = std::make_shared<TestVehicleTypeClassRepository>();
  CreateVehicleTypeClass cvtc(vehicleTypeClassRepository);
  CreateVehicleTypeClassRequest request;
  ResponseHandler responseHandler;
  CreateVehicleTypeClassResponse response;
//   setupResponseHandler(responseHandler, cvtc);

  /*
   * Initially, the repository is empty
   */
  QCOMPARE(vehicleTypeClassRepository->storageCount(), 0);
  /*
   * We create a vehicle type class
   */
  request.name = "RBDe 560 DO";
  request.alias = "DOMINO";
  QVERIFY(cvtc.execute(request));
  // Check the response
  response = cvtc.response();
  QCOMPARE(response.id.value(), 1u);
  QCOMPARE(response.name, QString("RBDe 560 DO"));
  QCOMPARE(response.alias, QString("DOMINO"));
  // Check that the vehicle type class was stored
  QCOMPARE(vehicleTypeClassRepository->storageCount(), 1);
  auto dataExp = vehicleTypeClassRepository->getById(response.id);
  QVERIFY(dataExp);
  auto data = *dataExp;
  QCOMPARE(data.id().value(), 1u);
  QCOMPARE(data.name(), QString("RBDe 560 DO"));
  QCOMPARE(data.alias(), QString("DOMINO"));
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
