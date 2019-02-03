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
#include "PrimaryKeyRecordAlgorithmTest.h"
#include "Mdt/Sql/Reflection/PrimaryKeyRecordAlgorithm.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include "Mdt/Reflection/AutoIncrementIdPrimaryKey.h"
#include "Mdt/Reflection/PrimaryKey.h"
#include <QString>
#include <QLatin1String>
#include <type_traits>

// using namespace Mdt::Reflection;

struct PkTestStruct
{
  qlonglong id;
  QString id_A;
  QString id_B;
  QString id_C;
};

MDT_REFLECT_STRUCT(
  (PkTestStruct),
  PkTest,
  (id),
  (id_A),
  (id_B),
  (id_C)
)


/*
 * Tests
 */

using AutoIdPk = Mdt::Reflection::AutoIncrementIdPrimaryKey<PkTestDef::id>;
using OneFieldPk = Mdt::Reflection::PrimaryKey<PkTestDef::id_A>;
using TwoFieldPk = Mdt::Reflection::PrimaryKey<PkTestDef::id_C, PkTestDef::id_A>;
using ThreeFieldPk = Mdt::Reflection::PrimaryKey<PkTestDef::id_A, PkTestDef::id_B, PkTestDef::id_C>;

void PrimaryKeyRecordAlgorithmTest::primaryKeyRecordFromValuesTest()
{
  const auto autoIdPkr = Mdt::Sql::Reflection::primaryKeyRecordFromValues<AutoIdPk>({1});
  QCOMPARE(autoIdPkr.toFieldNameList(), QStringList({QLatin1String("id")}));
  QCOMPARE(autoIdPkr.toValueList(), QVariantList({1}));

  const auto oneFieldPkr = Mdt::Sql::Reflection::primaryKeyRecordFromValues<OneFieldPk>({QLatin1String("a1")});
  QCOMPARE(oneFieldPkr.toFieldNameList(), QStringList({QLatin1String("id_A")}));
  QCOMPARE(oneFieldPkr.toValueList(), QVariantList({QLatin1String("a1")}));

  const auto twoFieldPkr = Mdt::Sql::Reflection::primaryKeyRecordFromValues<TwoFieldPk>({QLatin1String("c1"),QLatin1String("a1")});
  QCOMPARE(twoFieldPkr.toFieldNameList(), QStringList({QLatin1String("id_C"),QLatin1String("id_A")}));
  QCOMPARE(twoFieldPkr.toValueList(), QVariantList({QLatin1String("c1"),QLatin1String("a1")}));

  const auto threeFieldPkr = Mdt::Sql::Reflection::primaryKeyRecordFromValues<ThreeFieldPk>({QLatin1String("a1"),QLatin1String("b1"),QLatin1String("c1")});
  QCOMPARE(threeFieldPkr.toFieldNameList(), QStringList({QLatin1String("id_A"),QLatin1String("id_B"),QLatin1String("id_C")}));
  QCOMPARE(threeFieldPkr.toValueList(), QVariantList({QLatin1String("a1"),QLatin1String("b1"),QLatin1String("c1")}));
}

void PrimaryKeyRecordAlgorithmTest::primaryKeyRecordFromReflectedTest()
{
  PkTestStruct data;

  data.id = 1;
  data.id_A = QString::fromLatin1("a1");
  data.id_B = QString::fromLatin1("b1");
  data.id_C = QString::fromLatin1("c1");

  const auto autoIdPkr = Mdt::Sql::Reflection::primaryKeyRecordFromReflected<AutoIdPk>(data);
  QCOMPARE(autoIdPkr.toFieldNameList(), QStringList({QLatin1String("id")}));
  QCOMPARE(autoIdPkr.toValueList(), QVariantList({1}));

  const auto oneFieldPkr = Mdt::Sql::Reflection::primaryKeyRecordFromReflected<OneFieldPk>(data);
  QCOMPARE(oneFieldPkr.toFieldNameList(), QStringList({QLatin1String("id_A")}));
  QCOMPARE(oneFieldPkr.toValueList(), QVariantList({QLatin1String("a1")}));

  const auto twoFieldPkr = Mdt::Sql::Reflection::primaryKeyRecordFromReflected<TwoFieldPk>(data);
  QCOMPARE(twoFieldPkr.toFieldNameList(), QStringList({QLatin1String("id_C"),QLatin1String("id_A")}));
  QCOMPARE(twoFieldPkr.toValueList(), QVariantList({QLatin1String("c1"),QLatin1String("a1")}));

  const auto threeFieldPkr = Mdt::Sql::Reflection::primaryKeyRecordFromReflected<ThreeFieldPk>(data);
  QCOMPARE(threeFieldPkr.toFieldNameList(), QStringList({QLatin1String("id_A"),QLatin1String("id_B"),QLatin1String("id_C")}));
  QCOMPARE(threeFieldPkr.toValueList(), QVariantList({QLatin1String("a1"),QLatin1String("b1"),QLatin1String("c1")}));
}

void PrimaryKeyRecordAlgorithmTest::primaryKeyRecordCheckTest()
{
  PkTestStruct data;

  data.id = 1;
  data.id_A = QString::fromLatin1("a1");
  data.id_B = QString::fromLatin1("b1");
  data.id_C = QString::fromLatin1("c1");

  const auto autoIdPkr = Mdt::Sql::Reflection::primaryKeyRecordFromReflected<AutoIdPk>(data);
  QVERIFY( Mdt::Sql::Reflection::primaryKeyRecordHasCorrectFieldCount<AutoIdPk>(autoIdPkr) );
  QVERIFY( Mdt::Sql::Reflection::primaryKeyRecordHasCorrectFieldNameList<AutoIdPk>(autoIdPkr) );

  const auto twoFieldPkr = Mdt::Sql::Reflection::primaryKeyRecordFromReflected<TwoFieldPk>(data);
  QVERIFY( Mdt::Sql::Reflection::primaryKeyRecordHasCorrectFieldCount<TwoFieldPk>(twoFieldPkr) );
  QVERIFY( Mdt::Sql::Reflection::primaryKeyRecordHasCorrectFieldNameList<TwoFieldPk>(twoFieldPkr) );

  const auto threeFieldPkr = Mdt::Sql::Reflection::primaryKeyRecordFromReflected<ThreeFieldPk>(data);
  QVERIFY( Mdt::Sql::Reflection::primaryKeyRecordHasCorrectFieldCount<ThreeFieldPk>(threeFieldPkr) );
  QVERIFY( Mdt::Sql::Reflection::primaryKeyRecordHasCorrectFieldNameList<ThreeFieldPk>(threeFieldPkr) );
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  PrimaryKeyRecordAlgorithmTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
