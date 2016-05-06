/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "mdtClLinkVersionData.h"
#include "mdtClLinkVersion.h"
#include "mdtClLinkVersionModel.h"
#include "mdtClLink.h"
#include "mdtClLinkTypeData.h"
#include "mdtClLinkTypeModel.h"
#include "mdtClLinkDirectionData.h"
#include "mdtClLinkDirectionModel.h"
#include "mdtClVehicleTypeLink.h"
#include "mdtClVehicleTypeLinkKeyData.h"
#include "mdtClVehicleTypeLinkAssignationWidget.h"
#include "mdtClVehicleTypeCheckBox.h"
#include "mdtClArticleLink.h"
#include "mdtClArticleLinkUnitConnectionKeyData.h"
#include "mdtClUnitConnection.h"
#include "mdtClVehicleTypeLinkTestData.h"
#include "mdtApplication.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlRecord.h"
#include "mdtSqlForeignKeySetting.h"
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

/// \todo for sandbox
#include <algorithm>
#include <iterator>

#include <QDebug>

void mdtClLinkTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabase.isOpen());
}

void mdtClLinkTest::cleanupTestCase()
{
}

// void mdtClLinkTest::linkTypeDataTest()
// {
//   mdtClLinkTypeKeyData key;
// 
//   // Initial state
//   QVERIFY(key.isNull());
//   QVERIFY(key.type() == mdtClLinkType_t::Undefined);
//   // Set CABLELINK code
//   key.code = "CABLELINK";
//   QVERIFY(!key.isNull());
//   QVERIFY(key.type() == mdtClLinkType_t::CableLink);
//   // Clear
//   key.clear();
//   QVERIFY(key.code.isNull());
//   QVERIFY(key.isNull());
//   QVERIFY(key.type() == mdtClLinkType_t::Undefined);
//   // Set INTERNLINK code
//   key.code = "INTERNLINK";
//   QVERIFY(!key.isNull());
//   QVERIFY(key.type() == mdtClLinkType_t::InternalLink);
//   // Set CONNECTION code
//   key.code = "CONNECTION";
//   QVERIFY(!key.isNull());
//   QVERIFY(key.type() == mdtClLinkType_t::Connection);
//   // Set TESTLINK code
//   key.code = "TESTLINK";
//   QVERIFY(!key.isNull());
//   QVERIFY(key.type() == mdtClLinkType_t::TestLink);
//   // Clear
//   key.clear();
//   QVERIFY(key.code.isNull());
//   QVERIFY(key.isNull());
//   QVERIFY(key.type() == mdtClLinkType_t::Undefined);
//   // Set CableLink type
//   key.setType(mdtClLinkType_t::CableLink);
//   QVERIFY(!key.isNull());
//   QCOMPARE(key.code, QVariant("CABLELINK"));
//   // Set InternalLink type
//   key.setType(mdtClLinkType_t::InternalLink);
//   QVERIFY(!key.isNull());
//   QCOMPARE(key.code, QVariant("INTERNLINK"));
//   // Set Connection type
//   key.setType(mdtClLinkType_t::Connection);
//   QVERIFY(!key.isNull());
//   QCOMPARE(key.code, QVariant("CONNECTION"));
//   // Set Undefined type
//   key.setType(mdtClLinkType_t::Undefined);
//   QVERIFY(key.code.isNull());
//   QVERIFY(key.isNull());
//   // Set TestLink type
//   key.setType(mdtClLinkType_t::TestLink);
//   QVERIFY(!key.isNull());
//   QCOMPARE(key.code, QVariant("TESTLINK"));
//   // Clear
//   key.clear();
//   QVERIFY(key.code.isNull());
//   QVERIFY(key.isNull());
//   QVERIFY(key.type() == mdtClLinkType_t::Undefined);
// }

// void mdtClLinkTest::linkTypeModelTest()
// {
//   QLocale locale(QLocale::English);
//   mdtClLinkTypeModel m(pvDatabase, locale);
//   mdtClLinkTypeKeyData key;
// 
//   // Initial state
//   QCOMPARE(m.rowCount(), 4);
//   // Check row of key
//   key.code = "CABLELINK";
//   QVERIFY(m.row(key) >= 0);
//   QCOMPARE(m.row(mdtClLinkType_t::CableLink), m.row(key));
//   key.code = "INTERNLINK";
//   QVERIFY(m.row(key) >= 0);
//   QCOMPARE(m.row(mdtClLinkType_t::InternalLink), m.row(key));
//   key.code = "CONNECTION";
//   QVERIFY(m.row(key) >= 0);
//   QCOMPARE(m.row(mdtClLinkType_t::Connection), m.row(key));
//   key.code = "TESTLINK";
//   QVERIFY(m.row(key) >= 0);
//   QCOMPARE(m.row(mdtClLinkType_t::TestLink), m.row(key));
//   key.code = "FAKE";
//   QVERIFY(m.row(key) < 0);
//   // Check getting key data of row
//   QVERIFY(m.keyData(-1).isNull());
//   QVERIFY(m.unit(-1).isEmpty());
//   QVERIFY(!m.keyData(0).isNull());
//   QVERIFY(!m.keyData(1).isNull());
//   QVERIFY(!m.keyData(2).isNull());
//   QVERIFY(!m.keyData(3).isNull());
//   QVERIFY(m.keyData(50).isNull());
//   key.code = "CABLELINK";
//   QCOMPARE(m.keyData(m.row(key)).code, QVariant("CABLELINK"));
//   QCOMPARE(m.unit(m.row(key)), QString("Ohm"));
//   key.code = "INTERNLINK";
//   QCOMPARE(m.keyData(m.row(key)).code, QVariant("INTERNLINK"));
//   QCOMPARE(m.unit(m.row(key)), QString("Ohm"));
//   key.code = "CONNECTION";
//   QCOMPARE(m.keyData(m.row(key)).code, QVariant("CONNECTION"));
//   QCOMPARE(m.unit(m.row(key)), QString("Ohm"));
//   key.code = "TESTLINK";
//   QCOMPARE(m.keyData(m.row(key)).code, QVariant("TESTLINK"));
//   QCOMPARE(m.unit(m.row(key)), QString("Ohm"));
// }

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
  mdtClLinkDirectionModel m(pvDatabase, locale);
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
  mdtClLinkVersion lv(pvDatabase);
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
  // Check setting last version as current one
  QVERIFY(lv.setLastVersionAsCurrentVersion());
  QVERIFY(lv.currentVersion().isNull());
  // Add a version and check
  data.setVersion(1.2);
  QVERIFY(lv.addVersion(data));
  QVERIFY(lv.setLastVersionAsCurrentVersion());
  QCOMPARE(mdtClLinkVersion::currentVersion().version(), 1.2);
  data.setVersion(1.1);
  QVERIFY(lv.addVersion(data));
  QVERIFY(lv.setLastVersionAsCurrentVersion());
  QCOMPARE(mdtClLinkVersion::currentVersion().version(), 1.2);
  // Remove versions
  key.versionPk = 1200;
  QVERIFY(lv.removeVersion(key));
  key.versionPk = 1100;
  QVERIFY(lv.removeVersion(key));
  
  /**
   * \todo Check version adding better when links allready exists !!
   */
  QFAIL("Not completely implemented yet");
}

