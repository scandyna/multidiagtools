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
#include "SchemaTableTest.h"
#include "Mdt/Sql/Schema/Reflection/TableAlgorithm.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include "Mdt/Reflection/IdPrimaryKey.h"
#include "Mdt/Reflection/AutoIncrementIdPrimaryKey.h"
#include "Mdt/Reflection/PrimaryKey.h"

#include <QDebug>

using namespace Mdt::Sql::Schema;

void SchemaTableTest::initTestCase()
{
}

void SchemaTableTest::cleanupTestCase()
{
}

/*
 * Person
 */

struct PersonDataStruct
{
  qlonglong id = 0;
  QString firstName;
  QString lastName = "LN";
};

MDT_REFLECT_STRUCT(
  (PersonDataStruct),
  Person,
  (id),
  (firstName, FieldFlag::IsRequired),
  (lastName, FieldFlag::IsRequired, FieldMaxLength(250), FieldFlag::HasDefaultValue)
)

/*
 * Tests
 */

void SchemaTableTest::tableNoPkTest()
{
  const auto fieldTypeMap = Mdt::Sql::Schema::FieldTypeMap::make();

  auto table = Mdt::Sql::Schema::Reflection::tableFromReflectedImpl<PersonDef>(fieldTypeMap);
  QCOMPARE(table.fieldCount(), 3);
  QCOMPARE(table.primaryKeyType(), PrimaryKeyType::Unknown);
  // id
  QVERIFY(!table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldAutoIncrement(0));
  // firstName
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(!table.isFieldAutoIncrement(1));
  // lastName
  QVERIFY(!table.isFieldPartOfPrimaryKey(2));
  QVERIFY(!table.isFieldAutoIncrement(2));
}

void SchemaTableTest::tableAutoIncrementPkTest()
{
  using PersonPrimaryKey = Mdt::Reflection::AutoIncrementIdPrimaryKey<PersonDef::id>;

  const auto fieldTypeMap = Mdt::Sql::Schema::FieldTypeMap::make();

  auto table = Mdt::Sql::Schema::Reflection::tableFromReflectedImpl<PersonDef, PersonPrimaryKey>(fieldTypeMap);
  QCOMPARE(table.fieldCount(), 3);
  QCOMPARE(table.primaryKeyType(), PrimaryKeyType::AutoIncrementPrimaryKey);
  QCOMPARE(table.autoIncrementPrimaryKey().fieldName(), QString("id"));
  // id
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(table.isFieldAutoIncrement(0));
  // firstName
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(!table.isFieldAutoIncrement(1));
  // lastName
  QVERIFY(!table.isFieldPartOfPrimaryKey(2));
  QVERIFY(!table.isFieldAutoIncrement(2));
}

void SchemaTableTest::tablePkTest()
{
  using PersonPrimaryKey = Mdt::Reflection::PrimaryKey<PersonDef::firstName, PersonDef::lastName>;

  const auto fieldTypeMap = Mdt::Sql::Schema::FieldTypeMap::make();

  auto table = Mdt::Sql::Schema::Reflection::tableFromReflectedImpl<PersonDef, PersonPrimaryKey>(fieldTypeMap);
  QCOMPARE(table.fieldCount(), 3);
  QCOMPARE(table.primaryKeyType(), PrimaryKeyType::PrimaryKey);
  QCOMPARE(table.primaryKey().fieldNameList(), QStringList({"firstName","lastName"}));
  // id
  QVERIFY(!table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldAutoIncrement(0));
  // firstName
  QVERIFY(table.isFieldPartOfPrimaryKey(1));
  QVERIFY(!table.isFieldAutoIncrement(1));
  // lastName
  QVERIFY(table.isFieldPartOfPrimaryKey(2));
  QVERIFY(!table.isFieldAutoIncrement(2));
}

void SchemaTableTest::personTableTest()
{
  using PersonPrimaryKey = Mdt::Reflection::AutoIncrementIdPrimaryKey<PersonDef::id>;

  const auto fieldTypeMap = Mdt::Sql::Schema::FieldTypeMap::make();

  auto table = Mdt::Sql::Schema::Reflection::tableFromReflectedImpl<PersonDef, PersonPrimaryKey>(fieldTypeMap);
  QCOMPARE(table.tableName(), QString("Person"));
  QCOMPARE(table.fieldCount(), 3);
  // id
  QCOMPARE(table.fieldName(0), QString("id"));
  QCOMPARE(table.fieldType(0), FieldType::Bigint);
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(table.isFieldRequired(0));
  QVERIFY(table.isFieldUnique(0));
  QVERIFY(table.isFieldAutoIncrement(0));
  QVERIFY(table.fieldLength(0) < 1);
  QVERIFY(table.fieldDefaultValue(0).isNull());
  // firstName
  QCOMPARE(table.fieldName(1), QString("firstName"));
  QCOMPARE(table.fieldType(1), FieldType::Text);
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(table.isFieldRequired(1));
  QVERIFY(!table.isFieldUnique(1));
  QVERIFY(!table.isFieldAutoIncrement(1));
  QVERIFY(table.fieldLength(1) < 1);
  QVERIFY(table.fieldDefaultValue(1).isNull());
  // lastName
  QCOMPARE(table.fieldName(2), QString("lastName"));
  QCOMPARE(table.fieldType(2), FieldType::Varchar);
  QVERIFY(!table.isFieldPartOfPrimaryKey(2));
  QVERIFY(table.isFieldRequired(2));
  QVERIFY(!table.isFieldUnique(2));
  QVERIFY(!table.isFieldAutoIncrement(2));
  QCOMPARE(table.fieldLength(2), 250);
  QCOMPARE(table.fieldDefaultValue(2), QVariant("LN"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SchemaTableTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
