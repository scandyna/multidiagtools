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
#include "mdtClArticleConnectionTest.h"
#include "mdtClArticleConnection.h"
#include "mdtClConnectorTest.h"
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

void mdtClArticleConnectionTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabaseManager.database().isOpen());
}

void mdtClArticleConnectionTest::cleanupTestCase()
{
  QFile::remove(pvDbFileInfo.absoluteFilePath());
}

void mdtClArticleConnectionTest::articleConnectorDataTest()
{
  mdtClArticleConnectorKeyData acKeyData;
  mdtClArticleConnectorData acData;
  mdtClArticleConnectionKeyData acnxKey;
  mdtClArticleConnectionData acnxData;
  mdtClConnectorKeyData connectorFk;

  /*
   * Key data test
   */
  // Initial state
  QVERIFY(acKeyData.isNull());
  QVERIFY(!acKeyData.isBasedOnConnector());
  QCOMPARE(acData.connectionDataList().size(), 0);
  // Set a key - Not based on a connector
  acKeyData.id = 1;
  QVERIFY(acKeyData.isNull());
  acKeyData.setArticleId(2);
  QVERIFY(!acKeyData.isNull());
  QVERIFY(!acKeyData.isBasedOnConnector());
  // Clear
  acKeyData.clear();
  QVERIFY(acKeyData.id.isNull());
  QVERIFY(acKeyData.articleId().isNull());
  QVERIFY(acKeyData.connectorFk().isNull());
  QVERIFY(acKeyData.isNull());
  QVERIFY(!acKeyData.isBasedOnConnector());
  QCOMPARE(acData.connectionDataList().size(), 0);
  // Set a key - Based on a connector
  acKeyData.id = 5;
  acKeyData.setArticleId(6);
  connectorFk.id = 7;
  acKeyData.setConnectorFk(connectorFk);
  QVERIFY(!acKeyData.isNull());
  QVERIFY(acKeyData.isBasedOnConnector());
  // Clear
  acKeyData.clear();
  QVERIFY(acKeyData.id.isNull());
  QVERIFY(acKeyData.articleId().isNull());
  QVERIFY(acKeyData.connectorFk().isNull());
  QVERIFY(acKeyData.isNull());
  QVERIFY(!acKeyData.isBasedOnConnector());
  QCOMPARE(acData.connectionDataList().size(), 0);
  /*
   * Data test
   */
  // Initial state
  QVERIFY(acData.keyData().isNull());
  QVERIFY(acData.name.isNull());
  QVERIFY(acData.isNull());
  // Set data - Not based on a connector
  acKeyData.clear();
  acKeyData.id = 4;
  acKeyData.setArticleId(5);
  acData.setKeyData(acKeyData);
  acData.name = "ACNR 4";
  QVERIFY(!acData.isNull());
  QVERIFY(!acData.isBasedOnConnector());
  // Clear
  acData.clear();
  QVERIFY(acData.keyData().isNull());
  QVERIFY(acData.name.isNull());
  QVERIFY(acData.isNull());
  QVERIFY(!acData.isBasedOnConnector());
  QCOMPARE(acData.connectionDataList().size(), 0);
  // Set data - Based on a connector
  acData.clear();
  acKeyData.clear();
  connectorFk.clear();
  acKeyData.id = 7;
  acKeyData.setArticleId(8);
  connectorFk.id = 9;
  acKeyData.setConnectorFk(connectorFk);
  acData.setKeyData(acKeyData);
  acData.name = "ACNR 7";
  QVERIFY(!acData.isNull());
  QVERIFY(acData.isBasedOnConnector());
  QCOMPARE(acData.connectionDataList().size(), 0);
  // Clear
  acData.clear();
  QVERIFY(acData.keyData().isNull());
  QVERIFY(acData.name.isNull());
  QVERIFY(acData.isNull());
  QVERIFY(!acData.isBasedOnConnector());
  QCOMPARE(acData.connectionDataList().size(), 0);
  /*
   * Data test - with connections
   */
  // Set connector data - Based on a connector
  acData.clear();
  acKeyData.clear();
  connectorFk.clear();
  acKeyData.id = 5;
  acKeyData.setArticleId(6);
  connectorFk.id = 7;
  acKeyData.setConnectorFk(connectorFk);
  acData.setKeyData(acKeyData);
  acData.name = "ACNR 5";
  // Add article connection A
  acnxKey.clear();
  acnxData.clear();
  acnxKey.setArticleId(6);
  acnxKey.setConnectionType(mdtClConnectionType_t::Pin);
  acnxData.setKeyData(acnxKey);
  acnxData.name = "A";
  acData.addConnectionData(acnxData);
  // Add article connection B
  acnxKey.clear();
  acnxData.clear();
  acnxKey.setArticleId(6);
  acnxKey.setConnectionType(mdtClConnectionType_t::Socket);
  acnxData.setKeyData(acnxKey);
  acnxData.name = "B";
  acData.addConnectionData(acnxData);
  // Check
  QVERIFY(!acData.isNull());
  QVERIFY(acData.isBasedOnConnector());
  QCOMPARE(acData.connectionDataList().size(), 2);
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleId(), QVariant(6));
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleConnectorFk().articleId(), QVariant(6));
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleConnectorFk().id, QVariant(5));
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleConnectorFk().connectorFk().id, QVariant(7));
  QCOMPARE(acData.connectionDataList().at(0).name, QVariant("A"));
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleId(), QVariant(6));
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleConnectorFk().articleId(), QVariant(6));
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleConnectorFk().id, QVariant(5));
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleConnectorFk().connectorFk().id, QVariant(7));
  QCOMPARE(acData.connectionDataList().at(1).name, QVariant("B"));
  // Update article connectors ID and check
  acData.setId(1);
  QVERIFY(!acData.isNull());
  QVERIFY(acData.isBasedOnConnector());
  QCOMPARE(acData.connectionDataList().size(), 2);
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleId(), QVariant(6));
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleConnectorFk().articleId(), QVariant(6));
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleConnectorFk().id, QVariant(1));
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleConnectorFk().connectorFk().id, QVariant(7));
  QCOMPARE(acData.connectionDataList().at(0).name, QVariant("A"));
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleId(), QVariant(6));
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleConnectorFk().articleId(), QVariant(6));
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleConnectorFk().id, QVariant(1));
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleConnectorFk().connectorFk().id, QVariant(7));
  QCOMPARE(acData.connectionDataList().at(1).name, QVariant("B"));
  // Clear
  acData.clear();
  QVERIFY(acData.keyData().isNull());
  QVERIFY(acData.name.isNull());
  QVERIFY(acData.isNull());
  QVERIFY(!acData.isBasedOnConnector());
  QCOMPARE(acData.connectionDataList().size(), 0);
}

