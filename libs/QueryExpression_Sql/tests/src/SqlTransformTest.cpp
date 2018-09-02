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
#include "SqlTransformTest.h"
#include "Mdt/QueryExpression/SqlTransform.h"
#include "Mdt/QueryExpression/SelectField.h"
#include "Mdt/QueryExpression/SelectFieldList.h"
#include "Mdt/QueryExpression/SelectEntity.h"

using namespace Mdt::QueryExpression;

void SqlTransformTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
}

void SqlTransformTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SqlTransformTest::selectFieldDeclarationToSqlTest()
{
  QString expectedSql;
  const auto db = database();
  QVERIFY(db.isValid());

  SelectEntity person( EntityName("Person") );
  SelectEntity address( EntityName("Address"), "ADR");

  expectedSql = "*";
  QCOMPARE(selectFieldDeclarationToSql(SelectAllField(), db), expectedSql);

  expectedSql = "\"Person\".*";
  QCOMPARE(selectFieldDeclarationToSql(SelectAllField(person), db), expectedSql);

  expectedSql = "\"ADR\".*";
  QCOMPARE(selectFieldDeclarationToSql(SelectAllField(address), db), expectedSql);

  expectedSql = "\"age\"";
  QCOMPARE(selectFieldDeclarationToSql(SelectField(FieldName("age")), db), expectedSql);

  expectedSql = "\"age\" AS \"A\"";
  QCOMPARE(selectFieldDeclarationToSql(SelectField(FieldName("age"), "A"), db), expectedSql);

  expectedSql = "\"Person\".\"age\"";
  QCOMPARE(selectFieldDeclarationToSql(SelectField(person, FieldName("age")), db), expectedSql);

  expectedSql = "\"Person\".\"age\" AS \"PersonAge\"";
  QCOMPARE(selectFieldDeclarationToSql(SelectField(person, FieldName("age"), "PersonAge"), db), expectedSql);

  expectedSql = "\"ADR\".\"street\"";
  QCOMPARE(selectFieldDeclarationToSql(SelectField(address, FieldName("street")), db), expectedSql);

  expectedSql = "\"ADR\".\"street\" AS \"AddressStreet\"";
  QCOMPARE(selectFieldDeclarationToSql(SelectField(address, FieldName("street"), "AddressStreet"), db), expectedSql);
}

void SqlTransformTest::selectFieldListDeclarationToSqlTest()
{
  QString expectedSql;
  const auto db = database();

  SelectFieldList fieldList;

  fieldList.addField(FieldName("id"));
  expectedSql = " \"id\"";
  QCOMPARE(selectFieldListDeclarationToSql(fieldList, db), expectedSql);

  fieldList.addField(FieldName("age"),"A");
  expectedSql = " \"id\",\n"\
                " \"age\" AS \"A\"";
  QCOMPARE(selectFieldListDeclarationToSql(fieldList, db), expectedSql);

  fieldList.addField(FieldName("name"));
  expectedSql = " \"id\",\n"\
                " \"age\" AS \"A\",\n"\
                " \"name\"";
  QCOMPARE(selectFieldListDeclarationToSql(fieldList, db), expectedSql);
}

void SqlTransformTest::selectFieldConditionUsageToSqlTest()
{
  QString expectedSql;
  const auto db = database();
  QVERIFY(db.isValid());

  SelectEntity person( EntityName("Person") );
  SelectEntity address( EntityName("Address"), "ADR");

  expectedSql = "\"age\"";
  QCOMPARE(fieldConditionUsageToSql(EntityAndField(FieldName("age")), db), expectedSql);

  expectedSql = "\"age\"";
  QCOMPARE(fieldConditionUsageToSql(EntityAndField(FieldName("age"), "A"), db), expectedSql);

  expectedSql = "\"Person\".\"age\"";
  QCOMPARE(fieldConditionUsageToSql(EntityAndField(person, FieldName("age")), db), expectedSql);

  expectedSql = "\"Person\".\"age\"";
  QCOMPARE(fieldConditionUsageToSql(EntityAndField(person, FieldName("age"), "PersonAge"), db), expectedSql);

  expectedSql = "\"ADR\".\"street\"";
  QCOMPARE(fieldConditionUsageToSql(EntityAndField(address, FieldName("street")), db), expectedSql);

  expectedSql = "\"ADR\".\"street\"";
  QCOMPARE(fieldConditionUsageToSql(EntityAndField(address, FieldName("street"), "AddressStreet"), db), expectedSql);
}

void SqlTransformTest::selectFromEntityToSqlTest()
{
  QString expectedSql;
  const auto db = database();

  expectedSql = "\"Address\"";
  QCOMPARE(selectFromEntityToSql( SelectEntity(EntityName("Address") ), db), expectedSql);

  expectedSql = "\"Address\" \"ADR\"";
  QCOMPARE(selectFromEntityToSql( SelectEntity(EntityName("Address"), "ADR" ), db), expectedSql);
}

void SqlTransformTest::operatorToSqlTest()
{
  QCOMPARE(operatorToSql(ComparisonOperator::Equal), QString("="));
  QCOMPARE(operatorToSql(ComparisonOperator::NotEqual), QString("<>"));
  QCOMPARE(operatorToSql(ComparisonOperator::Less), QString("<"));
  QCOMPARE(operatorToSql(ComparisonOperator::LessEqual), QString("<="));
  QCOMPARE(operatorToSql(ComparisonOperator::Greater), QString(">"));
  QCOMPARE(operatorToSql(ComparisonOperator::GreaterEqual), QString(">="));
  QCOMPARE(operatorToSql(LogicalOperator::And), QString("AND"));
  QCOMPARE(operatorToSql(LogicalOperator::Or), QString("OR"));
}

void SqlTransformTest::likeExpressionToSqlTest()
{
  QFETCH(QString, likeExpressionString);
  QFETCH(QString, expectedSql);
  const auto db = database();

  LikeExpressionData likeExpression(likeExpressionString);
  QCOMPARE(likeExpressionToSql(likeExpression, db), expectedSql);
}

void SqlTransformTest::likeExpressionToSqlTest_data()
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

void SqlTransformTest::filterExpressionToSqlTest()
{
  using Like = LikeExpression;

  QString expectedSql;
  const auto db = database();
  FilterExpression filter;

  SelectEntity person( EntityName("Person") );
  SelectEntity address( EntityName("Address"), "ADR");

  SelectField id(FieldName("id"));
  SelectField personId(person, FieldName("id"), "personId");
  SelectField personName(person, FieldName("name"));
  SelectField addressPersonId(address, FieldName("personId"));

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

void SqlTransformTest::selectStatementToSqlLimitSyntaxTest()
{
  QString expectedSql;
  const auto db = database();

  SelectEntity person( EntityName("Person") );
  SelectField personId(person, FieldName("id"));

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

void SqlTransformTest::selectStatementToSqlTopSyntaxTest()
{
  QString expectedSql;
  const auto db = database();

  SelectEntity person( EntityName("Person") );
  SelectField personId(person, FieldName("id"));

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

void SqlTransformTest::selectStatementToSqlTest()
{
  QString expectedSql;
  const auto db = database();

  SelectEntity person( EntityName("Person") );
  SelectField personId(person, FieldName("id"));

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

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SqlTransformTest test;

  return QTest::qExec(&test, argc, argv);
}
