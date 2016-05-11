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
#include "LinkTest.h"
#include "mdtApplication.h"
#include "Mdt/CableList/DatabaseSchema.h"
#include "Mdt/CableList/LinkDirectionPk.h"
#include "Mdt/CableList/LinkDirectionModel.h"
#include "Mdt/CableList/LinkVersionPk.h"
#include "Mdt/CableList/LinkTypePk.h"
#include "Mdt/CableList/LinkTypeModel.h"

#include "Mdt/CableList/LinkPk.h"
#include "Mdt/CableList/LinkKey.h"
#include "Mdt/CableList/LinkData.h"
#include "Mdt/Numeric/Resistance.h"

void LinkTest::initTestCase()
{
  /*
   * Init and open database
   */
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  QVERIFY(pvDatabase.isValid());
  QVERIFY(pvTempFile.open());
  pvTempFile.close();
  pvDatabase.setDatabaseName(pvTempFile.fileName());
  QVERIFY(pvDatabase.open());
  /*
   * Create schema
   */
  createDatabaseSchema();
}

void LinkTest::cleanupTestCase()
{
  pvDatabase.close();
}

/*
 * Tests
 */

void LinkTest::versionPkTest()
{
  using Mdt::CableList::LinkVersionPk;

  /*
   * Constructions
   */
  LinkVersionPk pk;
  QVERIFY(pk.isNull());
  LinkVersionPk pk1(1);
  QVERIFY(!pk1.isNull());
  QCOMPARE(pk1.version(), 1);
  /*
   * Set
   */
  QVERIFY(pk.isNull());
  pk.setVersion(25);
  QVERIFY(!pk.isNull());
  QCOMPARE(pk.version(), 25);
  /*
   * Get from QVariant
   */
  pk = LinkVersionPk::fromQVariant(52);
  QVERIFY(!pk.isNull());
  QCOMPARE(pk.version(), 52);
  pk = LinkVersionPk::fromQVariant(QVariant());
  QVERIFY(pk.isNull());
  /*
   * Clear
   */
  pk.setVersion(22);
  QVERIFY(!pk.isNull());
  pk.clear();
  QVERIFY(pk.isNull());
  /*
   * Comparisons
   */
  // lsh and rhs are null
  QVERIFY(!(LinkVersionPk() == LinkVersionPk()));
  QVERIFY(LinkVersionPk() != LinkVersionPk());
  // lhs is null
  QVERIFY(!(LinkVersionPk() == LinkVersionPk(1)));
  QVERIFY(LinkVersionPk() != LinkVersionPk(1));
  // rhs is null
  QVERIFY(!(LinkVersionPk(1) == LinkVersionPk()));
  QVERIFY(LinkVersionPk(1) != LinkVersionPk());
  // lhs == rhs
  QVERIFY(LinkVersionPk(1) == LinkVersionPk(1));
  QVERIFY(!(LinkVersionPk(1) != LinkVersionPk(1)));
  // lhs != rhs
  QVERIFY(!(LinkVersionPk(1) == LinkVersionPk(2)));
  QVERIFY(LinkVersionPk(1) != LinkVersionPk(2));
}

void LinkTest::typePkTest()
{
  using Mdt::CableList::LinkTypePk;
  using Mdt::CableList::LinkType;

  /*
   * Constructions
   */
  // Default constructed
  LinkTypePk pk;
  QVERIFY(pk.isNull());
  QVERIFY(pk.type() == LinkType::Undefined);
  QVERIFY(pk.code().isEmpty());
  // Construct with a type
  LinkTypePk pk2(LinkType::CableLink);
  QVERIFY(!pk2.isNull());
  QVERIFY(pk2.type() == LinkType::CableLink);
  QCOMPARE(pk2.code(), QString("CABLELINK"));
  /*
   * Set
   */
  QVERIFY(pk.isNull());
  pk.setType(LinkType::CableLink);
  QVERIFY(!pk.isNull());
  QVERIFY(pk.type() == LinkType::CableLink);
  QCOMPARE(pk.code(), QString("CABLELINK"));
  /*
   * Clear
   */
  QVERIFY(!pk.isNull());
  pk.clear();
  QVERIFY(pk.isNull());
  QVERIFY(pk.type() == LinkType::Undefined);
  QVERIFY(pk.code().isEmpty());
  /*
   * Get from code
   */
  pk = LinkTypePk::fromCode("CABLELINK");
  QVERIFY(!pk.isNull());
  QVERIFY(pk.type() == LinkType::CableLink);
  QCOMPARE(pk.code(), QString("CABLELINK"));
  /*
   * Get from QVariant
   */
  pk = LinkTypePk::fromQVariant(QVariant());
  QVERIFY(pk.isNull());
  pk = LinkTypePk::fromQVariant("CABLELINK");
  QVERIFY(!pk.isNull());
  QVERIFY(pk.type() == LinkType::CableLink);
}

