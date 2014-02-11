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
  record.setValue("Name", "A");
  QVERIFY(art.addRecord(record, "ConnectorContact_tbl"));
  record.setValue("Id_PK", 2);
  record.setValue("Connector_Id_FK", 1);
  record.setValue("Name", "B");
  QVERIFY(art.addRecord(record, "ConnectorContact_tbl"));
  record.setValue("Id_PK", 3);
  record.setValue("Connector_Id_FK", 1);
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
  QCOMPARE(data.value("ArticleConnectorName"), QVariant("X2"));
  data = dataList.at(1);
  QCOMPARE(data.value("Connector_Id_FK"), QVariant(1));
  QCOMPARE(data.value("Article_Id_FK"), QVariant(1));
  QCOMPARE(data.value("ArticleConnectorName"), QVariant("X2"));
  data = dataList.at(2);
  QCOMPARE(data.value("Connector_Id_FK"), QVariant(1));
  QCOMPARE(data.value("Article_Id_FK"), QVariant(1));
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

void mdtCableListTest::mdtClLinkDataTest()
{
  QFAIL("Test not implemented yet");
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
  connectionData.setValue("ArticleContactName", "10");
  QVERIFY(art.addConnection(connectionData));
  // Add contact ID 20 to article ID 2
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 20);
  connectionData.setValue("Article_Id_FK", 2);
  connectionData.setValue("ArticleContactName", "20");
  QVERIFY(art.addConnection(connectionData));
  // Add contact ID 21 to article ID 2
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 21);
  connectionData.setValue("Article_Id_FK", 2);
  connectionData.setValue("ArticleContactName", "21");
  QVERIFY(art.addConnection(connectionData));
  // Check back data
  connectionData = art.getConnectionData(10, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(10));
  QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(1));
  QCOMPARE(connectionData.value("ArticleContactName"), QVariant("10"));
  connectionData = art.getConnectionData(20, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(20));
  QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(2));
  QCOMPARE(connectionData.value("ArticleContactName"), QVariant("20"));
  connectionData = art.getConnectionData(21, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(21));
  QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(2));
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
  QCOMPARE(connectionData.value("ArticleContactName"), QVariant("20"));
  connectionData = art.getConnectionData(21, &ok);
  connectionData = connectionDataList.at(1);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(21));
  QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(2));
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
}

void mdtCableListTest::removeTestUnits()
{
  mdtClUnit unit(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(unit.removeData("Unit_tbl", "Id_PK", 1000));
  QVERIFY(unit.removeData("Unit_tbl", "Id_PK", 2000));
  dataList = unit.getData("SELECT * FROM Unit_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTest::createTestVehicleTypeUnitAssignations()
{
  mdtClUnitVehicleType uvt(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(uvt.addUnitVehicleTypeAssignment(1000, 1));
  dataList = uvt.getData("SELECT * FROM VehicleType_Unit_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(dataList.at(0).value("VehicleType_Id_FK"), QVariant(1));
}

void mdtCableListTest::removeTestVehicleTypeUnitAssignations()
{
  mdtClUnitVehicleType uvt(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(uvt.removeUnitVehicleAssignment(1000, 1));
  dataList = uvt.getData("SELECT * FROM VehicleType_Unit_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTest::createTestUnitConnections()
{
  mdtClUnit unit(0, pvDatabaseManager.database());
  mdtClUnitConnectionData connectionData;
  bool ok;

  QVERIFY(connectionData.setup(pvDatabaseManager.database(), true));
  // Add and chek unit connection 10000
  connectionData.setValue("Id_PK", 10000);
  connectionData.setValue("Unit_Id_FK", 1000);
  connectionData.setValue("UnitContactName", "Unit contact 10000");
  QVERIFY(unit.addConnection(connectionData));
  connectionData = unit.getConnectionData(10000, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(10000));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 10000"));
  QVERIFY(connectionData.value("ArticleConnection_Id_FK").isNull());
  // Add and chek unit connection 20000
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 20000);
  connectionData.setValue("Unit_Id_FK", 2000);
  connectionData.setValue("ArticleConnection_Id_FK", 20);
  connectionData.setValue("UnitContactName", "Unit contact 20000");
  QVERIFY(unit.addConnection(connectionData));
  connectionData = unit.getConnectionData(20000, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(20000));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(2000));
  QCOMPARE(connectionData.value("ArticleConnection_Id_FK"), QVariant(20));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 20000"));
  QCOMPARE(connectionData.articleConnectionData().value("Id_PK"), QVariant(20));
}

void mdtCableListTest::removeTestUnitConnections()
{
  mdtClUnit unit(0, pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(unit.removeConnection(10000));
  QVERIFY(unit.removeConnection(20000));
  dataList = unit.getData("SELECT * FROM UnitConnection_tbl", &ok);
  QVERIFY(ok);
}

void mdtCableListTest::createTestUnitConnectors()
{
  mdtClUnit unit(0, pvDatabaseManager.database());
  mdtClUnitConnectorData connectorData;
  mdtClUnitConnectionData connectionData;
  QList<QVariant> idList;
  bool ok;

  QVERIFY(connectorData.setup(pvDatabaseManager.database(), true, true));
  QVERIFY(connectionData.setup(pvDatabaseManager.database(), true));
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
  // Check connection
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
