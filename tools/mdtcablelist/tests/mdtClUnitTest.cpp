/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtClUnitTest.h"
#include "mdtApplication.h"
#include "mdtClDatabaseSchema.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClLinkData.h"
#include <QTest>
#include <QString>
#include <QVariant>
#include <QPair>
#include <QList>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QSqlDatabase>

void mdtClUnitTest::initTestCase()
{
  QSqlDatabase db;

  QTemporaryFile dbFile;
  QFileInfo dbFileInfo;

  /*
   * Check Sqlite database creation
   */
  QVERIFY(dbFile.open());
  dbFileInfo.setFile(dbFile);
  mdtClDatabaseSchema schema(&pvDatabaseManager);
  QVERIFY(schema.createSchemaSqlite(dbFileInfo));
  QVERIFY(pvDatabaseManager.database().isOpen());
  /// \todo Check that tables and views are created
}

void mdtClUnitTest::unitConnectionDataTest()
{
  mdtClUnitConnectionData data;

  // Initial values
  QVERIFY(!data.isValid());
  QVERIFY(!data.connectionId().isValid());

  // Set/Get
  data.setConnectionId(25);
  QCOMPARE(data.connectionId(), QVariant(25));
}

void mdtClUnitTest::linkDataTest()
{
  mdtClLinkData data;
  QList<QVariant> list;

  /*
   * Check vehicle types
   */
  QCOMPARE(data.vehicleTypeStartEndIdList().size(), 0);
  // Add 1 start vehicle - Case Error (one list empty)
  data.addVehicleTypeStartId(1);
  QVERIFY(!data.buildVehicleTypeStartEndIdList());
  QCOMPARE(data.vehicleTypeStartEndIdList().size(), 0);
  // Add 1 end vehicle - Case 1 start -> 1 end
  data.addVehicleTypeEndId(11);
  QVERIFY(data.buildVehicleTypeStartEndIdList());
  QCOMPARE(data.vehicleTypeStartEndIdList().size(), 1);
  QCOMPARE(data.vehicleTypeStartEndIdList().at(0).first, QVariant(1));
  QCOMPARE(data.vehicleTypeStartEndIdList().at(0).second, QVariant(11));
  // Add a second start vehicle - Case 2 start -> 1 end
  data.addVehicleTypeStartId(2);
  QVERIFY(data.buildVehicleTypeStartEndIdList());
  QCOMPARE(data.vehicleTypeStartEndIdList().size(), 2);
  QCOMPARE(data.vehicleTypeStartEndIdList().at(0).first, QVariant(1));
  QCOMPARE(data.vehicleTypeStartEndIdList().at(0).second, QVariant(11));
  QCOMPARE(data.vehicleTypeStartEndIdList().at(1).first, QVariant(2));
  QCOMPARE(data.vehicleTypeStartEndIdList().at(1).second, QVariant(11));
  // Add a second end vehicle - Case 2*(1 start -> 1 end)
  data.addVehicleTypeEndId(12);
  QVERIFY(data.buildVehicleTypeStartEndIdList());
  QCOMPARE(data.vehicleTypeStartEndIdList().size(), 2);
  QCOMPARE(data.vehicleTypeStartEndIdList().at(0).first, QVariant(1));
  QCOMPARE(data.vehicleTypeStartEndIdList().at(0).second, QVariant(11));
  QCOMPARE(data.vehicleTypeStartEndIdList().at(1).first, QVariant(2));
  QCOMPARE(data.vehicleTypeStartEndIdList().at(1).second, QVariant(12));
  // Clear start list - Case Error (one list empty)
  list.clear();
  data.setVehicleTypeStartIdList(list);
  QVERIFY(!data.buildVehicleTypeStartEndIdList());
  QCOMPARE(data.vehicleTypeStartEndIdList().size(), 0);
  // Add 1 start vehicle - Case 1 start -> 2 end
  data.addVehicleTypeStartId(3);
  QVERIFY(data.buildVehicleTypeStartEndIdList());
  QCOMPARE(data.vehicleTypeStartEndIdList().size(), 2);
  QCOMPARE(data.vehicleTypeStartEndIdList().at(0).first, QVariant(3));
  QCOMPARE(data.vehicleTypeStartEndIdList().at(0).second, QVariant(11));
  QCOMPARE(data.vehicleTypeStartEndIdList().at(1).first, QVariant(3));
  QCOMPARE(data.vehicleTypeStartEndIdList().at(1).second, QVariant(12));

}

void mdtClUnitTest::cleanupTestCase()
{
}

/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtClUnitTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
