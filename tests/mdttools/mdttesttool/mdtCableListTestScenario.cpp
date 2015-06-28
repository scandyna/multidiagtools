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
#include "mdtCableListTestScenario.h"
#include "mdtSqlRecord.h"
#include "mdtTtBase.h"
#include "mdtClArticle.h"
#include "mdtClArticleLink.h"
#include "mdtClUnit.h"
#include "mdtClUnitConnection.h"
#include "mdtClConnector.h"
#include "mdtClConnectorData.h"
#include "mdtClConnectorKeyData.h"
#include "mdtClConnectionTypeData.h"
#include "mdtClConnectorContactData.h"
#include "mdtClArticleConnection.h"
#include "mdtClArticleConnectorData.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClUnitConnectorData.h"
#include "mdtClUnitVehicleType.h"
#include "mdtClLinkVersion.h"
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

void mdtCableListTestScenario::createSenario()
{
  createTestVehicleTypes();
  createTestArticles();
  createTestConnectors();
  createTestArticleConnections();
  createTestArticleLinks();
  createTestArticleConnectors();
  createTestUnits();
  createTestVehicleTypeUnitAssignations();
  createTestUnitConnections();
  createTestUnitConnectors();
  createTestLinks();
}

void mdtCableListTestScenario::removeScenario()
{
  removeTestLinks();
  removeTestUnitConnectors();
  removeTestUnitConnections();
  removeTestVehicleTypeUnitAssignations();
  removeTestUnits();
  removeTestArticleLinks();
  removeTestArticleConnectors();
  removeTestArticleConnections();
  removeTestArticles();
  removeTestConnectors();
  removeTestVehicleTypes();
  removeTestLinkVersions();
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

void mdtCableListTestScenario::createTestConnectors()
{
  mdtClConnector cnr(pvDatabase);
  mdtClConnectorKeyData connectorKey;
  mdtClConnectorData connectorData;
  mdtClConnectorContactKeyData contactKey;
  mdtClConnectorContactData contactData;
  bool ok;

  // Add connector 1
  connectorKey.id = 1;
  connectorData.setKeyData(connectorKey);
  connectorKey = cnr.addConnector(connectorData, true);
  QCOMPARE(connectorKey.id, QVariant(1));
  // Check back
  connectorData = cnr.getConnectorData(connectorKey, true, ok);
  QVERIFY(ok);
  QCOMPARE(connectorData.keyData().id, QVariant(1));
  // Add connector 2
  connectorData.clear();
  connectorKey.clear();
  connectorKey.id = 2;
  connectorData.setKeyData(connectorKey);
  connectorKey = cnr.addConnector(connectorData, true);
  QCOMPARE(connectorKey.id, QVariant(2));
  // Check back
  connectorData = cnr.getConnectorData(connectorKey, true, ok);
  QVERIFY(ok);
  QCOMPARE(connectorData.keyData().id, QVariant(2));
  /*
   * Create connect 3
   *  - Id_PK : 3
   *   -> Contact : Id_PK : 300 (, Connector_Id_FK : 3) , ConnectionType_Code_FK : P
   *   -> Contact : Id_PK : 301 (, Connector_Id_FK : 3) , ConnectionType_Code_FK : S
   */
  connectorKey.clear();
  connectorData.clear();
  // Setup connector data
  connectorKey.id = 3;
  connectorData.setKeyData(connectorKey);
  // Add contact 300
  contactData.clear();
  contactKey.clear();
  contactKey.id = 300;
  contactKey.setConnectionType(mdtClConnectionType_t::Pin);
  contactData.setKeyData(contactKey);
  connectorData.addContactData(contactData);
  // Add contact 301
  contactKey.id = 301;
  contactKey.setConnectionType(mdtClConnectionType_t::Socket);
  contactData.setKeyData(contactKey);
  connectorData.addContactData(contactData);
  // Add connector to DB
  connectorKey = cnr.addConnector(connectorData, true);
  QCOMPARE(connectorKey.id, QVariant(3));
  // Check back
  connectorData = cnr.getConnectorData(connectorKey, true, ok);
  QVERIFY(ok);
  QCOMPARE(connectorData.keyData().id, QVariant(3));
  QCOMPARE(connectorData.contactDataList().size(), 2);
  QCOMPARE(connectorData.contactDataList().at(0).keyData().id, QVariant(300));
  QCOMPARE(connectorData.contactDataList().at(0).keyData().connectorFk().id, QVariant(3));
  QVERIFY(connectorData.contactDataList().at(0).connectionType() == mdtClConnectionType_t::Pin);
  QCOMPARE(connectorData.contactDataList().at(1).keyData().id, QVariant(301));
  QCOMPARE(connectorData.contactDataList().at(1).keyData().connectorFk().id, QVariant(3));
  QVERIFY(connectorData.contactDataList().at(1).connectionType() == mdtClConnectionType_t::Socket);

  
/*
  mdtClArticle art(0, pvDatabase);
  
  mdtSqlRecord contactData;
  QList<QSqlRecord> dataList;*/
  

  // Add connector 1
//   QVERIFY(connectorData.setup(pvDatabase));
//   connectorData.setValue("Id_PK", 1);
//   QVERIFY(art.addRecord(connectorData, "Connector_tbl"));
//   // Check back
//   dataList = art.getData("SELECT * FROM Connector_tbl", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 1);
//   QCOMPARE(dataList.at(0).value("Id_PK"), QVariant(1));
//   // Add connector 2
//   connectorData.clearValues();
//   connectorData.setValue("Id_PK", 2);
//   QVERIFY(art.addRecord(connectorData, "Connector_tbl"));
//   // Check back
//   dataList = art.getData("SELECT * FROM Connector_tbl", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 2);
//   QCOMPARE(dataList.at(1).value("Id_PK"), QVariant(2));
  /*
   * Create connect 3
   *  - Id_PK : 3
   *   -> Contact : Id_PK : 300 (, Connector_Id_FK : 3) , ConnectionType_Code_FK : P
   *   -> Contact : Id_PK : 301 (, Connector_Id_FK : 3) , ConnectionType_Code_FK : S
   */
//   connectorData.clearValues();
//   connectorData.setValue("Id_PK", 3);
//   QVERIFY(art.addRecord(connectorData, "Connector_tbl"));
//   // Setup contact data
//   QVERIFY(contactData.addAllFields("ConnectorContact_tbl", pvDatabase));
//   /*
//    * Add contact ID 300
//    */
//   contactData.setValue("Id_PK", 300);
//   contactData.setValue("ConnectionType_Code_FK", "P");
//   connectorData.addContactData(contactData);
//   // Get contact data and check
//   contactData = connectorData.contactData(300, &ok);
//   QVERIFY(ok);
//   QCOMPARE(contactData.value("Id_PK"), QVariant(300));
//   QCOMPARE(contactData.value("Connector_Id_FK"), QVariant(3));
//   QCOMPARE(contactData.value("ConnectionType_Code_FK"), QVariant("P"));
//   // Add contact to db
//   QVERIFY(art.addRecord(contactData, "ConnectorContact_tbl"));
//   // Check back
//   dataList = art.getData("SELECT * FROM ConnectorContact_tbl WHERE Id_PK = 300", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 1);
//   QCOMPARE(dataList.at(0).value("Id_PK"), QVariant(300));
//   QCOMPARE(dataList.at(0).value("Connector_Id_FK"), QVariant(3));
//   QCOMPARE(dataList.at(0).value("ConnectionType_Code_FK"), QVariant("P"));
//   /*
//    * Add contact ID 301
//    */
//   contactData.setValue("Id_PK", 301);
//   contactData.setValue("ConnectionType_Code_FK", "S");
//   connectorData.addContactData(contactData);
//   // Get contact data and check
//   contactData = connectorData.contactData(301, &ok);
//   QVERIFY(ok);
//   QCOMPARE(contactData.value("Id_PK"), QVariant(301));
//   QCOMPARE(contactData.value("Connector_Id_FK"), QVariant(3));
//   QCOMPARE(contactData.value("ConnectionType_Code_FK"), QVariant("S"));
//   // Add contact to db
//   QVERIFY(art.addRecord(contactData, "ConnectorContact_tbl"));
//   // Check back
//   dataList = art.getData("SELECT * FROM ConnectorContact_tbl WHERE Id_PK = 301", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 1);
//   QCOMPARE(dataList.at(0).value("Id_PK"), QVariant(301));
//   QCOMPARE(dataList.at(0).value("Connector_Id_FK"), QVariant(3));
//   QCOMPARE(dataList.at(0).value("ConnectionType_Code_FK"), QVariant("S"));
}

void mdtCableListTestScenario::removeTestConnectors()
{
  mdtClArticle art(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

  // Remove connectors 1 and 2
  QVERIFY(art.removeData("Connector_tbl", "Id_PK", 1));
  QVERIFY(art.removeData("Connector_tbl", "Id_PK", 2));
  // Remove connector 3
  QVERIFY(art.removeData("ConnectorContact_tbl", "Id_PK", 300));
  QVERIFY(art.removeData("ConnectorContact_tbl", "Id_PK", 301));
  QVERIFY(art.removeData("Connector_tbl", "Id_PK", 3));
  // Check back
  dataList = art.getData("SELECT * FROM Connector_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTestScenario::createTestArticles()
{
  mdtClArticle art(0, pvDatabase);
  mdtSqlRecord record;
  QSqlRecord data;
  QList<QSqlRecord> dataList;
  bool ok;

  // Add a article 1
  QVERIFY(record.addAllFields("Article_tbl", pvDatabase));
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
  QVERIFY(record.addAllFields("Article_tbl", pvDatabase));
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

void mdtCableListTestScenario::removeTestArticles()
{
  mdtClArticle art(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(art.removeData("Article_tbl", "Id_PK", 1));
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  ///QCOMPARE(dataList.size(), 1);  // Fails if articles where never created
  QVERIFY(art.removeData("Article_tbl", "Id_PK", 2));
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTestScenario::createTestArticleConnections()
{
  mdtClArticleConnection acnx(pvDatabase);
  mdtClArticleConnectionKeyData key;
  mdtClArticleConnectionData connectionData;
  bool ok;

  /*
   * Add article connections
   */
  // Add contact ID 10 to article ID 1
  connectionData.clear();
  key.clear();
  key.id = 10;
  key.setArticleId(1);
  key.setConnectionType(mdtClConnectionType_t::Terminal);
  connectionData.setKeyData(key);
  connectionData.name = "10";
  connectionData.resistance = 0.1;
  QVERIFY(!acnx.addArticleConnection(connectionData, true).isNull());
  // Add contact ID 20 to article ID 2
  connectionData.clear();
  key.clear();
  key.id = 20;
  key.setArticleId(2);
  key.setConnectionType(mdtClConnectionType_t::Terminal);
  connectionData.setKeyData(key);
  connectionData.name = "20";
  connectionData.resistance = 0.2;
  QVERIFY(!acnx.addArticleConnection(connectionData, true).isNull());
  // Add contact ID 21 to article ID 2
  connectionData.clear();
  key.clear();
  key.id = 21;
  key.setArticleId(2);
  key.setConnectionType(mdtClConnectionType_t::Terminal);
  connectionData.setKeyData(key);
  connectionData.name = "21";
  connectionData.resistance = 0.21;
  QVERIFY(!acnx.addArticleConnection(connectionData, true).isNull());
  // Add contact ID 22 to article ID 2
  connectionData.clear();
  key.clear();
  key.id = 22;
  key.setArticleId(2);
  key.setConnectionType(mdtClConnectionType_t::Socket);
  connectionData.setKeyData(key);
  connectionData.name = "22";
  connectionData.resistance = 0.22;
  QVERIFY(!acnx.addArticleConnection(connectionData, true).isNull());
  /*
   * Check back
   */
  // Check contact ID 10
  key.clear();
  key.id = 10;
  connectionData = acnx.getArticleConnectionData(key, ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.keyData().id, QVariant(10));
  QCOMPARE(connectionData.keyData().articleId(), QVariant(1));
  QVERIFY(connectionData.connectionType() == mdtClConnectionType_t::Terminal);
  QCOMPARE(connectionData.name, QVariant("10"));
  QCOMPARE(connectionData.resistance, QVariant(0.1));
  // Check contact ID 20
  key.clear();
  key.id = 20;
  connectionData = acnx.getArticleConnectionData(key, ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.keyData().id, QVariant(20));
  QCOMPARE(connectionData.keyData().articleId(), QVariant(2));
  QVERIFY(connectionData.connectionType() == mdtClConnectionType_t::Terminal);
  QCOMPARE(connectionData.name, QVariant("20"));
  QCOMPARE(connectionData.resistance, QVariant(0.2));
  // Check contact ID 21
  key.clear();
  key.id = 21;
  connectionData = acnx.getArticleConnectionData(key, ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.keyData().id, QVariant(21));
  QCOMPARE(connectionData.keyData().articleId(), QVariant(2));
  QVERIFY(connectionData.connectionType() == mdtClConnectionType_t::Terminal);
  QCOMPARE(connectionData.name, QVariant("21"));
  QCOMPARE(connectionData.resistance, QVariant(0.21));
  // Check contact ID 22
  key.clear();
  key.id = 22;
  connectionData = acnx.getArticleConnectionData(key, ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.keyData().id, QVariant(22));
  QCOMPARE(connectionData.keyData().articleId(), QVariant(2));
  QVERIFY(connectionData.connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(connectionData.name, QVariant("22"));
  QCOMPARE(connectionData.resistance, QVariant(0.22));

//   mdtClArticle art(0, pvDatabase);
//   QList<QVariant> idList;
//   QList<mdtClArticleConnectionData> connectionDataList;
// 
//   // Add contact ID 10 to article ID 1
//   QVERIFY(connectionData.setup(pvDatabase));
//   connectionData.setValue("Id_PK", 10);
//   connectionData.setValue("Article_Id_FK", 1);
//   connectionData.setValue("ConnectionType_Code_FK", "T");
//   connectionData.setValue("ArticleContactName", "10");
//   connectionData.setValue("Resistance", 0.1);
//   QVERIFY(art.addConnection(connectionData));
  // Add contact ID 20 to article ID 2
//   connectionData.clearValues();
//   connectionData.setValue("Id_PK", 20);
//   connectionData.setValue("Article_Id_FK", 2);
//   connectionData.setValue("ConnectionType_Code_FK", "T");
//   connectionData.setValue("ArticleContactName", "20");
//   connectionData.setValue("Resistance", 0.2);
//   QVERIFY(art.addConnection(connectionData));
  // Add contact ID 21 to article ID 2
//   connectionData.clearValues();
//   connectionData.setValue("Id_PK", 21);
//   connectionData.setValue("Article_Id_FK", 2);
//   connectionData.setValue("ConnectionType_Code_FK", "T");
//   connectionData.setValue("ArticleContactName", "21");
//   connectionData.setValue("Resistance", 0.21);
//   QVERIFY(art.addConnection(connectionData));
  // Add contact ID 22 to article ID 2
//   connectionData.clearValues();
//   connectionData.setValue("Id_PK", 22);
//   connectionData.setValue("Article_Id_FK", 2);
//   connectionData.setValue("ConnectionType_Code_FK", "S");
//   connectionData.setValue("ArticleContactName", "22");
//   connectionData.setValue("Resistance", 0.22);
//   QVERIFY(art.addConnection(connectionData));
  // Check back data
//   connectionData = art.getConnectionData(10, &ok);
//   QVERIFY(ok);
//   QCOMPARE(connectionData.value("Id_PK"), QVariant(10));
//   QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(1));
//   QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("T"));
//   QCOMPARE(connectionData.value("ArticleContactName"), QVariant("10"));
//   QCOMPARE(connectionData.value("Resistance"), QVariant(0.1));
//   connectionData = art.getConnectionData(20, &ok);
//   QVERIFY(ok);
//   QCOMPARE(connectionData.value("Id_PK"), QVariant(20));
//   QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(2));
//   QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("T"));
//   QCOMPARE(connectionData.value("ArticleContactName"), QVariant("20"));
//   QCOMPARE(connectionData.value("Resistance"), QVariant(0.2));
//   connectionData = art.getConnectionData(21, &ok);
//   QVERIFY(ok);
//   QCOMPARE(connectionData.value("Id_PK"), QVariant(21));
//   QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(2));
//   QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("T"));
//   QCOMPARE(connectionData.value("ArticleContactName"), QVariant("21"));
//   QCOMPARE(connectionData.value("Resistance"), QVariant(0.21));
//   // Check connections data list getter method
//   idList.clear();
//   idList << 20 << 21;
//   connectionDataList = art.getConnectionDataListFromConnectionIdList(idList, &ok);
//   QVERIFY(ok);
//   QCOMPARE(connectionDataList.size(), 2);
//   connectionData = connectionDataList.at(0);
//   QCOMPARE(connectionData.value("Id_PK"), QVariant(20));
//   QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(2));
//   QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("T"));
//   QCOMPARE(connectionData.value("ArticleContactName"), QVariant("20"));
//   QCOMPARE(connectionData.value("Resistance"), QVariant(0.2));
//   connectionData = art.getConnectionData(21, &ok);
//   connectionData = connectionDataList.at(1);
//   QCOMPARE(connectionData.value("Id_PK"), QVariant(21));
//   QCOMPARE(connectionData.value("Article_Id_FK"), QVariant(2));
//   QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("T"));
//   QCOMPARE(connectionData.value("ArticleContactName"), QVariant("21"));
//   QCOMPARE(connectionData.value("Resistance"), QVariant(0.21));
}

void mdtCableListTestScenario::removeTestArticleConnections()
{
  mdtClArticleConnection acnx(pvDatabase);
  mdtClArticleConnectionKeyData key;

  key.id = 10;
  QVERIFY(acnx.removeArticleConnection(key));
  key.id = 20;
  QVERIFY(acnx.removeArticleConnection(key));
  key.id = 21;
  QVERIFY(acnx.removeArticleConnection(key));
  key.id = 22;
  QVERIFY(acnx.removeArticleConnection(key));
/*
  mdtClArticle art(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;
*/
//   qDebug() << "Removing article connections ...";
//   
//   QVERIFY(art.removeConnection(10));
//   QVERIFY(art.removeConnection(20));
//   QVERIFY(art.removeConnection(21));
//   QVERIFY(art.removeConnection(22));
//   dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
//   QVERIFY(ok);
//   
//   qDebug() << "All (?) article connections removed";
}

void mdtCableListTestScenario::createTestArticleConnectors()
{
  mdtClArticleConnection acnx(pvDatabase);
  mdtClArticleConnectorKeyData articleConnectorKey;
  mdtClArticleConnectorData articleConnectorData;
  mdtClArticleConnectionKeyData connectionKey;
  mdtClArticleConnectionData connectionData;
  mdtClConnectorKeyData connectorFk;
  bool ok;

  /*
   * Article connector 100
   */
  // Setup connector 100
  connectionData.clear();
  articleConnectorKey.clear();
  articleConnectorKey.id = 100;
  articleConnectorKey.setArticleId(1);
  articleConnectorData.setKeyData(articleConnectorKey);
  articleConnectorData.name = "Article connector 100";
  // Setup connection 15
  connectionData.clear();
  connectionKey.clear();
  connectionKey.id = 15;
  connectionKey.setArticleId(1);
  connectionKey.setConnectionType(mdtClConnectionType_t::Pin);
  connectionData.setKeyData(connectionKey);
  connectionData.name = "Article contact 15";
  articleConnectorData.addConnectionData(connectionData);
  // Add connector and check
  articleConnectorKey = acnx.addArticleConnector(articleConnectorData, true);
  QVERIFY(!articleConnectorKey.isNull());
  articleConnectorData = acnx.getArticleConnectorData(articleConnectorKey, true, ok);
  QVERIFY(ok);
  QCOMPARE(articleConnectorData.keyData().id, QVariant(100));
  QCOMPARE(articleConnectorData.keyData().articleId(), QVariant(1));
  QCOMPARE(articleConnectorData.name, QVariant("Article connector 100"));
  QCOMPARE(articleConnectorData.connectionDataList().size(), 1);
  QCOMPARE(articleConnectorData.connectionDataList().at(0).keyData().id, QVariant(15));
  QVERIFY(articleConnectorData.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Pin);
  QCOMPARE(articleConnectorData.connectionDataList().at(0).name, QVariant("Article contact 15"));
  /*
   * Article connector 200
   */
  // Setup connector 200
  articleConnectorData.clear();
  connectionData.clear();
  articleConnectorKey.clear();
  articleConnectorKey.id = 200;
  articleConnectorKey.setArticleId(2);
  connectorFk.id = 1;
  articleConnectorKey.setConnectorFk(connectorFk);
  articleConnectorData.setKeyData(articleConnectorKey);
  articleConnectorData.name = "Article connector 200";
  // Setup connection 25
  connectionData.clear();
  connectionKey.clear();
  connectionKey.id = 25;
  connectionKey.setArticleId(2);
  connectionKey.setConnectionType(mdtClConnectionType_t::Socket);
  connectionData.setKeyData(connectionKey);
  connectionData.name = "Article contact 25";
  articleConnectorData.addConnectionData(connectionData);
  // Add connector and check
  articleConnectorKey = acnx.addArticleConnector(articleConnectorData, true);
  QVERIFY(!articleConnectorKey.isNull());
  articleConnectorData = acnx.getArticleConnectorData(articleConnectorKey, true, ok);
  QVERIFY(ok);
  QCOMPARE(articleConnectorData.keyData().id, QVariant(200));
  QCOMPARE(articleConnectorData.keyData().articleId(), QVariant(2));
  QVERIFY(articleConnectorData.isBasedOnConnector());
  QCOMPARE(articleConnectorData.keyData().connectorFk().id, QVariant(1));
  QCOMPARE(articleConnectorData.name, QVariant("Article connector 200"));
  QCOMPARE(articleConnectorData.connectionDataList().size(), 1);
  QCOMPARE(articleConnectorData.connectionDataList().at(0).keyData().id, QVariant(25));
  QVERIFY(articleConnectorData.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(articleConnectorData.connectionDataList().at(0).name, QVariant("Article contact 25"));


//   mdtClArticle art(0, pvDatabase);
//   QList<QSqlRecord> dataList;
//   
//   
// 
//   // Setup data
//   QVERIFY(connectorData.setup(pvDatabase, true));
//   QVERIFY(connectionData.setup(pvDatabase));
  /*
   * Article connector 100
   */
//   // Setup connector 100
//   connectorData.setValue("Id_PK", 100);
//   connectorData.setValue("Article_Id_FK", 1);
//   connectorData.setValue("Name", "Article connector 100");
//   // Setup connection 15
//   connectionsData.setValue("Id_PK", 15);
//   connectionsData.setValue("ConnectionType_Code_FK", "P");
//   connectionsData.setValue("ArticleContactName", "Article contact 15");
//   connectorData.addConnectionData(connectionsData);
//   // Add connector and check
//   QVERIFY(art.addConnector(connectorData));
//   connectorData = art.getConnectorData(100, &ok, true, true);
//   QVERIFY(ok);
//   QCOMPARE(connectorData.value("Id_PK"), QVariant(100));
//   QCOMPARE(connectorData.value("Article_Id_FK"), QVariant(1));
//   QCOMPARE(connectorData.value("Name"), QVariant("Article connector 100"));
  /*
   * Article connector 200
   */
//   connectorData.clearValues();
//   // Setup connector 200
//   connectorData.setValue("Id_PK", 200);
//   connectorData.setValue("Article_Id_FK", 2);
//   connectorData.setValue("Connector_Id_FK", 1);
//   connectorData.setValue("Name", "Article connector 200");
//   // Setup connection 15
//   connectionData.setValue("Id_PK", 25);
//   connectionData.setValue("ConnectionType_Code_FK", "S");
//   connectionData.setValue("ArticleContactName", "Article contact 25");
//   connectorData.addConnectionData(connectionData);
//   // Add connector and check
//   QVERIFY(art.addConnector(connectorData));
//   connectorData = art.getConnectorData(200, &ok, true, true);
//   QVERIFY(ok);
//   QCOMPARE(connectorData.value("Id_PK"), QVariant(200));
//   QCOMPARE(connectorData.value("Article_Id_FK"), QVariant(2));
//   QCOMPARE(connectorData.value("Connector_Id_FK"), QVariant(1));
//   QCOMPARE(connectorData.value("Name"), QVariant("Article connector 200"));
}

void mdtCableListTestScenario::removeTestArticleConnectors()
{
  mdtClArticleConnection acnx(pvDatabase);
  mdtClArticleConnectorKeyData key;
  QList<QVariant> dataList;
  bool ok;

  key.id = 100;
  QVERIFY(acnx.removeArticleConnector(key, true));
  key.id = 200;
  QVERIFY(acnx.removeArticleConnector(key, true));
  dataList = acnx.getDataList<QVariant>("SELECT Id_PK FROM ArticleConnector_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);

//   mdtClArticle art(0, pvDatabase);
// 
//   qDebug() << "Removing article connectors ...";
//   
//   QVERIFY(art.removeConnector(100));
//   QVERIFY(art.removeConnector(200));
//   dataList = art.getData("SELECT * FROM ArticleConnector_tbl", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 0);
//   
//   qDebug() << "All article connectors removed";
}

void mdtCableListTestScenario::createTestArticleLinks()
{
  mdtClArticleLink alnk(pvDatabase);
  mdtClArticleLinkPkData pk;
  mdtClArticleLinkData data;
//   mdtClArticleConnectionKeyData acnnKey;

  // Add cable link 10 <--> 20
  data.clear();
  pk.clear();
  pk.connectionStartId = 10;
  pk.connectionEndId = 20;
  data.setPkData(pk);
  data.setLinkType(mdtClLinkType_t::CableLink);
  data.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  QVERIFY(alnk.addLink(data));
  // Add cable link 21 <--> 20
  data.clear();
  pk.clear();
  pk.connectionStartId = 21;
  pk.connectionEndId = 20;
  data.setPkData(pk);
  data.setLinkType(mdtClLinkType_t::CableLink);
  data.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  QVERIFY(alnk.addLink(data));
  // Add internal link 21 <--> 22
  data.clear();
  pk.clear();
  pk.connectionStartId = 21;
  pk.connectionEndId = 22;
  data.setPkData(pk);
  data.setLinkType(mdtClLinkType_t::InternalLink);
  data.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  QVERIFY(alnk.addLink(data));
  
  
  mdtClArticle art(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

//   QVERIFY(art.addCableLink(10, 20, "", 0.0));  
//   QVERIFY(art.addCableLink(21, 20, "", 0.0));
//   QVERIFY(art.addInternalLink(21, 22, "", 0.0));
  dataList = art.getData("SELECT * FROM ArticleLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 3);
  QCOMPARE(dataList.at(0).value("ArticleConnectionStart_Id_FK"), QVariant(10));
  QCOMPARE(dataList.at(0).value("ArticleConnectionEnd_Id_FK"), QVariant(20));
  QCOMPARE(dataList.at(1).value("ArticleConnectionStart_Id_FK"), QVariant(21));
  QCOMPARE(dataList.at(1).value("ArticleConnectionEnd_Id_FK"), QVariant(20));
  QCOMPARE(dataList.at(2).value("ArticleConnectionStart_Id_FK"), QVariant(21));
  QCOMPARE(dataList.at(2).value("ArticleConnectionEnd_Id_FK"), QVariant(22));
}

void mdtCableListTestScenario::removeTestArticleLinks()
{
  mdtClArticle art(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

  qDebug() << "Removing article links ...";
  
  QVERIFY(art.removeLink(10, 20));
  QVERIFY(art.removeLink(21, 20));
  QVERIFY(art.removeLink(21, 22));
  dataList = art.getData("SELECT * FROM ArticleLink_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  
  qDebug() << "All article links removed";
}

void mdtCableListTestScenario::createTestUnits()
{
  mdtClUnit unit(0, pvDatabase);
  mdtSqlRecord data;
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(data.addAllFields("Unit_tbl", pvDatabase));
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

void mdtCableListTestScenario::removeTestUnits()
{
  mdtClUnit unit(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

  // Check that linked tables are empty
  dataList = unit.getDataList<QSqlRecord>("SELECT * FROM VehicleType_Unit_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  dataList = unit.getDataList<QSqlRecord>("SELECT * FROM LinkBeam_UnitStart_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  dataList = unit.getDataList<QSqlRecord>("SELECT * FROM LinkBeam_UnitEnd_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  dataList = unit.getDataList<QSqlRecord>("SELECT * FROM UnitConnector_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  dataList = unit.getDataList<QSqlRecord>("SELECT * FROM UnitConnection_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Remove units
  QVERIFY(unit.removeData("Unit_tbl", "Id_PK", 1000));
  QVERIFY(unit.removeData("Unit_tbl", "Id_PK", 2000));
  QVERIFY(unit.removeData("Unit_tbl", "Id_PK", 2001));
  dataList = unit.getData("SELECT * FROM Unit_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTestScenario::createTestVehicleTypeUnitAssignations()
{
  mdtClUnitVehicleType uvt(0, pvDatabase);
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

void mdtCableListTestScenario::removeTestVehicleTypeUnitAssignations()
{
  mdtClUnitVehicleType uvt(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

  qDebug() << "Removing UVTA ...";
  
  QVERIFY(uvt.removeUnitVehicleAssignment(1000, 1));
  QVERIFY(uvt.removeUnitVehicleAssignment(2000, 2));
  QVERIFY(uvt.removeUnitVehicleAssignment(2001, 2));
  dataList = uvt.getData("SELECT * FROM VehicleType_Unit_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  
  qDebug() << "All UVTA removed";
}

void mdtCableListTestScenario::createTestUnitConnections()
{
  mdtClUnitConnection ucnx(pvDatabase);
  mdtClUnitConnectionKeyData key;
  mdtClUnitConnectionData data;
  bool ok;

  // Add and chek unit connection 10000
  key.clear();
  data.clear();
  key.id = 10000;
  key.setUnitId(1000);
  data.setKeyData(key);
  data.name = "Unit contact 10000";
  key = ucnx.addUnitConnection(data, true);
  QVERIFY(!key.isNull());
  data = ucnx.getUnitConnectionData(key, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().id, QVariant(10000));
  QCOMPARE(data.keyData().unitId(), QVariant(1000));
  QCOMPARE(data.name, QVariant("Unit contact 10000"));

  // Add and chek unit connection 10001
  key.clear();
  data.clear();
  key.id = 10001;
  key.setUnitId(1000);
  data.setKeyData(key);
  data.name = "Unit contact 10001";
  key = ucnx.addUnitConnection(data, true);
  QVERIFY(!key.isNull());
  data = ucnx.getUnitConnectionData(key, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().id, QVariant(10001));
  QCOMPARE(data.keyData().unitId(), QVariant(1000));
  QCOMPARE(data.name, QVariant("Unit contact 10001"));

  
  mdtClUnit unit(0, pvDatabase);
  mdtClUnitConnectionData connectionData;
  QList<QSqlRecord> dataList;
  

  QVERIFY(connectionData.setup(pvDatabase, true));
  // Add and chek unit connection 10000
//   connectionData.setValue("Id_PK", 10000);
//   connectionData.setValue("Unit_Id_FK", 1000);
//   connectionData.setValue("UnitContactName", "Unit contact 10000");
//   QVERIFY(unit.addConnection(connectionData));
//   connectionData = unit.getConnectionData(10000, true, &ok);
//   QVERIFY(ok);
//   QCOMPARE(connectionData.value("Id_PK"), QVariant(10000));
//   QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(1000));
//   QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 10000"));
//   QVERIFY(connectionData.value("ArticleConnection_Id_FK").isNull());

  // Add and chek unit connection 10001
//   connectionData.setValue("Id_PK", 10001);
//   connectionData.setValue("Unit_Id_FK", 1000);
//   connectionData.setValue("UnitContactName", "Unit contact 10001");
//   QVERIFY(unit.addConnection(connectionData));
//   connectionData = unit.getConnectionData(10001, true, &ok);
//   QVERIFY(ok);
//   QCOMPARE(connectionData.value("Id_PK"), QVariant(10001));
//   QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(1000));
//   QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 10001"));
//   QVERIFY(connectionData.value("ArticleConnection_Id_FK").isNull());
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
  /*
   * Add and chek unit connection 20001
   */
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
  /*
   * Add and chek unit connection 20002
   */
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
  /*
   * Add and chek unit connection 20003
   */
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
  /*
   * Add and chek unit connection 20004
   */
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 20004);
  connectionData.setValue("Unit_Id_FK", 2001);
  connectionData.setValue("ArticleConnection_Id_FK", 22);
  connectionData.setValue("UnitContactName", "Unit contact 20004");
  QVERIFY(unit.addConnection(connectionData));
  connectionData = unit.getConnectionData(20004, true, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(20004));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(2001));
  QCOMPARE(connectionData.value("ArticleConnection_Id_FK"), QVariant(22));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 20004"));
  QCOMPARE(connectionData.articleConnectionData().value("Id_PK"), QVariant(22));
  /*
   * Check links
   *  We have a article link from artice connection 21 to 22
   *  Check that link was added
   */
  dataList = unit.getData("SELECT * FROM UnitLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 3);
  QCOMPARE(dataList.at(2).value("UnitConnectionStart_Id_FK"), QVariant(20003));
  QCOMPARE(dataList.at(2).value("UnitConnectionEnd_Id_FK"), QVariant(20004));
  QCOMPARE(dataList.at(2).value("ArticleConnectionStart_Id_FK"), QVariant(21));
  QCOMPARE(dataList.at(2).value("ArticleConnectionEnd_Id_FK"), QVariant(22));
  QCOMPARE(dataList.at(2).value("StartUnit_Id_FK"), QVariant(2001));
  QCOMPARE(dataList.at(2).value("EndUnit_Id_FK"), QVariant(2001));
  // Check that vehicle type links are also added
  dataList = unit.getData("SELECT * FROM VehicleType_Link_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 3);
  QCOMPARE(dataList.at(2).value("VehicleTypeStart_Id_FK"), QVariant(2));
  QCOMPARE(dataList.at(2).value("VehicleTypeEnd_Id_FK"), QVariant(2));
  QCOMPARE(dataList.at(2).value("UnitConnectionStart_Id_FK"), QVariant(20003));
  QCOMPARE(dataList.at(2).value("UnitConnectionEnd_Id_FK"), QVariant(20004));
}

void mdtCableListTestScenario::removeTestUnitConnections()
{
  mdtClUnit unit(0, pvDatabase);
  mdtClLink lnk(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

  qDebug() << "Removing UCNX 10000 ...";
  
  QVERIFY(unit.removeConnection(10000));
  QVERIFY(unit.removeConnection(10001));
  // Check that unit connections where removed
  dataList = unit.getDataList<QSqlRecord>("SELECT * FROM UnitConnection_tbl", ok);
  QVERIFY(ok);
  // If createTestUnitConnections() was never called, we end here
  if(dataList.size() == 0){
    return;
  }
  QCOMPARE(dataList.size(), 5);
  // Get links that was added during connection creation
  dataList = unit.getData("SELECT * FROM UnitLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 3);
  // Remove unit connection 20000
  QVERIFY(unit.removeConnection(20000));
  // Check that link 20001 to 20000 was removed from Link_tbl
  dataList = unit.getData("SELECT * FROM UnitLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  QCOMPARE(dataList.at(0).value("UnitConnectionStart_Id_FK"), QVariant(20003));
  QCOMPARE(dataList.at(0).value("UnitConnectionEnd_Id_FK"), QVariant(20002));
  // Check that link 20001 to 20000 was removed from VehicleType_Link_tbl
  dataList = unit.getData("SELECT * FROM VehicleType_Link_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  QCOMPARE(dataList.at(0).value("UnitConnectionStart_Id_FK"), QVariant(20003));
  QCOMPARE(dataList.at(0).value("UnitConnectionEnd_Id_FK"), QVariant(20002));
  // Remove unit connection 20001
  QVERIFY(unit.removeConnection(20001));
  // Check that no more link was removed
  dataList = unit.getData("SELECT * FROM UnitLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 2);
  // Remove unit connection 20002
  QVERIFY(unit.removeConnection(20002));
  // Check that link 20003 to 20002 was removed from Link_tbl
  dataList = unit.getData("SELECT * FROM UnitLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  // Check that link 20003 to 20002 was removed from VehicleType_Link_tbl
  dataList = unit.getData("SELECT * FROM VehicleType_Link_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  // Remove unit connection 20003
  QVERIFY(unit.removeConnection(20003));
  dataList = unit.getData("SELECT * FROM UnitConnection_tbl", &ok);
  QVERIFY(ok);
  // Check that link 20003 to 20004 was removed from Link_tbl
  dataList = unit.getData("SELECT * FROM UnitLink_view", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Check that link 20003 to 20004 was removed from VehicleType_Link_tbl
  dataList = unit.getData("SELECT * FROM VehicleType_Link_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Remove unit connection 20004
  QVERIFY(unit.removeConnection(20004));
  // Check that unit connections where removed
  dataList = unit.getDataList<QSqlRecord>("SELECT * FROM UnitConnection_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);

  qDebug() << "ALL UNIT connections removed";
}

void mdtCableListTestScenario::createTestUnitConnectors()
{
  mdtClUnit unit(0, pvDatabase);
  mdtClUnitConnectorData connectorData;
  mdtClUnitConnectionData connectionData;
  QList<mdtClUnitConnectionData> connectionDataList;
  QList<QVariant> idList;
  QVariant id;
  bool ok;

  QVERIFY(connectorData.setup(pvDatabase, true, true));
  QVERIFY(connectionData.setup(pvDatabase, true));
  // Check unit connector ID by article connector ID method
  id = unit.getUnitConnectorIdBasedOnArticleConnectorId(QVariant(), 1000, &ok);
  QVERIFY(ok);
  QVERIFY(id.isNull());
  /*
   * Unit connector 100000:
   *  - Id_PK : 100000 , Unit_Id_FK : 1000 , Connector_Id_FK : NULL , ArticleConnector_Id_FK : NULL , Name : Unit connector 100000
   *  -> Connection: Id_PK 10005 , ConnectionType_Code_FK : T , UnitContactName : Unit contact 10005
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
  // Check unit connections part of unit connector
  idList = unit.getConnectionIdListPartOfConnectorId(100000, &ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 1);
  QVERIFY(idList.contains(10005));
  // Check connector ID getter method
  id = unit.getConnectorIdOfConnectionId(10005, &ok);
  QVERIFY(ok);
  QCOMPARE(id, QVariant(100000));

  /*
   * Unit connector 200000:
   *  - Id_PK : 200000 , Unit_Id_FK : 2000 , Connector_Id_FK : 1 , ArticleConnector_Id_FK : 200 , Name : Unit connector 200000
   *  -> Connection: Id_PK 20005 , ArticleConnection_Id_FK : 25 , ConnectionType_Code_FK : S , Name : Unit contact 20005
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
  // Add connector
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
  // Check connection ID 20005
  connectionData = connectorData.connectionData(20005, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(20005));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(2000));
  QCOMPARE(connectionData.value("UnitConnector_Id_FK"), QVariant(200000));
  QCOMPARE(connectionData.value("ArticleConnection_Id_FK"), QVariant(25));
  QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("S"));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 20005"));
  // Check unit connections part of unit connector
  idList = unit.getConnectionIdListPartOfConnectorId(200000, &ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 1);
  QVERIFY(idList.contains(20005));

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
  // We must edit Id_PK , else we will obtain an other (auto increment PK)
  QCOMPARE(connectionDataList.size(), 1);
  connectionDataList[0].setValue("Id_PK", 20005);
  connectionDataList[0].setValue("UnitContactName", "Unit contact 20005"); /// \todo Check if getUnitConnectionDataListFromArticleConnectionIdList() acts correctly
  // Add connection and check
  QVERIFY(unit.addConnections(connectionDataList));
  connectionData = unit.getConnectionData(20005, true, &ok);
  ///connectionData = connectorData.connectionData(20005, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(20005));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(2000));
  QCOMPARE(connectionData.value("UnitConnector_Id_FK"), QVariant(200000));
  QCOMPARE(connectionData.value("ArticleConnection_Id_FK"), QVariant(25));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 20005"));

  /*
   * Unit connector 300000:
   *  - Id_PK : 300000 , Unit_Id_FK : 1000 , Connector_Id_FK : 3 , ArticleConnector_Id_FK : NULL , Name : Unit connector 300000
   *   -> Connection: Id_PK 30005 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : P , Name : Unit contact 30005
   *   -> Connection: Id_PK 30006 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : Unit contact 30006
   */
  // Setup unit connector
  connectorData.clearValues();
  connectorData.setValue("Id_PK", 300000);
  connectorData.setValue("Unit_Id_FK", 1000);
  connectorData.setValue("Connector_Id_FK", 3);
  connectorData.setValue("Name", "Unit connector 300000");
  // Add contacts to unit connector data
  idList.clear();
  idList << 300 << 301;
  QVERIFY(unit.addConnectionDataListFromConnectorContactIdList(connectorData, idList));
  // Add connector
  QVERIFY(unit.addConnector(connectorData));
  // Check connector
  connectorData = unit.getConnectorData(300000, &ok, true, true, true);
  QVERIFY(ok);
  QCOMPARE(connectorData.value("Id_PK"), QVariant(300000));
  QCOMPARE(connectorData.value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(connectorData.value("Name"), QVariant("Unit connector 300000"));
  /*
   * Check connections
   */
  connectionDataList = connectorData.connectionDataList();
  QCOMPARE(connectionDataList.size(), 2);
  // Connection ID 30005
  connectionData = connectionDataList.at(0);
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(connectionData.value("UnitConnector_Id_FK"), QVariant(300000));
  QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("P"));
  // Edit contact ID
  id = connectionData.value("Id_PK");
  connectionData.setValue("Id_PK", 30005);
  QVERIFY(unit.editConnection(id, connectionData));
  connectionData = unit.getConnectionData(30005, false, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(30005));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(connectionData.value("UnitConnector_Id_FK"), QVariant(300000));
  QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("P"));
  // Connection ID 30006
  connectionData = connectionDataList.at(1);
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(connectionData.value("UnitConnector_Id_FK"), QVariant(300000));
  QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("S"));
  // Edit contact ID
  id = connectionData.value("Id_PK");
  connectionData.setValue("Id_PK", 30006);
  QVERIFY(unit.editConnection(id, connectionData));
  connectionData = unit.getConnectionData(30006, false, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(30006));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(connectionData.value("UnitConnector_Id_FK"), QVariant(300000));
  QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("S"));
  /*
   * Unit connector:
   *  - Id_PK : 400000 , Unit_Id_FK : 1000 , Connector_Id_FK : NULL , ArticleConnector_Id_FK : NULL , Name : Unit connector 400000
   *   -> Connection: Id_PK 40005 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : P , Name : A
   *   -> Connection: Id_PK 40006 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : B
   */
  // Setup unit connector
  connectorData.clearValues();
  connectorData.setValue("Id_PK", 400000);
  connectorData.setValue("Unit_Id_FK", 1000);
  connectorData.setValue("Name", "Unit connector 400000");
  // Add connection ID 40005
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 40005);
  connectionData.setValue("ConnectionType_Code_FK", "P");
  connectionData.setValue("UnitContactName", "A");
  connectorData.addConnectionData(connectionData);
  // Add connection ID 40006
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 40006);
  connectionData.setValue("ConnectionType_Code_FK", "S");
  connectionData.setValue("UnitContactName", "B");
  connectorData.addConnectionData(connectionData);
  // Add connector
  QVERIFY(unit.addConnector(connectorData));
  // Check connector
  connectorData = unit.getConnectorData(400000, &ok, true, true, true);
  QVERIFY(ok);
  QCOMPARE(connectorData.value("Id_PK"), QVariant(400000));
  QCOMPARE(connectorData.value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(connectorData.value("Name"), QVariant("Unit connector 400000"));
  /*
   * Check connections
   */
  connectionDataList = connectorData.connectionDataList();
  QCOMPARE(connectionDataList.size(), 2);
  // Check connection ID 40005
  connectionData = connectorData.connectionData(40005, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(40005));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(connectionData.value("UnitConnector_Id_FK"), QVariant(400000));
  QVERIFY(connectionData.value("ArticleConnection_Id_FK").isNull());
  QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("P"));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("A"));
  // Check connection ID 40006
  connectionData = connectorData.connectionData(40006, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(40006));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(connectionData.value("UnitConnector_Id_FK"), QVariant(400000));
  QVERIFY(connectionData.value("ArticleConnection_Id_FK").isNull());
  QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("S"));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("B"));
  // Check unit connections part of unit connector
  idList = unit.getConnectionIdListPartOfConnectorId(400000, &ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 2);
  QVERIFY(idList.contains(40005));
  QVERIFY(idList.contains(40006));
  // Check connector ID getter method
  id = unit.getConnectorIdOfConnectionId(40005, &ok);
  QVERIFY(ok);
  QCOMPARE(id, QVariant(400000));
  id = unit.getConnectorIdOfConnectionId(40006, &ok);
  QVERIFY(ok);
  QCOMPARE(id, QVariant(400000));

  /*
   * Unit connector:
   *  - Id_PK : 500000 , Unit_Id_FK : 2000 , Connector_Id_FK : NULL , ArticleConnector_Id_FK : NULL , Name : Unit connector 500000
   *   -> Connection: Id_PK 50005 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : A
   *   -> Connection: Id_PK 50006 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : B
   */
  // Setup unit connector
  connectorData.clearValues();
  connectorData.setValue("Id_PK", 500000);
  connectorData.setValue("Unit_Id_FK", 2000);
  connectorData.setValue("Name", "Unit connector 500000");
  // Add connection ID 50005
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 50005);
  connectionData.setValue("ConnectionType_Code_FK", "S");
  connectionData.setValue("UnitContactName", "A");
  connectorData.addConnectionData(connectionData);
  // Add connection ID 50006
  connectionData.clearValues();
  connectionData.setValue("Id_PK", 50006);
  connectionData.setValue("ConnectionType_Code_FK", "S");
  connectionData.setValue("UnitContactName", "B");
  connectorData.addConnectionData(connectionData);
  // Add connector
  QVERIFY(unit.addConnector(connectorData));
  // Check connector
  connectorData = unit.getConnectorData(500000, &ok, true, true, true);
  QVERIFY(ok);
  QCOMPARE(connectorData.value("Id_PK"), QVariant(500000));
  QCOMPARE(connectorData.value("Unit_Id_FK"), QVariant(2000));
  QCOMPARE(connectorData.value("Name"), QVariant("Unit connector 500000"));
  /*
   * Check connections
   */
  connectionDataList = connectorData.connectionDataList();
  QCOMPARE(connectionDataList.size(), 2);
  // Check connection ID 50005
  connectionData = connectorData.connectionData(50005, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(50005));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(2000));
  QCOMPARE(connectionData.value("UnitConnector_Id_FK"), QVariant(500000));
  QVERIFY(connectionData.value("ArticleConnection_Id_FK").isNull());
  QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("S"));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("A"));
  // Check connection ID 50006
  connectionData = connectorData.connectionData(50006, &ok);
  QVERIFY(ok);
  QCOMPARE(connectionData.value("Id_PK"), QVariant(50006));
  QCOMPARE(connectionData.value("Unit_Id_FK"), QVariant(2000));
  QCOMPARE(connectionData.value("UnitConnector_Id_FK"), QVariant(500000));
  QVERIFY(connectionData.value("ArticleConnection_Id_FK").isNull());
  QCOMPARE(connectionData.value("ConnectionType_Code_FK"), QVariant("S"));
  QCOMPARE(connectionData.value("UnitContactName"), QVariant("B"));
}

void mdtCableListTestScenario::removeTestUnitConnectors()
{
  mdtClUnit unit(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

  qDebug() << "Removing unit connectors ...";
  
  QVERIFY(unit.removeConnector(100000));
  QVERIFY(unit.removeConnector(200000));
  QVERIFY(unit.removeConnector(300000));
  QVERIFY(unit.removeConnector(400000));
  QVERIFY(unit.removeConnector(500000));
  dataList = unit.getData("SELECT * FROM UnitConnector_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  
  qDebug() << "All unit connectors removed";
}

void mdtCableListTestScenario::createTestLinkVersions()
{
  mdtClLinkVersion lv(pvDatabase);
  mdtClLinkVersionData data;
  bool ok;

  // Create version 0.5
  data.setVersion(0.5);
  QVERIFY(lv.addVersion(data));
  // Create version 1.0
  data.setVersion(1.0);
  QVERIFY(lv.addVersion(data));
  // Create version 1.5
  data.setVersion(1.5);
  QVERIFY(lv.addVersion(data));
  // Create version 2.0
  data.setVersion(2.0);
  QVERIFY(lv.addVersion(data));
  // Check back
  data.setVersion(0.5);
  data = lv.getVersionData(data.pk(), ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  data.setVersion(1.0);
  data = lv.getVersionData(data.pk(), ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  data.setVersion(1.5);
  data = lv.getVersionData(data.pk(), ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  data.setVersion(2.0);
  data = lv.getVersionData(data.pk(), ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
}

void mdtCableListTestScenario::removeTestLinkVersions()
{
  mdtClLinkVersion lv(pvDatabase);
  mdtClLinkVersionPkData pk;
  mdtClLinkVersionData data;
  bool ok;

  // Remove
  pk.versionPk.setValue(500);
  QVERIFY(lv.removeVersion(pk));
  pk.versionPk.setValue(1000);
  QVERIFY(lv.removeVersion(pk));
  pk.versionPk.setValue(1500);
  QVERIFY(lv.removeVersion(pk));
  pk.versionPk.setValue(2000);
  QVERIFY(lv.removeVersion(pk));
  // Check back
  pk.versionPk.setValue(500);
  data = lv.getVersionData(pk, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  pk.versionPk.setValue(1000);
  data = lv.getVersionData(pk, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  pk.versionPk.setValue(1500);
  data = lv.getVersionData(pk, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  pk.versionPk.setValue(2000);
  data = lv.getVersionData(pk, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
}

void mdtCableListTestScenario::createTestLinks()
{
  mdtClLink lnk(0, pvDatabase);
  mdtClUnit unit(0, pvDatabase);
  QList<QVariant> vtStartIdList, vtEndIdList;
  mdtClLinkData linkData;
  mdtClUnitConnectionData startConnectionData, endConnectionData;
  QList<QSqlRecord> dataList;
  QStringList expectedFields;
  bool ok;

  expectedFields << "UnitConnectionStart_Id_FK" << "UnitConnectionEnd_Id_FK" << "ArticleConnectionStart_Id_FK" << "ArticleConnectionEnd_Id_FK";
  QVERIFY(linkData.setup(pvDatabase));

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
  QVERIFY(!lnk.linkExists(10000, 10001, ok));
  QVERIFY(ok);
  QVERIFY(lnk.addLink(linkData));
  QVERIFY(lnk.linkExists(10000, 10001, ok));
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
  QVERIFY(!lnk.linkExists(10001, 20000, ok));
  QVERIFY(ok);
  QVERIFY(lnk.addLink(linkData));
  QVERIFY(lnk.linkExists(10001, 20000, ok));
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
  linkData = lnk.getLinkData(10001, 20000, true, true, ok);
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
  /*
   * Add link:
   *  - UnitConnectionStart_Id_FK : 30005 , UnitConnectionEnd_Id_FK : 40005 , Identification : Link 30005<->40005 , LinkType_Code_FK : CABLELINK , LinkDirection_Code_FK : BID
   *   -> VehicleTypeStart_Id_FK : 1 , VehicleTypeEnd_Id_FK : 1
   */
  startConnectionData = unit.getConnectionData(30005, false, &ok);
  QVERIFY(ok);
  endConnectionData = unit.getConnectionData(40005, false, &ok);
  QVERIFY(ok);
  // Setup vehicle link data
  vtStartIdList.clear();
  vtEndIdList.clear();
  vtStartIdList.append(1);
  vtEndIdList.append(1);
  // Setup link data
  linkData.setConnectionData(startConnectionData, endConnectionData);
  linkData.setValue("Identification", "Link 30005<->40005");
  linkData.setValue("LinkType_Code_FK", "CABLELINK");
  linkData.setValue("LinkDirection_Code_FK", "BID");
  QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
  // Add link
  QVERIFY(!lnk.linkExists(30005, 40005, ok));
  QVERIFY(ok);
  QVERIFY(lnk.addLink(linkData));
  QVERIFY(lnk.linkExists(30005, 40005, ok));
  QVERIFY(ok);
  // Check back
  linkData = lnk.getLinkData(30005, 40005, true, true, ok);
  QVERIFY(ok);
  QCOMPARE(linkData.value("Identification"), QVariant("Link 30005<->40005"));
  QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(30005));
  QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(40005));
  QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(30005));
  QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(40005));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(1));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId(), QVariant(30005));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId(), QVariant(40005));

  /*
   * Add link:
   *  - UnitConnectionStart_Id_FK : 40005 , UnitConnectionEnd_Id_FK : 50005 , Identification : Link 40005<->50005 , LinkType_Code_FK : CABLELINK , LinkDirection_Code_FK : BID
   *   -> VehicleTypeStart_Id_FK : 1 , VehicleTypeEnd_Id_FK : 1
   */
  startConnectionData = unit.getConnectionData(40005, false, &ok);
  QVERIFY(ok);
  endConnectionData = unit.getConnectionData(50005, false, &ok);
  QVERIFY(ok);
  // Setup vehicle link data
  vtStartIdList.clear();
  vtEndIdList.clear();
  vtStartIdList.append(1);
  vtEndIdList.append(1);
  // Setup link data
  linkData.setConnectionData(startConnectionData, endConnectionData);
  linkData.setValue("Identification", "Link 40005<->50005");
  linkData.setValue("LinkType_Code_FK", "CABLELINK");
  linkData.setValue("LinkDirection_Code_FK", "BID");
  QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
  // Add link
  QVERIFY(!lnk.linkExists(40005, 50005, ok));
  QVERIFY(ok);
  QVERIFY(lnk.addLink(linkData));
  QVERIFY(lnk.linkExists(40005, 50005, ok));
  QVERIFY(ok);
  // Check back
  linkData = lnk.getLinkData(40005, 50005, true, true, ok);
  QVERIFY(ok);
  QCOMPARE(linkData.value("Identification"), QVariant("Link 40005<->50005"));
  QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(40005));
  QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(50005));
  QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(40005));
  QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(50005));
  QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(1));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId(), QVariant(40005));
  QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId(), QVariant(50005));
}

void mdtCableListTestScenario::removeTestLinks()
{
  mdtClLink lnk(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(lnk.removeLink(10000, 10001));
  QVERIFY(lnk.removeLink(10001, 20000));
  QVERIFY(lnk.removeLink(30005, 40005));
  QVERIFY(lnk.removeLink(40005, 50005));
  dataList = lnk.getData("SELECT * FROM VehicleType_Link_tbl", &ok);
  QVERIFY(ok);
  QVERIFY( (dataList.size() == 3)||(dataList.size() == 0) );  // 3 links added during unit connection creation, must be tested by removeTestUnitConnections()
  ///QCOMPARE(dataList.size(), 3); // 3 links added during unit connection creation, must be tested by removeTestUnitConnections()
  dataList = lnk.getData("SELECT * FROM Link_tbl", &ok);
  QVERIFY(ok);
  QVERIFY( (dataList.size() == 3)||(dataList.size() == 0) );  // 3 links added during unit connection creation, must be tested by removeTestUnitConnections()
  ///QCOMPARE(dataList.size(), 3); // 3 links added during unit connection creation, must be tested by removeTestUnitConnections()
}