void LinkTest::typeCodeTest()
{
  using Mdt::CableList::LinkTypePk;
  using Mdt::CableList::LinkType;

  /*
   * Link type -> code
   */
  QVERIFY(LinkTypePk(LinkType::Undefined).code().isEmpty());
  QCOMPARE(LinkTypePk(LinkType::CableLink).code(), QString("CABLELINK"));
  QCOMPARE(LinkTypePk(LinkType::Connection).code(), QString("CONNECTION"));
  QCOMPARE(LinkTypePk(LinkType::InternalLink).code(), QString("INTERNLINK"));
  QCOMPARE(LinkTypePk(LinkType::TestLink).code(), QString("TESTLINK"));
  /*
   * Code -> link type
   */
  QVERIFY(LinkTypePk::fromCode("").type() == LinkType::Undefined);
  QVERIFY(LinkTypePk::fromCode("CABLELINK").type() == LinkType::CableLink);
  QVERIFY(LinkTypePk::fromCode("CONNECTION").type() == LinkType::Connection);
  QVERIFY(LinkTypePk::fromCode("INTERNLINK").type() == LinkType::InternalLink);
  QVERIFY(LinkTypePk::fromCode("TESTLINK").type() == LinkType::TestLink);
}

void LinkTest::typeModelTest()
{
  using Mdt::CableList::LinkTypeModel;
  using Mdt::CableList::LinkTypePk;
  using Mdt::CableList::LinkType;

  LinkTypeModel m(pvDatabase);

  /*
   * Initial state
   */
  QCOMPARE(m.rowCount(), 4);
  /*
   * Change locale (for sort order)
   */
  m.setLocale(QLocale::English);
  QCOMPARE(m.rowCount(), 4);
  /*
   * Get row for link type
   */
  QCOMPARE(m.row(LinkType::Undefined), -1);
  QCOMPARE(m.row(LinkType::CableLink), 0);
  QCOMPARE(m.row(LinkType::Connection), 1);
  QCOMPARE(m.row(LinkType::InternalLink), 2);
  QCOMPARE(m.row(LinkType::TestLink), 3);
  /*
   * Get row for link type PK
   */
  QCOMPARE(m.row(LinkTypePk(LinkType::Undefined)), -1);
  QCOMPARE(m.row(LinkTypePk(LinkType::CableLink)), 0);
  QCOMPARE(m.row(LinkTypePk(LinkType::Connection)), 1);
  QCOMPARE(m.row(LinkTypePk(LinkType::InternalLink)), 2);
  QCOMPARE(m.row(LinkTypePk(LinkType::TestLink)), 3);
  /*
   * Get PK at row
   */
  QVERIFY(m.primaryKey(-1).type() == LinkType::Undefined);
  QVERIFY(m.primaryKey(0).type() == LinkType::CableLink);
  QVERIFY(m.primaryKey(1).type() == LinkType::Connection);
  QVERIFY(m.primaryKey(2).type() == LinkType::InternalLink);
  QVERIFY(m.primaryKey(3).type() == LinkType::TestLink);
  /*
   * Get unit
   */
  QVERIFY(m.unit(-1).isEmpty());
  QCOMPARE(m.unit(0), QString("Ohm"));
  QCOMPARE(m.unit(1), QString("Ohm"));
  QCOMPARE(m.unit(2), QString("Ohm"));
  QCOMPARE(m.unit(3), QString("Ohm"));
}

