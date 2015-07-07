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
#include "mdtClLinkKeyData.h"
#include "mdtClLinkData.h"
#include "mdtClLinkVersionKeyData.h"
#include "mdtClLinkVersionData.h"
#include "mdtClLinkVersion.h"
#include "mdtClLinkVersionModel.h"
#include "mdtClLink.h"
#include "mdtClLinkTypeData.h"
#include "mdtClLinkTypeModel.h"
#include "mdtClLinkDirectionData.h"
#include "mdtClLinkDirectionModel.h"
#include "mdtClVehicleTypeLinkKeyData.h"
#include "mdtClVehicleTypeLinkAssignationWidgetItem.h"
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
  QCOMPARE(m.row(mdtClLinkType_t::CableLink), m.row(key));
  key.code = "INTERNLINK";
  QVERIFY(m.row(key) >= 0);
  QCOMPARE(m.row(mdtClLinkType_t::InternalLink), m.row(key));
  key.code = "CONNECTION";
  QVERIFY(m.row(key) >= 0);
  QCOMPARE(m.row(mdtClLinkType_t::Connection), m.row(key));
  key.code = "TESTLINK";
  QVERIFY(m.row(key) >= 0);
  QCOMPARE(m.row(mdtClLinkType_t::TestLink), m.row(key));
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
  QCOMPARE(m.row(mdtClLinkDirection_t::Bidirectional), m.row(key));
  key.code = "STE";
  QVERIFY(m.row(key) >= 0);
  QCOMPARE(m.row(mdtClLinkDirection_t::StartToEnd), m.row(key));
  key.code = "ETS";
  QVERIFY(m.row(key) >= 0);
  QCOMPARE(m.row(mdtClLinkDirection_t::EndToStart), m.row(key));
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
  QCOMPARE(m.pictureAscii(m.row(key)), QString("<-->"));
  key.code = "STE";
  QCOMPARE(m.keyData(m.row(key)).code, QVariant("STE"));
  QCOMPARE(m.pictureAscii(m.row(key)), QString("-->"));
  key.code = "ETS";
  QCOMPARE(m.keyData(m.row(key)).code, QVariant("ETS"));
  QCOMPARE(m.pictureAscii(m.row(key)), QString("<--"));
  // Check setting link type
  m.setLinkType(mdtClLinkType_t::CableLink);
  QCOMPARE(m.rowCount(), 1);
  QVERIFY(m.keyData(0).direction() == mdtClLinkDirection_t::Bidirectional);
  m.setLinkType(mdtClLinkType_t::Connection);
  QCOMPARE(m.rowCount(), 1);
  QVERIFY(m.keyData(0).direction() == mdtClLinkDirection_t::Bidirectional);
  m.setLinkType(mdtClLinkType_t::InternalLink);
  QCOMPARE(m.rowCount(), 1);
  QVERIFY(m.keyData(0).direction() == mdtClLinkDirection_t::Bidirectional);
  m.setLinkType(mdtClLinkType_t::TestLink);
  QCOMPARE(m.rowCount(), 1);
  QVERIFY(m.keyData(0).direction() == mdtClLinkDirection_t::Bidirectional);
  m.setLinkType(mdtClLinkType_t::Undefined);
  QCOMPARE(m.rowCount(), 0);
  QVERIFY(m.keyData(0).isNull());
}

void mdtClLinkTest::linkVersionDataTest()
{
  mdtClLinkVersionPkData pk;
  mdtClLinkVersionData data;

  /*
   * Link version PK data test
   */
  // Initial state
  QVERIFY(pk.isNull());
  // Set
  pk.versionPk = 1000;
  QVERIFY(!pk.isNull());
  // Clear
  pk.clear();
  QVERIFY(pk.versionPk.isNull());
  QVERIFY(pk.isNull());
  /*
   * Link version data test
   */
  // Initial state
  QVERIFY(data.isNull());
  QCOMPARE(data.version(), 0.0);
  // Set from PK - int
  data.setVersionPk(1000);
  QVERIFY(!data.isNull());
  QCOMPARE(data.versionPk(), 1000);
  QCOMPARE(data.version(), 1.0);
  // Clear
  data.clear();
  QCOMPARE(data.versionPk(), 0);
  QCOMPARE(data.version(), 0.0);
  QVERIFY(data.isNull());
  // Set from PK - QVariant
  data.setVersionPk(QVariant(1100));
  QVERIFY(!data.isNull());
  QCOMPARE(data.versionPk(), 1100);
  QCOMPARE(data.version(), 1.1);
  // Set from PK - null QVariant
  data.setVersionPk(QVariant());
  QVERIFY(data.isNull());
  QCOMPARE(data.versionPk(), 0);
  QCOMPARE(data.version(), 0.0);
  // Set version
  data.setVersion(1.2);
  QVERIFY(!data.isNull());
  QCOMPARE(data.versionPk(), 1200);
  QCOMPARE(data.version(), 1.2);
  // Clear
  data.clear();
  QCOMPARE(data.versionPk(), 0);
  QCOMPARE(data.version(), 0.0);
  QVERIFY(data.isNull());
}

