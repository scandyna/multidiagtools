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
#include "Mdt/Reflection/StructAlgorithm.h"
#include "Mdt/Reflection/FieldAlgorithm.h"
#include <QString>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/mpl/at.hpp>
#include <type_traits>

/*
 * Note: to check that the macro works properly,
 * we not declare using namespace Mdt::Reflection
 */

namespace FieldTupleSeqToMplVectorTest1{

  using field_list = boost::mpl::vector<MDT_REFLECTION_STRUCT_FIELD_TUPLE_ARG_LIST_TO_FIELD_ENUM( (int) )>;
  static_assert( std::is_same< boost::mpl::at_c<field_list, 0>::type, int >::value, "" );

} // namespace FieldTupleSeqToMplVectorTest1{

namespace FieldTupleSeqToMplVectorTest2{

  using field_list = boost::mpl::vector<MDT_REFLECTION_STRUCT_FIELD_TUPLE_ARG_LIST_TO_FIELD_ENUM( (int,bool) )>;
  static_assert( std::is_same< boost::mpl::at_c<field_list, 0>::type, int >::value, "" );

} // namespace FieldTupleSeqToMplVectorTest2{

namespace FieldTupleSeqToMplVectorTest3{

  using field_list = boost::mpl::vector<MDT_REFLECTION_STRUCT_FIELD_TUPLE_ARG_LIST_TO_FIELD_ENUM( (int,bool),(float) )>;
  static_assert( std::is_same< boost::mpl::at_c<field_list, 0>::type, int >::value, "" );
  static_assert( std::is_same< boost::mpl::at_c<field_list, 1>::type, float >::value, "" );

} // namespace FieldTupleSeqToMplVectorTest3{

/*
 * Reflect struct test
 */

struct PersonDataStruct
{
  qulonglong id;
  QString firstName;
  QString lastName;
};

MDT_REFLECT_STRUCT(
  (PersonDataStruct),
  Person,
  (id),
  (firstName, FieldFlag::IsRequired),
  (lastName, FieldFlag::IsRequired, FieldMaxLength(250))
)

static_assert( std::is_same<PersonDef::reflected_struct, PersonDataStruct>::value ,"" );
static_assert( std::is_same< PersonDef::field_list, boost::mpl::vector<PersonDef::id, PersonDef::firstName, PersonDef::lastName> >::value , "" );


namespace A{

  struct AddressDataStruct
  {
    int id;
    QString street;
    int personId;
  };

} // namespace A{

MDT_REFLECT_STRUCT(
  (A, AddressDataStruct),
  Address,
  (id),
  (street, FieldFlag::IsRequired),
  (personId)
)

static_assert( std::is_same<A::AddressDef::reflected_struct, A::AddressDataStruct>::value ,"" );
static_assert( std::is_same< A::AddressDef::field_list, boost::mpl::vector<A::AddressDef::id, A::AddressDef::street, A::AddressDef::personId> >::value , "" );


namespace A{ namespace B{

  struct SellerDataStruct
  {
    int id;
    QString name;
  };

}} // namespace A{ namespace B{

MDT_REFLECT_STRUCT(
  (A, B, SellerDataStruct),
  Seller,
  (id),
  (name, FieldFlag::IsRequired)
)

static_assert( std::is_same<A::B::SellerDef::reflected_struct, A::B::SellerDataStruct>::value ,"" );
static_assert( std::is_same< A::B::SellerDef::field_list, boost::mpl::vector<A::B::SellerDef::id, A::B::SellerDef::name> >::value , "" );


/*
 * Type traits tests
 */



// Macros for namespaceTest()

MDT_REFLECTION_STRUCT_NAMESPACE_BEGIN( (MyTestStruct) )
int testGlobalVar = 1;
MDT_REFLECTION_STRUCT_NAMESPACE_END( (MyTestStruct) )

MDT_REFLECTION_STRUCT_NAMESPACE_BEGIN( (TestNsA,MyTestStruct) )
int testVar = 2;
MDT_REFLECTION_STRUCT_NAMESPACE_END( (TestNsA,MyTestStruct) )

MDT_REFLECTION_STRUCT_NAMESPACE_BEGIN( (TestNsA,TestNsB,MyTestStruct) )
int testVar = 3;
MDT_REFLECTION_STRUCT_NAMESPACE_END( (TestNsA,TestNsB,MyTestStruct) )

