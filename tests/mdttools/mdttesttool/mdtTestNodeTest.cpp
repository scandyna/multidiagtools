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
#include "mdtTestNodeTest.h"
#include "mdtTtTestNode.h"
#include "mdtTtTestNodeUnit.h"
///#include "mdtTestToolTest.h"
#include "mdtCableListTestScenario.h"
#include "mdtApplication.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlRecord.h"
#include "mdtTtBase.h"
#include "mdtTtTestLinkData.h"
#include "mdtTtTestNodeUnit.h"
#include "mdtTtTestNodeUnitData.h"
#include "mdtSqlTableSelection.h"
#include "mdtClUnitConnectionData.h"
///#include "mdtTtTestItemNodeSetupData.h"
#include "mdtTtTestNodeRoute.h"
#include "mdtTtTestNodeSetupData.h"
#include "mdtTtTestNodeUnitSetupData.h"
///#include "mdtTtTestModelItemData.h"
///#include "mdtTtTestModelItemRouteData.h"
#include "mdtTtTestNodeManager.h"
#include "mdtMultiIoDevice.h"
#include "mdtDigitalIo.h"
///#include "mdtDeviceContainer.h"
///#include "mdtDeviceContainerWidget.h"
///#include "mdtDeviceU3606A.h"
///#include "mdtTtValueLimits.h"
///#include "mdtTtTestStep.h"
///#include "mdtTtTestStepContainer.h"
///#include "mdtTtTestStepWidget.h"
///#include "mdtTtTestStepContainerWidget.h"
#include "mdtTtTestNodeTestData.h"
#include <QTemporaryFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
///#include <QWidget>
///#include <QScrollArea>
///#include <QDialog>
#include <QStringList>
#include <QSqlDatabase>
///#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QFile>
///#include <QTimer>
///#include <QModelIndex>
#include <QList>
#include <QFileInfo>

#include <QDebug>

void mdtTestNodeTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabaseManager.database().isOpen());
}

void mdtTestNodeTest::cleanupTestCase()
{
}

void mdtTestNodeTest::testNodeUnitTest()
{
  mdtTtTestNodeUnit tnu(pvDatabaseManager.database());
  mdtTtTestNodeATestData tnd(pvDatabaseManager.database());
  QVariant id;
  bool ok;

  // Create test node A
  QVERIFY(tnd.populate());
  /*
   * Check connection ID getters
   */
  // Check with schema position and connection
  id = tnu.getConnectionId("A", "XMEAS", "+", ok);
  QVERIFY(ok);
  QVERIFY(!id.isNull());
  id = tnu.getConnectionId("A", "XMEAS", "-", ok);
  QVERIFY(ok);
  QVERIFY(!id.isNull());
  id = tnu.getConnectionId("FAKE", "XMEAS", "-", ok);
  QVERIFY(ok);
  QVERIFY(id.isNull());
  id = tnu.getConnectionId("A", "FAKE", "+", ok);
  QVERIFY(ok);
  QVERIFY(id.isNull());

}

void mdtTestNodeTest::testNodeRouteDataTest()
{
  mdtTtTestNodeRouteData data;

  /*
   * Simple set/get test
   */
  // Set data
  data.setId(1);
  data.setTestNodeId(2);
  data.setConnectionAId(3);
  data.setConnectionBId(4);
  data.setResistance(1.2);
  data.setCalibrationDate(QDateTime::fromString("2015-04-29T17:25:47", Qt::ISODate));
  data.addRelayToEnable(5, "K5", 4);
  data.addRelayToEnable(6, "K6", 5);
  // Check
  QCOMPARE(data.id(), QVariant(1));
  QCOMPARE(data.testNodeId(), QVariant(2));
  QCOMPARE(data.connectionAId(), QVariant(3));
  QCOMPARE(data.connectionBId(), QVariant(4));
  QCOMPARE(data.resistance().value(), 1.2);
  QCOMPARE(data.calibrationDate(), QDateTime::fromString("2015-04-29T17:25:47", Qt::ISODate));
  QCOMPARE(data.relaysToEnableCount(), 2);
  QCOMPARE(data.relayToEnableAt(0).id, QVariant(5));
  QCOMPARE(data.relayToEnableAt(0).schemaPosition, QVariant("K5"));
  QCOMPARE(data.relayToEnableAt(0).ioPosition, 4);
  QCOMPARE(data.relayToEnableAt(1).id, QVariant(6));
  QCOMPARE(data.relayToEnableAt(1).schemaPosition, QVariant("K6"));
  QCOMPARE(data.relayToEnableAt(1).ioPosition, 5);
  // Check clear
  data.clear();
  QVERIFY(data.id().isNull());
  QVERIFY(data.testNodeId().isNull());
  QVERIFY(data.connectionAId().isNull());
  QVERIFY(data.connectionBId().isNull());
  QVERIFY(data.resistance().isNull());
  QVERIFY(data.calibrationDate().isNull());
  QCOMPARE(data.relaysToEnableCount(), 0);
}

