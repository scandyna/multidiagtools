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
#include "mdtCableListTestScenario.h"
#include "mdtApplication.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlRecord.h"
#include "mdtTtBase.h"
#include "mdtTtTest.h"
#include "mdtTtTestLinkData.h"
#include "mdtTtTestNodeUnit.h"
#include "mdtTtTestNodeUnitData.h"
#include "mdtSqlTableSelection.h"
#include "mdtClUnitConnectionData.h"
#include "mdtTtTestItemNodeSetupData.h"
#include "mdtTtTestNodeSetupData.h"
#include "mdtTtTestNodeUnitSetupData.h"
#include "mdtTtTestNodeManager.h"
#include "mdtDeviceContainer.h"
#include "mdtDeviceContainerWidget.h"
#include "mdtDeviceU3606A.h"
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
  
  /**
  mdtCableListTestScenario scenario(pvDatabaseManager.database());
  scenario.createSenario();
  scenario.removeScenario();
  */
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
  QModelIndexList indexList;
  QModelIndex index;
  mdtSqlTableSelection s;
  QSqlQueryModel model;
  QStringList fieldList;

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
  /*
   * Check data removing from a mdtSqlTableSelection - Single field
   */
  fieldList.clear();
  fieldList << "Id_PK";
  // Add data
  QVERIFY(record.addAllFields("VehicleType_tbl", b.database()));
  record.setValue("Id_PK", 10);
  record.setValue("Type", "Vehicle type 10");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  record.setValue("Id_PK", 11);
  record.setValue("Type", "Vehicle type 11");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  record.setValue("Id_PK", 12);
  record.setValue("Type", "Vehicle type 12");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  record.setValue("Id_PK", 13);
  record.setValue("Type", "Vehicle type 13");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  record.setValue("Id_PK", 14);
  record.setValue("Type", "Vehicle type 14");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  // Setup model
  model.setQuery("SELECT Id_PK, Type FROM VehicleType_tbl", pvDatabaseManager.database());
  QCOMPARE(model.rowCount(), 6);
  // Remove with selection of 1 row
  indexList.clear();
  index = model.index(0, 0);
  indexList.append(index);
  s.setIndexList(indexList, fieldList, &model);
  QVERIFY(b.removeData("VehicleType_tbl", s, true));
  model.setQuery("SELECT Id_PK, Type FROM VehicleType_tbl", pvDatabaseManager.database());
  QCOMPARE(model.rowCount(), 5);
  // Remove with selection of 2 rows
  indexList.clear();
  index = model.index(0, 0);
  indexList.append(index);
  index = model.index(1, 0);
  indexList.append(index);
  s.setIndexList(indexList, fieldList, &model);
  QVERIFY(b.removeData("VehicleType_tbl", s, true));
  model.setQuery("SELECT Id_PK, Type FROM VehicleType_tbl", pvDatabaseManager.database());
  QCOMPARE(model.rowCount(), 3);
  // Remove with selection of 3 rows
  indexList.clear();
  index = model.index(0, 0);
  indexList.append(index);
  index = model.index(1, 0);
  indexList.append(index);
  index = model.index(2, 0);
  indexList.append(index);
  s.setIndexList(indexList, fieldList, &model);
  QVERIFY(b.removeData("VehicleType_tbl", s, true));
  model.setQuery("SELECT Id_PK, Type FROM VehicleType_tbl", pvDatabaseManager.database());
  QCOMPARE(model.rowCount(), 0);
  /*
   * Check data removing from a mdtSqlTableSelection - 2 fields
   */
  fieldList.clear();
  fieldList << "Id_PK" << "Type";
  // Add data
  QVERIFY(record.addAllFields("VehicleType_tbl", b.database()));
  record.setValue("Id_PK", 10);
  record.setValue("Type", "Vehicle type 10");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  record.setValue("Id_PK", 11);
  record.setValue("Type", "Vehicle type 11");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  record.setValue("Id_PK", 12);
  record.setValue("Type", "Vehicle type 12");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  record.setValue("Id_PK", 13);
  record.setValue("Type", "Vehicle type 13");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  record.setValue("Id_PK", 14);
  record.setValue("Type", "Vehicle type 14");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  record.setValue("Id_PK", 15);
  record.setValue("Type", "Vehicle type 15");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  // Setup model
  model.setQuery("SELECT Id_PK, Type FROM VehicleType_tbl", pvDatabaseManager.database());
  QCOMPARE(model.rowCount(), 6);
  // Remove with selection of 1 row
  indexList.clear();
  index = model.index(0, 0);
  indexList.append(index);
  index = model.index(0, 1);
  indexList.append(index);
  s.setIndexList(indexList, fieldList, &model);
  QVERIFY(b.removeData("VehicleType_tbl", s, true));
  model.setQuery("SELECT Id_PK, Type FROM VehicleType_tbl", pvDatabaseManager.database());
  QCOMPARE(model.rowCount(), 5);
  // Remove with selection of 2 rows
  indexList.clear();
  index = model.index(0, 0);
  indexList.append(index);
  index = model.index(0, 1);
  indexList.append(index);
  index = model.index(1, 0);
  indexList.append(index);
  index = model.index(1, 1);
  indexList.append(index);
  s.setIndexList(indexList, fieldList, &model);
  QVERIFY(b.removeData("VehicleType_tbl", s, true));
  model.setQuery("SELECT Id_PK, Type FROM VehicleType_tbl", pvDatabaseManager.database());
  QCOMPARE(model.rowCount(), 3);
  // Remove with selection of 3 rows
  indexList.clear();
  index = model.index(0, 0);
  indexList.append(index);
  index = model.index(0, 1);
  indexList.append(index);
  index = model.index(1, 0);
  indexList.append(index);
  index = model.index(1, 1);
  indexList.append(index);
  index = model.index(2, 0);
  indexList.append(index);
  index = model.index(2, 1);
  indexList.append(index);
  s.setIndexList(indexList, fieldList, &model);
  QVERIFY(b.removeData("VehicleType_tbl", s, true));
  model.setQuery("SELECT Id_PK, Type FROM VehicleType_tbl", pvDatabaseManager.database());
  QCOMPARE(model.rowCount(), 0);

}

