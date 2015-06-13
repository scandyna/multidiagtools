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

  /*
   * Key data test
   */
  // Initial state
  QVERIFY(key.isNull());
  QVERIFY(!key.isPartOfUnitConnector());
  QVERIFY(!key.isBasedOnArticleConnection());
  // Set key for a unit free unit connection
  
  // Set key for a unit connection that is part of a free unit connector
  
  // Set key for a unit connection based on a free article connection
  
  /*
   * Set key for a unit connection that is based on a unit connector, that is based on a article connector
   * -> Unit connection is also based on a article connection, that is based on the same article connector.
   */
  
  
  key.id = 1;
  QVERIFY(key.isNull());
  key.setUnitId(2);
  QVERIFY(key.isNull());
  key.connectionTypeFk.setType(mdtClConnectionType_t::Pin);
  QVERIFY(!key.isNull());
/// \todo update   key.unitConnectorFk.id = 3;
/// \todo update   key.unitConnectorFk.unitId = 4;
  QVERIFY(!key.unitConnectorFk.isNull());
  QVERIFY(key.isPartOfUnitConnector());
  key.articleConnectionFk.id = 5;
/// \todo update  key.articleConnectionFk.articleId = 6;
/// \todo update  key.articleConnectionFk.connectionTypeFk.setType(mdtClConnectionType_t::Pin);
  QVERIFY(!key.articleConnectionFk.isNull());
  QVERIFY(key.isBasedOnArticleConnection());
  // Clear
  key.clear();
  QVERIFY(key.id.isNull());
  QVERIFY(key.unitId().isNull());
  QVERIFY(key.unitConnectorFk.isNull());
  QVERIFY(key.articleConnectionFk.isNull());
  QVERIFY(key.connectionTypeFk.isNull());
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
  data.clear();
  key.clear();
  key.id = 5;
  key.setUnitId(6);
  key.connectionTypeFk.setType(mdtClConnectionType_t::Socket);
  data.setKeyData(key);
  QVERIFY(!data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  // Clear
  data.clear();
  QVERIFY(data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  // Set data for a unit connection part of unit connector
  key.clear();
  key.id = 1;
  key.setUnitId(2);
  key.unitConnectorFk.id = 3;
/// \todo update   key.unitConnectorFk.unitId = 2;
  key.connectionTypeFk.setType(mdtClConnectionType_t::Pin);
  data.setKeyData(key);
  QVERIFY(!data.isNull());
  QVERIFY(data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  // Clear
  data.clear();
  QVERIFY(data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  // Set data for a unit connection based on a article connection
  key.clear();
  key.id = 5;
  key.setUnitId(6);
  key.articleConnectionFk.id = 7;
  key.articleConnectionFk.articleId() = 8;
/// \todo update    key.articleConnectionFk.connectionTypeFk.setType(mdtClConnectionType_t::Socket);
  key.connectionTypeFk.setType(mdtClConnectionType_t::Socket);
  data.setKeyData(key);
  QVERIFY(!data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(data.isBasedOnArticleConnection());
  // Clear
  data.clear();
  QVERIFY(data.isNull());
  QVERIFY(!data.isPartOfUnitConnector());
  QVERIFY(!data.isBasedOnArticleConnection());
  // Data members set/clear
  key.clear();
  key.id = 1;
  key.setUnitId(2);
  key.connectionTypeFk.setType(mdtClConnectionType_t::Terminal);
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
  QFAIL("Not allready implemented");
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
