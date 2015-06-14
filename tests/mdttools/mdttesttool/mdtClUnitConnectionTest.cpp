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
#include "mdtClUnitConnectionTest.h"
#include "mdtApplication.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlRecord.h"
#include "mdtTtBase.h"
#include "mdtClConnector.h"
#include "mdtClConnectorData.h"
#include "mdtClConnectorContactData.h"
#include "mdtClConnectionTypeData.h"
#include "mdtClArticleConnectorKeyData.h"
#include "mdtClArticleConnectorData.h"
#include "mdtClArticleConnectionKeyData.h"
#include "mdtClArticleConnectionData.h"
#include "mdtClUnitConnection.h"
#include "mdtClUnitConnectorKeyData.h"
#include "mdtClUnitConnectorData.h"
#include "mdtClUnitConnectionKeyData.h"
#include "mdtClUnitConnectionData.h"
#include "mdtCableListTestScenario.h"
#include <QTemporaryFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QFile>
#include <QVariant>
#include <QList>
#include <QLocale>

#include <QDebug>

void mdtClUnitConnectionTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabaseManager.database().isOpen());
}

void mdtClUnitConnectionTest::cleanupTestCase()
{
  QFile::remove(pvDbFileInfo.absoluteFilePath());
}

void mdtClUnitConnectionTest::unitConnectorDataTest()
{
  mdtClUnitConnectorKeyData key;
  mdtClUnitConnectorData data;
  mdtClConnectorKeyData cnrKey;
  mdtClArticleConnectorKeyData acnrKey;

  /*
   * Key data test
   */
  // Initial state
  QVERIFY(key.isNull());
  QVERIFY(!key.isBasedOnConnector());
  QVERIFY(!key.isBasedOnArticleConnector());
  // Set key for a free unit connector
  key.id = 1;
  QVERIFY(key.isNull());
  key.setUnitId(2);
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(2));
  QVERIFY(!key.isBasedOnConnector());
  QVERIFY(!key.isBasedOnArticleConnector());
  // Clear
  key.clear();
  QVERIFY(key.id.isNull());
  QVERIFY(key.unitId().isNull());
  QVERIFY(key.connectorFk().isNull());
  QVERIFY(key.articleConnectorFk().isNull());
  QVERIFY(key.isNull());
  QVERIFY(!key.isBasedOnConnector());
  QVERIFY(!key.isBasedOnArticleConnector());
  // Set key for a unit connector based on a connector
  cnrKey.clear();
  key.id = 3;
  key.setUnitId(4);
  cnrKey.id = 5;
  key.setConnectorFk(cnrKey);
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(4));
  QCOMPARE(key.connectorFk().id, QVariant(5));
  QVERIFY(key.isBasedOnConnector());
  QVERIFY(!key.isBasedOnArticleConnector());
  // Clear
  key.clear();
  QVERIFY(key.id.isNull());
  QVERIFY(key.unitId().isNull());
  QVERIFY(key.connectorFk().isNull());
  QVERIFY(key.articleConnectorFk().isNull());
  QVERIFY(key.isNull());
  QVERIFY(!key.isBasedOnConnector());
  QVERIFY(!key.isBasedOnArticleConnector());
  // Set key for a unit connector based on a free article connector
  acnrKey.clear();
  key.id = 6;
  key.setUnitId(7);
  acnrKey.setArticleId(8);
  key.setArticleConnectorFk(acnrKey);
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(7));
  QVERIFY(!key.isBasedOnConnector());
  QCOMPARE(key.articleConnectorFk().articleId(), QVariant(8));
  QVERIFY(key.isBasedOnArticleConnector());
  // Clear
  key.clear();
  QVERIFY(key.id.isNull());
  QVERIFY(key.unitId().isNull());
  QVERIFY(key.connectorFk().isNull());
  QVERIFY(key.articleConnectorFk().isNull());
  QVERIFY(key.isNull());
  QVERIFY(!key.isBasedOnConnector());
  QVERIFY(!key.isBasedOnArticleConnector());
  // Set key for a unit connector based on a article connector that is based on a connector
  cnrKey.clear();
  acnrKey.clear();
  key.id = 9;
  key.setUnitId(10);
  cnrKey.id = 11;
  acnrKey.setArticleId(12);
  acnrKey.setConnectorFk(cnrKey);
  key.setArticleConnectorFk(acnrKey);
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(10));
  QCOMPARE(key.connectorFk().id, QVariant(11));
  QVERIFY(key.isBasedOnConnector());
  QCOMPARE(key.articleConnectorFk().articleId(), QVariant(12));
  QVERIFY(key.isBasedOnArticleConnector());
  // Clear
  key.clear();
  QVERIFY(key.id.isNull());
  QVERIFY(key.unitId().isNull());
  QVERIFY(key.connectorFk().isNull());
  QVERIFY(key.articleConnectorFk().isNull());
  QVERIFY(key.isNull());
  QVERIFY(!key.isBasedOnConnector());
  QVERIFY(!key.isBasedOnArticleConnector());
  /*
   * Data test
   */
  // Initial state
  QVERIFY(data.isNull());
  QVERIFY(!data.isBasedOnArticleConnector());
  QVERIFY(!data.isBasedOnArticleConnector());
  // Set data for a free unit connector
  key.clear();
  key.id = 5;
  key.setUnitId(6);
  data.setKeyData(key);
  data.name = "X1";
  QVERIFY(!data.isNull());
  QVERIFY(!data.isBasedOnConnector());
  QVERIFY(!data.isBasedOnArticleConnector());
  // Clear
  data.clear();
  QVERIFY(data.name.isNull());
  QVERIFY(data.isNull());
  QVERIFY(!data.isBasedOnArticleConnector());
  QVERIFY(!data.isBasedOnArticleConnector());
  // Set data for unit connector based on a connector
  key.clear();
  cnrKey.clear();
  cnrKey.id = 1;
  key.id = 2;
  key.setUnitId(3);
  key.setConnectorFk(cnrKey);
  data.setKeyData(key);
  data.name = "X2";
  QVERIFY(!data.isNull());
  QVERIFY(data.isBasedOnConnector());
  QVERIFY(!data.isBasedOnArticleConnector());
  // Clear
  data.clear();
  QVERIFY(data.name.isNull());
  QVERIFY(data.isNull());
  QVERIFY(!data.isBasedOnArticleConnector());
  QVERIFY(!data.isBasedOnArticleConnector());
  // Set data for a unit connector based on a free article connector
  key.clear();
  acnrKey.clear();
  acnrKey.id = 1;
  acnrKey.setArticleId(2);
  key.id = 3;
  key.setUnitId(4);
  key.setArticleConnectorFk(acnrKey);
  QVERIFY(!acnrKey.isNull());
  data.setKeyData(key);
  data.name = "X3";
  QVERIFY(!data.isNull());
  QVERIFY(!data.isBasedOnConnector());
  QVERIFY(data.isBasedOnArticleConnector());
  // Clear
  data.clear();
  QVERIFY(data.name.isNull());
  QVERIFY(data.isNull());
  QVERIFY(!data.isBasedOnArticleConnector());
  QVERIFY(!data.isBasedOnArticleConnector());
  // Set data for a unit connector based on a article connector that is based on a connector
  acnrKey.clear();
  cnrKey.clear();
  key.clear();
  key.id = 5;
  key.setUnitId(6);
  cnrKey.id = 7;
  acnrKey.setArticleId(8);
  acnrKey.setConnectorFk(cnrKey);
  key.setArticleConnectorFk(acnrKey);
  data.setKeyData(key);
  data.name = "X5";
  QVERIFY(!data.isNull());
  QVERIFY(data.isBasedOnConnector());
  QVERIFY(data.isBasedOnArticleConnector());
  // Clear
  data.clear();
  QVERIFY(data.name.isNull());
  QVERIFY(data.isNull());
  QVERIFY(!data.isBasedOnArticleConnector());
  QVERIFY(!data.isBasedOnArticleConnector());
}