void mdtTestToolTest::mdtTtTestLinkDataTest()
{
  mdtTtTestLinkData data;

  // Setup
  QVERIFY(data.setup(pvDatabaseManager.database()));
  QVERIFY(data.contains("Id_PK"));
  // Set/get
  data.setValue("Id_PK", 1);
  QCOMPARE(data.value("Id_PK"), QVariant(1));
}

void mdtTestToolTest::mdtTtTestNodeUnitDataTest()
{
  mdtTtTestNodeUnitData data;
  mdtSqlRecord unitData;
  mdtClUnitConnectionData connectionData;

  // Setup
  ///QVERIFY(data.setup(pvDatabaseManager.database(), true));
  QVERIFY(data.setup(pvDatabaseManager.database()));
  QVERIFY(data.contains("Unit_Id_FK_PK"));
  // Check setUnitData()
  QVERIFY(unitData.addAllFields("Unit_tbl", pvDatabaseManager.database()));
  unitData.setValue("Id_PK", 5);
  data.setUnitData(unitData);
  QCOMPARE(data.value("Unit_Id_FK_PK"), QVariant(5));
  QCOMPARE(data.unitData().value("Id_PK"), QVariant(5));
  // Check setTestConnectionData()
  /**
  QVERIFY(connectionData.setup(pvDatabaseManager.database(), false));
  connectionData.setValue("Id_PK", 3);
  data.setTestConnectionData(connectionData);
  QCOMPARE(data.value("TestConnection_Id_FK"), QVariant(3));
  QCOMPARE(data.testConnectionData().value("Id_PK"), QVariant(3));
  */
  // Clear values
  data.clearValues();
  QVERIFY(data.value("Unit_Id_FK_PK").isNull());
  QVERIFY(data.unitData().value("Id_PK").isNull());
  ///QVERIFY(data.testConnectionData().value("Id_PK").isNull());

}

