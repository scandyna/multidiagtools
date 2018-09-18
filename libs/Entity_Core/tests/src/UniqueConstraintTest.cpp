/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "UniqueConstraintTest.h"
#include "Mdt/Entity/UniqueConstraint.h"
#include "Mdt/Entity/UniqueConstraintAlgorithm.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/TypeTraits/IsUniqueConstraint.h"
#include <QString>
#include <QStringList>

using namespace Mdt::Entity;

/*
 * Entities
 */

struct PersonDataStruct
{
  int id;
  QString firstName;
  QString lastName;
};

MDT_ENTITY_DEF(
  (PersonDataStruct),
  Person,
  (id, FieldFlag::IsPrimaryKey),
  (firstName),
  (lastName)
)

using PersonFirstNameUniqueConstraint = Mdt::Entity::UniqueConstraint<PersonEntity, PersonDef::firstNameField>;
using PersonNameUniqueConstraint = Mdt::Entity::UniqueConstraint<PersonEntity, PersonDef::firstNameField, PersonDef::lastNameField>;

/*
 * Compile time tests
 */

void UniqueConstraintTest::isUniqueConstraintTest()
{
  static_assert( TypeTraits::IsUniqueConstraint<PersonFirstNameUniqueConstraint>::value ,"" );
  static_assert( TypeTraits::IsUniqueConstraint<PersonNameUniqueConstraint>::value ,"" );
  static_assert( !TypeTraits::IsUniqueConstraint<int>::value ,"" );
}

/*
 * Tests
 */

struct ToStringList
{
  template<typename Field>
  void operator()(const Field &)
  {
    fieldNameList.append( Field::fieldName() );
  }

  QStringList fieldNameList;
};

void UniqueConstraintTest::forEachUniqueConstraintFieldTest()
{
  ToStringList f;

  forEachUniqueConstraintField<PersonFirstNameUniqueConstraint>(f);
  QCOMPARE(f.fieldNameList.count(), 1);
  QCOMPARE(f.fieldNameList.at(0), QString("firstName"));

  f.fieldNameList.clear();
  forEachUniqueConstraintField<PersonNameUniqueConstraint>(f);
  QCOMPARE(f.fieldNameList.count(), 2);
  QCOMPARE(f.fieldNameList.at(0), QString("firstName"));
  QCOMPARE(f.fieldNameList.at(1), QString("lastName"));
}

void UniqueConstraintTest::getUniqueConstraintFieldNameListTest()
{
  QStringList fieldNames;

  fieldNames = getUniqueConstraintFieldNameList<PersonFirstNameUniqueConstraint>();
  QCOMPARE(fieldNames.count(), 1);
  QCOMPARE(fieldNames.at(0), QString("firstName"));

  fieldNames = getUniqueConstraintFieldNameList<PersonNameUniqueConstraint>();
  QCOMPARE(fieldNames.count(), 2);
  QCOMPARE(fieldNames.at(0), QString("firstName"));
  QCOMPARE(fieldNames.at(1), QString("lastName"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  UniqueConstraintTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
