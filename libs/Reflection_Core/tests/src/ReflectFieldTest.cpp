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
#include "ReflectFieldTest.h"
#include "Mdt/Reflection/ReflectField.h"
#include "Mdt/Reflection/FieldAlgorithm.h"
#include <QString>
#include <type_traits>

/*
 * Note: to check that the macro works properly,
 * we not declare using namespace Mdt::Reflection
 */

/*
 * Type traits tests
 */

/*
 * Tests
 */

namespace FieldGetFieldTest{

  using int1 = MDT_REFLECTION_STRUCT_FIELD_GET_FIELD( (int) );
  static_assert( std::is_same<int1, int>::value, "" );

  using int2 = MDT_REFLECTION_STRUCT_FIELD_GET_FIELD( (int, FieldFlag::IsRequired) );
  static_assert( std::is_same<int2, int>::value, "" );

} // namespace FieldGetFieldTest{


struct FieldDefTest1
{
  MDT_REFLECTION_STRUCT_FIELD_DEF(FieldDefTest1, (id) )
};
static_assert( std::is_same<FieldDefTest1::id::struct_def, FieldDefTest1>::value , "" );
static_assert( !Mdt::Reflection::isFieldRequired<FieldDefTest1::id>(), "" );

struct FieldDefTest2
{
  MDT_REFLECTION_STRUCT_FIELD_DEF(FieldDefTest2, (name, FieldFlag::IsRequired) )
};
static_assert( std::is_same<FieldDefTest2::name::struct_def, FieldDefTest2>::value , "" );
static_assert( Mdt::Reflection::isFieldRequired<FieldDefTest2::name>(), "" );

namespace Ns3{
  struct FieldDefTest
  {
    MDT_REFLECTION_STRUCT_FIELD_DEF(FieldDefTest, (name, FieldFlag::IsRequired) )
  };
} // namespace Ns3{
static_assert( std::is_same<Ns3::FieldDefTest::name::struct_def, Ns3::FieldDefTest>::value , "" );

void ReflectFieldTest::getFieldAttributesTest()
{
  using namespace Mdt::Reflection;

  FieldAttributes fa;

  fa = FieldAttributes( MDT_REFLECTION_STRUCT_FIELD_GET_FIELD_ATTRIBUTES_ENUM( (int) ) );
  QVERIFY(fa.isNull());

  fa = FieldAttributes( MDT_REFLECTION_STRUCT_FIELD_GET_FIELD_ATTRIBUTES_ENUM( (int, FieldFlag::IsRequired) ) );
  QVERIFY(!fa.isNull());
  QVERIFY(fa.isRequired());
  QVERIFY(!fa.hasDefaultValue());
  QCOMPARE(fa.maxLength(), 0);

  fa = FieldAttributes( MDT_REFLECTION_STRUCT_FIELD_GET_FIELD_ATTRIBUTES_ENUM( (int, FieldFlag::IsRequired, FieldMaxLength(250)) ) );
  QVERIFY(!fa.isNull());
  QVERIFY(fa.isRequired());
  QVERIFY(!fa.hasDefaultValue());
  QCOMPARE(fa.maxLength(), 250);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ReflectFieldTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
