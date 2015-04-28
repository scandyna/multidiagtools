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
#include "mdtTestToolTest.h"
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
#include "mdtTtTestNodeRouteData.h"
#include "mdtTtTestNodeSetupData.h"
#include "mdtTtTestNodeUnitSetupData.h"
///#include "mdtTtTestModelItemData.h"
///#include "mdtTtTestModelItemRouteData.h"
#include "mdtTtTestNodeManager.h"
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
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
///#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QFile>
#include <QVariant>
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
  data.addRelayToEnable(5, "K5");
  data.addRelayToEnable(6, "K6");
  // Check
  QCOMPARE(data.id(), QVariant(1));
  QCOMPARE(data.testNodeId(), QVariant(2));
  QCOMPARE(data.connectionAId(), QVariant(3));
  QCOMPARE(data.connectionBId(), QVariant(4));
  QCOMPARE(data.relaysToEnableCount(), 2);
  // Check clear
  data.clear();
  QVERIFY(data.id().isNull());
  QVERIFY(data.testNodeId().isNull());
  QVERIFY(data.connectionAId().isNull());
  QVERIFY(data.connectionBId().isNull());
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
  // Save route
  QVERIFY(tnr.addRoute(route));
  
  QSqlQuery q(pvDatabaseManager.database());
  q.exec("SELECT * FROM TestNodeRoute_view");
  while(q.next()){
    qDebug() << q.record();
  }
  
  // Get route from DB and check
  route = tnr.getRoute(testNodeId, connectionAId, connectionBId, ok);
  QVERIFY(ok);
  QVERIFY(!route.id().isNull());
  QCOMPARE(route.testNodeId(), testNodeId);
  QCOMPARE(route.connectionAId(), connectionAId);
  QCOMPARE(route.connectionBId(), connectionBId);
  QCOMPARE(route.relaysToEnableCount(), 2);
  
  // Remove route
  QVERIFY(tnr.removeRoute(route.id()));

}

void mdtTestNodeTest::routeAddRemoveTest()
{
  mdtTtTestNodeRoute route(0, pvDatabaseManager.database());
  mdtTtTestNodeRouteData data;
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
