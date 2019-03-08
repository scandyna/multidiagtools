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
#include "QueryExpressionSqlTransformTest.h"
#include "Mdt/Sql/QueryExpressionSqlTransform.h"
#include "Mdt/Sql/QueryEntity.h"
#include "Mdt/Sql/EntityAlias.h"
#include "Mdt/Sql/FieldAlias.h"
#include "Mdt/Sql/SelectField.h"
#include "Mdt/Sql/SelectFieldList.h"
#include <QLatin1String>

using namespace Mdt::Sql;

void QueryExpressionSqlTransformTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
}

void QueryExpressionSqlTransformTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void QueryExpressionSqlTransformTest::queryFieldToSqlTest()
{
  QString expectedSql;
  const auto db = database();
  QVERIFY(db.isValid());

  QueryEntity person("Person");
  QueryEntity address("Address", EntityAlias("ADR"));

  expectedSql = "\"age\"";
  QCOMPARE(queryFieldToSql(QueryField("age"), db), expectedSql);

  expectedSql = "\"age\"";
  QCOMPARE(queryFieldToSql(QueryField("age", FieldAlias("A")), db), expectedSql);

  expectedSql = "\"Person\".\"age\"";
  QCOMPARE(queryFieldToSql(QueryField(person, "age"), db), expectedSql);

  expectedSql = "\"Person\".\"age\"";
  QCOMPARE(queryFieldToSql(QueryField(person, "age", FieldAlias("PersonAge")), db), expectedSql);

  expectedSql = "\"ADR\".\"street\"";
  QCOMPARE(queryFieldToSql(QueryField(address, "street"), db), expectedSql);

  expectedSql = "\"ADR\".\"street\"";
  QCOMPARE(queryFieldToSql(QueryField(address, "street", FieldAlias("AddressStreet")), db), expectedSql);
}

void QueryExpressionSqlTransformTest::selectFieldToSqlTest()
{
  using Mdt::QueryExpression::SelectField;
  using Mdt::QueryExpression::SelectAllField;

  QString expectedSql;
  const auto db = database();
  QVERIFY(db.isValid());

  QueryEntity person("Person");
  QueryEntity address("Address", EntityAlias("ADR"));

  expectedSql = "*";
  QCOMPARE(selectFieldToSql(SelectAllField(), db), expectedSql);

  expectedSql = "\"Person\".*";
  QCOMPARE(selectFieldToSql(SelectAllField(person), db), expectedSql);

  expectedSql = "\"ADR\".*";
  QCOMPARE(selectFieldToSql(SelectAllField(address), db), expectedSql);

  expectedSql = "\"age\"";
  QCOMPARE(selectFieldToSql(SelectField("age"), db), expectedSql);

  expectedSql = "\"age\" AS \"A\"";
  QCOMPARE(selectFieldToSql(SelectField("age", FieldAlias("A")), db), expectedSql);

  expectedSql = "\"Person\".\"age\"";
  QCOMPARE(selectFieldToSql(SelectField(person, "age"), db), expectedSql);

  expectedSql = "\"Person\".\"age\" AS \"PersonAge\"";
  QCOMPARE(selectFieldToSql(SelectField(person, "age", FieldAlias("PersonAge")), db), expectedSql);

  expectedSql = "\"ADR\".\"street\"";
  QCOMPARE(selectFieldToSql(SelectField(address, "street"), db), expectedSql);

  expectedSql = "\"ADR\".\"street\" AS \"AddressStreet\"";
  QCOMPARE(selectFieldToSql(SelectField(address, "street", FieldAlias("AddressStreet")), db), expectedSql);
}

void QueryExpressionSqlTransformTest::selectFieldListToSqlTest()
{
  using Mdt::QueryExpression::SelectFieldList;

  QString expectedSql;
  const auto db = database();

  SelectFieldList fieldList;

  fieldList.addField("id");
  expectedSql = " \"id\"";
  QCOMPARE(selectFieldListToSql(fieldList, db), expectedSql);

  fieldList.addField("age", FieldAlias("A"));
  expectedSql = " \"id\",\n"\
                " \"age\" AS \"A\"";
  QCOMPARE(selectFieldListToSql(fieldList, db), expectedSql);

  fieldList.addField("name");
  expectedSql = " \"id\",\n"\
                " \"age\" AS \"A\",\n"\
                " \"name\"";
  QCOMPARE(selectFieldListToSql(fieldList, db), expectedSql);
}

