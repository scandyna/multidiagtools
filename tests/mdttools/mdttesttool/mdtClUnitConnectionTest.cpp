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

  /*
   * Key data test
   */
  // Initial state
  QVERIFY(key.isNull());
  QVERIFY(!key.isBasedOnConnector());
  QVERIFY(!key.isBasedOnArticleConnector());
  // Set
  key.id = 1;
  QVERIFY(key.isNull());
  key.unitId = 2;
  QVERIFY(!key.isNull());
  key.connectorFk.id = 3;
  QVERIFY(!key.isNull());
  QVERIFY(key.isBasedOnConnector());
  key.articleConnectorFk.id = 4;
  key.articleConnectorFk.articleId = 5;
  QVERIFY(!key.articleConnectorFk.isNull());
  QVERIFY(key.isBasedOnArticleConnector());
  // Clear
  key.clear();
  QVERIFY(key.id.isNull());
  QVERIFY(key.unitId.isNull());
  QVERIFY(key.connectorFk.isNull());
  QVERIFY(key.articleConnectorFk.isNull());
  QVERIFY(key.isNull());
  /*
   * Data test
   */
  /// \todo complete...
  
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
  // Set
  key.id = 1;
  QVERIFY(key.isNull());
  key.unitId = 2;
  QVERIFY(key.isNull());
  key.connectionTypeFk.setType(mdtClConnectionType_t::Pin);
  QVERIFY(!key.isNull());
  key.unitConnectorFk.id = 3;
  key.unitConnectorFk.unitId = 4;
  QVERIFY(!key.unitConnectorFk.isNull());
  QVERIFY(key.isPartOfUnitConnector());
  key.articleConnectionFk.id = 5;
  key.articleConnectionFk.articleId = 6;
  key.articleConnectionFk.connectionTypeFk.setType(mdtClConnectionType_t::Pin);
  QVERIFY(!key.articleConnectionFk.isNull());
  QVERIFY(key.isBasedOnArticleConnection());
  // Clear
  key.clear();
  QVERIFY(key.id.isNull());
  QVERIFY(key.unitId.isNull());
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