void mdtTestToolTest::mdtTtTestNodeUnitTest()
{
  mdtTtTestNodeUnit tnu(this, pvDatabaseManager.database());
  

  
}

void mdtTestToolTest::testNodeSetupDataTest()
{
  mdtTtTestItemNodeSetupData tiSetupData;
  mdtTtTestNodeSetupData tnSetupData;
  mdtTtTestNodeUnitSetupData tnuSetupData;
  mdtSqlRecord record;
  QSqlRecord rec;

  /*
   * Init
   */
  // Test item setup data - Initial state
  QVERIFY(!tiSetupData.hasMoreStep());
  QVERIFY(tiSetupData.currentDeviceIdentification().isNull());
  // Test node setup data - Initial state
  QCOMPARE(tnSetupData.nodeIdentification(), QString());
  QCOMPARE(tnSetupData.unitSetupList().size(), 0);
  // Setup test node unit setup data - Will get fields from TestNodeUnitSetup_tbl
  QVERIFY(tnuSetupData.setup(pvDatabaseManager.database()));
  QCOMPARE(tnuSetupData.ioType(), mdtTtTestNodeUnitSetupData::Unknown);
  QCOMPARE(tnuSetupData.ioPosition(), 0);
  QCOMPARE(tnuSetupData.schemaPosition(), QString());

  /*
   * Check Test node unit setup data
   */
  // Basic values - From TestNodeUnitSetup_tbl
  tnuSetupData.setValue("TestNodeUnit_Id_FK", 1);
  tnuSetupData.setValue("TestModelItem_Id_FK", 2);
  tnuSetupData.setValue("StepNumber", 3);
  QCOMPARE(tnuSetupData.value("TestNodeUnit_Id_FK"), QVariant(1));
  QCOMPARE(tnuSetupData.value("TestModelItem_Id_FK"), QVariant(2));
  QCOMPARE(tnuSetupData.value("StepNumber"), QVariant(3));
  // Clear
  tnuSetupData.clearValues();
  QVERIFY(tnuSetupData.value("TestNodeUnit_Id_FK").isNull());
  QVERIFY(tnuSetupData.value("TestModelItem_Id_FK").isNull());
  QVERIFY(tnuSetupData.value("StepNumber").isNull());

  /*
   * Check test node setup data
   */
  tnSetupData.setNodeIdentification("1");
  tnSetupData.setDeviceIdentification(25);
  QCOMPARE(tnSetupData.nodeIdentification(), QString("1"));
  QCOMPARE(tnSetupData.deviceIdentification(), QVariant(25));
  tnSetupData.clear();
  QVERIFY(tnSetupData.nodeIdentification().isNull());
  QVERIFY(tnSetupData.deviceIdentification().isNull());

  /*
   * Setup test node unit setup data from TestItemNodeUnitSetup_view
   */
  QVERIFY(record.addAllFields("TestItemNodeUnitSetup_view", pvDatabaseManager.database()));
  rec = record;
  tnuSetupData = rec;
  tnuSetupData.setValue("TestNode_Id_FK", 5);
  tnuSetupData.setValue("Type_Code_FK", "AI");
  tnuSetupData.setValue("IoPosition", 7);
  tnuSetupData.setValue("SchemaPosition", "K8");
  tnuSetupData.setValue("TestModelItem_Id_FK", 9);
  tnuSetupData.setValue("StepNumber", 10);
  tnuSetupData.setValue("TestNodeUnit_Id_FK", 11);
  QCOMPARE(tnuSetupData.value("TestNode_Id_FK"), QVariant(5));
  QCOMPARE(tnuSetupData.value("Type_Code_FK"), QVariant("AI"));
  QCOMPARE(tnuSetupData.value("IoPosition"), QVariant(7));
  QCOMPARE(tnuSetupData.value("SchemaPosition"), QVariant("K8"));
  QCOMPARE(tnuSetupData.value("TestModelItem_Id_FK"), QVariant(9));
  QCOMPARE(tnuSetupData.value("StepNumber"), QVariant(10));
  QCOMPARE(tnuSetupData.value("TestNodeUnit_Id_FK"), QVariant(11));
  QCOMPARE(tnuSetupData.ioType(), mdtTtTestNodeUnitSetupData::AnalogInput);
  QCOMPARE(tnuSetupData.ioPosition(), 7);
  QCOMPARE(tnuSetupData.schemaPosition(), QString("K8"));
  // Clear
  tnuSetupData.clearValues();
  QVERIFY(tnuSetupData.value("TestNode_Id_FK").isNull());
  QVERIFY(tnuSetupData.value("Type_Code_FK").isNull());
  QVERIFY(tnuSetupData.value("IoPosition").isNull());
  QVERIFY(tnuSetupData.value("SchemaPosition").isNull());
  QVERIFY(tnuSetupData.value("TestModelItem_Id_FK").isNull());
  QVERIFY(tnuSetupData.value("StepNumber").isNull());
  QVERIFY(tnuSetupData.value("TestNodeUnit_Id_FK").isNull());
  QCOMPARE(tnuSetupData.ioType(), mdtTtTestNodeUnitSetupData::Unknown);
  QCOMPARE(tnuSetupData.ioPosition(), 0);
  QCOMPARE(tnuSetupData.schemaPosition(), QString());

  /*
   * Build a setup
   *
   * |Item|Node|Unit(pos)|Step|
   * --------------------------
   * |  1 | 10 | 100 (K1)|  0 |
   * --------------------------
   * |  1 | 10 | 101 (K2)|  0 |
   * --------------------------
   * |  1 | 10 | 104 (K5)|  1 |
   * --------------------------
   *
   * This gives a total of 2 steps
   */
  QCOMPARE(tnSetupData.unitSetupList().size(), 0);
  QVERIFY(!tiSetupData.hasMoreStep());
  QVERIFY(tiSetupData.currentDeviceIdentification().isNull());
  // Database independant data
  tnSetupData.setNodeIdentification("1000");
  tnSetupData.setDeviceIdentification("W750:1000");
  QCOMPARE(tnSetupData.nodeIdentification(), QString("1000"));
  QCOMPARE(tnSetupData.deviceIdentification(), QVariant("W750:1000"));
  // Test model item 1, Node 10, unit 100, I/O pos. 0 (K1), step 0
  tnuSetupData.setValue("TestModelItem_Id_FK", 1);
  tnuSetupData.setValue("TestNode_Id_FK", 10);
  tnuSetupData.setValue("TestNodeUnit_Id_FK", 100);
  tnuSetupData.setValue("IoPosition", 0);
  tnuSetupData.setValue("SchemaPosition", "K1");
  tnuSetupData.setValue("StepNumber", 0);
  tnSetupData.addUnitSetup(tnuSetupData);
  QCOMPARE(tnSetupData.unitSetupList().size(), 1);
  // Test model item 1, Node 10, unit 101, I/O pos. 1 (K2), step 0
  tnuSetupData.setValue("TestModelItem_Id_FK", 1);
  tnuSetupData.setValue("TestNode_Id_FK", 10);
  tnuSetupData.setValue("TestNodeUnit_Id_FK", 101);
  tnuSetupData.setValue("IoPosition", 1);
  tnuSetupData.setValue("SchemaPosition", "K2");
  tnuSetupData.setValue("StepNumber", 0);
  tnSetupData.addUnitSetup(tnuSetupData);
  QCOMPARE(tnSetupData.unitSetupList().size(), 2);
  // Test model item 1, Node 10, unit 104, I/O pos. 4 (K5), step 1
  tnuSetupData.setValue("TestModelItem_Id_FK", 1);
  tnuSetupData.setValue("TestNode_Id_FK", 10);
  tnuSetupData.setValue("TestNodeUnit_Id_FK", 101);
  tnuSetupData.setValue("IoPosition", 4);
  tnuSetupData.setValue("SchemaPosition", "K5");
  tnuSetupData.setValue("StepNumber", 1);
  tnSetupData.addUnitSetup(tnuSetupData);
  QCOMPARE(tnSetupData.unitSetupList().size(), 3);
  // Add node setup to test item setup data and check
  tiSetupData.addNodeSetupData(tnSetupData);
  // Step 0
  QVERIFY(tiSetupData.hasMoreStep());
  tnSetupData = tiSetupData.getNextStep();
  QCOMPARE(tiSetupData.currentDeviceIdentification(), QVariant("W750:1000"));
  QCOMPARE(tnSetupData.unitSetupList().size(), 2);
  QCOMPARE(tnSetupData.unitSetupList().at(0).schemaPosition(), QString("K1"));
  QCOMPARE(tnSetupData.unitSetupList().at(0).ioPosition(), 0);
  QCOMPARE(tnSetupData.unitSetupList().at(1).schemaPosition(), QString("K2"));
  QCOMPARE(tnSetupData.unitSetupList().at(1).ioPosition(), 1);
  // Step 1
  QVERIFY(tiSetupData.hasMoreStep());
  tnSetupData = tiSetupData.getNextStep();
  QCOMPARE(tiSetupData.currentDeviceIdentification(), QVariant("W750:1000"));
  QCOMPARE(tnSetupData.unitSetupList().size(), 1);
  QCOMPARE(tnSetupData.unitSetupList().at(0).schemaPosition(), QString("K5"));
  QCOMPARE(tnSetupData.unitSetupList().at(0).ioPosition(), 4);
  // End
  QVERIFY(!tiSetupData.hasMoreStep());
  // Clear
  tiSetupData.clear();
  QVERIFY(!tiSetupData.hasMoreStep());
  QVERIFY(tiSetupData.currentDeviceIdentification().isNull());

  /*
   * Build a setup
   *
   * |Item|Node|Unit(pos)|Step|
   * --------------------------
   * |  1 | 10 | 100 (K1)|  0 |
   * --------------------------
   * |  1 | 20 | 121(K20)|  0 |
   * --------------------------
   * |  1 | 10 | 104 (K5)|  1 |
   * --------------------------
   *
   * This gives a total of 3 steps
   */
  // Test item 1, Node 10, unit 100 (K1), step 0
  tnSetupData.clearUnitSetup();
  QCOMPARE(tnSetupData.unitSetupList().size(), 0);
  tnuSetupData.clearValues();
  tnSetupData.setNodeIdentification("2000");
  tnSetupData.setDeviceIdentification("W750:2000");
  tnuSetupData.setValue("TestModelItem_Id_FK", 1);
  tnuSetupData.setValue("TestNode_Id_FK", 10);
  tnuSetupData.setValue("TestNodeUnit_Id_FK", 100);
  tnuSetupData.setValue("IoPosition", 0);
  tnuSetupData.setValue("SchemaPosition", "K1");
  tnuSetupData.setValue("StepNumber", 0);
  tnSetupData.addUnitSetup(tnuSetupData);
  QCOMPARE(tnSetupData.unitSetupList().size(), 1);
  tiSetupData.addNodeSetupData(tnSetupData);
  // Test item 1, Node 20, unit 121 (K20), step 0
  tnSetupData.clearUnitSetup();
  QCOMPARE(tnSetupData.unitSetupList().size(), 0);
  tnuSetupData.clearValues();
  tnSetupData.setNodeIdentification("2200");
  tnSetupData.setDeviceIdentification("W750:2200");
  tnuSetupData.setValue("TestModelItem_Id_FK", 1);
  tnuSetupData.setValue("TestNode_Id_FK", 20);
  tnuSetupData.setValue("TestNodeUnit_Id_FK", 121);
  tnuSetupData.setValue("IoPosition", 19);
  tnuSetupData.setValue("SchemaPosition", "K20");
  tnuSetupData.setValue("StepNumber", 0);
  tnSetupData.addUnitSetup(tnuSetupData);
  QCOMPARE(tnSetupData.unitSetupList().size(), 1);
  tiSetupData.addNodeSetupData(tnSetupData);
  // Test item 1, Node 10, unit 104 (K5), step 0
  tnSetupData.clearUnitSetup();
  QCOMPARE(tnSetupData.unitSetupList().size(), 0);
  tnuSetupData.clearValues();
  tnSetupData.setNodeIdentification("2000");
  tnSetupData.setDeviceIdentification("W750:2000");
  tnuSetupData.setValue("TestModelItem_Id_FK", 1);
  tnuSetupData.setValue("TestNode_Id_FK", 10);
  tnuSetupData.setValue("TestNodeUnit_Id_FK", 14);
  tnuSetupData.setValue("IoPosition", 4);
  tnuSetupData.setValue("SchemaPosition", "K5");
  tnuSetupData.setValue("StepNumber", 1);
  tnSetupData.addUnitSetup(tnuSetupData);
  QCOMPARE(tnSetupData.unitSetupList().size(), 1);
  tiSetupData.addNodeSetupData(tnSetupData);
  /*
   * Check. Note that for step 0, node 10 and 20 will be swapped,
   *  because QMultiMap (used in mdtTtTestItemNodeSetupData) inserts items
   *  that shares the same keys as a LIFO.
   */
  // Step 0 - Node 20
  QVERIFY(tiSetupData.hasMoreStep());
  tnSetupData = tiSetupData.getNextStep();
  QCOMPARE(tiSetupData.currentDeviceIdentification(), QVariant("W750:2200"));
  QCOMPARE(tnSetupData.unitSetupList().size(), 1);
  QCOMPARE(tnSetupData.unitSetupList().at(0).schemaPosition(), QString("K20"));
  QCOMPARE(tnSetupData.unitSetupList().at(0).ioPosition(), 19);
  // Step 0 - Node 10
  QVERIFY(tiSetupData.hasMoreStep());
  tnSetupData = tiSetupData.getNextStep();
  QCOMPARE(tiSetupData.currentDeviceIdentification(), QVariant("W750:2000"));
  QCOMPARE(tnSetupData.unitSetupList().size(), 1);
  QCOMPARE(tnSetupData.unitSetupList().at(0).schemaPosition(), QString("K1"));
  QCOMPARE(tnSetupData.unitSetupList().at(0).ioPosition(), 0);
  // Step 1
  QVERIFY(tiSetupData.hasMoreStep());
  tnSetupData = tiSetupData.getNextStep();
  QCOMPARE(tiSetupData.currentDeviceIdentification(), QVariant("W750:2000"));
  QCOMPARE(tnSetupData.unitSetupList().size(), 1);
  QCOMPARE(tnSetupData.unitSetupList().at(0).schemaPosition(), QString("K5"));
  QCOMPARE(tnSetupData.unitSetupList().at(0).ioPosition(), 4);
  // End
  QVERIFY(!tiSetupData.hasMoreStep());
  // Clear
  tiSetupData.clear();
  QVERIFY(!tiSetupData.hasMoreStep());
  QVERIFY(tiSetupData.currentDeviceIdentification().isNull());

}

