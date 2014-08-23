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
#include "mdtCableListTest.h"
#include "mdtApplication.h"
#include "mdtTtDatabaseSchema.h"
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
#include "mdtClDirectLink.h"
#include "mdtClLinkBeam.h"
#include "mdtClPathGraph.h"
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

void mdtCableListTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabaseManager.database().isOpen());
  pvScenario = new mdtCableListTestScenario(pvDatabaseManager.database());
}

void mdtCableListTest::cleanupTestCase()
{
  delete pvScenario;
}

void mdtCableListTest::articleConnectionDataTest()
{
  mdtClArticleConnectionData connectionData;

  // Check setup
  QVERIFY(connectionData.setup(pvDatabaseManager.database()));
  // Data set/get ..
  connectionData.setValue("Id_PK", 1);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(1));
}

void mdtCableListTest::articleConnectorDataTest()
{
  mdtClConnectorData connectorData;
  mdtClArticleConnectorData data;
  mdtClArticleConnectionData connectionData;
  bool ok;

  // Check setup
  QVERIFY(data.setup(pvDatabaseManager.database(), true));
  QVERIFY(!data.isBasedOnConnector());
  QCOMPARE(data.connectionDataList().size(), 0);
  // Set values and check
  data.setValue("Id_PK", 1);
  data.setValue("Article_Id_FK", 10);
  data.setValue("Connector_Id_FK", 100);
  QCOMPARE(data.value("Id_PK"), QVariant(1));
  QCOMPARE(data.value("Article_Id_FK"), QVariant(10));
  QCOMPARE(data.value("Connector_Id_FK"), QVariant(100));
  QVERIFY(data.isBasedOnConnector());
  // Add some connection data
  QVERIFY(connectionData.setup(pvDatabaseManager.database()));
  connectionData.setValue("Id_PK", 2);
  ///connectionData.setValue("Article_Id_FK", 10);
  ///connectionData.setValue("ArticleConnector_Id_FK", 1);
  data.addConnectionData(connectionData);
  QCOMPARE(data.connectionDataList().size(), 1);
  QCOMPARE(data.connectionDataList().at(0).value("Id_PK"), QVariant(2));
  QCOMPARE(data.connectionDataList().at(0).value("Article_Id_FK"), QVariant(10));
  QCOMPARE(data.connectionDataList().at(0).value("ArticleConnector_Id_FK"), QVariant(1));
  // Get data by connection ID - Existing ID
  connectionData.clear();
  QCOMPARE(connectionData.count(), 0);
  connectionData = data.connectionData(2, &ok);
  QVERIFY(ok);
  QVERIFY(connectionData.count() > 0);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(2));
  QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(10));
  QCOMPARE(connectionData.value("ArticleConnector_Id_FK"), QVariant(1));
  // Get data by connection ID - Not existing ID
  connectionData = data.connectionData(25, &ok);
  QVERIFY(!ok);
  // Assign a connector
  QVERIFY(connectorData.setup(pvDatabaseManager.database()));
  connectorData.setValue("Id_PK", 50);
  data.setConnectorData(connectorData);
  QVERIFY(data.isBasedOnConnector());
  QCOMPARE(data.value("Connector_Id_FK"), QVariant(50));
  // Clear values
  QVERIFY(data.connectionDataList().size() > 0);
  data.clearValues();
  QCOMPARE(data.value("Id_PK"), QVariant(QVariant::Int));
  QCOMPARE(data.value("Article_Id_FK"), QVariant(QVariant::Int));
  QCOMPARE(data.value("Connector_Id_FK"), QVariant(QVariant::Int));
  QVERIFY(!data.isBasedOnConnector());
  QCOMPARE(data.connectionDataList().size(), 0);
  // Add a connection and check clear
  QVERIFY(connectionData.setup(pvDatabaseManager.database()));
  data.addConnectionData(connectionData);
  QCOMPARE(data.connectionDataList().size(), 1);
  data.clear();
  QCOMPARE(data.value("Id_PK"), QVariant());
  QCOMPARE(data.value("Article_Id_FK"), QVariant());
  QCOMPARE(data.value("Connector_Id_FK"), QVariant());
  QVERIFY(!data.isBasedOnConnector());
  QCOMPARE(data.connectionDataList().size(), 0);
}