void LinkTest::directionPkTest()
{
  using Mdt::CableList::LinkDirectionPk;
  using Mdt::CableList::LinkDirectionType;

  /*
   * Construction
   */
  // Default constructed
  LinkDirectionPk pk;
  QVERIFY(pk.isNull());
  QVERIFY(pk.direction() == LinkDirectionType::Undefined);
  QVERIFY(pk.code().isEmpty());
  // Construct with a direction
  LinkDirectionPk pk2(LinkDirectionType::Bidirectional);
  QVERIFY(!pk2.isNull());
  QVERIFY(pk2.direction() == LinkDirectionType::Bidirectional);
  QCOMPARE(pk2.code(), QString("BID"));
  /*
   * Set
   */
  QVERIFY(pk.isNull());
  pk.setDirection(LinkDirectionType::Bidirectional);
  QVERIFY(!pk.isNull());
  QVERIFY(pk.direction() == LinkDirectionType::Bidirectional);
  QCOMPARE(pk.code(), QString("BID"));
  /*
   * Clear
   */
  QVERIFY(!pk.isNull());
  pk.clear();
  QVERIFY(pk.isNull());
  QVERIFY(pk.direction() == LinkDirectionType::Undefined);
  QVERIFY(pk.code().isEmpty());
  /*
   * Get from code
   */
  QVERIFY(pk.isNull());
  pk = LinkDirectionPk::fromCode("BID");
  QVERIFY(!pk.isNull());
  QVERIFY(pk.direction() == LinkDirectionType::Bidirectional);
  /*
   * Get from QVariant
   */
  QVERIFY(!pk.isNull());
  pk = LinkDirectionPk::fromQVariant(QVariant());
  QVERIFY(pk.isNull());
  pk = LinkDirectionPk::fromQVariant("BID");
  QVERIFY(!pk.isNull());
  QVERIFY(pk.direction() == LinkDirectionType::Bidirectional);
}

void LinkTest::directionCodeTest()
{
  using Mdt::CableList::LinkDirectionPk;
  using Mdt::CableList::LinkDirectionType;

  /*
   * Link direction -> code
   */
  QVERIFY(LinkDirectionPk(LinkDirectionType::Undefined).code().isEmpty());
  QCOMPARE(LinkDirectionPk(LinkDirectionType::Bidirectional).code(), QString("BID"));
  QCOMPARE(LinkDirectionPk(LinkDirectionType::StartToEnd).code(), QString("STE"));
  QCOMPARE(LinkDirectionPk(LinkDirectionType::EndToStart).code(), QString("ETS"));
  /*
   * Code -> link direction
   */
  QVERIFY(LinkDirectionPk::fromCode("").direction() == LinkDirectionType::Undefined);
  QVERIFY(LinkDirectionPk::fromCode("BID").direction() == LinkDirectionType::Bidirectional);
  QVERIFY(LinkDirectionPk::fromCode("STE").direction() == LinkDirectionType::StartToEnd);
  QVERIFY(LinkDirectionPk::fromCode("ETS").direction() == LinkDirectionType::EndToStart);
}

