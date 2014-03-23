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
#include "mdtTestToolTest.h"
#include "mdtApplication.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlRecord.h"
#include "mdtTtBase.h"
#include <QTemporaryFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QAbstractButton>
#include <QComboBox>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QFile>
#include <QVariant>
#include <QTimer>
#include <QAbstractButton>
#include <QModelIndex>
#include <QList>
#include <QFileInfo>

#include <QTableView>
#include <QItemSelectionModel>

#include <QDebug>

void mdtTestToolTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabaseManager.database().isOpen());
}

void mdtTestToolTest::cleanupTestCase()
{
}

void mdtTestToolTest::mdtTtBaseTest()
{
  mdtTtBase b(0, pvDatabaseManager.database());
  mdtSqlRecord record;
  QSqlRecord data;
  QList<QSqlRecord> dataList;
  bool ok;
  QString sql;
  QStringList fields;

  // Add data
  QVERIFY(record.addAllFields("VehicleType_tbl", b.database()));
  record.setValue("Id_PK", 1);
  record.setValue("Type", "Vehicle type 1");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  // Get data and check
  sql = "SELECT * FROM VehicleType_tbl";
  fields.clear();
  fields << "Id_PK" << "Type" << "SubType" << "SeriesNumber";
  dataList = b.getData(sql, &ok, fields);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(1));
  QCOMPARE(data.value("Type"), QVariant("Vehicle type 1"));
  QCOMPARE(data.value("SubType"), QVariant(QVariant::String));
  QVERIFY(data.value("SubType").isNull());
  QCOMPARE(data.value("SeriesNumber"), QVariant(QVariant::String));
  QVERIFY(data.value("SeriesNumber").isNull());
  // Get data from a non existing table - must fail
  sql = "SELECT * FROM jkhswjqkhkjqhwdjwhqkhj";
  dataList = b.getData(sql, &ok, fields);
  QVERIFY(!ok);
  QCOMPARE(dataList.size(), 0);
  // Get data from a existing table, but expect a field that not exists - must fail
  sql = "SELECT * FROM VehicleType_tbl";
  fields.clear();
  fields << "Id_PK" << "Type" << "HuhUHHUIJhkhjKHjhkjh" << "SeriesNumber";
  dataList = b.getData(sql, &ok, fields);
  QVERIFY(!ok);
  QCOMPARE(dataList.size(), 0);
  // Get data from a existing table, specifing only some expect fields - Must work
  sql = "SELECT * FROM VehicleType_tbl";
  fields.clear();
  fields << "Id_PK" << "SeriesNumber";
  dataList = b.getData(sql, &ok, fields);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  // Get data from a existing table without specifing expected fields - Must work
  sql = "SELECT * FROM VehicleType_tbl";
  dataList = b.getData(sql, &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  // Get data without ok pointer, must not crash
  dataList = b.getData(sql);
  QCOMPARE(dataList.size(), 1);
  // Add data
  QVERIFY(record.addAllFields("VehicleType_tbl", b.database()));
  record.setValue("Id_PK", 2);
  record.setValue("Type", "Vehicle type 2");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  // Get data and check
  sql = "SELECT * FROM VehicleType_tbl";
  dataList = b.getData(sql, &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  data = dataList.at(1);
  QCOMPARE(data.value("Id_PK"), QVariant(2));
  QCOMPARE(data.value("Type"), QVariant("Vehicle type 2"));
  // Remove first row
  QVERIFY(b.removeData("VehicleType_tbl", "Id_PK", 1));
  // Check that only first row was removed
  sql = "SELECT * FROM VehicleType_tbl";
  dataList = b.getData(sql, &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(2));
  QCOMPARE(data.value("Type"), QVariant("Vehicle type 2"));
  // Edit row
  record.clearValues();
  record.setValue("Type", "Vehicle type 22");
  QVERIFY(b.updateRecord("VehicleType_tbl", record, "Id_PK", 2));
  // Get data and check
  sql = "SELECT * FROM VehicleType_tbl";
  dataList = b.getData(sql, &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(2));
  QCOMPARE(data.value("Type"), QVariant("Vehicle type 22"));
  // Edit row with null value
  record.clearValues();
  record.setValue("Type", QVariant());
  QVERIFY(b.updateRecord("VehicleType_tbl", record, "Id_PK", 2));
  // Get data and check
  sql = "SELECT * FROM VehicleType_tbl";
  dataList = b.getData(sql, &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(2));
  QVERIFY(data.value("Type").isNull());
}

/*
 * Test database helper methods
 */

void mdtTestToolTest::createDatabaseSchema()
{
  QTemporaryFile dbFile;
  QFileInfo dbFileInfo;

  /*
   * Check Sqlite database creation
   */
  QVERIFY(dbFile.open());
  dbFile.close();
  dbFileInfo.setFile(dbFile.fileName() + ".db");
  mdtTtDatabaseSchema schema(&pvDatabaseManager);
  QVERIFY(schema.createSchemaSqlite(dbFileInfo));
  QVERIFY(pvDatabaseManager.database().isOpen());
  QVERIFY(schema.checkSchema());
}

/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtTestToolTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
