/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "SchemaForeignKeyTest.h"
#include "Mdt/Sql/Schema/ForeignKeySettings.h"
#include "Mdt/Sql/Schema/Reflection/ForeignKeyAlgorithm.h"
#include "Mdt/Reflection/PrimaryKey.h"
#include "Mdt/Reflection/Relation.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>

// #include <QDebug>

using namespace Mdt::Sql::Schema;

/*
struct ArticleDataStruct
{
  int id;
  QString description;
};

MDT_REFLECT_STRUCT(
  (ArticleDataStruct),
  Article,
  (id),
  (description, FieldFlag::IsRequired, FieldMaxLength(250))
)


struct ArticleDetailDataStruct
{
  qlonglong id;
  qlonglong articleId;
};

MDT_REFLECT_STRUCT(
  (ArticleDetailDataStruct),
  ArticleDetail,
  (id),
  (articleId)
)


struct ArticleTypeDataStruct
{
  QString code;
  QString label;
};

MDT_REFLECT_STRUCT(
  (ArticleTypeDataStruct),
  ArticleType,
  (code),
  (label)
)
*/

struct PrimaryEntityDataStruct
{
  int p_id;
  QString p_A_id;
  QString p_B_id;
  QString p_C_id;
};

MDT_REFLECT_STRUCT(
  (PrimaryEntityDataStruct),
  PrimaryEntity,
  (p_id),
  (p_A_id),
  (p_B_id),
  (p_C_id)
)

struct RelatedEntityDataStruct
{
  int f_id;
  QString f_A_id;
  QString f_B_id;
  QString f_C_id;
};

MDT_REFLECT_STRUCT(
  (RelatedEntityDataStruct),
  RelatedEntity,
  (f_id),
  (f_A_id),
  (f_B_id),
  (f_C_id)
)

/*
 * Tests
 */

void SchemaForeignKeyTest::fkSettingsTest()
{
  using Pk = Mdt::Reflection::PrimaryKey<PrimaryEntityDef::p_id>;
  using Relation = Mdt::Reflection::Relation<Pk, RelatedEntityDef::f_id>;

  ForeignKeySettings fkSettings;
  ForeignKey fk;

  fkSettings.setIndexed(false);
  fkSettings.setOnDeleteAction(ForeignKeyAction::NoAction);
  fkSettings.setOnUpdateAction(ForeignKeyAction::Restrict);
  fk = Mdt::Sql::Schema::Reflection::foreignKeyFromRelation<Relation>(fkSettings);
  QVERIFY(!fk.isIndexed());
  QCOMPARE(fk.onDeleteAction(), ForeignKeyAction::NoAction);
  QCOMPARE(fk.onUpdateAction(), ForeignKeyAction::Restrict);

  fkSettings.setIndexed(true);
  fkSettings.setOnDeleteAction(ForeignKeyAction::Cascade);
  fkSettings.setOnUpdateAction(ForeignKeyAction::SetNull);
  fk = Mdt::Sql::Schema::Reflection::foreignKeyFromRelation<Relation>(fkSettings);
  QVERIFY(fk.isIndexed());
  QCOMPARE(fk.onDeleteAction(), ForeignKeyAction::Cascade);
  QCOMPARE(fk.onUpdateAction(), ForeignKeyAction::SetNull);
}

void SchemaForeignKeyTest::oneFieldFkTest()
{
  using Pk = Mdt::Reflection::PrimaryKey<PrimaryEntityDef::p_id>;
  using Relation = Mdt::Reflection::Relation<Pk, RelatedEntityDef::f_id>;

  const auto fk = Mdt::Sql::Schema::Reflection::foreignKeyFromRelation<Relation>( ForeignKeySettings() );
  QCOMPARE(fk.tableName(), QLatin1String("RelatedEntity"));
  QCOMPARE(fk.fieldNameList(), QStringList({QLatin1String("f_id")}));
  QCOMPARE(fk.foreignTableName(), QLatin1String("PrimaryEntity"));
  QCOMPARE(fk.foreignTableFieldNameList(), QStringList({QLatin1String("p_id")}));
}

void SchemaForeignKeyTest::twoFieldFkTest()
{
  using Pk = Mdt::Reflection::PrimaryKey<PrimaryEntityDef::p_A_id, PrimaryEntityDef::p_B_id>;
  using Relation = Mdt::Reflection::Relation<Pk, RelatedEntityDef::f_A_id, RelatedEntityDef::f_B_id>;

  const auto fk = Mdt::Sql::Schema::Reflection::foreignKeyFromRelation<Relation>( ForeignKeySettings() );
  QCOMPARE(fk.tableName(), QLatin1String("RelatedEntity"));
  QCOMPARE(fk.fieldNameList(), QStringList({QLatin1String("f_A_id"),QLatin1String("f_B_id")}));
  QCOMPARE(fk.foreignTableName(), QLatin1String("PrimaryEntity"));
  QCOMPARE(fk.foreignTableFieldNameList(), QStringList({QLatin1String("p_A_id"),QLatin1String("p_B_id")}));
}

void SchemaForeignKeyTest::threeFieldFkTest()
{
  using Pk = Mdt::Reflection::PrimaryKey<PrimaryEntityDef::p_A_id, PrimaryEntityDef::p_B_id, PrimaryEntityDef::p_C_id>;
  using Relation = Mdt::Reflection::Relation<Pk, RelatedEntityDef::f_A_id, RelatedEntityDef::f_B_id, RelatedEntityDef::f_C_id>;

  const auto fk = Mdt::Sql::Schema::Reflection::foreignKeyFromRelation<Relation>( ForeignKeySettings() );
  QCOMPARE(fk.tableName(), QLatin1String("RelatedEntity"));
  QCOMPARE(fk.fieldNameList(), QStringList({QLatin1String("f_A_id"),QLatin1String("f_B_id"),QLatin1String("f_C_id")}));
  QCOMPARE(fk.foreignTableName(), QLatin1String("PrimaryEntity"));
  QCOMPARE(fk.foreignTableFieldNameList(), QStringList({QLatin1String("p_A_id"),QLatin1String("p_B_id"),QLatin1String("p_C_id")}));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SchemaForeignKeyTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
