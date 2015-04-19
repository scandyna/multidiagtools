/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtTtTestModelItemData.h"
#include "mdtTtTestModelItemRouteData.h"
#include "mdtTtTestNodeManager.h"
#include "mdtDeviceContainer.h"
#include "mdtDeviceContainerWidget.h"
#include "mdtDeviceU3606A.h"
#include "mdtTtValueLimits.h"
#include "mdtTtTestStep.h"
#include "mdtTtTestStepContainer.h"
#include "mdtTtTestStepWidget.h"
#include "mdtTtTestStepContainerWidget.h"
#include <QTemporaryFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QWidget>
#include <QScrollArea>
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
  QList<QVariant> variantList;
  QList<int> intList;
  QList<qlonglong> longLongList;
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
  // Check again with getDataList() method - Type: QSqlRecord
  dataList = b.getDataList<QSqlRecord>(sql, ok, fields);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(1));
  QCOMPARE(data.value("Type"), QVariant("Vehicle type 1"));
  QCOMPARE(data.value("SubType"), QVariant(QVariant::String));
  QVERIFY(data.value("SubType").isNull());
  QCOMPARE(data.value("SeriesNumber"), QVariant(QVariant::String));
  QVERIFY(data.value("SeriesNumber").isNull());
  // Check again with getDataList() method - Type: QVariant
  sql = "SELECT Id_PK FROM VehicleType_tbl";
  variantList = b.getDataList<QVariant>(sql, ok);
  QVERIFY(ok);
  QCOMPARE(variantList.size(), 1);
  QCOMPARE(variantList.at(0), QVariant(1));
  // Check again with getDataList() method - Type: int
  intList = b.getDataList<int>(sql, ok);
  QVERIFY(ok);
  QCOMPARE(intList.size(), 1);
  QCOMPARE(intList.at(0), 1);
  // Check again with getDataList() method - Type: int
  longLongList = b.getDataList<qlonglong>(sql, ok);
  QVERIFY(ok);
  QCOMPARE(longLongList.size(), 1);
  QCOMPARE(longLongList.at(0), static_cast<qlonglong>(1));
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
  /*
   * Check updateRecord with 1 key
   */
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
   * Add 2 records for next tests
   */
  record.clearValues();
  // Add data
  record.setValue("Id_PK", 3);
  record.setValue("Type", "Vehicle type 3");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  record.clearValues();
  record.setValue("Id_PK", 4);
  record.setValue("Type", "Vehicle type 4");
  QVERIFY(b.addRecord(record, "VehicleType_tbl"));
  // Get data and check
  sql = "SELECT * FROM VehicleType_tbl";
  dataList = b.getDataList<QSqlRecord>(sql, ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 3);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(2));
  QVERIFY(data.value("Type").isNull());
  data = dataList.at(1);
  QCOMPARE(data.value("Id_PK"), QVariant(3));
  QCOMPARE(data.value("Type"), QVariant("Vehicle type 3"));
  data = dataList.at(2);
  QCOMPARE(data.value("Id_PK"), QVariant(4));
  QCOMPARE(data.value("Type"), QVariant("Vehicle type 4"));
  /*
   * Check updateRecord with 2 keys
   */
  // Update
  record.clearValues();
  record.setValue("Type", "Vehicle type 33");
  QVERIFY(b.updateRecord("VehicleType_tbl", record, "Id_PK", 3, "Type", "Vehicle type 3"));
  // Get data and check
  sql = "SELECT * FROM VehicleType_tbl";
  dataList = b.getDataList<QSqlRecord>(sql, ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 3);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(2));
  QVERIFY(data.value("Type").isNull());
  data = dataList.at(1);
  QCOMPARE(data.value("Id_PK"), QVariant(3));
  QCOMPARE(data.value("Type"), QVariant("Vehicle type 33"));
  data = dataList.at(2);
  QCOMPARE(data.value("Id_PK"), QVariant(4));
  QCOMPARE(data.value("Type"), QVariant("Vehicle type 4"));
  // Update
  record.clearValues();
  record.setValue("Type", "Vehicle type 44");
  QVERIFY(b.updateRecord("VehicleType_tbl", record, "Id_PK", 4, "Type", "Vehicle type 4"));
  // Get data and check
  sql = "SELECT * FROM VehicleType_tbl";
  dataList = b.getDataList<QSqlRecord>(sql, ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 3);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(2));
  QVERIFY(data.value("Type").isNull());
  data = dataList.at(1);
  QCOMPARE(data.value("Id_PK"), QVariant(3));
  QCOMPARE(data.value("Type"), QVariant("Vehicle type 33"));
  data = dataList.at(2);
  QCOMPARE(data.value("Id_PK"), QVariant(4));
  QCOMPARE(data.value("Type"), QVariant("Vehicle type 44"));
  /*
   * Check data removing with 2 keys
   */
  // Remove a record
  QVERIFY(b.removeData("VehicleType_tbl", "Id_PK", 3, "Type", "Vehicle type 33"));
  // Get data and check
  sql = "SELECT * FROM VehicleType_tbl";
  dataList = b.getDataList<QSqlRecord>(sql, ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(2));
  QVERIFY(data.value("Type").isNull());
  data = dataList.at(1);
  QCOMPARE(data.value("Id_PK"), QVariant(4));
  QCOMPARE(data.value("Type"), QVariant("Vehicle type 44"));
  // Remove a record
  QVERIFY(b.removeData("VehicleType_tbl", "Id_PK", 4, "Type", "Vehicle type 44"));
  // Get data and check
  sql = "SELECT * FROM VehicleType_tbl";
  dataList = b.getDataList<QSqlRecord>(sql, ok);
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

