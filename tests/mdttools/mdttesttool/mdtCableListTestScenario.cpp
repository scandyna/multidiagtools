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
#include "mdtClVehicleTypeLink.h"
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
  createTestLinkVersions();
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
  mdtClConnectorPkData connectorPk;
  mdtClConnectorData connectorData;
  mdtClConnectorContactKeyData contactKey;
  mdtClConnectorContactData contactData;
  bool ok;

  // Add connector 1
  connectorPk.id = 1;
  connectorData.setPk(connectorPk);
  connectorPk = cnr.addConnector(connectorData, true);
  QCOMPARE(connectorPk.id, QVariant(1));
  // Check back
  connectorData = cnr.getConnectorData(connectorPk, true, ok);
  QVERIFY(ok);
  QCOMPARE(connectorData.pk().id, QVariant(1));
  // Add connector 2
  connectorData.clear();
  connectorPk.clear();
  connectorPk.id = 2;
  connectorData.setPk(connectorPk);
  connectorPk = cnr.addConnector(connectorData, true);
  QCOMPARE(connectorPk.id, QVariant(2));
  // Check back
  connectorData = cnr.getConnectorData(connectorPk, true, ok);
  QVERIFY(ok);
  QCOMPARE(connectorData.pk().id, QVariant(2));
  /*
   * Create connect 3
   *  - Id_PK : 3
   *   -> Contact : Id_PK : 300 (, Connector_Id_FK : 3) , ConnectionType_Code_FK : P
   *   -> Contact : Id_PK : 301 (, Connector_Id_FK : 3) , ConnectionType_Code_FK : S
   */
  connectorPk.clear();
  connectorData.clear();
  // Setup connector data
  connectorPk.id = 3;
  connectorData.setPk(connectorPk);
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
  connectorPk = cnr.addConnector(connectorData, true);
  QCOMPARE(connectorPk.id, QVariant(3));
  // Check back
  connectorData = cnr.getConnectorData(connectorPk, true, ok);
  QVERIFY(ok);
  QCOMPARE(connectorData.pk().id, QVariant(3));
  QCOMPARE(connectorData.contactDataList().size(), 2);
  QCOMPARE(connectorData.contactDataList().at(0).keyData().id, QVariant(300));
  QCOMPARE(connectorData.contactDataList().at(0).keyData().connectorFk().id, QVariant(3));
  QVERIFY(connectorData.contactDataList().at(0).connectionType() == mdtClConnectionType_t::Pin);
  QCOMPARE(connectorData.contactDataList().at(1).keyData().id, QVariant(301));
  QCOMPARE(connectorData.contactDataList().at(1).keyData().connectorFk().id, QVariant(3));
  QVERIFY(connectorData.contactDataList().at(1).connectionType() == mdtClConnectionType_t::Socket);
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
}

