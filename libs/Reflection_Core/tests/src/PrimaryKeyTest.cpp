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
#include "PrimaryKeyTest.h"
#include "Mdt/Reflection/PrimaryKey.h"
#include "Mdt/Reflection/IdPrimaryKey.h"
#include "Mdt/Reflection/AutoIncrementIdPrimaryKey.h"
#include "Mdt/Reflection/PrimaryKeyAlgorithm.h"
#include "Mdt/Reflection/FieldAlgorithm.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include <QLatin1String>
#include <QStringList>
#include <type_traits>

using namespace Mdt::Reflection;

/*
 * Struct
 */

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

/*
 * Compile time tests
 */

void pkFieldCountTest()
{
  using IdPk = Mdt::Reflection::IdPrimaryKey<PkTestDef::int_id>;
  using AutoIncrementIdPk = Mdt::Reflection::AutoIncrementIdPrimaryKey<PkTestDef::qulonglong_id>;
  using Pk1 = Mdt::Reflection::PrimaryKey<PkTestDef::str_A_id>;
  using Pk2 = Mdt::Reflection::PrimaryKey<PkTestDef::str_A_id, PkTestDef::str_B_id>;
  using Pk3 = Mdt::Reflection::PrimaryKey<PkTestDef::str_A_id, PkTestDef::str_C_id, PkTestDef::str_B_id>;

  static_assert( primaryKeyFieldCount<IdPk>() == 1 , "" );
  static_assert( primaryKeyFieldCount<AutoIncrementIdPk>() == 1 , "" );
  static_assert( primaryKeyFieldCount<Pk1>() == 1 , "" );
  static_assert( primaryKeyFieldCount<Pk2>() == 2 , "" );
  static_assert( primaryKeyFieldCount<Pk3>() == 3 , "" );
}

/*
 * Tests
 */

void PrimaryKeyTest::idPkTest()
{
  using Pk = Mdt::Reflection::IdPrimaryKey<PkTestDef::int_id>;

  QCOMPARE(nameFromPrimaryKey<Pk>(), "PkTest");
  QCOMPARE(fieldNameListFromPrimaryKey<Pk>(), QStringList({QLatin1String("int_id")}));
  static_assert( std::is_same<Pk::struct_def, PkTestDef>::value , "" );

  QCOMPARE(fieldNameFromIdPrimaryKeyField<Pk>(), "int_id");
}

void PrimaryKeyTest::autoIdPkTest()
{
  using Pk = Mdt::Reflection::AutoIncrementIdPrimaryKey<PkTestDef::qulonglong_id>;

  QCOMPARE(nameFromPrimaryKey<Pk>(), "PkTest");
  QCOMPARE(fieldNameListFromPrimaryKey<Pk>(), QStringList({QLatin1String("qulonglong_id")}));
  static_assert( std::is_same<Pk::struct_def, PkTestDef>::value , "" );

  QCOMPARE(fieldNameFromAutoIncrementIdPrimaryKeyField<Pk>(), "qulonglong_id");
}

void PrimaryKeyTest::oneFieldPkTest()
{
  using Pk = Mdt::Reflection::PrimaryKey<PkTestDef::str_A_id>;

  QCOMPARE(nameFromPrimaryKey<Pk>(), "PkTest");
  QCOMPARE(fieldNameListFromPrimaryKey<Pk>(), QStringList({QLatin1String("str_A_id")}));
  static_assert( std::is_same<Pk::struct_def, PkTestDef>::value , "" );
}

void PrimaryKeyTest::twoFieldPkTest()
{
  using Pk = Mdt::Reflection::PrimaryKey<PkTestDef::str_A_id, PkTestDef::str_B_id>;

  QCOMPARE(fieldNameListFromPrimaryKey<Pk>(), QStringList({QLatin1String("str_A_id"),QLatin1String("str_B_id")}));
  static_assert( std::is_same<Pk::struct_def, PkTestDef>::value , "" );
}

void PrimaryKeyTest::threeFieldPkTest()
{
  using Pk = Mdt::Reflection::PrimaryKey<PkTestDef::str_A_id, PkTestDef::str_C_id, PkTestDef::str_B_id>;

  QCOMPARE(fieldNameListFromPrimaryKey<Pk>(), QStringList({QLatin1String("str_A_id"),QLatin1String("str_C_id"),QLatin1String("str_B_id")}));
  static_assert( std::is_same<Pk::struct_def, PkTestDef>::value , "" );
}