void mdtTestToolTest::mdtTtTestNodeManagerTest()
{
  ///QFAIL("No implemented");
  std::shared_ptr<mdtTtTestNodeManager> m(new mdtTtTestNodeManager(0, pvDatabaseManager.database()));
  mdtDeviceContainerWidget w;
//   std::shared_ptr<mdtDeviceU3606A> devU3606A;
//   std::shared_ptr<mdtDeviceScpi> devScpi;
  QList<std::shared_ptr<mdtDevice>> devList;
// 
//   // Add a device that is not related to a test node
  m->addDevice<mdtDeviceU3606A>("U3606A");
  auto devU3606A = m->device<mdtDeviceU3606A>("U3606A");
  QVERIFY(devU3606A.get() != 0);
  devList = m->allDevices();
  QCOMPARE(devList.size(), 1);
  QVERIFY(devList.at(0).get() != 0);
  // Assign manager to widget
  w.setContainer(m->container());
  w.show();
//   
//   QTest::qWait(3000);
//   
//   // Add a new SCPI device
//   devScpi = m->addDevice<mdtDeviceScpi>("SCPI", "SN005", "Generic SCPI device");
//   
//   qDebug() << "U3606A count: " << devU3606A.use_count() << " , SCPI count: " << devScpi.use_count();
//   
//   devU3606A->connectToDevice(mdtDeviceInfo());
//   QTest::qWait(5000);
//   devU3606A->disconnectFromDevice();
//   
//   QTest::qWait(2000);
//   
//   m->clear();
//   devScpi = m->addDevice<mdtDeviceScpi>("SCPI6", "SN06", "Generic SCPI device 6");
//   devScpi->connectToDevice(mdtDeviceInfo());
//   devScpi = m->addDevice<mdtDeviceScpi>("SCPI7", "SN07", "Generic SCPI device 7");
//   devScpi->connectToDevice(mdtDeviceInfo());
//   devScpi = m->addDevice<mdtDeviceScpi>("SCPI8", "SN08", "Generic SCPI device 8");
//   devScpi->connectToDevice(mdtDeviceInfo());
//   
//   devU3606A = m->addDevice<mdtDeviceU3606A>("U3606A01", "SN01", "U3606A Multimeter, SN01");
//   devU3606A->connectToDevice(mdtDeviceInfo());
//   devU3606A = m->addDevice<mdtDeviceU3606A>("U3606A02", "SN02", "U3606A Multimeter, SN02");
//   devU3606A->connectToDevice(mdtDeviceInfo());
// 
//   qDebug() << "U3606A count: " << devU3606A.use_count() << " , SCPI count: " << devScpi.use_count();
//   
  while(w.isVisible()){
    QTest::qWait(1000);
  }
}

