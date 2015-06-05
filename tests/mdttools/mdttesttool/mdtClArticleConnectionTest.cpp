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

  /*
   * Key data test
   */
  // Initial state
  QVERIFY(acKeyData.isNull());
  QVERIFY(!acKeyData.isBasedOnConnector());
  // Set a key - Not based on a connector
  acKeyData.id = 1;
  QVERIFY(acKeyData.isNull());
  acKeyData.articleId = 2;
  QVERIFY(!acKeyData.isNull());
  QVERIFY(!acKeyData.isBasedOnConnector());
  // Clear
  acKeyData.clear();
  QVERIFY(acKeyData.id.isNull());
  QVERIFY(acKeyData.articleId.isNull());
  QVERIFY(acKeyData.connectorFk.isNull());
  QVERIFY(acKeyData.isNull());
  QVERIFY(!acKeyData.isBasedOnConnector());
  // Set a key - Based on a connector
  acKeyData.id = 5;
  acKeyData.articleId = 6;
  acKeyData.connectorFk.id = 7;
  QVERIFY(!acKeyData.isNull());
  QVERIFY(acKeyData.isBasedOnConnector());
  // Clear
  acKeyData.clear();
  QVERIFY(acKeyData.id.isNull());
  QVERIFY(acKeyData.articleId.isNull());
  QVERIFY(acKeyData.connectorFk.isNull());
  QVERIFY(acKeyData.isNull());
  QVERIFY(!acKeyData.isBasedOnConnector());
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
  acKeyData.articleId = 5;
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
  // Set data - Based on a connector
  acKeyData.clear();
  acKeyData.id = 7;
  acKeyData.articleId = 8;
  acKeyData.connectorFk.id = 9;
  acData.setKeyData(acKeyData);
  acData.name = "ACNR 7";
  QVERIFY(!acData.isNull());
  QVERIFY(acData.isBasedOnConnector());
  // Clear
  acData.clear();
  QVERIFY(acData.keyData().isNull());
  QVERIFY(acData.name.isNull());
  QVERIFY(acData.isNull());
  QVERIFY(!acData.isBasedOnConnector());
}

void mdtClArticleConnectionTest::articleConnectionDataTest()
{
  mdtClArticleConnectionKeyData acnxKey;
  mdtClArticleConnectionData acnxData;

  /*
   * Article connection key data test
   */
  // Initial state
  QVERIFY(acnxKey.isNull());
  QVERIFY(!acnxKey.isPartOfArticleConnector());
  // Set key - Not based on a article connector
  acnxKey.id = 1;
  QVERIFY(acnxKey.isNull());
  acnxKey.articleId = 2;
  QVERIFY(acnxKey.isNull());
  acnxKey.connectionTypeFk.setType(mdtClConnectionType_t::Terminal);
  QVERIFY(!acnxKey.isNull());
  QVERIFY(!acnxKey.isPartOfArticleConnector());
  // Clear
  acnxKey.clear();
  QVERIFY(acnxKey.id.isNull());
  QVERIFY(acnxKey.articleId.isNull());
  QVERIFY(acnxKey.articleConnectorFk.isNull());
  QVERIFY(acnxKey.connectionTypeFk.isNull());
  QVERIFY(acnxKey.isNull());
  QVERIFY(!acnxKey.isPartOfArticleConnector());
  // Set key - Based on a article connector
  acnxKey.id = 3;
  acnxKey.articleId = 4;
  acnxKey.connectionTypeFk.setType(mdtClConnectionType_t::Pin);
  acnxKey.articleConnectorFk.id = 5;
  acnxKey.articleConnectorFk.articleId = acnxKey.articleId;
  QVERIFY(!acnxKey.isNull());
  QVERIFY(acnxKey.isPartOfArticleConnector());
  // Clear
  acnxKey.clear();
  QVERIFY(acnxKey.id.isNull());
  QVERIFY(acnxKey.articleId.isNull());
  QVERIFY(acnxKey.articleConnectorFk.isNull());
  QVERIFY(acnxKey.connectionTypeFk.isNull());
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
  acnxKey.articleId = 2;
  acnxKey.connectionTypeFk.setType(mdtClConnectionType_t::Pin);
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
  QVERIFY(acnxData.keyData().connectionTypeFk.type() == mdtClConnectionType_t::Pin);
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
  // Set data - Part of a article connector
  acnxKey.clear();
  acnxKey.id = 3;
  acnxKey.articleId = 4;
  acnxKey.connectionTypeFk.setType(mdtClConnectionType_t::Pin);
  acnxKey.articleConnectorFk.id = 5;
  acnxKey.articleConnectorFk.articleId = 4;
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
  QVERIFY(acnxData.keyData().connectionTypeFk.type() == mdtClConnectionType_t::Pin);
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