void ReflectStructTest::namespaceTest()
{
  QCOMPARE(testGlobalVar, 1);
  QCOMPARE(TestNsA::testVar, 2);
  QCOMPARE(TestNsA::TestNsB::testVar, 3);

  int localVar = MDT_REFLECTION_STRUCT_NAMESPACE_REF( (fake) )testGlobalVar;
  QCOMPARE(localVar, 1);
  localVar = MDT_REFLECTION_STRUCT_NAMESPACE_REF( (TestNsA,fake) )testVar;
  QCOMPARE(localVar, 2);
  localVar = MDT_REFLECTION_STRUCT_NAMESPACE_REF( (TestNsA,TestNsB,fake) )testVar;
  QCOMPARE(localVar, 3);
}

void ReflectStructTest::usePersonTest()
{
  using namespace Mdt::Reflection;

  QString name;

  name = nameFromStructDef<PersonDef>();
  QCOMPARE(name, QString("Person"));

  name = nameFromField<PersonDef::id>();
  QCOMPARE(name, QString("Person"));

  name = fieldName<PersonDef::id>();
  QCOMPARE(name, QString("id"));
  QVERIFY(!isFieldRequired<PersonDef::id>());
  QVERIFY(!hasFieldDefaultValue<PersonDef::id>());
  QCOMPARE(fieldMaxLength<PersonDef::id>(), 0);

  name = fieldName<PersonDef::firstName>();
  QCOMPARE(name, QString("firstName"));
  QVERIFY(isFieldRequired<PersonDef::firstName>());
  QVERIFY(!hasFieldDefaultValue<PersonDef::firstName>());
  QCOMPARE(fieldMaxLength<PersonDef::firstName>(), 0);

  name = fieldName<PersonDef::lastName>();
  QCOMPARE(name, QString("lastName"));
  QVERIFY(isFieldRequired<PersonDef::lastName>());
  QVERIFY(!hasFieldDefaultValue<PersonDef::lastName>());
  QCOMPARE(fieldMaxLength<PersonDef::lastName>(), 250);
}

void ReflectStructTest::useAddressTest()
{
  using namespace Mdt::Reflection;
  using namespace A;

  QString name;

  name = nameFromStructDef<AddressDef>();
  QCOMPARE(name, QString("Address"));

  name = nameFromField<AddressDef::id>();
  QCOMPARE(name, QString("Address"));

  name = fieldName<AddressDef::id>();
  QCOMPARE(name, QString("id"));
  QVERIFY(!isFieldRequired<AddressDef::id>());
  QVERIFY(!hasFieldDefaultValue<AddressDef::id>());
  QCOMPARE(fieldMaxLength<AddressDef::id>(), 0);

  name = fieldName<AddressDef::street>();
  QCOMPARE(name, QString("street"));
  QVERIFY(isFieldRequired<AddressDef::street>());
  QVERIFY(!hasFieldDefaultValue<AddressDef::street>());
  QCOMPARE(fieldMaxLength<AddressDef::street>(), 0);

  name = fieldName<AddressDef::personId>();
  QCOMPARE(name, QString("personId"));
  QVERIFY(!isFieldRequired<AddressDef::personId>());
  QVERIFY(!hasFieldDefaultValue<AddressDef::personId>());
  QCOMPARE(fieldMaxLength<AddressDef::personId>(), 0);
}

void ReflectStructTest::useSellerTest()
{
  using namespace Mdt::Reflection;
  using namespace A::B;

  QString name;

  name = nameFromStructDef<SellerDef>();
  QCOMPARE(name, QString("Seller"));

  name = nameFromField<SellerDef::id>();
  QCOMPARE(name, QString("Seller"));

  name = fieldName<SellerDef::id>();
  QCOMPARE(name, QString("id"));
  QVERIFY(!isFieldRequired<SellerDef::id>());
  QVERIFY(!hasFieldDefaultValue<SellerDef::id>());
  QCOMPARE(fieldMaxLength<SellerDef::id>(), 0);

  name = fieldName<SellerDef::name>();
  QCOMPARE(name, QString("name"));
  QVERIFY(isFieldRequired<SellerDef::name>());
  QVERIFY(!hasFieldDefaultValue<SellerDef::name>());
  QCOMPARE(fieldMaxLength<SellerDef::name>(), 0);
}

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
