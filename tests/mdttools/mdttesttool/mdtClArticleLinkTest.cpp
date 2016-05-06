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
#include "mdtClArticleLinkTest.h"
#include "mdtClArticleLinkKeyData.h"
#include "mdtClArticleLinkData.h"
#include "mdtClArticleLink.h"
#include "mdtClArticleConnectionKeyData.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClUnitConnection.h"
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

#include <QDebug>

void mdtClArticleLinkTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabase.isOpen());
}

void mdtClArticleLinkTest::cleanupTestCase()
{
}

void mdtClArticleLinkTest::articleLinkDataTest()
{
  mdtClArticleLinkPkData pk;
  mdtClArticleLinkKeyData key;
  mdtClArticleLinkData data;
  mdtClArticleConnectionKeyData acKey;
//   mdtClLinkTypeKeyData ltKey;
  mdtClLinkDirectionKeyData ldKey;

  /*
   * Primary key data test
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
  /*
   * Key data test
   */
  // Initial state
  QVERIFY(key.isNull());
  // Set some data
  key.pk.connectionStartId = 1;
  key.pk.connectionEndId = 2;
  QVERIFY(!key.pk.isNull());
  QVERIFY(key.isNull());
  key.setLinkType(LinkType::CableLink);
  QVERIFY(key.isNull());
  key.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!key.isNull());
//   // Set some data
//   key.connectionStartFk.id = 1;
//   key.connectionStartFk.articleId = 10;
//   key.connectionStartFk.connectionTypeFk.setType(mdtClConnectionType_t::Socket);
//   QVERIFY(!key.connectionStartFk.isNull());
//   QVERIFY(key.isNull());
//   key.connectionEndFk.id = 2;
//   key.connectionEndFk.articleId = 10;
//   key.connectionEndFk.connectionTypeFk.setType(mdtClConnectionType_t::Socket);
//   QVERIFY(!key.connectionEndFk.isNull());
//   QVERIFY(key.isNull());
//   key.linkTypeFk.setType(LinkType::CableLink);
//   QVERIFY(key.isNull());
//   key.linkDirectionFk.setDirection(mdtClLinkDirection_t::Bidirectional);
//   QVERIFY(!key.isNull());
//   // Clear
  key.clear();
//   QVERIFY(key.connectionStartFk.isNull());
//   QVERIFY(key.connectionEndFk.isNull());
  QVERIFY(key.pk.isNull());
  QVERIFY(key.linkTypeFk().isNull());
  QVERIFY(key.linkDirectionFk().isNull());
  QVERIFY(key.isNull());
  /*
   * Data test
   */
  // Initial state
  QVERIFY(data.isNull());
  // Set data
  key.clear();
  pk.connectionStartId = 5;
  pk.connectionEndId = 6;
  key.pk = pk;
  key.setLinkType(LinkType::CableLink);
  key.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!key.isNull());
//   key.clear();
//   key.connectionStartFk.id = 5;
//   key.connectionStartFk.articleId = 20;
//   key.connectionStartFk.connectionTypeFk.setType(mdtClConnectionType_t::Pin);
//   QVERIFY(!key.connectionStartFk.isNull());
//   key.connectionEndFk.id = 6;
//   key.connectionEndFk.articleId = 20;
//   key.connectionEndFk.connectionTypeFk.setType(mdtClConnectionType_t::Socket);
//   QVERIFY(!key.connectionEndFk.isNull());
//   key.linkTypeFk.setType(LinkType::CableLink);
//   key.linkDirectionFk.setDirection(mdtClLinkDirection_t::Bidirectional);
//   QVERIFY(!key.isNull());
  data.setKeyData(key);
  data.indetification = "Link 5-6";
  data.sinceVersion = 1.0;
  data.modification = "new";
  data.resistance = 0.5;
  QVERIFY(!data.isNull());
  // Update key data
  acKey.clear();
  acKey.id = 10;
  data.setConnectionStartFk(acKey);
  acKey.id = 11;
  data.setConnectionEndFk(acKey);
  data.setLinkType(LinkType::CableLink);
  QCOMPARE(data.keyData().pk.connectionStartId, QVariant(10));
  QCOMPARE(data.keyData().pk.connectionEndId, QVariant(11));
  QVERIFY(data.linkType() == LinkType::CableLink);
  QVERIFY(data.linkDirection() == mdtClLinkDirection_t::Bidirectional);
  // Update link type and direction
  data.setLinkType(LinkType::Connection);
  data.setLinkDirection(mdtClLinkDirection_t::StartToEnd);
  QVERIFY(data.linkType() == LinkType::Connection);
  QVERIFY(data.linkDirection() == mdtClLinkDirection_t::StartToEnd);
  // Clear
  data.clear();
  QVERIFY(data.keyData().isNull());
  QVERIFY(data.indetification.isNull());
  QVERIFY(data.sinceVersion.isNull());
  QVERIFY(data.modification.isNull());
  QVERIFY(data.resistance.isNull());
  QVERIFY(data.isNull());
}

