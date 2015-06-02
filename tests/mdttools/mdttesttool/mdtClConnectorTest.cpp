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
#include "mdtClConnectorContactData.h"
#include "mdtClConnectionTypeData.h"
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

void mdtClConnectorTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabaseManager.database().isOpen());
}

void mdtClConnectorTest::cleanupTestCase()
{
  QFile::remove(pvDbFileInfo.absoluteFilePath());
}

void mdtClConnectorTest::connectionTypeDataTest()
{
  mdtClConnectionTypeKeyData keyData;
  mdtClConnectionTypeData data;
  QLocale locale(QLocale::English);

  /*
   * Check key data
   */
  // Initial state
  QVERIFY(keyData.isNull());
  // Set code
  keyData.code = "T";
  QVERIFY(!keyData.isNull());
  // Clear
  keyData.clear();
  QVERIFY(keyData.code.isNull());
  QVERIFY(keyData.isNull());
  // Set type directly
  keyData.code = "T";
  QVERIFY(keyData.type() == mdtClConnectionType_t::Terminal);
  QVERIFY(!keyData.isNull());
  keyData.code = "P";
  QVERIFY(keyData.type() == mdtClConnectionType_t::Pin);
  QVERIFY(!keyData.isNull());
  keyData.code = "S";
  QVERIFY(keyData.type() == mdtClConnectionType_t::Socket);
  QVERIFY(!keyData.isNull());
  // clear
  keyData.clear();
  QVERIFY(keyData.type() == mdtClConnectionType_t::Undefined);
  QVERIFY(keyData.isNull());
  /*
   * Check connection type data
   */
  // Initial state
  QVERIFY(data.isNull());
  QVERIFY(data.type() == mdtClConnectionType_t::Undefined);
  // Set type directly
  data.keyData.code = "T";
  QVERIFY(data.type() == mdtClConnectionType_t::Terminal);
  data.keyData.code = "P";
  QVERIFY(data.type() == mdtClConnectionType_t::Pin);
  data.keyData.code = "S";
  QVERIFY(data.type() == mdtClConnectionType_t::Socket);
  data.clear();
  QVERIFY(data.keyData.code.isNull());
  QVERIFY(data.isNull());
  QVERIFY(data.type() == mdtClConnectionType_t::Undefined);
  // Set type with setType()
  data.setType(mdtClConnectionType_t::Terminal);
  QCOMPARE(data.keyData.code, QVariant("T"));
  QVERIFY(data.type() == mdtClConnectionType_t::Terminal);
  data.setType(mdtClConnectionType_t::Pin);
  QCOMPARE(data.keyData.code, QVariant("P"));
  QVERIFY(data.type() == mdtClConnectionType_t::Pin);
  data.setType(mdtClConnectionType_t::Socket);
  QCOMPARE(data.keyData.code, QVariant("S"));
  QVERIFY(data.type() == mdtClConnectionType_t::Socket);
  data.setType(mdtClConnectionType_t::Undefined);
  QVERIFY(data.isNull());
  QVERIFY(data.type() == mdtClConnectionType_t::Undefined);
  // Setup a socket with names
  data.keyData.code = "S";
  data.nameEN = "Socket EN";
  data.nameFR = "Socket FR";
  data.nameDE = "Socket DE";
  data.nameIT = "Socket IT";
  QVERIFY(!data.isNull());
  QVERIFY(data.type() == mdtClConnectionType_t::Socket);
  QCOMPARE(data.name(locale), QString("Socket EN"));
  data.setType(mdtClConnectionType_t::Pin);
  QVERIFY(!data.isNull());
  QVERIFY(data.type() == mdtClConnectionType_t::Pin);
  QVERIFY(data.nameEN.isNull());
  QVERIFY(data.nameFR.isNull());
  QVERIFY(data.nameDE.isNull());
  QVERIFY(data.nameIT.isNull());
  QVERIFY(data.name(locale).isEmpty());
}