void mdtCableListTest::articleTest()
{
  mdtClArticle art(0, pvDatabaseManager.database());
  mdtSqlRecord record;
  QSqlRecord data;
  QList<QSqlRecord> dataList;
  mdtClConnectorData baseConnectorData;
  mdtClArticleConnectorData connectorData;
  mdtClArticleConnectionData connectionData;
  QList<QVariant> connectorContactIdList;
  bool ok;

  /*
   * Simple article manipulation
   */

  // Add a article
  QVERIFY(record.addAllFields("Article_tbl", pvDatabaseManager.database()));
  record.setValue("Id_PK", 1);
  record.setValue("ArticleCode", "1234");
  record.setValue("DesignationEN", "Article 1234");
  QVERIFY(art.addRecord(record, "Article_tbl"));
  // Check added article
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(1));
  QCOMPARE(data.value("ArticleCode"), QVariant("1234"));
  QCOMPARE(data.value("DesignationEN"), QVariant("Article 1234"));
  // Add a article
  record.clearValues();
  record.setValue("Id_PK", 2);
  record.setValue("ArticleCode", "ABC");
  record.setValue("DesignationEN", "Article ABC");
  QVERIFY(art.addRecord(record, "Article_tbl"));
  // Check existing articles
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(1));
  QCOMPARE(data.value("ArticleCode"), QVariant("1234"));
  QCOMPARE(data.value("DesignationEN"), QVariant("Article 1234"));
  data = dataList.at(1);
  QCOMPARE(data.value("Id_PK"), QVariant(2));
  QCOMPARE(data.value("ArticleCode"), QVariant("ABC"));
  QCOMPARE(data.value("DesignationEN"), QVariant("Article ABC"));

  /*
   * Article component
   */

  // Initially we have not assigned any component
  dataList = art.getData("SELECT * FROM ArticleComponent_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Assign article ID 2 as component of article ID 1
  QVERIFY(art.addComponent(1, 2, 5, "pce"));
  // Check back
  dataList = art.getData("SELECT * FROM ArticleComponent_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  QCOMPARE(data.value("Article_Id_PK"), QVariant(1));
  QCOMPARE(data.value("Component_Id_PK"), QVariant(2));
  QCOMPARE(data.value("ComponentQty"), QVariant(5));
  QCOMPARE(data.value("ComponentQtyUnit"), QVariant("pce"));
  // Try to remove wrong component assignation - Note: possibly ok that no error is returned
  art.removeComponent(5, 3);
  dataList = art.getData("SELECT * FROM ArticleComponent_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  // Remove component assignation
  QVERIFY(art.removeComponent(1, 2));
  dataList = art.getData("SELECT * FROM ArticleComponent_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);

  /*
   * Article connection
   */

  QVERIFY(connectionData.setup(pvDatabaseManager.database()));
  ///QVERIFY(record.addAllFields("ArticleConnection_tbl", pvDatabaseManager.database()));
  // Initially we have no connections
  dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Add a connection to article ID 1
  connectionData.setValue("Id_PK", 1);
  connectionData.setValue("Article_Id_FK", 1);
  connectionData.setValue("ArticleContactName", "Contact 1");
  connectionData.setValue("FunctionEN", "EN: Contact 1");
  QVERIFY(art.addConnection(connectionData));
  // Check back
  dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(1));
  QCOMPARE(data.value("Article_Id_FK"), QVariant(1));
  QCOMPARE(data.value("ArticleContactName"), QVariant("Contact 1"));
  QVERIFY(data.value("ArticleConnector_Id_FK").isNull());
  QCOMPARE(data.value("FunctionEN"), QVariant("EN: Contact 1"));
  // Try to delete article ID 1 - Must fail
  QVERIFY(!art.removeData("Article_tbl", "Id_PK", 1));
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  // Remove connection
  QVERIFY(art.removeConnection(1));
  // Check back
  dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);

  /*
   * Free article connector + connections
   */

  // Setup connector data for article ID 1
  connectorData.clear();
  QVERIFY(connectorData.setup(pvDatabaseManager.database(), false));
  connectorData.setValue("Id_PK", 1);
  connectorData.setValue("Article_Id_FK", 1);
  connectorData.setValue("Name", "X1");
  // Setup connection data
  ///connectionData.clear();
  ///QVERIFY(connectionData.setup(pvDatabaseManager.database()));
  connectionData.clearValues();
  for(int i = 0; i < 11; ++i){
    connectionData.clearValues();
    connectionData.setValue("Id_PK", i+1);
    connectionData.setValue("ArticleContactName", QString::number(i));
    connectionData.setValue("FunctionEN", "Contact " + QString::number(i));
    connectorData.addConnectionData(connectionData);
  }
  // Add connector
  QVERIFY(art.addConnector(connectorData));
  // Check back
  dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 11);
  for(int i = 0; i < dataList.size(); ++i){
    data = dataList.at(i);
    QCOMPARE(data.value("Id_PK"), QVariant(i+1));
    QCOMPARE(data.value("Article_Id_FK"), QVariant(1));
    QCOMPARE(data.value("ArticleConnector_Id_FK"), QVariant(1));
    QCOMPARE(data.value("ArticleConnectorName"), QVariant("X1"));
    QCOMPARE(data.value("ArticleContactName"), QVariant(QString::number(i)));
    QCOMPARE(data.value("FunctionEN"), QVariant("Contact " + QString::number(i)));
  }

  /*
   * Article connector based on a Connector
   */

  // Setup a Connector
  baseConnectorData.clear();
  QVERIFY(baseConnectorData.setup(pvDatabaseManager.database()));
  baseConnectorData.setValue("Id_PK", 1);
  QVERIFY(art.addRecord(baseConnectorData, "Connector_tbl"));
  // Add contacts to this connector
  record.clear();
  QVERIFY(record.addAllFields("ConnectorContact_tbl", pvDatabaseManager.database()));
  record.setValue("Id_PK", 1);
  record.setValue("Connector_Id_FK", 1);
  record.setValue("ConnectionType_Code_FK", "P");
  record.setValue("Name", "A");
  QVERIFY(art.addRecord(record, "ConnectorContact_tbl"));
  record.setValue("Id_PK", 2);
  record.setValue("Connector_Id_FK", 1);
  record.setValue("ConnectionType_Code_FK", "P");
  record.setValue("Name", "B");
  QVERIFY(art.addRecord(record, "ConnectorContact_tbl"));
  record.setValue("Id_PK", 3);
  record.setValue("Connector_Id_FK", 1);
  record.setValue("ConnectionType_Code_FK", "S");
  record.setValue("Name", "C");
  QVERIFY(art.addRecord(record, "ConnectorContact_tbl"));
  // Setup article connector data for article ID 1
  connectorData.clear();
  QVERIFY(connectorData.setup(pvDatabaseManager.database(), false));
  connectorData.setValue("Id_PK", 2);
  connectorData.setValue("Article_Id_FK", 1);
  connectorData.setValue("Connector_Id_FK", 1);
  connectorData.setValue("Name", "X2");
  // Setup connection data
  connectorContactIdList.clear();
  connectorContactIdList << 1 << 2 << 3;
  QCOMPARE(connectorData.connectionDataList().size(), 0);
  QVERIFY(art.addConnectionDataListFromConnectorContactIdList(connectorData, connectorContactIdList));
  QCOMPARE(connectorData.connectionDataList().size(), 3);
  // Add connector
  QVERIFY(art.addConnector(connectorData));
  // Check back
  dataList = art.getData("SELECT * FROM ArticleConnection_view WHERE Connector_Id_FK = 1", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 3);
  data = dataList.at(0);
  QCOMPARE(data.value("Connector_Id_FK"), QVariant(1));
  QCOMPARE(data.value("Article_Id_FK"), QVariant(1));
  QCOMPARE(data.value("ConnectionType_Code_FK"), QVariant("P"));
  QCOMPARE(data.value("ArticleConnectorName"), QVariant("X2"));
  data = dataList.at(1);
  QCOMPARE(data.value("Connector_Id_FK"), QVariant(1));
  QCOMPARE(data.value("Article_Id_FK"), QVariant(1));
  QCOMPARE(data.value("ConnectionType_Code_FK"), QVariant("P"));
  QCOMPARE(data.value("ArticleConnectorName"), QVariant("X2"));
  data = dataList.at(2);
  QCOMPARE(data.value("Connector_Id_FK"), QVariant(1));
  QCOMPARE(data.value("Article_Id_FK"), QVariant(1));
  QCOMPARE(data.value("ConnectionType_Code_FK"), QVariant("S"));
  QCOMPARE(data.value("ArticleConnectorName"), QVariant("X2"));

  /*
   * Article link + link path
   */

  // Initially, we have no article link
  dataList = art.getData("SELECT * FROM ArticleLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Add a link from CNN ID 1 to 2
  QVERIFY(art.addCableLink(1, 2, "1-2", 0.1));
  dataList = art.getData("SELECT * FROM ArticleLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  QCOMPARE(data.value("ArticleConnectionStart_Id_FK"), QVariant(1));
  QCOMPARE(data.value("ArticleConnectionEnd_Id_FK"), QVariant(2));
  QCOMPARE(data.value("LinkType_Code_FK"), QVariant("CABLELINK"));
  QCOMPARE(data.value("LinkDirection_Code_FK"), QVariant("BID"));
  QCOMPARE(data.value("Identification"), QVariant("1-2"));
  QCOMPARE(data.value("Value"), QVariant(0.1));
  QCOMPARE(data.value("ValueUnit"), QVariant("Ohm"));
  // Check that we cannot remove CNN ID 1
  QVERIFY(!art.removeConnection(1));
  dataList = art.getData("SELECT * FROM ArticleConnection_view WHERE Id_PK = 1", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  // Remove link
  QVERIFY(art.removeLink(1, 2));
  dataList = art.getData("SELECT * FROM ArticleLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);

  /*
   * Remove data
   */

  // Remove connector ID 1
  QVERIFY(art.removeConnector(1));
  dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 3);
  dataList = art.getData("SELECT * FROM ArticleConnector_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  // Remove connector ID 2
  QVERIFY(art.removeConnector(2));
  dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  dataList = art.getData("SELECT * FROM ArticleConnector_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Remove article
  QVERIFY(art.removeData("Article_tbl", "Id_PK", 1));
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QVERIFY(art.removeData("Article_tbl", "Id_PK", 2));
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Remove base connector
  QVERIFY(art.removeData("ConnectorContact_tbl", "Id_PK", 1));
  QVERIFY(art.removeData("ConnectorContact_tbl", "Id_PK", 2));
  QVERIFY(art.removeData("ConnectorContact_tbl", "Id_PK", 3));
  QVERIFY(art.removeData("Connector_tbl", "Id_PK", 1));
  dataList = art.getData("SELECT * FROM Connector_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);

  /*
   * Check helper methods
   */
  createTestArticles();
  createTestConnectors();
  createTestArticleConnections();
  createTestArticleLinks();
  createTestArticleConnectors();
  removeTestArticleLinks();
  removeTestArticleConnections();
  removeTestArticleConnectors();
  removeTestConnectors();
  removeTestArticles();
}

void mdtCableListTest::unitConnectionDataTest()
{
  mdtClUnitConnectionData connectionData;
  mdtClArticleConnectionData articleConnectionData;

  // Setup and initial flags
  QVERIFY(connectionData.setup(pvDatabaseManager.database(), true));
  QVERIFY(!connectionData.isBasedOnArticleConnection());

  // Set a free unit connection
  connectionData.setValue("Id_PK", 1);
  connectionData.setValue("Unit_Id_FK", 10);
  connectionData.setValue("UnitContactName", "Contact 1");
  QVERIFY(!connectionData.isBasedOnArticleConnection());
  QCOMPARE(connectionData.value("Id_PK"), QVariant(1));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(10));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Contact 1"));

  // Set a article based unit connection
  connectionData.setValue("Id_PK", 2);
  connectionData.setValue("Unit_Id_FK", 20);
  connectionData.setValue("ArticleConnection_Id_FK", 200);
  connectionData.setValue("UnitContactName", "Contact 2");
  QVERIFY(connectionData.isBasedOnArticleConnection());
  QCOMPARE(connectionData.value("Id_PK"), QVariant(2));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(20));
  QCOMPARE(connectionData.value("ArticleConnection_Id_FK"), QVariant(200));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Contact 2"));

  // Clear
  connectionData.clearValues();
  QVERIFY(!connectionData.isBasedOnArticleConnection());

  // Setup a artice connection data and set it to unit connection data
  QVERIFY(articleConnectionData.setup(pvDatabaseManager.database()));
  articleConnectionData.setValue("Id_PK", 100);
  ///articleConnectionData.setValue("ConnectionType_Code_FK", "P");
  connectionData.setArticleConnectionData(articleConnectionData);
  QVERIFY(connectionData.isBasedOnArticleConnection());
  QCOMPARE(connectionData.value("ArticleConnection_Id_FK"), QVariant(100));
  ///QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("P"));
}

void mdtCableListTest::unitConnectorDataTest()
{
  mdtClUnitConnectorData data;
  mdtClUnitConnectionData connectionData;
  mdtClArticleConnectorData articleConnectorData;
  mdtClArticleConnectionData articleConnectionData;
  mdtClConnectorData connectorData;
  bool ok;

  // Setup and initial flags
  QVERIFY(data.setup(pvDatabaseManager.database(), true, true));
  QVERIFY(!data.isBasedOnArticleConnector());
  QVERIFY(!data.isBasedOnConnector());
  QCOMPARE(data.connectionDataList().size(), 0);

  // Set some data and check
  data.setValue("Id_PK", 1);
  data.setValue("Unit_Id_FK", 10);
  QVERIFY(!data.isBasedOnArticleConnector());
  QVERIFY(!data.isBasedOnConnector());
  QCOMPARE(data.value("Id_PK"), QVariant(1));
  QCOMPARE(data.value("Unit_Id_FK"), QVariant(10));
  QCOMPARE(data.connectionDataList().size(), 0);

  // Setup connection data and add
  QVERIFY(connectionData.setup(pvDatabaseManager.database(), true));
  connectionData.setValue("Id_PK", 100);
  data.addConnectionData(connectionData);
  QCOMPARE(data.connectionDataList().size(), 1);
  connectionData.clear();
  QCOMPARE(connectionData.value("Id_PK"), QVariant());
  connectionData = data.connectionData(100, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(100));

  // Clear values
  QVERIFY(data.connectionDataList().size() > 0);
  data.clearValues();
  QVERIFY(!data.isBasedOnArticleConnector());
  QVERIFY(!data.isBasedOnConnector());
  QCOMPARE(data.connectionDataList().size(), 0);

  /*
   * Setup a connector and set it as base of unit connector
   */
  // Clear data
  data.clearValues();
  QVERIFY(!data.isBasedOnArticleConnector());
  QVERIFY(!data.isBasedOnConnector());
  // Setup connector
  QVERIFY(connectorData.setup(pvDatabaseManager.database()));
  connectorData.setValue("Id_PK", 1000);
  // Set connector to unit connector data
  data.setConnectorData(connectorData);
  QVERIFY(!data.isBasedOnArticleConnector());
  QVERIFY(data.isBasedOnConnector());
  connectorData.clear();
  QCOMPARE(connectorData.value("Id_PK"), QVariant());
  connectorData = data.connectorData();
  QCOMPARE(connectorData.value("Id_PK"), QVariant(1000));

  /*
   * Setup a free article connector and set it as base of unit connector
   * Check that unit connector is no longer based on connector (set previously)
   */
  // Setup free artice connector
  QVERIFY(articleConnectorData.setup(pvDatabaseManager.database(), true));
  articleConnectorData.setValue("Id_PK", 1212);
  // Set artice connector to data
  data.setArticleConnectorData(articleConnectorData);
  QVERIFY(data.isBasedOnArticleConnector());
  QVERIFY(!data.isBasedOnConnector());
  QCOMPARE(data.value("Connector_Id_FK"), QVariant(QVariant::Int));
  QCOMPARE(data.value("ArticleConnector_Id_FK"), QVariant(1212));
  QCOMPARE(data.articleConnectorData().value("Id_PK"), QVariant(1212));

  /*
   * Setup a article connector based on a connector and set it as base of unit connector
   * Check that unit connector is based on the correct artice connector, and the connector on witch artice connector is based
   */
  // Setup connector
  connectorData.clearValues();
  connectorData.setValue("Id_PK", 5656);
  // Setup artice connector
  articleConnectorData.clearValues();
  articleConnectorData.setValue("Id_PK", 4444);
  articleConnectorData.setConnectorData(connectorData);
  // Set artice connector to data
  data.setArticleConnectorData(articleConnectorData);
  QVERIFY(data.isBasedOnArticleConnector());
  QVERIFY(data.isBasedOnConnector());
  QCOMPARE(data.value("Connector_Id_FK"), QVariant(5656));
  QCOMPARE(data.value("ArticleConnector_Id_FK"), QVariant(4444));
  QCOMPARE(data.articleConnectorData().value("Id_PK"), QVariant(4444));

  // Clear values
  QVERIFY(connectionData.setup(pvDatabaseManager.database(), true));
  data.addConnectionData(connectionData);
  QVERIFY(data.connectionDataList().size() > 0);
  data.clearValues();
  QVERIFY(!data.isBasedOnArticleConnector());
  QVERIFY(!data.isBasedOnConnector());
  QCOMPARE(data.value("Id_PK"), QVariant(QVariant::Int));
  QCOMPARE(data.value("Unit_Id_FK"), QVariant(QVariant::Int));
  QCOMPARE(data.value("Connector_Id_FK"), QVariant(QVariant::Int));
  QCOMPARE(data.value("ArticleConnector_Id_FK"), QVariant(QVariant::Int));
  QCOMPARE(data.connectionDataList().size(), 0);

  // Clear
  QVERIFY(connectionData.setup(pvDatabaseManager.database(), true));
  data.addConnectionData(connectionData);
  QVERIFY(data.connectionDataList().size() > 0);
  data.clear();
  QVERIFY(!data.isBasedOnArticleConnector());
  QVERIFY(!data.isBasedOnConnector());
  QCOMPARE(data.value("Id_PK"), QVariant());
  QCOMPARE(data.value("Unit_Id_FK"), QVariant());
  QCOMPARE(data.value("Connector_Id_FK"), QVariant());
  QCOMPARE(data.value("ArticleConnector_Id_FK"), QVariant());
  QCOMPARE(data.connectionDataList().size(), 0);
}

void mdtCableListTest::unitTest()
{
  mdtClUnit unit(0, pvDatabaseManager.database());
  mdtClUnitConnectorData connectorData;

  QVERIFY(connectorData.setup(pvDatabaseManager.database(), true, true));
  // Create base structure
  createTestVehicleTypes();
  createTestArticles();
  createTestConnectors();
  createTestArticleConnections();
  createTestArticleLinks();
  createTestArticleConnectors();
  createTestUnits();
  createTestVehicleTypeUnitAssignations();
  createTestUnitConnections();

  /*
   * Try to add a unit connector that is based on a article connector
   *  and a base connector that not matches artice connector's base connector - Must fail
   */
  connectorData.clearValues();
  connectorData.setValue("Unit_Id_FK", 2000);
  connectorData.setValue("Connector_Id_FK", 2);
  connectorData.setValue("ArticleConnector_Id_FK", 200);
  QVERIFY(!unit.addConnector(connectorData));
  createTestUnitConnectors();

  // Remove base structure
  removeTestUnitConnections();
  removeTestUnitConnectors();
  removeTestVehicleTypeUnitAssignations();
  removeTestUnits();
  removeTestArticleLinks();
  removeTestArticleConnections();
  removeTestArticleConnectors();
  removeTestConnectors();
  removeTestArticles();
  removeTestVehicleTypes();
}

void mdtCableListTest::unitConnectionTest()
{
  mdtClUnit unit(0, pvDatabaseManager.database());
  mdtClLink lnk(0, pvDatabaseManager.database());
  mdtClUnitConnectionData connectionData;
  bool ok;

  // Create base structure
  createTestVehicleTypes();
  createTestArticles();
  createTestConnectors();
  createTestArticleConnections();
  createTestArticleLinks();
  createTestArticleConnectors();
  createTestUnits();
  createTestVehicleTypeUnitAssignations();
  createTestUnitConnections();

  /*
   * Check basic connection edition
   *  - Change unit connection 10000 : set UnitContactName : Edited contact
   */
  connectionData = unit.getConnectionData(10000, true, &ok);
  QVERIFY(ok);
  connectionData.setValue("UnitContactName", "Edited contact");
  QVERIFY(unit.editConnection(10000, connectionData));
  connectionData.clearValues();
  connectionData = unit.getConnectionData(10000, true, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Edited contact"));
  // Edit back to original values
  connectionData.setValue("UnitContactName", "Unit contact 10000");
  QVERIFY(unit.editConnection(10000, connectionData));
  connectionData.clearValues();
  connectionData = unit.getConnectionData(10000, true, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 10000"));

  // Remove base structure
  removeTestUnitConnections();
  removeTestVehicleTypeUnitAssignations();
  removeTestUnits();
  removeTestArticleLinks();
  removeTestArticleConnections();
  removeTestArticleConnectors();
  removeTestConnectors();
  removeTestArticles();
  removeTestVehicleTypes();
}

void mdtCableListTest::mdtClLinkDataTest()
{
  mdtClLinkData linkData;
  mdtClUnitConnectionData startConnectionData, endConnectionData;
  mdtClVehicleTypeLinkData vtLinkData;

  QVERIFY(linkData.setup(pvDatabaseManager.database()));
  QVERIFY(startConnectionData.setup(pvDatabaseManager.database(), true));
  QVERIFY(endConnectionData.setup(pvDatabaseManager.database(), true));

  // Setup connections data
  startConnectionData.setValue("Id_PK", 1);
  endConnectionData.setValue("Id_PK", 2);
  // Setup vehicle type link data
  vtLinkData.setVehicleTypeStartId(10);
  vtLinkData.setVehicleTypeEndId(20);
  // Set link data
  linkData.setConnectionData(startConnectionData, endConnectionData);
  linkData.setValue("Identification", "Link 12");
  linkData.addVehicleTypeLinkData(vtLinkData);
  // Check
  QCOMPARE(linkData.value("Identification"), QVariant("Link 12"));
  QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(1));
  QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(1));
  QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(2));
  QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(2));
  QCOMPARE(linkData.value("ArticleConnectionStart_Id_FK"), QVariant(QVariant::Int));
  QCOMPARE(linkData.value("ArticleConnectionStart_Id_FK"), QVariant(QVariant::Int));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId() , QVariant(10));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId() , QVariant(20));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId() , QVariant(1));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId() , QVariant(2));
  // Change start/end connection data and check
  startConnectionData.setValue("Id_PK", 3);
  endConnectionData.setValue("Id_PK", 4);
  linkData.setConnectionData(startConnectionData, endConnectionData);
  QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(3));
  QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(4));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId() , QVariant(3));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId() , QVariant(4));
  // Clear
  linkData.clearValues();
  QCOMPARE(linkData.value("Identification"), QVariant(QVariant::String));
  QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(QVariant::Int));
  QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(QVariant::Int));
  QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(QVariant::Int));
  QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(QVariant::Int));
  QCOMPARE(linkData.value("ArticleConnectionStart_Id_FK"), QVariant(QVariant::Int));
  QCOMPARE(linkData.value("ArticleConnectionStart_Id_FK"), QVariant(QVariant::Int));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 0);
}

