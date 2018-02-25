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
#include "Mdt/Entity/PrimaryKey.h"
#include "Mdt/Entity/Def.h"

using namespace Mdt::Entity;

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
  QString code;
  qlonglong startId;
  qlonglong endId;
  QString linkName;
};

MDT_ENTITY_DEF(
  (LinkDataStruct),
  Link,
  (code),
  (startId, FieldFlag::IsPrimaryKey),
  (endId, FieldFlag::IsPrimaryKey),
  (linkName, FieldFlag::IsRequired, FieldMaxLength(100))
)

/*
 * Tests
 */

void PrimaryKeyTest::fieldTest()
{
  PrimaryKeyField articlePkField(0, QVariant::LongLong);
  QCOMPARE(articlePkField.fieldIndex(), 0);
  QCOMPARE(articlePkField.fieldType(), QMetaType::LongLong);
  QCOMPARE(articlePkField.fieldName<ArticleDef>(), QString("id"));

  constexpr PrimaryKeyField linkStartPkField(1, QVariant::LongLong);
  QCOMPARE(linkStartPkField.fieldIndex(), 1);
  QCOMPARE(linkStartPkField.fieldType(), QMetaType::LongLong);
  QCOMPARE(linkStartPkField.fieldName<LinkDef>(), QString("startId"));

  PrimaryKeyField linkEndPkField(2, QVariant::LongLong);
  QCOMPARE(linkEndPkField.fieldIndex(), 2);
  QCOMPARE(linkEndPkField.fieldType(), QMetaType::LongLong);
  QCOMPARE(linkEndPkField.fieldName<LinkDef>(), QString("endId"));
}

void PrimaryKeyTest::primaryKeyFromEntityTest()
{
  auto articlePk = PrimaryKey::fromEntity<ArticleEntity>();
  QCOMPARE(articlePk.fieldCount(), 1);
  QCOMPARE(articlePk.fieldAt(0).fieldIndex(), 0);
  QCOMPARE(articlePk.fieldAt(0).fieldType(), QMetaType::LongLong);
  QCOMPARE(articlePk.fieldAt(0).fieldName<ArticleDef>(), QString("id"));
  QCOMPARE(articlePk.toFieldNameList<ArticleDef>(), QStringList({"id"}));

  auto linkPk = PrimaryKey::fromEntity<LinkEntity>();
  QCOMPARE(linkPk.fieldCount(), 2);
  QCOMPARE(linkPk.fieldAt(0).fieldIndex(), 1);
  QCOMPARE(linkPk.fieldAt(0).fieldType(), QMetaType::LongLong);
  QCOMPARE(linkPk.fieldAt(0).fieldName<LinkDef>(), QString("startId"));
  QCOMPARE(linkPk.fieldAt(1).fieldIndex(), 2);
  QCOMPARE(linkPk.fieldAt(1).fieldType(), QMetaType::LongLong);
  QCOMPARE(linkPk.fieldAt(1).fieldName<LinkDef>(), QString("endId"));
  QCOMPARE(linkPk.toFieldNameList<LinkDef>(), QStringList({"startId","endId"}));
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