void QueryExpressionSqlTransformTest::operatorToSqlTest()
{
  using Mdt::QueryExpression::ComparisonOperator;
  using Mdt::QueryExpression::LogicalOperator;

  QCOMPARE(operatorToSql(ComparisonOperator::Equal), QString("="));
  QCOMPARE(operatorToSql(ComparisonOperator::NotEqual), QString("<>"));
  QCOMPARE(operatorToSql(ComparisonOperator::Less), QString("<"));
  QCOMPARE(operatorToSql(ComparisonOperator::LessEqual), QString("<="));
  QCOMPARE(operatorToSql(ComparisonOperator::Greater), QString(">"));
  QCOMPARE(operatorToSql(ComparisonOperator::GreaterEqual), QString(">="));
  QCOMPARE(operatorToSql(LogicalOperator::And), QString("AND"));
  QCOMPARE(operatorToSql(LogicalOperator::Or), QString("OR"));
}

void QueryExpressionSqlTransformTest::likeExpressionToSqlTest()
{
  QFETCH(QString, likeExpressionString);
  QFETCH(QString, expectedSql);
  const auto db = database();

  Mdt::QueryExpression::LikeExpressionData likeExpression(likeExpressionString);
  QCOMPARE(likeExpressionToSql(likeExpression, db), expectedSql);
}

void QueryExpressionSqlTransformTest::likeExpressionToSqlTest_data()
{
  QTest::addColumn<QString>("likeExpressionString");
  QTest::addColumn<QString>("expectedSql");

  QTest::newRow("A") << "A" << " LIKE 'A' ESCAPE '\\'";
  QTest::newRow("?A") << "?A" << " LIKE '_A' ESCAPE '\\'";
  QTest::newRow("A?") << "A?" << " LIKE 'A_' ESCAPE '\\'";
  QTest::newRow("\\?A") << "\\?A" << " LIKE '?A' ESCAPE '\\'";
  QTest::newRow("_A") << "_A" << " LIKE '\\_A' ESCAPE '\\'";
  QTest::newRow("%A") << "%A" << " LIKE '\\%A' ESCAPE '\\'";
  QTest::newRow("?A?") << "?A?" << " LIKE '_A_' ESCAPE '\\'";
  QTest::newRow("*A") << "*A" << " LIKE '%A' ESCAPE '\\'";
  QTest::newRow("A*") << "A*" << " LIKE 'A%' ESCAPE '\\'";
  QTest::newRow("*A*") << "*A*" << " LIKE '%A%' ESCAPE '\\'";
  QTest::newRow("?A*") << "?A*" << " LIKE '_A%' ESCAPE '\\'";
  QTest::newRow("AB") << "AB" << " LIKE 'AB' ESCAPE '\\'";
  QTest::newRow("A'B") << "A'B" << " LIKE 'A''B' ESCAPE '\\'";
  QTest::newRow("?AB") << "?AB" << " LIKE '_AB' ESCAPE '\\'";
  QTest::newRow("A?B") << "A?B" << " LIKE 'A_B' ESCAPE '\\'";
  QTest::newRow("AB?") << "AB?" << " LIKE 'AB_' ESCAPE '\\'";
  QTest::newRow("*AB") << "*AB" << " LIKE '%AB' ESCAPE '\\'";
  QTest::newRow("A*B") << "A*B" << " LIKE 'A%B' ESCAPE '\\'";
  QTest::newRow("AB*") << "AB*" << " LIKE 'AB%' ESCAPE '\\'";
  QTest::newRow("A\\?B") << "A\\?B" << " LIKE 'A?B' ESCAPE '\\'";
  QTest::newRow("A_B") << "A_B" << " LIKE 'A\\_B' ESCAPE '\\'";
  QTest::newRow("A%B") << "A%B" << " LIKE 'A\\%B' ESCAPE '\\'";
}

void QueryExpressionSqlTransformTest::filterExpressionToSqlTest()
{
  using Like = Mdt::QueryExpression::LikeExpression;

  QString expectedSql;
  const auto db = database();
  Mdt::QueryExpression::FilterExpression filter;

  QueryEntity person("Person");
  QueryEntity address("Address", EntityAlias("ADR"));

  QueryField id("id");
  QueryField personId(person, "id", FieldAlias("personId"));
  QueryField personName(person, "name");
  QueryField addressPersonId(address, "personId");

  filter.setFilter(id == 25);
  expectedSql = "\"id\"=25";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);

  filter.setFilter(personId == 105);
  expectedSql = "\"Person\".\"id\"=105";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);

  filter.setFilter(personName == "ABCD");
  expectedSql = "\"Person\".\"name\"='ABCD'";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);

  filter.setFilter(personName == "A'BCD");
  expectedSql = "\"Person\".\"name\"='A''BCD'";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);

  QString matchingName = "DEF";
  filter.setFilter(personName == matchingName);
  expectedSql = "\"Person\".\"name\"='DEF'";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);

  filter.setFilter(personName == Like("?B*"));
  expectedSql = "\"Person\".\"name\" LIKE '_B%' ESCAPE '\\'";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);

  filter.setFilter( (id > 30) && (personId > 40) );
  expectedSql = "(\"id\">30)AND(\"Person\".\"id\">40)";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);

  filter.setFilter( (id <= 30) || (personId >= 40) );
  expectedSql = "(\"id\"<=30)OR(\"Person\".\"id\">=40)";
  QCOMPARE(filterExpressionToSql(filter, db), expectedSql);
}

