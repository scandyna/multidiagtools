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
#include "mdtClConnectorData.h"
#include "mdtClArticleConnectorData.h"
#include "mdtClArticleConnectorData.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClUnitConnectorData.h"
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
  QFAIL("Test not implemented yet");
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
  connectionData.setValue("Article_Id_FK", 10);
  connectionData.setValue("ArticleConnector_Id_FK", 1);
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


}

void mdtCableListTest::articleTest()
{
  mdtClArticle art(0, pvDatabaseManager.database());
  mdtSqlRecord record;
  QSqlRecord data;
  QList<QSqlRecord> dataList;
  mdtSqlRecord connectorData, connectionData;
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

  record.clear();
  QVERIFY(record.addAllFields("ArticleConnection_tbl", pvDatabaseManager.database()));
  // Initially we have not connections
  dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Add a connection to article ID 1
  record.setValue("Id_PK", 1);
  record.setValue("Article_Id_FK", 1);
  record.setValue("ArticleContactName", "Contact 1");
  record.setValue("FunctionEN", "EN: Contact 1");
  QVERIFY(art.addConnection(record));
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
  QVERIFY(connectorData.addAllFields("ArticleConnector_tbl", pvDatabaseManager.database()));
  connectorData.setValue("Id_PK", 1);
  connectorData.setValue("Article_Id_FK", 1);
  connectorData.setValue("Name", "X1");
  // Setup connection data
  connectionData.clear();
  QVERIFY(connectionData.addAllFields("ArticleConnection_tbl", pvDatabaseManager.database()));
  for(int i = 0; i < 11; ++i){
    connectionData.clearValues();
    connectionData.setValue("Id_PK", i+1);
    connectionData.setValue("ArticleContactName", QString::number(i));
    connectionData.setValue("FunctionEN", "Contact " + QString::number(i));
    dataList.append(connectionData);
  }
  // Add connector
  QVERIFY(art.addConnector(connectorData, dataList));
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
  record.clear();
  QVERIFY(record.addAllFields("Connector_tbl", pvDatabaseManager.database()));
  record.setValue("Id_PK", 1);
  QVERIFY(art.addRecord(record, "Connector_tbl"));
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
  QVERIFY(connectorData.addAllFields("ArticleConnector_tbl", pvDatabaseManager.database()));
  connectorData.setValue("Id_PK", 2);
  connectorData.setValue("Article_Id_FK", 1);
  connectorData.setValue("Connector_Id_FK", 1);
  connectorData.setValue("Name", "X2");
  // Setup connection data
  connectorContactIdList.clear();
  connectorContactIdList << 1 << 2 << 3;
  dataList = art.getConnectionDataListFromConnectorContactDataList(connectorContactIdList, &ok);
  QVERIFY(ok);
  // Add connector
  QVERIFY(art.addConnector(connectorData, dataList));
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
}

void mdtCableListTest::unitConnectionDataTest()
{
  QFAIL("Test not implemented yet");
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