void mdtTestNodeTest::routeBuildTest()
{
  mdtTtTestNode tn(pvDatabaseManager.database());
  mdtTtTestNodeUnit tnu(pvDatabaseManager.database());
  mdtTtTestNodeATestData tnAd(pvDatabaseManager.database());
  mdtTtTestNodeRoute tnr(pvDatabaseManager.database());
  mdtTtTestNodeRouteData route;
  QVariant testNodeId;
  QVariant connectionAId, connectionBId;
  mdtClPathGraph graph(pvDatabaseManager.database());
  bool ok;

  // Create test node and load graph's link list
  QVERIFY(tnAd.populate());
  
  
  QVERIFY(graph.loadLinkList());
  // Get test node ID
  testNodeId = tn.getTestNodeIdForAlias("A");
  QVERIFY(!testNodeId.isNull());
  // Add test node's relays to graph
  QVERIFY(tn.addRelaysToGraph(testNodeId, graph));
  /*
   * Build a route XMEAS+ to XTEST;A1
   * Must enable relays K1 and K30
   */
  // Get connection IDs
  connectionAId = tnu.getConnectionId("A", "XMEAS", "+", ok);
  QVERIFY(ok);
  QVERIFY(!connectionAId.isNull());
  connectionBId = tnu.getConnectionId("A", "XTEST", "A1", ok);
  QVERIFY(ok);
  QVERIFY(!connectionBId.isNull());
  // Build route
  route = tnr.buildRoute(testNodeId, connectionAId, connectionBId, graph, ok);
  QVERIFY(ok);
  ///QVERIFY(!route.isEmpty());
  QVERIFY(route.id().isNull()); // Route ID cannot be set before route is stored to database (auto increment ID)
  QCOMPARE(route.testNodeId(), testNodeId);
  QCOMPARE(route.connectionAId(), connectionAId);
  QCOMPARE(route.connectionBId(), connectionBId);
  QCOMPARE(route.relaysToEnableCount(), 2);
  QCOMPARE(route.relayToEnableAt(0).schemaPosition, QVariant("K1"));
  QCOMPARE(route.relayToEnableAt(0).ioPosition, 0);
  QCOMPARE(route.relayToEnableAt(1).schemaPosition, QVariant("K30"));
  QCOMPARE(route.relayToEnableAt(1).ioPosition, 29);
  // Save route
  QVERIFY(tnr.addRoute(route));
  // Get route from DB and check
  route = tnr.getRoute(testNodeId, connectionAId, connectionBId, ok);
  QVERIFY(ok);
  QVERIFY(!route.id().isNull());
  QCOMPARE(route.testNodeId(), testNodeId);
  QCOMPARE(route.connectionAId(), connectionAId);
  QCOMPARE(route.connectionBId(), connectionBId);
  QCOMPARE(route.relaysToEnableCount(), 2);
  QCOMPARE(route.relayToEnableAt(0).schemaPosition, QVariant("K1"));
  QCOMPARE(route.relayToEnableAt(0).ioPosition, 0);
  QCOMPARE(route.relayToEnableAt(1).schemaPosition, QVariant("K30"));
  QCOMPARE(route.relayToEnableAt(1).ioPosition, 29);
  // Remove route
  QVERIFY(tnr.removeRoute(route.id()));

}

