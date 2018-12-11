/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "PrimaryKeyTest.h"
#include "Mdt/Entity/SqlPrimaryKey.h"
#include "Mdt/Entity/SqlPrimaryKeyRecord.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/Entity/IntegralUniqueIdTemplate.h"
#include <QString>

using namespace Mdt::Entity;
namespace Sql = Mdt::Sql;

void PrimaryKeyTest::initTestCase()
{
}

void PrimaryKeyTest::cleanupTestCase()
{
}

/*
 * Entities
 */

class ArticleId : public Mdt::Entity::IntegralUniqueIdTemplate<ArticleId, int>
{
 public:

  using IntegralUniqueIdTemplate::IntegralUniqueIdTemplate;
};

struct ArticleDataStruct
{
  qlonglong id;
  QString description;
};

MDT_ENTITY_DEF(
  (ArticleDataStruct),
  Article,
  (id, FieldFlag::IsPrimaryKey),
  (description, FieldFlag::IsRequired, FieldMaxLength(250))
)

class ArticleData : public Mdt::Entity::DataTemplate<ArticleEntity>
{
 public:

  void setId(qlonglong id)
  {
    dataStruct().id = id;
  }
};

struct LinkDataStruct
{
  qlonglong startId;
  qlonglong endId;
  QString linkName;
};

MDT_ENTITY_DEF(
  (LinkDataStruct),
  Link,
  (startId, FieldFlag::IsPrimaryKey),
  (endId, FieldFlag::IsPrimaryKey),
  (linkName, FieldFlag::IsRequired, FieldMaxLength(100))
)

class LinkData : public Mdt::Entity::DataTemplate<LinkEntity>
{
 public:

  void setStartId(qlonglong id)
  {
    dataStruct().startId = id;
  }

  void setEndId(qlonglong id)
  {
    dataStruct().endId = id;
  }
};

struct NoPkDataStruct
{
  QString name;
};

MDT_ENTITY_DEF(
  (NoPkDataStruct),
  NoPk,
  (name)
)

/*
 * Tests
 */

void PrimaryKeyTest::fromEntityTest()
{
  const auto fieldTypeMap = Mdt::Sql::Schema::FieldTypeMap::make();

  auto articlePkc = SqlPrimaryKey::fromEntity<ArticleEntity>(fieldTypeMap);
  QCOMPARE(articlePkc.primaryKeyType(), Sql::Schema::PrimaryKeyType::AutoIncrementPrimaryKey);
  QCOMPARE(articlePkc.autoIncrementPrimaryKey().fieldName(), QString("id"));

  auto linkPkc = SqlPrimaryKey::fromEntity<LinkEntity>(fieldTypeMap);
  QCOMPARE(linkPkc.primaryKeyType(), Sql::Schema::PrimaryKeyType::PrimaryKey);
  auto linkPk = linkPkc.primaryKey();
  QCOMPARE(linkPk.fieldCount(), 2);
  QVERIFY(linkPk.contains("startId"));
  QVERIFY(linkPk.contains("endId"));

  auto noPkPkc = SqlPrimaryKey::fromEntity<NoPkEntity>(fieldTypeMap);
  QVERIFY(noPkPkc.isNull());
}

void PrimaryKeyTest::sqlPkRecordFromEntityDataTest()
{
  ArticleData articleData;
  articleData.setId(15);
  auto articlePkRecord = SqlPrimaryKeyRecord::fromEntityData(articleData);
  QCOMPARE(articlePkRecord.fieldCount(), 1);
  QCOMPARE(articlePkRecord.value("id"), QVariant(15));

  LinkData linkData;
  linkData.setStartId(5);
  linkData.setEndId(9);
  auto linkPkRecord = SqlPrimaryKeyRecord::fromEntityData(linkData);
  QCOMPARE(linkPkRecord.fieldCount(), 2);
  QCOMPARE(linkPkRecord.value("startId"), QVariant(5));
  QCOMPARE(linkPkRecord.value("endId"), QVariant(9));
}

void PrimaryKeyTest::sqlPkRecordFromUniqueId()
{
  ArticleId id(25);
  auto articlePkRecord = SqlPrimaryKeyRecord::fromUniqueId<ArticleEntity>(id);
  QCOMPARE(articlePkRecord.fieldCount(), 1);
  QCOMPARE(articlePkRecord.value("id"), QVariant(25));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  PrimaryKeyTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