void mdtCableListTest::linkTest()
{
  mdtClLink lnk(0, pvDatabaseManager.database());
  mdtClUnit unit(0, pvDatabaseManager.database());
  mdtClUnitVehicleType uvt(0, pvDatabaseManager.database());
  QList<QVariant> vtStartIdList, vtEndIdList;
  mdtClLinkData linkData;
  mdtClUnitConnectionData connectionData;
  ///QList<QSqlRecord> dataList;
  QList<mdtClVehicleTypeLinkData> vtLinkDataList;
  QString sql;
  bool ok;

  QVERIFY(linkData.setup(pvDatabaseManager.database()));

  /*
   * Check vehicle type link data list
   */

  // Add 1 start vehicle - Case Error (one list empty)
  vtStartIdList.append(10);
  QVERIFY(!lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 0);
  // Add 1 end vehicle - Case 1 start -> 1 end
  vtEndIdList.append(20);
  QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(10));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(20));
  // Add a second start vehicle - Case 2 start -> 1 end
  vtStartIdList.append(11);
  QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 2);
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(10));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(20));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(1).vehicleTypeStartId(), QVariant(11));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(1).vehicleTypeEndId(), QVariant(20));
  // Add a second end vehicle - Case 2*(1 start -> 1 end)
  vtEndIdList.append(21);
  QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 2);
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(10));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(20));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(1).vehicleTypeStartId(), QVariant(11));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(1).vehicleTypeEndId(), QVariant(21));
  // Clear start list - Case Error (one list empty)
  vtStartIdList.clear();
  QVERIFY(!lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 0);
  // Add 1 start vehicle - Case 1 start -> 2 end
  vtStartIdList.append(12);
  QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 2);
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(12));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(20));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(1).vehicleTypeStartId(), QVariant(12));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(1).vehicleTypeEndId(), QVariant(21));

  /// \todo suite
  // Create base structure
  pvScenario->createSenario();
  // Get link data for a non existing link, ok must be false
  linkData = lnk.getLinkData(135, 45687, true, true, &ok);
  QVERIFY(!ok);
  /*
   * Simple link edition
   *
   *  Edit link 10000<->10001:
   *   -> Identification : Edited link
   */
  linkData = lnk.getLinkData(10000, 10001, true, true, &ok);
  QVERIFY(ok);
  linkData.setValue("Identification", "Edited link");
  QVERIFY(lnk.editLink(10000, 10001, linkData));
  linkData = lnk.getLinkData(10000, 10001, true, true, &ok);
  QVERIFY(ok);
  QCOMPARE(linkData.value("Identification"), QVariant("Edited link"));
  QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(10000));
  QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(10001));
  QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(10000));
  QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(10001));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(1));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId(), QVariant(10000));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId(), QVariant(10001));
  // Restore original data
  linkData.setValue("Identification", "Link 10000<->10001");
  QVERIFY(lnk.editLink(10000, 10001, linkData));
  linkData = lnk.getLinkData(10000, 10001, true, true, &ok);
  QVERIFY(ok);
  QCOMPARE(linkData.value("Identification"), QVariant("Link 10000<->10001"));
  QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(10000));
  QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(10001));
  QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(10000));
  QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(10001));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(1));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId(), QVariant(10000));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId(), QVariant(10001));
  /*
   * Link edition with start/end change
   *
   *  Edit link 10000<->10001:
   *   -> UnitConnectionStart_Id_FK : 10000 , UnitConnectionEnd_Id_FK : 20000 , Identification : Edited link
   */
  QVERIFY(!lnk.linkExists(10000, 20000, &ok));
  QVERIFY(ok);
  linkData = lnk.getLinkData(10000, 10001, true, true, &ok);
  QVERIFY(ok);
  // Update end connection data
  connectionData = unit.getConnectionData(20000, false, &ok);
  QVERIFY(ok);
  linkData.setEndConnectionData(connectionData);
  // Update end vehicle type list
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
  vtStartIdList.clear();
  vtStartIdList.append(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId());
  vtEndIdList.clear();
  vtEndIdList.append(2);
  QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
  // Edit link and check
  QVERIFY(lnk.editLink(10000, 10001, linkData));
  QVERIFY(lnk.linkExists(10000, 20000, &ok));
  QVERIFY(ok);
  linkData = lnk.getLinkData(10000, 20000, true, true, &ok);
  QVERIFY(ok);
  QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(10000));
  QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(20000));
  QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(10000));
  QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(20000));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(2));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId(), QVariant(10000));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId(), QVariant(20000));
  /*
   * Link edition with start/end change (restore to original value)
   *
   *  Edit link 10000<->20000:
   *   -> UnitConnectionStart_Id_FK : 10000 , UnitConnectionEnd_Id_FK : 10001 , Identification : Edited link
   */
  QVERIFY(!lnk.linkExists(10000, 10001, &ok));
  QVERIFY(ok);
  linkData = lnk.getLinkData(10000, 20000, true, true, &ok);
  QVERIFY(ok);
  // Update end connection data
  connectionData = unit.getConnectionData(10001, false, &ok);
  QVERIFY(ok);
  linkData.setEndConnectionData(connectionData);
  // Update end vehicle type list
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
  vtStartIdList.clear();
  vtStartIdList.append(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId());
  vtEndIdList.clear();
  vtEndIdList.append(1);
  QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
  // Edit link and check
  QVERIFY(lnk.editLink(10000, 20000, linkData));
  QVERIFY(lnk.linkExists(10000, 10001, &ok));
  QVERIFY(ok);
  linkData = lnk.getLinkData(10000, 10001, true, true, &ok);
  QVERIFY(ok);
  QCOMPARE(linkData.value("Identification"), QVariant("Link 10000<->10001"));
  QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(10000));
  QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(10001));
  QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(10000));
  QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(10001));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(1));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId(), QVariant(10000));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId(), QVariant(10001));
  /*
   * Remove unit - vehicle type assignation and check that vehicle type links are removed
   *
   *  Remove Unit_Id_FK : 1000 , VehicleType_Id_FK : 1
   */
  // Check that we have expected vehicle type links: 10000-10001 , 10001-20000 , 30005-40005 , 40005-50005
  vtLinkDataList = lnk.getVehicleTypeLinkDataByUnitId(1000, &ok);
  QVERIFY(ok);
  QCOMPARE(vtLinkDataList.size(), 4);
  QCOMPARE(vtLinkDataList.at(0).unitConnectionStartId(), QVariant(10000));
  QCOMPARE(vtLinkDataList.at(0).unitConnectionEndId(), QVariant(10001));
  QCOMPARE(vtLinkDataList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtLinkDataList.at(0).vehicleTypeEndId(), QVariant(1));

  QCOMPARE(vtLinkDataList.at(1).unitConnectionStartId(), QVariant(30005));
  QCOMPARE(vtLinkDataList.at(1).unitConnectionEndId(), QVariant(40005));
  QCOMPARE(vtLinkDataList.at(1).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtLinkDataList.at(1).vehicleTypeEndId(), QVariant(1));

  QCOMPARE(vtLinkDataList.at(3).unitConnectionStartId(), QVariant(10001));
  QCOMPARE(vtLinkDataList.at(3).unitConnectionEndId(), QVariant(20000));
  QCOMPARE(vtLinkDataList.at(3).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtLinkDataList.at(3).vehicleTypeEndId(), QVariant(2));

  QCOMPARE(vtLinkDataList.at(2).unitConnectionStartId(), QVariant(40005));
  QCOMPARE(vtLinkDataList.at(2).unitConnectionEndId(), QVariant(50005));
  QCOMPARE(vtLinkDataList.at(2).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtLinkDataList.at(2).vehicleTypeEndId(), QVariant(1));

  // Remove assignation
  QVERIFY(uvt.removeUnitVehicleAssignment(1000, 1));
  // Check that links no longer exists
  vtLinkDataList = lnk.getVehicleTypeLinkDataByUnitId(1000, &ok);
  QVERIFY(ok);
  QCOMPARE(vtLinkDataList.size(), 0);
  
  /// \todo Units with more than 1 vehicle type association - 1-1
  
  /// \todo Units with more than 1 vehicle type association - 1-n
  
  /// \todo Links with U CNN based on ART CNN


  // Cleanup
  pvScenario->removeScenario();
}