void mdtClArticleLinkTest::articleLinkAddGetRemoveTest()
{
  mdtClArticleLink alnk(pvDatabase);
  mdtClArticleLinkPkData pk;
  mdtClArticleLinkData data;
  mdtCableListTestScenario scenario(pvDatabase);
  bool ok;

  // Populate db with base data
  scenario.createTestArticles();
  scenario.createTestArticleConnections();

  // Setup link data
  data.clear();
  pk.clear();
  pk.connectionStartId = 21;
  pk.connectionEndId = 22;
  data.setPkData(pk);
  data.setLinkType(LinkType::CableLink);
  data.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
  data.indetification = "Link 21-22";
  data.sinceVersion = 1.0;
  data.modification = "new";
  data.resistance = 0.1;
  // Add to database
  QVERIFY(alnk.addLink(data));
  // Get article link data from DB and check back
  data = alnk.getLinkData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().pk.connectionStartId, QVariant(21));
  QCOMPARE(data.keyData().pk.connectionEndId, QVariant(22));
  QVERIFY(data.linkType() == LinkType::CableLink);
  QVERIFY(data.linkDirection() == mdtClLinkDirection_t::Bidirectional);
  QCOMPARE(data.indetification, QVariant("Link 21-22"));
  ///QCOMPARE(data.sinceVersion, QVariant(1.0));
  ///QCOMPARE(data.modification, QVariant("new"));
  QCOMPARE(data.resistance, QVariant(0.1));
  QCOMPARE(alnk.relatedLinksCount(pk), 0);
  QVERIFY(!alnk.hasRelatedLinks(pk, ok));
  QVERIFY(ok);
  // Update article link
  data.setLinkType(LinkType::InternalLink);
  data.setLinkDirection(mdtClLinkDirection_t::StartToEnd);
  data.indetification = "Link 21-22 edited";
  data.sinceVersion = 1.1;
  data.modification = "update";
  data.resistance = 0.2;
  QVERIFY(alnk.updateLink(pk, data));
  QVERIFY(ok);
  // Get data back and check
  data = alnk.getLinkData(pk, ok);
  QVERIFY(ok);
  QVERIFY(!data.isNull());
  QCOMPARE(data.keyData().pk.connectionStartId, QVariant(21));
  QCOMPARE(data.keyData().pk.connectionEndId, QVariant(22));
  QVERIFY(data.linkType() == LinkType::InternalLink);
  QVERIFY(data.linkDirection() == mdtClLinkDirection_t::StartToEnd);
  QCOMPARE(data.indetification, QVariant("Link 21-22 edited"));
  ///QCOMPARE(data.sinceVersion, QVariant(1.1));
  ///QCOMPARE(data.modification, QVariant("update"));
  QCOMPARE(data.resistance, QVariant(0.2));
  // Remove article link
  QVERIFY(alnk.removeLink(pk));
  // Try to get a unexisting article link
  data = alnk.getLinkData(pk, ok);
  QVERIFY(ok);
  QVERIFY(data.isNull());
  QCOMPARE(alnk.relatedLinksCount(pk), 0);
  QVERIFY(!alnk.hasRelatedLinks(pk, ok));
  QVERIFY(ok);

}

