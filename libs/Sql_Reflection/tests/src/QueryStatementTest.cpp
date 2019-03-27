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
#include "Mdt/Sql/ReflectionDeleteStatement.h"
#include "Mdt/Sql/Reflection/InsertStatement.h"
#include "Mdt/Sql/Reflection/DeleteStatement.h"
#include "Mdt/Sql/Reflection/UpdateStatement.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include "Mdt/Reflection/AutoIncrementIdPrimaryKey.h"
#include "Mdt/Reflection/PrimaryKey.h"
#include "Mdt/Reflection/PrimaryKeyRecord.h"
#include "Mdt/Reflection/PrimaryKeyRecordAlgorithm.h"
#include "Mdt/QueryExpression/FilterExpression.h"
#include "Mdt/QueryExpression/Debug/ExpressionTreeToString.h"
#include <QString>
#include <QLatin1String>
#include <QLatin1Char>
#include <type_traits>

using namespace Mdt::Reflection;

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
 * Helpers
 */

QString toInfixExpressionString(const Mdt::QueryExpression::FilterExpression & filter)
{
  Q_ASSERT(!filter.isNull());

  return Mdt::QueryExpression::Debug::filterExpressionToInfixString(filter);
}

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
  auto statement = Mdt::Sql::Reflection::insertStatementFromReflected<AutoIncIdPk>(person);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toFieldNameList(), QStringList({QLatin1String("firstName"),QLatin1String("lastName")}));
  QCOMPARE(statement.toValueList(), QVariantList({QLatin1String("fN"),QLatin1String("lN")}));

  person.id = 1;
  person.firstName = QString::fromLocal8Bit("fN");
  person.lastName = QString::fromLocal8Bit("lN");
  statement = Mdt::Sql::Reflection::insertStatementFromReflected<AutoIncIdPk>(person);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toFieldNameList(), QStringList({QLatin1String("id"),QLatin1String("firstName"),QLatin1String("lastName")}));
  QCOMPARE(statement.toValueList(), QVariantList({1,QLatin1String("fN"),QLatin1String("lN")}));

  /// \todo Check if passing a null ID to a non auto increment PK is valid
  person.id = 0;
  person.firstName = QString::fromLocal8Bit("fN");
  person.lastName = QString::fromLocal8Bit("lN");
  statement = Mdt::Sql::Reflection::insertStatementFromReflected<Pk>(person);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toFieldNameList(), QStringList({QLatin1String("id"),QLatin1String("firstName"),QLatin1String("lastName")}));
  QCOMPARE(statement.toValueList(), QVariantList({0,QLatin1String("fN"),QLatin1String("lN")}));

  person.id = 1;
  person.firstName = QString::fromLocal8Bit("fN");
  person.lastName = QString::fromLocal8Bit("lN");
  statement = Mdt::Sql::Reflection::insertStatementFromReflected<Pk>(person);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toFieldNameList(), QStringList({QLatin1String("id"),QLatin1String("firstName"),QLatin1String("lastName")}));
  QCOMPARE(statement.toValueList(), QVariantList({1,QLatin1String("fN"),QLatin1String("lN")}));
}

void QueryStatementTest::deleteStatementFromReflectedPrimaryKeyTest()
{
  using AutoIncIdPk = AutoIncrementIdPrimaryKey<PersonDef::id>;
  using Pk = PrimaryKey<PersonDef::id>;
  using Pk2 = PrimaryKey<PersonDef::firstName, PersonDef::lastName>;

  using AutoIncIdPkRecord = PrimaryKeyRecord<AutoIncIdPk>;
  using PkRecord = PrimaryKeyRecord<Pk>;
  using PkRecord2 = PrimaryKeyRecord<Pk2>;

  QString expectedExpression;

  AutoIncIdPkRecord aipkr;
  aipkr.setValue<PersonDef::id>(1);
  auto statement = Mdt::Sql::Reflection::deleteStatementFromReflectedByPrimaryKey(aipkr);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  expectedExpression = QLatin1String("Person.id==1");
  QCOMPARE(toInfixExpressionString(statement.internalConditionsFilterExpression()), expectedExpression);

  PkRecord pkr;
  pkr.setValue<PersonDef::id>(1);
  statement = Mdt::Sql::Reflection::deleteStatementFromReflectedByPrimaryKey(pkr);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  expectedExpression = QLatin1String("Person.id==1");
  QCOMPARE(toInfixExpressionString(statement.internalConditionsFilterExpression()), expectedExpression);

  PkRecord2 pkr2;
  pkr2.setValue<PersonDef::firstName>(QLatin1String("a1"));
  pkr2.setValue<PersonDef::lastName>(QLatin1String("a2"));
  statement = Mdt::Sql::Reflection::deleteStatementFromReflectedByPrimaryKey(pkr2);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  expectedExpression = QLatin1String("(Person.firstName==a1)&&(Person.lastName==a2)");
  QCOMPARE(toInfixExpressionString(statement.internalConditionsFilterExpression()), expectedExpression);
}

