/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "StorageTableTest.h"
#include "Mdt/Sql/ReflectionStorageTableWithId.h"
#include "Mdt/Sql/Schema/Reflection.h"
#include "Mdt/Sql/Schema/Driver.h"
// #include "Mdt/Sql/Schema/Reflection/FieldAlgorithm.h"
// #include "Mdt/Sql/Schema/Field.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include "Mdt/Reflection/AutoIncrementIdPrimaryKey.h"
#include "Mdt/Numeric/IntegralUniqueIdTemplate.h"
#include <QString>
#include <QLatin1String>
#include <type_traits>

#include <QDebug>

using Mdt::Sql::ReflectionStorageTableWithId;
using namespace Mdt::Reflection;

void StorageTableTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  createDatabaseSchema();
}

void StorageTableTest::cleanupTestCase()
{
}

/*
 * Person
 */

struct PersonDataStruct
{
  int id;
  QString firstName;
  QString lastName;
};

MDT_REFLECT_STRUCT(
  (PersonDataStruct),
  Person,
  (id),
  (firstName, FieldFlag::IsRequired),
  (lastName, FieldFlag::HasDefaultValue)
)

class PersonId : public Mdt::Numeric::IntegralUniqueIdTemplate<PersonId, int>
{
 public:

  using IntegralUniqueIdTemplate::IntegralUniqueIdTemplate;
};

using PersonPrimaryKey = Mdt::Reflection::AutoIncrementIdPrimaryKey<PersonDef::id>;

/*
 * Entities
 */

struct TestCaseDataStruct
{
  qlonglong id = 0;
  QString name;
  double qty = 1.0;
};

MDT_REFLECT_STRUCT(
  (TestCaseDataStruct),
  TestCase,
  (id),
  (name, FieldFlag::IsRequired, FieldMaxLength(100)),
  (qty, FieldFlag::HasDefaultValue)
)

struct MyStruct
{
  int i = 0;
};
Q_DECLARE_METATYPE(MyStruct)

struct FieldTypeTestStruct
{
  int int_type;
  QString QString_type;
  MyStruct MyStruct_type;
};

MDT_REFLECT_STRUCT(
  (FieldTypeTestStruct),
  FieldTypeTest,
  (int_type),
  (QString_type),
  (MyStruct_type)
)

/*
 * Tests
 */

void StorageTableTest::tableWithIdConstructTest()
{
  const auto db = database();
  Mdt::Sql::ReflectionStorageTableWithId<PersonDef, PersonPrimaryKey, PersonId::value_type> table(db);

  QFAIL("Not complete");
}

void StorageTableTest::tableWithIdTest()
{
  const auto db = database();
  Mdt::Sql::ReflectionStorageTableWithId<PersonDef, PersonPrimaryKey, PersonId::value_type> table(db);
  Mdt::Expected<int> idEx;
  Mdt::Expected<PersonDataStruct> personEx;
  PersonDataStruct person;

  /*
   * Add
   */

  person.id = 1;
  person.firstName = QLatin1String("Fn1");
  person.lastName = QLatin1String("Ln1");
  idEx = table.add(person);
  QVERIFY(idEx);
  QCOMPARE(*idEx, 1);

  // Check auto increment
  person.id = 0;
  person.firstName = QLatin1String("Fn2");
  person.lastName = QLatin1String("Ln2");
  idEx = table.add(person);
  QVERIFY(idEx);
  QCOMPARE(*idEx, 2);

  /*
   * Get
   */

  personEx = table.get(1);
  QVERIFY(personEx);
  QCOMPARE(personEx->id, 1);
  QCOMPARE(personEx->firstName, QLatin1String("Fn1"));
  QCOMPARE(personEx->lastName, QLatin1String("Ln1"));

  personEx = table.get(5);
  QVERIFY(!personEx);
  QVERIFY(personEx.error().isError(Mdt::ErrorCode::NotFound));

  /*
   * Update
   */

  person.id = 1;
  person.firstName = QLatin1String("UFn1");
  person.lastName = QLatin1String("ULn1");
  QVERIFY(table.update(person));
  personEx = table.get(1);
  QVERIFY(personEx);
  QCOMPARE(personEx->id, 1);
  QCOMPARE(personEx->firstName, QLatin1String("UFn1"));
  QCOMPARE(personEx->lastName, QLatin1String("ULn1"));

  person.id = 3;
  person.firstName = QLatin1String("Fn3");
  person.lastName = QLatin1String("Ln3");
  idEx = table.add(person);
  QVERIFY(idEx);

  /*
   * Remove
   */

  personEx = table.get(1);
  QVERIFY(personEx);
  QCOMPARE(personEx->id, 1);
  personEx = table.get(2);
  QVERIFY(personEx);
  QCOMPARE(personEx->id, 2);
  personEx = table.get(3);
  QVERIFY(personEx);
  QCOMPARE(personEx->id, 3);

  QVERIFY(table.remove(2));
  personEx = table.get(1);
  QVERIFY(personEx);
  QCOMPARE(personEx->id, 1);
  personEx = table.get(2);
  QVERIFY(!personEx);
  personEx = table.get(3);
  QVERIFY(personEx);
  QCOMPARE(personEx->id, 3);

  QVERIFY(table.removeAll());
  personEx = table.get(1);
  QVERIFY(!personEx);
  personEx = table.get(2);
  QVERIFY(!personEx);
  personEx = table.get(3);
  QVERIFY(!personEx);
}

/*
 * Database schema
 */

void StorageTableTest::createDatabaseSchema()
{
  Mdt::Sql::Schema::Driver driver(database());
  QVERIFY(driver.isValid());

  auto personTable = Mdt::Sql::Schema::tableFromReflected<PersonDef, PersonPrimaryKey>();
  QVERIFY(driver.createTable(personTable));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  StorageTableTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
