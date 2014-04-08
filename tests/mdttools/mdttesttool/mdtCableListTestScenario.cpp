/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtCableListTestScenario.h"
#include "mdtSqlRecord.h"
#include "mdtTtBase.h"
#include "mdtClArticle.h"
#include "mdtClUnit.h"
#include "mdtClConnectorData.h"
#include "mdtClArticleConnectorData.h"
#include "mdtClArticleConnectorData.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClUnitConnectorData.h"
#include "mdtClUnitVehicleType.h"
#include "mdtClLink.h"
#include "mdtClLinkData.h"
#include "mdtClVehicleTypeLinkData.h"
#include <QTest>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QModelIndex>
#include <QList>

#include <QDebug>

mdtCableListTestScenario::mdtCableListTestScenario(QSqlDatabase db)
{
  pvDatabase = db;
}

void mdtCableListTestScenario::createTestVehicleTypes()
{
  mdtTtBase tb(0, pvDatabase);
  mdtSqlRecord record;
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(record.addAllFields("VehicleType_tbl", pvDatabase));
  /*
   * Add Vehicle type 1
   */
  record.setValue("Id_PK", 1);
  record.setValue("Type", "Vehicle type 1");
  QVERIFY(tb.addRecord(record, "VehicleType_tbl"));
  // Check back
  dataList = tb.getData("SELECT * FROM VehicleType_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Id_PK"), QVariant(1));
  QCOMPARE(dataList.at(0).value("Type"), QVariant("Vehicle type 1"));
  /*
   * Add Vehicle type 2
   */
  record.setValue("Id_PK", 2);
  record.setValue("Type", "Vehicle type 2");
  QVERIFY(tb.addRecord(record, "VehicleType_tbl"));
  // Check back
  dataList = tb.getData("SELECT * FROM VehicleType_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  QCOMPARE(dataList.at(1).value("Id_PK"), QVariant(2));
  QCOMPARE(dataList.at(1).value("Type"), QVariant("Vehicle type 2"));

}

void mdtCableListTestScenario::removeTestVehicleTypes()
{
  mdtTtBase tb(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(tb.removeData("VehicleType_tbl", "Id_PK", 1));
  QVERIFY(tb.removeData("VehicleType_tbl", "Id_PK", 2));
  dataList = tb.getData("SELECT * FROM VehicleType_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}