void mdtTestNodeTest::routeAddRemoveTest()
{
  mdtTtTestNode tn(pvDatabaseManager.database());
  mdtTtTestNodeRoute tnr(pvDatabaseManager.database());
  mdtTtTestNodeATestData tnAd(pvDatabaseManager.database());
  mdtTtTestNodeRouteData createdRoute, route;
  QVariant testNodeId;
  QList<mdtTtTestNodeRouteData> routeDataList;
  bool ok;

  // Create test node A
  QVERIFY(tnAd.populate());
  // Get test node ID
  testNodeId = tn.getTestNodeIdForAlias("A");
  QVERIFY(!testNodeId.isNull());
  /*
   * Build a route XMEAS+ to XTEST;A1
   *  - Must enable relays K1 and K30
   *  - We not set route resistance and calibration date
   */
  createdRoute = builRoute("A", "XMEAS", "+", "XTEST", "A1", ok);
  QVERIFY(ok);
  // Store route
  QVERIFY(tnr.addRoute(createdRoute));
  // Get route back and check
  route = getRoute("A", "XMEAS", "+", "XTEST", "A1", ok);
  QVERIFY(ok);
  QVERIFY(!route.id().isNull());
  QCOMPARE(route.testNodeId(), createdRoute.testNodeId());
  QCOMPARE(route.connectionAId(), createdRoute.connectionAId());
  QCOMPARE(route.connectionBId(), createdRoute.connectionBId());
  QVERIFY(route.resistance().isNull());
  QVERIFY(route.calibrationDate().isNull());
  QCOMPARE(route.relaysToEnableCount(), createdRoute.relaysToEnableCount());
  // Check all routes getter
  routeDataList = tnr.getAllRoutes(testNodeId, ok);
  QVERIFY(ok);
  QCOMPARE(routeDataList.size(), 1);
  routeDataList = tnr.getAllRoutesByAlias("A", ok);
  QVERIFY(ok);
  QCOMPARE(routeDataList.size(), 1);
  // Update route resistance in database using mdtTtTestNodeRoute
  QVERIFY(tnr.setRouteResistance(route.id(), 0.7, QDateTime::fromString("2015-02-12T10:22:33", Qt::ISODate)));
  // Get route back and check
  route = getRoute("A", "XMEAS", "+", "XTEST", "A1", ok);
  QVERIFY(ok);
  QCOMPARE(route.resistance().value(), 0.7);
  QCOMPARE(route.calibrationDate(), QDateTime::fromString("2015-02-12T10:22:33", Qt::ISODate));
  // Remove route
  ///QVERIFY(tnr.removeRoute(route.id()));
  /*
   * Build a route XMEAS- to XTEST;A2
   *  - Must enable relays K4 and K40
   *  - We also set route resistance and calibration date
   */
  createdRoute = builRoute("A", "XMEAS", "-", "XTEST", "A2", ok);
  QVERIFY(ok);
  // Update route resistance
  createdRoute.setResistance(2.4);
  createdRoute.setCalibrationDate(QDateTime::fromString("2014-03-29T17:44:56", Qt::ISODate));
  // Store route
  QVERIFY(tnr.addRoute(createdRoute));
  // Get route back and check
  route = getRoute("A", "XMEAS", "-", "XTEST", "A2", ok);
  QVERIFY(ok);
  QVERIFY(!route.id().isNull());
  QCOMPARE(route.testNodeId(), createdRoute.testNodeId());
  QCOMPARE(route.connectionAId(), createdRoute.connectionAId());
  QCOMPARE(route.connectionBId(), createdRoute.connectionBId());
  QCOMPARE(route.resistance(), createdRoute.resistance());
  QCOMPARE(route.calibrationDate(), createdRoute.calibrationDate());
  QCOMPARE(route.relaysToEnableCount(), createdRoute.relaysToEnableCount());
  // Check all routes getter
  routeDataList = tnr.getAllRoutes(testNodeId, ok);
  QCOMPARE(routeDataList.size(), 2);
  QVERIFY(ok);
  routeDataList = tnr.getAllRoutesByAlias("A", ok);
  QVERIFY(ok);
  QCOMPARE(routeDataList.size(), 2);
  // Remove routes that where created for test node A
  routeDataList = tnr.getAllRoutes(testNodeId, ok);
  QVERIFY(ok);
  for(const auto & routeData : routeDataList){
    QVERIFY(tnr.removeRoute(routeData.id()));
  }
  // Check all routes getter
  routeDataList = tnr.getAllRoutes(testNodeId, ok);
  QCOMPARE(routeDataList.size(), 0);

}