void mdtClArticleConnectionTest::articleConnectionDataTest()
{
  mdtClArticleConnectionKeyData acnxKey;
  mdtClArticleConnectionData acnxData;
  mdtClArticleConnectorKeyData acnrKeyData;

  /*
   * Article connection key data test
   */
  // Initial state
  QVERIFY(acnxKey.isNull());
  QVERIFY(!acnxKey.isPartOfArticleConnector());
  // Set key - Not based on a article connector
  acnxKey.id = 1;
  QVERIFY(acnxKey.isNull());
  acnxKey.setArticleId(2);
  QVERIFY(acnxKey.isNull());
  acnxKey.setConnectionType(mdtClConnectionType_t::Terminal);
  QVERIFY(!acnxKey.isNull());
  QVERIFY(!acnxKey.isPartOfArticleConnector());
  // Clear
  acnxKey.clear();
  QVERIFY(acnxKey.id.isNull());
  QVERIFY(acnxKey.articleId().isNull());
  QVERIFY(acnxKey.articleConnectorFk().isNull());
  QVERIFY(acnxKey.connectionTypeFk().isNull());
  QVERIFY(acnxKey.isNull());
  QVERIFY(!acnxKey.isPartOfArticleConnector());
  // Set key - Based on a article connector
  acnrKeyData.clear();
  acnxKey.id = 3;
  acnxKey.setArticleId(4);
  acnxKey.setConnectionType(mdtClConnectionType_t::Pin);
  acnrKeyData.id = 5;
  acnrKeyData.setArticleId(acnxKey.articleId());
  acnxKey.setArticleConnectorFk(acnrKeyData);
  QVERIFY(!acnxKey.isNull());
  QVERIFY(acnxKey.isPartOfArticleConnector());
  // Clear
  acnxKey.clear();
  QVERIFY(acnxKey.id.isNull());
  QVERIFY(acnxKey.articleId().isNull());
  QVERIFY(acnxKey.articleConnectorFk().isNull());
  QVERIFY(acnxKey.connectionTypeFk().isNull());
  QVERIFY(acnxKey.isNull());
  QVERIFY(!acnxKey.isPartOfArticleConnector());
  /*
   * Article connection data test
   */
  // Initial state
  QVERIFY(acnxData.isNull());
  QVERIFY(!acnxData.isPartOfArticleConnector());
  // Set data - Not part of a article connector
  acnxKey.clear();
  acnxKey.id = 1;
  acnxKey.setArticleId(2);
  acnxKey.setConnectionType(mdtClConnectionType_t::Pin);
  acnxData.setKeyData(acnxKey);
  acnxData.name = "ACNX 1";
  acnxData.resistance = 0.12;
  acnxData.ioType = "DO";
  acnxData.functionFR = "FFR01";
  acnxData.functionEN = "FEN01";
  acnxData.functionDE = "FDE01";
  acnxData.functionIT = "FIT01";
  QVERIFY(!acnxData.isNull());
  QVERIFY(!acnxData.isPartOfArticleConnector());
  QVERIFY(acnxData.connectionType() == mdtClConnectionType_t::Pin);
  // Clear
  acnrKeyData.clear();
  acnxData.clear();
  QVERIFY(acnxData.name.isNull());
  QVERIFY(acnxData.resistance.isNull());
  QVERIFY(acnxData.ioType.isNull());
  QVERIFY(acnxData.functionEN.isNull());
  QVERIFY(acnxData.functionFR.isNull());
  QVERIFY(acnxData.functionDE.isNull());
  QVERIFY(acnxData.functionIT.isNull());
  QVERIFY(acnxData.isNull());
  QVERIFY(!acnxData.isPartOfArticleConnector());
  // Set data - Part of a article connector
  acnxKey.clear();
  acnxKey.id = 3;
  acnxKey.setArticleId(4);
  acnxKey.setConnectionType(mdtClConnectionType_t::Pin);
  acnrKeyData.id = 5;
  acnrKeyData.setArticleId(4);
  acnxKey.setArticleConnectorFk(acnrKeyData);
  acnxData.setKeyData(acnxKey);
  acnxData.name = "ACNX 3";
  acnxData.resistance = 0.25;
  acnxData.ioType = "DO";
  acnxData.functionFR = "FFR03";
  acnxData.functionEN = "FEN03";
  acnxData.functionDE = "FDE03";
  acnxData.functionIT = "FIT03";
  QVERIFY(!acnxData.isNull());
  QVERIFY(acnxData.isPartOfArticleConnector());
  QVERIFY(acnxData.connectionType() == mdtClConnectionType_t::Pin);
  // Clear
  acnxData.clear();
  QVERIFY(acnxData.name.isNull());
  QVERIFY(acnxData.resistance.isNull());
  QVERIFY(acnxData.ioType.isNull());
  QVERIFY(acnxData.functionEN.isNull());
  QVERIFY(acnxData.functionFR.isNull());
  QVERIFY(acnxData.functionDE.isNull());
  QVERIFY(acnxData.functionIT.isNull());
  QVERIFY(acnxData.isNull());
  QVERIFY(!acnxData.isPartOfArticleConnector());
  // Check setting article connector FK
  acnxKey.clear();
  acnxKey.id = 4;
  acnxKey.setArticleId(5);
  acnxKey.setConnectionType(mdtClConnectionType_t::Socket);
  acnxData.setKeyData(acnxKey);
  acnrKeyData.clear();
  acnrKeyData.id = 6;
  acnrKeyData.setArticleId(5);
  acnxKey.setArticleId(5);
  acnxData.setArticleConnectorFk(acnrKeyData);
  QVERIFY(!acnxData.isNull());
  QVERIFY(acnxData.isPartOfArticleConnector());
  QVERIFY(acnxData.connectionType() == mdtClConnectionType_t::Socket);
  QVERIFY(acnxData.keyData().articleConnectorFk().id == acnrKeyData.id);
  // Clear
  acnxData.clear();
  QVERIFY(acnxData.name.isNull());
  QVERIFY(acnxData.resistance.isNull());
  QVERIFY(acnxData.ioType.isNull());
  QVERIFY(acnxData.functionEN.isNull());
  QVERIFY(acnxData.functionFR.isNull());
  QVERIFY(acnxData.functionDE.isNull());
  QVERIFY(acnxData.functionIT.isNull());
  QVERIFY(acnxData.isNull());
  QVERIFY(!acnxData.isPartOfArticleConnector());
}

