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
#include "SelectQueryTest.h"
#include "Mdt/QueryExpression/SelectQuery.h"
#include "Mdt/QueryExpression/SelectEntity.h"

using namespace Mdt::QueryExpression;

/*
 * Tests
 */

void SelectQueryTest::selectEntityTest()
{
  SelectEntity person1( EntityName("Person1") );
  QCOMPARE(person1.name(), QString("Person1"));
  QVERIFY(person1.alias().isEmpty());
  QCOMPARE(person1.aliasOrName(), QString("Person1"));

  SelectEntity person2( EntityName("Person2"), "P2" );
  QCOMPARE(person2.name(), QString("Person2"));
  QCOMPARE(person2.alias(), QString("P2"));
  QCOMPARE(person2.aliasOrName(), QString("P2"));
}

void SelectQueryTest::simpleSetGetTest()
{
  SelectQuery query;

  query.setEntityName( EntityName("Person") );
  QCOMPARE(query.entityName(), QString("Person"));

  query.setEntityName(EntityName("A"));
  QCOMPARE(query.entityName(), QString("A"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SelectQueryTest test;

  return QTest::qExec(&test, argc, argv);
}