void mdtTestNodeTest::shortDetectionTest()
{
  mdtTtTestNode tn(pvDatabaseManager.database());
  mdtTtTestNodeRoute tnr(0, pvDatabaseManager.database());
  mdtTtTestNodeATestData tnAd(pvDatabaseManager.database());
  mdtClPathGraph graph(pvDatabaseManager.database());
  mdtTtTestNodeRouteData route;
  QVariant connectionAId, connectionBId;
  bool ok;

  // Create test node A
  QVERIFY(tnAd.populate());
  // Load link list
  QVERIFY(graph.loadLinkList());
  /*
   * Build a route XMEAS+ to XTEST;A1
   * Must enable relays K1 and K30
   */
  route = builRoute("A", "XMEAS", "+", "XTEST", "A1", ok);
  QVERIFY(ok);
  // Check about short
  connectionAId = getConnectionId("A", "XMEAS", "+", ok);
  QVERIFY(ok);
  connectionBId = getConnectionId("A", "XMEAS", "-", ok);
  QVERIFY(ok);
  QVERIFY(!tn.makesShortCircuit(connectionAId, connectionBId, route.relaysToEnableVector(), graph, ok));
  QVERIFY(ok);
  connectionAId = getConnectionId("A", "XMEAS", "+", ok);
  QVERIFY(ok);
  connectionBId = getConnectionId("A", "XTEST", "A1", ok);
  QVERIFY(ok);
  QVERIFY(tn.makesShortCircuit(connectionAId, connectionBId, route.relaysToEnableVector(), graph, ok));
  QVERIFY(ok);
  /// \todo tester en donnat B et A (inverse)

}

void mdtTestNodeTest::testNodeManagerTest()
{
  mdtTtTestNodeATestData tnAd(pvDatabaseManager.database());
  mdtTtTestNodeManager tnm(pvDatabaseManager.database());
  mdtDigitalIo *dout;

  // Create test node A in database
  QVERIFY(tnAd.populate());
  // Add test node A
  auto device = tnm.addDevice<mdtMultiIoDevice>("A");
  QVERIFY(tnm.device<mdtMultiIoDevice>("A") == device);
  QCOMPARE(tnm.allDevices().count(), 1);
  QCOMPARE(tnm.allDevices().at(0)->alias(), QString("A"));
  /*
   * Some subclasses of mdtMultiIoDevice can, once connected to device,
   * detect the I/O map of physical device.
   * Here, we build it manually regarding test node A.
   */
  /*
   * Test I/O coherence map checking functions
   */
  // Check with empty device I/O container
  QVERIFY(!tnm.checkDeviceIoMapCoherence("A"));
  // Add to less relays
  for(int i = 0; i < 10; ++i){
    dout = new mdtDigitalIo;
    dout->setAddress(i);
    device->addOutput(dout);
  }
  QVERIFY(!tnm.checkDeviceIoMapCoherence("A"));
  // Add enouth relays
  device->ios()->deleteIos();
  for(int i = 0; i < 41; ++i){
    dout = new mdtDigitalIo;
    dout->setAddress(i);
    device->addOutput(dout);
  }
  QVERIFY(tnm.checkDeviceIoMapCoherence("A"));
  /*
   * Check I/O labelling
   */
  QVERIFY(tnm.setDeviceIosLabelShort("A"));
  auto digitalOuptus = device->ios()->digitalOutputs();
  // Check K1
  dout = digitalOuptus.at(0);
  QCOMPARE(dout->labelShort(), QString("K1"));
  // Check K2
  dout = digitalOuptus.at(1);
  QCOMPARE(dout->labelShort(), QString("K2"));
  // Check K3
  dout = digitalOuptus.at(2);
  QCOMPARE(dout->labelShort(), QString("K3"));
  // Check K4
  dout = digitalOuptus.at(3);
  QCOMPARE(dout->labelShort(), QString("K4"));
  // Check K30
  dout = digitalOuptus.at(29);
  QCOMPARE(dout->labelShort(), QString("K30"));
  // Check K31
  dout = digitalOuptus.at(30);
  QCOMPARE(dout->labelShort(), QString("K31"));
  // Check K40
  dout = digitalOuptus.at(39);
  QCOMPARE(dout->labelShort(), QString("K40"));
  // Check K40
  dout = digitalOuptus.at(40);
  QCOMPARE(dout->labelShort(), QString("K41"));
  /*
   * Check setting relays
   */
  // Build a relays list
  mdtTtTestNodeRouteData route;
  route.addRelayToEnable(0, "K1", 0);
  route.addRelayToEnable(0, "K2", 1);
  route.addRelayToEnable(0, "K30", 29);
  route.addRelayToEnable(0, "K40", 39);
  // Set relays to enable to device
  QVERIFY(tnm.setRelaysToEnable(route.relaysToEnableVector(), device));
  // Check digital outputs states
  QVERIFY(digitalOuptus.at(0)->value().valueBool() == true);
  QVERIFY(digitalOuptus.at(1)->value().valueBool() == true);
  QVERIFY(digitalOuptus.at(2)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(3)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(4)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(5)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(6)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(7)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(8)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(9)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(10)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(11)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(12)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(13)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(14)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(15)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(16)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(17)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(18)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(19)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(20)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(21)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(22)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(23)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(24)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(25)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(26)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(27)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(28)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(29)->value().valueBool() == true);
  QVERIFY(digitalOuptus.at(30)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(31)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(32)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(33)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(34)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(35)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(36)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(37)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(38)->value().valueBool() == false);
  QVERIFY(digitalOuptus.at(39)->value().valueBool() == true);
}