void mdtCableListTest::linkAutoConnectionTest()
{
  mdtClLink lnk(0, pvDatabaseManager.database());
  QList<mdtClLinkData> cnnLinkDataList;
  mdtClLinkData cnnLinkData;
  mdtClUnitConnectionData a, b;
  QList<mdtClUnitConnectionData> A, B;
  QList<QVariant> vtStartIdList, vtEndIdList;
  bool ok;

  // Setup connection data
  QVERIFY(a.setup(pvDatabaseManager.database(), false));
  QVERIFY(b.setup(pvDatabaseManager.database(), false));

  /*
   * Case 1 :
   *  - A and B contains same amount of connections
   *  - A contains only pins
   *  - B contains only sockets
   */
  A.clear();
  B.clear();
  // Create A list
  a.clearValues();
  a.setValue("Id_PK", 1);
  a.setValue("UnitContactName", "1");
  a.setValue("ConnectionType_Code_FK", "P");
  A  << a;
  a.clearValues();
  a.setValue("Id_PK", 2);
  a.setValue("UnitContactName", "2");
  a.setValue("ConnectionType_Code_FK", "P");
  A  << a;
  a.clearValues();
  a.setValue("Id_PK", 3);
  a.setValue("UnitContactName", "3");
  a.setValue("ConnectionType_Code_FK", "P");
  A  << a;
  // Create B list
  b.clearValues();
  b.setValue("Id_PK", 12);
  b.setValue("UnitContactName", "2");
  b.setValue("ConnectionType_Code_FK", "S");
  B << b;
  b.clearValues();
  b.setValue("Id_PK", 11);
  b.setValue("UnitContactName", "1");
  b.setValue("ConnectionType_Code_FK", "S");
  B << b;
  b.clearValues();
  b.setValue("Id_PK", 13);
  b.setValue("UnitContactName", "3");
  b.setValue("ConnectionType_Code_FK", "S");
  B << b;
  // Build link data list and check
  cnnLinkDataList = lnk.getConnectionLinkListByName(A, B);
  QCOMPARE(cnnLinkDataList.size(), 3);
  // Check base data
  QCOMPARE(cnnLinkDataList.at(0).value("LinkType_Code_FK"), QVariant("CONNECTION"));
  QCOMPARE(cnnLinkDataList.at(0).value("LinkDirection_Code_FK"), QVariant("BID"));
  QCOMPARE(cnnLinkDataList.at(1).value("LinkType_Code_FK"), QVariant("CONNECTION"));
  QCOMPARE(cnnLinkDataList.at(1).value("LinkDirection_Code_FK"), QVariant("BID"));
  QCOMPARE(cnnLinkDataList.at(2).value("LinkType_Code_FK"), QVariant("CONNECTION"));
  QCOMPARE(cnnLinkDataList.at(2).value("LinkDirection_Code_FK"), QVariant("BID"));
  // Check connection - A and B have same size, so order is like A
  a = cnnLinkDataList.at(0).startConnectionData();
  b = cnnLinkDataList.at(0).endConnectionData();
  QCOMPARE(a.value("UnitContactName"), b.value("UnitContactName"));
  a = cnnLinkDataList.at(1).startConnectionData();
  b = cnnLinkDataList.at(1).endConnectionData();
  QCOMPARE(a.value("UnitContactName"), b.value("UnitContactName"));
  a = cnnLinkDataList.at(2).startConnectionData();
  b = cnnLinkDataList.at(2).endConnectionData();
  QCOMPARE(a.value("UnitContactName"), b.value("UnitContactName"));
  /*
   * Case 2 :
   *  - A < B
   *  - A contains only pins
   *  - B contains only sockets
   */
  A.clear();
  B.clear();
  // Create A list
  a.clearValues();
  a.setValue("Id_PK", 1);
  a.setValue("UnitContactName", "1");
  a.setValue("ConnectionType_Code_FK", "P");
  A  << a;
  // Create B list
  b.clearValues();
  b.setValue("Id_PK", 11);
  b.setValue("UnitContactName", "1");
  b.setValue("ConnectionType_Code_FK", "S");
  B << b;
  b.clearValues();
  b.setValue("Id_PK", 12);
  b.setValue("UnitContactName", "2");
  b.setValue("ConnectionType_Code_FK", "S");
  B << b;
  // Build link data list and check
  cnnLinkDataList = lnk.getConnectionLinkListByName(A, B);
  QCOMPARE(cnnLinkDataList.size(), 1);
  // Check base data
  QCOMPARE(cnnLinkDataList.at(0).value("LinkType_Code_FK"), QVariant("CONNECTION"));
  QCOMPARE(cnnLinkDataList.at(0).value("LinkDirection_Code_FK"), QVariant("BID"));
  // Check connection - A < B , so order is like A
  a = cnnLinkDataList.at(0).startConnectionData();
  b = cnnLinkDataList.at(0).endConnectionData();
  QCOMPARE(a.value("UnitContactName"), b.value("UnitContactName"));
  /*
   * Case 3 :
   *  - A > B
   *  - A contains only pins
   *  - B contains only sockets
   */
  A.clear();
  B.clear();
  // Create A list
  a.clearValues();
  a.setValue("Id_PK", 1);
  a.setValue("UnitContactName", "1");
  a.setValue("ConnectionType_Code_FK", "P");
  A  << a;
  a.clearValues();
  a.setValue("Id_PK", 2);
  a.setValue("UnitContactName", "2");
  a.setValue("ConnectionType_Code_FK", "P");
  A  << a;
  // Create B list
  b.clearValues();
  b.setValue("Id_PK", 12);
  b.setValue("UnitContactName", "2");
  b.setValue("ConnectionType_Code_FK", "S");
  B << b;
  // Build link data list and check
  cnnLinkDataList = lnk.getConnectionLinkListByName(A, B);
  QCOMPARE(cnnLinkDataList.size(), 1);
  // Check base data
  QCOMPARE(cnnLinkDataList.at(0).value("LinkType_Code_FK"), QVariant("CONNECTION"));
  QCOMPARE(cnnLinkDataList.at(0).value("LinkDirection_Code_FK"), QVariant("BID"));
  // Check connection
  a = cnnLinkDataList.at(0).startConnectionData();
  b = cnnLinkDataList.at(0).endConnectionData();
  QCOMPARE(a.value("UnitContactName"), b.value("UnitContactName"));

  /// \todo Combinaisons with non compatible P-S , with T, etc...

  // Create base structure for next tests
  createTestVehicleTypes();
  createTestArticles();
  createTestConnectors();
  createTestArticleConnections();
  createTestArticleLinks();
  createTestArticleConnectors();
  createTestUnits();
  createTestVehicleTypeUnitAssignations();
  createTestUnitConnectors();

  /*
   * Check connect method with following connectors:
   *
   *  - START connector: Id_PK : 400000 , Unit_Id_FK : 1000 , Connector_Id_FK : NULL , ArticleConnector_Id_FK : NULL , Name : Unit connector 400000
   *   -> Connection: Id_PK 40005 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : P , Name : A
   *   -> Connection: Id_PK 40006 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : B
   *
   *  - END connector: Id_PK : 500000 , Unit_Id_FK : 2000 , Connector_Id_FK : NULL , ArticleConnector_Id_FK : NULL , Name : Unit connector 500000
   *   -> Connection: Id_PK 50005 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : A
   *   -> Connection: Id_PK 50006 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : B
   *
   * For vehicle types, we take:
   *
   *  - START: Id_PK : 1
   *  - END : Id_PK : 2
   */
  // Create connection (i.e. links)
  QVERIFY(lnk.connectByContactName(400000, 500000, 1, 2));
  /*
   * Check link data
   */
  // Get link from connection 40005 -> 50005 : must exist
  QVERIFY(lnk.linkExists(40005, 50005, &ok));
  QVERIFY(ok);
  cnnLinkData = lnk.getLinkData(40005, 50005, true, true, &ok);
  QVERIFY(ok);
  QCOMPARE(cnnLinkData.value("LinkType_Code_FK"), QVariant("CONNECTION"));
  QCOMPARE(cnnLinkData.value("LinkDirection_Code_FK"), QVariant("BID"));
  // Check that link from 40006 -> 50006 not exists
  QVERIFY(!lnk.linkExists(40006, 50006, &ok));
  QVERIFY(ok);


  /*
   * Check disconnect method
   */
  vtStartIdList.clear();
  vtEndIdList.clear();
  vtStartIdList << 1;
  vtEndIdList << 2;
  QVERIFY(lnk.disconnectConnectors(400000, 500000, vtStartIdList, vtEndIdList));
  QVERIFY(!lnk.linkExists(40005, 50005, &ok));

  // Remove base structure
  removeTestUnitConnectors();
  removeTestVehicleTypeUnitAssignations();
  removeTestUnits();
  removeTestArticleLinks();
  removeTestArticleConnections();
  removeTestArticleConnectors();
  removeTestConnectors();
  removeTestArticles();
  removeTestVehicleTypes();
}


