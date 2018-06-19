/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "WhereExpressionTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/WhereField.h"
#include "Mdt/Sql/Expression/WhereExpressionGrammar.h"
#include "Mdt/Sql/Expression/WhereExpressionSqlTransform.h"
#include "Mdt/Sql/WhereExpression.h"
#include <QSqlDatabase>
#include <boost/proto/matches.hpp>
#include <boost/proto/literal.hpp>
#include <boost/proto/transform/arg.hpp>

namespace Sql = Mdt::Sql;

/*
 * Init/cleanup
 */

void WhereExpressionTest::initTestCase()
{
  // Get database instance
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!pvDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
}

void WhereExpressionTest::cleanupTestCase()
{
}

/*
 * Compile time tests
 */

template<typename Expr, typename Grammar>
constexpr bool expressionMatchesGrammar()
{
  return boost::proto::matches< Expr, Grammar >::value;
}

void WhereExpressionTest::grammarTest()
{
  using Sql::Expression::WhereExpressionGrammar;
  using Sql::WhereField;
  using Sql::TableName;
  using Sql::FieldName;
  using Like = Sql::LikeExpression;

  WhereField clientId(TableName("Client_tbl"), FieldName("Id_PK"));
  WhereField adrClientId(TableName("Address_tbl"), FieldName("Client_Id_FK"));

  static_assert(  expressionMatchesGrammar< decltype( clientId == 25 ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId == adrClientId ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId == Like("?25?") ) , WhereExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId != Like("?25?") ) , WhereExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId < Like("?25?") ) , WhereExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId > Like("?25?") ) , WhereExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId <= Like("?25?") ) , WhereExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId >= Like("?25?") ) , WhereExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( Like("?25?") == clientId ) , WhereExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId && 25 ) , WhereExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId && adrClientId ) , WhereExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId || 25 ) , WhereExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId || adrClientId ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == 25) && (adrClientId == 52) ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (adrClientId < 20) ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId < 25) && (adrClientId == clientId) ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (clientId > 10) && (adrClientId < 100) ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (clientId > 10) && (adrClientId < 100) && (adrClientId > 2) ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (clientId > 10) && (clientId < 1000) && (adrClientId < 100) && (adrClientId > 2) ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == 25) || (adrClientId == 52) ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (adrClientId < 20) ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId < 25) || (adrClientId == clientId) ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (clientId > 10) || (adrClientId < 100) ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (clientId > 10) || (adrClientId < 100) || (adrClientId > 2) ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (clientId > 10) || (clientId < 1000) || (adrClientId < 100) || (adrClientId > 2) ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && ( (adrClientId < 20) || (clientId < 50) ) ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( ( (clientId == adrClientId) || (clientId == 2) ) && ( (adrClientId < 20) || (clientId < 50) ) ) , WhereExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || ( (adrClientId > 20) && (adrClientId < 50) ) ) , WhereExpressionGrammar >() , "" );
}

/*
 * Runtime tests
 */

void WhereExpressionTest::fieldTest()
{
  using Sql::WhereField;
  using Sql::FieldName;
  using Sql::TableName;

  WhereField A(TableName("A_tbl"), FieldName("id_A"));
  QCOMPARE( boost::proto::_value()(A).tableName(), QString("A_tbl"));
  QCOMPARE( boost::proto::_value()(A).fieldName(), QString("id_A"));
}

void WhereExpressionTest::sqlTransformTest()
{
  using Sql::WhereField;
  using Like = Sql::LikeExpression;
  using Sql::TableName;
  using Sql::FieldName;
  using Sql::Expression::WhereExpressionSqlTransform;

  auto db = pvDatabase;
  QVERIFY(db.isValid());
  QString expectedSql;
  WhereExpressionSqlTransform transform;

  WhereField clientId(TableName("Client_tbl"), FieldName("Id_PK"));
  /*
   * Simple ==
   */
  // Check with integral value
  expectedSql = "\"Client_tbl\".\"Id_PK\"=25";
  QCOMPARE(transform(clientId == 25, 0, db), expectedSql);
  // Check with string literal
  expectedSql = "\"Client_tbl\".\"Id_PK\"='A25'";
  QCOMPARE(transform(clientId == "A25", 0, db), expectedSql);
  // Check with LIKE expression
  expectedSql = "\"Client_tbl\".\"Id_PK\" LIKE '_25_'";
  QCOMPARE(transform(clientId == Like("?25?"), 0, db), expectedSql);
  /*
   * AND
   */
  expectedSql = "(\"Client_tbl\".\"Id_PK\">0)AND(\"Client_tbl\".\"Id_PK\"<44)";
  QCOMPARE(transform( (clientId > 0) && (clientId < 44), 0, db), expectedSql);
  /*
   * OR
   */
  expectedSql = "(\"Client_tbl\".\"Id_PK\"=25)OR(\"Client_tbl\".\"Id_PK\"=44)";
  QCOMPARE(transform( (clientId == 25) || (clientId == 44), 0, db), expectedSql);
  /*
   * AND , OR
   */
  expectedSql = "((\"Client_tbl\".\"Id_PK\">1)AND(\"Client_tbl\".\"Id_PK\"<10))OR(\"Client_tbl\".\"Id_PK\"=44)";
  QCOMPARE(transform( ((clientId > 1) && (clientId < 10)) || (clientId == 44), 0, db), expectedSql);
  expectedSql = "((\"Client_tbl\".\"Id_PK\">1)AND(\"Client_tbl\".\"Id_PK\"<10))OR((\"Client_tbl\".\"Id_PK\">20)AND(\"Client_tbl\".\"Id_PK\"<30))";
  QCOMPARE(transform( ((clientId > 1) && (clientId < 10)) || ((clientId > 20) && (clientId < 30)), 0, db), expectedSql);
  expectedSql = "((\"Client_tbl\".\"Id_PK\">1)AND(\"Client_tbl\".\"Id_PK\"<10))OR(\"Client_tbl\".\"Id_PK\" LIKE '44%')";
  QCOMPARE(transform( ((clientId > 1) && (clientId < 10)) || (clientId == Like("44*")), 0, db), expectedSql);
}

