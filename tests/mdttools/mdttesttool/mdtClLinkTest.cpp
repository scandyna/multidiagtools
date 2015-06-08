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
#include "mdtClLinkTest.h"
#include "mdtClLinkTypeData.h"
#include "mdtClLinkTypeModel.h"
#include "mdtClLinkDirectionData.h"
#include "mdtClLinkDirectionModel.h"
#include "mdtApplication.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlRecord.h"
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

void mdtClLinkTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabaseManager.database().isOpen());
}

void mdtClLinkTest::cleanupTestCase()
{
  QFile::remove(pvDbFileInfo.absoluteFilePath());
}

void mdtClLinkTest::linkTypeDataTest()
{
  mdtClLinkTypeKeyData key;

  // Initial state
  QVERIFY(key.isNull());
  QVERIFY(key.type() == mdtClLinkType_t::Undefined);
  // Set CABLELINK code
  key.code = "CABLELINK";
  QVERIFY(!key.isNull());
  QVERIFY(key.type() == mdtClLinkType_t::CableLink);
  // Clear
  key.clear();
  QVERIFY(key.code.isNull());
  QVERIFY(key.isNull());
  QVERIFY(key.type() == mdtClLinkType_t::Undefined);
  // Set INTERNLINK code
  key.code = "INTERNLINK";
  QVERIFY(!key.isNull());
  QVERIFY(key.type() == mdtClLinkType_t::InternalLink);
  // Set CONNECTION code
  key.code = "CONNECTION";
  QVERIFY(!key.isNull());
  QVERIFY(key.type() == mdtClLinkType_t::Connection);
  // Set TESTLINK code
  key.code = "TESTLINK";
  QVERIFY(!key.isNull());
  QVERIFY(key.type() == mdtClLinkType_t::TestLink);
  // Clear
  key.clear();
  QVERIFY(key.code.isNull());
  QVERIFY(key.isNull());
  QVERIFY(key.type() == mdtClLinkType_t::Undefined);
  // Set CableLink type
  key.setType(mdtClLinkType_t::CableLink);
  QVERIFY(!key.isNull());
  QCOMPARE(key.code, QVariant("CABLELINK"));
  // Set InternalLink type
  key.setType(mdtClLinkType_t::InternalLink);
  QVERIFY(!key.isNull());
  QCOMPARE(key.code, QVariant("INTERNLINK"));
  // Set Connection type
  key.setType(mdtClLinkType_t::Connection);
  QVERIFY(!key.isNull());
  QCOMPARE(key.code, QVariant("CONNECTION"));
  // Set Undefined type
  key.setType(mdtClLinkType_t::Undefined);
  QVERIFY(key.code.isNull());
  QVERIFY(key.isNull());
  // Set TestLink type
  key.setType(mdtClLinkType_t::TestLink);
  QVERIFY(!key.isNull());
  QCOMPARE(key.code, QVariant("TESTLINK"));
  // Clear
  key.clear();
  QVERIFY(key.code.isNull());
  QVERIFY(key.isNull());
  QVERIFY(key.type() == mdtClLinkType_t::Undefined);
}

void mdtClLinkTest::linkTypeModelTest()
{
  QLocale locale(QLocale::English);
  mdtClLinkTypeModel m(pvDatabaseManager.database(), locale);
  mdtClLinkTypeKeyData key;

  // Initial state
  QCOMPARE(m.rowCount(), 4);
  // Check row of key
  key.code = "CABLELINK";
  QVERIFY(m.row(key) >= 0);
  key.code = "INTERNLINK";
  QVERIFY(m.row(key) >= 0);
  key.code = "CONNECTION";
  QVERIFY(m.row(key) >= 0);
  key.code = "TESTLINK";
  QVERIFY(m.row(key) >= 0);
  key.code = "FAKE";
  QVERIFY(m.row(key) < 0);
  // Check getting key data of row
  QVERIFY(m.keyData(-1).isNull());
  QVERIFY(m.unit(-1).isEmpty());
  QVERIFY(!m.keyData(0).isNull());
  QVERIFY(!m.keyData(1).isNull());
  QVERIFY(!m.keyData(2).isNull());
  QVERIFY(!m.keyData(3).isNull());
  QVERIFY(m.keyData(50).isNull());
  key.code = "CABLELINK";
  QCOMPARE(m.keyData(m.row(key)).code, QVariant("CABLELINK"));
  QCOMPARE(m.unit(m.row(key)), QString("Ohm"));
  key.code = "INTERNLINK";
  QCOMPARE(m.keyData(m.row(key)).code, QVariant("INTERNLINK"));
  QCOMPARE(m.unit(m.row(key)), QString("Ohm"));
  key.code = "CONNECTION";
  QCOMPARE(m.keyData(m.row(key)).code, QVariant("CONNECTION"));
  QCOMPARE(m.unit(m.row(key)), QString("Ohm"));
  key.code = "TESTLINK";
  QCOMPARE(m.keyData(m.row(key)).code, QVariant("TESTLINK"));
  QCOMPARE(m.unit(m.row(key)), QString("Ohm"));
}