void mdtClUnitConnectionTest::unitConnectionDataTest()
{
  mdtClUnitConnectionKeyData key;
  mdtClUnitConnectionData data;
  mdtClArticleConnectionKeyData articleConnectionFk;
  mdtClUnitConnectorKeyData unitConnectorFk;
  mdtClArticleConnectorKeyData articleConnectorFk;

  /*
   * Key data test
   */
  // Initial state
  QVERIFY(key.isNull());
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  // Set key for a unit free unit connection
  key.id = 1;
  QVERIFY(key.isNull());
  key.setUnitId(2);
  QVERIFY(key.isNull());
  key.setConnectionType(mdtClConnectionType_t::Terminal);
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(2));
  QVERIFY(key.connectionTypeFk().type() == mdtClConnectionType_t::Terminal);
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  // Clear
  key.clear();
  QVERIFY(key.id.isNull());
  QVERIFY(key.unitId().isNull());
  QVERIFY(key.unitConnectorFk().isNull());
  QVERIFY(key.articleConnectionFk().isNull());
  QVERIFY(key.connectionTypeFk().isNull());
  QVERIFY(key.isNull());
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  // Set key for a unit connection that is part of a free unit connector
  unitConnectorFk.clear();
  key.id = 4;
  key.setUnitId(5);
  key.setConnectionType(mdtClConnectionType_t::Pin);
  unitConnectorFk.setUnitId(5);
  key.setUnitConnectorFk(unitConnectorFk);
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(5));
  QVERIFY(key.connectionTypeFk().type() == mdtClConnectionType_t::Pin);
  QVERIFY(key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  // Clear
  key.clear();
  QVERIFY(key.id.isNull());
  QVERIFY(key.unitId().isNull());
  QVERIFY(key.unitConnectorFk().isNull());
  QVERIFY(key.articleConnectionFk().isNull());
  QVERIFY(key.connectionTypeFk().isNull());
  QVERIFY(key.isNull());
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  // Set key for a unit connection based on a free article connection
  articleConnectionFk.clear();
  key.id = 6;
  key.setUnitId(7);
  articleConnectionFk.setArticleId(8);
  articleConnectionFk.setConnectionType(mdtClConnectionType_t::Socket);
  key.setArticleConnectionFk(articleConnectionFk);
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(7));
  QVERIFY(key.connectionTypeFk().type() == mdtClConnectionType_t::Socket);
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(key.isBasedOnArticleConnection());
  // Clear
  key.clear();
  QVERIFY(key.id.isNull());
  QVERIFY(key.unitId().isNull());
  QVERIFY(key.unitConnectorFk().isNull());
  QVERIFY(key.articleConnectionFk().isNull());
  QVERIFY(key.connectionTypeFk().isNull());
  QVERIFY(key.isNull());
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  /*
   * Set key for a unit connection that is based on a unit connector, that is based on a article connector
   * -> Unit connection is also based on a article connection, that is based on the same article connector.
   */
  // Setup article connector
  articleConnectorFk.clear();
  articleConnectorFk.setArticleId(1);
  // Setup article connection
  articleConnectionFk.clear();
  articleConnectionFk.setArticleId(1);
  articleConnectionFk.setConnectionType(mdtClConnectionType_t::Pin);
  articleConnectionFk.setArticleConnectorFk(articleConnectorFk);
  // Setup unit connector
  unitConnectorFk.clear();
  unitConnectorFk.setUnitId(2);
  unitConnectorFk.setArticleConnectorFk(articleConnectorFk);
  // Setup unit connection
  key.id = 3;
  key.setUnitId(2);
  key.setUnitConnectorFk(unitConnectorFk);
  key.setArticleConnectionFk(articleConnectionFk);
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(2));
  QVERIFY(key.connectionTypeFk().type() == mdtClConnectionType_t::Pin);
  QVERIFY(key.isPartOfUnitConnector());
  QVERIFY(key.isBasedOnArticleConnection());
  // Clear
  key.clear();
  QVERIFY(key.id.isNull());
  QVERIFY(key.unitId().isNull());
  QVERIFY(key.unitConnectorFk().isNull());
  QVERIFY(key.articleConnectionFk().isNull());
  QVERIFY(key.connectionTypeFk().isNull());
  QVERIFY(key.isNull());
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  /*
   * Data test
   */
  // Initial state
  QVERIFY(data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  // Set data for a free unit connection
  key.clear();
  key.id = 5;
  key.setUnitId(6);
  key.setConnectionType(mdtClConnectionType_t::Terminal);
  data.setKeyData(key);
  QVERIFY(!data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Terminal);
  // Clear
  data.clear();
  QVERIFY(data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Undefined);
  // Set data for a unit connection part of unit connector
  unitConnectorFk.clear();
  key.clear();
  key.id = 1;
  key.setUnitId(2);
  unitConnectorFk.setUnitId(2);
  key.setUnitConnectorFk(unitConnectorFk);
  key.setConnectionType(mdtClConnectionType_t::Pin);
  data.setKeyData(key);
  QVERIFY(!data.isNull());
  QVERIFY(data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Pin);
  // Clear
  data.clear();
  QVERIFY(data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  // Set data for a unit connection based on a article connection
  articleConnectionFk.clear();
  key.clear();
  key.id = 5;
  key.setUnitId(6);
  articleConnectionFk.setArticleId(7);
  articleConnectionFk.setConnectionType(mdtClConnectionType_t::Socket);
  key.setArticleConnectionFk(articleConnectionFk);
  data.setKeyData(key);
  QVERIFY(!data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(data.isBasedOnArticleConnection());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Socket);
  // Clear
  data.clear();
  QVERIFY(data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  // Data members set/clear
  key.clear();
  key.id = 1;
  key.setUnitId(2);
  key.setConnectionType(mdtClConnectionType_t::Terminal);
  data.setKeyData(key);
  data.name = "A";
  data.resistance = 1.0;
  data.schemaPage = "20";
  data.signalName = "SIG_A";
  data.swAddress = 25;
  data.functionEN = "EN";
  data.functionFR = "FR";
  data.functionDE = "DE";
  data.functionIT = "IT";
  QVERIFY(!data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Terminal);
  // Clear
  data.clear();
  QVERIFY(data.name.isNull());
  QVERIFY(data.resistance.isNull());
  QVERIFY(data.schemaPage.isNull());
  QVERIFY(data.signalName.isNull());
  QVERIFY(data.swAddress.isNull());
  QVERIFY(data.functionEN.isNull());
  QVERIFY(data.functionFR.isNull());
  QVERIFY(data.functionDE.isNull());
  QVERIFY(data.functionIT.isNull());
}

void mdtClUnitConnectionTest::unitConnectorWithConnectionsTest()
{
  mdtClUnitConnectorKeyData unitConnectorKey;
  mdtClUnitConnectorData unitConnectorData;
  mdtClUnitConnectionKeyData unitConnectionKey;
  mdtClUnitConnectionData unitConnectionData;

  /*
   * Initial state
   */
  QCOMPARE(unitConnectorData.connectionDataList().size(), 0);
  /*
   * Setup a free unit connector
   */
  // Setup unit connector key
  unitConnectorKey.clear();
  unitConnectorKey.setUnitId(1);
  // Setup unit connector data
  unitConnectorData.clear();
  unitConnectorData.setKeyData(unitConnectorKey);
  unitConnectorData.name = "X1";
  // Setup unit connection key
  unitConnectionKey.clear();
  unitConnectionKey.setConnectionType(mdtClConnectionType_t::Pin);
  // Setup unit connection data and add
  unitConnectionData.clear();
  unitConnectionData.setKeyData(unitConnectionKey);
  unitConnectionData.name = "A";
  unitConnectorData.addConnectionData(unitConnectionData);
  // Check
  QCOMPARE(unitConnectorData.connectionDataList().size(), 1);
  QCOMPARE(unitConnectorData.connectionDataList().at(0).keyData().unitId(), QVariant(1));
  QCOMPARE(unitConnectorData.connectionDataList().at(0).keyData().unitConnectorFk().unitId(), QVariant(1));
  QVERIFY(unitConnectorData.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Pin);
  QCOMPARE(unitConnectorData.connectionDataList().at(0).name, QVariant("A"));
  // Setup unit connection key
  
  // Setup unit connection data and add
  unitConnectionData.clear();
  unitConnectionData.setConnectionType(mdtClConnectionType_t::Socket);
  unitConnectionData.name = "B";
  unitConnectorData.addConnectionData(unitConnectionData);
  // Check
  QCOMPARE(unitConnectorData.connectionDataList().size(), 2);
  QCOMPARE(unitConnectorData.connectionDataList().at(0).keyData().unitId(), QVariant(1));
  QCOMPARE(unitConnectorData.connectionDataList().at(0).keyData().unitConnectorFk().unitId(), QVariant(1));
  QVERIFY(unitConnectorData.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Pin);
  QCOMPARE(unitConnectorData.connectionDataList().at(0).name, QVariant("A"));
  QCOMPARE(unitConnectorData.connectionDataList().at(1).keyData().unitId(), QVariant(1));
  QCOMPARE(unitConnectorData.connectionDataList().at(1).keyData().unitConnectorFk().unitId(), QVariant(1));
  QVERIFY(unitConnectorData.connectionDataList().at(1).connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(unitConnectorData.connectionDataList().at(1).name, QVariant("B"));
  // Clear
  unitConnectorData.clear();
  QCOMPARE(unitConnectorData.connectionDataList().size(), 0);
  
  /// \test Add other combinaisons
}

void mdtClUnitConnectionTest::unitConnectionAddGetRemoveTest()
{
  mdtClUnitConnection ucnx(pvDatabaseManager.database());
  mdtCableListTestScenario scenario(pvDatabaseManager.database());
  mdtClUnitConnectionKeyData key1, key2;
  mdtClUnitConnectionData data;
  mdtClArticleConnectionKeyData articleConnectionFk;
  bool ok;

  /*
   * Populate databse
   */
  scenario.createTestArticles();
  scenario.createTestArticleConnections();
  scenario.createTestUnits();
  /*
   * Add a free unit connection
   */
  // Setup unit connection data
  data.clear();
  key1.setUnitId(1000);
  key1.setConnectionType(mdtClConnectionType_t::Terminal);
  data.setKeyData(key1);
  data.name = "A";
  data.resistance = 1.0;
  data.schemaPage = "25";
  data.signalName = "Name_A";
  data.swAddress = 128;
  data.functionEN = "FEN";
  data.functionFR = "FFR";
  data.functionDE = "FDE";
  data.functionIT = "FIT";
  // Add unit connection to databse
  key1 = ucnx.addUnitConnection(data, true);
  QVERIFY(!key1.isNull());
  QCOMPARE(key1.unitId(), QVariant(1000));
  // Get data from database and check
  data = ucnx.getUnitConnectionData(key1, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(1000));
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Terminal);
  QCOMPARE(data.name, QVariant("A"));
  QCOMPARE(data.resistance, QVariant(1.0));
  QCOMPARE(data.schemaPage, QVariant("25"));
  QCOMPARE(data.signalName, QVariant("Name_A"));
  QCOMPARE(data.swAddress, QVariant(128));
  QCOMPARE(data.functionEN, QVariant("FEN"));
  QCOMPARE(data.functionFR, QVariant("FFR"));
  QCOMPARE(data.functionDE, QVariant("FDE"));
  QCOMPARE(data.functionIT, QVariant("FIT"));
  /*
   * Add a unit connection based on a article connection
   */
  // Setup article connection FK
  articleConnectionFk.clear();
  articleConnectionFk.id = 20;
  articleConnectionFk.setArticleId(2);
  articleConnectionFk.setConnectionType(mdtClConnectionType_t::Terminal);
  // Setup unit connection FK
  key2.setUnitId(2000);
  key2.setArticleConnectionFk(articleConnectionFk);
  // Setup unit connection data
  data.clear();
  data.setKeyData(key2);
  data.name = "B";
  // Add unit connection to databse
  key2 = ucnx.addUnitConnection(data, true);
  QVERIFY(!key2.isNull());
  QCOMPARE(key2.unitId(), QVariant(2000));
  // Get data from database and check
  data = ucnx.getUnitConnectionData(key2, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(2000));
  QCOMPARE(data.keyData().articleConnectionFk().id, QVariant(20));
  QCOMPARE(data.keyData().articleConnectionFk().articleId(), QVariant(2));
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(data.isBasedOnArticleConnection());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Terminal);
  QCOMPARE(data.name, QVariant("B"));
  /*
   * Remove connections
   */
  // Remove connection key1
  QVERIFY(ucnx.removeUnitConnection(key1));
  data = ucnx.getUnitConnectionData(key1, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  // Remove connection key2
  QVERIFY(ucnx.removeUnitConnection(key2));
  data = ucnx.getUnitConnectionData(key2, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
}

void mdtClUnitConnectionTest::unitConnectionWithArticleLinkAddGetRemoveTest()
{
  QFAIL("Not implementd yet");
}

void mdtClUnitConnectionTest::unitConnectorAddGetRemoveTest()
{
  QFAIL("Not implementd yet");
}

void mdtClUnitConnectionTest::unitConnectorWithConnectionsAddGetRemoveTest()
{
  QFAIL("Not implementd yet");
}

/*
 * Test database helper methods
 */

void mdtClUnitConnectionTest::createDatabaseSchema()
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
  mdtClUnitConnectionTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
