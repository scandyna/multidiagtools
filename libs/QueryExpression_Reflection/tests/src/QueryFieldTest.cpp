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
#include "QueryFieldTest.h"
#include "Mdt/QueryExpression/ReflectionQueryField.h"
// #include "Mdt/QueryExpression/Reflection/QueryEntity.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include <QLatin1String>

using namespace Mdt::QueryExpression;

/*
 * Entities
 */

struct PersonDataStruct
{
  qlonglong id;
  QString firstName;
};

MDT_REFLECT_STRUCT(
  (PersonDataStruct),
  Person,
  (id),
  (firstName)
)

/*
 * Tests
 */

// void QueryFieldTest::queryEntityTest()
// {
//   using namespace Mdt::QueryExpression;
//   using namespace Mdt::QueryExpression::Reflection;
// 
//   QueryEntity<PersonDef> person1;
//   QVERIFY(!person1.isNull());
//   QCOMPARE(person1.name(), QString("Person"));
//   QVERIFY(!person1.hasAlias());
// 
//   QFAIL("Not complete");
// }

void QueryFieldTest::constructTest()
{
  ReflectionQueryField<PersonDef::id> personId;
  const auto personIdEaF = personId.entityAndField();
  QCOMPARE(personIdEaF.entityName(), QLatin1String("Person"));
  QCOMPARE(personIdEaF.entityAliasOrName(), QLatin1String("Person"));
  QCOMPARE(personIdEaF.fieldName(), QLatin1String("id"));
  QCOMPARE(personIdEaF.fieldAliasOrName(), QLatin1String("id"));

  ReflectionQueryField<PersonDef::firstName> personName1(FieldAlias(QLatin1String("PersonName")));
  const auto personName1EaF = personName1.entityAndField();
  QCOMPARE(personName1EaF.entityName(), QLatin1String("Person"));
  QCOMPARE(personName1EaF.entityAliasOrName(), QLatin1String("Person"));
  QCOMPARE(personName1EaF.fieldName(), QLatin1String("firstName"));
  QCOMPARE(personName1EaF.fieldAliasOrName(), QLatin1String("PersonName"));

  ReflectionQueryField<PersonDef::firstName> personName2( EntityAlias(QLatin1String("P")), FieldAlias(QLatin1String("PersonName")));
  const auto personName2EaF = personName2.entityAndField();
  QCOMPARE(personName2EaF.entityName(), QLatin1String("Person"));
  QCOMPARE(personName2EaF.entityAliasOrName(), QLatin1String("P"));
  QCOMPARE(personName2EaF.fieldName(), QLatin1String("firstName"));
  QCOMPARE(personName2EaF.fieldAliasOrName(), QLatin1String("PersonName"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  QueryFieldTest test;

  return QTest::qExec(&test, argc, argv);
}
