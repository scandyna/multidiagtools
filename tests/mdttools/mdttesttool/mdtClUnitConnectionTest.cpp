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
#include "mdtClUnitConnectionSelectionDialog.h"
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
  mdtClUnitConnectorPkData pk;
  mdtClUnitConnectorKeyData key;
  mdtClUnitConnectorData data;
  mdtClConnectorKeyData cnrKey;
  mdtClArticleConnectorKeyData acnrKey;

  /*
   * PK data test
   */
  // initial state
  QVERIFY(pk.isNull());
  // Set
  pk.id = 1;
  QVERIFY(!pk.isNull());
  // Clear
  pk.clear();
  QVERIFY(pk.id.isNull());
  QVERIFY(pk.isNull());
  /*
   * Key data test
   */
  // Initial state
  QVERIFY(key.isNull());
  QVERIFY(!key.isBasedOnConnector());
  QVERIFY(!key.isBasedOnArticleConnector());
  // Set key for a free unit connector
  key.pk.id = 1;
  QVERIFY(key.isNull());
  key.setUnitId(2);
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(2));
  QVERIFY(!key.isBasedOnConnector());
  QVERIFY(!key.isBasedOnArticleConnector());
  // Clear
  key.clear();
  QVERIFY(key.pk.isNull());
  QVERIFY(key.unitId().isNull());
  QVERIFY(key.connectorFk().isNull());
  QVERIFY(key.articleConnectorFk().isNull());
  QVERIFY(key.isNull());
  QVERIFY(!key.isBasedOnConnector());
  QVERIFY(!key.isBasedOnArticleConnector());
  // Set key for a unit connector based on a connector
  cnrKey.clear();
  key.pk.id = 3;
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
  QVERIFY(key.pk.isNull());
  QVERIFY(key.unitId().isNull());
  QVERIFY(key.connectorFk().isNull());
  QVERIFY(key.articleConnectorFk().isNull());
  QVERIFY(key.isNull());
  QVERIFY(!key.isBasedOnConnector());
  QVERIFY(!key.isBasedOnArticleConnector());
  // Set key for a unit connector based on a free article connector
  acnrKey.clear();
  key.pk.id = 6;
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
  QVERIFY(key.pk.isNull());
  QVERIFY(key.unitId().isNull());
  QVERIFY(key.connectorFk().isNull());
  QVERIFY(key.articleConnectorFk().isNull());
  QVERIFY(key.isNull());
  QVERIFY(!key.isBasedOnConnector());
  QVERIFY(!key.isBasedOnArticleConnector());
  // Set key for a unit connector based on a article connector that is based on a connector
  cnrKey.clear();
  acnrKey.clear();
  key.pk.id = 9;
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
  QVERIFY(key.pk.isNull());
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
  key.pk.id = 5;
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
  key.pk.id = 2;
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
  key.pk.id = 3;
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
  key.pk.id = 5;
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
  mdtClUnitConnectionPkData pk;
  mdtClUnitConnectionKeyData key;
  mdtClUnitConnectionData data;
  mdtClArticleConnectionKeyData articleConnectionFk;
  mdtClUnitConnectorKeyData unitConnectorFk;
  mdtClArticleConnectorKeyData articleConnectorFk;

  /*
   * PK data test
   */
  // Initial state
  QVERIFY(pk.isNull());
  // Set
  pk.id = 1;
  QVERIFY(!pk.isNull());
  // Clear
  pk.clear();
  QVERIFY(pk.id.isNull());
  QVERIFY(pk.isNull());
  /*
   * Key data test
   */
  // Initial state
  QVERIFY(key.isNull());
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  // Set key for a unit free unit connection
  key.pk.id = 1;
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
  QVERIFY(key.pk.isNull());
  QVERIFY(key.unitId().isNull());
  QVERIFY(key.unitConnectorFk().isNull());
  QVERIFY(key.articleConnectionFk().isNull());
  QVERIFY(key.connectionTypeFk().isNull());
  QVERIFY(key.isNull());
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  // Set key for a unit connection that is part of a free unit connector
  unitConnectorFk.clear();
  key.pk.id = 4;
  key.setUnitId(5);
  key.setConnectionType(mdtClConnectionType_t::Pin);
  unitConnectorFk.setUnitId(5);
  key.setUnitConnectorFk(unitConnectorFk);
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(5));
  QVERIFY(key.connectionTypeFk().type() == mdtClConnectionType_t::Pin);
  QVERIFY(key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  // Clear unit connector FK
  key.clearUnitConnectorFk();
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(5));
  QVERIFY(key.connectionTypeFk().type() == mdtClConnectionType_t::Pin);
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  // Set unit connector FK again
  key.setUnitConnectorFk(unitConnectorFk);
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(5));
  QVERIFY(key.connectionTypeFk().type() == mdtClConnectionType_t::Pin);
  QVERIFY(key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  // Clear
  key.clear();
  QVERIFY(key.pk.isNull());
  QVERIFY(key.unitId().isNull());
  QVERIFY(key.unitConnectorFk().isNull());
  QVERIFY(key.articleConnectionFk().isNull());
  QVERIFY(key.connectionTypeFk().isNull());
  QVERIFY(key.isNull());
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  // Set key for a unit connection based on a free article connection
  articleConnectionFk.clear();
  key.pk.id = 6;
  key.setUnitId(7);
  articleConnectionFk.setArticleId(8);
  articleConnectionFk.setConnectionType(mdtClConnectionType_t::Socket);
  key.setArticleConnectionFk(articleConnectionFk);
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(7));
  QVERIFY(key.connectionTypeFk().type() == mdtClConnectionType_t::Socket);
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(key.isBasedOnArticleConnection());
  // Clear article connection FK
  key.clearArticleConnectionFk();
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(7));
  QVERIFY(key.connectionTypeFk().type() == mdtClConnectionType_t::Socket);
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  // Set article connection again
  key.setArticleConnectionFk(articleConnectionFk);
  QVERIFY(!key.isNull());
  QCOMPARE(key.unitId(), QVariant(7));
  QVERIFY(key.connectionTypeFk().type() == mdtClConnectionType_t::Socket);
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(key.isBasedOnArticleConnection());
  // Clear
  key.clear();
  QVERIFY(key.pk.isNull());
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
  key.pk.id = 3;
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
  QVERIFY(key.pk.isNull());
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
  key.pk.id = 5;
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
  key.pk.id = 1;
  key.setUnitId(2);
  unitConnectorFk.setUnitId(2);
  key.setUnitConnectorFk(unitConnectorFk);
  key.setConnectionType(mdtClConnectionType_t::Pin);
  data.setKeyData(key);
  QVERIFY(!data.isNull());
  QVERIFY(data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Pin);
  // Clear unit connector FK
  data.clearUnitConnectorFk();
  QVERIFY(!data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Pin);
  // Set unit connector FK again
  data.setUnitConnectorFk(unitConnectorFk);
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
  key.pk.id = 5;
  key.setUnitId(6);
  articleConnectionFk.setArticleId(7);
  articleConnectionFk.setConnectionType(mdtClConnectionType_t::Socket);
  key.setArticleConnectionFk(articleConnectionFk);
  data.setKeyData(key);
  QVERIFY(!data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(data.isBasedOnArticleConnection());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Socket);
  // Clear article connection FK
  data.clearArticleConnectionFk();
  QVERIFY(!data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Socket);
  // Set article connection again
  data.setArticleConnectionFk(articleConnectionFk);
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
  key.pk.id = 1;
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