void mdtClLinkTest::linkVersionAddGetRemoveTest()
{
  mdtClLinkVersion lv(pvDatabaseManager.database());
  mdtClLinkVersionData data;
  mdtClLinkVersionPkData key;
  bool ok;

  // Initial state
  QVERIFY(mdtClLinkVersion::currentVersion().isNull());
  QVERIFY(lv.currentVersion().isNull());
  // Check get last version
  data = lv.getLastVersionData(ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  // Add version 1.0
  data.setVersion(1.0);
  QVERIFY(lv.addVersion(data));
  // Get back and check
  key.versionPk.setValue(1000);
  data = lv.getVersionData(key, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.versionPk(), 1000);
  QCOMPARE(data.version(), 1.0);
  // Check get last version
  data = lv.getLastVersionData(ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.versionPk(), 1000);
  QCOMPARE(data.version(), 1.0);
  // Check setting current version
  QVERIFY(lv.setCurrentVersion(1.0));
  QCOMPARE(lv.currentVersion().version(), 1.0);
  QCOMPARE(mdtClLinkVersion::currentVersion().version(), 1.0);
  // Remove version 1.0
  QVERIFY(lv.removeVersion(key));
  data = lv.getVersionData(key, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  // After removing version that is current version, currentVersion() must retrun a null version
  QVERIFY(mdtClLinkVersion::currentVersion().isNull());
  QVERIFY(lv.currentVersion().isNull());
  // Check that setting a non existing version fails
  QVERIFY(!lv.setCurrentVersion(1.0));
  QVERIFY(lv.currentVersion().isNull());
  
}

void mdtClLinkTest::linkVersionModelTest()
{
  mdtCableListTestScenario scenario(pvDatabaseManager.database());
  mdtClLinkVersionPkData pk;

  // Populate versions
  scenario.createTestLinkVersions();

  mdtClLinkVersionModel m(pvDatabaseManager.database());
  // Initial state
  QCOMPARE(m.rowCount(), 4);
  // Check row of version
  pk.versionPk.setValue(500);
  QCOMPARE(m.row(pk), 0);
  pk.versionPk.setValue(1000);
  QCOMPARE(m.row(pk), 1);
  pk.versionPk.setValue(1500);
  QCOMPARE(m.row(pk), 2);
  pk.versionPk.setValue(2000);
  QCOMPARE(m.row(pk), 3);
  pk.versionPk.setValue(5000000);
  QCOMPARE(m.row(pk), -1);
  // Check PK of row
  QCOMPARE(m.versionPk(0).versionPk.value(), 500);
  QCOMPARE(m.versionPk(1).versionPk.value(), 1000);
  QCOMPARE(m.versionPk(2).versionPk.value(), 1500);
  QCOMPARE(m.versionPk(3).versionPk.value(), 2000);
  QVERIFY(m.versionPk(500).isNull());

}

void mdtClLinkTest::linkDataTest()
{
  mdtClLinkPkData pk;

  /*
   * Link PK data test
   */
  // Initial state
  QVERIFY(pk.isNull());
  // Set
  pk.connectionStartId = 1;
  QVERIFY(pk.isNull());
  pk.connectionEndId = 2;
  QVERIFY(!pk.isNull());
  // Clear
  pk.clear();
  QVERIFY(pk.connectionStartId.isNull());
  QVERIFY(pk.connectionEndId.isNull());
  QVERIFY(pk.isNull());
}

void mdtClLinkTest::vehicleTypeLinkKeyDataTest()
{
  mdtClVehicleTypeLinkKeyData key;
  mdtClLinkPkData linkFk;

  // Initial state
  QVERIFY(key.isNull());
  // Set
  key.setVehicleTypeStartId(1);
  QVERIFY(key.isNull());
  key.setVehicleTypeEndId(2);
  QVERIFY(key.isNull());
  linkFk.connectionStartId = 3;
  linkFk.connectionEndId = 4;
  key.setLinkFk(linkFk);
  QVERIFY(!key.isNull());
  // Check
  QCOMPARE(key.vehicleTypeStartId(), QVariant(1));
  QCOMPARE(key.vehicleTypeEndId(), QVariant(2));
  QCOMPARE(key.linkFk().connectionStartId, QVariant(3));
  QCOMPARE(key.linkFk().connectionEndId, QVariant(4));
  // Clear
  key.clear();
  QVERIFY(key.vehicleTypeStartId().isNull());
  QVERIFY(key.vehicleTypeEndId().isNull());
  QVERIFY(key.linkFk().isNull());
  QVERIFY(key.isNull());
}

void mdtClLinkTest::vehicleTypeLinkAssignationWidgetItemTest()
{
  mdtSqlRecord record;

  // Setup vehicle type data record
  QVERIFY(record.addAllFields("Unit_VehicleType_view", pvDatabaseManager.database()));
  record.setValue("VehicleType_Id_FK", 1);
  record.setValue("Type", "Type 1");
  record.setValue("SubType", "Sub type 1");
  record.setValue("SeriesNumber", "Serie 1");
  // Create item and check its initial state
  mdtClVehicleTypeLinkAssignationWidgetItem item(nullptr, record);
  item.show();
  QVERIFY(!item.isChecked());
  QVERIFY(item.keyData().isNull());
  

  /*
   * Play
   */
  while(item.isVisible()){
    QTest::qWait(500);
  }
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
