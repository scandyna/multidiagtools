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
#include "mdtClConnectorTest.h"
#include "mdtApplication.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlRecord.h"
#include "mdtTtBase.h"
#include "mdtClConnector.h"
#include "mdtClConnectorData.h"
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
#include <QDebug>

void mdtClConnectorTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabaseManager.database().isOpen());
}

void mdtClConnectorTest::cleanupTestCase()
{
  QFile::remove(pvDbFileInfo.absoluteFilePath());
}

void mdtClConnectorTest::connectorDataTest()
{
  mdtClConnectorKeyData keyData;
  mdtClConnectorData data;

  /*
   * Check key data
   */
  // Initial state
  QVERIFY(keyData.id.isNull());
  QVERIFY(keyData.isNull());
  // Set ID
  keyData.id = 1;
  QCOMPARE(keyData.id, QVariant(1));
  QVERIFY(!keyData.isNull());
  // Clear
  keyData.clear();
  QVERIFY(keyData.id.isNull());
  QVERIFY(keyData.isNull());
  /*
   * Check connector data
   */
  // Initial sate
  QVERIFY(data.isNull());
  // Set data
  data.gender = "Male";
  QVERIFY(data.isNull());
  data.keyData.id = 1;
  QVERIFY(!data.isNull());
  // Clear
  data.clear();
  QVERIFY(data.isNull());
  QVERIFY(data.keyData.isNull());
  QVERIFY(data.gender.isNull());
  QVERIFY(data.form.isNull());
  QVERIFY(data.manufacturer.isNull());
  QVERIFY(data.manufacturerConfigCode.isNull());
  QVERIFY(data.manufacturerArticleCode.isNull());
}

void mdtClConnectorTest::connectorAddGetRemoveTest()
{
  mdtClConnector cnr(pvDatabaseManager.database());
  mdtClConnectorData data;
  mdtClConnectorKeyData key;
  bool ok;

  /*
   * Add a connector
   */
  data.gender = "Male";
  data.form = "Round";
  data.manufacturer = "Manufacturer 1";
  data.manufacturerConfigCode = "Manufacturer cfg 1";
  data.manufacturerArticleCode = "Manufacturer art 1";
  key = cnr.addConnector(data, true);
  QVERIFY(!key.isNull());
  // Get connector back and check
  data = cnr.getConnectorData(key, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.gender, QVariant("Male"));
  QCOMPARE(data.form, QVariant("Round"));
  QCOMPARE(data.manufacturer, QVariant("Manufacturer 1"));
  QCOMPARE(data.manufacturerConfigCode, QVariant("Manufacturer cfg 1"));
  QCOMPARE(data.manufacturerArticleCode, QVariant("Manufacturer art 1"));
  // Remove connector
  QVERIFY(cnr.removeConnector(key));
  data = cnr.getConnectorData(key, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
}


/*
 * Test database helper methods
 */

void mdtClConnectorTest::createDatabaseSchema()
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
  mdtClConnectorTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