void QueryExpressionSqlTransformTest::joinOperatorToSqlTest()
{
  using Mdt::QueryExpression::JoinOperator;

  QCOMPARE(joinOperatorToSql(JoinOperator::Join), QString("JOIN"));
  QCOMPARE(joinOperatorToSql(JoinOperator::LeftJoin), QString("LEFT JOIN"));
}

void QueryExpressionSqlTransformTest::joinConstraintExpressionToSqlTest()
{
  QString expectedSql;
  const auto db = database();
  Mdt::QueryExpression::JoinConstraintExpression join;

  QueryEntity person("Person");
  QueryEntity address("Address", EntityAlias("ADR"));

  QueryField personId(person, "id", FieldAlias("personId"));
  QueryField addressPersonId(address, "personId");

  join.setJoin(addressPersonId == personId);
  expectedSql = "\"ADR\".\"personId\"=\"Person\".\"id\"";
  QCOMPARE(joinConstraintExpressionToSql(join, db), expectedSql);
}

void QueryExpressionSqlTransformTest::joinClauseToSqlTest()
{
  using Mdt::QueryExpression::JoinConstraintExpression;
  using Mdt::QueryExpression::JoinClause;
  using Mdt::QueryExpression::JoinOperator;

  QString expectedSql;
  const auto db = database();
  JoinConstraintExpression constraintExpression;

  QueryEntity person("Person");
  QueryEntity address("Address", EntityAlias("ADR"));

  QueryField personId(person, "id", FieldAlias("personId"));
  QueryField addressPersonId(address, "personId");

  constraintExpression.setJoin(addressPersonId == personId);
  JoinClause join1(JoinOperator::Join, address, constraintExpression);
  expectedSql = "JOIN\n"\
                " \"Address\" \"ADR\"\n"\
                "  ON \"ADR\".\"personId\"=\"Person\".\"id\"";
  QCOMPARE(joinClauseToSql(join1, db), expectedSql);
}

void QueryExpressionSqlTransformTest::joinClauseListToSqlTest()
{
  using Mdt::QueryExpression::JoinConstraintExpression;
  using Mdt::QueryExpression::JoinClause;
  using Mdt::QueryExpression::JoinClauseList;
  using Mdt::QueryExpression::JoinOperator;

  QString expectedSql;
  const auto db = database();
  JoinConstraintExpression constraintExpression;

  QueryEntity person("Person");
  QueryEntity address("Address", EntityAlias("ADR"));
  QueryEntity land("Land");

  QueryField personId(person, "id", FieldAlias("personId"));
  QueryField addressPersonId(address, "personId");
  QueryField landId(land, "id");
  QueryField addressLandId(address, "landId");

  JoinClauseList joinList;
  constraintExpression.setJoin(addressPersonId == personId);
  joinList.addClause(JoinOperator::Join, address, constraintExpression);
  expectedSql = "JOIN\n"\
                " \"Address\" \"ADR\"\n"\
                "  ON \"ADR\".\"personId\"=\"Person\".\"id\"";
  QCOMPARE(joinClauseListToSql(joinList, db), expectedSql);

  constraintExpression.setJoin(addressLandId == landId);
  joinList.addClause(JoinOperator::Join, land, constraintExpression);
  expectedSql = "JOIN\n"\
                " \"Address\" \"ADR\"\n"\
                "  ON \"ADR\".\"personId\"=\"Person\".\"id\"\n"\
                "JOIN\n"\
                " \"Land\"\n"\
                "  ON \"ADR\".\"landId\"=\"Land\".\"id\"";
  QCOMPARE(joinClauseListToSql(joinList, db), expectedSql);
}

void QueryExpressionSqlTransformTest::selectFromEntityToSqlTest()
{
  QString expectedSql;
  const auto db = database();

  expectedSql = "\"Address\"";
  QCOMPARE(selectFromEntityToSql( QueryEntity("Address"), db), expectedSql);

  expectedSql = "\"Address\" \"ADR\"";
  QCOMPARE(selectFromEntityToSql( QueryEntity("Address", EntityAlias("ADR") ), db), expectedSql);
}