void WhereExpressionTest::expressionContructCopySqliteTest()
{
  using Sql::TableName;
  using Sql::FieldName;
  using Sql::WhereField;
  using Sql::WhereExpression;

  auto db = pvDatabase;
  QVERIFY(db.isValid());

  WhereField A(TableName("A"), FieldName("a"));
  WhereField B(TableName("B"), FieldName("b"));
  QString expectedSql;

  /*
   * Default construct
   */
  WhereExpression exp;
  QVERIFY(exp.isNull());
  /*
   * Construct
   */
  // Construct a expression
  WhereExpression exp1( A == 1 );
  QVERIFY(!exp1.isNull());
  expectedSql = "\"A\".\"a\"=1";
  QCOMPARE(exp1.toSql(db), expectedSql);
  // Construct a expression (move constructor)
  WhereExpression exp2 = ( A == 2 );
  expectedSql = "\"A\".\"a\"=2";
  QCOMPARE(exp2.toSql(db), expectedSql);
  // Copy construct
  WhereExpression exp22( exp2 );
  QCOMPARE(exp22.toSql(db), expectedSql);
  /*
   * Check lifetime
   */
  // Create exp3
  auto *exp3 = new WhereExpression( A == 3 );
  expectedSql = "\"A\".\"a\"=3";
  QCOMPARE(exp3->toSql(db), expectedSql);
  // Copy construct exp33 based on exp3
  WhereExpression exp33( *exp3 );
  expectedSql = "\"A\".\"a\"=3";
  QCOMPARE(exp33.toSql(db), expectedSql);
  // Delete exp3 and check that exp33 is still valid
  delete exp3;
  exp3 = nullptr;
  expectedSql = "\"A\".\"a\"=3";
  QCOMPARE(exp33.toSql(db), expectedSql);
}

void WhereExpressionTest::expressionAssignSqliteTest()
{
  using Sql::TableName;
  using Sql::FieldName;
  using Sql::WhereField;
  using Sql::WhereExpression;

  auto db = pvDatabase;
  QVERIFY(db.isValid());

  WhereField A(TableName("A"), FieldName("a"));
  WhereField B(TableName("B"), FieldName("b"));
  QString expectedSql;

  // Use setter
  WhereExpression exp1;
  exp1.setExpression( A == 1 );
  expectedSql = "\"A\".\"a\"=1";
  QCOMPARE(exp1.toSql(db), expectedSql);
  // Move assing
  exp1 = ( A == 11 );
  expectedSql = "\"A\".\"a\"=11";
  QCOMPARE(exp1.toSql(db), expectedSql);
  /*
   * Check copy assign
   */
  WhereExpression exp2( A == 2 );
  expectedSql = "\"A\".\"a\"=2";
  QCOMPARE(exp2.toSql(db), expectedSql);
  // Assign
  exp2 = exp1;
  expectedSql = "\"A\".\"a\"=11";
  QCOMPARE(exp2.toSql(db), expectedSql);
  // Change exp1 and check that exp2 is not touched
  exp1.setExpression( A == 1 );
  expectedSql = "\"A\".\"a\"=1";
  QCOMPARE(exp1.toSql(db), expectedSql);
  expectedSql = "\"A\".\"a\"=11";
  QCOMPARE(exp2.toSql(db), expectedSql);
  // Change exp2 and check that exp1 is not touched
  exp2.setExpression( A == 2 );
  expectedSql = "\"A\".\"a\"=1";
  QCOMPARE(exp1.toSql(db), expectedSql);
  expectedSql = "\"A\".\"a\"=2";
  QCOMPARE(exp2.toSql(db), expectedSql);
  /*
   * Check lifetime
   */
  // Create exp3
  auto *exp3 = new WhereExpression( A == 3 );
  expectedSql = "\"A\".\"a\"=3";
  QCOMPARE(exp3->toSql(db), expectedSql);
  // Assign exp3 to exp1
  exp1 = *exp3;
  QCOMPARE(exp1.toSql(db), expectedSql);
  // Delete exp3 and check that exp33 is still valid
  delete exp3;
  exp3 = nullptr;
  expectedSql = "\"A\".\"a\"=3";
  QCOMPARE(exp1.toSql(db), expectedSql);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  WhereExpressionTest test;

  return QTest::qExec(&test, argc, argv);
}
