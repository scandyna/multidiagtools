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
#include "mdtCableListTest.h"
#include "mdtApplication.h"
#include "mdtClApplicationWidgets.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlRecord.h"
#include "mdtTtBase.h"
#include "mdtClArticle.h"
#include "mdtClUnit.h"
#include "mdtClUnitConnection.h"
#include "mdtClConnectorData.h"
#include "mdtClArticleConnectorData.h"
#include "mdtClArticleConnectorData.h"
#include "mdtClArticleLink.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClUnitConnectorData.h"
#include "mdtClUnitVehicleType.h"
#include "mdtClLink.h"
#include "mdtClLinkData.h"
#include "mdtClVehicleTypeLink.h"
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
  QFile::remove(pvDbFileInfo.absoluteFilePath());
}

void mdtCableListTest::applicationWidgetsTest()
{
  mdtClApplicationWidgets::setDatabase(pvDatabaseManager.database());
  QCOMPARE(mdtClApplicationWidgets::database().connectionName(), pvDatabaseManager.database().connectionName());
  QCOMPARE(mdtClApplicationWidgets::database().connectionName(), pvDatabaseManager.database().connectionName());
  qDebug() << "Cnn: " << mdtClApplicationWidgets::database().connectionName();
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
  dataList = art.getDataList<QSqlRecord>("SELECT * FROM Article_tbl", ok);
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
  dataList = art.getDataList<QSqlRecord>("SELECT * FROM Article_tbl", ok);
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
  dataList = art.getDataList<QSqlRecord>("SELECT * FROM ArticleComponent_view", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Assign article ID 2 as component of article ID 1
  QVERIFY(art.addComponent(1, 2, 5, "pce"));
  // Check back
  dataList = art.getDataList<QSqlRecord>("SELECT * FROM ArticleComponent_view", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  QCOMPARE(data.value("Article_Id_PK"), QVariant(1));
  QCOMPARE(data.value("Component_Id_PK"), QVariant(2));
  QCOMPARE(data.value("ComponentQty"), QVariant(5));
  QCOMPARE(data.value("ComponentQtyUnit"), QVariant("pce"));
  // Try to remove wrong component assignation - Note: possibly ok that no error is returned
  art.removeComponent(5, 3);
  dataList = art.getDataList<QSqlRecord>("SELECT * FROM ArticleComponent_view", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  // Remove component assignation
  QVERIFY(art.removeComponent(1, 2));
  dataList = art.getDataList<QSqlRecord>("SELECT * FROM ArticleComponent_view", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);

  /*
   * Article connection
   */

//   QVERIFY(connectionData.setup(pvDatabaseManager.database()));
//   ///QVERIFY(record.addAllFields("ArticleConnection_tbl", pvDatabaseManager.database()));
//   // Initially we have no connections
//   dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 0);
//   // Add a connection to article ID 1
//   connectionData.setValue("Id_PK", 1);
//   connectionData.setValue("Article_Id_FK", 1);
//   connectionData.setValue("ArticleContactName", "Contact 1");
//   connectionData.setValue("FunctionEN", "EN: Contact 1");
//   QVERIFY(art.addConnection(connectionData));
//   // Check back
//   dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 1);
//   data = dataList.at(0);
//   QCOMPARE(data.value("Id_PK"), QVariant(1));
//   QCOMPARE(data.value("Article_Id_FK"), QVariant(1));
//   QCOMPARE(data.value("ArticleContactName"), QVariant("Contact 1"));
//   QVERIFY(data.value("ArticleConnector_Id_FK").isNull());
//   QCOMPARE(data.value("FunctionEN"), QVariant("EN: Contact 1"));
//   // Try to delete article ID 1 - Must fail
//   QVERIFY(!art.removeData("Article_tbl", "Id_PK", 1));
//   dataList = art.getData("SELECT * FROM Article_tbl", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 2);
//   // Remove connection
//   QVERIFY(art.removeConnection(1));
//   // Check back
//   dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 0);

  /*
   * Free article connector + connections
   */

  // Setup connector data for article ID 1
//   connectorData.clear();
//   QVERIFY(connectorData.setup(pvDatabaseManager.database(), false));
//   connectorData.setValue("Id_PK", 1);
//   connectorData.setValue("Article_Id_FK", 1);
//   connectorData.setValue("Name", "X1");
//   // Setup connection data
//   ///connectionData.clear();
//   ///QVERIFY(connectionData.setup(pvDatabaseManager.database()));
//   connectionData.clearValues();
//   for(int i = 0; i < 11; ++i){
//     connectionData.clearValues();
//     connectionData.setValue("Id_PK", i+1);
//     connectionData.setValue("ArticleContactName", QString::number(i));
//     connectionData.setValue("FunctionEN", "Contact " + QString::number(i));
//     connectorData.addConnectionData(connectionData);
//   }
//   // Add connector
//   QVERIFY(art.addConnector(connectorData));
//   // Check back
//   dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 11);
//   for(int i = 0; i < dataList.size(); ++i){
//     data = dataList.at(i);
//     QCOMPARE(data.value("Id_PK"), QVariant(i+1));
//     QCOMPARE(data.value("Article_Id_FK"), QVariant(1));
//     QCOMPARE(data.value("ArticleConnector_Id_FK"), QVariant(1));
//     QCOMPARE(data.value("ArticleConnectorName"), QVariant("X1"));
//     QCOMPARE(data.value("ArticleContactName"), QVariant(QString::number(i)));
//     QCOMPARE(data.value("FunctionEN"), QVariant("Contact " + QString::number(i)));
//   }

  /*
   * Article connector based on a Connector
   */

//   // Setup a Connector
//   baseConnectorData.clear();
//   QVERIFY(baseConnectorData.setup(pvDatabaseManager.database()));
//   baseConnectorData.setValue("Id_PK", 1);
//   QVERIFY(art.addRecord(baseConnectorData, "Connector_tbl"));
//   // Add contacts to this connector
//   record.clear();
//   QVERIFY(record.addAllFields("ConnectorContact_tbl", pvDatabaseManager.database()));
//   record.setValue("Id_PK", 1);
//   record.setValue("Connector_Id_FK", 1);
//   record.setValue("ConnectionType_Code_FK", "P");
//   record.setValue("Name", "A");
//   QVERIFY(art.addRecord(record, "ConnectorContact_tbl"));
//   record.setValue("Id_PK", 2);
//   record.setValue("Connector_Id_FK", 1);
//   record.setValue("ConnectionType_Code_FK", "P");
//   record.setValue("Name", "B");
//   QVERIFY(art.addRecord(record, "ConnectorContact_tbl"));
//   record.setValue("Id_PK", 3);
//   record.setValue("Connector_Id_FK", 1);
//   record.setValue("ConnectionType_Code_FK", "S");
//   record.setValue("Name", "C");
//   QVERIFY(art.addRecord(record, "ConnectorContact_tbl"));
//   // Setup article connector data for article ID 1
//   connectorData.clear();
//   QVERIFY(connectorData.setup(pvDatabaseManager.database(), false));
//   connectorData.setValue("Id_PK", 2);
//   connectorData.setValue("Article_Id_FK", 1);
//   connectorData.setValue("Connector_Id_FK", 1);
//   connectorData.setValue("Name", "X2");
//   // Setup connection data
//   connectorContactIdList.clear();
//   connectorContactIdList << 1 << 2 << 3;
//   QCOMPARE(connectorData.connectionDataList().size(), 0);
//   QVERIFY(art.addConnectionDataListFromConnectorContactIdList(connectorData, connectorContactIdList));
//   QCOMPARE(connectorData.connectionDataList().size(), 3);
//   // Add connector
//   QVERIFY(art.addConnector(connectorData));
//   // Check back
//   dataList = art.getData("SELECT * FROM ArticleConnection_view WHERE Connector_Id_FK = 1", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 3);
//   data = dataList.at(0);
//   QCOMPARE(data.value("Connector_Id_FK"), QVariant(1));
//   QCOMPARE(data.value("Article_Id_FK"), QVariant(1));
//   QCOMPARE(data.value("ConnectionType_Code_FK"), QVariant("P"));
//   QCOMPARE(data.value("ArticleConnectorName"), QVariant("X2"));
//   data = dataList.at(1);
//   QCOMPARE(data.value("Connector_Id_FK"), QVariant(1));
//   QCOMPARE(data.value("Article_Id_FK"), QVariant(1));
//   QCOMPARE(data.value("ConnectionType_Code_FK"), QVariant("P"));
//   QCOMPARE(data.value("ArticleConnectorName"), QVariant("X2"));
//   data = dataList.at(2);
//   QCOMPARE(data.value("Connector_Id_FK"), QVariant(1));
//   QCOMPARE(data.value("Article_Id_FK"), QVariant(1));
//   QCOMPARE(data.value("ConnectionType_Code_FK"), QVariant("S"));
//   QCOMPARE(data.value("ArticleConnectorName"), QVariant("X2"));

  /*
   * Article link + link path
   */

  // Initially, we have no article link
//   dataList = art.getData("SELECT * FROM ArticleLink_view", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 0);
//   // Add a link from CNN ID 1 to 2
//   QVERIFY(art.addCableLink(1, 2, "1-2", 0.1));
//   dataList = art.getData("SELECT * FROM ArticleLink_view", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 1);
//   data = dataList.at(0);
//   QCOMPARE(data.value("ArticleConnectionStart_Id_FK"), QVariant(1));
//   QCOMPARE(data.value("ArticleConnectionEnd_Id_FK"), QVariant(2));
//   QCOMPARE(data.value("LinkType_Code_FK"), QVariant("CABLELINK"));
//   QCOMPARE(data.value("LinkDirection_Code_FK"), QVariant("BID"));
//   QCOMPARE(data.value("Identification"), QVariant("1-2"));
//   QCOMPARE(data.value("Resistance"), QVariant(0.1));
//   QCOMPARE(data.value("ValueUnit"), QVariant("Ohm"));
//   // Check that we cannot remove CNN ID 1
// //   QVERIFY(!art.removeConnection(1));
//   dataList = art.getData("SELECT * FROM ArticleConnection_view WHERE Id_PK = 1", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 1);
//   // Remove link
//   QVERIFY(art.removeLink(1, 2));
//   dataList = art.getData("SELECT * FROM ArticleLink_view", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 0);

  /*
   * Remove data
   */

//   // Remove connector ID 1
//   QVERIFY(art.removeConnector(1));
//   dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 3);
//   dataList = art.getData("SELECT * FROM ArticleConnector_tbl", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 1);
//   // Remove connector ID 2
//   QVERIFY(art.removeConnector(2));
//   dataList = art.getData("SELECT * FROM ArticleConnection_view", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 0);
//   dataList = art.getData("SELECT * FROM ArticleConnector_tbl", &ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 0);
  // Remove article
  QVERIFY(art.removeData("Article_tbl", "Id_PK", 1));
  dataList = art.getDataList<QSqlRecord>("SELECT * FROM Article_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QVERIFY(art.removeData("Article_tbl", "Id_PK", 2));
  dataList = art.getDataList<QSqlRecord>("SELECT * FROM Article_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Remove base connector
  QVERIFY(art.removeData("ConnectorContact_tbl", "Id_PK", 1));
  QVERIFY(art.removeData("ConnectorContact_tbl", "Id_PK", 2));
  QVERIFY(art.removeData("ConnectorContact_tbl", "Id_PK", 3));
  QVERIFY(art.removeData("Connector_tbl", "Id_PK", 1));
  dataList = art.getDataList<QSqlRecord>("SELECT * FROM Connector_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtCableListTest::unitConnectionUpdateTest()
{
  QFAIL("Not implemented yet");
  
//   mdtClArticle art(0, pvDatabaseManager.database());
//   mdtCableListTestScenario scenario(pvDatabaseManager.database());
//   QList<QSqlRecord> dataList;
//   bool ok;
// 
//   // Create scenario
//   scenario.createTestVehicleTypes();
//   scenario.createTestArticles();
//   scenario.createTestConnectors();
//   scenario.createTestArticleConnections();
//   scenario.createTestArticleLinks();
//   scenario.createTestArticleConnectors();
//   scenario.createTestUnits();
//   scenario.createTestVehicleTypeUnitAssignations();
//   scenario.createTestUnitConnections();

  /*
   * We make update test on Resistance
   */
//   // Check initial Resistance of unit connections
//   dataList = art.getDataList<QSqlRecord>("SELECT * FROM UnitConnection_tbl", ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 7);
//   QCOMPARE(dataList.at(0).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(1).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(2).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(3).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(4).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(5).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(6).value("Resistance"), QVariant(0.0));
//   // Check update - Article connection ID: Null - Must update nothing
//   QVERIFY(!art.updateUnitConnections(QStringList("Resistance"), QVariant()));
//   dataList = art.getDataList<QSqlRecord>("SELECT * FROM UnitConnection_tbl", ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 7);
//   QCOMPARE(dataList.at(0).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(1).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(2).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(3).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(4).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(5).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(6).value("Resistance"), QVariant(0.0));
//   // Check update - Article connection ID: 20
//   QVERIFY(art.updateUnitConnections(QStringList("Resistance"), 20));
//   dataList = art.getDataList<QSqlRecord>("SELECT * FROM UnitConnection_tbl", ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 7);
//   QCOMPARE(dataList.at(0).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(1).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(2).value("Resistance"), QVariant(0.2));
//   QCOMPARE(dataList.at(3).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(4).value("Resistance"), QVariant(0.2));
//   QCOMPARE(dataList.at(5).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(6).value("Resistance"), QVariant(0.0));
//   // Check update - Article connection ID: 21
//   QVERIFY(art.updateUnitConnections(QStringList("Resistance"), 21));
//   dataList = art.getDataList<QSqlRecord>("SELECT * FROM UnitConnection_tbl", ok);
//   QVERIFY(ok);
//   QCOMPARE(dataList.size(), 7);
//   QCOMPARE(dataList.at(0).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(1).value("Resistance"), QVariant(0.0));
//   QCOMPARE(dataList.at(2).value("Resistance"), QVariant(0.2));
//   QCOMPARE(dataList.at(3).value("Resistance"), QVariant(0.21));
//   QCOMPARE(dataList.at(4).value("Resistance"), QVariant(0.2));
//   QCOMPARE(dataList.at(5).value("Resistance"), QVariant(0.21));
//   QCOMPARE(dataList.at(6).value("Resistance"), QVariant(0.0));
}

void mdtCableListTest::unitTest()
{
  mdtClUnit unit(0, pvDatabaseManager.database());
  mdtClUnitConnectorData connectorData;
  mdtCableListTestScenario scenario(pvDatabaseManager.database());

  ///QVERIFY(connectorData.setup(pvDatabaseManager.database(), true, true));
  // Create base structure
  scenario.createTestVehicleTypes();
  scenario.createTestArticles();
  scenario.createTestConnectors();
  scenario.createTestArticleConnections();
  scenario.createTestArticleLinks();
  scenario.createTestArticleConnectors();
  scenario.createTestUnits();
  scenario.createTestVehicleTypeUnitAssignations();
  scenario.createTestUnitConnections();
  scenario.createTestUnitConnectors();

  /*
   * Try to add a unit connector that is based on a article connector
   *  and a base connector that not matches artice connector's base connector - Must fail
   */
  /**
  connectorData.clearValues();
  connectorData.setValue("Unit_Id_FK", 2000);
  connectorData.setValue("Connector_Id_FK", 2);
  connectorData.setValue("ArticleConnector_Id_FK", 200);
  QVERIFY(!unit.addConnector(connectorData));
  */
//   createTestUnitConnectors();

}

// void mdtCableListTest::unitConnectionTest()
// {
//   mdtClUnit unit(0, pvDatabaseManager.database());
//   mdtClLink lnk(0, pvDatabaseManager.database());
//   mdtClUnitConnectionData connectionData;
//   mdtCableListTestScenario scenario(pvDatabaseManager.database());
//   bool ok;
// 
//   // Create base structure
//   scenario.createTestVehicleTypes();
//   scenario.createTestArticles();
//   scenario.createTestConnectors();
//   scenario.createTestArticleConnections();
//   scenario.createTestArticleLinks();
//   scenario.createTestArticleConnectors();
//   scenario.createTestUnits();
//   scenario.createTestVehicleTypeUnitAssignations();
//   scenario.createTestUnitConnections();
// 
//   /*
//    * Check basic connection edition
//    *  - Change unit connection 10000 : set UnitContactName : Edited contact
//    */
//   connectionData = unit.getConnectionData(10000, true, &ok);
//   QVERIFY(ok);
//   connectionData.setValue("UnitContactName", "Edited contact");
//   QVERIFY(unit.editConnection(10000, connectionData));
//   connectionData.clearValues();
//   connectionData = unit.getConnectionData(10000, true, &ok);
//   QVERIFY(ok);
//   QCOMPARE(connectionData.value("UnitContactName"), QVariant("Edited contact"));
//   // Edit back to original values
//   connectionData.setValue("UnitContactName", "Unit contact 10000");
//   QVERIFY(unit.editConnection(10000, connectionData));
//   connectionData.clearValues();
//   connectionData = unit.getConnectionData(10000, true, &ok);
//   QVERIFY(ok);
//   QCOMPARE(connectionData.value("UnitContactName"), QVariant("Unit contact 10000"));
// }

// void mdtCableListTest::mdtClLinkDataTest()
// {
//   mdtClLinkData linkData;
//   mdtClUnitConnectionData startConnectionData, endConnectionData;
//   mdtClVehicleTypeLinkData vtLinkData;
// 
//   QVERIFY(linkData.setup(pvDatabaseManager.database()));
//   QVERIFY(startConnectionData.setup(pvDatabaseManager.database(), true));
//   QVERIFY(endConnectionData.setup(pvDatabaseManager.database(), true));
//   QVERIFY(!linkData.vehicleTypeLinksEdited());
// 
//   // Setup connections data
//   startConnectionData.setValue("Id_PK", 1);
//   endConnectionData.setValue("Id_PK", 2);
//   // Setup vehicle type link data
//   vtLinkData.setVehicleTypeStartId(10);
//   vtLinkData.setVehicleTypeEndId(20);
//   // Set link data
//   linkData.setConnectionData(startConnectionData, endConnectionData);
//   linkData.setValue("Identification", "Link 12");
//   QVERIFY(!linkData.vehicleTypeLinksEdited());
//   linkData.addVehicleTypeLinkData(vtLinkData);
//   QVERIFY(linkData.vehicleTypeLinksEdited());
//   // Check
//   QCOMPARE(linkData.value("Identification"), QVariant("Link 12"));
//   QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(1));
//   QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(1));
//   QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(2));
//   QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(2));
//   QCOMPARE(linkData.value("ArticleConnectionStart_Id_FK"), QVariant(QVariant::Int));
//   QCOMPARE(linkData.value("ArticleConnectionStart_Id_FK"), QVariant(QVariant::Int));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId() , QVariant(10));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId() , QVariant(20));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId() , QVariant(1));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId() , QVariant(2));
//   // Change start/end connection data and check
//   startConnectionData.setValue("Id_PK", 3);
//   endConnectionData.setValue("Id_PK", 4);
//   linkData.setConnectionData(startConnectionData, endConnectionData);
//   QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(3));
//   QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(4));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId() , QVariant(3));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId() , QVariant(4));
//   // Clear
//   linkData.clearValues();
//   QVERIFY(!linkData.vehicleTypeLinksEdited());
//   QCOMPARE(linkData.value("Identification"), QVariant(QVariant::String));
//   QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(QVariant::Int));
//   QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(QVariant::Int));
//   QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(QVariant::Int));
//   QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(QVariant::Int));
//   QCOMPARE(linkData.value("ArticleConnectionStart_Id_FK"), QVariant(QVariant::Int));
//   QCOMPARE(linkData.value("ArticleConnectionStart_Id_FK"), QVariant(QVariant::Int));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 0);
// }

void mdtCableListTest::linkTest()
{
  mdtClLink lnk(pvDatabaseManager.database());
  mdtClUnitVehicleType uvt(pvDatabaseManager.database());
  mdtClVehicleTypeLink vtl(pvDatabaseManager.database());
  mdtCableListTestScenario scenario(pvDatabaseManager.database());
  mdtClLinkData linkData;
  mdtClLinkPkData linkPk;
  mdtClVehicleTypeStartEndKeyData vt;
  QList<mdtClVehicleTypeStartEndKeyData> vtList;
  mdtClLinkVersionPkData linkVersionFk;
  mdtClModificationPkData modificationFk;
  ///mdtClLinkModificationKeyData linkModificationFk;
  bool ok;

  // Create base structure
  scenario.createSenario();
  // Get link data for a non existing link
  linkPk.connectionStartId = 135;
  linkPk.connectionEndId = 45678;
  linkPk.versionFk.versionPk = 500;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(linkData.isNull());
  /*
   * Simple link edition
   *
   *  Edit link 10000<->10001:
   *   -> Identification : Edited link
   */
  // Get link data
  linkPk.connectionStartId = 10000;
  linkPk.connectionEndId = 10001;
  linkPk.versionFk.versionPk = 500;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(!linkData.isNull());
  // Setup modification key (we keep it as in scenario)
  ///linkModificationFk.setLinkFk(linkPk);
  ///linkModificationFk.setLinkVersionFk(linkVersionFk);
  ///linkModificationFk.setModificationFk(modificationFk);
  // Setup expected vehicle types (we keep them as in scenario)
  vtList.clear();
  vt.setVehicleTypeStartId(1);
  vt.setVehicleTypeEndId(1);
  vtList << vt;
  // Update link
  linkData.identification = "Edited link";
  QVERIFY(lnk.updateLink(linkPk, linkData, vtList, true));
  // Get back and check - Link_tbl part
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(!linkData.isNull());
  QCOMPARE(linkData.identification, QVariant("Edited link"));
  // Check VehicleType_Link_tbl part
  vtList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtList.size(), 1);
  QCOMPARE(vtList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtList.at(0).vehicleTypeEndId(), QVariant(1));
  /*
   * Link edition with start/end change
   *
   *  Edit link 10000<->10001:
   *   -> UnitConnectionStart_Id_FK : 10000 , UnitConnectionEnd_Id_FK : 20000 , Identification : Edited link 10000-20000
   */
  // Get link data
  linkPk.connectionStartId = 10000;
  linkPk.connectionEndId = 10001;
  linkPk.versionFk.versionPk = 500;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(!linkData.isNull());
  // Setup modification key (we keep it as in scenario)
//   linkModificationFk.setLinkFk(linkPk);
//   linkVersionFk.versionPk = 500;
//   linkModificationFk.setLinkVersionFk(linkVersionFk);
//   modificationFk.setModification(mdtClModification_t::New);
//   linkModificationFk.setModificationFk(modificationFk);
  // Setup expected vehicle types (we keep them as in scenario)
  vtList.clear();
  vt.setVehicleTypeStartId(1);
  vt.setVehicleTypeEndId(1);
  vtList << vt;
  // Update link
  linkPk.connectionStartId = 10000;
  linkPk.connectionEndId = 20000;
  linkPk.versionFk.versionPk = 500;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  linkData.setPk(linkPk);
  linkData.identification = "Edited link 10000-20000";
  ///linkModificationFk.setLinkFk(linkPk);
  linkPk.connectionStartId = 10000;
  linkPk.connectionEndId = 10001;
  QVERIFY(lnk.updateLink(linkPk, linkData, vtList, true));
  // Get back and check - Link_tbl part
  linkPk.connectionStartId = 10000;
  linkPk.connectionEndId = 20000;
  linkPk.versionFk.versionPk = 500;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(!linkData.isNull());
  QCOMPARE(linkData.identification, QVariant("Edited link 10000-20000"));
  // Check VehicleType_Link_tbl part
  vtList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtList.size(), 1);
  QCOMPARE(vtList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtList.at(0).vehicleTypeEndId(), QVariant(1));
  // Remove link
  linkPk.connectionStartId = 10000;
  linkPk.connectionEndId = 20000;
  QVERIFY(lnk.removeLink(linkPk, true));
  /*
   * Remove unit - vehicle type assignation and check that vehicle type links are removed
   *
   *  Remove Unit_Id_FK : 1000 , VehicleType_Id_FK : 1
   *
   *  Unit with ID 1000 has following connections: 10000, 10001, 10005, 30005, 30006, 40005, 40006
   *  This gives following links: 10000-10001 , 10001-20000 , 30005-40005 , 40005-50005
   *  Finaly, this gives following assignations:
   *   - Link 10000-10001 -> VehicleTypeStart_Id_FK : 1 , VehicleTypeEnd_Id_FK : 1
   *   - Link 10001-20000 -> VehicleTypeStart_Id_FK : 1 , VehicleTypeEnd_Id_FK : 2
   *   - Link 30005-40005 -> VehicleTypeStart_Id_FK : 1 , VehicleTypeEnd_Id_FK : 1
   *   - Link 40005-50005 -> VehicleTypeStart_Id_FK : 1 , VehicleTypeEnd_Id_FK : 1
   */
  /// \todo Add other link-vt assignations that are not related to unit 1000 and check that they are not removed !!!
  // We rebuild the scenario
  scenario.removeScenario();
  scenario.createSenario();
  // Check that we have expected assignations
  linkPk.connectionStartId = 10000;
  linkPk.connectionEndId = 10001;
  linkPk.versionFk.versionPk = 500;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  vtList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtList.size(), 1);
  linkPk.connectionStartId = 10001;
  linkPk.connectionEndId = 20000;
  vtList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtList.size(), 1);
  linkPk.connectionStartId = 30005;
  linkPk.connectionEndId = 40005;
  vtList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtList.size(), 1);
  linkPk.connectionStartId = 40005;
  linkPk.connectionEndId = 50005;
  vtList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtList.size(), 1);
  // Remove unit - vehicle type assignation
  QVERIFY(uvt.removeUnitVehicleAssignment(1000, 1));
  // Check that vehicle type - link assignations where removed
  linkPk.connectionStartId = 10000;
  linkPk.connectionEndId = 10001;
  vtList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtList.size(), 0);
  linkPk.connectionStartId = 10001;
  linkPk.connectionEndId = 20000;
  vtList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtList.size(), 0);
  linkPk.connectionStartId = 30005;
  linkPk.connectionEndId = 40005;
  vtList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtList.size(), 0);
  linkPk.connectionStartId = 40005;
  linkPk.connectionEndId = 50005;
  vtList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtList.size(), 0);


