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
#include "GetAllVehicleTypesTest.h"
#include "MemoryVehicleTypeRepository.h"
#include "Mdt/Railway/TaskList/VehicleType.h"
#include "Mdt/Railway/TaskList/GetAllVehicleTypes.h"
#include <QObject>
#include <memory>
#include <vector>

#include <QDebug>

using namespace Mdt::Railway::TaskList;

void GetAllVehicleTypesTest::initTestCase()
{
}

/*
 * Helpers
 */

class ResponseHandler : public QObject
{
 public:

  void setUseCase(GetAllVehicleTypes & useCase)
  {
    connect(&useCase, &GetAllVehicleTypes::newRecordFetched, this, &ResponseHandler::addRecord);
    connect(&useCase, &GetAllVehicleTypes::failed, this, &ResponseHandler::setError);
  }

  void addRecord(const GetAllVehicleTypesRecord & record)
  {
    mRecords.push_back(record);
  }

  void setError(const GetAllVehicleTypesError & error)
  {
    mError = error;
  }

  int recordCount() const
  {
    return mRecords.size();
  }

  const GetAllVehicleTypesRecord & recordAt(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < recordCount());

    return mRecords[index];
  }

  GetAllVehicleTypesError error() const
  {
    return mError;
  }

 private:

  std::vector<GetAllVehicleTypesRecord> mRecords;
  GetAllVehicleTypesError mError;
};

/*
 * Tests
 */

void GetAllVehicleTypesTest::getAllTest()
{
  auto repository = std::make_shared<MemoryVehicleTypeRepository>();
  GetAllVehicleTypes getAllVehicleTypes(repository);
  ResponseHandler responseHandler;
  responseHandler.setUseCase(getAllVehicleTypes);

  VehicleType rbde1("RBDe 560 DO", "DOMINO", "1");
  VehicleType rbde2("RBDe 560 DO", "DOMINO", "2");
  VehicleType rae514("RAe 514", "DTZ", "1");
  QVERIFY(repository->add(rbde1));
  QVERIFY(repository->add(rbde2));
  QVERIFY(repository->add(rae514));

  QCOMPARE(responseHandler.recordCount(), 0);
  QVERIFY(getAllVehicleTypes.execute());
  QCOMPARE(responseHandler.recordCount(), 3);
  QCOMPARE(responseHandler.recordAt(0).name, QVariant("RBDe 560 DO"));
  QCOMPARE(responseHandler.recordAt(0).alias, QVariant("DOMINO"));
  QCOMPARE(responseHandler.recordAt(0).manufacturerSerie, QVariant("1"));
  QCOMPARE(responseHandler.recordAt(1).name, QVariant("RBDe 560 DO"));
  QCOMPARE(responseHandler.recordAt(1).alias, QVariant("DOMINO"));
  QCOMPARE(responseHandler.recordAt(1).manufacturerSerie, QVariant("2"));
  QCOMPARE(responseHandler.recordAt(2).name, QVariant("RAe 514"));
  QCOMPARE(responseHandler.recordAt(2).alias, QVariant("DTZ"));
  QCOMPARE(responseHandler.recordAt(2).manufacturerSerie, QVariant("1"));
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
  GetAllVehicleTypesTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
