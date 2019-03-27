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
#include "PrimaryKeyAlgorithmTest.h"
#include "Mdt/QueryExpression/ReflectionPrimaryKeyRecordAlgorithm.h"
#include "Mdt/QueryExpression/ReflectionSelectStatement.h"
#include "Mdt/QueryExpression/Debug/ExpressionTreeToString.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include "Mdt/Reflection/PrimaryKey.h"
#include "Mdt/Reflection/PrimaryKeyRecord.h"
#include <QLatin1String>

using namespace Mdt::QueryExpression;
using namespace Mdt::QueryExpression::Debug;

/*
 * Entities
 */

struct PersonDataStruct
{
  qlonglong id = 0;
  QString firstName;
};

MDT_REFLECT_STRUCT(
  (PersonDataStruct),
  Person,
  (id),
  (firstName)
)

using PersonPrimaryKey = Mdt::Reflection::PrimaryKey<PersonDef::id>;


struct CableLinkDataStruct
{
  qlonglong startConnectionId = 0;
  qlonglong endConnectionId = 0;
  QString identification;
};

MDT_REFLECT_STRUCT(
  (CableLinkDataStruct),
  CableLink,
  (startConnectionId),
  (endConnectionId),
  (identification)
)

using CableLinkPrimaryKey = Mdt::Reflection::PrimaryKey<CableLinkDef::startConnectionId, CableLinkDef::endConnectionId>;
using CableLinkIdRecord = Mdt::Reflection::PrimaryKeyRecord<CableLinkPrimaryKey>;

/*
 * Tests
 */

void PrimaryKeyAlgorithmTest::filterExpressionFromIdPrimaryKeyValueTest()
{
  FilterExpression filter;
  QString expectedStr;

  filter = filterExpressionFromIdPrimaryKeyValue<PersonPrimaryKey>(25);
  expectedStr = QLatin1String("Person.id==25");
  QCOMPARE(filterExpressionToInfixString(filter), expectedStr);
}

void PrimaryKeyAlgorithmTest::filterExpressionFromPrimaryKeyRecordTest()
{
  FilterExpression filter;
  QString expectedStr;

  CableLinkIdRecord linkIdRecord;
  linkIdRecord.setValue<CableLinkDef::startConnectionId>(12);
  linkIdRecord.setValue<CableLinkDef::endConnectionId>(30);

  filter = filterExpressionFromPrimaryKeyRecord(linkIdRecord);
  expectedStr = QLatin1String("(CableLink.startConnectionId==12)&&(CableLink.endConnectionId==30)");
  QCOMPARE(filterExpressionToInfixString(filter), expectedStr);

  using OneFieldPk = Mdt::Reflection::PrimaryKey<CableLinkDef::endConnectionId>;
  using OneFieldPkr = Mdt::Reflection::PrimaryKeyRecord<OneFieldPk>;

  OneFieldPkr pkr1;
  pkr1.setValue<CableLinkDef::endConnectionId>(12);
  filter = filterExpressionFromPrimaryKeyRecord(pkr1);
  expectedStr = QLatin1String("CableLink.endConnectionId==12");
  QCOMPARE(filterExpressionToInfixString(filter), expectedStr);

  using ThreFieldPk = Mdt::Reflection::PrimaryKey<CableLinkDef::identification, CableLinkDef::startConnectionId, CableLinkDef::endConnectionId>;
  using ThreFieldPkr = Mdt::Reflection::PrimaryKeyRecord<ThreFieldPk>;

  ThreFieldPkr pkr3;
  pkr3.setValue<CableLinkDef::identification>(QString::fromLatin1("I"));
  pkr3.setValue<CableLinkDef::startConnectionId>(25);
  pkr3.setValue<CableLinkDef::endConnectionId>(36);

  filter = filterExpressionFromPrimaryKeyRecord(pkr3);
  expectedStr = QLatin1String("((CableLink.identification==I)&&(CableLink.startConnectionId==25))&&(CableLink.endConnectionId==36)");
  QCOMPARE(filterExpressionToInfixString(filter), expectedStr);
}

/*
void PrimaryKeyAlgorithmTest::filterExpressionFromStructPrimaryKeyTest()
{
  FilterExpression filter;
  QString expectedStr;

  PersonDataStruct person;
  person.id = 25;
  filter = filterExpressionFromStructPrimaryKey<PersonPrimaryKey>(person);
  expectedStr = QLatin1String("Person.id==25");
  QCOMPARE(filterExpressionToInfixString(filter), expectedStr);

  CableLinkDataStruct link;
  link.startConnectionId = 12;
  link.endConnectionId = 30;
  filter = filterExpressionFromStructPrimaryKey<CableLinkPrimaryKey>(link);
  expectedStr = QLatin1String("(Link.startConnectionId==12)&&(Link.endConnectionId==30)");
  QCOMPARE(filterExpressionToInfixString(filter), expectedStr);

  QFAIL("Not complete");
}
*/

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  PrimaryKeyAlgorithmTest test;

  return QTest::qExec(&test, argc, argv);
}