void mdtClUnitConnectionTest::unitConnectorWithConnectionsDataTest()
{
  mdtClUnitConnectorKeyData unitConnectorKey;
  mdtClUnitConnectorData unitConnectorData;
  mdtClUnitConnectionKeyData unitConnectionKey;
  mdtClUnitConnectionData unitConnectionData;
  mdtClArticleConnectorKeyData articleConnectorFk;
  mdtClArticleConnectionKeyData articleConnectionFk;

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
  // Update unit connectors ID
  unitConnectorData.setId(10);
  // Check
  QCOMPARE(unitConnectorData.connectionDataList().size(), 2);
  QCOMPARE(unitConnectorData.connectionDataList().at(0).keyData().unitId(), QVariant(1));
  QCOMPARE(unitConnectorData.connectionDataList().at(0).keyData().unitConnectorFk().unitId(), QVariant(1));
  QCOMPARE(unitConnectorData.connectionDataList().at(0).keyData().unitConnectorFk().pk.id, QVariant(10));
  QVERIFY(unitConnectorData.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Pin);
  QCOMPARE(unitConnectorData.connectionDataList().at(0).name, QVariant("A"));
  QCOMPARE(unitConnectorData.connectionDataList().at(1).keyData().unitId(), QVariant(1));
  QCOMPARE(unitConnectorData.connectionDataList().at(1).keyData().unitConnectorFk().unitId(), QVariant(1));
  QCOMPARE(unitConnectorData.connectionDataList().at(1).keyData().unitConnectorFk().pk.id, QVariant(10));
  QVERIFY(unitConnectorData.connectionDataList().at(1).connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(unitConnectorData.connectionDataList().at(1).name, QVariant("B"));
  // Clear
  unitConnectorData.clear();
  QCOMPARE(unitConnectorData.connectionDataList().size(), 0);
  /*
   * Setup a unit connector based on a article connector
   */
  // Setup article connector
  articleConnectorFk.clear();
  articleConnectorFk.setArticleId(1);
  // Setup unit connector
  unitConnectorKey.clear();
  unitConnectorKey.setUnitId(2);
  unitConnectorKey.setArticleConnectorFk(articleConnectorFk);
  unitConnectorData.setKeyData(unitConnectorKey);
  // Setup a article connection
  articleConnectionFk.clear();
  articleConnectionFk.setArticleId(1);
  articleConnectionFk.setArticleConnectorFk(articleConnectorFk);
  articleConnectionFk.setConnectionType(mdtClConnectionType_t::Pin);
  // Setup a unit connection and add it
  unitConnectionKey.clear();
  unitConnectionKey.setArticleConnectionFk(articleConnectionFk);
  unitConnectionData.setKeyData(unitConnectionKey);
  unitConnectionData.name = "C";
  unitConnectorData.addConnectionData(unitConnectionData);
  // Check
  QCOMPARE(unitConnectorData.connectionDataList().size(), 1);
  QCOMPARE(unitConnectorData.connectionDataList().at(0).keyData().unitId(), QVariant(2));
  QCOMPARE(unitConnectorData.connectionDataList().at(0).keyData().unitConnectorFk().unitId(), QVariant(2));
  QCOMPARE(unitConnectorData.connectionDataList().at(0).keyData().articleConnectionFk().articleId(), QVariant(1));
  QVERIFY(unitConnectorData.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Pin);
  QVERIFY(unitConnectorData.connectionDataList().at(0).isPartOfUnitConnector());
  QVERIFY(unitConnectorData.connectionDataList().at(0).isBasedOnArticleConnection());
  QCOMPARE(unitConnectorData.connectionDataList().at(0).name, QVariant("C"));
  // Update unit connector ID
  unitConnectorData.setId(30);
  QCOMPARE(unitConnectorData.connectionDataList().at(0).keyData().unitConnectorFk().pk.id, QVariant(30));
  
  /// \test Add other combinaisons
}

void mdtClUnitConnectionTest::unitConnectorDataFromConnectorContactsTest()
{
  mdtClUnitConnection ucnx(pvDatabaseManager.database());
  mdtClUnitConnectorKeyData key;
  mdtClUnitConnectorData data;
  mdtClConnectorKeyData connectorFk;
  mdtClConnectorContactKeyData contactKey;
  mdtClConnectorContactData contactData;
  QList<mdtClConnectorContactData> contactDataList;

  // Setup unit connector
  key.setUnitId(1);
  data.setKeyData(key);
  // Setup connector
  connectorFk.id = 10;
  // Setup a connector contact and add to list
  contactKey.clear();
  contactData.clear();
  contactData.setConnectorFk(connectorFk);
  contactData.setConnectionType(mdtClConnectionType_t::Pin);
  contactData.name = "A";
  contactDataList.append(contactData);
  // Setup a connector contact and add to list
  contactKey.clear();
  contactData.clear();
  contactData.setConnectorFk(connectorFk);
  contactData.setConnectionType(mdtClConnectionType_t::Socket);
  contactData.name = "B";
  contactDataList.append(contactData);
  // Add contacts to unit connector
  ucnx.addConnectionsToUnitConnector(data, contactDataList);
  // Check
  QCOMPARE(data.connectionDataList().size(), 2);
  QVERIFY(data.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Pin);
  QCOMPARE(data.connectionDataList().at(0).name, QVariant("A"));
  QVERIFY(data.connectionDataList().at(1).connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(data.connectionDataList().at(1).name, QVariant("B"));
}

void mdtClUnitConnectionTest::unitConnectorDataFromArticleConnectionListTest()
{
  mdtClUnitConnection ucnx(pvDatabaseManager.database());
  mdtClUnitConnectorKeyData key;
  mdtClUnitConnectorData data;
  mdtClArticleConnectorKeyData articleConnectorFk;
  mdtClArticleConnectorData articleConnectorData;
  mdtClArticleConnectionData articleConnectionData;
  ///QList<mdtClArticleConnectionData> articleConnectionDataList;

  // Setup article connector
  articleConnectorFk.setArticleId(1);
  articleConnectorData.setKeyData(articleConnectorFk);
  // Setup unit connector
  key.setUnitId(2);
  key.setArticleConnectorFk(articleConnectorFk);
  data.setKeyData(key);
  // Setup a article connection and add to list
  articleConnectionData.clear();
  articleConnectionData.setConnectionType(mdtClConnectionType_t::Pin);
  articleConnectionData.name = "A";
  articleConnectorData.addConnectionData(articleConnectionData);
  // Setup a article connection and add to list
  articleConnectionData.clear();
  articleConnectionData.setConnectionType(mdtClConnectionType_t::Socket);
  articleConnectionData.name = "B";
  articleConnectorData.addConnectionData(articleConnectionData);
  // Build unit connection list
  ucnx.addConnectionsToUnitConnector(data, articleConnectorData.connectionDataList());
  // Check
  QCOMPARE(data.connectionDataList().size(), 2);
  ///QCOMPARE(data.connectionDataList().at(0).keyData().articleConnectionFk().articleConnectorFk().articleId(), QVariant(1));
  QCOMPARE(data.connectionDataList().at(0).keyData().articleConnectionFk().articleId(), QVariant(1));
  QCOMPARE(data.connectionDataList().at(0).keyData().unitId(), QVariant(2));
  QVERIFY(data.connectionDataList().at(0).isPartOfUnitConnector());
  QVERIFY(data.connectionDataList().at(0).isBasedOnArticleConnection());
  QVERIFY(data.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Pin);
  QCOMPARE(data.connectionDataList().at(0).name, QVariant("A"));
  QCOMPARE(data.connectionDataList().at(1).keyData().articleConnectionFk().articleConnectorFk().articleId(), QVariant(1));
  QCOMPARE(data.connectionDataList().at(1).keyData().articleConnectionFk().articleId(), QVariant(1));
  QCOMPARE(data.connectionDataList().at(1).keyData().unitId(), QVariant(2));
  QVERIFY(data.connectionDataList().at(1).isPartOfUnitConnector());
  QVERIFY(data.connectionDataList().at(1).isBasedOnArticleConnection());
  QVERIFY(data.connectionDataList().at(1).connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(data.connectionDataList().at(1).name, QVariant("B"));
}

void mdtClUnitConnectionTest::unitConnectionAddGetRemoveTest()
{
  mdtClUnitConnection ucnx(pvDatabaseManager.database());
  mdtCableListTestScenario scenario(pvDatabaseManager.database());
  mdtClUnitConnectionPkData pk1, pk2;
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
  pk1 = ucnx.addUnitConnection(data, true);
  QVERIFY(!pk1.isNull());
  // Get data from database and check
  data = ucnx.getUnitConnectionData(pk1, ok);
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
  // Edit unit connection
  data.name = "C";
  data.resistance = 1.2;
  data.schemaPage = "30";
  data.signalName = "Name_C";
  data.swAddress = 300;
  data.functionEN = "FEN2";
  data.functionFR = "FFR2";
  data.functionDE = "FDE2";
  data.functionIT = "FIT2";
  QVERIFY(ucnx.updateUnitConnection(pk1, data));
  // Get data from database and check
  data = ucnx.getUnitConnectionData(pk1, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(1000));
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Terminal);
  QCOMPARE(data.name, QVariant("C"));
  QCOMPARE(data.resistance, QVariant(1.2));
  QCOMPARE(data.schemaPage, QVariant("30"));
  QCOMPARE(data.signalName, QVariant("Name_C"));
  QCOMPARE(data.swAddress, QVariant(300));
  QCOMPARE(data.functionEN, QVariant("FEN2"));
  QCOMPARE(data.functionFR, QVariant("FFR2"));
  QCOMPARE(data.functionDE, QVariant("FDE2"));
  QCOMPARE(data.functionIT, QVariant("FIT2"));
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
  pk2 = ucnx.addUnitConnection(data, true);
  QVERIFY(!pk2.isNull());
  // Get data from database and check
  data = ucnx.getUnitConnectionData(pk2, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(2000));
  QCOMPARE(data.keyData().articleConnectionFk().id, QVariant(20));
  QCOMPARE(data.keyData().articleConnectionFk().articleId(), QVariant(2));
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(data.isBasedOnArticleConnection());
  QVERIFY(data.connectionType() == mdtClConnectionType_t::Terminal);
  QCOMPARE(data.name, QVariant("B"));
  // Check that getting unit connection for given article connection key also works
  data = ucnx.getUnitConnectionData(articleConnectionFk, ok);
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
  QVERIFY(ucnx.removeUnitConnection(pk1));
  data = ucnx.getUnitConnectionData(pk1, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  // Remove connection key2
  QVERIFY(ucnx.removeUnitConnection(pk2));
  data = ucnx.getUnitConnectionData(pk2, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
}

void mdtClUnitConnectionTest::unitConnectionWithArticleLinkAddGetRemoveTest()
{
  QFAIL("Not implementd yet");
}

void mdtClUnitConnectionTest::unitConnectorAddGetRemoveTest()
{
  mdtClUnitConnection ucnx(pvDatabaseManager.database());
  mdtClUnitConnectorPkData pk1, pk2, pk3;
  mdtClUnitConnectorKeyData key1, key2, key3;
  mdtClUnitConnectorData data;
  mdtClConnectorKeyData connectorFk;
  mdtClArticleConnectorKeyData articleConnectorFk;
  mdtClArticleConnectionKeyData articleConnectionKey;
  mdtCableListTestScenario scenario(pvDatabaseManager.database());
  bool ok;

  /*
   * Populate data
   */
  scenario.createTestConnectors();
  scenario.createTestArticles();
  scenario.createTestArticleConnectors();
  scenario.createTestUnits();
  /*
   * Add a free unit connector
   */
  // Setup unit connector key
  key1.setUnitId(1000);
  // Setup unit connector data
  data.clear();
  data.setKeyData(key1);
  data.name = "X1";
  // Add to database
  pk1 = ucnx.addUnitConnector(data, true);
  QVERIFY(!pk1.isNull());
  // Get data back and check
  data = ucnx.getUnitConnectorData(pk1, true, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(1000));
  QCOMPARE(data.name, QVariant("X1"));
  QVERIFY(!data.isBasedOnConnector());
  QVERIFY(!data.isBasedOnArticleConnector());
  QCOMPARE(data.connectionDataList().size(), 0);
  // Edit connector name
  QVERIFY(ucnx.updateUnitConnectorName(pk1, "X11"));
  // Get data back and check
  data = ucnx.getUnitConnectorData(pk1, true, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(1000));
  QCOMPARE(data.name, QVariant("X11"));
  QVERIFY(!data.isBasedOnConnector());
  QVERIFY(!data.isBasedOnArticleConnector());
  QCOMPARE(data.connectionDataList().size(), 0);
  /*
   * Add a unit connector based on a connector (from Connector_tbl)
   */
  // Setup connector FK
  connectorFk.clear();
  connectorFk.id = 1;
  // Setup unit connector key
  key2.setUnitId(1000);
  key2.setConnectorFk(connectorFk);
  // Setup unit connector data
  data.clear();
  data.setKeyData(key2);
  data.name = "X2";
  // Add to database
  pk2 = ucnx.addUnitConnector(data, true);
  QVERIFY(!pk2.isNull());
  // Get data back and check
  data = ucnx.getUnitConnectorData(pk2, true, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(1000));
  QCOMPARE(data.keyData().connectorFk().id, QVariant(1));
  QCOMPARE(data.name, QVariant("X2"));
  QVERIFY(data.isBasedOnConnector());
  QVERIFY(!data.isBasedOnArticleConnector());
  QCOMPARE(data.connectionDataList().size(), 0);
  /*
   * Add a unit connector based on a article connector
   */
  // Setup article connector FK
  articleConnectorFk.clear();
  articleConnectorFk.id = 200;
  articleConnectorFk.setArticleId(2);
  // Setup unit connector key
  key3.setUnitId(2000);
  key3.setArticleConnectorFk(articleConnectorFk);
  // Setup unit connector data
  data.clear();
  data.setKeyData(key3);
  data.name = "X3";
  // Add to database
  pk3 = ucnx.addUnitConnector(data, true);
  QVERIFY(!pk3.isNull());
  // Get data back and check
  data = ucnx.getUnitConnectorData(pk3, true, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(2000));
  QCOMPARE(data.keyData().articleConnectorFk().id, QVariant(200));
  QCOMPARE(data.keyData().articleConnectorFk().articleId(), QVariant(2));
  QCOMPARE(data.keyData().articleConnectorFk().connectorFk().id, QVariant(1));
  QCOMPARE(data.keyData().connectorFk().id, QVariant(1));
  QCOMPARE(data.name, QVariant("X3"));
  QVERIFY(data.isBasedOnConnector());
  QVERIFY(data.isBasedOnArticleConnector());
  QCOMPARE(data.connectionDataList().size(), 0);
  // Check getting unit connector key from article connection key
  articleConnectionKey.clear();
  articleConnectionKey.id = 25;
  key3 = ucnx.getUnitConnectorKeyData(articleConnectionKey, 2000, ok);
  QVERIFY(ok);
  QVERIFY(!key3.isNull());
  QCOMPARE(key3.unitId(), QVariant(2000));
  QCOMPARE(key3.articleConnectorFk().id, QVariant(200));
  QCOMPARE(key3.articleConnectorFk().articleId(), QVariant(2));
  QCOMPARE(key3.articleConnectorFk().connectorFk().id, QVariant(1));
  QCOMPARE(key3.connectorFk().id, QVariant(1));
  QVERIFY(key3.isBasedOnConnector());
  QVERIFY(key3.isBasedOnArticleConnector());
  /*
   * Remove connectors
   */
  // Remove connector pk1
  QVERIFY(ucnx.removeUnitConnector(pk1, true));
  data = ucnx.getUnitConnectorData(pk1, true, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  // Remove connector pk2
  QVERIFY(ucnx.removeUnitConnector(pk2, true));
  data = ucnx.getUnitConnectorData(pk2, true, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  // Remove connector pk3
  QVERIFY(ucnx.removeUnitConnector(pk3, true));
  data = ucnx.getUnitConnectorData(pk3, true, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
}

void mdtClUnitConnectionTest::unitConnectorWithConnectionsAddGetRemoveTest()
{
  mdtClUnitConnection ucnx(pvDatabaseManager.database());
  mdtClUnitConnectorPkData pk1, pk2;
  mdtClUnitConnectorKeyData key1, key2;
  mdtClUnitConnectorData data;
  mdtClUnitConnectionKeyData unitConnectionKey;
  mdtClUnitConnectionData unitConnectionsData;
  mdtClArticleConnectorKeyData articleConnectorFk;
  mdtClArticleConnectionKeyData articleConnectionFk;
  mdtCableListTestScenario scenario(pvDatabaseManager.database());
  bool ok;

  /*
   * Populate data
   */
  scenario.createTestConnectors();
  scenario.createTestArticles();
  scenario.createTestArticleConnectors();
  scenario.createTestUnits();

  /*
   * Free unit connector with
   */
  // Setup unit connector key
  key1.setUnitId(1000);
  // Setup unit connector data
  data.clear();
  data.setKeyData(key1);
  data.name = "X1";
  // Setup unit connection data and add to unit connector
  unitConnectionsData.clear();
  unitConnectionsData.setConnectionType(mdtClConnectionType_t::Pin);
  unitConnectionsData.name = "A";
  unitConnectionsData.resistance = 1.0;
  unitConnectionsData.schemaPage = "26";
  unitConnectionsData.signalName = "Sig_A";
  unitConnectionsData.swAddress = 1256;
  unitConnectionsData.functionEN = "FEN";
  unitConnectionsData.functionFR = "FFR";
  unitConnectionsData.functionDE = "FDE";
  unitConnectionsData.functionIT = "FIT";
  data.addConnectionData(unitConnectionsData);
  // Setup unit connection data and add to unit connector
  unitConnectionsData.clear();
  unitConnectionsData.setConnectionType(mdtClConnectionType_t::Socket);
  unitConnectionsData.name = "B";
  unitConnectionsData.resistance = 1.2;
  unitConnectionsData.schemaPage = "27";
  unitConnectionsData.signalName = "Sig_B";
  unitConnectionsData.swAddress = 1259;
  unitConnectionsData.functionEN = "FEN2";
  unitConnectionsData.functionFR = "FFR2";
  unitConnectionsData.functionDE = "FDE2";
  unitConnectionsData.functionIT = "FIT2";
  data.addConnectionData(unitConnectionsData);
  // Add to database
  pk1 = ucnx.addUnitConnector(data, true);
  QVERIFY(!pk1.isNull());
  // Get data back and check
  data = ucnx.getUnitConnectorData(pk1, true, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(1000));
  QCOMPARE(data.name, QVariant("X1"));
  QVERIFY(!data.isBasedOnConnector());
  QVERIFY(!data.isBasedOnArticleConnector());
  QCOMPARE(data.connectionDataList().size(), 2);
  // Check unit connection A
  QVERIFY(!data.connectionDataList().at(0).isNull());
  QCOMPARE(data.connectionDataList().at(0).keyData().unitId(), QVariant(1000));
  QCOMPARE(data.connectionDataList().at(0).keyData().unitConnectorFk().unitId(), QVariant(1000));
  QVERIFY(data.connectionDataList().at(0).isPartOfUnitConnector());
  QVERIFY(!data.connectionDataList().at(0).isBasedOnArticleConnection());
  QVERIFY(data.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Pin);
  QCOMPARE(data.connectionDataList().at(0).name, QVariant("A"));
  QCOMPARE(data.connectionDataList().at(0).resistance, QVariant(1.0));
  QCOMPARE(data.connectionDataList().at(0).schemaPage, QVariant("26"));
  QCOMPARE(data.connectionDataList().at(0).signalName, QVariant("Sig_A"));
  QCOMPARE(data.connectionDataList().at(0).swAddress, QVariant(1256));
  QCOMPARE(data.connectionDataList().at(0).functionEN, QVariant("FEN"));
  QCOMPARE(data.connectionDataList().at(0).functionFR, QVariant("FFR"));
  QCOMPARE(data.connectionDataList().at(0).functionDE, QVariant("FDE"));
  QCOMPARE(data.connectionDataList().at(0).functionIT, QVariant("FIT"));
  // Check unit connection B
  QVERIFY(!data.connectionDataList().at(1).isNull());
  QCOMPARE(data.connectionDataList().at(1).keyData().unitId(), QVariant(1000));
  QCOMPARE(data.connectionDataList().at(1).keyData().unitConnectorFk().unitId(), QVariant(1000));
  QVERIFY(data.connectionDataList().at(1).isPartOfUnitConnector());
  QVERIFY(!data.connectionDataList().at(1).isBasedOnArticleConnection());
  QVERIFY(data.connectionDataList().at(1).connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(data.connectionDataList().at(1).name, QVariant("B"));
  QCOMPARE(data.connectionDataList().at(1).resistance, QVariant(1.2));
  QCOMPARE(data.connectionDataList().at(1).schemaPage, QVariant("27"));
  QCOMPARE(data.connectionDataList().at(1).signalName, QVariant("Sig_B"));
  QCOMPARE(data.connectionDataList().at(1).swAddress, QVariant(1259));
  QCOMPARE(data.connectionDataList().at(1).functionEN, QVariant("FEN2"));
  QCOMPARE(data.connectionDataList().at(1).functionFR, QVariant("FFR2"));
  QCOMPARE(data.connectionDataList().at(1).functionDE, QVariant("FDE2"));
  QCOMPARE(data.connectionDataList().at(1).functionIT, QVariant("FIT2"));
  // Get back connector data from unit connection B
  unitConnectionKey = data.connectionDataList().at(1).keyData();
  data = ucnx.getUnitConnectorData(unitConnectionKey.pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(1000));
  QCOMPARE(data.name, QVariant("X1"));
  /*
   * Unit connector based on a article connector
   */
  // Setup article connector FK
  articleConnectorFk.clear();
  articleConnectorFk.id = 200;
  articleConnectorFk.setArticleId(2);
  // Setup unit connector key
  key2.setUnitId(2000);
  key2.setArticleConnectorFk(articleConnectorFk);
  // Setup unit connector data
  data.clear();
  data.setKeyData(key2);
  data.name = "X2";
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
  unitConnectionsData.name = "C";
  unitConnectionsData.resistance = 2.0;
  unitConnectionsData.schemaPage = "35";
  unitConnectionsData.signalName = "Sig_C";
  unitConnectionsData.swAddress = 3556;
  unitConnectionsData.functionEN = "FEN3";
  unitConnectionsData.functionFR = "FFR3";
  unitConnectionsData.functionDE = "FDE3";
  unitConnectionsData.functionIT = "FIT3";
  data.addConnectionData(unitConnectionsData);
  // Add to database
  pk2 = ucnx.addUnitConnector(data, true);
  QVERIFY(!pk2.isNull());
  // Get data back and check
  data = ucnx.getUnitConnectorData(pk2, true, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().unitId(), QVariant(2000));
  QCOMPARE(data.name, QVariant("X2"));
  QVERIFY(data.isBasedOnConnector());
  QVERIFY(data.isBasedOnArticleConnector());
  QCOMPARE(data.connectionDataList().size(), 1);
  // Check unit connection A
  QVERIFY(!data.connectionDataList().at(0).isNull());
  QCOMPARE(data.connectionDataList().at(0).keyData().unitId(), QVariant(2000));
  QCOMPARE(data.connectionDataList().at(0).keyData().unitConnectorFk().unitId(), QVariant(2000));
  QCOMPARE(data.connectionDataList().at(0).keyData().unitConnectorFk().connectorFk().id, QVariant(1));
  QCOMPARE(data.connectionDataList().at(0).keyData().articleConnectionFk().id, QVariant(25));
  QCOMPARE(data.connectionDataList().at(0).keyData().articleConnectionFk().articleId(), QVariant(2));
  QVERIFY(data.connectionDataList().at(0).keyData().articleConnectionFk().connectionTypeFk().type() == mdtClConnectionType_t::Socket);
  QCOMPARE(data.connectionDataList().at(0).keyData().articleConnectionFk().articleConnectorFk().id, QVariant(200));
  QCOMPARE(data.connectionDataList().at(0).keyData().articleConnectionFk().articleConnectorFk().articleId(), QVariant(2));
  QCOMPARE(data.connectionDataList().at(0).keyData().articleConnectionFk().articleConnectorFk().connectorFk().id, QVariant(1));
  QVERIFY(data.connectionDataList().at(0).isPartOfUnitConnector());
  QVERIFY(data.connectionDataList().at(0).isBasedOnArticleConnection());
  QVERIFY(data.connectionDataList().at(0).connectionType() == mdtClConnectionType_t::Socket);
  QCOMPARE(data.connectionDataList().at(0).name, QVariant("C"));
  QCOMPARE(data.connectionDataList().at(0).resistance, QVariant(2.0));
  QCOMPARE(data.connectionDataList().at(0).schemaPage, QVariant("35"));
  QCOMPARE(data.connectionDataList().at(0).signalName, QVariant("Sig_C"));
  QCOMPARE(data.connectionDataList().at(0).swAddress, QVariant(3556));
  QCOMPARE(data.connectionDataList().at(0).functionEN, QVariant("FEN3"));
  QCOMPARE(data.connectionDataList().at(0).functionFR, QVariant("FFR3"));
  QCOMPARE(data.connectionDataList().at(0).functionDE, QVariant("FDE3"));
  QCOMPARE(data.connectionDataList().at(0).functionIT, QVariant("FIT3"));
  /*
   * Remove unit connectors
   */
  // Remove connector pk1
  QVERIFY(ucnx.removeUnitConnector(pk1, true));
  data = ucnx.getUnitConnectorData(pk1, true, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  // Remove connector pk2
  QVERIFY(ucnx.removeUnitConnector(pk2, true));
  data = ucnx.getUnitConnectorData(pk2, true, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
}

void mdtClUnitConnectionTest::unitConnectionSelectionDialogTest()
{
  QFAIL("Not implemented yet");
  
  mdtClUnitConnectionSelectionDialog dialog(nullptr);
  mdtCableListTestScenario scenario(pvDatabaseManager.database());

  // Populate with test data
  scenario.createTestVehicleTypes();
  scenario.createTestConnectors();
  scenario.createTestArticles();
  scenario.createTestArticleConnections();
  scenario.createTestArticleConnectors();
  scenario.createTestUnits();
  scenario.createTestUnitConnections();
  scenario.createTestArticleConnectors();
  scenario.createTestLinkVersions();
  scenario.createTestLinks();
  
  
  dialog.exec();
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
