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
#include "StructTest.h"
#include "Mdt/Reflection/StructAlgorithm.h"
#include "Mdt/Reflection/FieldAlgorithm.h"
#include <QString>
#include <QStringList>
#include <QVariant>

#include "Mdt/Reflection/FieldAttributes.h"
#include <boost/fusion/adapted/struct/adapt_assoc_struct.hpp>
#include <boost/fusion/include/adapt_assoc_struct.hpp>
#include <boost/mpl/vector.hpp>

using namespace Mdt::Reflection;

/*
 * Person struct
 */

struct PersonDataStruct
{
  PersonDataStruct() = default;
  // Make shure the algorithms do not copy this struct
  PersonDataStruct(const PersonDataStruct &) = delete;
  PersonDataStruct & operator=(const PersonDataStruct &) = delete;
  PersonDataStruct(PersonDataStruct &&) = delete;
  PersonDataStruct & operator=(PersonDataStruct &&) = delete;

  int id = 0;
  QString firstName;
  QString lastName;
};

/*
 * Person struct - auto generated stuff
 */

struct PersonDef
{
  using reflected_struct = PersonDataStruct;

  static constexpr const char *name_()
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

  using field_list = boost::mpl::vector<id, firstName, lastName>;
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

  static constexpr const char *name_()
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

  using field_list = boost::mpl::vector<id, street, personId>;
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

void StructTest::nameTest()
{
  QString name;

  name = nameFromStructDef<PersonDef>();
  QCOMPARE(name, QString("Person"));

  name = nameFromStructDef<AddressDef>();
  QCOMPARE(name, QString("Address"));
}

struct AddNameAndFieldNameToList
{
  AddNameAndFieldNameToList(QStringList & list)
   : mList(list)
  {
  }

  template<typename FieldValuePair>
  void operator()(const FieldValuePair & p) const
  {
    using Field = typename FieldValuePair::first_type;

    const QString name = QString(nameFromField<Field>()) + QString('.') + QString(fieldName<Field>())\
                       + QString(':') + QVariant(p.second).toString();
    mList << name;
  }

 private:

  QStringList & mList;
};

void StructTest::forEachFieldValuePairTest()
{
  QStringList fieldNameList;
  AddNameAndFieldNameToList f(fieldNameList);

  fieldNameList.clear();
  PersonDataStruct person{1,"fA","lA"};
  forEachFieldValuePairInStruct(person, f);
  QCOMPARE(fieldNameList, QStringList({"Person.id:1","Person.firstName:fA","Person.lastName:lA"}));

  fieldNameList.clear();
  AddressDataStruct address{10,"sA",1};
  forEachFieldValuePairInStruct(address, f);
  QCOMPARE(fieldNameList, QStringList({"Address.id:10","Address.street:sA","Address.personId:1"}));
}

struct AddFieldNameToList
{
  AddFieldNameToList(QStringList & list)
   : mFieldNameList(list)
  {
  }

  template<typename Field>
  void operator()(Field)
  {
    const QString name = QString(nameFromField<Field>()) + QString('.') + QString(fieldName<Field>());

    mFieldNameList << name;
  }

private:

  QStringList & mFieldNameList;
};

void StructTest::forEachFieldTest()
{
  QStringList fieldNameList;
  AddFieldNameToList f(fieldNameList);

  fieldNameList.clear();
  forEachFieldInStructDef<PersonDef>(f);
  QCOMPARE(fieldNameList, QStringList({"Person.id","Person.firstName","Person.lastName"}));

  fieldNameList.clear();
  forEachFieldInStructDef<AddressDef>(f);
  QCOMPARE(fieldNameList, QStringList({"Address.id","Address.street","Address.personId"}));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  StructTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
