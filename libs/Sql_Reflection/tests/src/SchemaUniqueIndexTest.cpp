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
#include "SchemaUniqueIndexTest.h"
#include "Mdt/Sql/Schema/Reflection/UniqueIndexAlgorithm.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>

// #include <QDebug>

using namespace Mdt::Sql::Schema;


struct TestStruct
{
  int id;
  QString code;
  QString type;
};

MDT_REFLECT_STRUCT(
  (TestStruct),
  Test,
  (id),
  (code),
  (type)
)


/*
 * Tests
 */

void SchemaUniqueIndexTest::oneFieldIndexTest()
{
  using UC = Mdt::Reflection::UniqueConstraint<TestDef::id>;

  const auto index = Mdt::Sql::Schema::Reflection::uniqueIndexFromReflected<UC>();
  QVERIFY(index.isUnique());
  QCOMPARE(index.tableName(), QLatin1String("Test"));
  QVERIFY(!index.name().isEmpty());
  QCOMPARE(index.fieldNameList(), QStringList({QLatin1String("id")}));
  QVERIFY(!index.isNull());
}

void SchemaUniqueIndexTest::twoFieldIndexTest()
{
  using UCA = Mdt::Reflection::UniqueConstraint<TestDef::id, TestDef::code>;
  using UCB = Mdt::Reflection::UniqueConstraint<TestDef::id, TestDef::type>;
  using UCC = Mdt::Reflection::UniqueConstraint<TestDef::code, TestDef::id>;

  const auto indexA = Mdt::Sql::Schema::Reflection::uniqueIndexFromReflected<UCA>();
  QVERIFY(indexA.isUnique());
  QCOMPARE(indexA.tableName(), QLatin1String("Test"));
  QVERIFY(!indexA.name().isEmpty());
  QCOMPARE(indexA.fieldNameList(), QStringList({QLatin1String("id"),QLatin1String("code")}));
  QVERIFY(!indexA.isNull());

  const auto indexB = Mdt::Sql::Schema::Reflection::uniqueIndexFromReflected<UCB>();
  QVERIFY(indexB.isUnique());
  QCOMPARE(indexB.tableName(), QLatin1String("Test"));
  QVERIFY(!indexB.name().isEmpty());
  QCOMPARE(indexB.fieldNameList(), QStringList({QLatin1String("id"),QLatin1String("type")}));
  QVERIFY(!indexB.isNull());

  const auto indexC = Mdt::Sql::Schema::Reflection::uniqueIndexFromReflected<UCC>();
  QVERIFY(indexC.isUnique());
  QCOMPARE(indexC.tableName(), QLatin1String("Test"));
  QVERIFY(!indexC.name().isEmpty());
  QCOMPARE(indexC.fieldNameList(), QStringList({QLatin1String("code"),QLatin1String("id")}));
  QVERIFY(!indexC.isNull());
}

void SchemaUniqueIndexTest::threeFieldIndexTest()
{
  using UC = Mdt::Reflection::UniqueConstraint<TestDef::id, TestDef::code, TestDef::type>;

  const auto index = Mdt::Sql::Schema::Reflection::uniqueIndexFromReflected<UC>();
  QVERIFY(index.isUnique());
  QCOMPARE(index.tableName(), QLatin1String("Test"));
  QVERIFY(!index.name().isEmpty());
  QCOMPARE(index.fieldNameList(), QStringList({QLatin1String("id"),QLatin1String("code"),QLatin1String("type")}));
  QVERIFY(!index.isNull());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SchemaUniqueIndexTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