QVariant mdtTestNodeTest::getConnectionId(const QString & testNodeAlias, const QString & schemaPosition, const QString & contact, bool & ok)
{
  mdtTtTestNodeUnit tnu(pvDatabaseManager.database());

  return tnu.getConnectionId(testNodeAlias, schemaPosition, contact, ok);
}

mdtTtTestNodeRouteData mdtTestNodeTest::builRoute(const QString & testNodeAlias, const QString & schemaPositionA, const QString & contactA, const QString & schemaPositionB, const QString & contactB, bool & ok)
{
  mdtTtTestNodeRouteData routeData;
  mdtTtTestNode tn(pvDatabaseManager.database());
  mdtTtTestNodeUnit tnu(pvDatabaseManager.database());
  mdtClPathGraph graph(pvDatabaseManager.database());
  mdtTtTestNodeRoute tnr(pvDatabaseManager.database());
  QVariant testNodeId;
  QVariant connectionAId, connectionBId;

  // Get test node ID
  testNodeId = tn.getTestNodeIdForAlias(testNodeAlias);
  if(testNodeId.isNull()){
    ok = false;
    return routeData;
  }
  // Load link list and add test node's relays to graph
  if(!graph.loadLinkList()){
    ok = false;
    return routeData;
  }
  if(!tn.addRelaysToGraph(testNodeId, graph)){
    ok = false;
    return routeData;
  }
  // Get connection IDs
  connectionAId = tnu.getConnectionId(testNodeAlias, schemaPositionA, contactA, ok);
  if(!ok){
    return routeData;
  }
  connectionBId = tnu.getConnectionId(testNodeAlias, schemaPositionB, contactB, ok);
  if(!ok){
    return routeData;
  }
  // Build route
  routeData = tnr.buildRoute(testNodeId, connectionAId, connectionBId, graph, ok);
  if(!ok){
    return routeData;
  }

  return routeData;
}

mdtTtTestNodeRouteData mdtTestNodeTest::getRoute(const QString & testNodeAlias, const QString & schemaPositionA, const QString & contactA, const QString & schemaPositionB, const QString & contactB, bool & ok)
{
  mdtTtTestNodeRouteData routeData;
  mdtTtTestNode tn(pvDatabaseManager.database());
  mdtTtTestNodeUnit tnu(pvDatabaseManager.database());
  mdtTtTestNodeRoute tnr(pvDatabaseManager.database());
  QVariant testNodeId;
  QVariant connectionAId, connectionBId;

  // Get test node ID
  testNodeId = tn.getTestNodeIdForAlias(testNodeAlias);
  if(testNodeId.isNull()){
    ok = false;
    return routeData;
  }
  // Get connection IDs
  connectionAId = tnu.getConnectionId(testNodeAlias, schemaPositionA, contactA, ok);
  if(!ok){
    return routeData;
  }
  connectionBId = tnu.getConnectionId(testNodeAlias, schemaPositionB, contactB, ok);
  if(!ok){
    return routeData;
  }
  // Get route
  routeData = tnr.getRoute(testNodeId, connectionAId, connectionBId, ok);
  if(!ok){
    return routeData;
  }

  return routeData;
}

void mdtTestNodeTest::createDatabaseSchema()
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
  mdtTestNodeTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
