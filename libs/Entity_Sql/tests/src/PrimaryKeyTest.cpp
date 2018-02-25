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
#include "Mdt/Entity/Def.h"
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