void QueryExpressionSqlTransformTest::selectStatementToSqlLimitSyntaxTest()
{
  using Mdt::QueryExpression::SelectStatement;

  QString expectedSql;
  const auto db = database();

  QueryEntity person("Person");
  QueryField personId(person, "id");

  SelectStatement stm;
  stm.setEntityName("Person");
  stm.selectAllFields();
  expectedSql = "SELECT\n"\
                " *\n"\
                "FROM \"Person\"\n"\
                "LIMIT 150";
  QCOMPARE(selectStatementToSqlLimitSyntax(stm, 150, db), expectedSql);

  stm.clear();
  stm.setEntity(person);
  stm.selectAllFields();
  stm.setFilter(personId > 29);
  expectedSql = "SELECT\n"\
                " *\n"\
                "FROM \"Person\"\n"
                "WHERE \"Person\".\"id\">29\n"\
                "LIMIT 1234";
  QCOMPARE(selectStatementToSqlLimitSyntax(stm, 1234, db), expectedSql);
}

void QueryExpressionSqlTransformTest::selectStatementToSqlTopSyntaxTest()
{
  using Mdt::QueryExpression::SelectStatement;

  QString expectedSql;
  const auto db = database();

  QueryEntity person("Person");
  QueryField personId(person, "id");

  SelectStatement stm;
  stm.setEntityName("Person");
  stm.selectAllFields();
  expectedSql = "SELECT TOP 150\n"\
                " *\n"\
                "FROM \"Person\"";
  QCOMPARE(selectStatementToSqlTopSyntax(stm, 150, db), expectedSql);

  stm.clear();
  stm.setEntity(person);
  stm.selectAllFields();
  stm.setFilter(personId > 29);
  expectedSql = "SELECT TOP 1234\n"\
                " *\n"\
                "FROM \"Person\"\n"
                "WHERE \"Person\".\"id\">29";
  QCOMPARE(selectStatementToSqlTopSyntax(stm, 1234, db), expectedSql);
}

void QueryExpressionSqlTransformTest::selectStatementToSqlTest()
{
  using Mdt::QueryExpression::SelectStatement;

  QString expectedSql;
  const auto db = database();

  QueryEntity person("Person");
  QueryField personId(person, "id");

  SelectStatement stm;
  stm.setEntityName("Person");
  stm.selectAllFields();
  expectedSql = "SELECT\n"\
                " *\n"\
                "FROM \"Person\"";
  QCOMPARE(selectStatementToSql(stm, 0, db), expectedSql);

  stm.clear();
  stm.setEntity(person);
  stm.selectAllFields();
  stm.setFilter(personId > 29);
  expectedSql = "SELECT\n"\
                " *\n"\
                "FROM \"Person\"\n"
                "WHERE \"Person\".\"id\">29";
  QCOMPARE(selectStatementToSql(stm, 0, db), expectedSql);

  expectedSql = "SELECT\n"\
                " *\n"\
                "FROM \"Person\"\n"
                "WHERE \"Person\".\"id\">29\n"\
                "LIMIT 1200";
  QCOMPARE(selectStatementToSql(stm, 1200, db), expectedSql);
}

void QueryExpressionSqlTransformTest::selectStatementWithJoinToSqlTest()
{
  using Mdt::QueryExpression::SelectStatement;

  QString expectedSql;
  const auto db = database();

  QueryEntity person("Person");
  QueryEntity address("Address", EntityAlias("ADR"));

  QueryField personId(person, "id");
  QueryField addressPersonId(address, "personId");

  SelectStatement stm;
  stm.setEntity(person);
  stm.selectAllFields();
  stm.joinEntity(address, addressPersonId == personId);
  expectedSql = "SELECT\n"\
                " *\n"\
                "FROM \"Person\"\n"\
                "JOIN\n"\
                " \"Address\" \"ADR\"\n"\
                "  ON \"ADR\".\"personId\"=\"Person\".\"id\"";
  QCOMPARE(selectStatementToSql(stm, 0, db), expectedSql);

  stm.clear();
  stm.setEntity(person);
  stm.selectAllFields();
  stm.joinEntity(address, addressPersonId == personId);
  stm.setFilter(personId > 29);
  expectedSql = "SELECT\n"\
                " *\n"\
                "FROM \"Person\"\n"\
                "JOIN\n"\
                " \"Address\" \"ADR\"\n"\
                "  ON \"ADR\".\"personId\"=\"Person\".\"id\"\n"\
                "WHERE \"Person\".\"id\">29";
  QCOMPARE(selectStatementToSql(stm, 0, db), expectedSql);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  QueryExpressionSqlTransformTest test;

  return QTest::qExec(&test, argc, argv);
}

