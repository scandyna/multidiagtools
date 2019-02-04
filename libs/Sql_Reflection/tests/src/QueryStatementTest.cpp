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
#include "QueryStatementTest.h"
#include "Mdt/Expected.h"
#include "Mdt/Sql/InsertQuery.h"
#include "Mdt/Sql/PrimaryKeyRecord.h"
#include "Mdt/Sql/Reflection/InsertStatement.h"
#include "Mdt/Sql/Reflection/DeleteStatement.h"
#include "Mdt/Sql/Reflection/PrimaryKeyRecordAlgorithm.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include "Mdt/Reflection/AutoIncrementIdPrimaryKey.h"
#include "Mdt/Reflection/PrimaryKey.h"
#include <QString>
#include <QLatin1String>
#include <type_traits>

using namespace Mdt::Reflection;
// using Mdt::Sql::Schema::FieldType;

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
/*
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
*/

/*
 * Helpers
 */



/*
 * Tests
 */

void QueryStatementTest::insertStatementFromReflectedTest()
{
  using AutoIncIdPk = AutoIncrementIdPrimaryKey<PersonDef::id>;
  using Pk = PrimaryKey<PersonDef::id>;

  PersonDataStruct person;

  person.id = 0;
  person.firstName = QString::fromLocal8Bit("fN");
  person.lastName = QString::fromLocal8Bit("lN");
  auto statement = Mdt::Sql::Reflection::insertStatementFromReflected<PersonDef, AutoIncIdPk>(person);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toFieldNameList(), QStringList({QLatin1String("firstName"),QLatin1String("lastName")}));
  QCOMPARE(statement.toValueList(), QVariantList({QLatin1String("fN"),QLatin1String("lN")}));

  person.id = 1;
  person.firstName = QString::fromLocal8Bit("fN");
  person.lastName = QString::fromLocal8Bit("lN");
  statement = Mdt::Sql::Reflection::insertStatementFromReflected<PersonDef, AutoIncIdPk>(person);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toFieldNameList(), QStringList({QLatin1String("id"),QLatin1String("firstName"),QLatin1String("lastName")}));
  QCOMPARE(statement.toValueList(), QVariantList({1,QLatin1String("fN"),QLatin1String("lN")}));

  /// \todo Check if passing a null ID to a non auto increment PK is valid
  person.id = 0;
  person.firstName = QString::fromLocal8Bit("fN");
  person.lastName = QString::fromLocal8Bit("lN");
  statement = Mdt::Sql::Reflection::insertStatementFromReflected<PersonDef, Pk>(person);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toFieldNameList(), QStringList({QLatin1String("id"),QLatin1String("firstName"),QLatin1String("lastName")}));
  QCOMPARE(statement.toValueList(), QVariantList({0,QLatin1String("fN"),QLatin1String("lN")}));

  person.id = 1;
  person.firstName = QString::fromLocal8Bit("fN");
  person.lastName = QString::fromLocal8Bit("lN");
  statement = Mdt::Sql::Reflection::insertStatementFromReflected<PersonDef, Pk>(person);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toFieldNameList(), QStringList({QLatin1String("id"),QLatin1String("firstName"),QLatin1String("lastName")}));
  QCOMPARE(statement.toValueList(), QVariantList({1,QLatin1String("fN"),QLatin1String("lN")}));
}

void QueryStatementTest::deleteStatementFromReflectedPrimaryKeyTest()
{
  using AutoIncIdPk = AutoIncrementIdPrimaryKey<PersonDef::id>;
  using Pk = PrimaryKey<PersonDef::id>;
  using Pk2 = PrimaryKey<PersonDef::firstName, PersonDef::lastName>;

  auto pkr = Mdt::Sql::Reflection::primaryKeyRecordFromValues<AutoIncIdPk>({1});
  auto statement = Mdt::Sql::Reflection::deleteStatementFromReflectedPrimaryKey<AutoIncIdPk>(pkr);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toConditionsFieldNameList(), QStringList({QLatin1String("id")}));
  QCOMPARE(statement.toConditionsValueList(), QVariantList({1}));

  pkr = Mdt::Sql::Reflection::primaryKeyRecordFromValues<Pk>({1});
  statement = Mdt::Sql::Reflection::deleteStatementFromReflectedPrimaryKey<Pk>(pkr);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toConditionsFieldNameList(), QStringList({QLatin1String("id")}));
  QCOMPARE(statement.toConditionsValueList(), QVariantList({1}));

  pkr = Mdt::Sql::Reflection::primaryKeyRecordFromValues<Pk2>({QLatin1String("a1"),QLatin1String("a2")});
  statement = Mdt::Sql::Reflection::deleteStatementFromReflectedPrimaryKey<Pk2>(pkr);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toConditionsFieldNameList(), QStringList({QLatin1String("firstName"),QLatin1String("lastName")}));
  QCOMPARE(statement.toConditionsValueList(), QVariantList({QLatin1String("a1"),QLatin1String("a2")}));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  QueryStatementTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