void mdtClArticleConnectionTest::articleConnectionAddGetRemoveTest()
{
  mdtClArticleConnection acnx(pvDatabaseManager.database());
  mdtClArticleConnectionKeyData key;
  mdtClArticleConnectionData data;
  mdtCableListTestScenario scenario(pvDatabaseManager.database());
  bool ok;

  /*
   * Create articles
   */
  scenario.createTestArticles();
  /*
   * Article connection:
   *  - Belongs to article 1
   *  - Not part of a article connector
   *  - Type: Socket
   */
  // Setup key data
  key.setArticleId(1);
  key.setConnectionType(mdtClConnectionType_t::Socket);
  // Setup article connection data
  data.setKeyData(key);
  data.name = "A";
  data.ioType = "DO";
  data.functionEN = "FEN";
  data.functionFR = "FFR";
  data.functionDE = "FDE";
  data.functionIT = "FIT";
  // Add article connection to database
  key = acnx.addArticleConnection(data, true);
  QVERIFY(!key.isNull());
  // Get data back and check
  data = acnx.getArticleConnectionData(key, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().articleId(), QVariant(1));
  QVERIFY(!data.isPartOfArticleConnector());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(data.name, QVariant("A"));
  QVERIFY(data.resistance.isNull());
  QCOMPARE(data.ioType, QVariant("DO"));
  QCOMPARE(data.functionEN, QVariant("FEN"));
  QCOMPARE(data.functionFR, QVariant("FFR"));
  QCOMPARE(data.functionDE, QVariant("FDE"));
  QCOMPARE(data.functionIT, QVariant("FIT"));
  /*
   * Update article connection:
   *  - Belongs to article 2
   *  - Not part of a article connector
   *  - Type: Pin
   */
  // Update data
  key = data.keyData();
  key.setArticleId(2);
  key.setConnectionType(mdtClConnectionType_t::Pin);
  data.setKeyData(key);
  data.name = "B";
  data.resistance = 0.12;
  data.ioType = "DI";
  data.functionEN = "FEN 2";
  data.functionFR = "FFR 2";
  data.functionDE = "FDE 2";
  data.functionIT = "FIT 2";
  // Update in database
  QVERIFY(acnx.updateArticleConnection(key, data));
  // Get data back and check
  data = acnx.getArticleConnectionData(key, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().articleId(), QVariant(2));
  QVERIFY(!data.isPartOfArticleConnector());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Pin);
  QCOMPARE(data.name, QVariant("B"));
  QCOMPARE(data.resistance, QVariant(0.12));
  QCOMPARE(data.ioType, QVariant("DI"));
  QCOMPARE(data.functionEN, QVariant("FEN 2"));
  QCOMPARE(data.functionFR, QVariant("FFR 2"));
  QCOMPARE(data.functionDE, QVariant("FDE 2"));
  QCOMPARE(data.functionIT, QVariant("FIT 2"));
  // Remove connection
  QVERIFY(acnx.removeArticleConnection(key));
  data = acnx.getArticleConnectionData(key, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
}