void mdtClLinkTest::linkVersionModelTest()
{
  mdtCableListTestScenario scenario(pvDatabase);
  mdtClLinkVersionPkData pk;

  // Populate versions
  scenario.createTestLinkVersions();

  mdtClLinkVersionModel m(pvDatabase);
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

// void mdtClLinkTest::linkModificationDataTest()
// {
//   mdtClLinkModificationKeyData key;
//   mdtClLinkPkData linkFk;
//   mdtClLinkVersionData linkVersionData;
//   mdtClModificationPkData modificationFk;
// 
//   // Initial state
//   QVERIFY(key.isNull());
//   // Setup FKs
//   linkFk.connectionStartId = 10;
//   linkFk.connectionEndId = 11;
//   linkVersionData.setVersion(1.2);
//   modificationFk.code = "NEW";
//   // Set
//   key.setLinkFk(linkFk);
//   QVERIFY(key.isNull());
//   key.setLinkVersionFk(linkVersionData.pk());
//   QVERIFY(key.isNull());
//   key.setModificationFk(modificationFk);
//   QVERIFY(!key.isNull());
//   QCOMPARE(key.linkFk().connectionStartId, QVariant(10));
//   QCOMPARE(key.linkFk().connectionEndId, QVariant(11));
//   linkVersionData.setPk(key.linkVersionFk());
//   QCOMPARE(linkVersionData.version(), 1.2);
//   QCOMPARE(key.modificationFk().code, QString("NEW"));
//   // Clear
//   key.clear();
//   QVERIFY(key.linkFk().isNull());
//   QVERIFY(key.linkVersionFk().isNull());
//   QVERIFY(key.modificationFk().isNull());
//   QVERIFY(key.isNull());
// }

// void mdtClLinkTest::linkModificationAddGetRemoveTest()
// {
//   mdtClLink lnk(pvDatabase);
//   mdtSqlForeignKeySetting fkSetting(pvDatabase, mdtSqlForeignKeySetting::Temporary);
//   mdtClLinkModificationKeyData key;
//   mdtClLinkPkData linkFk;
//   mdtClLinkVersionData version;
//   mdtClModificationPkData modificationFk;
//   bool ok;
// 
//   // For this simple test, we not need to have the whole database schema, so we disable foreign keys constraints
//   QVERIFY(fkSetting.disable());
// 
//   /*
//    * Add a modification
//    */
//   // Setup data
//   linkFk.connectionStartId = 10;
//   linkFk.connectionEndId = 11;
//   version.setVersion(1.2);
//   modificationFk.setModification(mdtClModification_t::New);
//   key.setLinkFk(linkFk);
//   key.setLinkVersionFk(version.pk());
//   key.setModificationFk(modificationFk);
//   // Add to database
//   QVERIFY(lnk.addModification(key));
//   /// \todo Get back and check
//   
//   // Remove from database
//   QVERIFY(lnk.removeModification(key));
//   /*
//    * Check removing all modifications for a link
//    */
//   // Setup data
//   linkFk.connectionStartId = 10;
//   linkFk.connectionEndId = 11;
//   version.setVersion(1.2);
//   modificationFk.setModification(mdtClModification_t::New);
//   key.setLinkFk(linkFk);
//   key.setLinkVersionFk(version.pk());
//   key.setModificationFk(modificationFk);
//   // Add to database
//   QVERIFY(lnk.addModification(key));
//   // Setup data
//   version.setVersion(1.3);
//   modificationFk.setModification(mdtClModification_t::Exists);
//   key.setLinkFk(linkFk);
//   key.setLinkVersionFk(version.pk());
//   key.setModificationFk(modificationFk);
//   // Add to database
//   QVERIFY(lnk.addModification(key));
//   // Remove
//   QVERIFY(lnk.removeModifications(linkFk));
//   /// \todo Get back and check that modifications where removed
// 
// }

void mdtClLinkTest::vehicleTypeStartEndKeyDataTest()
{
  mdtClVehicleTypeStartEndKeyData key, key2;
  QList<mdtClVehicleTypeStartEndKeyData> keyList, keyList2, resultKeyList;
  mdtClVehicleTypeStartEndLessThan lt;

  // Initial state
  QVERIFY(key.isNull());
  // Set
  key.setVehicleTypeStartId(1);
  QVERIFY(key.isNull());
  key.setVehicleTypeEndId(2);
  QVERIFY(!key.isNull());
  // Check
  QCOMPARE(key.vehicleTypeStartId(), QVariant(1));
  QCOMPARE(key.vehicleTypeEndId(), QVariant(2));
  // Clear
  key.clear();
  QVERIFY(key.vehicleTypeStartId().isNull());
  QVERIFY(key.vehicleTypeEndId().isNull());
  QVERIFY(key.isNull());
  /*
   * Check mdtClVehicleTypeStartEndLessThan
   */
  // Direct compare check
  key.setVehicleTypeStartId(1);
  key.setVehicleTypeEndId(2);
  key2.setVehicleTypeStartId(2);
  key2.setVehicleTypeEndId(1);
  QVERIFY(lt(key, key2));
  QVERIFY(!lt(key2, key));
  key.setVehicleTypeStartId(1);
  key.setVehicleTypeEndId(2);
  key2.setVehicleTypeStartId(1);
  key2.setVehicleTypeEndId(2);
  QVERIFY(!lt(key, key2));
  QVERIFY(!lt(key2, key));
  key.setVehicleTypeStartId(2);
  key.setVehicleTypeEndId(1);
  key2.setVehicleTypeStartId(1);
  key2.setVehicleTypeEndId(2);
  QVERIFY(!lt(key, key2));
  QVERIFY(lt(key2, key));
  // Build list of keys
  keyList.clear();
  key.setVehicleTypeStartId(2);
  key.setVehicleTypeEndId(1);
  keyList.append(key);
  key.setVehicleTypeStartId(1);
  key.setVehicleTypeEndId(2);
  keyList.append(key);
  key.setVehicleTypeStartId(7);
  key.setVehicleTypeEndId(8);
  keyList.append(key);
  key.setVehicleTypeStartId(4);
  key.setVehicleTypeEndId(3);
  keyList.append(key);
  // Sort
  std::sort(keyList.begin(), keyList.end(), mdtClVehicleTypeStartEndLessThan());
  // Check
  QCOMPARE(keyList.size(), 4);
  QCOMPARE(keyList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(keyList.at(0).vehicleTypeEndId(), QVariant(2));
  QCOMPARE(keyList.at(1).vehicleTypeStartId(), QVariant(2));
  QCOMPARE(keyList.at(1).vehicleTypeEndId(), QVariant(1));
  QCOMPARE(keyList.at(2).vehicleTypeStartId(), QVariant(4));
  QCOMPARE(keyList.at(2).vehicleTypeEndId(), QVariant(3));
  QCOMPARE(keyList.at(3).vehicleTypeStartId(), QVariant(7));
  QCOMPARE(keyList.at(3).vehicleTypeEndId(), QVariant(8));
  /*
   * Check that applying set_difference works:
   *  - keyList = (1,2),(2,1),(4,3),(7,8)
   *  - keyList2 = (2,1),(7,8)
   *  - resultKeyList = keyList \ keyList2 = (1,2),(4,3)
   */
  // Build keyList2
  keyList2.clear();
  key.setVehicleTypeStartId(2);
  key.setVehicleTypeEndId(1);
  keyList2.append(key);
  key.setVehicleTypeStartId(7);
  key.setVehicleTypeEndId(8);
  keyList2.append(key);
  // Apply set_difference
  std::set_difference(keyList.constBegin(), keyList.constEnd(), keyList2.constBegin(), keyList2.constEnd(), std::inserter(resultKeyList, resultKeyList.begin()), mdtClVehicleTypeStartEndLessThan());
  // Check
  QCOMPARE(resultKeyList.size(), 2);
  QCOMPARE(resultKeyList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(resultKeyList.at(0).vehicleTypeEndId(), QVariant(2));
  QCOMPARE(resultKeyList.at(1).vehicleTypeStartId(), QVariant(4));
  QCOMPARE(resultKeyList.at(1).vehicleTypeEndId(), QVariant(3));
}

void mdtClLinkTest::vehicleTypeLinkKeyDataTest()
{
  mdtClVehicleTypeLinkKeyData key;
  mdtClVehicleTypeStartEndKeyData vehicleTypeStartEndFk;
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
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  key.setLinkFk(linkFk);
  QVERIFY(!key.isNull());
  // Check
  QCOMPARE(key.vehicleTypeStartId(), QVariant(1));
  QCOMPARE(key.vehicleTypeEndId(), QVariant(2));
  QCOMPARE(key.vehicleTypeStartEndFk().vehicleTypeStartId(), QVariant(1));
  QCOMPARE(key.vehicleTypeStartEndFk().vehicleTypeEndId(), QVariant(2));
  QCOMPARE(key.linkFk().connectionStartId, QVariant(3));
  QCOMPARE(key.linkFk().connectionEndId, QVariant(4));
  // Clear
  key.clear();
  QVERIFY(key.vehicleTypeStartId().isNull());
  QVERIFY(key.vehicleTypeEndId().isNull());
  QVERIFY(key.linkFk().isNull());
  QVERIFY(key.isNull());
  // Set - With vehicle type start/end FK
  vehicleTypeStartEndFk.setVehicleTypeStartId(10);
  vehicleTypeStartEndFk.setVehicleTypeEndId(11);
  linkFk.connectionStartId = 12;
  linkFk.connectionEndId = 13;
  key.setVehicleTypeStartEndFk(vehicleTypeStartEndFk);
  key.setLinkFk(linkFk);
  // Check
  QCOMPARE(key.vehicleTypeStartId(), QVariant(10));
  QCOMPARE(key.vehicleTypeEndId(), QVariant(11));
  QCOMPARE(key.vehicleTypeStartEndFk().vehicleTypeStartId(), QVariant(10));
  QCOMPARE(key.vehicleTypeStartEndFk().vehicleTypeEndId(), QVariant(11));
  QCOMPARE(key.linkFk().connectionStartId, QVariant(12));
  QCOMPARE(key.linkFk().connectionEndId, QVariant(13));
}

void mdtClLinkTest::vehicleTypeLinkAddGetRemoveTest()
{
  mdtClVehicleTypeLink vtl(pvDatabase);
  mdtSqlForeignKeySetting fkSetting(pvDatabase, mdtSqlForeignKeySetting::Temporary);
  mdtClVehicleTypeLinkKeyData key;
  QList<mdtClVehicleTypeLinkKeyData> keyList;
  mdtClVehicleTypeStartEndKeyData vtStartEndKey;
  QList<mdtClVehicleTypeStartEndKeyData> vtStartEndKeyList;
  mdtClLinkPkData linkFk;
  bool ok;

  // For this test we not need all data of schema, so we disable foreign keys constraints
  QVERIFY(fkSetting.disable());

  // Add a assignation
  key.setVehicleTypeStartId(1);
  key.setVehicleTypeEndId(2);
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  key.setLinkFk(linkFk);
  QVERIFY(vtl.addVehicleTypeLink(key));
  // Get back and check
  keyList = vtl.getVehicleTypeLinkKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(keyList.size(), 1);
  QCOMPARE(keyList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(keyList.at(0).vehicleTypeEndId(), QVariant(2));
  QCOMPARE(keyList.at(0).linkFk().versionFk.versionPk.value(), 100);
  QVERIFY(keyList.at(0).linkFk().modificationFk.modification() == mdtClModification_t::New);
  QCOMPARE(keyList.at(0).linkFk().connectionStartId, QVariant(10));
  QCOMPARE(keyList.at(0).linkFk().connectionEndId, QVariant(11));
  // Check getting vehicle type start, end key data
  vtStartEndKeyList = vtl.getVehicleTypeStartEndKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(vtStartEndKeyList.size(), 1);
  QCOMPARE(vtStartEndKeyList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtStartEndKeyList.at(0).vehicleTypeEndId(), QVariant(2));
  // Remove added assignation
  QVERIFY(vtl.removeVehicleTypeLink(key));
  keyList = vtl.getVehicleTypeLinkKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(keyList.size(), 0);
  /*
   * Check adding and removing a list of vehicle types to a link
   */
  // Setup link
  linkFk.clear();
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  // Setup vehicle types
  vtStartEndKeyList.clear();
  vtStartEndKey.setVehicleTypeStartId(1);
  vtStartEndKey.setVehicleTypeEndId(2);
  vtStartEndKeyList.append(vtStartEndKey);
  vtStartEndKey.setVehicleTypeStartId(3);
  vtStartEndKey.setVehicleTypeEndId(4);
  vtStartEndKeyList.append(vtStartEndKey);
  // Add assignations
  QVERIFY(vtl.addVehicleTypeLinks(linkFk, vtStartEndKeyList, true));
  // Get back and check
  keyList = vtl.getVehicleTypeLinkKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(keyList.size(), 2);
  QCOMPARE(keyList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(keyList.at(0).vehicleTypeEndId(), QVariant(2));
  QCOMPARE(keyList.at(0).linkFk().connectionStartId, QVariant(10));
  QCOMPARE(keyList.at(0).linkFk().connectionEndId, QVariant(11));
  QCOMPARE(keyList.at(0).linkFk().versionFk.versionPk.value(), 100);
  QVERIFY(keyList.at(0).linkFk().modificationFk.modification() == mdtClModification_t::New);
  QCOMPARE(keyList.at(1).vehicleTypeStartId(), QVariant(3));
  QCOMPARE(keyList.at(1).vehicleTypeEndId(), QVariant(4));
  QCOMPARE(keyList.at(1).linkFk().connectionStartId, QVariant(10));
  QCOMPARE(keyList.at(1).linkFk().connectionEndId, QVariant(11));
  // Remove assignations
  QVERIFY(vtl.removeVehicleTypeLinks(linkFk, vtStartEndKeyList, true));
  // Get back and check
  keyList = vtl.getVehicleTypeLinkKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(keyList.size(), 0);
  /*
   * Check removing all vehicle types assigned to a link
   */
  // Add a assignations
  key.setVehicleTypeStartId(1);
  key.setVehicleTypeEndId(2);
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  key.setLinkFk(linkFk);
  QVERIFY(vtl.addVehicleTypeLink(key));
  key.setVehicleTypeStartId(3);
  key.setVehicleTypeEndId(4);
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  key.setLinkFk(linkFk);
  QVERIFY(vtl.addVehicleTypeLink(key));
  // Get back and check that they where added
  keyList = vtl.getVehicleTypeLinkKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(keyList.size(), 2);
  // Remove assignations
  QVERIFY(vtl.removeVehicleTypeLinks(linkFk));
  keyList = vtl.getVehicleTypeLinkKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(keyList.size(), 0);
  /*
   * Check getting and removing vehicle types assigned to a link only for a version
   */
  // Add a assignation for version 0.1
  key.setVehicleTypeStartId(1);
  key.setVehicleTypeEndId(2);
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  key.setLinkFk(linkFk);
  QVERIFY(vtl.addVehicleTypeLink(key));
  // Add a assignation for version 0.2
  key.setVehicleTypeStartId(1);
  key.setVehicleTypeEndId(2);
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 200;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  key.setLinkFk(linkFk);
  QVERIFY(vtl.addVehicleTypeLink(key));
  // Get assignations for version 0.1
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  keyList = vtl.getVehicleTypeLinkKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(keyList.size(), 1);
  // Get assignations for version 0.2
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 200;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  keyList = vtl.getVehicleTypeLinkKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(keyList.size(), 1);
  // Remove assignations for version 0.1
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  QVERIFY(vtl.removeVehicleTypeLinks(linkFk));
  // Get assignations for version 0.1 - Must return nothing
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  keyList = vtl.getVehicleTypeLinkKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(keyList.size(), 0);
  // Get assignations for version 0.2 - Must return 1 item
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 200;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  keyList = vtl.getVehicleTypeLinkKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(keyList.size(), 1);
  // Remove assignations for version 0.2
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 200;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  QVERIFY(vtl.removeVehicleTypeLinks(linkFk));
  // Get assignations for version 0.1 - Must return nothing
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  keyList = vtl.getVehicleTypeLinkKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(keyList.size(), 0);
  // Get assignations for version 0.2 - Must return nothing
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 200;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  keyList = vtl.getVehicleTypeLinkKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(keyList.size(), 0);
  /*
   * Get/remove vehicle types start/end for a version
   */
  // Add a assignation for version 0.1
  key.setVehicleTypeStartId(1);
  key.setVehicleTypeEndId(2);
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  key.setLinkFk(linkFk);
  QVERIFY(vtl.addVehicleTypeLink(key));
  // Add a assignation for version 0.2
  key.setVehicleTypeStartId(1);
  key.setVehicleTypeEndId(2);
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 200;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  key.setLinkFk(linkFk);
  QVERIFY(vtl.addVehicleTypeLink(key));
  // Get vehicle types start/end for version 0.1
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  vtStartEndKeyList = vtl.getVehicleTypeStartEndKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(vtStartEndKeyList.size(), 1);
  // Get vehicle types start/end for version 0.2
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 200;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  vtStartEndKeyList = vtl.getVehicleTypeStartEndKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(vtStartEndKeyList.size(), 1);
  // Remove vehicle types start/end for version 0.1
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  vtStartEndKeyList = vtl.getVehicleTypeStartEndKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(vtStartEndKeyList.size(), 1);
  QVERIFY(vtl.removeVehicleTypeLinks(linkFk, vtStartEndKeyList, true));
  // Get vehicle types start/end for version 0.1 - Must return nothing
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  vtStartEndKeyList = vtl.getVehicleTypeStartEndKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(vtStartEndKeyList.size(), 0);
  // Get vehicle types start/end for version 0.2 - Must return 1 item
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 200;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  vtStartEndKeyList = vtl.getVehicleTypeStartEndKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(vtStartEndKeyList.size(), 1);
  // Remove vehicle types start/end for version 0.2
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 200;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  vtStartEndKeyList = vtl.getVehicleTypeStartEndKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(vtStartEndKeyList.size(), 1);
  QVERIFY(vtl.removeVehicleTypeLinks(linkFk, vtStartEndKeyList, true));
  // Get vehicle types start/end for version 0.1 - Must return nothing
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 100;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  vtStartEndKeyList = vtl.getVehicleTypeStartEndKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(vtStartEndKeyList.size(), 0);
  // Get vehicle types start/end for version 0.2 - Must return nothing
  linkFk.connectionStartId = 10;
  linkFk.connectionEndId = 11;
  linkFk.versionFk.versionPk = 200;
  linkFk.modificationFk.setModification(mdtClModification_t::New);
  vtStartEndKeyList = vtl.getVehicleTypeStartEndKeyDataList(linkFk, ok);
  QVERIFY(ok);
  QCOMPARE(vtStartEndKeyList.size(), 0);
}

void mdtClLinkTest::vehicleTypeLinkUpdateTest()
{
  mdtClVehicleTypeLink vtl(pvDatabase);
  mdtClVehicleTypeLinkTestData testData(pvDatabase);
  mdtSqlForeignKeySetting fkSetting(pvDatabase, mdtSqlForeignKeySetting::Temporary);
  mdtClVehicleTypeStartEndKeyData vtKey;
  QList<mdtClVehicleTypeStartEndKeyData> vtKeyList;
  QList<mdtClVehicleTypeLinkKeyData> vtlKeyList;
  mdtClLinkPkData linkPk;
  bool ok;

  // For this test we not need all data of schema, so we disable foreign keys constraints
  QVERIFY(fkSetting.disable());
  // Create vehicle types
  QVERIFY(testData.createVehicleType(1, "Type 1", "Sub type 1", "1"));
  QVERIFY(testData.createVehicleType(2, "Type 2", "Sub type 2", "1"));
  QVERIFY(testData.createVehicleType(3, "Type 3", "Sub type 3", "1"));

  /*
   * For this test, assume we have 3 links:
   *  - UCS : 101, UCE: 102
   *  - UCS : 103, UCE: 104
   *  - UCS : 105, UCE: 106
   */

  /*
   * Update link UCS: 101, UCE: 102
   *
   * We want assign vehicle type 1 to this link
   */
  // Setup link PK of link to update
  linkPk.connectionStartId = 101;
  linkPk.connectionEndId = 102;
  linkPk.versionFk.versionPk = 100;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  // Setup the expected assignation list
  vtKeyList.clear();
  vtKey.setVehicleTypeStartId(1);
  vtKey.setVehicleTypeEndId(1);
  vtKeyList.append(vtKey);
  // Update
  QVERIFY(vtl.updateVehicleTypeLink(linkPk, vtKeyList, true));
  // Check
  vtlKeyList = vtl.getVehicleTypeLinkKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtlKeyList.size(), 1);
  QCOMPARE(vtlKeyList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtlKeyList.at(0).vehicleTypeEndId(), QVariant(1));
  QCOMPARE(vtlKeyList.at(0).linkFk().connectionStartId, QVariant(101));
  QCOMPARE(vtlKeyList.at(0).linkFk().connectionEndId, QVariant(102));
  // Calling update with the same expected vehicle types must not change anything
  QVERIFY(vtl.updateVehicleTypeLink(linkPk, vtKeyList, true));
  // Check
  vtlKeyList = vtl.getVehicleTypeLinkKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtlKeyList.size(), 1);
  QCOMPARE(vtlKeyList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtlKeyList.at(0).vehicleTypeEndId(), QVariant(1));
  QCOMPARE(vtlKeyList.at(0).linkFk().connectionStartId, QVariant(101));
  QCOMPARE(vtlKeyList.at(0).linkFk().connectionEndId, QVariant(102));
  /*
   * Update link UCS: 103, UCE: 104
   *
   * We want assign vehicle type 1 and 2 to this link
   */
  // Setup link PK of link to update
  linkPk.connectionStartId = 103;
  linkPk.connectionEndId = 104;
  linkPk.versionFk.versionPk = 100;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  // Setup the expected assignation list
  vtKeyList.clear();
  vtKey.setVehicleTypeStartId(1);
  vtKey.setVehicleTypeEndId(1);
  vtKeyList.append(vtKey);
  vtKey.setVehicleTypeStartId(2);
  vtKey.setVehicleTypeEndId(2);
  vtKeyList.append(vtKey);
  // Update
  QVERIFY(vtl.updateVehicleTypeLink(linkPk, vtKeyList, true));
  // Check
  vtlKeyList = vtl.getVehicleTypeLinkKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtlKeyList.size(), 2);
  QCOMPARE(vtlKeyList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtlKeyList.at(0).vehicleTypeEndId(), QVariant(1));
  QCOMPARE(vtlKeyList.at(0).linkFk().connectionStartId, QVariant(103));
  QCOMPARE(vtlKeyList.at(0).linkFk().connectionEndId, QVariant(104));
  QCOMPARE(vtlKeyList.at(1).vehicleTypeStartId(), QVariant(2));
  QCOMPARE(vtlKeyList.at(1).vehicleTypeEndId(), QVariant(2));
  QCOMPARE(vtlKeyList.at(1).linkFk().connectionStartId, QVariant(103));
  QCOMPARE(vtlKeyList.at(1).linkFk().connectionEndId, QVariant(104));
  /*
   * Remove assigned vehicle types using update function
   */
  vtKeyList.clear();
  // Remove vehicle types assigned to link 101-102
  linkPk.connectionStartId = 101;
  linkPk.connectionEndId = 102;
  linkPk.versionFk.versionPk = 100;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  QVERIFY(vtl.updateVehicleTypeLink(linkPk, vtKeyList, true));
  // Check that all vehicle types assigned to link 101-102 are removed
  vtlKeyList = vtl.getVehicleTypeLinkKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtlKeyList.size(), 0);
  // Check that vehicle types assigned to link 103-104 are keeped
  linkPk.connectionStartId = 103;
  linkPk.connectionEndId = 104;
  linkPk.versionFk.versionPk = 100;
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  vtlKeyList = vtl.getVehicleTypeLinkKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtlKeyList.size(), 2);
  // Remove vehicle types assigned to link 103-104
  QVERIFY(vtl.updateVehicleTypeLink(linkPk, vtKeyList, true));
  vtlKeyList = vtl.getVehicleTypeLinkKeyDataList(linkPk, ok);
  QVERIFY(ok);
  QCOMPARE(vtlKeyList.size(), 0);
}

