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
#include "Mdt/Reflection/ReflectStruct.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>
#include <QVariant>

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

MDT_REFLECT_STRUCT(
  (PersonDataStruct),
  Person,
  (id),
  (firstName, FieldFlag::IsRequired),
  (lastName, FieldFlag::HasDefaultValue)
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

MDT_REFLECT_STRUCT(
  (AddressDataStruct),
  Address,
  (id),
  (street, FieldMaxLength(25),FieldFlag::IsRequired, FieldFlag::HasDefaultValue),
  (personId, FieldFlag::IsRequired)
)

/*
 * Tests
 */

void StructTest::nameTest()
{
  QCOMPARE(nameFromStructDef<PersonDef>(), "Person");

  QCOMPARE(nameFromStructDef<AddressDef>(), "Address");
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

    const QString name = QLatin1String(nameFromField<Field>()) + QLatin1Char('.') + QLatin1String(fieldName<Field>())\
                       + QLatin1Char(':') + QVariant(p.second).toString();
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
  PersonDataStruct person{1,QLatin1String("fA"),QLatin1String("lA")};
  forEachFieldValuePairInStruct(person, f);
  QCOMPARE(fieldNameList, QStringList({QLatin1String("Person.id:1"),QLatin1String("Person.firstName:fA"),QLatin1String("Person.lastName:lA")}));

  fieldNameList.clear();
  AddressDataStruct address{10,QLatin1String("sA"),1};
  forEachFieldValuePairInStruct(address, f);
  QCOMPARE(fieldNameList, QStringList({QLatin1String("Address.id:10"),QLatin1String("Address.street:sA"),QLatin1String("Address.personId:1")}));
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
    const QString name = QLatin1String(nameFromField<Field>()) + QLatin1Char('.') + QLatin1String(fieldName<Field>());

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
  QCOMPARE(fieldNameList, QStringList({QLatin1String("Person.id"),QLatin1String("Person.firstName"),QLatin1String("Person.lastName")}));

  fieldNameList.clear();
  forEachFieldInStructDef<AddressDef>(f);
  QCOMPARE(fieldNameList, QStringList({QLatin1String("Address.id"),QLatin1String("Address.street"),QLatin1String("Address.personId")}));
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