void mdtCableListTestScenario::createTestArticleConnectors()
{
  mdtClArticleConnection acnx(pvDatabase);
  mdtClArticleConnectorKeyData articleConnectorKey;
  mdtClArticleConnectorData articleConnectorData;
  mdtClArticleConnectionKeyData connectionKey;
  mdtClArticleConnectionData connectionData;
  mdtClConnectorPkData connectorFk;
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
  mdtClArticleLink alnk(pvDatabase);
  mdtClArticleLinkPkData pk;

  pk.connectionStartId = 10;
  pk.connectionEndId = 20;
  QVERIFY(alnk.removeLink(pk));
  pk.connectionStartId = 21;
  pk.connectionEndId = 20;
  QVERIFY(alnk.removeLink(pk));
  pk.connectionStartId = 21;
  pk.connectionEndId = 22;
  QVERIFY(alnk.removeLink(pk));
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
  mdtClLink lnk(pvDatabase);
  mdtClUnitConnectionPkData pk;
  mdtClUnitConnectionKeyData key;
  mdtClUnitConnectionData data;
  mdtClArticleConnectionKeyData acnxFk;
  mdtClLinkPkData linkPk;
  mdtClLinkData linkData;
  QList<QSqlRecord> dataList;
  bool ok;

  // Add and chek unit connection 10000
  key.clear();
  data.clear();
  pk.id = 10000;
  key.pk = pk;
  key.setUnitId(1000);
  data.setKeyData(key);
  data.setConnectionType(mdtClConnectionType_t::Terminal);
  data.name = "Unit contact 10000";
  pk = ucnx.addUnitConnection(data, true);
  QVERIFY(!pk.isNull());
  data = ucnx.getUnitConnectionData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().pk.id, QVariant(10000));
  QCOMPARE(data.keyData().unitId(), QVariant(1000));
  QCOMPARE(data.name, QVariant("Unit contact 10000"));

  // Add and chek unit connection 10001
  key.clear();
  data.clear();
  pk.id = 10001;
  key.pk = pk;
  key.setUnitId(1000);
  data.setKeyData(key);
  data.setConnectionType(mdtClConnectionType_t::Terminal);
  data.name = "Unit contact 10001";
  pk = ucnx.addUnitConnection(data, true);
  QVERIFY(!pk.isNull());
  data = ucnx.getUnitConnectionData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().pk.id, QVariant(10001));
  QCOMPARE(data.keyData().unitId(), QVariant(1000));
  QCOMPARE(data.name, QVariant("Unit contact 10001"));

  /*
   * Add and chek unit connection 20000
   */
  // Add connection and check
  key.clear();
  data.clear();
  acnxFk.clear();
  acnxFk.id = 20;
  acnxFk.setArticleId(2);
  acnxFk.setConnectionType(mdtClConnectionType_t::Terminal);
  pk.id = 20000;
  key.pk = pk;
  key.setUnitId(2000);
  key.setArticleConnectionFk(acnxFk);
  data.setKeyData(key);
  data.name = "Unit contact 20000";
  pk = ucnx.addUnitConnection(data, true);
  QVERIFY(!pk.isNull());
  data = ucnx.getUnitConnectionData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().pk.id, QVariant(20000));
  QCOMPARE(data.keyData().unitId(), QVariant(2000));
  QVERIFY(data.isBasedOnArticleConnection());
  QCOMPARE(data.keyData().articleConnectionFk().articleId(), QVariant(2));
  QCOMPARE(data.name, QVariant("Unit contact 20000"));
  /*
   * Check links
   *  We have a article link from artice connection 21 to 20
   *  Check that no link was added now
   */
  dataList = ucnx.getDataList<QSqlRecord>("SELECT * FROM UnitLink_view", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  /*
   * Add and chek unit connection 20001
   */
  // Add connection and check
  key.clear();
  data.clear();
  acnxFk.clear();
  acnxFk.id = 21;
  acnxFk.setArticleId(2);
  acnxFk.setConnectionType(mdtClConnectionType_t::Terminal);
  pk.id = 20001;
  key.pk = pk;
  key.setUnitId(2000);
  key.setArticleConnectionFk(acnxFk);
  data.setKeyData(key);
  data.name = "Unit contact 20001";
  pk = ucnx.addUnitConnection(data, true);
  QVERIFY(!pk.isNull());
  data = ucnx.getUnitConnectionData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().pk.id, QVariant(20001));
  QCOMPARE(data.keyData().unitId(), QVariant(2000));
  QVERIFY(data.isBasedOnArticleConnection());
  QCOMPARE(data.name, QVariant("Unit contact 20001"));
  /*
   * Check links
   *  We have a article link from artice connection 21 to 20
   *  Check that link was added
   */
  linkPk.connectionStartId = 20001;
  linkPk.connectionEndId = 20000;
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(!linkData.isNull());
  QCOMPARE(linkData.keyData().articleLinkFk().connectionStartId, QVariant(21));
  QCOMPARE(linkData.keyData().articleLinkFk().connectionEndId, QVariant(20));
  /*
   * Add and chek unit connection 20002
   */
  // Add connection and check
  key.clear();
  data.clear();
  acnxFk.clear();
  acnxFk.id = 20;
  acnxFk.setArticleId(2);
  acnxFk.setConnectionType(mdtClConnectionType_t::Terminal);
  pk.id = 20002;
  key.pk = pk;
  key.setUnitId(2001);
  key.setArticleConnectionFk(acnxFk);
  data.setKeyData(key);
  data.name = "Unit contact 20002";
  pk = ucnx.addUnitConnection(data, true);
  QVERIFY(!pk.isNull());
  data = ucnx.getUnitConnectionData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().pk.id, QVariant(20002));
  QCOMPARE(data.keyData().unitId(), QVariant(2001));
  QVERIFY(data.isBasedOnArticleConnection());
  QCOMPARE(data.name, QVariant("Unit contact 20002"));
  /*
   * Check links
   *  We have a article link from artice connection 21 to 20
   *  Check that no more link was added now
   */
  dataList = ucnx.getDataList<QSqlRecord>("SELECT * FROM UnitLink_view", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  /*
   * Add and chek unit connection 20003
   */
  // Add connection and check
  key.clear();
  data.clear();
  acnxFk.clear();
  acnxFk.id = 21;
  acnxFk.setArticleId(2);
  acnxFk.setConnectionType(mdtClConnectionType_t::Terminal);
  pk.id = 20003;
  key.pk = pk;
  key.setUnitId(2001);
  key.setArticleConnectionFk(acnxFk);
  data.setKeyData(key);
  data.name = "Unit contact 20003";
  pk = ucnx.addUnitConnection(data, true);
  QVERIFY(!pk.isNull());
  data = ucnx.getUnitConnectionData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().pk.id, QVariant(20003));
  QCOMPARE(data.keyData().unitId(), QVariant(2001));
  QVERIFY(data.isBasedOnArticleConnection());
  QCOMPARE(data.name, QVariant("Unit contact 20003"));
  /*
   * Check links
   *  We have a article link from artice connection 21 to 20
   *  Check that link was added
   */
  linkPk.connectionStartId = 20003;
  linkPk.connectionEndId = 20002;
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(!linkData.isNull());
  QCOMPARE(linkData.keyData().articleLinkFk().connectionStartId, QVariant(21));
  QCOMPARE(linkData.keyData().articleLinkFk().connectionEndId, QVariant(20));
  /*
   * Add and chek unit connection 20004
   */
  // Add connection and check
  key.clear();
  data.clear();
  acnxFk.clear();
  acnxFk.id = 22;
  acnxFk.setArticleId(2);
  acnxFk.setConnectionType(mdtClConnectionType_t::Terminal);
  pk.id = 20004;
  key.pk = pk;
  key.setUnitId(2001);
  key.setArticleConnectionFk(acnxFk);
  data.setKeyData(key);
  data.name = "Unit contact 20004";
  pk = ucnx.addUnitConnection(data, true);
  QVERIFY(!pk.isNull());
  data = ucnx.getUnitConnectionData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().pk.id, QVariant(20004));
  QCOMPARE(data.keyData().unitId(), QVariant(2001));
  QVERIFY(data.isBasedOnArticleConnection());
  QCOMPARE(data.name, QVariant("Unit contact 20004"));
  /*
   * Check links
   *  We have a article link from artice connection 21 to 22
   *  Check that link was added
   */
  linkPk.connectionStartId = 20003;
  linkPk.connectionEndId = 20004;
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(!linkData.isNull());
  QCOMPARE(linkData.keyData().articleLinkFk().connectionStartId, QVariant(21));
  QCOMPARE(linkData.keyData().articleLinkFk().connectionEndId, QVariant(22));
}

