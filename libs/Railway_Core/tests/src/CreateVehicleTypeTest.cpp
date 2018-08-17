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
#include "Mdt/Railway/CreateVehicleType.h"
#include "Mdt/Railway/VehicleTypeRepository.h"
#include "Mdt/Entity/IntegralUniqueIdTemplate.h"
#include "Mdt/Entity/PrimaryKeyRecord.h"
#include "Mdt/Entity/MemoryEntityRepository.h"
#include <QStringList>
#include <QObject>

using namespace Mdt::Railway;

/*
 * Test repository
 */

class TestVehicleTypeRepository : public VehicleTypeRepository, public Mdt::Entity::MemoryEntityRepository<VehicleTypeData, VehicleTypeId>
{
 public:

  QStringList storageRecordStringList(int id) const
  {
    const auto dataExp = getById(VehicleTypeId(id));
    Q_ASSERT(dataExp);
    const auto data = *dataExp;
    return QStringList{ data.manufacturerSerie() };
  }
};

/*
 * Helper classes
 */

class ResponseHandler : public QObject
{
 public:

  CreateVehicleTypeResponse response() const
  {
    return mResponse;
  }

  Error error() const
  {
    return mError;
  }

 public slots:

  void setResponse(const CreateVehicleTypeResponse & response)
  {
    mResponse = response;
  }

  void setError(const Error & error)
  {
    mError = error;
  }

 private:

  CreateVehicleTypeResponse mResponse;
  Error mError;
};

void setupResponseHandler(const ResponseHandler & handler, const CreateVehicleType & cvt)
{
  QObject::connect(&cvt, &CreateVehicleType::succeed, &handler, &ResponseHandler::setResponse);
  QObject::connect(&cvt, &CreateVehicleType::failed, &handler, &ResponseHandler::setError);
}

/*
 * tests
 */

void CreateVehicleTypeTest::createFromEmptyRepository()
{
  auto repository = std::make_shared<TestVehicleTypeRepository>();
  CreateVehicleType cvt(repository);
  CreateVehicleTypeRequest request;
  ResponseHandler responseHandler;
  CreateVehicleTypeResponse response;
  setupResponseHandler(responseHandler, cvt);

  /*
   * Initially, the repository is empty
   */
  QCOMPARE(repository->storageCount(), 0);
  /*
   * We create a vehicle type
   */
  request.className = "RBDe 560 DO";
  request.alias = "DOMINO";
  request.manufacturerSerie = "1";
  QVERIFY(cvt.execute(request));
  // Check that the vehicle type was stored
  QCOMPARE(repository->storageCount(), 1);
  QCOMPARE(repository->storageRecordStringList(1), QStringList({"RBDe 560 DO","DOMINO","1"}));
  // Check the response
  response = responseHandler.response();
  QCOMPARE(response.vehicleTypeId, QString("1"));
  QCOMPARE(response.className, QString("RBDe 560 DO"));
  QCOMPARE(response.alias, QString("DOMINO"));
  QCOMPARE(response.manufacturerSerie, QString("1"));
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