void mdtCableListTest::pathGraphTest()
{
  ///QFAIL("Test not implemented yet");
  
  mdtClPathGraph graph(pvDatabaseManager.database());
  QList<QVariant> idList;
  ///QVariant data;
  bool ok;

  // Initial
  idList = graph.getLinkedConnectionIdList(0);
  QVERIFY(idList.isEmpty());
  idList = graph.getShortestPath(0, 1, ok);
  QVERIFY(!ok);
  QVERIFY(idList.isEmpty());
  QVERIFY(graph.getUserData(0, 1).isNull());
  QVERIFY(!graph.connectionsAreLinked(0, 1));

  /*
   * Testst with free data
   */
  // (1)-(2)
  graph.addLink(1, 2);
  idList = graph.getLinkedConnectionIdList(1);
  QCOMPARE(idList.size(), 1);
  QCOMPARE(idList.at(0), QVariant(2));
  idList = graph.getShortestPath(1, 2, ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 2);
  QCOMPARE(idList.at(0), QVariant(1));
  QCOMPARE(idList.at(1), QVariant(2));
  QVERIFY(graph.getUserData(1, 2).isNull());
  QVERIFY(graph.connectionsAreLinked(1, 2));
  QVERIFY(graph.connectionsAreLinked(2, 1));
  // (1)-(2)-(3)
  graph.addLink(2, 3, "2-3");
  idList = graph.getLinkedConnectionIdList(1);
  QCOMPARE(idList.size(), 2);
  QCOMPARE(idList.at(0), QVariant(2));
  QCOMPARE(idList.at(1), QVariant(3));
  idList = graph.getShortestPath(1, 3, ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 3);
  QCOMPARE(idList.at(0), QVariant(1));
  QCOMPARE(idList.at(1), QVariant(2));
  QCOMPARE(idList.at(2), QVariant(3));
  QCOMPARE(graph.getUserData(2, 3), QVariant("2-3"));
  QCOMPARE(graph.getUserData(3, 2), QVariant("2-3"));
  QVERIFY(graph.connectionsAreLinked(1, 2));
  QVERIFY(graph.connectionsAreLinked(1, 3));
  QVERIFY(graph.connectionsAreLinked(3, 2));
  QVERIFY(graph.connectionsAreLinked(2, 3));
  QVERIFY(graph.connectionsAreLinked(3, 1));
  // (1)-(2)-(3)-(4)
  graph.addLink(3, 4);
  idList = graph.getLinkedConnectionIdList(1);
  QCOMPARE(idList.size(), 3);
  QCOMPARE(idList.at(0), QVariant(2));
  QCOMPARE(idList.at(1), QVariant(3));
  QCOMPARE(idList.at(2), QVariant(4));
  idList = graph.getShortestPath(1, 4, ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 4);
  QCOMPARE(idList.at(0), QVariant(1));
  QCOMPARE(idList.at(1), QVariant(2));
  QCOMPARE(idList.at(2), QVariant(3));
  QCOMPARE(idList.at(3), QVariant(4));
  QVERIFY(graph.connectionsAreLinked(1, 4));
  // (1)-(2)-(3)-(4)     (10)-(11)
  graph.addLink(10, 11);
  idList = graph.getLinkedConnectionIdList(1);
  QCOMPARE(idList.size(), 3);
  QCOMPARE(idList.at(0), QVariant(2));
  QCOMPARE(idList.at(1), QVariant(3));
  QCOMPARE(idList.at(2), QVariant(4));
  idList = graph.getLinkedConnectionIdList(10);
  QCOMPARE(idList.size(), 1);
  QCOMPARE(idList.at(0), QVariant(11));
  idList = graph.getShortestPath(1, 4, ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 4);
  QCOMPARE(idList.at(0), QVariant(1));
  QCOMPARE(idList.at(1), QVariant(2));
  QCOMPARE(idList.at(2), QVariant(3));
  QCOMPARE(idList.at(3), QVariant(4));
  idList = graph.getShortestPath(10, 11, ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 2);
  QCOMPARE(idList.at(0), QVariant(10));
  QCOMPARE(idList.at(1), QVariant(11));
  idList = graph.getShortestPath(1, 11, ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 0);
  // (1)-(2)-(3)-(4)     (10)-(11)
  //   \---(5)---/
  graph.addLink(1, 5, 15);
  graph.addLink(5, 4);
  idList = graph.getLinkedConnectionIdList(1);
  QCOMPARE(idList.size(), 4);
  QVERIFY(idList.contains(2));
  QVERIFY(idList.contains(3));
  QVERIFY(idList.contains(4));
  QVERIFY(idList.contains(5));
  idList = graph.getShortestPath(1, 4, ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 3);
  QCOMPARE(idList.at(0), QVariant(1));
  QCOMPARE(idList.at(1), QVariant(5));
  QCOMPARE(idList.at(2), QVariant(4));
  QCOMPARE(graph.getUserData(1, 5), QVariant(15));
  QCOMPARE(graph.getUserData(5, 1), QVariant(15));
  QVERIFY(graph.connectionsAreLinked(1, 4));
  QVERIFY(graph.connectionsAreLinked(1, 5));
  QVERIFY(graph.connectionsAreLinked(5, 4));
  // Clear data that we added and check
  graph.removeAddedLinks();
  idList = graph.getLinkedConnectionIdList(1);
  QCOMPARE(idList.size(), 0);

  /*
   * Tests with database data
   */
  // Scenario
  pvScenario->createSenario();
  // Load link list
  QVERIFY(graph.loadLinkList());
  // Check that manually added links was cleared
  idList = graph.getLinkedConnectionIdList(1);
  QCOMPARE(idList.size(), 0);
  // Check linked connections
  idList = graph.getLinkedConnectionIdList(10000);
  QCOMPARE(idList.size(), 3);
  QVERIFY(idList.contains(10001));
  QVERIFY(idList.contains(20000));
  QVERIFY(idList.contains(20001));
  // Check linked connectors
  idList = graph.getLinkedConnectorIdList(300000, &ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 2);
  QVERIFY(idList.contains(400000));
  QVERIFY(idList.contains(500000));
  // Check path from 10000 to 20001
  idList = graph.getShortestPath(10000, 20001, ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 4);
  QCOMPARE(idList.at(0), QVariant(10000));
  QCOMPARE(idList.at(1), QVariant(10001));
  QCOMPARE(idList.at(2), QVariant(20000));
  QCOMPARE(idList.at(3), QVariant(20001));
  /*
   * Call method to remove manually added links - Must change nothing
   */
  // Check linked connections
  idList = graph.getLinkedConnectionIdList(10000);
  QCOMPARE(idList.size(), 3);
  QVERIFY(idList.contains(10001));
  QVERIFY(idList.contains(20000));
  QVERIFY(idList.contains(20001));
  // Check path from 10000 to 20001
  idList = graph.getShortestPath(10000, 20001, ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 4);
  QCOMPARE(idList.at(0), QVariant(10000));
  QCOMPARE(idList.at(1), QVariant(10001));
  QCOMPARE(idList.at(2), QVariant(20000));
  QCOMPARE(idList.at(3), QVariant(20001));

  pvScenario->removeScenario();
}

