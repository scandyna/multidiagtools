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
#include "ReflectionTest.h"
#include "Mdt/Sql/Schema/Reflection.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include "Mdt/Reflection/IdPrimaryKey.h"
#include "Mdt/Reflection/AutoIncrementIdPrimaryKey.h"
#include "Mdt/Reflection/PrimaryKey.h"
#include "Mdt/Reflection/UniqueConstraint.h"
#include <QLatin1String>
#include <QStringList>

// #include <QDebug>

using namespace Mdt::Sql::Schema;

void ReflectionTest::initTestCase()
{
}

void ReflectionTest::cleanupTestCase()
{
}

/*
 * Person
 */

struct PersonDataStruct
{
  qlonglong id = 0;
  QString firstName;
  QString lastName = QLatin1String("LN");
};

MDT_REFLECT_STRUCT(
  (PersonDataStruct),
  Person,
  (id),
  (firstName, FieldFlag::IsRequired),
  (lastName, FieldFlag::IsRequired, FieldMaxLength(250), FieldFlag::HasDefaultValue)
)

/*
 * Address
 */

struct AddressDataStruct
{
  qlonglong id = 0;
  QString street;
  qlonglong personId = 0;
};

MDT_REFLECT_STRUCT(
  (AddressDataStruct),
  Address,
  (id),
  (street),
  (personId)
)

/*
 * Tests
 */

void ReflectionTest::tableNoPkTest()
{
  auto table = tableFromReflected<PersonDef>();
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

void ReflectionTest::tableAutoIncrementPkTest()
{
  using PersonPrimaryKey = Mdt::Reflection::AutoIncrementIdPrimaryKey<PersonDef::id>;

  auto table = tableFromReflected<PersonDef, PersonPrimaryKey>();
  QCOMPARE(table.fieldCount(), 3);
  QCOMPARE(table.primaryKeyType(), PrimaryKeyType::AutoIncrementPrimaryKey);
  QCOMPARE(table.autoIncrementPrimaryKey().fieldName(), QLatin1String("id"));
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

void ReflectionTest::tablePkTest()
{
  using PersonPrimaryKey = Mdt::Reflection::PrimaryKey<PersonDef::firstName, PersonDef::lastName>;

  auto table = tableFromReflected<PersonDef, PersonPrimaryKey>();
  QCOMPARE(table.fieldCount(), 3);
  QCOMPARE(table.primaryKeyType(), PrimaryKeyType::PrimaryKey);
  QCOMPARE(table.primaryKey().fieldNameList(), QStringList({QLatin1String("firstName"),QLatin1String("lastName")}));
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

void ReflectionTest::personTableTest()
{
  using PersonPrimaryKey = Mdt::Reflection::AutoIncrementIdPrimaryKey<PersonDef::id>;

  auto table = tableFromReflected<PersonDef, PersonPrimaryKey>();
  QCOMPARE(table.tableName(), QLatin1String("Person"));
  QCOMPARE(table.fieldCount(), 3);
  // id
  QCOMPARE(table.fieldName(0), QLatin1String("id"));
  QCOMPARE(table.fieldType(0), FieldType::Bigint);
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(table.isFieldRequired(0));
  QVERIFY(table.isFieldUnique(0));
  QVERIFY(table.isFieldAutoIncrement(0));
  QVERIFY(table.fieldLength(0) < 1);
  QVERIFY(table.fieldDefaultValue(0).isNull());
  // firstName
  QCOMPARE(table.fieldName(1), QLatin1String("firstName"));
  QCOMPARE(table.fieldType(1), FieldType::Text);
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(table.isFieldRequired(1));
  QVERIFY(!table.isFieldUnique(1));
  QVERIFY(!table.isFieldAutoIncrement(1));
  QVERIFY(table.fieldLength(1) < 1);
  QVERIFY(table.fieldDefaultValue(1).isNull());
  // lastName
  QCOMPARE(table.fieldName(2), QLatin1String("lastName"));
  QCOMPARE(table.fieldType(2), FieldType::Varchar);
  QVERIFY(!table.isFieldPartOfPrimaryKey(2));
  QVERIFY(table.isFieldRequired(2));
  QVERIFY(!table.isFieldUnique(2));
  QVERIFY(!table.isFieldAutoIncrement(2));
  QCOMPARE(table.fieldLength(2), 250);
  QCOMPARE(table.fieldDefaultValue(2), QVariant(QLatin1String("LN")));
}

void ReflectionTest::addUniqueConstraintToTableTest()
{
  using PersonUniqueConstraint = Mdt::Reflection::UniqueConstraint<PersonDef::firstName, PersonDef::lastName>;

  auto table = tableFromReflected<PersonDef>();
  QCOMPARE(table.indexList().size(), 0);

  addUniqueConstraintToTable<PersonUniqueConstraint>(table);
  QCOMPARE(table.indexList().size(), 1);
  const auto index = table.indexList().at(0);
  QVERIFY(index.isUnique());
  QCOMPARE(index.fieldNameList(), QStringList({QLatin1String("firstName"),QLatin1String("lastName")}));
}

void ReflectionTest::addForeignKeyFromRelationToTableTest()
{
  using PersonPrimaryKey = Mdt::Reflection::AutoIncrementIdPrimaryKey<PersonDef::id>;
  using AddressPrimaryKey = Mdt::Reflection::AutoIncrementIdPrimaryKey<AddressDef::id>;
  using PersonAddressRelation = Mdt::Reflection::Relation<PersonPrimaryKey, AddressDef::personId>;

  auto personTable = tableFromReflected<PersonDef, PersonPrimaryKey>();
  auto addressTable = tableFromReflected<AddressDef, AddressPrimaryKey>();

  ForeignKeySettings fkSettings;
  fkSettings.setIndexed(true);
  fkSettings.setOnDeleteAction(ForeignKeyAction::Restrict);
  fkSettings.setOnUpdateAction(ForeignKeyAction::Cascade);

  QCOMPARE(addressTable.foreignKeyList().size(), 0);

  addForeignKeyFromRelationToTable<PersonAddressRelation>(addressTable, fkSettings);
  QCOMPARE(addressTable.foreignKeyList().size(), 1);
  const auto fk = addressTable.foreignKeyList().at(0);
  QCOMPARE(fk.tableName(), QLatin1String("Address"));
  QCOMPARE(fk.fieldNameList(), QStringList({QLatin1String("personId")}));
  QCOMPARE(fk.foreignTableName(), QLatin1String("Person"));
  QCOMPARE(fk.foreignTableFieldNameList(), QStringList({QLatin1String("id")}));
  QVERIFY(fk.isIndexed());
  QCOMPARE(fk.onDeleteAction(), ForeignKeyAction::Restrict);
  QCOMPARE(fk.onUpdateAction(), ForeignKeyAction::Cascade);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ReflectionTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