void mdtClLinkTest::vehicleTypeCheckBoxTest()
{
  mdtSqlRecord record;

  // Setup vehicle type data record
  QVERIFY(record.addAllFields("Unit_VehicleType_view", pvDatabase));
  record.setValue("VehicleType_Id_FK", 1);
  record.setValue("Type", "Type 1");
  record.setValue("SubType", "Sub type 1");
  record.setValue("SeriesNumber", "Serie 1");
  // Create item and check its initial state
  mdtClVehicleTypeCheckBox item(nullptr, record);
  item.show();
  QVERIFY(!item.isChecked());
  QCOMPARE(item.vehicleTypeId(), QVariant(1));

  /*
   * Play
   */
  /*
  while(item.isVisible()){
    QTest::qWait(500);
  }
  */
}

void mdtClLinkTest::vehicleTypeLinkAssignationWidgetTest()
{
  mdtClVehicleTypeLinkAssignationWidget vtlw(nullptr, pvDatabase);
  mdtSqlForeignKeySetting fkSetting(pvDatabase, mdtSqlForeignKeySetting::Temporary);
  mdtClVehicleTypeLinkTestData testData(pvDatabase);
  mdtClLinkPkData linkPk;
  QList<mdtClVehicleTypeStartEndKeyData> vtStartEndKeyList;

  // For this test, we not need to create all stuff, so diable foreign key constraints
  QVERIFY(fkSetting.disable());
  // Create vehicle types
  QVERIFY(testData.createVehicleType(1, "Type 1", "Sub type 1", "1"));
  QVERIFY(testData.createVehicleType(2, "Type 2", "Sub type 2", "1"));
  QVERIFY(testData.createVehicleType(3, "Type 3", "Sub type 3", "1"));
  // Create vehicle type - unit assignations
  QVERIFY(testData.addVehicleTypeUnitAssignation(1, 10));
  QVERIFY(testData.addVehicleTypeUnitAssignation(2, 10));
  // Create vehicle type - link assignations
  QVERIFY(testData.addVehicleTypeLinkAssignation(1, 101, 102, 100, "NEW"));
  
  
  linkPk.connectionStartId = 101;
  linkPk.connectionEndId = 102;
  linkPk.versionFk.versionPk = 100;
  linkPk.modificationFk.setModification(mdtClModification_t::New);

  QVERIFY(vtlw.buildVehicleTypeList(10, 10));
  QVERIFY(vtlw.selectVehicleTypeAssignedToLink(linkPk));
  vtStartEndKeyList = vtlw.getSelectedVehicleTypeList();
  QCOMPARE(vtStartEndKeyList.size(), 1);
  QCOMPARE(vtStartEndKeyList.at(0).vehicleTypeStartId(), QVariant(1));
  QCOMPARE(vtStartEndKeyList.at(0).vehicleTypeEndId(), QVariant(1));
  
  /*
   * Play
   */
  vtlw.show();
  /*
  while(vtlw.isVisible()){
    QTest::qWait(500);
  }
  */
}

