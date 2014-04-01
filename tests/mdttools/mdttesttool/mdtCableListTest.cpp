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
}

void mdtCableListTest::cleanupTestCase()
{
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
  connectionData.setArticleConnectionData(articleConnectionData);
  QVERIFY(connectionData.isBasedOnArticleConnection());
  QCOMPARE(connectionData.value("ArticleConnection_Id_FK"), QVariant(100));
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
  QVERIFY(unit.editConnection(connectionData));
  connectionData.clearValues();
  connectionData = unit.getConnectionData(10000, true, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Edited contact"));
  // Edit back to original values
  connectionData.setValue("UnitContactName", "Unit contact 10000");
  QVERIFY(unit.editConnection(connectionData));
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
  createTestVehicleTypes();
  createTestArticles();
  createTestConnectors();
  createTestArticleConnections();
  createTestArticleLinks();
  createTestArticleConnectors();
  createTestUnits();
  createTestVehicleTypeUnitAssignations();
  createTestUnitConnections();
  // Add links
  createTestLinks();
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
  // Check that we have expected vehicle type links
  vtLinkDataList = lnk.getVehicleTypeLinkDataByUnitId(1000, &ok);
  QVERIFY(ok);
  QCOMPARE(vtLinkDataList.size(), 2);
  QCOMPARE(vtLinkDataList.at(0).unitConnectionStartId(), QVariant(10000));
  QCOMPARE(vtLinkDataList.at(0).unitConnectionEndId(), QVariant(10001));
  QCOMPARE(vtLinkDataList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtLinkDataList.at(0).vehicleTypeEndId(), QVariant(1));
  QCOMPARE(vtLinkDataList.at(1).unitConnectionStartId(), QVariant(10001));
  QCOMPARE(vtLinkDataList.at(1).unitConnectionEndId(), QVariant(20000));
  QCOMPARE(vtLinkDataList.at(1).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtLinkDataList.at(1).vehicleTypeEndId(), QVariant(2));
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
  removeTestLinks();
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

void mdtCableListTest::pathGraphTest()
{
  QFAIL("Test not implemented yet");
}

/*
 * Test data manipulation methods
 */

void mdtCableListTest::createTestVehicleTypes()
{
  mdtTtBase tb(0, pvDatabaseManager.database());
  mdtSqlRecord record;
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(record.addAllFields("VehicleType_tbl", pvDatabaseManager.database()));
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

void mdtCableListTest::removeTestVehicleTypes()
{
  mdtTtBase tb(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(tb.removeData("VehicleType_tbl", "Id_PK", 1));
  QVERIFY(tb.removeData("VehicleType_tbl", "Id_PK", 2));
  dataList = tb.getData("SELECT * FROM VehicleType_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTest::createTestConnectors()
{
  mdtClArticle art(0, pvDatabaseManager.database());
  mdtClConnectorData connectorData;
  QList<QSqlRecord> dataList;
  bool ok;

  // Add connector 1
  QVERIFY(connectorData.setup(pvDatabaseManager.database()));
  connectorData.setValue("Id_PK", 1);
  QVERIFY(art.addRecord(connectorData, "Connector_tbl"));
  // Check back
  dataList = art.getData("SELECT * FROM Connector_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Id_PK"), QVariant(1));
  // Add connector 2
  connectorData.clearValues();
  connectorData.setValue("Id_PK", 2);
  QVERIFY(art.addRecord(connectorData, "Connector_tbl"));
  // Check back
  dataList = art.getData("SELECT * FROM Connector_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  QCOMPARE(dataList.at(1).value("Id_PK"), QVariant(2));

}

void mdtCableListTest::removeTestConnectors()
{
  mdtClArticle art(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(art.removeData("Connector_tbl", "Id_PK", 1));
  QVERIFY(art.removeData("Connector_tbl", "Id_PK", 2));
  dataList = art.getData("SELECT * FROM Connector_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}


void mdtCableListTest::createTestArticles()
{
  mdtClArticle art(0, pvDatabaseManager.database());
  mdtSqlRecord record;
  QSqlRecord data;
  QList<QSqlRecord> dataList;
  bool ok;

  // Add a article 1
  QVERIFY(record.addAllFields("Article_tbl", pvDatabaseManager.database()));
  record.setValue("Id_PK", 1);
  record.setValue("ArticleCode", "1234");
  record.setValue("DesignationEN", "Article 1");
  QVERIFY(art.addRecord(record, "Article_tbl"));
  // Check added article
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  QCOMPARE(data.value("Id_PK"), QVariant(1));
  QCOMPARE(data.value("ArticleCode"), QVariant("1234"));
  QCOMPARE(data.value("DesignationEN"), QVariant("Article 1"));
  // Add a article 2
  QVERIFY(record.addAllFields("Article_tbl", pvDatabaseManager.database()));
  record.setValue("Id_PK", 2);
  record.setValue("ArticleCode", "5678");
  record.setValue("DesignationEN", "Article 2");
  QVERIFY(art.addRecord(record, "Article_tbl"));
  // Check added article
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  data = dataList.at(1);
  QCOMPARE(data.value("Id_PK"), QVariant(2));
  QCOMPARE(data.value("ArticleCode"), QVariant("5678"));
  QCOMPARE(data.value("DesignationEN"), QVariant("Article 2"));

}

void mdtCableListTest::removeTestArticles()
{
  mdtClArticle art(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(art.removeData("Article_tbl", "Id_PK", 1));
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QVERIFY(art.removeData("Article_tbl", "Id_PK", 2));
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTest::createTestArticleConnections()
{
  mdtClArticle art(0, pvDatabaseManager.database());
  mdtClArticleConnectionData connectionData;
  QList<QVariant> idList;
  QList<mdtClArticleConnectionData> connectionDataList;
  bool ok;

  // Add contact ID 10 to article ID 1
  QVERIFY(connectionData.setup(pvDatabaseManager.database()));
  connectionData.setValue("Id_PK", 10);
  connectionData.setValue("Article_Id_FK", 1);
  connectionData.setValue("ConnectionType_Code_FK", "T");
  connectionData.setValue("ArticleContactName", "10");
  QVERIFY(art.addConnection(connectionData));
  // Add contact ID 20 to article ID 2
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 20);
  connectionData.setValue("Article_Id_FK", 2);
  connectionData.setValue("ConnectionType_Code_FK", "T");
  connectionData.setValue("ArticleContactName", "20");
  QVERIFY(art.addConnection(connectionData));
  // Add contact ID 21 to article ID 2
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 21);
  connectionData.setValue("Article_Id_FK", 2);
  connectionData.setValue("ConnectionType_Code_FK", "T");
  connectionData.setValue("ArticleContactName", "21");
  QVERIFY(art.addConnection(connectionData));
  // Check back data
  connectionData = art.getConnectionData(10, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(10));
  QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(1));
  QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("T"));
  QCOMPARE(connectionData.value("ArticleContactName"), QVariant("10"));
  connectionData = art.getConnectionData(20, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(20));
  QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(2));
  QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("T"));
  QCOMPARE(connectionData.value("ArticleContactName"), QVariant("20"));
  connectionData = art.getConnectionData(21, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(21));
  QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(2));
  QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("T"));
  QCOMPARE(connectionData.value("ArticleContactName"), QVariant("21"));
  // Check connections data list getter method
  idList.clear();
  idList << 20 << 21;
  connectionDataList = art.getConnectionDataListFromConnectionIdList(idList, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionDataList.size(), 2);
  connectionData = connectionDataList.at(0);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(20));
  QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(2));
  QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("T"));
  QCOMPARE(connectionData.value("ArticleContactName"), QVariant("20"));
  connectionData = art.getConnectionData(21, &ok);
  connectionData = connectionDataList.at(1);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(21));
  QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(2));
  QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("T"));
  QCOMPARE(connectionData.value("ArticleContactName"), QVariant("21"));
}

