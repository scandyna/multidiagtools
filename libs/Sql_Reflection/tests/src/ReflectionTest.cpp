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
#include "ReflectionTest.h"
#include "Mdt/Sql/Schema/Reflection.h"
#include "Mdt/Reflection/PrimaryKey.h"

#include "Mdt/Reflection/FieldAttributes.h"

#include <boost/fusion/adapted/struct/adapt_assoc_struct.hpp>
#include <boost/fusion/include/adapt_assoc_struct.hpp>

#include <boost/fusion/adapted/adt/adapt_assoc_adt.hpp>
#include <boost/fusion/include/adapt_assoc_adt.hpp>

#include <boost/fusion/include/for_each.hpp>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/for_each.hpp>

#include <QDebug>

using namespace Mdt::Sql::Schema;

void ReflectionTest::initTestCase()
{
}

void ReflectionTest::cleanupTestCase()
{
}

/*
 * Person
 */

struct PersonDataStruct
{
  int id = 0;
  QString firstName;
  QString lastName;
};

/*
 * Person - generated part
 */

struct PersonDef
{
  using DataStruct = PersonDataStruct;

  static constexpr const char *name()
  {
    return "Person";
  }

  struct id
  {
    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes();
    }
  };

  struct firstName
  {
    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes();
    }
  };

  struct lastName
  {
    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes();
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
 * Tests
 */

void ReflectionTest::personTableTest()
{
  auto table = tableFromReflected<PersonDef>(PersonDataStruct());

  QFAIL("Not complete");
}

void ReflectionTest::tableNoPkTest()
{
  auto table = tableFromReflected<PersonDef>(PersonDataStruct());

  QFAIL("Not complete");
}

void ReflectionTest::tableAutoIncrementPkTest()
{
  QFAIL("Not complete");
}

void ReflectionTest::tablePkTest()
{
  using PersonPrimaryKey = Mdt::Reflection::PrimaryKey<PersonDef, PersonDef::id>;

  auto table = tableFromReflected<PersonDef, PersonPrimaryKey>(PersonDataStruct());

  QFAIL("Not complete");
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ReflectionTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