void mdtTestToolTest::mdtTtTestNodeManagerTest()
{
  std::shared_ptr<mdtTtTestNodeManager> m(new mdtTtTestNodeManager(0, pvDatabaseManager.database()));
  mdtDeviceContainerWidget w;
  std::shared_ptr<mdtDeviceU3606A> devU3606A;
  std::shared_ptr<mdtDeviceScpi> devScpi;
  QList<std::shared_ptr<mdtDevice>> devList;

  // Add a device that is not related to a test node
  m->addDevice<mdtDeviceU3606A>("U3606A", "SN01", "U3606A Multimeter, SN01");
  devU3606A = m->device<mdtDeviceU3606A>("U3606A");
  QVERIFY(devU3606A.get() != 0);
  devList = m->allDevices();
  QCOMPARE(devList.size(), 1);
  QVERIFY(devList.at(0).get() != 0);
  // Assign manager to widget
  w.setContainer(m->container());
  w.show();
  
  QTest::qWait(3000);
  
  // Add a new SCPI device
  devScpi = m->addDevice<mdtDeviceScpi>("SCPI", "SN005", "Generic SCPI device");
  
  qDebug() << "U3606A count: " << devU3606A.use_count() << " , SCPI count: " << devScpi.use_count();
  
  devU3606A->connectToDevice(mdtDeviceInfo());
  QTest::qWait(5000);
  devU3606A->disconnectFromDevice();
  
  QTest::qWait(2000);
  
  m->clear();
  devScpi = m->addDevice<mdtDeviceScpi>("SCPI6", "SN06", "Generic SCPI device 6");
  devScpi->connectToDevice(mdtDeviceInfo());
  devScpi = m->addDevice<mdtDeviceScpi>("SCPI7", "SN07", "Generic SCPI device 7");
  devScpi->connectToDevice(mdtDeviceInfo());
  devScpi = m->addDevice<mdtDeviceScpi>("SCPI8", "SN08", "Generic SCPI device 8");
  devScpi->connectToDevice(mdtDeviceInfo());
  
  devU3606A = m->addDevice<mdtDeviceU3606A>("U3606A01", "SN01", "U3606A Multimeter, SN01");
  devU3606A->connectToDevice(mdtDeviceInfo());
  devU3606A = m->addDevice<mdtDeviceU3606A>("U3606A02", "SN02", "U3606A Multimeter, SN02");
  devU3606A->connectToDevice(mdtDeviceInfo());

  qDebug() << "U3606A count: " << devU3606A.use_count() << " , SCPI count: " << devScpi.use_count();
  
  while(w.isVisible()){
    QTest::qWait(1000);
  }
}