void mdtCableListTest::removeTestArticleConnections()
{
  mdtClArticle art(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(art.removeConnection(10));
  QVERIFY(art.removeConnection(20));
  QVERIFY(art.removeConnection(21));
  dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
  QVERIFY(ok);
}

void mdtCableListTest::createTestArticleConnectors()
{
  mdtClArticle art(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  mdtClArticleConnectorData connectorData;
  mdtClArticleConnectionData connectionsData;
  bool ok;

  // Setup data
  QVERIFY(connectorData.setup(pvDatabaseManager.database(), true));
  QVERIFY(connectionsData.setup(pvDatabaseManager.database()));
  /*
   * Article connector 100
   */
  // Setup connector 100
  connectorData.setValue("Id_PK", 100);
  connectorData.setValue("Article_Id_FK", 1);
  connectorData.setValue("Name", "Article connector 100");
  // Setup connection 15
  connectionsData.setValue("Id_PK", 15);
  connectionsData.setValue("ConnectionType_Code_FK", "P");
  connectionsData.setValue("ArticleContactName", "Article contact 15");
  connectorData.addConnectionData(connectionsData);
  // Add connector and check
  QVERIFY(art.addConnector(connectorData));
  connectorData = art.getConnectorData(100, &ok, true, true);
  QVERIFY(ok);
  QCOMPARE(connectorData.value("Id_PK"), QVariant(100));
  QCOMPARE(connectorData.value("Article_Id_FK"), QVariant(1));
  QCOMPARE(connectorData.value("Name"), QVariant("Article connector 100"));
  /*
   * Article connector 200
   */
  connectorData.clearValues();
  // Setup connector 200
  connectorData.setValue("Id_PK", 200);
  connectorData.setValue("Article_Id_FK", 2);
  connectorData.setValue("Connector_Id_FK", 1);
  connectorData.setValue("Name", "Article connector 200");
  // Setup connection 15
  connectionsData.setValue("Id_PK", 25);
  connectionsData.setValue("ConnectionType_Code_FK", "S");
  connectionsData.setValue("ArticleContactName", "Article contact 25");
  connectorData.addConnectionData(connectionsData);
  // Add connector and check
  QVERIFY(art.addConnector(connectorData));
  connectorData = art.getConnectorData(200, &ok, true, true);
  QVERIFY(ok);
  QCOMPARE(connectorData.value("Id_PK"), QVariant(200));
  QCOMPARE(connectorData.value("Article_Id_FK"), QVariant(2));
  QCOMPARE(connectorData.value("Connector_Id_FK"), QVariant(1));
  QCOMPARE(connectorData.value("Name"), QVariant("Article connector 200"));
}

void mdtCableListTest::removeTestArticleConnectors()
{
  mdtClArticle art(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(art.removeConnector(100));
  QVERIFY(art.removeConnector(200));
  dataList = art.getData("SELECT * FROM ArticleConnector_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTest::createTestArticleLinks()
{
  mdtClArticle art(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(art.addCableLink(10, 20, "", 0.0));  
  QVERIFY(art.addCableLink(21, 20, "", 0.0));
  dataList = art.getData("SELECT * FROM ArticleLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  QCOMPARE(dataList.at(0).value("ArticleConnectionStart_Id_FK"), QVariant(10));
  QCOMPARE(dataList.at(0).value("ArticleConnectionEnd_Id_FK"), QVariant(20));
  QCOMPARE(dataList.at(1).value("ArticleConnectionStart_Id_FK"), QVariant(21));
  QCOMPARE(dataList.at(1).value("ArticleConnectionEnd_Id_FK"), QVariant(20));
}

void mdtCableListTest::removeTestArticleLinks()
{
  mdtClArticle art(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(art.removeLink(10, 20));
  QVERIFY(art.removeLink(21, 20));
  dataList = art.getData("SELECT * FROM ArticleLink_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTest::createTestUnits()
{
  mdtClUnit unit(0, pvDatabaseManager.database());
  mdtSqlRecord data;
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(data.addAllFields("Unit_tbl", pvDatabaseManager.database()));
  // Add and check unit 1000
  data.setValue("Id_PK", 1000);
  data.setValue("SchemaPosition", "Position 1000");
  QVERIFY(unit.addRecord(data, "Unit_tbl"));
  dataList = unit.getData("SELECT * FROM Unit_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Id_PK"), QVariant(1000));
  QCOMPARE(dataList.at(0).value("SchemaPosition"), QVariant("Position 1000"));
  // Add and check unit 2000
  data.setValue("Id_PK", 2000);
  data.setValue("Article_Id_FK", 2);
  data.setValue("SchemaPosition", "Position 2000");
  QVERIFY(unit.addRecord(data, "Unit_tbl"));
  dataList = unit.getData("SELECT * FROM Unit_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  QCOMPARE(dataList.at(1).value("Id_PK"), QVariant(2000));
  QCOMPARE(dataList.at(1).value("Article_Id_FK"), QVariant(2));
  QCOMPARE(dataList.at(1).value("SchemaPosition"), QVariant("Position 2000"));
  // Add and check unit 2001
  data.setValue("Id_PK", 2001);
  data.setValue("Article_Id_FK", 2);
  data.setValue("SchemaPosition", "Position 2001");
  QVERIFY(unit.addRecord(data, "Unit_tbl"));
  dataList = unit.getData("SELECT * FROM Unit_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 3);
  QCOMPARE(dataList.at(2).value("Id_PK"), QVariant(2001));
  QCOMPARE(dataList.at(2).value("Article_Id_FK"), QVariant(2));
  QCOMPARE(dataList.at(2).value("SchemaPosition"), QVariant("Position 2001"));
}

void mdtCableListTest::removeTestUnits()
{
  mdtClUnit unit(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(unit.removeData("Unit_tbl", "Id_PK", 1000));
  QVERIFY(unit.removeData("Unit_tbl", "Id_PK", 2000));
  QVERIFY(unit.removeData("Unit_tbl", "Id_PK", 2001));
  dataList = unit.getData("SELECT * FROM Unit_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTest::createTestVehicleTypeUnitAssignations()
{
  mdtClUnitVehicleType uvt(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  // Unit 1000 <-> VehicleType 1
  QVERIFY(uvt.addUnitVehicleTypeAssignment(1000, 1));
  dataList = uvt.getData("SELECT * FROM VehicleType_Unit_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(dataList.at(0).value("VehicleType_Id_FK"), QVariant(1));
  // Unit 2000 <-> VehicleType 2
  QVERIFY(uvt.addUnitVehicleTypeAssignment(2000, 2));
  dataList = uvt.getData("SELECT * FROM VehicleType_Unit_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  QCOMPARE(dataList.at(1).value("Unit_Id_FK"), QVariant(2000));
  QCOMPARE(dataList.at(1).value("VehicleType_Id_FK"), QVariant(2));
  // Unit 2001 <-> VehicleType 2
  QVERIFY(uvt.addUnitVehicleTypeAssignment(2001, 2));
  dataList = uvt.getData("SELECT * FROM VehicleType_Unit_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 3);
  QCOMPARE(dataList.at(2).value("Unit_Id_FK"), QVariant(2001));
  QCOMPARE(dataList.at(2).value("VehicleType_Id_FK"), QVariant(2));
}

void mdtCableListTest::removeTestVehicleTypeUnitAssignations()
{
  mdtClUnitVehicleType uvt(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(uvt.removeUnitVehicleAssignment(1000, 1));
  QVERIFY(uvt.removeUnitVehicleAssignment(2000, 2));
  QVERIFY(uvt.removeUnitVehicleAssignment(2001, 2));
  dataList = uvt.getData("SELECT * FROM VehicleType_Unit_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTest::createTestUnitConnections()
{
  mdtClUnit unit(0, pvDatabaseManager.database());
  mdtClUnitConnectionData connectionData;
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(connectionData.setup(pvDatabaseManager.database(), true));
  // Add and chek unit connection 10000
  connectionData.setValue("Id_PK", 10000);
  connectionData.setValue("Unit_Id_FK", 1000);
  connectionData.setValue("UnitContactName", "Unit contact 10000");
  QVERIFY(unit.addConnection(connectionData));
  connectionData = unit.getConnectionData(10000, true, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(10000));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 10000"));
  QVERIFY(connectionData.value("ArticleConnection_Id_FK").isNull());

  // Add and chek unit connection 10001
  connectionData.setValue("Id_PK", 10001);
  connectionData.setValue("Unit_Id_FK", 1000);
  connectionData.setValue("UnitContactName", "Unit contact 10001");
  QVERIFY(unit.addConnection(connectionData));
  connectionData = unit.getConnectionData(10001, true, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(10001));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 10001"));
  QVERIFY(connectionData.value("ArticleConnection_Id_FK").isNull());
  /*
   * Add and chek unit connection 20000
   */
  // Add connection and check
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 20000);
  connectionData.setValue("Unit_Id_FK", 2000);
  connectionData.setValue("ArticleConnection_Id_FK", 20);
  connectionData.setValue("UnitContactName", "Unit contact 20000");
  QVERIFY(unit.addConnection(connectionData));
  connectionData = unit.getConnectionData(20000, true, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(20000));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(2000));
  QCOMPARE(connectionData.value("ArticleConnection_Id_FK"), QVariant(20));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 20000"));
  QCOMPARE(connectionData.articleConnectionData().value("Id_PK"), QVariant(20));
  /*
   * Check links
   *  We have a article link from artice connection 21 to 20
   *  Check that no link was added now
   */
  dataList = unit.getData("SELECT * FROM UnitLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Add and chek unit connection 20001
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 20001);
  connectionData.setValue("Unit_Id_FK", 2000);
  connectionData.setValue("ArticleConnection_Id_FK", 21);
  connectionData.setValue("UnitContactName", "Unit contact 20001");
  QVERIFY(unit.addConnection(connectionData));
  connectionData = unit.getConnectionData(20001, true, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(20001));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(2000));
  QCOMPARE(connectionData.value("ArticleConnection_Id_FK"), QVariant(21));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 20001"));
  QCOMPARE(connectionData.articleConnectionData().value("Id_PK"), QVariant(21));
  /*
   * Check links
   *  We have a article link from artice connection 21 to 20
   *  Check that link was added
   */
  dataList = unit.getData("SELECT * FROM UnitLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("UnitConnectionStart_Id_FK"), QVariant(20001));
  QCOMPARE(dataList.at(0).value("UnitConnectionEnd_Id_FK"), QVariant(20000));
  QCOMPARE(dataList.at(0).value("ArticleConnectionStart_Id_FK"), QVariant(21));
  QCOMPARE(dataList.at(0).value("ArticleConnectionEnd_Id_FK"), QVariant(20));
  QCOMPARE(dataList.at(0).value("StartUnit_Id_FK"), QVariant(2000));
  QCOMPARE(dataList.at(0).value("EndUnit_Id_FK"), QVariant(2000));
  // Check that vehicle type links are also added
  dataList = unit.getData("SELECT * FROM VehicleType_Link_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("VehicleTypeStart_Id_FK"), QVariant(2));
  QCOMPARE(dataList.at(0).value("VehicleTypeEnd_Id_FK"), QVariant(2));
  QCOMPARE(dataList.at(0).value("UnitConnectionStart_Id_FK"), QVariant(20001));
  QCOMPARE(dataList.at(0).value("UnitConnectionEnd_Id_FK"), QVariant(20000));
  // Add and chek unit connection 20002
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 20002);
  connectionData.setValue("Unit_Id_FK", 2001);
  connectionData.setValue("ArticleConnection_Id_FK", 20);
  connectionData.setValue("UnitContactName", "Unit contact 20002");
  QVERIFY(unit.addConnection(connectionData));
  connectionData = unit.getConnectionData(20002, true, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(20002));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(2001));
  QCOMPARE(connectionData.value("ArticleConnection_Id_FK"), QVariant(20));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 20002"));
  QCOMPARE(connectionData.articleConnectionData().value("Id_PK"), QVariant(20));
  /*
   * Check links
   *  We have a article link from artice connection 21 to 20
   *  Check that no more link was added now
   */
  dataList = unit.getData("SELECT * FROM UnitLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  // Add and chek unit connection 20003
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 20003);
  connectionData.setValue("Unit_Id_FK", 2001);
  connectionData.setValue("ArticleConnection_Id_FK", 21);
  connectionData.setValue("UnitContactName", "Unit contact 20003");
  QVERIFY(unit.addConnection(connectionData));
  connectionData = unit.getConnectionData(20003, true, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(20003));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(2001));
  QCOMPARE(connectionData.value("ArticleConnection_Id_FK"), QVariant(21));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 20003"));
  QCOMPARE(connectionData.articleConnectionData().value("Id_PK"), QVariant(21));
  /*
   * Check links
   *  We have a article link from artice connection 21 to 20
   *  Check that link was added
   */
  dataList = unit.getData("SELECT * FROM UnitLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  QCOMPARE(dataList.at(1).value("UnitConnectionStart_Id_FK"), QVariant(20003));
  QCOMPARE(dataList.at(1).value("UnitConnectionEnd_Id_FK"), QVariant(20002));
  QCOMPARE(dataList.at(1).value("ArticleConnectionStart_Id_FK"), QVariant(21));
  QCOMPARE(dataList.at(1).value("ArticleConnectionEnd_Id_FK"), QVariant(20));
  QCOMPARE(dataList.at(1).value("StartUnit_Id_FK"), QVariant(2001));
  QCOMPARE(dataList.at(1).value("EndUnit_Id_FK"), QVariant(2001));
  // Check that vehicle type links are also added
  dataList = unit.getData("SELECT * FROM VehicleType_Link_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  QCOMPARE(dataList.at(1).value("VehicleTypeStart_Id_FK"), QVariant(2));
  QCOMPARE(dataList.at(1).value("VehicleTypeEnd_Id_FK"), QVariant(2));
  QCOMPARE(dataList.at(1).value("UnitConnectionStart_Id_FK"), QVariant(20003));
  QCOMPARE(dataList.at(1).value("UnitConnectionEnd_Id_FK"), QVariant(20002));
}

void mdtCableListTest::removeTestUnitConnections()
{
  mdtClUnit unit(0, pvDatabaseManager.database());
  mdtClLink lnk(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(unit.removeConnection(10000));
  QVERIFY(unit.removeConnection(10001));
  // Remove links that was added during connection creation
  dataList = unit.getData("SELECT * FROM UnitLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  // Remove unit connection 20000
  QVERIFY(unit.removeConnection(20000));
  // Check that link 20001 to 20000 was removed from Link_tbl
  dataList = unit.getData("SELECT * FROM UnitLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("UnitConnectionStart_Id_FK"), QVariant(20003));
  QCOMPARE(dataList.at(0).value("UnitConnectionEnd_Id_FK"), QVariant(20002));
  // Check that link 20001 to 20000 was removed from VehicleType_Link_tbl
  dataList = unit.getData("SELECT * FROM VehicleType_Link_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("UnitConnectionStart_Id_FK"), QVariant(20003));
  QCOMPARE(dataList.at(0).value("UnitConnectionEnd_Id_FK"), QVariant(20002));
  // Remove unit connection 20001
  QVERIFY(unit.removeConnection(20001));
  // Check that no more link was removed
  dataList = unit.getData("SELECT * FROM UnitLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  // Remove unit connection 20002
  QVERIFY(unit.removeConnection(20002));
  // Check that link 20003 to 20002 was removed from Link_tbl
  dataList = unit.getData("SELECT * FROM UnitLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Check that link 20003 to 20002 was removed from VehicleType_Link_tbl
  dataList = unit.getData("SELECT * FROM VehicleType_Link_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Remove unit connection 20003
  QVERIFY(unit.removeConnection(20003));
  dataList = unit.getData("SELECT * FROM UnitConnection_tbl", &ok);
  QVERIFY(ok);
  
}

void mdtCableListTest::createTestUnitConnectors()
{
  mdtClUnit unit(0, pvDatabaseManager.database());
  mdtClUnitConnectorData connectorData;
  mdtClUnitConnectionData connectionData;
  QList<mdtClUnitConnectionData> connectionDataList;
  QList<QVariant> idList;
  QVariant id;
  bool ok;

  QVERIFY(connectorData.setup(pvDatabaseManager.database(), true, true));
  QVERIFY(connectionData.setup(pvDatabaseManager.database(), true));
  // Check unit connector ID by article connector ID method
  id = unit.getUnitConnectorIdBasedOnArticleConnectorId(QVariant(), 1000, &ok);
  QVERIFY(ok);
  QVERIFY(id.isNull());
  /*
   * Unit connector 100000:
   *  - Id_PK : 100000 , Unit_Id_FK : 1000 , Connector_Id_FK : NULL , ArticleConnector_Id_FK : NULL , Name : Unit connector 100000
   *  -> Connection: Id_PK 10005 , UnitContactName : Unit contact 10005
   */
  // Setup and add unit connector
  connectorData.setValue("Id_PK", 100000);
  connectorData.setValue("Unit_Id_FK", 1000);
  connectorData.setValue("Name", "Unit connector 100000");
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 10005);
  connectionData.setValue("UnitContactName", "Unit contact 10005");
  connectorData.addConnectionData(connectionData);
  QVERIFY(unit.addConnector(connectorData));
  // Check connector
  connectorData = unit.getConnectorData(100000, &ok, true, true, true);
  QVERIFY(ok);
  QCOMPARE(connectorData.value("Id_PK"), QVariant(100000));
  QCOMPARE(connectorData.value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(connectorData.value("Name"), QVariant("Unit connector 100000"));
  // Check unit connector ID by article connector ID method
  id = unit.getUnitConnectorIdBasedOnArticleConnectorId(QVariant(), 1000, &ok);
  QVERIFY(ok);
  QVERIFY(id.isNull());
  // Check connection
  connectionData = connectorData.connectionData(10005, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(10005));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(connectionData.value("UnitConnector_Id_FK"), QVariant(100000));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 10005"));

  /*
   * Unit connector 200000:
   *  - Id_PK : 200000 , Unit_Id_FK : 2000 , Connector_Id_FK : 1 , ArticleConnector_Id_FK : 200 , Name : Unit connector 200000
   *  -> Connection: Id_PK 20005 , ArticleConnection_Id_FK : 25 , Name : Unit contact 20005
   */
  connectorData.clearValues();
  // Setup and add unit connector
  connectorData.setValue("Id_PK", 200000);
  connectorData.setValue("Unit_Id_FK", 2000);
  connectorData.setValue("Name", "Unit connector 200000");
  QVERIFY(unit.addArticleConnectorData(connectorData, 200, false));
  // Add unit connections based on article connections
  idList.clear();
  idList << 25;
  QVERIFY(unit.addConnectionDataListFromArticleConnectionIdList(connectorData, idList, true));
  // Edit unit connections
  QCOMPARE(connectorData.connectionDataList().size(), 1);
  connectionData = connectorData.connectionDataList().at(0);
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Article contact 25"));
  connectionData.setValue("Id_PK", 20005);
  connectionData.setValue("UnitContactName", "Unit contact 20005");
  connectorData.connectionDataList()[0] = connectionData;
  QVERIFY(unit.addConnector(connectorData));
  // Check connector
  connectorData = unit.getConnectorData(200000, &ok, true, true, true);
  QVERIFY(ok);
  QCOMPARE(connectorData.value("Id_PK"), QVariant(200000));
  QCOMPARE(connectorData.value("Unit_Id_FK"), QVariant(2000));
  QCOMPARE(connectorData.value("Connector_Id_FK"), QVariant(1));
  QCOMPARE(connectorData.value("ArticleConnector_Id_FK"), QVariant(200));
  QCOMPARE(connectorData.value("Name"), QVariant("Unit connector 200000"));
  // Check unit connector ID by article connector ID method
  id = unit.getUnitConnectorIdBasedOnArticleConnectorId(200, 2000, &ok);
  QVERIFY(ok);
  QCOMPARE(id, QVariant(200000));
  // Check connection
  connectionData = connectorData.connectionData(20005, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(20005));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(2000));
  QCOMPARE(connectionData.value("UnitConnector_Id_FK"), QVariant(200000));
  QCOMPARE(connectionData.value("ArticleConnection_Id_FK"), QVariant(25));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 20005"));

  /*
   * Now remove unit connection 20005.
   * Then, add a unit connection based on artice connection ID 25.
   * We know that article connection ID 25 is based on article connector ID 200,
   *  and unit has unit connector ID 200000 wich is based on artice connector ID 200.
   * So, check that adding a unit connection based on article connection ID 25
   *  will make this unit connection based on unit connector ID 200000.
   */
  // Remove unit connection 20005
  QVERIFY(unit.removeConnection(20005));
  connectionData = unit.getConnectionData(20005, false, &ok);
  QVERIFY(ok);
  QVERIFY(connectionData.value("Id_PK").isNull());
  // get unit connections based on article connections
  idList.clear();
  idList << 25;
  connectionDataList = unit.getUnitConnectionDataListFromArticleConnectionIdList(2000, idList, true, &ok);
  QVERIFY(ok);
  // Add connection and check
  QVERIFY(unit.addConnections(connectionDataList));
  connectionData = connectorData.connectionData(20005, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(20005));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(2000));
  QCOMPARE(connectionData.value("UnitConnector_Id_FK"), QVariant(200000));
  QCOMPARE(connectionData.value("ArticleConnection_Id_FK"), QVariant(25));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 20005"));
}

void mdtCableListTest::removeTestUnitConnectors()
{
  mdtClUnit unit(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(unit.removeConnector(100000));
  QVERIFY(unit.removeConnector(200000));
  dataList = unit.getData("SELECT * FROM UnitConnector_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTest::createTestLinks()
{
  mdtClLink lnk(0, pvDatabaseManager.database());
  mdtClUnit unit(0, pvDatabaseManager.database());
  QList<QVariant> vtStartIdList, vtEndIdList;
  mdtClLinkData linkData;
  mdtClUnitConnectionData startConnectionData, endConnectionData;
  QList<QSqlRecord> dataList;
  QStringList expectedFields;
  bool ok;

  expectedFields << "UnitConnectionStart_Id_FK" << "UnitConnectionEnd_Id_FK" << "ArticleConnectionStart_Id_FK" << "ArticleConnectionEnd_Id_FK";
  QVERIFY(linkData.setup(pvDatabaseManager.database()));

  /*
   * Add link with unit connections not based on article connections
   */
  // Setup connections data
  startConnectionData = unit.getConnectionData(10000, false, &ok);
  QVERIFY(ok);
  endConnectionData = unit.getConnectionData(10001, false, &ok);
  QVERIFY(ok);
  // Setup vehicle link data
  vtStartIdList.append(1);
  vtEndIdList.append(1);
  // Setup link data
  linkData.setConnectionData(startConnectionData, endConnectionData);
  linkData.setValue("Identification", "Link 10000<->10001");
  linkData.setValue("LinkType_Code_FK", "CABLELINK");
  linkData.setValue("LinkDirection_Code_FK", "BID");
  QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
  // Add link
  QVERIFY(!lnk.linkExists(10000, 10001, &ok));
  QVERIFY(ok);
  QVERIFY(lnk.addLink(linkData));
  QVERIFY(lnk.linkExists(10000, 10001, &ok));
  QVERIFY(ok);
  // Check back in Link_tbl
  dataList = lnk.getData("SELECT * FROM LinkList_view WHERE UnitConnectionStart_Id_FK = 10000 AND UnitConnectionEnd_Id_FK = 10001", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Identification"), QVariant("Link 10000<->10001"));
  QCOMPARE(dataList.at(0).value("LinkType_Code_FK"), QVariant("CABLELINK"));
  // Check back in VehicleType_Link_tbl
  dataList = lnk.getData("SELECT * FROM VehicleType_Link_tbl WHERE UnitConnectionStart_Id_FK = 10000 AND UnitConnectionEnd_Id_FK = 10001 AND VehicleTypeStart_Id_FK = 1 AND VehicleTypeEnd_Id_FK = 1", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  /*
   * Add link with unit connection based on article connection
   *  If link should based on a article link, it was allready added
   *  during unit connection insersion.
   *  So, link created here is NOT based on article link.
   */
  startConnectionData = unit.getConnectionData(10001, false, &ok);
  QVERIFY(ok);
  endConnectionData = unit.getConnectionData(20000, false, &ok);
  QVERIFY(ok);
  // Setup vehicle link data
  vtStartIdList.clear();
  vtEndIdList.clear();
  vtStartIdList.append(1);
  vtEndIdList.append(2);
  // Setup link data
  linkData.setConnectionData(startConnectionData, endConnectionData);
  linkData.setValue("Identification", "Link 10001<->20000");
  linkData.setValue("LinkType_Code_FK", "CABLELINK");
  linkData.setValue("LinkDirection_Code_FK", "BID");
  QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
  // Add link
  QVERIFY(!lnk.linkExists(10001, 20000, &ok));
  QVERIFY(ok);
  QVERIFY(lnk.addLink(linkData));
  QVERIFY(lnk.linkExists(10001, 20000, &ok));
  QVERIFY(ok);
  // Check back in Link_tbl - We take UnitLink_view because it include ArticleConnectionStart/End_Id_FK
  dataList = lnk.getData("SELECT * FROM UnitLink_view WHERE UnitConnectionStart_Id_FK = 10001 AND UnitConnectionEnd_Id_FK = 20000", &ok, expectedFields);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Identification"), QVariant("Link 10001<->20000"));
  QCOMPARE(dataList.at(0).value("LinkType_Code_FK"), QVariant("CABLELINK"));
  QVERIFY(dataList.at(0).value("ArticleConnectionStart_Id_FK").isNull());
  QVERIFY(dataList.at(0).value("ArticleConnectionEnd_Id_FK").isNull());
  // Check back in VehicleType_Link_tbl
  dataList = lnk.getData("SELECT * FROM VehicleType_Link_tbl WHERE UnitConnectionStart_Id_FK = 10001 AND UnitConnectionEnd_Id_FK = 20000 AND VehicleTypeStart_Id_FK = 1 AND VehicleTypeEnd_Id_FK = 2", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  // Check also with getLinkData
  linkData = lnk.getLinkData(10001, 20000, true, true, &ok);
  QVERIFY(ok);
  QCOMPARE(linkData.value("Identification"), QVariant("Link 10001<->20000"));
  QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(10001));
  QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(20000));
  QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(10001));
  QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(20000));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(2));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId(), QVariant(10001));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId(), QVariant(20000));
}

void mdtCableListTest::removeTestLinks()
{
  mdtClLink lnk(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(lnk.removeLink(10000, 10001));
  QVERIFY(lnk.removeLink(10001, 20000));
  dataList = lnk.getData("SELECT * FROM VehicleType_Link_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2); // 2 links added during unit connection creation
  dataList = lnk.getData("SELECT * FROM Link_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2); // 2 links added during unit connection creation
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