void mdtTestToolTest::mdtTtTestModelItemRouteDataTest()
{
  mdtTtTestModelItemRouteData routeData;
  mdtTtTestNodeUnitSetupData setupData;
  mdtSqlRecord record;

  // Init
  QVERIFY(setupData.setup(pvDatabaseManager.database()));
  // Simple set/get
  routeData.setId(1);
  routeData.setTestModelItemId(2);
  routeData.setTestLinkId(3);
  routeData.setDutConnectionId(4);
  routeData.setTestConnectionId(5);
  routeData.setMeasureConnectionId(6);
  QCOMPARE(routeData.id() ,QVariant(1));
  QCOMPARE(routeData.testModelItemId() ,QVariant(2));
  QCOMPARE(routeData.testLinkId() ,QVariant(3));
  QCOMPARE(routeData.dutConnectionId() ,QVariant(4));
  QCOMPARE(routeData.testConnectionId() ,QVariant(5));
  QCOMPARE(routeData.measureConnectionId() ,QVariant(6));
  /*
   * Setup data
   */
  setupData.clearValues();
  setupData.setValue("TestNodeUnit_Id_FK", 10);
  routeData.addSetupData(setupData);
  QCOMPARE(routeData.setupDataList().size() ,1);
  QCOMPARE(routeData.setupDataList().at(0).value("TestModelItem_Id_FK") ,QVariant(2));
  QCOMPARE(routeData.setupDataList().at(0).value("TestModelItemRoute_Id_FK") ,QVariant(1));
  // Update ID
  routeData.setId(20);
  QCOMPARE(routeData.id() ,QVariant(20));
  QCOMPARE(routeData.setupDataList().at(0).value("TestModelItemRoute_Id_FK") ,QVariant(20));
  // Update test model item ID
  routeData.setTestModelItemId(21);
  QCOMPARE(routeData.testModelItemId() ,QVariant(21));
  QCOMPARE(routeData.setupDataList().at(0).value("TestModelItem_Id_FK") ,QVariant(21));
  /*
   * Setup data from TestModelItemRoute_tbl
   */
  // Setup record
  QVERIFY(record.addAllFields("TestModelItemRoute_tbl", pvDatabaseManager.database()));
  record.setValue("Id_PK", 10);
  record.setValue("TestModelItem_Id_FK", 11);
  record.setValue("TestLink_Id_FK", 12);
  record.setValue("MeasureTestNodeUnitConnection_Id_FK", 15);
  routeData = mdtTtTestModelItemRouteData(record);
  // Check route data
  QCOMPARE(routeData.id() ,QVariant(10));
  QCOMPARE(routeData.testModelItemId() ,QVariant(11));
  QCOMPARE(routeData.testLinkId() ,QVariant(12));
  QVERIFY(routeData.dutConnectionId().isNull());
  QVERIFY(routeData.testConnectionId().isNull());
  QCOMPARE(routeData.measureConnectionId() ,QVariant(15));
  QCOMPARE(routeData.setupDataList().size() ,0);
  /*
   * Setup data from TestModelItemRoute_view
   */
  // Setup record
  QVERIFY(record.addAllFields("TestModelItemRoute_view", pvDatabaseManager.database()));
  record.setValue("Id_PK", 20);
  record.setValue("TestModelItem_Id_FK", 21);
  record.setValue("TestLink_Id_FK", 22);
  record.setValue("DutConnection_Id_FK", 23);
  record.setValue("TestConnection_Id_FK", 24);
  record.setValue("MeasureTestNodeUnitConnection_Id_FK", 25);
  routeData = mdtTtTestModelItemRouteData(record);
  // Check route data
  QCOMPARE(routeData.id() ,QVariant(20));
  QCOMPARE(routeData.testModelItemId() ,QVariant(21));
  QCOMPARE(routeData.testLinkId() ,QVariant(22));
  QCOMPARE(routeData.dutConnectionId() ,QVariant(23));
  QCOMPARE(routeData.testConnectionId() ,QVariant(24));
  QCOMPARE(routeData.measureConnectionId() ,QVariant(25));
  QCOMPARE(routeData.setupDataList().size() ,0);
  /*
   * Build a list of test node unit setup data,
   *  with fields commeing from TestNodeUnitSetup_view.
   *  So, we have informations about test node units (offset, schema pos., ...)
   *
   * Note: here, we are at TestModelItem_Id_FK 100
   */
  // Update test route data to test model item 100
  routeData.setTestModelItemId(100);
  QCOMPARE(routeData.testModelItemId() ,QVariant(100));
  // Setup record
  QVERIFY(record.addAllFields("TestNodeUnitSetup_view", pvDatabaseManager.database()));
  // Setup for K1, witch is a bus coupling relay
  record.setValue("TestNodeUnit_Id_FK", 1);
  record.setValue("StepNumber", 0);
  record.setValue("State", true);
  record.setValue("Type_Code_FK", "BUSCPLREL");
  record.setValue("IoPosition", 1);
  record.setValue("SchemaPosition", "K1");
  record.setValue("CalibrationOffset", 0.1);
  // Add to route data and check
  setupData = mdtTtTestNodeUnitSetupData(record);
  routeData.addSetupData(setupData);
  QCOMPARE(routeData.setupDataList().size() ,1);
  QCOMPARE(routeData.setupDataList().at(0).value("TestModelItem_Id_FK") ,QVariant(100));
  QCOMPARE(routeData.setupDataList().at(0).value("TestNodeUnit_Id_FK") ,QVariant(1));
  QCOMPARE(routeData.setupDataList().at(0).value("StepNumber") ,QVariant(0));
  QCOMPARE(routeData.setupDataList().at(0).value("State") ,QVariant(true));
  QCOMPARE(routeData.setupDataList().at(0).value("Type_Code_FK") ,QVariant("BUSCPLREL"));
  QCOMPARE(routeData.setupDataList().at(0).value("IoPosition") ,QVariant(1));
  QCOMPARE(routeData.setupDataList().at(0).value("SchemaPosition") ,QVariant("K1"));
  QCOMPARE(routeData.setupDataList().at(0).value("CalibrationOffset") ,QVariant(0.1));
  QCOMPARE(routeData.routeResistance().valueDouble(), 0.1);
  QCOMPARE(routeData.busCouplingRelaysResistance().valueDouble(), 0.1);
  QCOMPARE(routeData.channelRelaysResistance().valueDouble(), 0.0);
  QCOMPARE(routeData.busCouplingRelaysIdList().size(), 1);
  QCOMPARE(routeData.busCouplingRelaysIdList().at(0), QVariant(1));
  QCOMPARE(routeData.channelRelaysIdList().size(), 0);
  // Setup for K2, witch is a bus coupling relay
  record.clearValues();
  record.setValue("TestNodeUnit_Id_FK", 2);
  record.setValue("StepNumber", 0);
  record.setValue("State", true);
  record.setValue("Type_Code_FK", "BUSCPLREL");
  record.setValue("IoPosition", 2);
  record.setValue("SchemaPosition", "K2");
  record.setValue("CalibrationOffset", 0.2);
  // Add to route data and check
  setupData = mdtTtTestNodeUnitSetupData(record);
  routeData.addSetupData(setupData);
  QCOMPARE(routeData.setupDataList().size() ,2);
  QCOMPARE(routeData.setupDataList().at(1).value("TestModelItem_Id_FK") ,QVariant(100));
  QCOMPARE(routeData.setupDataList().at(1).value("TestNodeUnit_Id_FK") ,QVariant(2));
  QCOMPARE(routeData.setupDataList().at(1).value("StepNumber") ,QVariant(0));
  QCOMPARE(routeData.setupDataList().at(1).value("State") ,QVariant(true));
  QCOMPARE(routeData.setupDataList().at(1).value("Type_Code_FK") ,QVariant("BUSCPLREL"));
  QCOMPARE(routeData.setupDataList().at(1).value("IoPosition") ,QVariant(2));
  QCOMPARE(routeData.setupDataList().at(1).value("SchemaPosition") ,QVariant("K2"));
  QCOMPARE(routeData.setupDataList().at(1).value("CalibrationOffset") ,QVariant(0.2));
  QCOMPARE(routeData.routeResistance().valueDouble(), 0.3);
  QCOMPARE(routeData.busCouplingRelaysResistance().valueDouble(), 0.3);
  QCOMPARE(routeData.channelRelaysResistance().valueDouble(), 0.0);
  QCOMPARE(routeData.busCouplingRelaysIdList().size(), 2);
  QCOMPARE(routeData.busCouplingRelaysIdList().at(1), QVariant(2));
  QCOMPARE(routeData.channelRelaysIdList().size(), 0);
  // Setup for K30, witch is a channel relay
  record.clearValues();
  record.setValue("TestNodeUnit_Id_FK", 30);
  record.setValue("StepNumber", 0);
  record.setValue("State", true);
  record.setValue("Type_Code_FK", "CHANELREL");
  record.setValue("IoPosition", 30);
  record.setValue("SchemaPosition", "K30");
  record.setValue("CalibrationOffset", 0.4);
  // Add to route data and check
  setupData = mdtTtTestNodeUnitSetupData(record);
  routeData.addSetupData(setupData);
  QCOMPARE(routeData.setupDataList().size() ,3);
  QCOMPARE(routeData.setupDataList().at(2).value("TestModelItem_Id_FK") ,QVariant(100));
  QCOMPARE(routeData.setupDataList().at(2).value("TestNodeUnit_Id_FK") ,QVariant(30));
  QCOMPARE(routeData.setupDataList().at(2).value("StepNumber") ,QVariant(0));
  QCOMPARE(routeData.setupDataList().at(2).value("State") ,QVariant(true));
  QCOMPARE(routeData.setupDataList().at(2).value("Type_Code_FK") ,QVariant("CHANELREL"));
  QCOMPARE(routeData.setupDataList().at(2).value("IoPosition") ,QVariant(30));
  QCOMPARE(routeData.setupDataList().at(2).value("SchemaPosition") ,QVariant("K30"));
  QCOMPARE(routeData.setupDataList().at(2).value("CalibrationOffset") ,QVariant(0.4));
  QCOMPARE(routeData.routeResistance().valueDouble(), 0.7);
  QCOMPARE(routeData.busCouplingRelaysResistance().valueDouble(), 0.3);
  QCOMPARE(routeData.channelRelaysResistance().valueDouble(), 0.4);
  QCOMPARE(routeData.busCouplingRelaysIdList().size(), 2);
  QCOMPARE(routeData.busCouplingRelaysIdList().at(0), QVariant(1));
  QCOMPARE(routeData.busCouplingRelaysIdList().at(1), QVariant(2));
  QCOMPARE(routeData.channelRelaysIdList().size(), 1);
  QCOMPARE(routeData.channelRelaysIdList().at(0), QVariant(30));
  // Setup for K31, witch is a channel relay
  record.clearValues();
  record.setValue("TestNodeUnit_Id_FK", 31);
  record.setValue("StepNumber", 0);
  record.setValue("State", true);
  record.setValue("Type_Code_FK", "CHANELREL");
  record.setValue("IoPosition", 31);
  record.setValue("SchemaPosition", "K31");
  record.setValue("CalibrationOffset", 0.5);
  // Add to route data and check
  setupData = mdtTtTestNodeUnitSetupData(record);
  routeData.addSetupData(setupData);
  QCOMPARE(routeData.setupDataList().size() ,4);
  QCOMPARE(routeData.setupDataList().at(3).value("TestModelItem_Id_FK") ,QVariant(100));
  QCOMPARE(routeData.setupDataList().at(3).value("TestNodeUnit_Id_FK") ,QVariant(31));
  QCOMPARE(routeData.setupDataList().at(3).value("StepNumber") ,QVariant(0));
  QCOMPARE(routeData.setupDataList().at(3).value("State") ,QVariant(true));
  QCOMPARE(routeData.setupDataList().at(3).value("Type_Code_FK") ,QVariant("CHANELREL"));
  QCOMPARE(routeData.setupDataList().at(3).value("IoPosition") ,QVariant(31));
  QCOMPARE(routeData.setupDataList().at(3).value("SchemaPosition") ,QVariant("K31"));
  QCOMPARE(routeData.setupDataList().at(3).value("CalibrationOffset") ,QVariant(0.5));
  QCOMPARE(routeData.routeResistance().valueDouble(), 1.2);
  QCOMPARE(routeData.busCouplingRelaysResistance().valueDouble(), 0.3);
  QCOMPARE(routeData.channelRelaysResistance().valueDouble(), 0.9);
  QCOMPARE(routeData.busCouplingRelaysIdList().size(), 2);
  QCOMPARE(routeData.busCouplingRelaysIdList().at(0), QVariant(1));
  QCOMPARE(routeData.busCouplingRelaysIdList().at(1), QVariant(2));
  QCOMPARE(routeData.channelRelaysIdList().size(), 2);
  QCOMPARE(routeData.channelRelaysIdList().at(0), QVariant(30));
  QCOMPARE(routeData.channelRelaysIdList().at(1), QVariant(31));
}