void mdtCableListTestScenario::removeTestUnitConnections()
{
  mdtClUnitConnection ucnx(pvDatabase);
  mdtClLink lnk(pvDatabase);
  mdtClUnitConnectionPkData ucnxPk;
  mdtClLinkPkData linkPk;

  // Remove created links
  linkPk.connectionStartId = 20001;
  linkPk.connectionEndId = 20000;
  linkPk.versionFk.versionPk = 100; /// \todo Update once link creation is fixed
  linkPk.modificationFk.setModification(mdtClModification_t::New); /// \todo Update once link creation is fixed
  QVERIFY(lnk.removeLink(linkPk, true));
  linkPk.connectionStartId = 20003;
  linkPk.connectionEndId = 20002;
  QVERIFY(lnk.removeLink(linkPk, true));
  linkPk.connectionStartId = 20003;
  linkPk.connectionEndId = 20004;
  QVERIFY(lnk.removeLink(linkPk, true));
  
  bool ok;
  QList<QSqlRecord> reclist = ucnx.getDataList<QSqlRecord>("SELECT * FROM Link_tbl", ok);

  // Remove created unit connections
  ucnxPk.id = 10000;
  QVERIFY(ucnx.removeUnitConnection(ucnxPk));
  ucnxPk.id = 10001;
  QVERIFY(ucnx.removeUnitConnection(ucnxPk));
  ucnxPk.id = 20000;
  QVERIFY(ucnx.removeUnitConnection(ucnxPk));
  ucnxPk.id = 20001;
  QVERIFY(ucnx.removeUnitConnection(ucnxPk));
  ucnxPk.id = 20002;
  QVERIFY(ucnx.removeUnitConnection(ucnxPk));
  ucnxPk.id = 20003;
  QVERIFY(ucnx.removeUnitConnection(ucnxPk));
  ucnxPk.id = 20004;
  QVERIFY(ucnx.removeUnitConnection(ucnxPk));
}

