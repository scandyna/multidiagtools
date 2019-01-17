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
#include "SchemaPrimaryKeyTest.h"
#include "Mdt/Sql/Schema/Reflection/PrimaryKeyAlgorithm.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include "Mdt/Reflection/IdPrimaryKey.h"
#include "Mdt/Reflection/AutoIncrementIdPrimaryKey.h"
#include "Mdt/Reflection/PrimaryKey.h"
#include <QString>
#include <QStringList>

// #include <QDebug>

using namespace Mdt::Sql::Schema;

/*
 * Person
 */

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

struct PkTestStruct
{
  short int short_int_id;
  int int_id;
  qlonglong qlonglong_id;
  qulonglong qulonglong_id;
  QString str_A_id;
  QString str_B_id;
  QString str_C_id;
};

MDT_REFLECT_STRUCT(
  (PkTestStruct),
  PkTest,
  (short_int_id),
  (int_id),
  (qlonglong_id),
  (qulonglong_id),
  (str_A_id),
  (str_B_id),
  (str_C_id)
)

using ArticlePrimaryKey = Mdt::Reflection::AutoIncrementIdPrimaryKey<ArticleDef::id>;
using ArticleDetailPrimaryKey = Mdt::Reflection::IdPrimaryKey<ArticleDetailDef::id>;
using ArticleTypePrimaryKey = Mdt::Reflection::PrimaryKey<ArticleTypeDef::code>;

/*
 * Tests
 */

void SchemaPrimaryKeyTest::autoIncrementPkTest()
{
  using ShortIntPk = Mdt::Reflection::AutoIncrementIdPrimaryKey<PkTestDef::short_int_id>;
  using IntPk = Mdt::Reflection::AutoIncrementIdPrimaryKey<PkTestDef::int_id>;
  using QlonglongPk = Mdt::Reflection::AutoIncrementIdPrimaryKey<PkTestDef::qlonglong_id>;

  const auto fieldTypeMap = FieldTypeMap::make();

  const AutoIncrementPrimaryKey shortIntPk = Mdt::Sql::Schema::Reflection::autoIncrementPrimaryKeyFromReflected<ShortIntPk>(fieldTypeMap);
  QCOMPARE(shortIntPk.fieldName(), QString("short_int_id"));
  QCOMPARE(shortIntPk.fieldType(), FieldType::Smallint);

  const AutoIncrementPrimaryKey intPk = Mdt::Sql::Schema::Reflection::autoIncrementPrimaryKeyFromReflected<IntPk>(fieldTypeMap);
  QCOMPARE(intPk.fieldName(), QString("int_id"));
  QCOMPARE(intPk.fieldType(), FieldType::Integer);

  const AutoIncrementPrimaryKey qlonglongPk = Mdt::Sql::Schema::Reflection::autoIncrementPrimaryKeyFromReflected<QlonglongPk>(fieldTypeMap);
  QCOMPARE(qlonglongPk.fieldName(), QString("qlonglong_id"));
  QCOMPARE(qlonglongPk.fieldType(), FieldType::Bigint);

  const AutoIncrementPrimaryKey articlePk = Mdt::Sql::Schema::Reflection::autoIncrementPrimaryKeyFromReflected<ArticlePrimaryKey>(fieldTypeMap);
  QCOMPARE(articlePk.fieldName(), QString("id"));
  QCOMPARE(articlePk.fieldType(), FieldType::Integer);
}

void SchemaPrimaryKeyTest::idPkTest()
{
  const PrimaryKey pk = Mdt::Sql::Schema::Reflection::primaryKeyFromReflectedId<ArticleDetailPrimaryKey>();
  QCOMPARE(pk.fieldNameList(), QStringList({"id"}));
}

void SchemaPrimaryKeyTest::pkTest()
{
  using Pk1 = Mdt::Reflection::PrimaryKey<PkTestDef::int_id>;
  using Pk2 = Mdt::Reflection::PrimaryKey<PkTestDef::str_A_id, PkTestDef::int_id>;
  using Pk3 = Mdt::Reflection::PrimaryKey<PkTestDef::str_A_id, PkTestDef::str_B_id, PkTestDef::str_C_id>;

  const PrimaryKey pk1 = Mdt::Sql::Schema::Reflection::primaryKeyFromReflected<Pk1>();
  QCOMPARE(pk1.fieldNameList(), QStringList({"int_id"}));

  const PrimaryKey pk2 = Mdt::Sql::Schema::Reflection::primaryKeyFromReflected<Pk2>();
  QCOMPARE(pk2.fieldNameList(), QStringList({"str_A_id","int_id"}));

  const PrimaryKey pk3 = Mdt::Sql::Schema::Reflection::primaryKeyFromReflected<Pk3>();
  QCOMPARE(pk3.fieldNameList(), QStringList({"str_A_id","str_B_id","str_C_id"}));
}

void SchemaPrimaryKeyTest::pkContainerTest()
{
  const auto fieldTypeMap = FieldTypeMap::make();

  const auto aiPkc = Mdt::Sql::Schema::Reflection::primaryKeyContainerFromReflected<ArticlePrimaryKey>(fieldTypeMap);
  QCOMPARE(aiPkc.primaryKeyType(), PrimaryKeyType::AutoIncrementPrimaryKey);
  QCOMPARE(aiPkc.fieldNameList(), QStringList({"id"}));

  const auto idPkc = Mdt::Sql::Schema::Reflection::primaryKeyContainerFromReflected<ArticleDetailPrimaryKey>(fieldTypeMap);
  QCOMPARE(idPkc.primaryKeyType(), PrimaryKeyType::PrimaryKey);
  QCOMPARE(idPkc.fieldNameList(), QStringList({"id"}));

  const auto pkc = Mdt::Sql::Schema::Reflection::primaryKeyContainerFromReflected<ArticleTypePrimaryKey>(fieldTypeMap);
  QCOMPARE(pkc.primaryKeyType(), PrimaryKeyType::PrimaryKey);
  QCOMPARE(pkc.fieldNameList(), QStringList({"code"}));

  using Pk2 = Mdt::Reflection::PrimaryKey<ArticleTypeDef::code, ArticleTypeDef::label>;
  const auto pkc2 = Mdt::Sql::Schema::Reflection::primaryKeyContainerFromReflected<Pk2>(fieldTypeMap);
  QCOMPARE(pkc2.primaryKeyType(), PrimaryKeyType::PrimaryKey);
  QCOMPARE(pkc2.fieldNameList(), QStringList({"code","label"}));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SchemaPrimaryKeyTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
