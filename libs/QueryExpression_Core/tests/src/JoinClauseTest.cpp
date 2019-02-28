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
#include "JoinClauseTest.h"
#include "Mdt/QueryExpression/JoinClause.h"
#include "Mdt/QueryExpression/JoinClauseList.h"
#include "Mdt/QueryExpression/JoinConstraintExpression.h"
#include "Mdt/QueryExpression/QueryEntity.h"
#include "Mdt/QueryExpression/QueryField.h"

#include "Mdt/QueryExpression/SelectEntity.h"
#include "Mdt/QueryExpression/SelectField.h"
#include "Mdt/QueryExpression/EntityName.h"
#include "Mdt/QueryExpression/FieldName.h"

using namespace Mdt::QueryExpression;

/*
 * Tests
 */

void JoinClauseTest::joinClauseTest()
{
//   SelectEntity person( EntityName("Person") );
//   SelectEntity address( EntityName("Address"), "ADR");
// 
//   SelectField personId( person, FieldName("id") );
//   SelectField addressPersonId( address, FieldName("personId") );

  QueryEntity person("Person");
  QueryEntity address("Address", EntityAlias("ADR"));

  QueryField personId(person, "id");
  QueryField addressPersonId(address, "personId");

  JoinConstraintExpression joinExpression;

  joinExpression.setJoin( addressPersonId == personId );
  JoinClause join(JoinOperator::Join, address, joinExpression);
  QCOMPARE(join.joinOperator(), JoinOperator::Join);
  QCOMPARE(join.entityAliasOrName(), QString("ADR"));
  QVERIFY(!join.joinConstraintExpression().isNull());
}

void JoinClauseTest::joinClauseListTest()
{
  QueryEntity person("Person");
  QueryEntity address("Address", EntityAlias("ADR"));

  QueryField personId(person, "id");
  QueryField addressPersonId(address, "personId");

  JoinConstraintExpression joinExpression;

  JoinClauseList list;
  QCOMPARE(list.clauseCount(), 0);
  QVERIFY(list.isEmpty());

  joinExpression.setJoin( addressPersonId == personId );
  list.addClause(JoinOperator::Join, address, joinExpression);
  QCOMPARE(list.clauseCount(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.clauseAt(0).joinOperator(), JoinOperator::Join);
  QCOMPARE(list.clauseAt(0).entityAliasOrName(), QString("ADR"));
  QVERIFY(!list.clauseAt(0).joinConstraintExpression().isNull());

  list.clear();
  QVERIFY(list.isEmpty());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  JoinClauseTest test;

  return QTest::qExec(&test, argc, argv);
}