// void mdtClArticleLinkTest::getArticleLinkByUnitConnectionTest()
// {
//   mdtClArticleLink alnk(pvDatabase);
//   mdtSqlForeignKeySetting fkSetting(pvDatabase, mdtSqlForeignKeySetting::Temporary);
//   mdtClArticleLinkPkData aLinkPk;
//   mdtClArticleLinkData aLinkData;
//   QList<mdtClArticleLinkData> aLinkDataList;
//   mdtClArticleConnectionKeyData acnxFk;
//   mdtClUnitConnectionPkData ucnxPk;
//   mdtClUnitConnectionKeyData ucnxKey;
//   mdtClUnitConnectionData ucnxData;
//   mdtClUnitConnection ucnx(pvDatabase);
//   bool ok;
// 
//   /*
//    * For this test we work directly ArticleLink_tbl and UnitConnection_tbl.
//    * We not need the complete schema, so we disable foreign key check
//    */
//   QVERIFY(fkSetting.disable());
//   /*
//    * Create article links:
//    *  - 10-11 , CableLink, Bidirectional
//    *  - 12-13 , CableLink, Bidirectional
//    *  - 14-15 , InternalLink, Bidirectional
//    *  - 16-17 , InternalLink, Bidirectional
//    */
//   aLinkData.clear();
//   aLinkPk.connectionStartId = 10;
//   aLinkPk.connectionEndId = 11;
//   aLinkData.setPkData(aLinkPk);
//   aLinkData.setLinkType(LinkType::CableLink);
//   aLinkData.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
//   QVERIFY(alnk.addLink(aLinkData));
//   aLinkData.clear();
//   aLinkPk.connectionStartId = 12;
//   aLinkPk.connectionEndId = 13;
//   aLinkData.setPkData(aLinkPk);
//   aLinkData.setLinkType(LinkType::CableLink);
//   aLinkData.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
//   QVERIFY(alnk.addLink(aLinkData));
//   aLinkData.clear();
//   aLinkPk.connectionStartId = 14;
//   aLinkPk.connectionEndId = 15;
//   aLinkData.setPkData(aLinkPk);
//   aLinkData.setLinkType(LinkType::InternalLink);
//   aLinkData.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
//   QVERIFY(alnk.addLink(aLinkData));
//   aLinkData.clear();
//   aLinkPk.connectionStartId = 16;
//   aLinkPk.connectionEndId = 17;
//   aLinkData.setPkData(aLinkPk);
//   aLinkData.setLinkType(LinkType::InternalLink);
//   aLinkData.setLinkDirection(mdtClLinkDirection_t::Bidirectional);
//   QVERIFY(alnk.addLink(aLinkData));
//   /*
//    * Create unit connections:
//    *  - 100 , part of unit 1 , NOT based on a article connection
//    *  - 101 , part of unit 1 , NOT based on a article connection
//    *  - 110 , part of unit 1 , based on article connection 10
//    *  - 111 , part of unit 1 , based on article connection 11
//    *  - 114 , part of unit 1 , based on article connection 14
//    *  - 115 , part of unit 1 , based on article connection 15
//    *  - 212 , part of unit 2 , based on article connection 12
//    *  - 213 , part of unit 2 , based on article connection 13
//    *  - 216 , part of unit 2 , based on article connection 16
//    *  - 217 , part of unit 2 , based on article connection 17
//    */
//   // Add unit connection 100
//   ucnxData.clear();
//   ucnxKey.pk.id = 100;
//   ucnxKey.setUnitId(1);
//   ucnxData.setKeyData(ucnxKey);
//   ucnxData.setConnectionType(mdtClConnectionType_t::Pin);
//   QVERIFY(!ucnx.addUnitConnection(ucnxData, true).isNull());
//   // Getting article link must return nothing
//   ucnxPk.id = 100;
//   aLinkDataList = alnk.getLinkDataList(ucnxPk, 1, ok);
//   QVERIFY(ok);
//   QCOMPARE(aLinkDataList.size(), 0);
//   // Add unit connection 101
//   ucnxData.clear();
//   ucnxKey.pk.id = 101;
//   ucnxKey.setUnitId(1);
//   ucnxData.setKeyData(ucnxKey);
//   ucnxData.setConnectionType(mdtClConnectionType_t::Pin);
//   QVERIFY(!ucnx.addUnitConnection(ucnxData, true).isNull());
//   // Getting article link must return nothing
//   ucnxPk.id = 101;
//   aLinkDataList = alnk.getLinkDataList(ucnxPk, 1, ok);
//   QVERIFY(ok);
//   QCOMPARE(aLinkDataList.size(), 0);
//   // Add unit connection 110
//   ucnxData.clear();
//   acnxFk.id = 10;
//   acnxFk.setArticleId(1);
//   acnxFk.setConnectionType(mdtClConnectionType_t::Pin);
//   ucnxKey.pk.id = 110;
//   ucnxKey.setUnitId(1);
//   ucnxKey.setArticleConnectionFk(acnxFk);
//   ucnxData.setKeyData(ucnxKey);
//   QVERIFY(!ucnx.addUnitConnection(ucnxData, true).isNull());
//   // Getting article link must return nothing
//   ucnxPk.id = 110;
//   aLinkDataList = alnk.getLinkDataList(ucnxPk, 1, ok);
//   QVERIFY(ok);
//   QCOMPARE(aLinkDataList.size(), 0);
//   // Add unit connection 111
//   ucnxData.clear();
//   acnxFk.id = 11;
//   acnxFk.setArticleId(1);
//   acnxFk.setConnectionType(mdtClConnectionType_t::Pin);
//   ucnxKey.pk.id = 111;
//   ucnxKey.setUnitId(1);
//   ucnxKey.setArticleConnectionFk(acnxFk);
//   ucnxData.setKeyData(ucnxKey);
//   QVERIFY(!ucnx.addUnitConnection(ucnxData, true).isNull());
//   // Getting article link must return 10-11
//   ucnxPk.id = 111;
//   aLinkDataList = alnk.getLinkDataList(ucnxPk, 1, ok);
//   QVERIFY(ok);
//   QCOMPARE(aLinkDataList.size(), 1);
//   QCOMPARE(aLinkDataList.at(0).keyData().pk.connectionStartId, QVariant(10));
//   QCOMPARE(aLinkDataList.at(0).keyData().pk.connectionEndId, QVariant(11));
//   // Add unit connection 112
//   ucnxData.clear();
//   acnxFk.id = 12;
//   acnxFk.setArticleId(1);
//   acnxFk.setConnectionType(mdtClConnectionType_t::Pin);
//   ucnxKey.pk.id = 112;
//   ucnxKey.setUnitId(1);
//   ucnxKey.setArticleConnectionFk(acnxFk);
//   ucnxData.setKeyData(ucnxKey);
//   QVERIFY(!ucnx.addUnitConnection(ucnxData, true).isNull());
//   // Getting article link must return nothing
//   ucnxPk.id = 114;
//   aLinkDataList = alnk.getLinkDataList(ucnxPk, 1, ok);
//   QVERIFY(ok);
//   QCOMPARE(aLinkDataList.size(), 0);
//   // Add unit connection 114
//   ucnxData.clear();
//   acnxFk.id = 14;
//   acnxFk.setArticleId(1);
//   acnxFk.setConnectionType(mdtClConnectionType_t::Pin);
//   ucnxKey.pk.id = 114;
//   ucnxKey.setUnitId(1);
//   ucnxKey.setArticleConnectionFk(acnxFk);
//   ucnxData.setKeyData(ucnxKey);
//   QVERIFY(!ucnx.addUnitConnection(ucnxData, true).isNull());
//   // Add unit connection 115
//   ucnxData.clear();
//   acnxFk.id = 15;
//   acnxFk.setArticleId(1);
//   acnxFk.setConnectionType(mdtClConnectionType_t::Pin);
//   ucnxKey.pk.id = 115;
//   ucnxKey.setUnitId(1);
//   ucnxKey.setArticleConnectionFk(acnxFk);
//   ucnxData.setKeyData(ucnxKey);
//   QVERIFY(!ucnx.addUnitConnection(ucnxData, true).isNull());
//   // Getting article link must return 14-15
//   ucnxPk.id = 115;
//   aLinkDataList = alnk.getLinkDataList(ucnxPk, 1, ok);
//   QVERIFY(ok);
//   QCOMPARE(aLinkDataList.size(), 1);
//   QCOMPARE(aLinkDataList.at(0).keyData().pk.connectionStartId, QVariant(14));
//   QCOMPARE(aLinkDataList.at(0).keyData().pk.connectionEndId, QVariant(15));
//   // Add unit connection 212
//   ucnxData.clear();
//   acnxFk.id = 12;
//   acnxFk.setArticleId(1);
//   acnxFk.setConnectionType(mdtClConnectionType_t::Pin);
//   ucnxKey.pk.id = 212;
//   ucnxKey.setUnitId(2);
//   ucnxKey.setArticleConnectionFk(acnxFk);
//   ucnxData.setKeyData(ucnxKey);
//   QVERIFY(!ucnx.addUnitConnection(ucnxData, true).isNull());
//   // Getting article link must return nothing
//   ucnxPk.id = 212;
//   aLinkDataList = alnk.getLinkDataList(ucnxPk, 2, ok);
//   QVERIFY(ok);
//   QCOMPARE(aLinkDataList.size(), 0);
//   // Add unit connection 213
//   ucnxData.clear();
//   acnxFk.id = 13;
//   acnxFk.setArticleId(1);
//   acnxFk.setConnectionType(mdtClConnectionType_t::Pin);
//   ucnxKey.pk.id = 213;
//   ucnxKey.setUnitId(2);
//   ucnxKey.setArticleConnectionFk(acnxFk);
//   ucnxData.setKeyData(ucnxKey);
//   QVERIFY(!ucnx.addUnitConnection(ucnxData, true).isNull());
//   // Getting article link must return 12-13
//   ucnxPk.id = 213;
//   aLinkDataList = alnk.getLinkDataList(ucnxPk, 2, ok);
//   QVERIFY(ok);
//   QCOMPARE(aLinkDataList.size(), 1);
//   QCOMPARE(aLinkDataList.at(0).keyData().pk.connectionStartId, QVariant(12));
//   QCOMPARE(aLinkDataList.at(0).keyData().pk.connectionEndId, QVariant(13));
//   // Add unit connection 216
//   ucnxData.clear();
//   acnxFk.id = 16;
//   acnxFk.setArticleId(1);
//   acnxFk.setConnectionType(mdtClConnectionType_t::Pin);
//   ucnxKey.pk.id = 216;
//   ucnxKey.setUnitId(2);
//   ucnxKey.setArticleConnectionFk(acnxFk);
//   ucnxData.setKeyData(ucnxKey);
//   QVERIFY(!ucnx.addUnitConnection(ucnxData, true).isNull());
//   // Getting article link must return nothing
//   ucnxPk.id = 216;
//   aLinkDataList = alnk.getLinkDataList(ucnxPk, 2, ok);
//   QVERIFY(ok);
//   QCOMPARE(aLinkDataList.size(), 0);
//   // Add unit connection 217
//   ucnxData.clear();
//   acnxFk.id = 17;
//   acnxFk.setArticleId(1);
//   acnxFk.setConnectionType(mdtClConnectionType_t::Pin);
//   ucnxKey.pk.id = 217;
//   ucnxKey.setUnitId(2);
//   ucnxKey.setArticleConnectionFk(acnxFk);
//   ucnxData.setKeyData(ucnxKey);
//   QVERIFY(!ucnx.addUnitConnection(ucnxData, true).isNull());
//   // Getting article link must return 16-17
//   ucnxPk.id = 217;
//   aLinkDataList = alnk.getLinkDataList(ucnxPk, 2, ok);
//   QVERIFY(ok);
//   QCOMPARE(aLinkDataList.size(), 1);
//   QCOMPARE(aLinkDataList.at(0).keyData().pk.connectionStartId, QVariant(16));
//   QCOMPARE(aLinkDataList.at(0).keyData().pk.connectionEndId, QVariant(17));
//   /*
//    * Remove created unit connections
//    */
//   ucnxPk.id = 100;
//   QVERIFY(ucnx.removeUnitConnection(ucnxPk));
//   ucnxPk.id = 101;
//   QVERIFY(ucnx.removeUnitConnection(ucnxPk));
//   ucnxPk.id = 110;
//   QVERIFY(ucnx.removeUnitConnection(ucnxPk));
//   ucnxPk.id = 111;
//   QVERIFY(ucnx.removeUnitConnection(ucnxPk));
//   ucnxPk.id = 112;
//   QVERIFY(ucnx.removeUnitConnection(ucnxPk));
//   ucnxPk.id = 113;
//   QVERIFY(ucnx.removeUnitConnection(ucnxPk));
//   ucnxPk.id = 114;
//   QVERIFY(ucnx.removeUnitConnection(ucnxPk));
//   ucnxPk.id = 115;
//   QVERIFY(ucnx.removeUnitConnection(ucnxPk));
//   ucnxPk.id = 212;
//   QVERIFY(ucnx.removeUnitConnection(ucnxPk));
//   ucnxPk.id = 213;
//   QVERIFY(ucnx.removeUnitConnection(ucnxPk));
//   ucnxPk.id = 216;
//   QVERIFY(ucnx.removeUnitConnection(ucnxPk));
//   ucnxPk.id = 217;
//   QVERIFY(ucnx.removeUnitConnection(ucnxPk));
//   /*
//    * Remove created article links
//    */
//   aLinkPk.connectionStartId = 10;
//   aLinkPk.connectionEndId = 11;
//   QVERIFY(alnk.removeLink(aLinkPk));
//   aLinkPk.connectionStartId = 12;
//   aLinkPk.connectionEndId = 13;
//   QVERIFY(alnk.removeLink(aLinkPk));
//   aLinkPk.connectionStartId = 14;
//   aLinkPk.connectionEndId = 15;
//   QVERIFY(alnk.removeLink(aLinkPk));
//   aLinkPk.connectionStartId = 16;
//   aLinkPk.connectionEndId = 17;
//   QVERIFY(alnk.removeLink(aLinkPk));
// }

/*
 * Test database helper methods
 */

void mdtClArticleLinkTest::createDatabaseSchema()
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
  mdtClArticleLinkTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