void PrimaryKeyTest::qMetaTypeFromIdPkTest()
{
  using ShortIntPk = IdPrimaryKey<PkTestDef::short_int_id>;
  using IntPk = IdPrimaryKey<PkTestDef::int_id>;
  using LongLongPk = IdPrimaryKey<PkTestDef::qlonglong_id>;
  using ULongLongPk = IdPrimaryKey<PkTestDef::qulonglong_id>;

  QCOMPARE(qMetaTypeFromIdPrimaryKeyField<ShortIntPk>(), QMetaType::Short);
  QCOMPARE(qMetaTypeFromIdPrimaryKeyField<IntPk>(), QMetaType::Int);
  QCOMPARE(qMetaTypeFromIdPrimaryKeyField<LongLongPk>(), QMetaType::LongLong);
  QCOMPARE(qMetaTypeFromIdPrimaryKeyField<ULongLongPk>(), QMetaType::ULongLong);
}

void PrimaryKeyTest::qMetaTypeFromAutoIncrementIdPkTest()
{
  using ShortIntPk = AutoIncrementIdPrimaryKey<PkTestDef::short_int_id>;
  using IntPk = AutoIncrementIdPrimaryKey<PkTestDef::int_id>;
  using LongLongPk = AutoIncrementIdPrimaryKey<PkTestDef::qlonglong_id>;
  using ULongLongPk = AutoIncrementIdPrimaryKey<PkTestDef::qulonglong_id>;

  QCOMPARE(qMetaTypeFromAutoIncrementIdPrimaryKeyField<ShortIntPk>(), QMetaType::Short);
  QCOMPARE(qMetaTypeFromAutoIncrementIdPrimaryKeyField<IntPk>(), QMetaType::Int);
  QCOMPARE(qMetaTypeFromAutoIncrementIdPrimaryKeyField<LongLongPk>(), QMetaType::LongLong);
  QCOMPARE(qMetaTypeFromAutoIncrementIdPrimaryKeyField<ULongLongPk>(), QMetaType::ULongLong);
}

void PrimaryKeyTest::isNullValuePartOfAutoIncrementIdPrimaryKeyTest()
{
  using AutoIncrementIdPk = AutoIncrementIdPrimaryKey<PkTestDef::int_id>;
  using IdPk = IdPrimaryKey<PkTestDef::int_id>;
  using StrPk = PrimaryKey<PkTestDef::str_A_id>;
  using TwoFieldPk = PrimaryKey<PkTestDef::int_id, PkTestDef::str_A_id>;

  bool is = isNullValuePartOfAutoIncrementIdPrimaryKey<AutoIncrementIdPk, PkTestDef::int_id>(0);
  QVERIFY(is);
  is = isNullValuePartOfAutoIncrementIdPrimaryKey<AutoIncrementIdPk, PkTestDef::int_id>(1);
  QVERIFY(!is);

  is = isNullValuePartOfAutoIncrementIdPrimaryKey<AutoIncrementIdPk, PkTestDef::str_C_id>( QString::fromLatin1("") );
  QVERIFY(!is);
  is = isNullValuePartOfAutoIncrementIdPrimaryKey<AutoIncrementIdPk, PkTestDef::str_C_id>( QString::fromLatin1("A") );
  QVERIFY(!is);

  is = isNullValuePartOfAutoIncrementIdPrimaryKey<IdPk, PkTestDef::int_id>(0);
  QVERIFY(!is);
  is = isNullValuePartOfAutoIncrementIdPrimaryKey<IdPk, PkTestDef::int_id>(1);
  QVERIFY(!is);

  is = isNullValuePartOfAutoIncrementIdPrimaryKey<StrPk, PkTestDef::str_A_id>( QString::fromLatin1("") );
  QVERIFY(!is);
  is = isNullValuePartOfAutoIncrementIdPrimaryKey<StrPk, PkTestDef::str_A_id>( QString::fromLatin1("A") );
  QVERIFY(!is);

  is = isNullValuePartOfAutoIncrementIdPrimaryKey<TwoFieldPk, PkTestDef::int_id>(0);
  QVERIFY(!is);
  is = isNullValuePartOfAutoIncrementIdPrimaryKey<TwoFieldPk, PkTestDef::int_id>(1);
  QVERIFY(!is);
  is = isNullValuePartOfAutoIncrementIdPrimaryKey<TwoFieldPk, PkTestDef::str_A_id>( QString::fromLatin1("") );
  QVERIFY(!is);
  is = isNullValuePartOfAutoIncrementIdPrimaryKey<TwoFieldPk, PkTestDef::str_A_id>( QString::fromLatin1("A") );
  QVERIFY(!is);
  is = isNullValuePartOfAutoIncrementIdPrimaryKey<TwoFieldPk, PkTestDef::str_C_id>( QString::fromLatin1("") );
  QVERIFY(!is);
  is = isNullValuePartOfAutoIncrementIdPrimaryKey<TwoFieldPk, PkTestDef::str_C_id>( QString::fromLatin1("A") );
  QVERIFY(!is);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  PrimaryKeyTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