//   mdtClUnit unit(pvDatabaseManager.database());
//   QList<QVariant> vtStartIdList, vtEndIdList;
//   mdtClUnitConnectionData connectionData;
//   QList<mdtClVehicleTypeLinkData> vtLinkDataList;
//   QString sql;

//   QVERIFY(linkData.setup(pvDatabaseManager.database()));

  /*
   * Check vehicle type link data list
   */
//   // Add 1 start vehicle - Case Error (one list empty)
//   vtStartIdList.append(10);
//   QVERIFY(!lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 0);
//   // Add 1 end vehicle - Case 1 start -> 1 end
//   vtEndIdList.append(20);
//   QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(10));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(20));
//   // Add a second start vehicle - Case 2 start -> 1 end
//   vtStartIdList.append(11);
//   QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 2);
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(10));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(20));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(1).vehicleTypeStartId(), QVariant(11));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(1).vehicleTypeEndId(), QVariant(20));
//   // Add a second end vehicle - Case 2*(1 start -> 1 end)
//   vtEndIdList.append(21);
//   QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 2);
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(10));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(20));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(1).vehicleTypeStartId(), QVariant(11));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(1).vehicleTypeEndId(), QVariant(21));
//   // Clear start list - Case Error (one list empty)
//   vtStartIdList.clear();
//   QVERIFY(!lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 0);
//   // Add 1 start vehicle - Case 1 start -> 2 end
//   vtStartIdList.append(12);
//   QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 2);
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(12));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(20));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(1).vehicleTypeStartId(), QVariant(12));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(1).vehicleTypeEndId(), QVariant(21));

  /*
   * Simple link edition
   *
   *  Edit link 10000<->10001:
   *   -> Identification : Edited link
   */
