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
#include "FieldTest.h"
#include "Mdt/Reflection/FieldAlgorithm.h"
#include <QString>

#include "Mdt/Reflection/FieldAttributes.h"
#include <boost/fusion/adapted/struct/adapt_assoc_struct.hpp>
#include <boost/fusion/include/adapt_assoc_struct.hpp>

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
 * Person struct - auto generated stuff
 */

struct PersonDef
{
  using reflected_struct = PersonDataStruct;

  static constexpr const char *name()
  {
    return "Person";
  }

  struct id
  {
    using struct_def = PersonDef;

    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes();
    }
  };

  struct firstName
  {
    using struct_def = PersonDef;

    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes(FieldFlag::IsRequired);
    }
  };

  struct lastName
  {
    using struct_def = PersonDef;

    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes(FieldFlag::HasDefaultValue);
    }
  };

};

BOOST_FUSION_ADAPT_ASSOC_STRUCT(
  PersonDataStruct,
  (id, PersonDef::id)
  (firstName, PersonDef::firstName)
  (lastName, PersonDef::lastName)
)

/*
 * Address struct
 */

struct AddressDataStruct
{
  int id;
  QString street;
  int personId;
};

/*
 * Address struct - auto generated stuff
 */

struct AddressDef
{
  using reflected_struct = AddressDataStruct;

  static constexpr const char *name()
  {
    return "Address";
  }

  struct id
  {
    using struct_def = AddressDef;

    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes();
    }
  };

  struct street
  {
    using struct_def = AddressDef;

    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes(FieldMaxLength(25),FieldFlag::IsRequired, FieldFlag::HasDefaultValue);
    }
  };

  struct personId
  {
    using struct_def = AddressDef;

    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes(FieldFlag::IsRequired);
    }
  };

};

BOOST_FUSION_ADAPT_ASSOC_STRUCT(
  AddressDataStruct,
  (id, AddressDef::id)
  (street, AddressDef::street)
  (personId, AddressDef::personId)
)

/*
 * Helpers
 */


/*
 * Type traits tests
 */

/*
 * Tests
 */

void FieldTest::fieldNameByIndexInStructTest()
{
  QString fieldName;

  fieldName = fieldNameAtInStruct<PersonDataStruct, 0>();
  QCOMPARE(fieldName, QString("id"));
  fieldName = fieldNameAtInStruct<PersonDataStruct, 1>();
  QCOMPARE(fieldName, QString("firstName"));
  fieldName = fieldNameAtInStruct<PersonDataStruct, 2>();
  QCOMPARE(fieldName, QString("lastName"));

  fieldName = fieldNameAtInStruct<AddressDataStruct, 0>();
  QCOMPARE(fieldName, QString("id"));
  fieldName = fieldNameAtInStruct<AddressDataStruct, 1>();
  QCOMPARE(fieldName, QString("street"));
  fieldName = fieldNameAtInStruct<AddressDataStruct, 2>();
  QCOMPARE(fieldName, QString("personId"));
}

void FieldTest::fieldNameFromFieldTest()
{
  QString fName;

  fName = fieldName<PersonDef::id>();
  QCOMPARE(fName, QString("id"));
  fName = fieldName<PersonDef::firstName>();
  QCOMPARE(fName, QString("firstName"));
  fName = fieldName<PersonDef::lastName>();
  QCOMPARE(fName, QString("lastName"));

  fName = fieldName<AddressDef::id>();
  QCOMPARE(fName, QString("id"));
  fName = fieldName<AddressDef::street>();
  QCOMPARE(fName, QString("street"));
  fName = fieldName<AddressDef::personId>();
  QCOMPARE(fName, QString("personId"));
}

void FieldTest::fieldAttributesTest()
{
  bool isRequired;
  bool hasDefaultValue;
  int maxLength;

  isRequired = isFieldRequired<PersonDef::id>();
  QVERIFY(!isRequired);
  hasDefaultValue = hasFieldDefaultValue<PersonDef::id>();
  QVERIFY(!hasDefaultValue);
  maxLength = fieldMaxLength<PersonDef::id>();
  QCOMPARE(maxLength, 0);

  isRequired = isFieldRequired<PersonDef::firstName>();
  QVERIFY(isRequired);
  hasDefaultValue = hasFieldDefaultValue<PersonDef::firstName>();
  QVERIFY(!hasDefaultValue);
  maxLength = fieldMaxLength<PersonDef::firstName>();
  QCOMPARE(maxLength, 0);

  isRequired = isFieldRequired<PersonDef::lastName>();
  QVERIFY(!isRequired);
  hasDefaultValue = hasFieldDefaultValue<PersonDef::lastName>();
  QVERIFY(hasDefaultValue);
  maxLength = fieldMaxLength<PersonDef::lastName>();
  QCOMPARE(maxLength, 0);

  isRequired = isFieldRequired<AddressDef::id>();
  QVERIFY(!isRequired);
  hasDefaultValue = hasFieldDefaultValue<AddressDef::id>();
  QVERIFY(!hasDefaultValue);
  maxLength = fieldMaxLength<AddressDef::id>();
  QCOMPARE(maxLength, 0);

  isRequired = isFieldRequired<AddressDef::street>();
  QVERIFY(isRequired);
  hasDefaultValue = hasFieldDefaultValue<AddressDef::street>();
  QVERIFY(hasDefaultValue);
  maxLength = fieldMaxLength<AddressDef::street>();
  QCOMPARE(maxLength, 25);

  isRequired = isFieldRequired<AddressDef::personId>();
  QVERIFY(isRequired);
  hasDefaultValue = hasFieldDefaultValue<AddressDef::personId>();
  QVERIFY(!hasDefaultValue);
  maxLength = fieldMaxLength<AddressDef::personId>();
  QCOMPARE(maxLength, 0);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  FieldTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
