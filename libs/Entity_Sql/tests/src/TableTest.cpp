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
#include "TableTest.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/Relation.h"
#include "Mdt/Entity/UniqueConstraint.h"
#include "Mdt/Entity/SqlTable.h"
#include "Mdt/Sql/Schema/Table.h"
#include <QString>

using namespace Mdt::Entity;
namespace Sql = Mdt::Sql;

struct ArticleDataStruct
{
  qlonglong id;
  QString description;
  double qty;
};

MDT_ENTITY_DEF(
  (ArticleDataStruct),
  Article,
  (id, FieldFlag::IsPrimaryKey),
  (description, FieldMaxLength(250)),
  (qty)
)

struct ArticleVendorDataStruct
{
  qlonglong id;
  qlonglong articleId;
};

MDT_ENTITY_DEF(
  (ArticleVendorDataStruct),
  ArticleVendor,
  (id, FieldFlag::IsPrimaryKey),
  (articleId)
)

using ArticleVendorUniqueConstraint = UniqueConstraint<ArticleVendorEntity, ArticleVendorDef::idField, ArticleVendorDef::articleIdField>;

using ArticleVendorRelation = Relation<ArticleEntity, ArticleVendorEntity, ArticleVendorDef::articleIdField>;

void TableTest::initTestCase()
{
}

void TableTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void TableTest::fromEntityTest()
{
  using SqlFieldType = Sql::Schema::FieldType;

  const auto table = SqlTable::fromEntity<ArticleEntity>();

  QCOMPARE(table.tableName(), QString("Article"));
  QCOMPARE(table.fieldCount(), 3);
  // id
  QCOMPARE(table.fieldName(0), QString("id"));
  QCOMPARE(table.fieldType(0), SqlFieldType::Bigint);
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(table.isFieldRequired(0));
  QVERIFY(table.isFieldUnique(0));
  QVERIFY(table.isFieldAutoIncrement(0));
  QVERIFY(table.fieldLength(0) < 1);
  // description
  QCOMPARE(table.fieldName(1), QString("description"));
  QCOMPARE(table.fieldType(1), SqlFieldType::Varchar);
  QVERIFY(!table.isFieldRequired(1));
  QVERIFY(!table.isFieldUnique(1));
  QCOMPARE(table.fieldLength(1), 250);
  // qty
  QCOMPARE(table.fieldName(2), QString("qty"));
  QCOMPARE(table.fieldType(2), SqlFieldType::Double);
  QVERIFY(!table.isFieldRequired(2));
  QVERIFY(!table.isFieldUnique(2));
  QVERIFY(table.fieldLength(2) < 1);
}

void TableTest::addForeignKeyToTableTest()
{
  using Sql::Schema::ForeignKeyAction;

  auto articleVendorTable = SqlTable::fromEntity<ArticleVendorEntity>();

  QCOMPARE(articleVendorTable.foreignKeyList().size(), 0);

  Sql::Schema::ForeignKeySettings fkSetting;
  fkSetting.setIndexed(true);
  fkSetting.setOnDeleteAction(ForeignKeyAction::Restrict);
  fkSetting.setOnUpdateAction(ForeignKeyAction::Cascade);

  SqlTable::addForeignKeyFromRelationToTable<ArticleVendorRelation>(articleVendorTable, fkSetting);
  QCOMPARE(articleVendorTable.foreignKeyList().size(), 1);
  const auto fk = articleVendorTable.foreignKeyList().at(0);
  QCOMPARE(fk.parentTableName(), QString("Article"));
  QCOMPARE(fk.childTableName(), QString("ArticleVendor"));
  QCOMPARE(fk.parentTableFieldNameList(), QStringList({"id"}));
  QCOMPARE(fk.childTableFieldNameList(), QStringList({"articleId"}));
  QCOMPARE(fk.onDeleteAction(), ForeignKeyAction::Restrict);
  QCOMPARE(fk.onUpdateAction(), ForeignKeyAction::Cascade);
  QVERIFY(fk.isIndexed());
}

void TableTest::addUniqueConstraintToTableTest()
{
  auto articleVendorTable = SqlTable::fromEntity<ArticleVendorEntity>();
  QCOMPARE(articleVendorTable.indexList().size(), 0);
  SqlTable::addUniqueConstraintToTable<ArticleVendorUniqueConstraint>(articleVendorTable);
  QCOMPARE(articleVendorTable.indexList().size(), 1);
  const auto index = articleVendorTable.indexList().at(0);
  QVERIFY(index.isUnique());
  QCOMPARE(index.fieldCount(), 2);
  QCOMPARE(index.fieldName(0), QString("id"));
  QCOMPARE(index.fieldName(1), QString("articleId"));
  QVERIFY(!index.name().isEmpty());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  TableTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