void LinkTest::directionModelTest()
{
  using Mdt::CableList::LinkDirectionModel;
  using Mdt::CableList::LinkDirectionPk;
  using Mdt::CableList::LinkDirectionType;
  using Mdt::CableList::LinkType;

  LinkDirectionModel m(pvDatabase);

  /*
   * Initial state
   */
  QCOMPARE(m.rowCount(), 3);
  /*
   * Change locale
   *
   * Note: this model does not sort.
   *       The order is: BID, STE, ETS
   */
  m.setLocale(QLocale::English);
  QCOMPARE(m.rowCount(), 3);
  /*
   * Get row for direction
   */
  QCOMPARE(m.row(LinkDirectionType::Undefined), -1);
  QCOMPARE(m.row(LinkDirectionType::Bidirectional), 0);
  QCOMPARE(m.row(LinkDirectionType::StartToEnd), 1);
  QCOMPARE(m.row(LinkDirectionType::EndToStart), 2);
  /*
   * Get row for direction PK
   */
  QCOMPARE(m.row(LinkDirectionPk(LinkDirectionType::Undefined)), -1);
  QCOMPARE(m.row(LinkDirectionPk(LinkDirectionType::Bidirectional)), 0);
  QCOMPARE(m.row(LinkDirectionPk(LinkDirectionType::StartToEnd)), 1);
  QCOMPARE(m.row(LinkDirectionPk(LinkDirectionType::EndToStart)), 2);
  /*
   * Get direction PK for row
   */
  QVERIFY(m.directionPk(-1).direction() == LinkDirectionType::Undefined);
  QVERIFY(m.directionPk(0).direction() == LinkDirectionType::Bidirectional);
  QVERIFY(m.directionPk(1).direction() == LinkDirectionType::StartToEnd);
  QVERIFY(m.directionPk(2).direction() == LinkDirectionType::EndToStart);
  QVERIFY(m.directionPk(30).direction() == LinkDirectionType::Undefined);
  /*
   * Get ASCII picture
   */
  QVERIFY(m.pictureAscii(-1).isEmpty());
  QCOMPARE(m.pictureAscii(0), QString("<-->"));
  QCOMPARE(m.pictureAscii(1), QString("-->"));
  QCOMPARE(m.pictureAscii(2), QString("<--"));
  /*
   * Filter available directions for given link type
   */
  // Set cable link: can only be bidirectional
  m.setLinkType(LinkType::CableLink);
  QCOMPARE(m.rowCount(), 1);
  QVERIFY(m.directionPk(0).direction() == LinkDirectionType::Bidirectional);
  // Set internal link: can only be bidirectional
  m.setLinkType(LinkType::InternalLink);
  QCOMPARE(m.rowCount(), 1);
  QVERIFY(m.directionPk(0).direction() == LinkDirectionType::Bidirectional);
  // Set connection: can only be bidirectional
  m.setLinkType(LinkType::Connection);
  QCOMPARE(m.rowCount(), 1);
  QVERIFY(m.directionPk(0).direction() == LinkDirectionType::Bidirectional);
  // Set test link: can only be bidirectional
  m.setLinkType(LinkType::TestLink);
  QCOMPARE(m.rowCount(), 1);
  QVERIFY(m.directionPk(0).direction() == LinkDirectionType::Bidirectional);
  // Reset filter
  m.setLinkType(LinkType::Undefined);
  QCOMPARE(m.rowCount(), 3);
}