void mdtTestToolTest::testItemNodeNodeSetupDataTest()
{
  mdtTtTestItemNodeSetupData tiSetupData;
  mdtTtTestNodeSetupData tnSetupData;
  mdtTtTestModelItemRouteData routeData;
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
  /// \todo Obselete ?
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
   * Setup test node unit setup data from TestNodeUnitSetup_view
   */
  QVERIFY(tnuSetupData.addAllFields("TestNodeUnitSetup_view", pvDatabaseManager.database()));
  QVERIFY(tnuSetupData.contains("TestModelItem_Id_FK"));
  QVERIFY(tnuSetupData.contains("TestNodeUnit_Id_FK"));
  QVERIFY(tnuSetupData.contains("TestModelItemRoute_Id_FK"));
  QVERIFY(tnuSetupData.contains("StepNumber"));
  QVERIFY(tnuSetupData.contains("State"));
  QVERIFY(tnuSetupData.contains("Value"));
  QVERIFY(tnuSetupData.contains("IoPosition"));
  QVERIFY(tnuSetupData.contains("SchemaPosition"));
  QVERIFY(tnuSetupData.contains("CalibrationOffset"));

  /*
   * Build a setup
   *
   * |Item|Node|Unit(pos)|Step|Route|
   * --------------------------------
   * |  1 | 10 | 100 (K1)|  0 | 20  |
   * --------------------------------
   * |  1 | 10 | 101 (K2)|  0 | 20  |
   * --------------------------------
   * |  1 | 10 | 104 (AO)|  1 |null |
   * --------------------------------
   *
   * This gives a total of 2 steps
   */
  QVERIFY(!tiSetupData.hasMoreStep());
  QVERIFY(tiSetupData.currentDeviceIdentification().isNull());
  QCOMPARE(tnSetupData.unitSetupList().size(), 0);
  QCOMPARE(routeData.setupDataList().size(), 0);
  // Set test node base data
  tnSetupData.setNodeIdentification("1000");
  tnSetupData.setDeviceIdentification("W750:1000");
  QCOMPARE(tnSetupData.nodeIdentification(), QString("1000"));
  QCOMPARE(tnSetupData.deviceIdentification(), QVariant("W750:1000"));
  // Setup route data 20
  routeData.setId(20);
  routeData.setTestModelItemId(1);
  // Setup node unit data for K1
  tnuSetupData.setValue("TestModelItem_Id_FK", 1);
  tnuSetupData.setValue("TestNode_Id_FK", 10);
  tnuSetupData.setValue("TestNodeUnit_Id_FK", 100);
  tnuSetupData.setValue("Type_Code_FK", "BUSCPLREL");
  tnuSetupData.setValue("IoPosition", 0);
  tnuSetupData.setValue("SchemaPosition", "K1");
  tnuSetupData.setValue("StepNumber", 0);
  // Add node unit setup of K1 to route data 20
  routeData.addSetupData(tnuSetupData);
  QCOMPARE(routeData.setupDataList().size(), 1);
  // Setup node unit data for K2
  tnuSetupData.clearValues();
  tnuSetupData.setValue("TestModelItem_Id_FK", 1);
  tnuSetupData.setValue("TestNode_Id_FK", 10);
  tnuSetupData.setValue("TestNodeUnit_Id_FK", 101);
  tnuSetupData.setValue("Type_Code_FK", "CHANELREL");
  tnuSetupData.setValue("IoPosition", 1);
  tnuSetupData.setValue("SchemaPosition", "K2");
  tnuSetupData.setValue("StepNumber", 0);
  // Add node unit setup of K2 to route data 20
  routeData.addSetupData(tnuSetupData);
  QCOMPARE(routeData.setupDataList().size(), 2);
  // Add route data to test node data
  tnSetupData.addRouteData(routeData);
  QCOMPARE(tnSetupData.routeDataList().size(), 1);
  QCOMPARE(tnSetupData.unitSetupList().size(), 2);
  // Setup node unit data for AO
  tnuSetupData.setValue("TestModelItem_Id_FK", 1);
  tnuSetupData.setValue("TestNode_Id_FK", 10);
  tnuSetupData.setValue("TestNodeUnit_Id_FK", 101);
  tnuSetupData.setValue("IoPosition", 4);
  tnuSetupData.setValue("SchemaPosition", "K5");
  tnuSetupData.setValue("StepNumber", 1);
  // Add node unit data to test node data
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
  QCOMPARE(tnSetupData.routeDataList().size(), 1);
  QCOMPARE(tnSetupData.routeDataList().at(0).id(), QVariant(20));
  QCOMPARE(tnSetupData.routeDataList().at(0).testModelItemId(), QVariant(1));
  QCOMPARE(tnSetupData.routeDataList().at(0).busCouplingRelaysIdList().size(), 1);
  QCOMPARE(tnSetupData.routeDataList().at(0).busCouplingRelaysIdList().at(0), QVariant(100));
  QCOMPARE(tnSetupData.routeDataList().at(0).channelRelaysIdList().size(), 1);
  QCOMPARE(tnSetupData.routeDataList().at(0).channelRelaysIdList().at(0), QVariant(101));
  QCOMPARE(tnSetupData.routeDataList().at(0).setupDataList().size(), 2);
  QCOMPARE(tnSetupData.routeDataList().at(0).setupDataList().at(0).schemaPosition(), QString("K1"));
  QCOMPARE(tnSetupData.routeDataList().at(0).setupDataList().at(0).schemaPosition(), QString("K2"));
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

/// \todo Is mdtTtTestModelItemData obselete ?
void mdtTestToolTest::mdtTtTestModelItemDataTest()
{
  mdtTtTestModelItemData data;
  mdtTtTestModelItemRouteData routeData;

  QVERIFY(data.setup(pvDatabaseManager.database()));
  QVERIFY(data.contains("Id_PK"));
  // Simple set/get
  data.setValue("Id_PK", 1);
  QCOMPARE(data.value("Id_PK"), QVariant(1));
  // Check route data update
  data.addRouteData(routeData);
  QCOMPARE(data.routeDataList().size(), 1);
  QCOMPARE(data.routeDataList().at(0).testModelItemId(), QVariant(1));
  data.setId(2);
  QCOMPARE(data.value("Id_PK"), QVariant(2));
  QCOMPARE(data.routeDataList().at(0).testModelItemId(), QVariant(2));
  // clear values
  data.clearValues();
  QVERIFY(data.contains("Id_PK"));
  QVERIFY(data.value("Id_PK").isNull());
  QCOMPARE(data.routeDataList().size(), 0);
  // clear
  data.clear();
  QVERIFY(!data.contains("Id_PK"));
  QVERIFY(data.value("Id_PK").isNull());
  QCOMPARE(data.routeDataList().size(), 0);
}

void mdtTestToolTest::testStepTest()
{
  std::shared_ptr<mdtTtTestNodeManager> tnm(new mdtTtTestNodeManager(0, pvDatabaseManager.database()));
  mdtTtValueLimits limits;

  /*
   * Check test step without widget
   */
  mdtTtTestStep ts1(tnm);
  // Initial state
  QVERIFY(ts1.state() == mdtTtTestStep::State_t::Initial);
  QVERIFY(!ts1.abortSupported());
  QVERIFY(ts1.widget() == nullptr);
  /// ...
  ts1.setRunning();
  QVERIFY(ts1.state() == mdtTtTestStep::State_t::Running);
  ts1.reset();
  QVERIFY(ts1.state() == mdtTtTestStep::State_t::Initial);
  /*
   * Check state update when checking values that are out of some limits
   */
  // Setup limits
  limits.setLimits(2.3, 2.4, 2.6, 2.7);
  // Run step
  ts1.setRunning();
  QVERIFY(ts1.state() == mdtTtTestStep::State_t::Running);
  // Check a value that is Ok with isValueOk()
  QVERIFY(ts1.isValueOk(2.5, limits));
  QVERIFY(ts1.state() == mdtTtTestStep::State_t::Running);
  // Check a value that is not ok with isValueOk()
  QVERIFY(!ts1.isValueOk(2.0, limits));
  ///QVERIFY(ts1.state() == mdtTtTestStep::State_t::Fail);

  
  /*
   * Check with a test step widget
   *  -> check when test step is destroyed first
   */
  mdtTtTestStepWidget tsw;
  mdtTtTestStep *ts2 = new mdtTtTestStep(tnm, &tsw);
  tsw.show();
  // Initial state
  QVERIFY(ts2->state() == mdtTtTestStep::State_t::Initial);
  QVERIFY(!ts2->abortSupported());
  QVERIFY(ts2->widget() == &tsw);
  /// ...
  ts2->setRunning();
  QVERIFY(ts2->state() == mdtTtTestStep::State_t::Running);
  ///QTest::qWait(2000);
  ts2->reset();
  QVERIFY(ts2->state() == mdtTtTestStep::State_t::Initial);

  // Delete ts2
  delete ts2;
  ts2 = nullptr;
  // Here, tsw must be valid anymore
  tsw.setMessage("tsw after delete of test step");

  /*
   * Check with a test step widget
   *  -> check when test step widget is destroyed first
   */
  mdtTtTestStepWidget *tsw3 = new mdtTtTestStepWidget;
  mdtTtTestStep ts3(tnm, tsw3);
  tsw3->show();
  // Initial state
  QVERIFY(ts3.state() == mdtTtTestStep::State_t::Initial);
  QVERIFY(!ts3.abortSupported());
  QVERIFY(ts3.widget() == tsw3);
  /// ...
  ts3.setRunning();
  QVERIFY(ts3.state() == mdtTtTestStep::State_t::Running);
  ///QTest::qWait(2000);
  ts3.reset();
  QVERIFY(ts3.state() == mdtTtTestStep::State_t::Initial);

  // Delete tsw3
  delete tsw3;
  tsw3 = nullptr;
  QVERIFY(ts3.widget() == nullptr);
  // Check that running noew does not crash
  ts3.setRunning();
  QVERIFY(ts3.state() == mdtTtTestStep::State_t::Running);

  ///QTest::qWait(2000);
  
  // Play
  /*
  while(tsw.isVisible()){
    QTest::qWait(500);
  }
  */
}

void mdtTestToolTest::testStepContainerTest()
{
  mdtTtTestStepContainer tsc;
  std::shared_ptr<mdtTtTestStep> ts;
  std::shared_ptr<mdtTtTestNodeManager> tnm(new mdtTtTestNodeManager(0, pvDatabaseManager.database()));

  /*
   * Setup test step container widget
   */
  QScrollArea *sa = new QScrollArea;
  sa->setWidgetResizable(true);
  mdtTtTestStepContainerWidget *tscw = new mdtTtTestStepContainerWidget;
  tscw->setContainer(&tsc);
  sa->setWidget(tscw);
  sa->resize(700, 400);
  sa->show();
  /*
   * Create test steps
   */
  // Initial state of container
  QCOMPARE(tsc.count(), 0);
  QVERIFY(tsc.state() == mdtTtTestStep::State_t::Initial);
  QCOMPARE(tscw->stepWidgetsCount(), tsc.count());
  // Create a test step - use version that accepts index
  ts = tsc.addStep(0, tnm, new mdtTtTestStepWidget);
  QCOMPARE(tsc.count(), 1);
  QCOMPARE(tscw->stepWidgetsCount(), tsc.count());
  QVERIFY(ts.get() != nullptr);
  ts->setTitle("Test step 1");
  // Create a test step - use version that accepts index
  ts = tsc.addStep(1, tnm, new mdtTtTestStepWidget);
  QCOMPARE(tsc.count(), 2);
  QCOMPARE(tscw->stepWidgetsCount(), tsc.count());
  QVERIFY(ts.get() != nullptr);
  ts->setTitle("Test step 2");
  // Create a test step
  ts = tsc.addStep(tnm, new mdtTtTestStepWidget);
  QCOMPARE(tsc.count(), 3);
  QCOMPARE(tscw->stepWidgetsCount(), tsc.count());
  QVERIFY(ts.get() != nullptr);
  ts->setTitle("Test step 3");
  // Check global state
  QVERIFY(tsc.state() == mdtTtTestStep::State_t::Initial);
  
  
  ///QTest::qWait(2000);
  /*
   * Run steps
   *  - step 1 succeed
   *  - step 2 finishes with warn
   *  - step 3 fails
   */
  // Run step 1
  tsc.step(0)->setRunning();
  QVERIFY(tsc.state() == mdtTtTestStep::State_t::Running);
  ///QTest::qWait(2000);
  tsc.step(0)->setFinishedSuccess();
  QVERIFY(tsc.state() == mdtTtTestStep::State_t::Initial);
  // Run step 2
  tsc.step(1)->setRunning();
  QVERIFY(tsc.state() == mdtTtTestStep::State_t::Running);
  ///QTest::qWait(2000);
  tsc.step(1)->setFinishedWarn("Just limit");
  QVERIFY(tsc.state() == mdtTtTestStep::State_t::Warn);
  // Run step 3
  tsc.step(2)->setRunning();
  QVERIFY(tsc.state() == mdtTtTestStep::State_t::Running);
  ///QTest::qWait(2000);
  tsc.step(2)->setFinishedFail("Failure");
  QVERIFY(tsc.state() == mdtTtTestStep::State_t::Fail);
  /*
   * Run steps - all steps succeed
   */
  ///QTest::qWait(2000);
  tsc.step(0)->reset();
  tsc.step(1)->reset();
  tsc.step(2)->reset();
  // Run step 1
  tsc.step(0)->setRunning();
  QVERIFY(tsc.state() == mdtTtTestStep::State_t::Running);
  ///QTest::qWait(2000);
  tsc.step(0)->setFinishedSuccess();
  QVERIFY(tsc.state() == mdtTtTestStep::State_t::Initial);
  // Run step 2
  tsc.step(1)->setRunning();
  QVERIFY(tsc.state() == mdtTtTestStep::State_t::Running);
  ///QTest::qWait(2000);
  tsc.step(1)->setFinishedSuccess();
  QVERIFY(tsc.state() == mdtTtTestStep::State_t::Initial);
  // Run step 3
  tsc.step(2)->setRunning();
  QVERIFY(tsc.state() == mdtTtTestStep::State_t::Running);
  ///QTest::qWait(2000);
  tsc.step(2)->setFinishedSuccess();
  QVERIFY(tsc.state() == mdtTtTestStep::State_t::Success);
  /*
   * Check reset
   */
  // Set some states
  tsc.step(0)->setFinishedSuccess();
  QVERIFY(tsc.step(0)->state() == mdtTtTestStep::State_t::Success);
  tsc.step(1)->setFinishedWarn();
  QVERIFY(tsc.step(1)->state() == mdtTtTestStep::State_t::Warn);
  tsc.step(2)->setFinishedFail();
  QVERIFY(tsc.step(2)->state() == mdtTtTestStep::State_t::Fail);
  // Reset and check
  tsc.reset();
  QVERIFY(tsc.step(0)->state() == mdtTtTestStep::State_t::Initial);
  QVERIFY(tsc.step(1)->state() == mdtTtTestStep::State_t::Initial);
  QVERIFY(tsc.step(2)->state() == mdtTtTestStep::State_t::Initial);
  /*
   * Check clear
   */
  tsc.clear();
  QCOMPARE(tsc.count(), 0);
  QCOMPARE(tscw->stepWidgetsCount(), tsc.count());

  // Play
  /*
  while(sa->isVisible()){
    QTest::qWait(500);
  }
  */

  // Cleanup
  delete sa;
//  delete tscw;
}

void mdtTestToolTest::testStepContainerBenchmark()
{
  mdtTtTestStepContainer tsc;
  std::shared_ptr<mdtTtTestStep> ts;
  std::shared_ptr<mdtTtTestNodeManager> tnm(new mdtTtTestNodeManager(0, pvDatabaseManager.database()));
  mdtTtTestStep::State_t globalState;

  tsc.addStep(tnm);
  tsc.addStep(tnm);
  tsc.addStep(tnm);

  QBENCHMARK{
    for(int i = 0; i < tsc.count(); ++i){
      ts = tsc.step(i);
      ts->reset();
    }
    globalState = tsc.state();
  }
  QVERIFY(globalState == mdtTtTestStep::State_t::Initial);
}

void mdtTestToolTest::mdtTtTestTest()
{
  /*
   * Simple range checking
   */
  QVERIFY(mdtTtTest::isInRange(1.0, 0.0, 2.0));
  QVERIFY(mdtTtTest::isInRange(0.0, 0.0, 2.0));
  QVERIFY(mdtTtTest::isInRange(2.0, 0.0, 2.0));
  QVERIFY(!mdtTtTest::isInRange(-0.01, 0.0, 2.0));
  QVERIFY(!mdtTtTest::isInRange(2.01, 0.0, 2.0));
  QVERIFY(!mdtTtTest::isInRange(-0.01, 0.0));
  QVERIFY(mdtTtTest::isInRange(0.0, 0.0));
  QVERIFY(mdtTtTest::isInRange(1.0, 0.0));
  QVERIFY(mdtTtTest::isInRange(1e15, 0.0));
  QVERIFY(mdtTtTest::isInRange(std::numeric_limits<double>::max(), 0.0));
  QVERIFY(!mdtTtTest::isInRange(-10.01, -10.0));
  QVERIFY(mdtTtTest::isInRange(-10.0, -10.0));
  QVERIFY(mdtTtTest::isInRange(0.0, -10.0));
  QVERIFY(mdtTtTest::isInRange(1.0, -10.0));
  QVERIFY(mdtTtTest::isInRange(1e15, -10.0));
  QVERIFY(mdtTtTest::isInRange(std::numeric_limits<double>::max(), -10.0));

  /*
   * Range checking with fail and limit values
   *
   *        failMin   limitMix     expectedValue      limitMax    failMax
   * -inf------]----------]---------------|---------------[----------[------+inf
   *
   * ex:      800        900             1000            1100       1200
   */
  QVERIFY(mdtTtTest::isInOkRange(1000.0, 900.0, 1100.0));
  QVERIFY(mdtTtTest::isInOkRange(900.1, 900.0, 1100.0));
  QVERIFY(mdtTtTest::isInOkRange(1099.9, 900.0, 1100.0));
  QVERIFY(!mdtTtTest::isInOkRange(900.0, 900.0, 1100.0));
  QVERIFY(!mdtTtTest::isInOkRange(1100.0, 900.0, 1100.0));
  QVERIFY(!mdtTtTest::isInLimitRange(1000.0, 900.0, 1100.0, 800.0, 1200.0));
  QVERIFY(!mdtTtTest::isInLimitRange(900.1, 900.0, 1100.0, 800.0, 1200.0));
  QVERIFY(!mdtTtTest::isInLimitRange(1099.9, 900.0, 1100.0, 800.0, 1200.0));
  QVERIFY(mdtTtTest::isInLimitRange(900.0, 900.0, 1100.0, 800.0, 1200.0));
  QVERIFY(mdtTtTest::isInLimitRange(1100.0, 900.0, 1100.0, 800.0, 1200.0));
  QVERIFY(mdtTtTest::isInLimitRange(800.1, 900.0, 1100.0, 800.0, 1200.0));
  QVERIFY(mdtTtTest::isInLimitRange(850.0, 900.0, 1100.0, 800.0, 1200.0));
  QVERIFY(mdtTtTest::isInLimitRange(1199.9, 900.0, 1100.0, 800.0, 1200.0));
  QVERIFY(mdtTtTest::isInLimitRange(1150.0, 900.0, 1100.0, 800.0, 1200.0));
  QVERIFY(!mdtTtTest::isInLimitRange(800.0, 900.0, 1100.0, 800.0, 1200.0));
  QVERIFY(!mdtTtTest::isInLimitRange(1200.0, 900.0, 1100.0, 800.0, 1200.0));
  QVERIFY(!mdtTtTest::isInLimitRange(750.0, 900.0, 1100.0, 800.0, 1200.0));
  QVERIFY(!mdtTtTest::isInLimitRange(1250.0, 900.0, 1100.0, 800.0, 1200.0));
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