void mdtClLinkTest::linkDirectionDataTest()
{
  mdtClLinkDirectionKeyData key;

  // Initial state
  QVERIFY(key.isNull());
  QVERIFY(key.direction() == mdtClLinkDirection_t::Undefined);
  // Set BID code
  key.code = "BID";
  QVERIFY(!key.isNull());
  QVERIFY(key.direction() == mdtClLinkDirection_t::Bidirectional);
  // Clear
  key.clear();
  QVERIFY(key.code.isNull());
  QVERIFY(key.isNull());
  QVERIFY(key.direction() == mdtClLinkDirection_t::Undefined);
  // Set STE code
  key.code = "STE";
  QVERIFY(!key.isNull());
  QVERIFY(key.direction() == mdtClLinkDirection_t::StartToEnd);
  // Set ETS code
  key.code = "ETS";
  QVERIFY(!key.isNull());
  QVERIFY(key.direction() == mdtClLinkDirection_t::EndToStart);
  // Clear
  key.clear();
  QVERIFY(key.code.isNull());
  QVERIFY(key.isNull());
  QVERIFY(key.direction() == mdtClLinkDirection_t::Undefined);
  // Set Bidirectional direction
  key.setDirection(mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!key.isNull());
  QCOMPARE(key.code, QVariant("BID"));
  // Set Undefined direction
  key.setDirection(mdtClLinkDirection_t::Undefined);
  QVERIFY(key.code.isNull());
  QVERIFY(key.isNull());
  // Set StartToEnd direction
  key.setDirection(mdtClLinkDirection_t::StartToEnd);
  QVERIFY(!key.isNull());
  QCOMPARE(key.code, QVariant("STE"));
  // Set EndToStart direction
  key.setDirection(mdtClLinkDirection_t::EndToStart);
  QVERIFY(!key.isNull());
  QCOMPARE(key.code, QVariant("ETS"));
  // Clear
  key.clear();
  QVERIFY(key.code.isNull());
  QVERIFY(key.isNull());
  QVERIFY(key.direction() == mdtClLinkDirection_t::Undefined);
}

void mdtClLinkTest::linkDirectionModelTest()
{
  QLocale locale(QLocale::English);
  mdtClLinkDirectionModel m(pvDatabaseManager.database(), locale);
  mdtClLinkDirectionKeyData key;
  mdtClLinkTypeKeyData typeKey;

  // Initial state
  QCOMPARE(m.rowCount(), 3);
  // Check row of key
  key.code = "BID";
  QVERIFY(m.row(key) >= 0);
  key.code = "STE";
  QVERIFY(m.row(key) >= 0);
  key.code = "ETS";
  QVERIFY(m.row(key) >= 0);
  key.code = "FAKE";
  QVERIFY(m.row(key) < 0);
  // Check getting key data of row
  QVERIFY(m.keyData(-1).isNull());
  QVERIFY(!m.keyData(0).isNull());
  QVERIFY(!m.keyData(1).isNull());
  QVERIFY(!m.keyData(2).isNull());
  QVERIFY(m.keyData(50).isNull());
  key.code = "BID";
  QCOMPARE(m.keyData(m.row(key)).code, QVariant("BID"));
  key.code = "STE";
  QCOMPARE(m.keyData(m.row(key)).code, QVariant("STE"));
  key.code = "ETS";
  QCOMPARE(m.keyData(m.row(key)).code, QVariant("ETS"));
  // Check setting link type
  typeKey.setType(mdtClLinkType_t::CableLink);
  m.setLinkType(typeKey);
  QCOMPARE(m.rowCount(), 1);
  QVERIFY(m.keyData(0).direction() == mdtClLinkDirection_t::Bidirectional);
  typeKey.setType(mdtClLinkType_t::Connection);
  m.setLinkType(typeKey);
  QCOMPARE(m.rowCount(), 1);
  QVERIFY(m.keyData(0).direction() == mdtClLinkDirection_t::Bidirectional);
  typeKey.setType(mdtClLinkType_t::InternalLink);
  m.setLinkType(typeKey);
  QCOMPARE(m.rowCount(), 1);
  QVERIFY(m.keyData(0).direction() == mdtClLinkDirection_t::Bidirectional);
  typeKey.setType(mdtClLinkType_t::TestLink);
  m.setLinkType(typeKey);
  QCOMPARE(m.rowCount(), 1);
  QVERIFY(m.keyData(0).direction() == mdtClLinkDirection_t::Bidirectional);
  typeKey.setType(mdtClLinkType_t::Undefined);
  m.setLinkType(typeKey);
  QCOMPARE(m.rowCount(), 0);
  QVERIFY(m.keyData(0).isNull());

}


/*
 * Test database helper methods
 */

void mdtClLinkTest::createDatabaseSchema()
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
  mdtClLinkTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