//   linkData = lnk.getLinkData(10000, 10001, true, true, ok);
//   QVERIFY(ok);
//   // Set PK as non generated, so mdtClLink will only update (not remove/add)
//   linkData.setHasValue("UnitConnectionStart_Id_FK", false);
//   linkData.setHasValue("UnitConnectionEnd_Id_FK", false);
//   QVERIFY(!linkData.vehicleTypeLinksEdited());
//   // Update link data
//   linkData.setValue("Identification", "Edited link");
//   QVERIFY(lnk.editLink(10000, 10001, linkData));
//   linkData = lnk.getLinkData(10000, 10001, true, true, ok);
//   QVERIFY(ok);
//   QCOMPARE(linkData.value("Identification"), QVariant("Edited link"));
//   QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(10000));
//   QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(10001));
//   QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(10000));
//   QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(10001));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(1));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(1));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId(), QVariant(10000));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId(), QVariant(10001));
//   // Restore original data
//   linkData.setValue("Identification", "Link 10000<->10001");
//   linkData.setHasValue("UnitConnectionStart_Id_FK", false);
//   linkData.setHasValue("UnitConnectionEnd_Id_FK", false);
//   QVERIFY(!linkData.vehicleTypeLinksEdited());
//   QVERIFY(lnk.editLink(10000, 10001, linkData));
//   linkData = lnk.getLinkData(10000, 10001, true, true, ok);
//   QVERIFY(ok);
//   QCOMPARE(linkData.value("Identification"), QVariant("Link 10000<->10001"));
//   QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(10000));
//   QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(10001));
//   QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(10000));
//   QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(10001));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(1));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(1));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId(), QVariant(10000));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId(), QVariant(10001));
  /*
   * Link edition with start/end change
   *
   *  Edit link 10000<->10001:
   *   -> UnitConnectionStart_Id_FK : 10000 , UnitConnectionEnd_Id_FK : 20000 , Identification : Edited link
   */
