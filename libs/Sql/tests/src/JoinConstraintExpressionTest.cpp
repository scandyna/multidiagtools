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
#include "JoinConstraintExpressionTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/JoinConstraintField.h"
#include "Mdt/Sql/JoinConstraintExpression.h"
#include "Mdt/Sql/Expression/JoinConstraint/Grammar.h"
#include "Mdt/Sql/Expression/JoinConstraint/SqlTransform.h"
#include <QSqlDatabase>
#include <boost/proto/matches.hpp>
#include <boost/proto/literal.hpp>

// #include <boost/proto/proto.hpp>

/*
 * Init / cleanup
 */

void JoinConstraintExpressionTest::initTestCase()
{
  // Get database instance
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!pvDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
}

void JoinConstraintExpressionTest::cleanupTestCase()
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

/// \todo Should add grammarLiteralValueTest to check int, float, double, const char*, QDate, ...
/**
 * \note How to distiguisch :"some string" or "Date-Time" formatted string ??
 *  - Check while generating SQL ??
 *  - Simply request user to be explicit (f.ex. QDate("xx.yy.zz") ) ?
 *  - Define some TAG for date/time/datetime literal terminal ?
 *   -> Ex: Date("xx.yy.zz")
 * --> QDate/etc.. seems to be the simplest..
 */

// void JoinConstraintExpressionTest::terminalTypeTest()
// {
//   namespace Sql = Mdt::Sql;
// 
//   using Sql::JoinConstraintField;
//   using Sql::TableName;
//   using Sql::FieldName;
// 
//   JoinConstraintField clientId(TableName("Client_tbl"), FieldName("Id_PK"));
// 
//   decltype( boost::proto::_value()(clientId) );
// }