void mdtClConnectorTest::connectionTypeGetTest()
{
  mdtClConnector cnr(pvDatabaseManager.database());
  mdtClConnectionTypeData data;
  bool ok;

  // Terminal type
  data = cnr.getConnectionTypeData("T", ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData.code, QVariant("T"));
  QVERIFY(data.type() == mdtClConnectionType_t::Terminal);
  // Pin
  data = cnr.getConnectionTypeData("P", ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData.code, QVariant("P"));
  QVERIFY(data.type() == mdtClConnectionType_t::Pin);
  // Socket
  data = cnr.getConnectionTypeData("S", ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData.code, QVariant("S"));
  QVERIFY(data.type() == mdtClConnectionType_t::Socket);
  // Non existant
  data = cnr.getConnectionTypeData("NonExist", ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  QVERIFY(data.keyData.isNull());
  QVERIFY(data.type() == mdtClConnectionType_t::Undefined);
}

void mdtClConnectorTest::contactDataTest()
{
  mdtClConnectorContactKeyData keyData;
  mdtClConnectorContactData data;

  /*
   * Key data test
   */
  // Initial state
  QVERIFY(keyData.isNull());
  // Set keys
  keyData.id = 1;
  QVERIFY(keyData.isNull());
  keyData.connectorFk.id = 2;
  QVERIFY(keyData.isNull());
  keyData.connectionTypeFk.code = "P";
  QVERIFY(!keyData.isNull());
  // Clear
  keyData.clear();
  QVERIFY(keyData.id.isNull());
  QVERIFY(keyData.connectorFk.isNull());
  QVERIFY(keyData.connectionTypeFk.isNull());
  QVERIFY(keyData.isNull());
  /*
   * Data test
   */
  // Initial state
  QVERIFY(data.isNull());
  // Set data
  keyData.clear();
  keyData.id = 3;
  keyData.connectorFk.id = 4;
  keyData.connectionTypeFk.code = "S";
  data.setKeyData(keyData);
  data.name = "Name 3";
  QVERIFY(!data.isNull());
  // Clear
  data.clear();
  QVERIFY(data.isNull());
  QVERIFY(data.name.isNull());
}

void mdtClConnectorTest::contactAddGetRemoveTest()
{
  mdtClConnector cnr(pvDatabaseManager.database());
  mdtClConnectorKeyData connectorKey;
  mdtClConnectorData connectorData;
  mdtClConnectorContactKeyData contactKey;
  mdtClConnectorContactData contactData;
  bool ok;

  /*
   * We must first add a connector
   */
  connectorData.form = "Round";
  connectorKey = cnr.addConnector(connectorData, true);
  QVERIFY(!connectorKey.isNull());
  /*
   * Setup and add contact
   */
  contactKey.clear();
  contactKey.connectorFk = connectorKey;
  contactKey.connectionTypeFk.code = "S";
  contactData.setKeyData(contactKey);
  contactData.name = "A";
  contactKey = cnr.addContact(contactData, true);
  QVERIFY(!contactKey.isNull());
  // Get contact back from DB and check
  contactData = cnr.getContactData(contactKey, ok);
  QVERIFY(ok);
  QVERIFY(!contactData.isNull());
  QCOMPARE(contactData.keyData().id, contactKey.id);
  QCOMPARE(contactData.keyData().connectorFk.id, connectorKey.id);
  QCOMPARE(contactData.keyData().connectionTypeFk.code, QVariant("S"));
  QCOMPARE(contactData.name, QVariant("A"));
  /*
   * Remove conact and connector
   */
  QVERIFY(cnr.removeContact(contactKey));
  QVERIFY(cnr.removeConnector(connectorKey, true));
  contactData = cnr.getContactData(contactKey, ok);
  QVERIFY(ok);
  QVERIFY(contactData.isNull());
}

void mdtClConnectorTest::connectorDataTest()
{
  mdtClConnectorKeyData keyData;
  mdtClConnectorData data;
  mdtClConnectorContactData contactData;

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
  QCOMPARE(data.contactDataList().size(), 0);
  // Set data
  data.gender = "Male";
  QVERIFY(data.isNull());
  keyData.id = 1;
  data.setKeyData(keyData);
  QVERIFY(!data.isNull());
  // Add a contact
  contactData.name = "A";
  data.addContactData(contactData);
  QCOMPARE(data.contactDataList().size(), 1);
  QCOMPARE(data.contactDataList().at(0).keyData().connectorFk.id, QVariant(1));
  QCOMPARE(data.contactDataList().at(0).name, QVariant("A"));
  // Clear
  data.clear();
  QVERIFY(data.isNull());
  QVERIFY(data.keyData().isNull());
  QVERIFY(data.gender.isNull());
  QVERIFY(data.form.isNull());
  QVERIFY(data.manufacturer.isNull());
  QVERIFY(data.manufacturerConfigCode.isNull());
  QVERIFY(data.manufacturerArticleCode.isNull());
  QCOMPARE(data.contactDataList().size(), 0);
}

void mdtClConnectorTest::connectorAddGetRemoveTest()
{
  mdtClConnector cnr(pvDatabaseManager.database());
  mdtClConnectorData data;
  mdtClConnectorKeyData key;
  mdtClConnectorContactData contactData;
  mdtClConnectorContactKeyData contactKey;
  bool ok;

  /*
   * Add a connector without contact
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
  QVERIFY(cnr.removeConnector(key, true));
  data = cnr.getConnectorData(key, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  /*
   * Add a connector with contacts
   */
  // Set connector data
  data.gender = "Male";
  data.form = "Round";
  data.manufacturer = "Manufacturer 2";
  data.manufacturerConfigCode = "Manufacturer cfg 2";
  data.manufacturerArticleCode = "Manufacturer art 2";
  // Add contacts
  contactKey.connectionTypeFk.setType(mdtClConnectionType_t::Pin);
  contactData.setKeyData(contactKey);
  contactData.name = "A";
  data.addContactData(contactData);
  contactData.name = "B";
  data.addContactData(contactData);
  // Add connector to DB
  key = cnr.addConnector(data, true);
  QVERIFY(!key.isNull());
  // Get connector back and check
  data = cnr.getConnectorData(key, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.gender, QVariant("Male"));
  QCOMPARE(data.form, QVariant("Round"));
  QCOMPARE(data.manufacturer, QVariant("Manufacturer 2"));
  QCOMPARE(data.manufacturerConfigCode, QVariant("Manufacturer cfg 2"));
  QCOMPARE(data.manufacturerArticleCode, QVariant("Manufacturer art 2"));
  QCOMPARE(data.contactDataList().size(), 2);
  QVERIFY(data.contactDataList().at(0).keyData().connectionTypeFk.type() == mdtClConnectionType_t::Pin);
  QCOMPARE(data.contactDataList().at(0).name, QVariant("A"));
  QVERIFY(data.contactDataList().at(1).keyData().connectionTypeFk.type() == mdtClConnectionType_t::Pin);
  QCOMPARE(data.contactDataList().at(1).name, QVariant("B"));
  // Remove connector
  QVERIFY(cnr.removeConnector(key, true));
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