void mdtClLinkTest::linkDataTest()
{
  mdtClLinkPkData pk;
  mdtClLinkKeyData key;
  mdtClLinkData data;
  mdtClArticleLinkPkData articleLinkFk;

  /*
   * Link PK data test
   */
  // Initial state
  QVERIFY(pk.isNull());
  // Set
  pk.connectionStartId = 1;
  QVERIFY(pk.isNull());
  pk.connectionEndId = 2;
  QVERIFY(pk.isNull());
  pk.versionFk.versionPk = 100;
  QVERIFY(pk.isNull());
  pk.modificationFk.setModification(mdtClModification_t::New);
  QVERIFY(!pk.isNull());
  // Clear
  pk.clear();
  QVERIFY(pk.connectionStartId.isNull());
  QVERIFY(pk.connectionEndId.isNull());
  QVERIFY(pk.versionFk.isNull());
  QVERIFY(pk.modificationFk.isNull());
  QVERIFY(pk.isNull());
  /*
   * Link key data test
   */
  // Initial state
  QVERIFY(key.isNull());
  // Setup PK and FKs
  pk.connectionStartId = 10;
  pk.connectionEndId = 11;
  pk.versionFk.versionPk = 100;
  pk.modificationFk.setModification(mdtClModification_t::New);
  articleLinkFk.connectionStartId = 1;
  articleLinkFk.connectionEndId = 2;
  // Set
  key.setPk(pk);
  QVERIFY(key.isNull());
  key.setLinkType(mdtClLinkType_t::CableLink);
  QVERIFY(key.isNull());
  key.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!key.isNull());
  key.setArticleLinkFk(articleLinkFk);
  key.setWireId(100);
  key.setLinkBeamId(105);
  QVERIFY(!key.isNull());
  QCOMPARE(key.pk().connectionStartId, QVariant(10));
  QCOMPARE(key.pk().connectionEndId, QVariant(11));
  QVERIFY(key.linkTypeFk().type() == mdtClLinkType_t::CableLink);
  QVERIFY(key.linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QCOMPARE(key.articleLinkFk().connectionStartId, QVariant(1));
  QCOMPARE(key.articleLinkFk().connectionEndId, QVariant(2));
  QCOMPARE(key.wireId(), QVariant(100));
  QCOMPARE(key.linkBeamId(), QVariant(105));
  // Clear
  key.clear();
  QVERIFY(key.pk().isNull());
  QVERIFY(key.linkTypeFk().isNull());
  QVERIFY(key.linkDirectionFk().isNull());
  QVERIFY(key.articleLinkFk().isNull());
  QVERIFY(key.wireId().isNull());
  QVERIFY(key.linkBeamId().isNull());
  QVERIFY(key.isNull());
  /*
   * Link data test
   */
  // Initial state
  QVERIFY(data.isNull());
  // Setup PK and FKs
  pk.connectionStartId = 12;
  pk.connectionEndId = 13;
  articleLinkFk.connectionStartId = 3;
  articleLinkFk.connectionEndId = 4;
  // Set
  data.setPk(pk);
  QVERIFY(data.isNull());
  data.setLinkType(mdtClLinkType_t::Connection);
  QVERIFY(data.isNull());
  data.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!data.isNull());
  data.setArticleLinkFk(articleLinkFk);
  data.setWireId(20);
  data.identification = "Link 12-13";
  data.resistance = 1.2;
  data.length = 25.0;
  data.remarks = "Remark 1";
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().pk().connectionStartId, QVariant(12));
  QCOMPARE(data.keyData().pk().connectionEndId, QVariant(13));
  QVERIFY(data.keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(data.keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QCOMPARE(data.keyData().articleLinkFk().connectionStartId, QVariant(3));
  QCOMPARE(data.keyData().articleLinkFk().connectionEndId, QVariant(4));
  QCOMPARE(data.keyData().wireId(), QVariant(20));
  // Clear
  data.clear();
  QVERIFY(data.keyData().isNull());
  QVERIFY(data.identification.isNull());
  QVERIFY(data.resistance.isNull());
  QVERIFY(data.length.isNull());
  QVERIFY(data.remarks.isNull());
  QVERIFY(data.isNull());
}

void mdtClLinkTest::linkAddGetRemoveTest()
{
  mdtClLink lnk(pvDatabase);
  mdtSqlForeignKeySetting fkSetting(pvDatabase, mdtSqlForeignKeySetting::Temporary);
  mdtClLinkData data;
  mdtClLinkPkData pk;
  mdtClArticleLinkPkData articleLinkFk;
  bool ok;

  // For this simple test, we not need to have the whole database schema, so we disable foreign keys constraints
  QVERIFY(fkSetting.disable());

  /*
   * Add a link - Not based on a article link
   *  - Versionning: v1.5, New
   */
  // Setup link pk data
  pk.connectionStartId = 10;
  pk.connectionEndId = 11;
  pk.versionFk.versionPk = 1500;
  pk.modificationFk.setModification(mdtClModification_t::New);
  // Setup link data
  data.clear();
  data.setPk(pk);
  data.setLinkType(mdtClLinkType_t::InternalLink);
  data.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  data.setWireId(2);
  data.identification = "Link 10-11";
  data.resistance = 1.2;
  data.length = 12.0;
  data.remarks = "Remark 10-11";
  // Add
  QVERIFY(lnk.addLink(data));
  /*
   * Add the same link
   *  - Versionning: v1.6, Existing
   */
  // Setup link pk data
  pk.versionFk.versionPk = 1600;
  pk.modificationFk.setModification(mdtClModification_t::Exists);
  // Setup link data
  data.clear();
  data.setPk(pk);
  data.setLinkType(mdtClLinkType_t::InternalLink);
  data.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  data.setWireId(2);
  data.identification = "Link 10-11";
  data.resistance = 1.2;
  data.length = 12.0;
  data.remarks = "Remark 10-11";
  // Add
  QVERIFY(lnk.addLink(data));
  /*
   * Get data
   */
  // Get back and check version 1.5
  pk.versionFk.versionPk = 1500;
  pk.modificationFk.setModification(mdtClModification_t::New);
  data = lnk.getLinkData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.pk().connectionStartId, QVariant(10));
  QCOMPARE(data.pk().connectionEndId, QVariant(11));
  QCOMPARE(data.pk().versionFk.versionPk.value(), 1500);
  QVERIFY(data.pk().modificationFk.modification() == mdtClModification_t::New);
  QVERIFY(data.keyData().linkTypeFk().type() == mdtClLinkType_t::InternalLink);
  QVERIFY(data.keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(data.keyData().articleLinkFk().isNull());
  QCOMPARE(data.keyData().wireId(), QVariant(2));
  QCOMPARE(data.identification, QVariant("Link 10-11"));
  QCOMPARE(data.resistance.value(), 1.2);
  QCOMPARE(data.length.value(), 12.0);
  QCOMPARE(data.remarks, QVariant("Remark 10-11"));
  // Get back and check version 1.6
  pk.versionFk.versionPk = 1600;
  pk.modificationFk.setModification(mdtClModification_t::Exists);
  data = lnk.getLinkData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.pk().connectionStartId, QVariant(10));
  QCOMPARE(data.pk().connectionEndId, QVariant(11));
  QCOMPARE(data.pk().versionFk.versionPk.value(), 1600);
  QVERIFY(data.pk().modificationFk.modification() == mdtClModification_t::Exists);
  QVERIFY(data.keyData().linkTypeFk().type() == mdtClLinkType_t::InternalLink);
  QVERIFY(data.keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(data.keyData().articleLinkFk().isNull());
  QCOMPARE(data.keyData().wireId(), QVariant(2));
  QCOMPARE(data.identification, QVariant("Link 10-11"));
  QCOMPARE(data.resistance.value(), 1.2);
  QCOMPARE(data.length.value(), 12.0);
  QCOMPARE(data.remarks, QVariant("Remark 10-11"));
  /*
   * Update version 1.5:
   *  - Change only identification and remarks (No change on PK)
   */
  // Get link
  pk.connectionStartId = 10;
  pk.connectionEndId = 11;
  pk.versionFk.versionPk = 1500;
  pk.modificationFk.setModification(mdtClModification_t::New);
  data = lnk.getLinkData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  // Update
  data.setPk(pk);
  data.identification = "Link 10-11 edited";
  data.remarks = "Remark 10-11 edited";
  QVERIFY(lnk.updateLink(pk, data));
  // Get data back and check
  data = lnk.getLinkData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.identification, QVariant("Link 10-11 edited"));
  QCOMPARE(data.remarks, QVariant("Remark 10-11 edited"));
  /*
   * Update version 1.6:
   *  - Change connections start/end, version and modification (PK change)
   */
  // Get link
  pk.connectionStartId = 10;
  pk.connectionEndId = 11;
  pk.versionFk.versionPk = 1600;
  pk.modificationFk.setModification(mdtClModification_t::Exists);
  data = lnk.getLinkData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  // Update
  pk.connectionStartId = 15;
  pk.connectionEndId = 16;
  pk.versionFk.versionPk = 1700;
  pk.modificationFk.setModification(mdtClModification_t::ModRem);
  data.setPk(pk);
  data.identification = "Link 15-16";
  data.remarks = "Remark 15-16 REM";
  pk.connectionStartId = 10;
  pk.connectionEndId = 11;
  pk.versionFk.versionPk = 1600;
  pk.modificationFk.setModification(mdtClModification_t::Exists);
  QVERIFY(lnk.updateLink(pk, data));
  // Get data back and check
  pk.connectionStartId = 15;
  pk.connectionEndId = 16;
  pk.versionFk.versionPk = 1700;
  pk.modificationFk.setModification(mdtClModification_t::ModRem);
  data = lnk.getLinkData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.identification, QVariant("Link 15-16"));
  QCOMPARE(data.remarks, QVariant("Remark 15-16 REM"));
  // Add a new modification for this link
  pk.modificationFk.setModification(mdtClModification_t::ModNew);
  data.setPk(pk);
  data.remarks = "Remark 15-16 NEW";
  QVERIFY(lnk.addLink(data));
  data = lnk.getLinkData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.identification, QVariant("Link 15-16"));
  QCOMPARE(data.remarks, QVariant("Remark 15-16 NEW"));
  /*
   * Remove test
   */
  // Remove link 10-11 version 1.5
  pk.connectionStartId = 10;
  pk.connectionEndId = 11;
  pk.versionFk.versionPk = 1500;
  pk.modificationFk.setModification(mdtClModification_t::New);
  QVERIFY(lnk.removeLink(pk, true));
  data = lnk.getLinkData(pk, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  // Remove link 15-16 version 1.7 ModRem
  pk.connectionStartId = 15;
  pk.connectionEndId = 16;
  pk.versionFk.versionPk = 1700;
  pk.modificationFk.setModification(mdtClModification_t::ModRem);
  QVERIFY(lnk.removeLink(pk, true));
  data = lnk.getLinkData(pk, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  // Remove link 15-16 version 1.7 ModNew
  pk.connectionStartId = 15;
  pk.connectionEndId = 16;
  pk.versionFk.versionPk = 1700;
  pk.modificationFk.setModification(mdtClModification_t::ModNew);
  QVERIFY(lnk.removeLink(pk, true));
  data = lnk.getLinkData(pk, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
}

void mdtClLinkTest::linkAndVehicleTypeAddGetRemoveTest()
{
  mdtClLink lnk(pvDatabase);
  mdtSqlForeignKeySetting fkSetting(pvDatabase, mdtSqlForeignKeySetting::Temporary);
  mdtClLinkData linkData;
  mdtClLinkPkData linkPk;
  ///mdtClLinkModificationKeyData linkModificationKey;
  mdtClLinkVersionData linkVersion;
  ///mdtClModificationPkData modificationKey;
  mdtClVehicleTypeStartEndKeyData vtKey;
  QList<mdtClVehicleTypeStartEndKeyData> vtKeyList;
  bool ok;

  // For this simple test, we not need to have the whole database schema, so we disable foreign keys constraints
  QVERIFY(fkSetting.disable());

  /*
   * Add a link with modification and list of vehicle types to assign
   */
  // Setup link data
  linkPk.connectionStartId = 15;
  linkPk.connectionEndId = 16;
  linkVersion.setVersion(1.1);
  linkPk.versionFk = linkVersion.pk();
  linkPk.modificationFk.setModification(mdtClModification_t::New);
  linkData.setPk(linkPk);
  linkData.setLinkType(mdtClLinkType_t::CableLink);
  linkData.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  linkData.identification = "Link 15-16";
  // Setup modification key
//   linkModificationKey.setLinkFk(linkPk);
//   linkVersion.setVersion(1.1);
//   linkModificationKey.setLinkVersionFk(linkVersion.pk());
//   modificationKey.setModification(mdtClModification_t::New);
//   linkModificationKey.setModificationFk(modificationKey);
  // Setup vehicle type list
  vtKey.setVehicleTypeStartId(1);
  vtKey.setVehicleTypeEndId(2);
  vtKeyList.append(vtKey);
  vtKey.setVehicleTypeStartId(3);
  vtKey.setVehicleTypeEndId(4);
  vtKeyList.append(vtKey);
  // Add link
  QVERIFY(lnk.addLink(linkData, vtKeyList, true));
  // Get link back and check
  
  // Get modification back and check
  
  // Get assigned vehicle types and check
  
  /*
   * Update
   */
  
  /*
   * Remove
   */
  
  QFAIL("Not completly implemented");
}

void mdtClLinkTest::linkFromArticleLinkAddTest()
{
  using Mdt::CableList::UnitConnectionPk;

  mdtClLink lnk(pvDatabase);
  mdtClLinkPkData linkPk;
  mdtClLinkData linkData;
  mdtClArticleLink alnk(pvDatabase);
  mdtSqlForeignKeySetting fkSetting(pvDatabase, mdtSqlForeignKeySetting::Temporary);
  mdtClArticleLinkPkData aLinkPk;
  mdtClArticleLinkData aLinkData;
  QList<mdtClArticleLinkData> aLinkDataList;
  mdtClArticleConnectionKeyData acnxFk;
  UnitConnectionPk ucnxPk;
  mdtClUnitConnectionKeyData ucnxKey;
  mdtClUnitConnectionData ucnxData;
  mdtClUnitConnection ucnx(pvDatabase);
  bool ok;
  mdtClArticleLinkUnitConnectionKeyData alucnxKey;
  mdtClLinkVersionPkData versionPk;
  mdtClModificationPkData modificationPk;

  /*
   * For this test we work directly on ArticleLink_tbl and UnitConnection_tbl.
   * We not need the complete schema, so we disable foreign key check
   */
  QVERIFY(fkSetting.disable());
  /*
   * Article link - unit connection PK data test
   */
  // Initial state
  QVERIFY(alucnxKey.isNull());
  // Set
  alucnxKey.articleLinkPk.connectionStartId = 1;
  alucnxKey.articleLinkPk.connectionEndId = 2;
  QVERIFY(alucnxKey.isNull());
  alucnxKey.unitConnectionStartPk.setId(3);
  QVERIFY(alucnxKey.isNull());
  alucnxKey.unitConnectionEndPk.setId(4);
  QVERIFY(!alucnxKey.isNull());
  // Clear
  alucnxKey.clear();
  QVERIFY(alucnxKey.articleLinkPk.isNull());
  QVERIFY(alucnxKey.unitConnectionStartPk.isNull());
  QVERIFY(alucnxKey.unitConnectionEndPk.isNull());
  QVERIFY(alucnxKey.isNull());
  /*
   * Create article links:
   *  - 10-11 , CableLink, Bidirectional
   */
  aLinkData.clear();
  aLinkPk.connectionStartId = 10;
  aLinkPk.connectionEndId = 11;
  aLinkData.setPkData(aLinkPk);
  aLinkData.setLinkType(mdtClLinkType_t::CableLink);
  aLinkData.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  aLinkData.indetification = "Link 10-11";
  aLinkData.resistance = 1.3;
  QVERIFY(alnk.addLink(aLinkData));
  /*
   * Create unit connections:
   *  - 110 , part of unit 1 , based on article connection 10
   *  - 111 , part of unit 1 , based on article connection 11
   */
  // Setup data for unit connection 110
  ucnxData.clear();
  acnxFk.id = 10;
  acnxFk.setArticleId(1);
  acnxFk.setConnectionType(mdtClConnectionType_t::Pin);
  ucnxKey.setPk(UnitConnectionPk(110));
  ucnxKey.setUnitId(1);
  ucnxKey.setArticleConnectionFk(acnxFk);
  ucnxData.setKeyData(ucnxKey);
  // Add unit connection
  QVERIFY(!ucnx.addUnitConnection(ucnxData, true).isNull());
  ///QVERIFY(!ucnx.linksHaveBeenAdded());
  // At this state, no links has to be created
  QCOMPARE(ucnx.linkToCreateKeyList().size(), 0);
//   ucnxPk.id = 110;
//   QVERIFY(!alnk.hasRelatedLinksToCreate(ucnxPk, 1, ok));
//   QVERIFY(ok);
//   aLinkDataList = alnk.getLinkDataList(ucnxPk, 1, ok);
//   QVERIFY(ok);
//   QCOMPARE(aLinkDataList.size(), 0);
  // Setup data for unit connection 111
  ucnxData.clear();
  acnxFk.id = 11;
  acnxFk.setArticleId(1);
  acnxFk.setConnectionType(mdtClConnectionType_t::Pin);
  ucnxKey.setPk(UnitConnectionPk(111));
  ucnxKey.setUnitId(1);
  ucnxKey.setArticleConnectionFk(acnxFk);
  ucnxData.setKeyData(ucnxKey);
  // Add unit connection
  QVERIFY(!ucnx.addUnitConnection(ucnxData, true).isNull());
  ///QVERIFY(ucnx.linksHaveBeenAdded());
  // At this state, 1 link has to be created
  QCOMPARE(ucnx.linkToCreateKeyList().size(), 1);
  QCOMPARE(ucnx.linkToCreateKeyList().at(0).articleLinkPk.connectionStartId, QVariant(10));
  QCOMPARE(ucnx.linkToCreateKeyList().at(0).articleLinkPk.connectionEndId, QVariant(11));
  QCOMPARE(ucnx.linkToCreateKeyList().at(0).unitConnectionStartPk.id(), 110);
  QCOMPARE(ucnx.linkToCreateKeyList().at(0).unitConnectionEndPk.id(), 111);
  // Create the link
  versionPk.versionPk = 100;
  modificationPk.setModification(mdtClModification_t::Exists);
  QVERIFY(lnk.addLinkList(ucnx.linkToCreateKeyList(), versionPk, modificationPk, true));
  // Get link back and check
  linkPk.connectionStartId = 110;
  linkPk.connectionEndId = 111;
  linkPk.versionFk = versionPk;
  linkPk.modificationFk = modificationPk;
  linkData = lnk.getLinkData(linkPk, ok);
  QVERIFY(ok);
  QVERIFY(!linkData.isNull());
  QCOMPARE(linkData.keyData().articleLinkFk().connectionStartId, QVariant(10));
  QCOMPARE(linkData.keyData().articleLinkFk().connectionEndId, QVariant(11));
  
//   ucnxPk.id = 111;
//   QVERIFY(alnk.hasRelatedLinksToCreate(ucnxPk, 1, ok));
//   QVERIFY(ok);
//   QCOMPARE(alnk.relatedLinksToCreateCount(ucnxPk, 1), 1);
//   aLinkDataList = alnk.getLinkDataList(ucnxPk, 1, ok);
//   QVERIFY(ok);
//   QCOMPARE(aLinkDataList.size(), 1);
//   // Getting article link must return 10-11
//   ucnxPk.id = 111;
//   aLinkDataList = alnk.getLinkDataList(ucnxPk, 1, ok);
//   QVERIFY(ok);
//   QCOMPARE(aLinkDataList.size(), 1);
//   // Link was allready created when connection was added. Remove it, so we can check..
//   linkPk.connectionStartId = 110;
//   linkPk.connectionEndId = 111;
//   QVERIFY(lnk.removeLink(linkPk, true));
//   // Create link on base of this article link
//   QVERIFY(lnk.addLink(aLinkData, 1));
//   // Get link back and check
  // Link was allready created when connection was added. Check it
//   linkPk.connectionStartId = 110;
//   linkPk.connectionEndId = 111;
//   qDebug() << "About to check link...";
//   linkData = lnk.getLinkData(linkPk, ok);
//   QVERIFY(ok);
//   QVERIFY(!linkData.isNull());
//   QCOMPARE(linkData.keyData().articleLinkFk().connectionStartId, QVariant(10));
//   QCOMPARE(linkData.keyData().articleLinkFk().connectionEndId, QVariant(11));
//   QVERIFY(linkData.keyData().linkTypeFk().type() == mdtClLinkType_t::CableLink);
//   QVERIFY(linkData.keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
//   QCOMPARE(linkData.identification, QVariant("Link 10-11"));
//   QCOMPARE(linkData.resistance.value(), 1.3);
//   // Check getting link data by article link PK
//   aLinkPk.connectionStartId = 10;
//   aLinkPk.connectionEndId = 11;
//   linkData = lnk.getLinkData(aLinkPk, 1, 1, ok);
//   QVERIFY(ok);
//   QVERIFY(!linkData.isNull());
//   QCOMPARE(linkData.keyData().articleLinkFk().connectionStartId, QVariant(10));
//   QCOMPARE(linkData.keyData().articleLinkFk().connectionEndId, QVariant(11));
//   QVERIFY(linkData.keyData().linkTypeFk().type() == mdtClLinkType_t::CableLink);
//   QVERIFY(linkData.keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
//   QCOMPARE(linkData.identification, QVariant("Link 10-11"));
//   QCOMPARE(linkData.resistance.value(), 1.3);
  /*
   * Remove created links
   */
  linkPk.connectionStartId = 110;
  linkPk.connectionEndId = 111;
  QVERIFY(lnk.removeLink(linkPk, true));
  /*
   * Remove created unit connections
   */
  ucnxPk.setId(110);
  QVERIFY(ucnx.removeUnitConnection(ucnxPk));
  ucnxPk.setId(111);
  QVERIFY(ucnx.removeUnitConnection(ucnxPk));
  /*
   * Remove created article links
   */
  aLinkPk.connectionStartId = 10;
  aLinkPk.connectionEndId = 11;
  QVERIFY(alnk.removeLink(aLinkPk));
}

/*
 * Test database helper methods
 */

void mdtClLinkTest::createDatabaseSchema()
{
  /*
   * Init and open database
   */
  QVERIFY(pvTempFile.open());
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  pvDatabase.setDatabaseName(pvTempFile.fileName());
  QVERIFY(pvDatabase.open());
  /*
   * Check Sqlite database creation
   */
  mdtTtDatabaseSchema schema;
  QVERIFY(schema.buildSchema());
  QVERIFY(schema.databaseSchema().createSchema(pvDatabase));
  QVERIFY(schema.checkSchema(pvDatabase));
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