void LinkTest::linkPkTest()
{
  using Mdt::CableList::LinkPk;
  using Mdt::CableList::UnitConnectionPk;
  using Mdt::CableList::LinkVersionPk;
  using Mdt::CableList::ModificationType;
  using Mdt::CableList::ModificationPk;

  LinkPk pk;

  /*
   * Initial state
   */
  QVERIFY(pk.isNull());
  /*
   * Set
   */
  pk.setConnectionStart(UnitConnectionPk(1));
  QVERIFY(pk.isNull());
  QCOMPARE(pk.connectionStart().id(), 1);
  pk.setConnectionEnd(UnitConnectionPk(2));
  QVERIFY(pk.isNull());
  QCOMPARE(pk.connectionEnd().id(), 2);
  pk.setVersion(LinkVersionPk(100));
  QVERIFY(pk.isNull());
  QCOMPARE(pk.version().version(), 100);
  pk.setModification(ModificationPk(ModificationType::New));
  QVERIFY(pk.modification().type() == ModificationType::New);
  QVERIFY(!pk.isNull());
  /*
   * Clear
   */
  QVERIFY(!pk.isNull());
  pk.clear();
  QVERIFY(pk.connectionStart().isNull());
  QVERIFY(pk.connectionEnd().isNull());
  QVERIFY(pk.version().isNull());
  QVERIFY(pk.modification().isNull());
  /*
   * Clear start/end connection
   */
  pk.setConnectionStart(UnitConnectionPk(1));
  pk.setConnectionEnd(UnitConnectionPk(2));
  pk.setVersion(LinkVersionPk(100));
  pk.setModification(ModificationPk(ModificationType::New));
  QVERIFY(!pk.isNull());
  // Clear start connection
  pk.clearConnectionStart();
  QVERIFY(pk.connectionStart().isNull());
  QVERIFY(!pk.connectionEnd().isNull());
  QVERIFY(pk.isNull());
  pk.setConnectionStart(UnitConnectionPk(1));
  QVERIFY(!pk.isNull());
  // Clear end connection
  pk.clearConnectionEnd();
  QVERIFY(!pk.connectionStart().isNull());
  QVERIFY(pk.connectionEnd().isNull());
  QVERIFY(pk.isNull());
  /*
   * Equality comparison
   */
  LinkPk lhs, rhs;
  // lsh and rhs are null
  QVERIFY(!(lhs == rhs));
  QVERIFY(lhs != rhs);
  // lhs is null
  lhs.clear();
  rhs.setConnectionStart(UnitConnectionPk(1));
  rhs.setConnectionEnd(UnitConnectionPk(2));
  rhs.setVersion(LinkVersionPk(100));
  rhs.setModification(ModificationPk(ModificationType::New));
  QVERIFY(!(lhs == rhs));
  QVERIFY(lhs != rhs);
  // rhs is null
  lhs.setConnectionStart(UnitConnectionPk(1));
  lhs.setConnectionEnd(UnitConnectionPk(2));
  lhs.setVersion(LinkVersionPk(100));
  lhs.setModification(ModificationPk(ModificationType::New));
  rhs.clear();
  QVERIFY(!(lhs == rhs));
  QVERIFY(lhs != rhs);
  // lhs == rhs
  rhs = lhs;
  QVERIFY(lhs == rhs);
  QVERIFY(!(lhs != rhs));
  // lhs != rhs
  rhs.setConnectionEnd(UnitConnectionPk(20));
  QVERIFY(!(lhs == rhs));
  QVERIFY(lhs != rhs);
}

void LinkTest::linkPkListTest()
{

}

void LinkTest::linkPkListBenchmark()
{
  
}

void LinkTest::linkKeyTest()
{
  using Mdt::CableList::LinkKey;
  using Mdt::CableList::LinkPk;
  using Mdt::CableList::LinkType;
  using Mdt::CableList::LinkTypePk;
  using Mdt::CableList::LinkDirectionType;
  using Mdt::CableList::LinkDirectionPk;
  using Mdt::CableList::UnitConnectionPk;
  using Mdt::CableList::LinkVersionPk;
  using Mdt::CableList::ModificationType;
  using Mdt::CableList::ModificationPk;
  using Mdt::CableList::ArticleConnectionPk;
  using Mdt::CableList::ArticleLinkPk;
  using Mdt::CableList::WirePk;
  using Mdt::CableList::LinkBeamPk;

  LinkPk pk;
  LinkKey key;
  ArticleLinkPk articleLinkPk;

  qDebug() << "sizeof(key): " << sizeof(key);
  
  /*
   * Initial state
   */
  QVERIFY(key.isNull());
  /*
   * Set
   */
  // Set link PK
  pk.setConnectionStart(UnitConnectionPk(1));
  pk.setConnectionEnd(UnitConnectionPk(2));
  pk.setVersion(LinkVersionPk(100));
  pk.setModification(ModificationPk(ModificationType::New));
  QVERIFY(!pk.isNull());
  key.setPk(pk);
  QVERIFY(key.pk() == pk);
  QVERIFY(key.isNull());
  // Set link type
  key.setLinkType(LinkTypePk(LinkType::CableLink));
  QVERIFY(key.linkType().type() == LinkType::CableLink);
  QVERIFY(key.isNull());
  // Set link direction
  key.setLinkDirection(LinkDirectionPk(LinkDirectionType::Bidirectional));
  QVERIFY(key.linkDirection().direction() == LinkDirectionType::Bidirectional);
  // Now, all mandatory FK are set
  QVERIFY(!key.isNull());
  // Set article link
  articleLinkPk.setConnectionStart(ArticleConnectionPk(10));
  articleLinkPk.setConnectionEnd(ArticleConnectionPk(20));
  QVERIFY(!articleLinkPk.isNull());
  key.setArticleLink(articleLinkPk);
  QCOMPARE(key.articleLink().connectionStart().id(), 10);
  QCOMPARE(key.articleLink().connectionEnd().id(), 20);
  QVERIFY(!key.isNull());
  // Set wire
  key.setWire(WirePk(111));
  QCOMPARE(key.wire().id(), 111);
  QVERIFY(!key.isNull());
  // Set link beam
  key.setLinkBeam(LinkBeamPk(555));
  QCOMPARE(key.linkBeam().id(), 555);
  QVERIFY(!key.isNull());
  /*
   * Clear
   */
  QVERIFY(!key.isNull());
  key.clear();
  QVERIFY(key.pk().isNull());
  QVERIFY(key.linkType().isNull());
  QVERIFY(key.linkDirection().isNull());
  QVERIFY(key.articleLink().isNull());
  QVERIFY(key.wire().isNull());
  QVERIFY(key.linkBeam().isNull());
  QVERIFY(key.isNull());
}