void QueryStatementTest::deleteStatementFromReflectedByIdTest()
{
  using AutoIncIdPk = AutoIncrementIdPrimaryKey<PersonDef::id>;
  using Pk = PrimaryKey<PersonDef::id>;

  QString expectedExpression;

  auto statement = Mdt::Sql::Reflection::deleteStatementFromReflectedById<AutoIncIdPk>(1);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  expectedExpression = QLatin1String("Person.id==1");
  QCOMPARE(toInfixExpressionString(statement.internalConditionsFilterExpression()), expectedExpression);

  statement = Mdt::Sql::Reflection::deleteStatementFromReflectedById<Pk>(2);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  expectedExpression = QLatin1String("Person.id==2");
  QCOMPARE(toInfixExpressionString(statement.internalConditionsFilterExpression()), expectedExpression);
}

void QueryStatementTest::deleteAllStatementFromReflectedTest()
{
  auto statement = Mdt::Sql::Reflection::deleteAllStatementFromReflected<PersonDef>();
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QVERIFY(statement.internalConditionsFilterExpression().isNull());
}

void QueryStatementTest::updateStatementFromReflectedByPrimaryKeyTest()
{
  using AutoIncIdPk = AutoIncrementIdPrimaryKey<PersonDef::id>;
  using Pk = PrimaryKey<PersonDef::id>;
  using Pk2 = PrimaryKey<PersonDef::firstName, PersonDef::lastName>;

  using AutoIncIdPkRecord = PrimaryKeyRecord<AutoIncIdPk>;
  using PkRecord = PrimaryKeyRecord<Pk>;
  using PkRecord2 = PrimaryKeyRecord<Pk2>;

  QString expectedExpression;
  PersonDataStruct person;

  person.id = 1;
  person.firstName = QString::fromLocal8Bit("fN");
  person.lastName = QString::fromLocal8Bit("lN");

  AutoIncIdPkRecord aipkr;
  aipkr.setValue<PersonDef::id>(1);
  auto statement = Mdt::Sql::Reflection::updateStatementFromReflectedByPrimaryKey(person, aipkr);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toFieldNameList(), QStringList({QLatin1String("firstName"),QLatin1String("lastName")}));
  QCOMPARE(statement.toValueList(), QVariantList({QLatin1String("fN"),QLatin1String("lN")}));
  expectedExpression = QLatin1String("Person.id==1");
  QCOMPARE(toInfixExpressionString(statement.internalConditionsFilterExpression()), expectedExpression);

  PkRecord pkr;
  pkr.setValue<PersonDef::id>(1);
  statement = Mdt::Sql::Reflection::updateStatementFromReflectedByPrimaryKey(person, pkr);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toFieldNameList(), QStringList({QLatin1String("firstName"),QLatin1String("lastName")}));
  QCOMPARE(statement.toValueList(), QVariantList({QLatin1String("fN"),QLatin1String("lN")}));
  expectedExpression = QLatin1String("Person.id==1");
  QCOMPARE(toInfixExpressionString(statement.internalConditionsFilterExpression()), expectedExpression);

  PkRecord2 pkr2;
  pkr2.setValue<PersonDef::firstName>(QLatin1String("a1"));
  pkr2.setValue<PersonDef::lastName>(QLatin1String("a2"));
  statement = Mdt::Sql::Reflection::updateStatementFromReflectedByPrimaryKey(person, pkr2);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toFieldNameList(), QStringList({QLatin1String("id")}));
  QCOMPARE(statement.toValueList(), QVariantList({1}));
  expectedExpression = QLatin1String("(Person.firstName==a1)&&(Person.lastName==a2)");
  QCOMPARE(toInfixExpressionString(statement.internalConditionsFilterExpression()), expectedExpression);
}

void QueryStatementTest::updateStatementFromReflectedTest()
{
  using AutoIncIdPk = AutoIncrementIdPrimaryKey<PersonDef::id>;
  using Pk2 = PrimaryKey<PersonDef::firstName, PersonDef::lastName>;

  QString expectedExpression;
  PersonDataStruct person;

  person.id = 1;
  person.firstName = QString::fromLocal8Bit("fN");
  person.lastName = QString::fromLocal8Bit("lN");

  auto statement = Mdt::Sql::Reflection::updateStatementFromReflected<AutoIncIdPk>(person);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toFieldNameList(), QStringList({QLatin1String("firstName"),QLatin1String("lastName")}));
  QCOMPARE(statement.toValueList(), QVariantList({QLatin1String("fN"),QLatin1String("lN")}));
  expectedExpression = QLatin1String("Person.id==1");
  QCOMPARE(toInfixExpressionString(statement.internalConditionsFilterExpression()), expectedExpression);

  statement = Mdt::Sql::Reflection::updateStatementFromReflected<Pk2>(person);
  QCOMPARE(statement.tableName(), QLatin1String("Person"));
  QCOMPARE(statement.toFieldNameList(), QStringList({QLatin1String("id")}));
  QCOMPARE(statement.toValueList(), QVariantList({1}));
  expectedExpression = QLatin1String("(Person.firstName==fN)&&(Person.lastName==lN)");
  QCOMPARE(toInfixExpressionString(statement.internalConditionsFilterExpression()), expectedExpression);
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