void mdtTestToolTest::mdtTtTestTest()
{
  /**
  QVariant dblVal;
  mdtValue value;

  // Check value -> double conversions
  value.setValue(1.0, false, false);
  dblVal = mdtTtTest::valueToDouble(value);
  QCOMPARE(dblVal.toDouble(), 1.0);
  value.setValue(-100, true, false);
  dblVal = mdtTtTest::valueToDouble(value);
  QCOMPARE(dblVal.toDouble(), -1e300);
  value.setValue(100, false, true);
  dblVal = mdtTtTest::valueToDouble(value);
  QCOMPARE(dblVal.toDouble(), 1e300);
  // Check double -> value conversions
  value = mdtTtTest::doubleToValue(QVariant());
  QVERIFY(!value.isValid());
  value = mdtTtTest::doubleToValue(1.0);
  QCOMPARE(value.valueDouble(), 1.0);
  QVERIFY(!value.isMinusOl());
  QVERIFY(!value.isPlusOl());
  value = mdtTtTest::doubleToValue(-1e300);
  QCOMPARE(value.valueDouble(), -1e300);
  QVERIFY(value.isMinusOl());
  QVERIFY(!value.isPlusOl());
  value = mdtTtTest::doubleToValue(1e300);
  QCOMPARE(value.valueDouble(), 1e300);
  QVERIFY(!value.isMinusOl());
  QVERIFY(value.isPlusOl());
  */

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