void JoinConstraintExpressionTest::grammarComparisonTest()
{
  namespace JoinConstraint = Mdt::Sql::Expression::JoinConstraint;
  namespace Sql = Mdt::Sql;

  using JoinConstraint::Comparison;
  using Sql::JoinConstraintField;
  using Sql::TableName;
  using Sql::FieldName;

  JoinConstraintField clientId(TableName("Client_tbl"), FieldName("Id_PK"));
  JoinConstraintField adrClientId(TableName("Address_tbl"), FieldName("Client_Id_FK"));

  static_assert(  expressionMatchesGrammar< decltype( clientId == 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 == clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId == "ID25" ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId == u8"ID25" ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( "ID25" == clientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( u8"ID25" == clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId == adrClientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( adrClientId == clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId != 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 != clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId != "ID25" ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId != u8"ID25" ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( "ID25" != clientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( u8"ID25" != clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId != adrClientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( adrClientId != clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId < 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 < clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId < "ID25" ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( "ID25" < clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId < adrClientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( adrClientId < clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId <= 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 <= clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId <= "ID25" ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( "ID25" <= clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId <= adrClientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( adrClientId <= clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId > 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 > clientId) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId > "ID25" ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( "ID25" > clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId > adrClientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( adrClientId > clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId >= 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 >= clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId >= "ID25" ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( "ID25" >= clientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId >= adrClientId ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( adrClientId >= clientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId + 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId - 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId << 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId >> 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId % 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId / 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId * 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId && 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId || 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId = 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId + adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId - adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId << adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId >> adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId % adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId / adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId * adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId && adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId || adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId = adrClientId ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId == (adrClientId < 25) ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( (clientId == adrClientId) == 32 ) , Comparison >() , "" );
//   static_assert( !expressionMatchesGrammar< decltype( clientId == clientId ) , Comparison >() , "" );
}

void JoinConstraintExpressionTest::grammarTest()
{
  namespace JoinConstraint = Mdt::Sql::Expression::JoinConstraint;
  namespace Sql = Mdt::Sql;

  using JoinConstraint::Grammar;
  using Sql::JoinConstraintField;
  using Sql::TableName;
  using Sql::FieldName;

  JoinConstraintField clientId(TableName("Client_tbl"), FieldName("Id_PK"));
  JoinConstraintField adrClientId(TableName("Address_tbl"), FieldName("Client_Id_FK"));

  static_assert(  expressionMatchesGrammar< decltype( clientId == 25 ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId == adrClientId ) , Grammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId && 25 ) , Grammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId && adrClientId ) , Grammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId || 25 ) , Grammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId || adrClientId ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == 25) && (adrClientId == 52) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (adrClientId < 20) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId < 25) && (adrClientId == clientId) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (clientId > 10) && (adrClientId < 100) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (clientId > 10) && (adrClientId < 100) && (adrClientId > 2) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (clientId > 10) && (clientId < 1000) && (adrClientId < 100) && (adrClientId > 2) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == 25) || (adrClientId == 52) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (adrClientId < 20) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId < 25) || (adrClientId == clientId) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (clientId > 10) || (adrClientId < 100) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (clientId > 10) || (adrClientId < 100) || (adrClientId > 2) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (clientId > 10) || (clientId < 1000) || (adrClientId < 100) || (adrClientId > 2) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && ( (adrClientId < 20) || (clientId < 50) ) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( ( (clientId == adrClientId) || (clientId == 2) ) && ( (adrClientId < 20) || (clientId < 50) ) ) , Grammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || ( (adrClientId > 20) && (adrClientId < 50) ) ) , Grammar >() , "" );
}

/*
 * Rutime tests
 */

void JoinConstraintExpressionTest::fieldTest()
{
  namespace Sql = Mdt::Sql;

  using Sql::JoinConstraintField;
  using Sql::TableName;
  using Sql::FieldName;


  JoinConstraintField A(TableName("A_tbl"), FieldName("id_A"));
  QCOMPARE( boost::proto::_value()(A).tableName(), QString("A_tbl"));
  QCOMPARE( boost::proto::_value()(A).fieldName(), QString("id_A"));
}

void JoinConstraintExpressionTest::terminalSqlTransformTest()
{
  namespace Sql = Mdt::Sql;

  using Sql::JoinConstraintField;
  using Sql::TableName;
  using Sql::FieldName;
  using Sql::Expression::JoinConstraint::LeftTerminalSqlTransform;
  using Sql::Expression::JoinConstraint::RightTerminalSqlTransform;

  auto db = pvDatabase;
  QVERIFY(db.isValid());
  QString expectedSql;
  LeftTerminalSqlTransform leftTerminalTransform;
  RightTerminalSqlTransform rightTerminalTransform;

  JoinConstraintField clientId(TableName("Client_tbl"), FieldName("Id_PK"));
  /*
   * Left terminal
   */
  expectedSql = "\"Client_tbl\".\"Id_PK\"";
  QCOMPARE(leftTerminalTransform(clientId, 0, db), expectedSql);
  /*
   * Right terminal with JoinConstraintField
   */
  expectedSql = "\"Client_tbl\".\"Id_PK\"";
  QCOMPARE(rightTerminalTransform(clientId, 0, db), expectedSql);
  /*
   * Right terminal with literal value
   */
  // int literal
  expectedSql = "25";
  QCOMPARE(rightTerminalTransform(boost::proto::lit(25), 0, db), expectedSql);
  // string literal
  expectedSql = "'str'";
  QCOMPARE(rightTerminalTransform(boost::proto::lit("str"), 0, db), expectedSql);

}

void JoinConstraintExpressionTest::comparisonSqlTransformTest()
{
  namespace Sql = Mdt::Sql;

  using Sql::JoinConstraintField;
  using Sql::TableName;
  using Sql::FieldName;
  using Sql::Expression::JoinConstraint::ComparisonSqlTransform;

  auto db = pvDatabase;
  QVERIFY(db.isValid());
  QString expectedSql;
  ComparisonSqlTransform transform;

  JoinConstraintField clientId(TableName("Client_tbl"), FieldName("Id_PK"));

  // ==
  expectedSql = "\"Client_tbl\".\"Id_PK\"=25";
  QCOMPARE(transform(clientId == 25, 0, db), expectedSql);
  // !=
  expectedSql = "\"Client_tbl\".\"Id_PK\"<>25";
  QCOMPARE(transform(clientId != 25, 0, db), expectedSql);
  // <
  expectedSql = "\"Client_tbl\".\"Id_PK\"<25";
  QCOMPARE(transform(clientId < 25, 0, db), expectedSql);
  // <=
  expectedSql = "\"Client_tbl\".\"Id_PK\"<=25";
  QCOMPARE(transform(clientId <= 25, 0, db), expectedSql);
  // >
  expectedSql = "\"Client_tbl\".\"Id_PK\">25";
  QCOMPARE(transform(clientId > 25, 0, db), expectedSql);
  // <=
  expectedSql = "\"Client_tbl\".\"Id_PK\">=25";
  QCOMPARE(transform(clientId >= 25, 0, db), expectedSql);
}

void JoinConstraintExpressionTest::sqlTransformTest()
{
  namespace Sql = Mdt::Sql;

  using Sql::JoinConstraintField;
  using Sql::TableName;
  using Sql::FieldName;
  using Sql::Expression::JoinConstraint::SqlTransform;

  auto db = pvDatabase;
  QVERIFY(db.isValid());
  QString expectedSql;
  SqlTransform transform;

  JoinConstraintField clientId(TableName("Client_tbl"), FieldName("Id_PK"));
  /*
   * Simple ==
   */
  expectedSql = "\"Client_tbl\".\"Id_PK\"=25";
  QCOMPARE(transform(clientId == 25, 0, db), expectedSql);
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
}

void JoinConstraintExpressionTest::expressionContructCopySqliteTest()
{
  namespace Sql = Mdt::Sql;

  using Sql::TableName;
  using Sql::FieldName;
  using Sql::JoinConstraintField;
  using Sql::JoinConstraintExpression;

  auto db = pvDatabase;
  QVERIFY(db.isValid());

  JoinConstraintField A(TableName("A"), FieldName("a"));
  JoinConstraintField B(TableName("B"), FieldName("b"));
  QString expectedSql;

  /*
   * Default construct
   */
  JoinConstraintExpression exp;
  /*
   * Construct
   */
  // Construct a expression
  JoinConstraintExpression exp1( A == 1 );
  expectedSql = "\"A\".\"a\"=1";
  QCOMPARE(exp1.toSql(db), expectedSql);
  // Construct a expression (move constructor)
  JoinConstraintExpression exp2 = ( A == 2 );
  expectedSql = "\"A\".\"a\"=2";
  QCOMPARE(exp2.toSql(db), expectedSql);
  // Copy construct
  JoinConstraintExpression exp22( exp2 );
  QCOMPARE(exp22.toSql(db), expectedSql);
  /*
   * Check lifetime
   */
  // Create exp3
  auto *exp3 = new JoinConstraintExpression( A == 3 );
  expectedSql = "\"A\".\"a\"=3";
  QCOMPARE(exp3->toSql(db), expectedSql);
  // Copy construct exp33 based on exp3
  JoinConstraintExpression exp33( *exp3 );
  expectedSql = "\"A\".\"a\"=3";
  QCOMPARE(exp33.toSql(db), expectedSql);
  // Delete exp3 and check that exp33 is still valid
  delete exp3;
  exp3 = nullptr;
  expectedSql = "\"A\".\"a\"=3";
  QCOMPARE(exp33.toSql(db), expectedSql);
}

void JoinConstraintExpressionTest::expressionAssignSqliteTest()
{
  namespace Sql = Mdt::Sql;

  using Sql::TableName;
  using Sql::FieldName;
  using Sql::JoinConstraintField;
  using Sql::JoinConstraintExpression;

  auto db = pvDatabase;
  QVERIFY(db.isValid());

  JoinConstraintField A(TableName("A"), FieldName("a"));
  JoinConstraintField B(TableName("B"), FieldName("b"));
  QString expectedSql;

  // Use setter
  JoinConstraintExpression exp1;
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
  JoinConstraintExpression exp2( A == 2 );
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
  auto *exp3 = new JoinConstraintExpression( A == 3 );
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
  JoinConstraintExpressionTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