//   QVERIFY(!lnk.linkExists(10000, 20000, ok));
//   QVERIFY(ok);
//   linkData = lnk.getLinkData(10000, 10001, true, true, ok);
//   QVERIFY(ok);
//   // Update end connection data
//   connectionData = unit.getConnectionData(20000, false, &ok);
//   QVERIFY(ok);
//   linkData.setEndConnectionData(connectionData);
//   // Update end vehicle type list
//   QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
//   vtStartIdList.clear();
//   vtStartIdList.append(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId());
//   vtEndIdList.clear();
//   vtEndIdList.append(2);
//   QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
//   // Edit link and check
//   QVERIFY(lnk.editLink(10000, 10001, linkData));
//   QVERIFY(lnk.linkExists(10000, 20000, ok));
//   QVERIFY(ok);
//   linkData = lnk.getLinkData(10000, 20000, true, true, ok);
//   QVERIFY(ok);
//   QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(10000));
//   QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(20000));
//   QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(10000));
//   QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(20000));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(1));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(2));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId(), QVariant(10000));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId(), QVariant(20000));
//   /*
//    * Link edition with start/end change (restore to original value)
//    *
//    *  Edit link 10000<->20000:
//    *   -> UnitConnectionStart_Id_FK : 10000 , UnitConnectionEnd_Id_FK : 10001 , Identification : Edited link
//    */
//   QVERIFY(!lnk.linkExists(10000, 10001, ok));
//   QVERIFY(ok);
//   linkData = lnk.getLinkData(10000, 20000, true, true, ok);
//   QVERIFY(ok);
//   // Update end connection data
//   connectionData = unit.getConnectionData(10001, false, &ok);
//   QVERIFY(ok);
//   linkData.setEndConnectionData(connectionData);
//   // Update end vehicle type list
//   QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
//   vtStartIdList.clear();
//   vtStartIdList.append(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId());
//   vtEndIdList.clear();
//   vtEndIdList.append(1);
//   QVERIFY(lnk.buildVehicleTypeLinkDataList(linkData, vtStartIdList, vtEndIdList));
//   // Edit link and check
//   QVERIFY(lnk.editLink(10000, 20000, linkData));
//   QVERIFY(lnk.linkExists(10000, 10001, ok));
//   QVERIFY(ok);
//   linkData = lnk.getLinkData(10000, 10001, true, true, ok);
//   QVERIFY(ok);
//   QCOMPARE(linkData.value("Identification"), QVariant("Link 10000<->10001"));
//   QCOMPARE(linkData.value("UnitConnectionStart_Id_FK"), QVariant(10000));
//   QCOMPARE(linkData.value("UnitConnectionEnd_Id_FK"), QVariant(10001));
//   QCOMPARE(linkData.startConnectionData().value("Id_PK"), QVariant(10000));
//   QCOMPARE(linkData.endConnectionData().value("Id_PK"), QVariant(10001));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().size(), 1);
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeStartId(), QVariant(1));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).vehicleTypeEndId(), QVariant(1));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionStartId(), QVariant(10000));
//   QCOMPARE(linkData.vehicleTypeLinkDataList().at(0).unitConnectionEndId(), QVariant(10001));
  /*
   * Remove unit - vehicle type assignation and check that vehicle type links are removed
   *
   *  Remove Unit_Id_FK : 1000 , VehicleType_Id_FK : 1
   */
  // Check that we have expected vehicle type links: 10000-10001 , 10001-20000 , 30005-40005 , 40005-50005
//   vtLinkDataList = lnk.getVehicleTypeLinkDataByUnitId(1000, &ok);
//   QVERIFY(ok);
//   QCOMPARE(vtLinkDataList.size(), 4);
//   QCOMPARE(vtLinkDataList.at(0).unitConnectionStartId(), QVariant(10000));
//   QCOMPARE(vtLinkDataList.at(0).unitConnectionEndId(), QVariant(10001));
//   QCOMPARE(vtLinkDataList.at(0).vehicleTypeStartId(), QVariant(1));
//   QCOMPARE(vtLinkDataList.at(0).vehicleTypeEndId(), QVariant(1));
// 
//   QCOMPARE(vtLinkDataList.at(1).unitConnectionStartId(), QVariant(30005));
//   QCOMPARE(vtLinkDataList.at(1).unitConnectionEndId(), QVariant(40005));
//   QCOMPARE(vtLinkDataList.at(1).vehicleTypeStartId(), QVariant(1));
//   QCOMPARE(vtLinkDataList.at(1).vehicleTypeEndId(), QVariant(1));
// 
//   QCOMPARE(vtLinkDataList.at(3).unitConnectionStartId(), QVariant(10001));
//   QCOMPARE(vtLinkDataList.at(3).unitConnectionEndId(), QVariant(20000));
//   QCOMPARE(vtLinkDataList.at(3).vehicleTypeStartId(), QVariant(1));
//   QCOMPARE(vtLinkDataList.at(3).vehicleTypeEndId(), QVariant(2));
// 
//   QCOMPARE(vtLinkDataList.at(2).unitConnectionStartId(), QVariant(40005));
//   QCOMPARE(vtLinkDataList.at(2).unitConnectionEndId(), QVariant(50005));
//   QCOMPARE(vtLinkDataList.at(2).vehicleTypeStartId(), QVariant(1));
//   QCOMPARE(vtLinkDataList.at(2).vehicleTypeEndId(), QVariant(1));
// 
//   // Remove assignation
//   QVERIFY(uvt.removeUnitVehicleAssignment(1000, 1));
//   // Check that links no longer exists
//   vtLinkDataList = lnk.getVehicleTypeLinkDataByUnitId(1000, &ok);
//   QVERIFY(ok);
//   QCOMPARE(vtLinkDataList.size(), 0);
  
  /// \todo Units with more than 1 vehicle type association - 1-1
  
  /// \todo Units with more than 1 vehicle type association - 1-n
  
  /// \todo Links with U CNN based on ART CNN

}

