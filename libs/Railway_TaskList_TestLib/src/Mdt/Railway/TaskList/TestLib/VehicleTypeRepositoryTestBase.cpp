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
#include "VehicleTypeRepositoryTestBase.h"
#include "VehicleTypeRepositoryResponseHandler.h"
#include "Mdt/Railway/TaskList/VehicleType.h"
#include "Mdt/Railway/TaskList/VehicleType_p.h"

#include "Mdt/Container/Vector.h"

#include <QString>
#include <QLatin1String>
#include <QVariant>

#include <QVariantList>
#include <QVector>

#include <QMetaType>
#include <QtTest/QtTest>

// using VehicleTypeDataTable = QVector<QVariantList>;
using VehicleTypeDataTable = Mdt::Container::Vector<Mdt::Railway::TaskList::VehicleType>;
Q_DECLARE_METATYPE(VehicleTypeDataTable);

namespace Mdt{ namespace Railway{ namespace TaskList{ namespace TestLib{

void addGetUpdateRemoveTest(VehicleTypeRepository & repository)
{
  /*
   * Add, get
   */

  VehicleType rbde1("RBDe 560 DO", "DOMINO", "1");
  const auto rbde1Id = repository.add(rbde1);
  QVERIFY(rbde1Id);
  QVERIFY(!rbde1Id->isNull());

  auto vehicle = repository.getById(*rbde1Id);
  QVERIFY(vehicle);
  QCOMPARE(vehicle->id(), *rbde1Id);
  QCOMPARE(vehicle->name(), QLatin1String("RBDe 560 DO"));
  QCOMPARE(vehicle->alias(), QLatin1String("DOMINO"));
  QCOMPARE(vehicle->manufacturerSerie(), QLatin1String("1"));

  /*
   * Add more
   * Will validate update and remove
   */

  VehicleType dtzRAe("RAe 514", "DTZ", "1");
  VehicleType dtzRBe("RBe 514", "DTZ", "1");

  const auto dtzRAeId = repository.add(dtzRAe);
  const auto dtzRBeId = repository.add(dtzRBe);
  QVERIFY(dtzRAeId);
  QVERIFY(dtzRBeId);

  /*
   * Update
   */

  vehicle = repository.getById(*rbde1Id);
  QVERIFY(vehicle);
  vehicle->setName("RBDe 560 MPZ");
  QVERIFY(repository.update(*vehicle));

  vehicle = repository.getById(*rbde1Id);
  QVERIFY(vehicle);
  QCOMPARE(vehicle->name(), QLatin1String("RBDe 560 MPZ"));
  QCOMPARE(vehicle->alias(), QLatin1String("DOMINO"));
  QCOMPARE(vehicle->manufacturerSerie(), QLatin1String("1"));

  vehicle = repository.getById(*rbde1Id);
  QVERIFY(vehicle);
  QCOMPARE(vehicle->name(), QLatin1String("RBDe 560 MPZ"));

  vehicle = repository.getById(*dtzRAeId);
  QVERIFY(vehicle);
  QCOMPARE(vehicle->name(), QLatin1String("RAe 514"));

  vehicle = repository.getById(*dtzRBeId);
  QVERIFY(vehicle);
  QCOMPARE(vehicle->name(), QLatin1String("RBe 514"));

  /*
   * Remove
   */

  QVERIFY(repository.remove(*dtzRAeId));

  vehicle = repository.getById(*rbde1Id);
  QVERIFY(vehicle);
  QCOMPARE(vehicle->name(), QLatin1String("RBDe 560 MPZ"));

  vehicle = repository.getById(*dtzRAeId);
  QVERIFY(!vehicle);

  vehicle = repository.getById(*dtzRBeId);
  QVERIFY(vehicle);
  QCOMPARE(vehicle->name(), QLatin1String("RBe 514"));

  QVERIFY(repository.removeAll());

  vehicle = repository.getById(*rbde1Id);
  QVERIFY(!vehicle);

  vehicle = repository.getById(*dtzRAeId);
  QVERIFY(!vehicle);

  vehicle = repository.getById(*dtzRBeId);
  QVERIFY(!vehicle);
}

void getAllAsyncTest(VehicleTypeRepository & repository)
{
  QFETCH(VehicleTypeDataTable, dataTable);

  VehicleTypeRepositoryResponseHandler rh;
  rh.connectRepository(repository);

  for(auto & vt : dataTable){
    const auto id = repository.add(vt);
    QVERIFY(id);
    privateVehicleTypeDataStruct(vt).id = id->value();
    QVERIFY(repository.update(vt));
  }

  QCOMPARE(rh.recordCount(), 0);
  QVERIFY(repository.getAllAsync());
  QTRY_COMPARE_WITH_TIMEOUT(rh.recordCount(), dataTable.size(), 5000);

  for(int i = 0; i < dataTable.size(); ++i){
    QCOMPARE(rh.recordAt(i).id(), dataTable.at(i).id());
    QCOMPARE(rh.recordAt(i).name(), dataTable.at(i).name());
    QCOMPARE(rh.recordAt(i).alias(), dataTable.at(i).alias());
    QCOMPARE(rh.recordAt(i).manufacturerSerie(), dataTable.at(i).manufacturerSerie());
  }
}

void MDT_RAILWAY_TASKLIST_CORE_EXPORT getAllAsyncTestBuildDataSet()
{
  QTest::addColumn<VehicleTypeDataTable>("dataTable");

  QTest::newRow("1") << VehicleTypeDataTable{
    VehicleType("RAe 514", "DTZ", "1")
  };

  QTest::newRow("2") << VehicleTypeDataTable{
    VehicleType("RAe 514", "DTZ", "1"),
    VehicleType("RBe 514", "DTZ", "1")
  };

}

}}}} // namespace Mdt{ namespace Railway{ namespace TaskList{ namespace TestLib{