void mdtCableListTestScenario::createTestUnitConnectors()
{
  mdtClUnitConnection ucnx(pvDatabase);
  mdtClUnitConnectorPkData pk;
  mdtClUnitConnectorKeyData key;
  mdtClUnitConnectorData data;
  mdtClConnectorPkData connectorFk;
  mdtClUnitConnectionKeyData unitConnectionKey;
  mdtClUnitConnectionData unitConnectionsData;
  mdtClArticleConnectorKeyData articleConnectorFk;
  mdtClArticleConnectionKeyData articleConnectionFk;
  bool ok;

  /*
   * Unit connector 100000:
   *  - Id_PK : 100000 , Unit_Id_FK : 1000 , Connector_Id_FK : NULL , ArticleConnector_Id_FK : NULL , Name : Unit connector 100000
   *  -> Connection: Id_PK 10005 , ConnectionType_Code_FK : T , UnitContactName : Unit contact 10005
   */
  // Setup unit connector key
  key.clear();
  key.pk.id = 100000;
  key.setUnitId(1000);
  // Setup unit connector data
  data.clear();
  data.setKeyData(key);
  data.name = "Unit connector 100000";
  // Setup unit connection data and add to unit connector
  unitConnectionsData.clear();
  unitConnectionKey.clear();
  unitConnectionKey.pk.id = 10005;
  unitConnectionKey.setUnitId(1000);
  unitConnectionKey.setUnitConnectorFk(key);
  unitConnectionKey.setConnectionType(mdtClConnectionType_t::Terminal);
  unitConnectionsData.setKeyData(unitConnectionKey);
  unitConnectionsData.name = "Unit contact 10005";
  data.addConnectionData(unitConnectionsData);
  // Add to database
  pk = ucnx.addUnitConnector(data, true);
  QVERIFY(!pk.isNull());
  // Get data back and check
  data = ucnx.getUnitConnectorData(pk, true, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(1000));
  QCOMPARE(data.name, QVariant("Unit connector 100000"));
  QCOMPARE(data.connectionDataList().size(), 1);
  // Check unit connection A
  QVERIFY(!data.connectionDataList().at(0).isNull());
  QCOMPARE(data.connectionDataList().at(0).keyData().unitId(), QVariant(1000));
  QVERIFY(data.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Terminal);
  QCOMPARE(data.connectionDataList().at(0).name, QVariant("Unit contact 10005"));
  /*
   * Unit connector 200000:
   *  - Id_PK : 200000 , Unit_Id_FK : 2000 , Connector_Id_FK : 1 , ArticleConnector_Id_FK : 200 , Name : Unit connector 200000
   *  -> Connection: Id_PK 20005 , ArticleConnection_Id_FK : 25 , ConnectionType_Code_FK : S , Name : Unit contact 20005
   */
  // Setup article connector FK
  articleConnectorFk.clear();
  articleConnectorFk.id = 200;
  articleConnectorFk.setArticleId(2);
  // Setup unit connector key
  key.pk.id = 200000;
  key.setUnitId(2000);
  key.setArticleConnectorFk(articleConnectorFk);
  // Setup unit connector data
  data.clear();
  data.setKeyData(key);
  data.name = "Unit connector 200000";
  // Setup article connection key data
  articleConnectionFk.clear();
  articleConnectionFk.id = 25;
  articleConnectionFk.setArticleId(2);
  articleConnectionFk.setArticleConnectorFk(articleConnectorFk);
  articleConnectionFk.setConnectionType(mdtClConnectionType_t::Socket);
  // Setup unit connection key
  unitConnectionKey.clear();
  unitConnectionKey.setArticleConnectionFk(articleConnectionFk);
  // Setup unit connection data and add to unit connector
  unitConnectionsData.clear();
  unitConnectionsData.setKeyData(unitConnectionKey);
  unitConnectionsData.name = "Unit contact 20005";
  data.addConnectionData(unitConnectionsData);
  // Add to database
  pk = ucnx.addUnitConnector(data, true);
  QVERIFY(!pk.isNull());
  // Get data back and check
  data = ucnx.getUnitConnectorData(pk, true, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(2000));
  QCOMPARE(data.name, QVariant("Unit connector 200000"));
  QVERIFY(data.isBasedOnConnector());
  QVERIFY(data.isBasedOnArticleConnector());
  QCOMPARE(data.connectionDataList().size(), 1);
  // Check unit connection
  QVERIFY(!data.connectionDataList().at(0).isNull());
  QVERIFY(data.connectionDataList().at(0).isPartOfUnitConnector());
  QVERIFY(data.connectionDataList().at(0).isBasedOnArticleConnection());
  QVERIFY(data.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(data.connectionDataList().at(0).name, QVariant("Unit contact 20005"));
  /*
   * Unit connector 300000:
   *  - Id_PK : 300000 , Unit_Id_FK : 1000 , Connector_Id_FK : 3 , ArticleConnector_Id_FK : NULL , Name : Unit connector 300000
   *   -> Connection: Id_PK 30005 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : P , Name : Unit contact 30005
   *   -> Connection: Id_PK 30006 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : Unit contact 30006
   */
  // Setup connector FK
  connectorFk.clear();
  connectorFk.id = 3;
  // Setup unit connector key
  key.clear();
  key.pk.id = 300000;
  key.setUnitId(1000);
  key.setConnectorFk(connectorFk);
  // Setup unit connector data
  data.clear();
  data.setKeyData(key);
  data.name = "Unit connector 300000";
  // Setup unit connection data and add to unit connector
  unitConnectionsData.clear();
  unitConnectionKey.clear();
  unitConnectionKey.pk.id = 30005;
  unitConnectionKey.setUnitId(1000);
  unitConnectionKey.setUnitConnectorFk(key);
  unitConnectionKey.setConnectionType(mdtClConnectionType_t::Pin);
  unitConnectionsData.setKeyData(unitConnectionKey);
  unitConnectionsData.name = "A";
  data.addConnectionData(unitConnectionsData);
  // Setup unit connection data and add to unit connector
  unitConnectionsData.clear();
  unitConnectionKey.clear();
  unitConnectionKey.pk.id = 30006;
  unitConnectionKey.setUnitId(1000);
  unitConnectionKey.setUnitConnectorFk(key);
  unitConnectionKey.setConnectionType(mdtClConnectionType_t::Socket);
  unitConnectionsData.setKeyData(unitConnectionKey);
  unitConnectionsData.name = "B";
  data.addConnectionData(unitConnectionsData);
  // Add to database
  pk = ucnx.addUnitConnector(data, true);
  QVERIFY(!pk.isNull());
  // Get data back and check
  data = ucnx.getUnitConnectorData(pk, true, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(1000));
  QVERIFY(data.isBasedOnConnector());
  QCOMPARE(data.name, QVariant("Unit connector 300000"));
  QCOMPARE(data.connectionDataList().size(), 2);
  // Check unit connection A
  QVERIFY(!data.connectionDataList().at(0).isNull());
  QVERIFY(data.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Pin);
  QCOMPARE(data.connectionDataList().at(0).name, QVariant("A"));
  // Check unit connection B
  QVERIFY(!data.connectionDataList().at(1).isNull());
  QVERIFY(data.connectionDataList().at(1).connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(data.connectionDataList().at(1).name, QVariant("B"));
  /*
   * Unit connector:
   *  - Id_PK : 400000 , Unit_Id_FK : 1000 , Connector_Id_FK : NULL , ArticleConnector_Id_FK : NULL , Name : Unit connector 400000
   *   -> Connection: Id_PK 40005 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : P , Name : A
   *   -> Connection: Id_PK 40006 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : B
   */
  // Setup unit connector key
  key.clear();
  key.pk.id = 400000;
  key.setUnitId(1000);
  // Setup unit connector data
  data.clear();
  data.setKeyData(key);
  data.name = "Unit connector 400000";
  // Setup unit connection data and add to unit connector
  unitConnectionsData.clear();
  unitConnectionKey.clear();
  unitConnectionKey.pk.id = 40005;
  unitConnectionKey.setUnitId(1000);
  unitConnectionKey.setUnitConnectorFk(key);
  unitConnectionKey.setConnectionType(mdtClConnectionType_t::Pin);
  unitConnectionsData.setKeyData(unitConnectionKey);
  unitConnectionsData.name = "A";
  data.addConnectionData(unitConnectionsData);
  // Setup unit connection data and add to unit connector
  unitConnectionsData.clear();
  unitConnectionKey.clear();
  unitConnectionKey.pk.id = 40006;
  unitConnectionKey.setUnitId(1000);
  unitConnectionKey.setUnitConnectorFk(key);
  unitConnectionKey.setConnectionType(mdtClConnectionType_t::Socket);
  unitConnectionsData.setKeyData(unitConnectionKey);
  unitConnectionsData.name = "B";
  data.addConnectionData(unitConnectionsData);
  // Add to database
  pk = ucnx.addUnitConnector(data, true);
  QVERIFY(!pk.isNull());
  // Get data back and check
  data = ucnx.getUnitConnectorData(pk, true, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(1000));
  QCOMPARE(data.name, QVariant("Unit connector 400000"));
  QCOMPARE(data.connectionDataList().size(), 2);
  // Check unit connection A
  QVERIFY(!data.connectionDataList().at(0).isNull());
  QVERIFY(data.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Pin);
  QCOMPARE(data.connectionDataList().at(0).name, QVariant("A"));
  // Check unit connection B
  QVERIFY(!data.connectionDataList().at(1).isNull());
  QVERIFY(data.connectionDataList().at(1).connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(data.connectionDataList().at(1).name, QVariant("B"));
  /*
   * Unit connector:
   *  - Id_PK : 500000 , Unit_Id_FK : 2000 , Connector_Id_FK : NULL , ArticleConnector_Id_FK : NULL , Name : Unit connector 500000
   *   -> Connection: Id_PK 50005 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : A
   *   -> Connection: Id_PK 50006 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : B
   */
  // Setup unit connector key
  key.clear();
  key.pk.id = 500000;
  key.setUnitId(2000);
  // Setup unit connector data
  data.clear();
  data.setKeyData(key);
  data.name = "Unit connector 500000";
  // Setup unit connection data and add to unit connector
  unitConnectionsData.clear();
  unitConnectionKey.clear();
  unitConnectionKey.pk.id = 50005;
  unitConnectionKey.setUnitId(2000);
  unitConnectionKey.setUnitConnectorFk(key);
  unitConnectionKey.setConnectionType(mdtClConnectionType_t::Socket);
  unitConnectionsData.setKeyData(unitConnectionKey);
  unitConnectionsData.name = "A";
  data.addConnectionData(unitConnectionsData);
  // Setup unit connection data and add to unit connector
  unitConnectionsData.clear();
  unitConnectionKey.clear();
  unitConnectionKey.pk.id = 50006;
  unitConnectionKey.setUnitId(2000);
  unitConnectionKey.setUnitConnectorFk(key);
  unitConnectionKey.setConnectionType(mdtClConnectionType_t::Socket);
  unitConnectionsData.setKeyData(unitConnectionKey);
  unitConnectionsData.name = "B";
  data.addConnectionData(unitConnectionsData);
  // Add to database
  pk = ucnx.addUnitConnector(data, true);
  QVERIFY(!pk.isNull());
  // Get data back and check
  data = ucnx.getUnitConnectorData(pk, true, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(2000));
  QCOMPARE(data.name, QVariant("Unit connector 500000"));
  QCOMPARE(data.connectionDataList().size(), 2);
  // Check unit connection A
  QVERIFY(!data.connectionDataList().at(0).isNull());
  QVERIFY(data.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(data.connectionDataList().at(0).name, QVariant("A"));
  // Check unit connection B
  QVERIFY(!data.connectionDataList().at(1).isNull());
  QVERIFY(data.connectionDataList().at(1).connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(data.connectionDataList().at(1).name, QVariant("B"));
}

void mdtCableListTestScenario::removeTestUnitConnectors()
{
  mdtClUnitConnection ucnx(pvDatabase);
  mdtClUnitConnectorPkData pk;

  pk.id = 100000;
  QVERIFY(ucnx.removeUnitConnector(pk, true));
  pk.id = 200000;
  QVERIFY(ucnx.removeUnitConnector(pk, true));
  pk.id = 300000;
  QVERIFY(ucnx.removeUnitConnector(pk, true));
  pk.id = 400000;
  QVERIFY(ucnx.removeUnitConnector(pk, true));
  pk.id = 500000;
  QVERIFY(ucnx.removeUnitConnector(pk, true));
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
  mdtClLink lnk(pvDatabase);
  mdtClVehicleTypeLink vtl(pvDatabase);
  mdtClLinkData linkData;
  mdtClLinkPkData linkPk;
  mdtClVehicleTypeStartEndKeyData vt;
  QList<mdtClVehicleTypeStartEndKeyData> vtList;
  mdtClLinkVersionPkData linkVersionFk;
  mdtClModificationPkData modificationFk;
  ///mdtClLinkModificationKeyData linkModificationFk;
  bool ok;

  /*
   * Add link with unit connections not based on article connections
   */
  // Setup vehicle types list data
  vtList.clear();
  vt.setVehicleTypeStartId(1);
  vt.setVehicleTypeEndId(1);
  vtList << vt;
  // Setup link PK data
  linkPk.connectionStartId = 10000;
  linkPk.connectionEndId = 10001;
  linkPk.versionFk.versionPk = 500;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  // Setup modification data
//   linkVersionFk.versionPk = 500;
//   modificationFk.setModification(mdtClModification_t::New);
//   linkModificationFk.setLinkFk(linkPk);
//   linkModificationFk.setLinkVersionFk(linkVersionFk);
//   linkModificationFk.setModificationFk(modificationFk);
  // Setup link data
  linkData.setPk(linkPk);
  linkData.setLinkType(mdtClLinkType_t::CableLink);
  linkData.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  linkData.identification = "Link 10000<->10001";
  // Add link
  QVERIFY(!lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  ///QVERIFY(lnk.addLink(linkData, linkModificationFk, vtList, true));
  QVERIFY(lnk.addLink(linkData, vtList, true));
  QVERIFY(lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  // Get back and check - Link_tbl part
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(!linkData.isNull());
  QCOMPARE(linkData.pk().connectionStartId, QVariant(10000));
  QCOMPARE(linkData.pk().connectionEndId, QVariant(10001));
  QVERIFY(linkData.keyData().linkTypeFk().type() == mdtClLinkType_t::CableLink);
  QVERIFY(linkData.keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(linkData.keyData().articleLinkFk().isNull());
  QVERIFY(linkData.keyData().wireId().isNull());
  QCOMPARE(linkData.identification, QVariant("Link 10000<->10001"));
  // Check VehicleType_Link_tbl part
  vtList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtList.size(), 1);
  QCOMPARE(vtList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtList.at(0).vehicleTypeEndId(), QVariant(1));
  /*
   * Add link with unit connection based on article connection
   *  If link should based on a article link, it was allready added
   *  during unit connection insersion.
   *  So, link created here is NOT based on article link.
   */
  // Setup vehicle types list data
  vtList.clear();
  vt.setVehicleTypeStartId(1);
  vt.setVehicleTypeEndId(2);
  vtList << vt;
  // Setup link PK data
  linkPk.connectionStartId = 10001;
  linkPk.connectionEndId = 20000;
  linkPk.versionFk.versionPk = 500;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  // Setup modification data
//   linkVersionFk.versionPk = 500;
//   modificationFk.setModification(mdtClModification_t::New);
//   linkModificationFk.setLinkFk(linkPk);
//   linkModificationFk.setLinkVersionFk(linkVersionFk);
//   linkModificationFk.setModificationFk(modificationFk);
  // Setup link data
  linkData.setPk(linkPk);
  linkData.setLinkType(mdtClLinkType_t::CableLink);
  linkData.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  linkData.identification = "Link 10001<->20000";
  // Add link
  QVERIFY(!lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  ///QVERIFY(lnk.addLink(linkData, linkModificationFk, vtList, true));
  QVERIFY(lnk.addLink(linkData, vtList, true));
  QVERIFY(lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  // Get back and check - Link_tbl part
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(!linkData.isNull());
  QCOMPARE(linkData.pk().connectionStartId, QVariant(10001));
  QCOMPARE(linkData.pk().connectionEndId, QVariant(20000));
  QVERIFY(linkData.keyData().linkTypeFk().type() == mdtClLinkType_t::CableLink);
  QVERIFY(linkData.keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(linkData.keyData().articleLinkFk().isNull());
  QVERIFY(linkData.keyData().wireId().isNull());
  QCOMPARE(linkData.identification, QVariant("Link 10001<->20000"));
  // Check VehicleType_Link_tbl part
  vtList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtList.size(), 1);
  QCOMPARE(vtList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtList.at(0).vehicleTypeEndId(), QVariant(2));
  /*
   * Add link:
   *  - UnitConnectionStart_Id_FK : 30005 , UnitConnectionEnd_Id_FK : 40005 , Identification : Link 30005<->40005 , LinkType_Code_FK : CABLELINK , LinkDirection_Code_FK : BID
   *   -> Version_FK : 500 , Modification_Code_FK : NEW
   *   -> VehicleTypeStart_Id_FK : 1 , VehicleTypeEnd_Id_FK : 1
   */
  // Setup vehicle types list data
  vtList.clear();
  vt.setVehicleTypeStartId(1);
  vt.setVehicleTypeEndId(1);
  vtList << vt;
  // Setup link PK data
  linkPk.connectionStartId = 30005;
  linkPk.connectionEndId = 40005;
  linkPk.versionFk.versionPk = 500;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  // Setup modification data
//   linkVersionFk.versionPk = 500;
//   modificationFk.setModification(mdtClModification_t::New);
//   linkModificationFk.setLinkFk(linkPk);
//   linkModificationFk.setLinkVersionFk(linkVersionFk);
//   linkModificationFk.setModificationFk(modificationFk);
  // Setup link data
  linkData.setPk(linkPk);
  linkData.setLinkType(mdtClLinkType_t::CableLink);
  linkData.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  linkData.identification = "Link 30005<->40005";
  // Add link
  QVERIFY(!lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  ///QVERIFY(lnk.addLink(linkData, linkModificationFk, vtList, true));
  QVERIFY(lnk.addLink(linkData, vtList, true));
  QVERIFY(lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  // Get back and check - Link_tbl part
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(!linkData.isNull());
  QCOMPARE(linkData.pk().connectionStartId, QVariant(30005));
  QCOMPARE(linkData.pk().connectionEndId, QVariant(40005));
  QVERIFY(linkData.keyData().linkTypeFk().type() == mdtClLinkType_t::CableLink);
  QVERIFY(linkData.keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(linkData.keyData().articleLinkFk().isNull());
  QVERIFY(linkData.keyData().wireId().isNull());
  QCOMPARE(linkData.identification, QVariant("Link 30005<->40005"));
  /// \todo Check in LinkModification_tbl ?
  // Check VehicleType_Link_tbl part
  vtList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtList.size(), 1);
  QCOMPARE(vtList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtList.at(0).vehicleTypeEndId(), QVariant(1));
  /*
   * Add link:
   *  - UnitConnectionStart_Id_FK : 40005 , UnitConnectionEnd_Id_FK : 50005 , Identification : Link 40005<->50005 , LinkType_Code_FK : CABLELINK , LinkDirection_Code_FK : BID
   *   -> Version_FK : 500 , Modification_Code_FK : NEW
   *   -> VehicleTypeStart_Id_FK : 1 , VehicleTypeEnd_Id_FK : 1
   */
  // Setup vehicle types list data
  vtList.clear();
  vt.setVehicleTypeStartId(1);
  vt.setVehicleTypeEndId(1);
  vtList << vt;
  // Setup link PK data
  linkPk.connectionStartId = 40005;
  linkPk.connectionEndId = 50005;
  linkPk.versionFk.versionPk = 500;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  // Setup modification data
//   linkVersionFk.versionPk = 500;
//   modificationFk.setModification(mdtClModification_t::New);
//   linkModificationFk.setLinkFk(linkPk);
//   linkModificationFk.setLinkVersionFk(linkVersionFk);
//   linkModificationFk.setModificationFk(modificationFk);
  // Setup link data
  linkData.setPk(linkPk);
  linkData.setLinkType(mdtClLinkType_t::CableLink);
  linkData.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  linkData.identification = "Link 40005<->50005";
  // Add link
  QVERIFY(!lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  ///QVERIFY(lnk.addLink(linkData, linkModificationFk, vtList, true));
  QVERIFY(lnk.addLink(linkData, vtList, true));
  QVERIFY(lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  // Get back and check - Link_tbl part
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(!linkData.isNull());
  QCOMPARE(linkData.pk().connectionStartId, QVariant(40005));
  QCOMPARE(linkData.pk().connectionEndId, QVariant(50005));
  QVERIFY(linkData.keyData().linkTypeFk().type() == mdtClLinkType_t::CableLink);
  QVERIFY(linkData.keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(linkData.keyData().articleLinkFk().isNull());
  QVERIFY(linkData.keyData().wireId().isNull());
  QCOMPARE(linkData.identification, QVariant("Link 40005<->50005"));
  /// \todo Check in LinkModification_tbl ?
  // Check VehicleType_Link_tbl part
  vtList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtList.size(), 1);
  QCOMPARE(vtList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtList.at(0).vehicleTypeEndId(), QVariant(1));
}

void mdtCableListTestScenario::removeTestLinks()
{
  mdtClLink lnk(pvDatabase);
  mdtClLinkPkData linkPk;
  QList<QSqlRecord> dataList;

  linkPk.connectionStartId = 10000;
  linkPk.connectionEndId = 10001;
  linkPk.versionFk.versionPk = 500;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  QVERIFY(lnk.removeLink(linkPk, true));
  linkPk.connectionStartId = 10001;
  linkPk.connectionEndId = 20000;
  QVERIFY(lnk.removeLink(linkPk, true));
  linkPk.connectionStartId = 30005;
  linkPk.connectionEndId = 40005;
  QVERIFY(lnk.removeLink(linkPk, true));
  linkPk.connectionStartId = 40005;
  linkPk.connectionEndId = 50005;
  QVERIFY(lnk.removeLink(linkPk, true));
}