void LinkTest::linkDataTest()
{
  using Mdt::CableList::LinkData;
  using Mdt::CableList::LinkKey;
  using Mdt::CableList::LinkPk;
  using Mdt::CableList::LinkType;
  using Mdt::CableList::LinkTypePk;
  using Mdt::CableList::LinkDirectionType;
  using Mdt::CableList::LinkDirectionPk;
  using Mdt::CableList::UnitConnectionPk;
  using Mdt::CableList::LinkVersionPk;
  using Mdt::CableList::ModificationType;
  using Mdt::CableList::ModificationPk;
  using Mdt::CableList::ArticleConnectionPk;
  using Mdt::CableList::ArticleLinkPk;
  using Mdt::CableList::WirePk;
  using Mdt::CableList::LinkBeamPk;
  using Mdt::Numeric::Resistance;

  LinkPk pk;
  LinkKey key;
  ArticleLinkPk articleLinkPk;
  LinkData data;

  /*
   * Initial state
   */
  QVERIFY(data.isNull());
  /*
   * Set key
   */
  // Set link PK part
  pk.setConnectionStart(UnitConnectionPk(1));
  pk.setConnectionEnd(UnitConnectionPk(2));
  pk.setVersion(LinkVersionPk(100));
  pk.setModification(ModificationPk(ModificationType::New));
  QVERIFY(!pk.isNull());
  key.setPk(pk);
  // Set other mandatory parts
  key.setLinkType(LinkTypePk(LinkType::CableLink));
  key.setLinkDirection(LinkDirectionPk(LinkDirectionType::Bidirectional));
  QVERIFY(!key.isNull());
  data.setKey(key);
  QCOMPARE(data.key().pk().connectionStart().id(), 1);
  QVERIFY(!data.isNull());
  /*
   * Set other attributes
   */
  data.setIdentification("1234");
  QCOMPARE(data.identification(), QVariant("1234"));
  data.setResistance(Resistance(1.5));
  QCOMPARE(data.resistance().value().toDouble(), 1.5);
  /// \todo Add also length
  
  /*
   * Clear
   */
  data.clear();
  QVERIFY(data.key().isNull());
  QVERIFY(data.identification().isNull());
  QVERIFY(data.resistance().isNull());
  /// \todo Add also length
}

/*
 * Helper functions
 */

void LinkTest::createDatabaseSchema()
{
  Mdt::CableList::DatabaseSchema schema;

  QVERIFY(pvDatabase.isOpen());
  QVERIFY(schema.createSchema(pvDatabase));
  QVERIFY(schema.checkSchema(pvDatabase));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  LinkTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
