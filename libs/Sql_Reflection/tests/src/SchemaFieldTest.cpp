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
#include "SchemaFieldTest.h"
#include "Mdt/Sql/Schema/Reflection/FieldAlgorithm.h"
#include "Mdt/Sql/Schema/Field.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include <QString>
#include <QLatin1String>
#include <type_traits>

using namespace Mdt::Reflection;
using Mdt::Sql::Schema::FieldType;

/*
 * Person struct
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

void SchemaFieldTest::fieldFromReflectedTest()
{
  const TestCaseDataStruct dataStruct{};
  const auto fieldTypeMap = Mdt::Sql::Schema::FieldTypeMap::make();

  const auto id = Mdt::Sql::Schema::Reflection::fieldFromReflected<TestCaseDef::id>(dataStruct, fieldTypeMap);
  QCOMPARE(id.type(), FieldType::Bigint);
  QVERIFY(!id.isUnsigned());
  QCOMPARE(id.name(), QLatin1String("id"));
  QVERIFY(!id.isRequired());  // Required flag for primary key is handled by Mdt::Sql::Schema::Table
  QVERIFY(!id.isUnique());    // Unique flag for primary key is handled by Mdt::Sql::Schema::Table
  QVERIFY(id.defaultValue().isNull());
  QVERIFY(id.length() < 1);
  QVERIFY(!id.isNull());

  const auto name = Mdt::Sql::Schema::Reflection::fieldFromReflected<TestCaseDef::name>(dataStruct, fieldTypeMap);
  QCOMPARE(name.type(), FieldType::Varchar);
  QVERIFY(!name.isUnsigned());
  QCOMPARE(name.name(), QLatin1String("name"));
  QVERIFY(name.isRequired());
  QVERIFY(!name.isUnique());
  QVERIFY(name.defaultValue().isNull());
  QCOMPARE(name.length(), 100);
  QVERIFY(!name.isNull());

  const auto qty = Mdt::Sql::Schema::Reflection::fieldFromReflected<TestCaseDef::qty>(dataStruct, fieldTypeMap);
  QCOMPARE(qty.type(), FieldType::Double);
  QVERIFY(!qty.isUnsigned());
  QCOMPARE(qty.name(), QLatin1String("qty"));
  QVERIFY(!qty.isRequired());
  QVERIFY(!qty.isUnique());
  QCOMPARE(qty.defaultValue(), QVariant(1.0));
  QVERIFY(id.length() < 1);
  QVERIFY(!qty.isNull());
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SchemaFieldTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
