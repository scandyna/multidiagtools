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
#include "ReflectStructTest.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include <QString>
#include <boost/mpl/at.hpp>
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


/*
 * Address struct
 */

struct AddressDataStruct
{
  int id;
  QString street;
  int personId;
};

/**
 * Sandbox
 */

/// \todo Go to ReflectFieldTest
namespace FieldGetFieldTest{

  using int1 = MDT_REFLECTION_STRUCT_FIELD_GET_FIELD( (int) );
  static_assert( std::is_same<int1, int>::value, "" );

  using int2 = MDT_REFLECTION_STRUCT_FIELD_GET_FIELD( (int, FieldFlag::IsRequired) );
  static_assert( std::is_same<int2, int>::value, "" );

} // namespace FieldGetFieldTest{

using ReflectedStruct =
  MDT_REFLECTION_STRUCT_GET_STRUCT( \
    (PersonDataStruct)              \
    (Person)                        \
    (id)                            \
  );
static_assert( std::is_same<ReflectedStruct, PersonDataStruct>::value, "" );


namespace GenFieldListTest1{

  MDT_REFLECTION_STRUCT_GEN_FIELD_LIST( \
    (int)                               \
  )
  static_assert( std::is_same< boost::mpl::at_c<field_list, 0>::type, int >::value, "" );

} // namespace FieldListTest1{

namespace GenFieldListTest2{

  MDT_REFLECTION_STRUCT_GEN_FIELD_LIST( \
    (int,bool)                          \
  )
  static_assert( std::is_same< boost::mpl::at_c<field_list, 0>::type, int >::value, "" );

} // namespace FieldListTest2{

namespace GenFieldListTest3{

  MDT_REFLECTION_STRUCT_GEN_FIELD_LIST( \
    (int,bool)                          \
    (float)                             \
  )
  static_assert( std::is_same< boost::mpl::at_c<field_list, 0>::type, int >::value, "" );
  static_assert( std::is_same< boost::mpl::at_c<field_list, 1>::type, float >::value, "" );

} // namespace FieldListTest3{

/*
 * Helpers
 */


/*
 * Type traits tests
 */

/*
 * Tests
 */



/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ReflectStructTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