void mdtCableListTest::directLinkTest()
{
  mdtClDirectLink *dlnk;
  mdtClPathGraph graph(pvDatabaseManager.database());
  QSqlRecord record;
  QList<QSqlRecord> dataList;
  QList<QVariant> idList;
  QList<QVariant> expectedIdList;
  QList<QVariant> allConnectionsIdList;
  bool ok;
  int i;

  // Scenario
  pvScenario->createSenario();
  // Load link list
  QVERIFY(graph.loadLinkList());
  // Initial
  dlnk = new mdtClDirectLink(0, pvDatabaseManager.database());
  QVERIFY(pvDatabaseManager.database().tables().contains("DirectLink_tbl"));
  record = pvDatabaseManager.database().record("DirectLink_tbl");
  QVERIFY(record.contains("UnitConnectionStart_Id_FK"));
  QVERIFY(record.contains("UnitConnectionEnd_Id_FK"));
  // Get unit connections for next tests
  dataList = dlnk->getData("SELECT Id_PK FROM UnitConnection_tbl", &ok);
  QVERIFY(ok);
  for(i = 0; i < dataList.size(); ++i){
    allConnectionsIdList.append(dataList.at(i).value(0));
  }
  // getUnitConnectionIdListPartOfUnit test - Unit ID: 1000
  expectedIdList.clear();
  expectedIdList << 10000 << 10001 << 10005 << 30005 << 30006 << 40005 << 40006;
  idList = dlnk->getUnitConnectionIdListPartOfUnit(allConnectionsIdList, 1000, &ok);
  QVERIFY(ok);
  QCOMPARE(idList, expectedIdList);
  // getUnitConnectionIdListPartOfUnitConnector test - Unit connector ID: 500000
  expectedIdList.clear();
  expectedIdList << 50005 << 50006;
  idList = dlnk->getUnitConnectionIdListPartOfUnitConnector(allConnectionsIdList, 500000, &ok);
  QVERIFY(ok);
  QCOMPARE(idList, expectedIdList);
  // addLink test
  QVERIFY(!dlnk->addLink(10000, QVariant()));
  QVERIFY(!dlnk->addLink(QVariant(), 20000));
  QVERIFY(dlnk->addLink(10000, 20000));
  dataList = dlnk->getData("SELECT * FROM DirectLink_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("UnitConnectionStart_Id_FK"), QVariant(10000));
  QCOMPARE(dataList.at(0).value("UnitConnectionEnd_Id_FK"), QVariant(20000));
  QVERIFY(dlnk->removeData("DirectLink_tbl", "UnitConnectionStart_Id_FK", 10000, "UnitConnectionEnd_Id_FK", 20000));
  dataList = dlnk->getData("SELECT * FROM DirectLink_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Check link adding between 2 connectors
  QVERIFY(dlnk->addLinksByUnitConnector(300000, 500000, &graph));
  dataList = dlnk->getData("SELECT * FROM DirectLink_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("UnitConnectionStart_Id_FK"), QVariant(30005));
  QCOMPARE(dataList.at(0).value("UnitConnectionEnd_Id_FK"), QVariant(50005));
  QCOMPARE(dataList.at(0).value("UnitConnectorStart_Id_FK"), QVariant(300000));
  QCOMPARE(dataList.at(0).value("UnitConnectorEnd_Id_FK"), QVariant(500000));


  delete dlnk;
  QVERIFY(!pvDatabaseManager.database().tables().contains("DirectLink_tbl"));
  pvScenario->removeScenario();
}

void mdtCableListTest::linkBeamTest()
{
  mdtClLinkBeam lb(0, pvDatabaseManager.database());
  mdtSqlRecord record;
  QList<QSqlRecord> dataList;
  bool ok;

  // Scenario
  pvScenario->createSenario();
  /*
   * Create a LinkBeam
   */
  QVERIFY(record.addAllFields("LinkBeam_tbl", pvDatabaseManager.database()));
  record.setValue("Id_PK", 1);
  record.setValue("Identification", "Link beam 1");
  QVERIFY(lb.addRecord(record, "LinkBeam_tbl"));
  dataList = lb.getData("SELECT * FROM LinkBeam_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Id_PK"), QVariant(1));
  QCOMPARE(dataList.at(0).value("Identification"), QVariant("Link beam 1"));
  /*
   * Add a start unit
   */
  QVERIFY(lb.addStartUnit(1000, 1));
  dataList = lb.getData("SELECT * FROM LinkBeam_UnitStart_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(dataList.at(0).value("LinkBeam_Id_FK"), QVariant(1));
  /*
   * Add a end unit
   */
  QVERIFY(lb.addEndUnit(2000, 1));
  dataList = lb.getData("SELECT * FROM LinkBeam_UnitEnd_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Unit_Id_FK"), QVariant(2000));
  QCOMPARE(dataList.at(0).value("LinkBeam_Id_FK"), QVariant(1));
  /*
   * Add (existing) link from unit connection ID 10000 -> 10001 to link beam 1
   */
  QVERIFY(lb.addLink(10000, 10001, 1));
  dataList = lb.getData("SELECT * FROM UnitLink_view WHERE LinkBeam_Id_FK = 1", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("UnitConnectionStart_Id_FK"), QVariant(10000));
  QCOMPARE(dataList.at(0).value("UnitConnectionEnd_Id_FK"), QVariant(10001));
  QVERIFY(!dataList.at(0).value("LinkBeam_Id_FK").isNull());
  QCOMPARE(dataList.at(0).value("LinkBeam_Id_FK"), QVariant(1));
  /*
   * Remove (existing) link from unit connection ID 10000 -> 10001 from link beam 1
   */
  QVERIFY(lb.removeLink(10000, 10001));
  dataList = lb.getData("SELECT * FROM UnitLink_view WHERE UnitConnectionStart_Id_FK = 10000 AND UnitConnectionEnd_Id_FK = 10001", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QVERIFY(dataList.at(0).value("LinkBeam_Id_FK").isNull());

  /*
   * Remove start unit
   */
  QVERIFY(lb.removeStartUnit(1000, 1));
  dataList = lb.getData("SELECT * FROM LinkBeam_UnitStart_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  /*
   * Remove end unit
   */
  QVERIFY(lb.removeEndUnit(2000, 1));
  dataList = lb.getData("SELECT * FROM LinkBeam_UnitEnd_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);

  // Cleanup
  pvScenario->removeScenario();
}

/*
 * Test data manipulation methods
 */

void mdtCableListTest::createTestVehicleTypes()
{
  pvScenario->createTestVehicleTypes();
}

void mdtCableListTest::removeTestVehicleTypes()
{
  pvScenario->removeTestVehicleTypes();
}

void mdtCableListTest::createTestConnectors()
{
  pvScenario->createTestConnectors();
}

void mdtCableListTest::removeTestConnectors()
{
  pvScenario->removeTestConnectors();
}


void mdtCableListTest::createTestArticles()
{
  pvScenario->createTestArticles();
}

void mdtCableListTest::removeTestArticles()
{
  pvScenario->removeTestArticles();
}

void mdtCableListTest::createTestArticleConnections()
{
  pvScenario->createTestArticleConnections();
}

void mdtCableListTest::removeTestArticleConnections()
{
  pvScenario->removeTestArticleConnections();
}

void mdtCableListTest::createTestArticleConnectors()
{
  pvScenario->createTestArticleConnectors();
}

void mdtCableListTest::removeTestArticleConnectors()
{
  pvScenario->removeTestArticleConnectors();
}

void mdtCableListTest::createTestArticleLinks()
{
  pvScenario->createTestArticleLinks();
}

void mdtCableListTest::removeTestArticleLinks()
{
  pvScenario->removeTestArticleLinks();
}

void mdtCableListTest::createTestUnits()
{
  pvScenario->createTestUnits();
}

void mdtCableListTest::removeTestUnits()
{
  pvScenario->removeTestUnits();
}

void mdtCableListTest::createTestVehicleTypeUnitAssignations()
{
  pvScenario->createTestVehicleTypeUnitAssignations();
}

void mdtCableListTest::removeTestVehicleTypeUnitAssignations()
{
  pvScenario->removeTestVehicleTypeUnitAssignations();
}

void mdtCableListTest::createTestUnitConnections()
{
  pvScenario->createTestUnitConnections();
}

void mdtCableListTest::removeTestUnitConnections()
{
  pvScenario->removeTestUnitConnections();
}

void mdtCableListTest::createTestUnitConnectors()
{
  pvScenario->createTestUnitConnectors();
}

void mdtCableListTest::removeTestUnitConnectors()
{
  pvScenario->removeTestUnitConnectors();
}

void mdtCableListTest::createTestLinks()
{
  pvScenario->createTestLinks();
}

void mdtCableListTest::removeTestLinks()
{
  pvScenario->removeTestLinks();
}


/*
 * Test database helper methods
 */

void mdtCableListTest::createDatabaseSchema()
{
  QTemporaryFile dbFile;
  QFileInfo dbFileInfo;

  /*
   * Check Sqlite database creation
   */
  QVERIFY(dbFile.open());
  dbFile.close();
  dbFileInfo.setFile(dbFile.fileName() + ".db");
  ///dbFileInfo.setFile("tests.db");
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
  mdtCableListTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