void mdtClArticleConnectionTest::articleConnectorAddGetRemoveTest()
{
  mdtClArticleConnection acnx(pvDatabaseManager.database());
  mdtClArticleConnectorKeyData key1, key2;
  mdtClArticleConnectorData acData;
  mdtClArticleConnectionKeyData acnxKey;
  mdtClArticleConnectionData acnxData;
  mdtClConnectorKeyData connectorFk;
  mdtClConnectorContactKeyData ccKey;
  mdtClConnectorContactData ccData;
  QList<mdtClConnectorContactData> ccDataList;
  mdtCableListTestScenario scenario(pvDatabaseManager.database());
  bool ok;

  /*
   * Create articles
   */
  scenario.createTestConnectors();
  scenario.createTestArticles();
  /*
   * Article connector:
   *  - Not based on a connector
   *  - No connections
   */
  // Setup data
  key1.setArticleId(1);
  acData.setKeyData(key1);
  acData.name = "ACNR 1";
  // Add to database
  key1 = acnx.addArticleConnector(acData, true);
  QVERIFY(!key1.isNull());
  // Get data and check back
  acData = acnx.getArticleConnectorData(key1, true, ok);
  QVERIFY(ok);
  QVERIFY(!acData.isNull());
  QVERIFY(!acData.isBasedOnConnector());
  QCOMPARE(acData.keyData().articleId(), QVariant(1));
  QCOMPARE(acData.name, QVariant("ACNR 1"));
  /*
   * Article connector:
   *  - Based on connector 1
   *  - No connections
   */
  // Setup data
  key2.clear();
  acData.clear();
  key2.setArticleId(1);
  connectorFk.id = 1;
  key2.setConnectorFk(connectorFk);
  acData.setKeyData(key2);
  acData.name = "ACNR 2";
  // Add to database
  key2 = acnx.addArticleConnector(acData, true);
  QVERIFY(!key2.isNull());
  // Get data and check back
  acData = acnx.getArticleConnectorData(key2, true, ok);
  QVERIFY(ok);
  QVERIFY(!acData.isNull());
  QVERIFY(acData.isBasedOnConnector());
  QCOMPARE(acData.keyData().connectorFk().id, QVariant(1));
  QCOMPARE(acData.keyData().articleId(), QVariant(1));
  QCOMPARE(acData.name, QVariant("ACNR 2"));
  /*
   * Remove connectors 1 and 2
   */
  // Remove connector 1
  QVERIFY(acnx.removeArticleConnector(key1, true));
  acData = acnx.getArticleConnectorData(key1, true, ok);
  QVERIFY(ok);
  QVERIFY(acData.isNull());
  // Remove connector 2
  QVERIFY(acnx.removeArticleConnector(key2, true));
  acData = acnx.getArticleConnectorData(key2, true, ok);
  QVERIFY(ok);
  QVERIFY(acData.isNull());
  /*
   * Article connector:
   *  - Not based on a connector
   *  - 2 connections:
   *   -> A , Pin
   *   -> B , Socket
   */
  // Setup article connector data
  key1.clear();
  acData.clear();
  key1.setArticleId(1);
  acData.setKeyData(key1);
  acData.name = "ACNR 1";
  // Add article connection A
  acnxKey.clear();
  acnxData.clear();
  acnxKey.setArticleId(1);
  acnxKey.setConnectionType(mdtClConnectionType_t::Pin);
  acnxData.setKeyData(acnxKey);
  acnxData.name = "A";
  acData.addConnectionData(acnxData);
  // Add article connection B
  acnxKey.clear();
  acnxData.clear();
  acnxKey.setArticleId(1);
  acnxKey.setConnectionType(mdtClConnectionType_t::Socket);
  acnxData.setKeyData(acnxKey);
  acnxData.name = "B";
  acData.addConnectionData(acnxData);
  // Add to database
  key1 = acnx.addArticleConnector(acData, true);
  QVERIFY(!key1.isNull());
  // Get data and check back
  acData = acnx.getArticleConnectorData(key1, true, ok);
  QVERIFY(ok);
  QVERIFY(!acData.isNull());
  QVERIFY(!acData.isBasedOnConnector());
  QCOMPARE(acData.keyData().articleId(), QVariant(1));
  QCOMPARE(acData.name, QVariant("ACNR 1"));
  QCOMPARE(acData.connectionDataList().size(), 2);
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleId(), QVariant(1));
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleConnectorFk().articleId(), QVariant(1));
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleConnectorFk().id, key1.id);
  QVERIFY(acData.connectionDataList().at(0).keyData().articleConnectorFk().connectorFk().isNull());
  QVERIFY(acData.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Pin);
  QCOMPARE(acData.connectionDataList().at(0).name, QVariant("A"));
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleId(), QVariant(1));
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleConnectorFk().articleId(), QVariant(1));
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleConnectorFk().id, key1.id);
  QVERIFY(acData.connectionDataList().at(1).keyData().articleConnectorFk().connectorFk().isNull());
  QVERIFY(acData.connectionDataList().at(1).connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(acData.connectionDataList().at(1).name, QVariant("B"));
  // Get data without article connections
  acData = acnx.getArticleConnectorData(key1, false, ok);
  QVERIFY(ok);
  QVERIFY(!acData.isNull());
  QVERIFY(!acData.isBasedOnConnector());
  QCOMPARE(acData.keyData().articleId(), QVariant(1));
  QCOMPARE(acData.name, QVariant("ACNR 1"));
  QCOMPARE(acData.connectionDataList().size(), 0);
  /*
   * Article connector:
   *  - Based on connector 1
   *  - 2 connections:
   *   -> C , Pin
   *   -> D , Socket
   */
  ccDataList.clear();
  // Setup article connector data
  key2.clear();
  connectorFk.clear();
  acData.clear();
  key2.setArticleId(1);
  connectorFk.id = 1;
  key2.setConnectorFk(connectorFk);
  acData.setKeyData(key2);
  acData.name = "ACNR 2";
  // Setup connector contact C
  connectorFk.clear();
  ccData.clear();
  ccKey.clear();
  connectorFk.id = 1;
  ccKey.setConnectorFk(connectorFk);
  ccKey.setConnectionType(mdtClConnectionType_t::Pin);
  ccData.setKeyData(ccKey);
  ccData.name = "C";
  ccDataList.append(ccData);
  // Setup connector contact D
  connectorFk.clear();
  ccData.clear();
  ccKey.clear();
  connectorFk.id = 1;
  ccKey.setConnectorFk(connectorFk);
  ccKey.setConnectionType(mdtClConnectionType_t::Socket);
  ccData.setKeyData(ccKey);
  ccData.name = "D";
  ccDataList.append(ccData);
  // Add article connections to article connector
  acnx.addConnectionsToArticleConnector(acData, ccDataList);
  // Add to database
  key2 = acnx.addArticleConnector(acData, true);
  QVERIFY(!key2.isNull());
  qDebug() << "Added CNR: " << key2.id;
  // Get data and check back
  acData = acnx.getArticleConnectorData(key2, true, ok);
  QVERIFY(ok);
  QVERIFY(!acData.isNull());
  QVERIFY(acData.isBasedOnConnector());
  QCOMPARE(acData.keyData().articleId(), QVariant(1));
  QCOMPARE(acData.name, QVariant("ACNR 2"));
  QCOMPARE(acData.connectionDataList().size(), 2);
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleId(), QVariant(1));
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleConnectorFk().articleId(), QVariant(1));
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleConnectorFk().id, key2.id);
  QCOMPARE(acData.connectionDataList().at(0).keyData().articleConnectorFk().connectorFk().id, QVariant(1));
  QVERIFY(acData.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Pin);
  QCOMPARE(acData.connectionDataList().at(0).name, QVariant("C"));
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleId(), QVariant(1));
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleConnectorFk().articleId(), QVariant(1));
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleConnectorFk().id, key2.id);
  QCOMPARE(acData.connectionDataList().at(1).keyData().articleConnectorFk().connectorFk().id, QVariant(1));
  QVERIFY(acData.connectionDataList().at(1).connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(acData.connectionDataList().at(1).name, QVariant("D"));
  // Get data without article connections
  acData = acnx.getArticleConnectorData(key2, false, ok);
  QVERIFY(ok);
  QVERIFY(!acData.isNull());
  QVERIFY(acData.isBasedOnConnector());
  QCOMPARE(acData.keyData().articleId(), QVariant(1));
  QCOMPARE(acData.name, QVariant("ACNR 2"));
  QCOMPARE(acData.connectionDataList().size(), 0);
  // Edit connector name
  QVERIFY(acnx.updateArticleConnectorName(key2, "ACNR 22"));
  acData = acnx.getArticleConnectorData(key2, false, ok);
  QVERIFY(ok);
  QVERIFY(!acData.isNull());
  QCOMPARE(acData.name, QVariant("ACNR 22"));
  /*
   * Remove connectors 1 and 2
   */
  // Remove connector 1
  QVERIFY(acnx.removeArticleConnector(key1, true));
  acData = acnx.getArticleConnectorData(key1, true, ok);
  QVERIFY(ok);
  QVERIFY(acData.isNull());
  // Remove connector 2
  QVERIFY(acnx.removeArticleConnector(key2, true));
  acData = acnx.getArticleConnectorData(key2, true, ok);
  QVERIFY(ok);
  QVERIFY(acData.isNull());
}

/*
 * Test database helper methods
 */

void mdtClArticleConnectionTest::createDatabaseSchema()
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
  mdtClArticleConnectionTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