void mdtCableListTest::linkUpdateFromArticleLinkTest()
{
  mdtClLink lnk(pvDatabaseManager.database());
  mdtClLinkPkData linkPk;
  mdtClArticleLink alnk(pvDatabaseManager.database());
  mdtClArticleLinkPkData aLnkPk;
  
  mdtClArticle art(0, pvDatabaseManager.database());
  mdtCableListTestScenario scenario(pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  ///mdtClUnit unit(0, pvDatabaseManager.database());
  ///mdtClUnitVehicleType uvt(0, pvDatabaseManager.database());
  ///QList<QVariant> vtStartIdList, vtEndIdList;
  mdtClLinkData linkData;
  ///mdtClUnitConnectionData connectionData;
  ///QList<mdtClVehicleTypeLinkData> vtLinkDataList;
  QString sql;
  bool ok;

//   QVERIFY(linkData.setup(pvDatabaseManager.database()));

  // Create base structure
  scenario.createSenario();

  /*
   * Check that we have initial expected link data
   *
   * Scenario will create several links.
   * createTestUnitConnections() creates:
   *  - 20001-20000 , CABLELINK
   *  - 20003-20002 , CABLELINK
   *  - 20003-20004 , INTERNLINK
   * createTestLinks() creates:
   *  - 10000-10001 , CABLELINK
   *  - 10001-20000 , CABLELINK
   *  - 30005-40005 , CABLELINK
   *  - 40005-50005 , CABLELINK
   */
  linkPk.connectionStartId = 20001;
  linkPk.connectionEndId = 20000;
  QVERIFY(lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  linkPk.connectionStartId = 20003;
  linkPk.connectionEndId = 20002;
  QVERIFY(lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  linkPk.connectionStartId = 20003;
  linkPk.connectionEndId = 20004;
  QVERIFY(lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  linkPk.connectionStartId = 10000;
  linkPk.connectionEndId = 10001;
  QVERIFY(lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  linkPk.connectionStartId = 10001;
  linkPk.connectionEndId = 20000;
  QVERIFY(lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  linkPk.connectionStartId = 30005;
  linkPk.connectionEndId = 40005;
  QVERIFY(lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  linkPk.connectionStartId = 40005;
  linkPk.connectionEndId = 50005;
  QVERIFY(lnk.linkExists(linkPk, ok));
  QVERIFY(ok);
  
//   QVERIFY(lnk.linkExists(20001, 20000, ok));
//   QVERIFY(ok);
//   QVERIFY(lnk.linkExists(20003, 20002, ok));
//   QVERIFY(ok);
//   QVERIFY(lnk.linkExists(20003, 20004, ok));
//   QVERIFY(ok);
//   QVERIFY(lnk.linkExists(10000, 10001, ok));
//   QVERIFY(ok);
//   QVERIFY(lnk.linkExists(10001, 20000, ok));
//   QVERIFY(ok);
//   QVERIFY(lnk.linkExists(30005, 40005, ok));
//   QVERIFY(ok);
//   QVERIFY(lnk.linkExists(40005, 50005, ok));
//   QVERIFY(ok);
  /*
   * We are interested about links created by createTestUnitConnections()
   * because they are based on article links.
   * Check there initial data:
   *  - Unit connection 20000 is based on article connection 20 , witch is part of article 2
   *  - Unit connection 20001 is based on article connection 21 , witch is part of article 2
   *  - Unit connection 20002 is based on article connection 20 , witch is part of article 2
   *  - Unit connection 20003 is based on article connection 21 , witch is part of article 2
   *  - Unit connection 20004 is based on article connection 22 , witch is part of article 2
   */
  linkPk.connectionStartId = 20001;
  linkPk.connectionEndId = 20000;
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(linkData.keyData().linkTypeFk().type() == mdtClLinkType_t::CableLink);
  linkPk.connectionStartId = 20003;
  linkPk.connectionEndId = 20002;
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(linkData.keyData().linkTypeFk().type() == mdtClLinkType_t::CableLink);
  linkPk.connectionStartId = 20003;
  linkPk.connectionEndId = 20004;
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(linkData.keyData().linkTypeFk().type() == mdtClLinkType_t::InternalLink);
//   linkData = lnk.getLinkData(20001, 20000, false, false, ok);
//   QVERIFY(ok);
//   QCOMPARE(linkData.value("LinkType_Code_FK"), QVariant("CABLELINK"));
//   linkData = lnk.getLinkData(20003, 20002, false, false, ok);
//   QVERIFY(ok);
//   QCOMPARE(linkData.value("LinkType_Code_FK"), QVariant("CABLELINK"));
//   linkData = lnk.getLinkData(20003, 20004, false, false, ok);
//   QVERIFY(ok);
//   QCOMPARE(linkData.value("LinkType_Code_FK"), QVariant("INTERNLINK"));
  /*
   * Edit article link 21-20 and check that links 20001-20000 , update raleted links and check
   */
  // Check number of related links
  aLnkPk.connectionStartId = 21;
  aLnkPk.connectionEndId = 20;
  QCOMPARE(alnk.relatedLinksCount(aLnkPk), 2);
  QVERIFY(alnk.hasRelatedLinks(aLnkPk, ok));
  QVERIFY(ok);
  // Edit article link
  sql = "SELECT * FROM ArticleLink_tbl WHERE ArticleConnectionStart_Id_FK = 21 AND ArticleConnectionEnd_Id_FK = 20";
  dataList = art.getDataList<QSqlRecord>(sql, ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  data = dataList.at(0);
  data.setValue("LinkType_Code_FK", "INTERNLINK");
  ///QVERIFY(art.editLink(21, 20, data));
  // Update related links in Link_tbl
  ///QVERIFY(art.updateRelatedLinks(21, 20, "LinkType_Code_FK"));
  // Check that LinkType_Code_FK was updated
  linkPk.connectionStartId = 20001;
  linkPk.connectionEndId = 20000;
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(linkData.keyData().linkTypeFk().type() == mdtClLinkType_t::InternalLink);
//   linkData = lnk.getLinkData(20001, 20000, false, false, ok);
//   QVERIFY(ok);
//   QCOMPARE(linkData.value("LinkType_Code_FK"), QVariant("INTERNLINK"));
}

// void mdtCableListTest::linkConnectableConnectorTest()
// {
//   mdtClLink lnk(pvDatabaseManager.database());
//   mdtClUnitConnection ucnx(pvDatabaseManager.database());
//   mdtClConnectableCriteria criteria;
//   mdtClUnitConnectionKeyData ucnxKey;
//   mdtClUnitConnectionData cnxA, cnxB;
//   mdtClConnectorPkData cnrKey;
//   mdtClConnectorData cnrData;
//   ///mdtClConnectorContactKeyData contactKey;
//   mdtClConnectorContactData contactData;
//   QList<mdtClConnectorContactData> contactDataList;
//   mdtClUnitConnectorKeyData ucnrKey;
//   mdtClUnitConnectorData ucnrA, ucnrB;
//   bool ok;
//   
//   mdtClConnectorData cnrS, cnrE;
//   mdtClUnitConnectorData ucnrS, ucnrE;
// 
//   /*
//    * Setup data
//    */
// //   QVERIFY(cnxA.setup(pvDatabaseManager.database(), false));
// //   QVERIFY(cnxB.setup(pvDatabaseManager.database(), false));
// //   QVERIFY(cnrS.setup(pvDatabaseManager.database()));
// //   QVERIFY(cnrE.setup(pvDatabaseManager.database()));
//   QVERIFY(ucnrS.setup(pvDatabaseManager.database(), false, false));
//   QVERIFY(ucnrE.setup(pvDatabaseManager.database(), false, false));
//   /*
//    * Check contact checking method.
//    * In current version, only checkContactType has sense here
//    */
//   criteria.checkContactType = true;
//   /*
//    * Setup connections:
//    *  A is a socket
//    *  B is a socket
//    *  -> Must return false
//    */
// //   cnxA.setConnectionType(mdtClConnectionType_t::Socket);
// //   cnxB.setConnectionType(mdtClConnectionType_t::Socket);
// // //   cnxA.setValue("ConnectionType_Code_FK", "S");
// // //   cnxB.setValue("ConnectionType_Code_FK", "S");
// //   QVERIFY(!lnk.canConnectConnections(cnxA, cnxB, criteria));
// //   /*
// //    * Setup connections:
// //    *  A is a socket
// //    *  B is a pin
// //    *  -> Must return true
// //    */
// //   cnxA.setConnectionType(mdtClConnectionType_t::Socket);
// //   cnxB.setConnectionType(mdtClConnectionType_t::Pin);
// //   QVERIFY(lnk.canConnectConnections(cnxA, cnxB, criteria));
// //   /*
// //    * Setup connections:
// //    *  A is a pin
// //    *  B is a socket
// //    *  -> Must return true
// //    */
// //   cnxA.setConnectionType(mdtClConnectionType_t::Pin);
// //   cnxB.setConnectionType(mdtClConnectionType_t::Socket);
// //   QVERIFY(lnk.canConnectConnections(cnxA, cnxB, criteria));
// //   /*
// //    * Setup connections:
// //    *  A is a terminal
// //    *  B is a terminal
// //    *  -> Must return true
// //    */
// //   cnxA.setConnectionType(mdtClConnectionType_t::Terminal);
// //   cnxB.setConnectionType(mdtClConnectionType_t::Terminal);
// //   QVERIFY(lnk.canConnectConnections(cnxA, cnxB, criteria));
// //   /*
// //    * Setup connections:
// //    *  A is a terminal
// //    *  B is a socket
// //    *  -> Must return false
// //    */
// //   cnxA.setConnectionType(mdtClConnectionType_t::Terminal);
// //   cnxB.setConnectionType(mdtClConnectionType_t::Socket);
// //   QVERIFY(!lnk.canConnectConnections(cnxA, cnxB, criteria));
// //   /*
// //    * Setup connections:
// //    *  A is a terminal
// //    *  B is a pin
// //    *  -> Must return false
// //    */
// //   cnxA.setConnectionType(mdtClConnectionType_t::Terminal);
// //   cnxB.setConnectionType(mdtClConnectionType_t::Pin);
// //   QVERIFY(!lnk.canConnectConnections(cnxA, cnxB, criteria));
// //   /*
// //    * Setup connections:
// //    *  A is a socket
// //    *  B is a terminal
// //    *  -> Must return false
// //    */
// //   cnxA.setConnectionType(mdtClConnectionType_t::Socket);
// //   cnxB.setConnectionType(mdtClConnectionType_t::Terminal);
// //   QVERIFY(!lnk.canConnectConnections(cnxA, cnxB, criteria));
// //   /*
// //    * Setup connections:
// //    *  A is a terminal
// //    *  B is a pin
// //    *  -> Must return false
// //    */
// //   cnxA.setConnectionType(mdtClConnectionType_t::Terminal);
// //   cnxB.setConnectionType(mdtClConnectionType_t::Pin);
// //   QVERIFY(!lnk.canConnectConnections(cnxA, cnxB, criteria));
//   /*
//    * Setup criteria for connector checking
//    */
//   criteria.checkContactCount = true;
//   criteria.checkContactType = true;
//   criteria.checkForm = true;
//   criteria.checkGenderAreOpposite = true;
//   /// \todo Once implemented, add insert checks
//   criteria.checkInsert = true;
//   criteria.checkInsertRotation = true;
//   /*
//    * Setup 2 compatible connectors:
//    *  - A:
//    *   -> Female
//    *   -> Form: Round
//    *   -> Insert 12-3 (fake)
//    *   -> Insert rotation code: none
//    *   -> 2 sockets (A, B)
//    *   -> 1 terminal (GND)
//    *  - B:
//    *   -> Male
//    *   -> Form: Round
//    *   -> Insert 12-3 (fake)
//    *   -> Insert rotation code: none
//    *   -> 2 pins (A, B)
//    *   -> 1 terminal (GND)
//    */
//   // Setup base connector for A and add it to database
//   cnrKey.clear();
//   cnrKey.id = 10;
//   cnrData.clear();
//   cnrData.setPk(cnrKey);
//   cnrData.gender = "Female";
//   cnrData.form = "Round";
//   contactData.setConnectionType(mdtClConnectionType_t::Socket);
//   contactData.name = "A";
//   cnrData.addContactData(contactData);
//   contactData.setConnectionType(mdtClConnectionType_t::Socket);
//   contactData.name = "B";
//   cnrData.addContactData(contactData);
//   contactData.setConnectionType(mdtClConnectionType_t::Terminal);
//   contactData.name = "GND";
//   cnrData.addContactData(contactData);
//   QVERIFY(!ucnx.addConnector(cnrData, true).isNull());
//   // Setup base connector for B and add it to database
//   cnrKey.clear();
//   cnrKey.id = 11;
//   cnrData.clear();
//   cnrData.setPk(cnrKey);
//   cnrData.gender = "Male";
//   cnrData.form = "Round";
//   contactData.setConnectionType(mdtClConnectionType_t::Pin);
//   contactData.name = "A";
//   cnrData.addContactData(contactData);
//   contactData.setConnectionType(mdtClConnectionType_t::Pin);
//   contactData.name = "B";
//   cnrData.addContactData(contactData);
//   contactData.setConnectionType(mdtClConnectionType_t::Terminal);
//   contactData.name = "GND";
//   cnrData.addContactData(contactData);
//   QVERIFY(!ucnx.addConnector(cnrData, true).isNull());
//   // Setup unit connector A
//   ucnrKey.clear();
//   ucnrA.clear();
//   cnrKey.id = 10;
//   ucnrKey.setUnitId(1000);
//   ucnrKey.setConnectorFk(cnrKey);
//   ucnrA.setKeyData(ucnrKey);
//   contactDataList = ucnx.getContactDataList(cnrKey, ok);
//   QVERIFY(ok);
//   ucnx.addConnectionsToUnitConnector(ucnrA, contactDataList);
//   // Setup unit connector B
//   ucnrKey.clear();
//   ucnrB.clear();
//   cnrKey.id = 11;
//   ucnrKey.setUnitId(1000);
//   ucnrKey.setConnectorFk(cnrKey);
//   ucnrB.setKeyData(ucnrKey);
//   contactDataList = ucnx.getContactDataList(cnrKey, ok);
//   QVERIFY(ok);
//   ucnx.addConnectionsToUnitConnector(ucnrB, contactDataList);
//   /*
//    * Check
//    */
//   QVERIFY(lnk.canConnectConnectors(ucnrA, ucnrB, criteria, ok));
//   /*
//    * Remove created connectors
//    */
//   cnrKey.id = 10;
//   QVERIFY(ucnx.removeConnector(cnrKey, true));
//   cnrKey.id = 11;
//   QVERIFY(ucnx.removeConnector(cnrKey, true));
// 
// //   cnrS.clearValues();
// //   ucnrS.clearValues();
// //   cnrS.setValue("Gender", "Female");
// //   cnrS.setValue("Insert", "12-3");
// //   cnrS.setValue("InsertRotation", QVariant());  // To check null with empty string values comparison
// //   ucnrS.setConnectorData(cnrS);
// //   // Setup start contact A
// //   cnxA.clearValues();
// //   cnxA.setValue("UnitContactName", "A");
// //   cnxA.setValue("ConnectionType_Code_FK", "S");
// //   ucnrS.addConnectionData(cnxA);
// //   // Setup start contact B
// //   cnxA.clearValues();
// //   cnxA.setValue("UnitContactName", "B");
// //   cnxA.setValue("ConnectionType_Code_FK", "S");
// //   ucnrS.addConnectionData(cnxA);
// //   // Setup start contact GND
// //   cnxA.clearValues();
// //   cnxA.setValue("UnitContactName", "GND");
// //   cnxA.setValue("ConnectionType_Code_FK", "T");
// //   ucnrS.addConnectionData(cnxA);
// //   // Setup end connector
// //   cnrE.clearValues();
// //   ucnrE.clearValues();
// //   cnrE.setValue("Gender", "Male");
// //   cnrE.setValue("Insert", "12-3");
// //   cnrE.setValue("InsertRotation", "");  // To check null with empty string values comparison
// //   ucnrE.setConnectorData(cnrE);
// //   // Setup end contact A
// //   cnxB.clearValues();
// //   cnxB.setValue("UnitContactName", "A");
// //   cnxB.setValue("ConnectionType_Code_FK", "S");
// //   ucnrE.addConnectionData(cnxB);
// //   // Setup end contact B
// //   cnxB.clearValues();
// //   cnxB.setValue("UnitContactName", "B");
// //   cnxB.setValue("ConnectionType_Code_FK", "S");
// //   ucnrE.addConnectionData(cnxB);
// //   // Setup end contact GND
// //   cnxB.clearValues();
// //   cnxB.setValue("UnitContactName", "GND");
// //   cnxB.setValue("ConnectionType_Code_FK", "T");
// //   ucnrE.addConnectionData(cnxB);
// //   // Check
// //   QVERIFY(lnk.canConnectConnectors(ucnrA, ucnrB, criteria, ok));
// }

// void mdtCableListTest::linkAutoConnectionTest()
// {
//   mdtClLink lnk(pvDatabaseManager.database());
//   mdtCableListTestScenario scenario(pvDatabaseManager.database());
//   mdtClLinkPkData linkPk;
//   QList<mdtClLinkData> cnnLinkDataList;
//   mdtClLinkData cnnLinkData;
//   mdtClConnectableCriteria criteria;
//   mdtClUnitConnectionData a, b;
//   QList<mdtClUnitConnectionData> A, B;
//   QList<QVariant> vtStartIdList, vtEndIdList;
//   bool ok;
// 
//   // Setup connection data
//   QVERIFY(a.setup(pvDatabaseManager.database(), false));
//   QVERIFY(b.setup(pvDatabaseManager.database(), false));
// 
//   /*
//    * Case 1 :
//    *  - A and B contains same amount of connections
//    *  - A contains only pins
//    *  - B contains only sockets
//    */
//   A.clear();
//   B.clear();
//   // Create A list
//   a.clearValues();
//   a.setValue("Id_PK", 1);
//   a.setValue("UnitContactName", "1");
//   a.setValue("ConnectionType_Code_FK", "P");
//   A  << a;
//   a.clearValues();
//   a.setValue("Id_PK", 2);
//   a.setValue("UnitContactName", "2");
//   a.setValue("ConnectionType_Code_FK", "P");
//   A  << a;
//   a.clearValues();
//   a.setValue("Id_PK", 3);
//   a.setValue("UnitContactName", "3");
//   a.setValue("ConnectionType_Code_FK", "P");
//   A  << a;
//   // Create B list
//   b.clearValues();
//   b.setValue("Id_PK", 12);
//   b.setValue("UnitContactName", "2");
//   b.setValue("ConnectionType_Code_FK", "S");
//   B << b;
//   b.clearValues();
//   b.setValue("Id_PK", 11);
//   b.setValue("UnitContactName", "1");
//   b.setValue("ConnectionType_Code_FK", "S");
//   B << b;
//   b.clearValues();
//   b.setValue("Id_PK", 13);
//   b.setValue("UnitContactName", "3");
//   b.setValue("ConnectionType_Code_FK", "S");
//   B << b;
//   // Setup criteria: we check only contact genders here (P, S, T)
//   criteria.checkGenderAreOpposite = false;
//   criteria.checkContactCount = false;
//   criteria.checkContactType = true;
//   criteria.checkContactName = false;
//   criteria.checkForm = false;
//   criteria.checkInsert = false;
//   criteria.checkInsertRotation = false;
//   // Build link data list and check
//   cnnLinkDataList = lnk.getConnectionLinkListByName(A, B, criteria);
//   QCOMPARE(cnnLinkDataList.size(), 3);
//   // Check base data
//   QCOMPARE(cnnLinkDataList.at(0).value("LinkType_Code_FK"), QVariant("CONNECTION"));
//   QCOMPARE(cnnLinkDataList.at(0).value("LinkDirection_Code_FK"), QVariant("BID"));
//   QCOMPARE(cnnLinkDataList.at(1).value("LinkType_Code_FK"), QVariant("CONNECTION"));
//   QCOMPARE(cnnLinkDataList.at(1).value("LinkDirection_Code_FK"), QVariant("BID"));
//   QCOMPARE(cnnLinkDataList.at(2).value("LinkType_Code_FK"), QVariant("CONNECTION"));
//   QCOMPARE(cnnLinkDataList.at(2).value("LinkDirection_Code_FK"), QVariant("BID"));
//   // Check connection - A and B have same size, so order is like A
//   a = cnnLinkDataList.at(0).startConnectionData();
//   b = cnnLinkDataList.at(0).endConnectionData();
//   QCOMPARE(a.value("UnitContactName"), b.value("UnitContactName"));
//   a = cnnLinkDataList.at(1).startConnectionData();
//   b = cnnLinkDataList.at(1).endConnectionData();
//   QCOMPARE(a.value("UnitContactName"), b.value("UnitContactName"));
//   a = cnnLinkDataList.at(2).startConnectionData();
//   b = cnnLinkDataList.at(2).endConnectionData();
//   QCOMPARE(a.value("UnitContactName"), b.value("UnitContactName"));
//   /*
//    * Case 2 :
//    *  - A < B
//    *  - A contains only pins
//    *  - B contains only sockets
//    */
//   A.clear();
//   B.clear();
//   // Create A list
//   a.clearValues();
//   a.setValue("Id_PK", 1);
//   a.setValue("UnitContactName", "1");
//   a.setValue("ConnectionType_Code_FK", "P");
//   A  << a;
//   // Create B list
//   b.clearValues();
//   b.setValue("Id_PK", 11);
//   b.setValue("UnitContactName", "1");
//   b.setValue("ConnectionType_Code_FK", "S");
//   B << b;
//   b.clearValues();
//   b.setValue("Id_PK", 12);
//   b.setValue("UnitContactName", "2");
//   b.setValue("ConnectionType_Code_FK", "S");
//   B << b;
//   // Setup criteria: we check only contact genders here (P, S, T)
//   criteria.checkGenderAreOpposite = false;
//   criteria.checkContactCount = false;
//   criteria.checkContactType = true;
//   criteria.checkContactName = false;
//   criteria.checkForm = false;
//   criteria.checkInsert = false;
//   criteria.checkInsertRotation = false;
//   // Build link data list and check
//   cnnLinkDataList = lnk.getConnectionLinkListByName(A, B, criteria);
//   QCOMPARE(cnnLinkDataList.size(), 1);
//   // Check base data
//   QCOMPARE(cnnLinkDataList.at(0).value("LinkType_Code_FK"), QVariant("CONNECTION"));
//   QCOMPARE(cnnLinkDataList.at(0).value("LinkDirection_Code_FK"), QVariant("BID"));
//   // Check connection - A < B , so order is like A
//   a = cnnLinkDataList.at(0).startConnectionData();
//   b = cnnLinkDataList.at(0).endConnectionData();
//   QCOMPARE(a.value("UnitContactName"), b.value("UnitContactName"));
//   /*
//    * Case 3 :
//    *  - A > B
//    *  - A contains only pins
//    *  - B contains only sockets
//    */
//   A.clear();
//   B.clear();
//   // Create A list
//   a.clearValues();
//   a.setValue("Id_PK", 1);
//   a.setValue("UnitContactName", "1");
//   a.setValue("ConnectionType_Code_FK", "P");
//   A  << a;
//   a.clearValues();
//   a.setValue("Id_PK", 2);
//   a.setValue("UnitContactName", "2");
//   a.setValue("ConnectionType_Code_FK", "P");
//   A  << a;
//   // Create B list
//   b.clearValues();
//   b.setValue("Id_PK", 12);
//   b.setValue("UnitContactName", "2");
//   b.setValue("ConnectionType_Code_FK", "S");
//   B << b;
//   // Build link data list and check
//   /// \todo Connectable criteria
//   cnnLinkDataList = lnk.getConnectionLinkListByName(A, B, mdtClConnectableCriteria());
//   QCOMPARE(cnnLinkDataList.size(), 1);
//   // Check base data
//   QCOMPARE(cnnLinkDataList.at(0).value("LinkType_Code_FK"), QVariant("CONNECTION"));
//   QCOMPARE(cnnLinkDataList.at(0).value("LinkDirection_Code_FK"), QVariant("BID"));
//   // Check connection
//   a = cnnLinkDataList.at(0).startConnectionData();
//   b = cnnLinkDataList.at(0).endConnectionData();
//   QCOMPARE(a.value("UnitContactName"), b.value("UnitContactName"));
// 
//   /// \todo Combinaisons with non compatible P-S , with T, etc...
// 
//   // Create base structure for next tests
//   scenario.createTestVehicleTypes();
//   scenario.createTestArticles();
//   scenario.createTestConnectors();
//   scenario.createTestArticleConnections();
//   scenario.createTestArticleLinks();
//   scenario.createTestArticleConnectors();
//   scenario.createTestUnits();
//   scenario.createTestVehicleTypeUnitAssignations();
//   scenario.createTestUnitConnectors();
// 
//   /*
//    * Check connect method with following connectors:
//    *
//    *  - START connector: Id_PK : 400000 , Unit_Id_FK : 1000 , Connector_Id_FK : NULL , ArticleConnector_Id_FK : NULL , Name : Unit connector 400000
//    *   -> Connection: Id_PK 40005 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : P , Name : A
//    *   -> Connection: Id_PK 40006 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : B
//    *
//    *  - END connector: Id_PK : 500000 , Unit_Id_FK : 2000 , Connector_Id_FK : NULL , ArticleConnector_Id_FK : NULL , Name : Unit connector 500000
//    *   -> Connection: Id_PK 50005 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : A
//    *   -> Connection: Id_PK 50006 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : B
//    *
//    * For vehicle types, we take:
//    *
//    *  - START: Id_PK : 1
//    *  - END : Id_PK : 2
//    */
//   // Setup criteria: we check only contact names and genders here (P, S, T)
//   criteria.checkGenderAreOpposite = false;
//   criteria.checkContactCount = false;
//   criteria.checkContactType = true;
//   criteria.checkContactName = true;
//   criteria.checkForm = false;
//   criteria.checkInsert = false;
//   criteria.checkInsertRotation = false;
//   // Create connection (i.e. links)
//   QFAIL("connectByContactName() not implemted yet");
//   ///QVERIFY(lnk.connectByContactName(400000, 500000, 1, 2, criteria));
//   /*
//    * Check link data
//    */
//   // Get link from connection 40005 -> 50005 : must exist
//   linkPk.connectionStartId = 40005;
//   linkPk.connectionEndId = 50005;
//   QVERIFY(lnk.linkExists(linkPk, ok));
//   QVERIFY(ok);
//   cnnLinkData = lnk.getLinkData(linkPk, ok);
//   ///cnnLinkData = lnk.getLinkData(40005, 50005, true, true, ok);
//   QVERIFY(ok);
//   QVERIFY(cnnLinkData.keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
//   QVERIFY(cnnLinkData.keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
// //   QCOMPARE(cnnLinkData.value("LinkType_Code_FK"), QVariant("CONNECTION"));
// //   QCOMPARE(cnnLinkData.value("LinkDirection_Code_FK"), QVariant("BID"));
//   // Check that link from 40006 -> 50006 not exists
//   linkPk.connectionStartId = 40006;
//   linkPk.connectionEndId = 50006;
//   QVERIFY(!lnk.linkExists(linkPk, ok));
// //   QVERIFY(!lnk.linkExists(40006, 50006, ok));
//   QVERIFY(ok);
// 
// 
//   /*
//    * Check disconnect method
//    */
//   vtStartIdList.clear();
//   vtEndIdList.clear();
//   vtStartIdList << 1;
//   vtEndIdList << 2;
//   QFAIL("disconnectConnectors() not implemented yet");
//   ///QVERIFY(lnk.disconnectConnectors(400000, 500000, vtStartIdList, vtEndIdList));
//   linkPk.connectionStartId = 40005;
//   linkPk.connectionEndId = 50005;
//   QVERIFY(!lnk.linkExists(linkPk, ok));
// //   QVERIFY(!lnk.linkExists(40005, 50005, ok));
// }


void mdtCableListTest::pathGraphTest()
{
  mdtClPathGraph graph(pvDatabaseManager.database());
  mdtCableListTestScenario scenario(pvDatabaseManager.database());
  QList<QVariant> idList;
  bool ok;

  // Initial
  idList = graph.getLinkedConnectionIdList(0, ok);
  QVERIFY(!ok);
  QVERIFY(idList.isEmpty());
  idList = graph.getShortestPath(0, 1, ok);
  QVERIFY(!ok);
  QVERIFY(idList.isEmpty());
  QVERIFY(graph.getUserData(0, 1).isNull());
  QVERIFY(!graph.connectionsAreLinked(0, 1));

  /*
   * Tests with free data
   */
  // (1)-(2)
  graph.addLink(1, 2);
  idList = graph.getLinkedConnectionIdList(1, ok);
  QVERIFY(ok);
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
  idList = graph.getLinkedConnectionIdList(1, ok);
  QVERIFY(ok);
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
  idList = graph.getLinkedConnectionIdList(1, ok);
  QVERIFY(ok);
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
  idList = graph.getLinkedConnectionIdList(1, ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 3);
  QCOMPARE(idList.at(0), QVariant(2));
  QCOMPARE(idList.at(1), QVariant(3));
  QCOMPARE(idList.at(2), QVariant(4));
  idList = graph.getLinkedConnectionIdList(10, ok);
  QVERIFY(ok);
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
  idList = graph.getLinkedConnectionIdList(1, ok);
  QVERIFY(ok);
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
  idList = graph.getLinkedConnectionIdList(1, ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 0);

  /*
   * Tests with database data
   */
  // Scenario
  scenario.createSenario();
  // Load link list
  QVERIFY(graph.loadLinkList());
  // Check that manually added links was cleared
  idList = graph.getLinkedConnectionIdList(1, ok);
  QVERIFY(!ok);
  QCOMPARE(idList.size(), 0);
  // Check linked connections
  idList = graph.getLinkedConnectionIdList(10000, ok);
  QVERIFY(ok);
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
   * Check that links of type CABLELINK and INTERNLINK are taken in acount
   *
   * Scenario will create several links.
   * createTestUnitConnections() creates:
   *  - 20001-20000 , CABLELINK
   *  - 20003-20002 , CABLELINK
   *  - 20003-20004 , INTERNLINK
   * createTestLinks() creates:
   *  - 10000-10001 , CABLELINK
   *  - 10001-20000 , CABLELINK
   *  - 30005-40005 , CABLELINK
   *  - 40005-50005 , CABLELINK
   * At the end, we have 3 paths:
   *  - 20001,20000,10001,10000
   *  - 20003,20002,20004
   *  - 30005,40005,50005
   */
  // Check path: 20001,20000,10001,10000
  idList = graph.getLinkedConnectionIdList(20001, ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 3);
  ///QVERIFY(idList.contains(20001));
  QVERIFY(idList.contains(20000));
  QVERIFY(idList.contains(10001));
  QVERIFY(idList.contains(10000));
  // Check path: 20003,20002,20004
  idList = graph.getLinkedConnectionIdList(20003, ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 2);
  ///QVERIFY(idList.contains(20003));
  QVERIFY(idList.contains(20002));
  QVERIFY(idList.contains(20004));
  // Check path: 30005,40005,50005
  idList = graph.getLinkedConnectionIdList(30005, ok);
  QVERIFY(ok);
  QCOMPARE(idList.size(), 2);
  ///QVERIFY(idList.contains(30005));
  QVERIFY(idList.contains(40005));
  QVERIFY(idList.contains(50005));

  /*
   * Call method to remove manually added links - Must change nothing
   */
  // Check linked connections
  idList = graph.getLinkedConnectionIdList(10000, ok);
  QVERIFY(ok);
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
}

void mdtCableListTest::directLinkTest()
{
  mdtClDirectLink *dlnk;
  mdtClPathGraph graph(pvDatabaseManager.database());
  mdtCableListTestScenario scenario(pvDatabaseManager.database());
  QSqlRecord record;
  QList<QSqlRecord> dataList;
  QList<QVariant> idList;
  QList<QVariant> expectedIdList;
  QList<QVariant> allConnectionsIdList;
  bool ok;
  int i;

  // Scenario
  scenario.createSenario();
  // Load link list
  QVERIFY(graph.loadLinkList());
  // Initial
  dlnk = new mdtClDirectLink(0, pvDatabaseManager.database());
  QVERIFY(pvDatabaseManager.database().tables().contains("DirectLink_tbl"));
  record = pvDatabaseManager.database().record("DirectLink_tbl");
  QVERIFY(record.contains("UnitConnectionStart_Id_FK"));
  QVERIFY(record.contains("UnitConnectionEnd_Id_FK"));
  // Get unit connections for next tests
  dataList = dlnk->getDataList<QSqlRecord>("SELECT Id_PK FROM UnitConnection_tbl", ok);
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
  dataList = dlnk->getDataList<QSqlRecord>("SELECT * FROM DirectLink_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("UnitConnectionStart_Id_FK"), QVariant(10000));
  QCOMPARE(dataList.at(0).value("UnitConnectionEnd_Id_FK"), QVariant(20000));
  QVERIFY(dlnk->removeData("DirectLink_tbl", "UnitConnectionStart_Id_FK", 10000, "UnitConnectionEnd_Id_FK", 20000));
  dataList = dlnk->getDataList<QSqlRecord>("SELECT * FROM DirectLink_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Check link adding between 2 connectors
  QVERIFY(dlnk->addLinksByUnitConnector(300000, 500000, &graph));
  dataList = dlnk->getDataList<QSqlRecord>("SELECT * FROM DirectLink_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("UnitConnectionStart_Id_FK"), QVariant(30005));
  QCOMPARE(dataList.at(0).value("UnitConnectionEnd_Id_FK"), QVariant(50005));
  QCOMPARE(dataList.at(0).value("UnitConnectorStart_Id_FK"), QVariant(300000));
  QCOMPARE(dataList.at(0).value("UnitConnectorEnd_Id_FK"), QVariant(500000));


  delete dlnk;
  QVERIFY(!pvDatabaseManager.database().tables().contains("DirectLink_tbl"));
}

void mdtCableListTest::linkBeamTest()
{
  mdtClLinkBeam lb(0, pvDatabaseManager.database());
  mdtCableListTestScenario scenario(pvDatabaseManager.database());
  mdtSqlRecord record;
  QList<QSqlRecord> dataList;
  bool ok;

  // Scenario
  scenario.createSenario();
  /*
   * Create a LinkBeam
   */
  QVERIFY(record.addAllFields("LinkBeam_tbl", pvDatabaseManager.database()));
  record.setValue("Id_PK", 1);
  record.setValue("Identification", "Link beam 1");
  QVERIFY(lb.addRecord(record, "LinkBeam_tbl"));
  dataList = lb.getDataList<QSqlRecord>("SELECT * FROM LinkBeam_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Id_PK"), QVariant(1));
  QCOMPARE(dataList.at(0).value("Identification"), QVariant("Link beam 1"));
  /*
   * Add a start unit
   */
  QVERIFY(lb.addStartUnit(1000, 1));
  dataList = lb.getDataList<QSqlRecord>("SELECT * FROM LinkBeam_UnitStart_view", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Unit_Id_FK"), QVariant(1000));
  QCOMPARE(dataList.at(0).value("LinkBeam_Id_FK"), QVariant(1));
  /*
   * Add a end unit
   */
  QVERIFY(lb.addEndUnit(2000, 1));
  dataList = lb.getDataList<QSqlRecord>("SELECT * FROM LinkBeam_UnitEnd_view", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("Unit_Id_FK"), QVariant(2000));
  QCOMPARE(dataList.at(0).value("LinkBeam_Id_FK"), QVariant(1));
  /*
   * Add (existing) link from unit connection ID 10000 -> 10001 to link beam 1
   */
  QVERIFY(lb.addLink(10000, 10001, 1));
  dataList = lb.getDataList<QSqlRecord>("SELECT * FROM UnitLink_view WHERE LinkBeam_Id_FK = 1", ok);
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
  dataList = lb.getDataList<QSqlRecord>("SELECT * FROM UnitLink_view WHERE UnitConnectionStart_Id_FK = 10000 AND UnitConnectionEnd_Id_FK = 10001", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QVERIFY(dataList.at(0).value("LinkBeam_Id_FK").isNull());

  /*
   * Remove start unit
   */
  QVERIFY(lb.removeStartUnit(1000, 1));
  dataList = lb.getDataList<QSqlRecord>("SELECT * FROM LinkBeam_UnitStart_view", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  /*
   * Remove end unit
   */
  QVERIFY(lb.removeEndUnit(2000, 1));
  dataList = lb.getDataList<QSqlRecord>("SELECT * FROM LinkBeam_UnitEnd_view", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

/*
 * Test database helper methods
 */

void mdtCableListTest::createDatabaseSchema()
{
  QTemporaryFile dbFile;

  /*
   * Check Sqlite database creation
   */
  QVERIFY(dbFile.open());
  dbFile.close();
  pvDbFileInfo.setFile(dbFile.fileName() + ".db");
  mdtTtDatabaseSchema schema(&pvDatabaseManager);
  QVERIFY(schema.createSchemaSqlite(pvDbFileInfo));
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
