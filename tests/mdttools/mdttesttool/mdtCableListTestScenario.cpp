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
#include "mdtCableListTestScenario.h"
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
  mdtClArticle art(0, pvDatabase);
  mdtClConnectorData connectorData;
  mdtSqlRecord contactData;
  QList<QSqlRecord> dataList;
  bool ok;

  // Add connector 1
  QVERIFY(connectorData.setup(pvDatabase));
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
  /*
   * Create connect 3
   *  - Id_PK : 3
   *   -> Contact : Id_PK : 300 (, Connector_Id_FK : 3) , ConnectionType_Code_FK : P
   *   -> Contact : Id_PK : 301 (, Connector_Id_FK : 3) , ConnectionType_Code_FK : S
   */
  connectorData.clearValues();
  connectorData.setValue("Id_PK", 3);
  QVERIFY(art.addRecord(connectorData, "Connector_tbl"));
  // Setup contact data
  QVERIFY(contactData.addAllFields("ConnectorContact_tbl", pvDatabase));
  /*
   * Add contact ID 300
   */
  contactData.setValue("Id_PK", 300);
  contactData.setValue("ConnectionType_Code_FK", "P");
  connectorData.addContactData(contactData);
  // Get contact data and check
  contactData = connectorData.contactData(300, &ok);
  QVERIFY(ok);
  QCOMPARE(contactData.value("Id_PK"), QVariant(300));
  QCOMPARE(contactData.value("Connector_Id_FK"), QVariant(3));
  QCOMPARE(contactData.value("ConnectionType_Code_FK"), QVariant("P"));
  // Add contact to db
  QVERIFY(art.addRecord(contactData, "ConnectorContact_tbl"));
  // Check back
  dataList = art.getData("SELECT * FROM ConnectorContact_tbl WHERE Id_PK = 300", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Id_PK"), QVariant(300));
  QCOMPARE(dataList.at(0).value("Connector_Id_FK"), QVariant(3));
  QCOMPARE(dataList.at(0).value("ConnectionType_Code_FK"), QVariant("P"));
  /*
   * Add contact ID 301
   */
  contactData.setValue("Id_PK", 301);
  contactData.setValue("ConnectionType_Code_FK", "S");
  connectorData.addContactData(contactData);
  // Get contact data and check
  contactData = connectorData.contactData(301, &ok);
  QVERIFY(ok);
  QCOMPARE(contactData.value("Id_PK"), QVariant(301));
  QCOMPARE(contactData.value("Connector_Id_FK"), QVariant(3));
  QCOMPARE(contactData.value("ConnectionType_Code_FK"), QVariant("S"));
  // Add contact to db
  QVERIFY(art.addRecord(contactData, "ConnectorContact_tbl"));
  // Check back
  dataList = art.getData("SELECT * FROM ConnectorContact_tbl WHERE Id_PK = 301", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Id_PK"), QVariant(301));
  QCOMPARE(dataList.at(0).value("Connector_Id_FK"), QVariant(3));
  QCOMPARE(dataList.at(0).value("ConnectionType_Code_FK"), QVariant("S"));
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
  QCOMPARE(dataList.size(), 1);
  QVERIFY(art.removeData("Article_tbl", "Id_PK", 2));
  dataList = art.getData("SELECT * FROM Article_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTestScenario::createTestArticleConnections()
{
  mdtClArticle art(0, pvDatabase);
  mdtClArticleConnectionData connectionData;
  QList<QVariant> idList;
  QList<mdtClArticleConnectionData> connectionDataList;
  bool ok;

  // Add contact ID 10 to article ID 1
  QVERIFY(connectionData.setup(pvDatabase));
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

void mdtCableListTestScenario::removeTestArticleConnections()
{
  mdtClArticle art(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(art.removeConnection(10));
  QVERIFY(art.removeConnection(20));
  QVERIFY(art.removeConnection(21));
  dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
  QVERIFY(ok);
}

void mdtCableListTestScenario::createTestArticleConnectors()
{
  mdtClArticle art(0, pvDatabase);
  QList<QSqlRecord> dataList;
  mdtClArticleConnectorData connectorData;
  mdtClArticleConnectionData connectionsData;
  bool ok;

  // Setup data
  QVERIFY(connectorData.setup(pvDatabase, true));
  QVERIFY(connectionsData.setup(pvDatabase));
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

void mdtCableListTestScenario::removeTestArticleConnectors()
{
  mdtClArticle art(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(art.removeConnector(100));
  QVERIFY(art.removeConnector(200));
  dataList = art.getData("SELECT * FROM ArticleConnector_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTestScenario::createTestArticleLinks()
{
  mdtClArticle art(0, pvDatabase);
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

void mdtCableListTestScenario::removeTestArticleLinks()
{
  mdtClArticle art(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(art.removeLink(10, 20));
  QVERIFY(art.removeLink(21, 20));
  dataList = art.getData("SELECT * FROM ArticleLink_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
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

  QVERIFY(uvt.removeUnitVehicleAssignment(1000, 1));
  QVERIFY(uvt.removeUnitVehicleAssignment(2000, 2));
  QVERIFY(uvt.removeUnitVehicleAssignment(2001, 2));
  dataList = uvt.getData("SELECT * FROM VehicleType_Unit_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTestScenario::createTestUnitConnections()
{
  mdtClUnit unit(0, pvDatabase);
  mdtClUnitConnectionData connectionData;
  QList<QSqlRecord> dataList;
  bool ok;

  QVERIFY(connectionData.setup(pvDatabase, true));
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

void mdtCableListTestScenario::removeTestUnitConnections()
{
  mdtClUnit unit(0, pvDatabase);
  mdtClLink lnk(0, pvDatabase);
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

  /*
   * Unit connector 200000:
   *  - Id_PK : 200000 , Unit_Id_FK : 2000 , Connector_Id_FK : 1 , ArticleConnector_Id_FK : 200 , Name : Unit connector 200000
   *  -> Connection: Id_PK 20005 , ArticleConnection_Id_FK : 25 , ConnectionType_Code_FK : S , Name : Unit contact 20005
   *  -> Connection: Id_PK 20006 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : P , Name : A
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

  QVERIFY(unit.removeConnector(100000));
  QVERIFY(unit.removeConnector(200000));
  QVERIFY(unit.removeConnector(300000));
  QVERIFY(unit.removeConnector(400000));
  QVERIFY(unit.removeConnector(500000));
  dataList = unit.getData("SELECT * FROM UnitConnector_tbl", &ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
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

void mdtCableListTestScenario::removeTestLinks()
{
  mdtClLink lnk(0, pvDatabase);
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
