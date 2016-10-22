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

#include <boost/proto/proto.hpp>

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

// void JoinConstraintExpressionTest::transforGetJoinConstraintFieldSqlTest()
// {
//   namespace Sql = Mdt::Sql;
// 
//   using Sql::JoinConstraintField;
//   using Sql::TableName;
//   using Sql::FieldName;
//   using Sql::Expression::JoinConstraint::GetJoinConstraintFieldSql;
// 
//   auto db = pvDatabase;
//   QVERIFY(db.isValid());
//   QString expectedSql;
//   GetJoinConstraintFieldSql transform;
// 
//   JoinConstraintField clientId(TableName("Client_tbl"), FieldName("Id_PK"));
//   expectedSql = "\"Client_tbl\".\"Id_PK\"";
//   QCOMPARE(transform(clientId, db), expectedSql);
// }

// void JoinConstraintExpressionTest::transforGetLiteralValueSqlTest()
// {
// 
// }

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
  /*
   * ==
   */
  expectedSql = "\"Client_tbl\".\"Id_PK\"=25";
  QCOMPARE(transform(clientId == 25, 0, db), expectedSql);

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
  expectedSql = "(\"Client_tbl\".\"Id_PK\"=25)AND(\"Client_tbl\".\"Id_PK\"=44)";
  QCOMPARE(transform( (clientId == 25) && (clientId == 44), 0, db), expectedSql);
  /*
   * OR
   */
  expectedSql = "(\"Client_tbl\".\"Id_PK\"=25)OR(\"Client_tbl\".\"Id_PK\"=44)";
  QCOMPARE(transform( (clientId == 25) || (clientId == 44), 0, db), expectedSql);
  /*
   * AND , OR
   */
  expectedSql = "Some str";
  QCOMPARE(transform( ((clientId == 25) && (clientId == 77)) || (clientId == 44), 0, db), expectedSql);
  expectedSql = "Some str";
  QCOMPARE(transform( ((clientId == 25) && (clientId == 77)) || ((clientId == 44) && (clientId == 88)), 0, db), expectedSql);
  expectedSql = "Some str";
  QCOMPARE(transform( ((clientId == 25) || (clientId == 77)) && (clientId == 44), 0, db), expectedSql);

}

// void JoinConstraintExpressionTest::transformGetTerminalSql()
// {
//   namespace Sql = Mdt::Sql;
// 
//   using Sql::JoinConstraintField;
//   using Sql::TableName;
//   using Sql::FieldName;
//   using Sql::Expression::JoinConstraint::GetTerminalSql;
// 
//   auto db = pvDatabase;
//   QVERIFY(db.isValid());
//   QString expectedSql;
//   GetTerminalSql transform;
// 
//   JoinConstraintField clientId(TableName("Client_tbl"), FieldName("Id_PK"));
//   expectedSql = "\"Client_tbl\".\"Id_PK\"";
//   /// QCOMPARE(transform(clientId, 0, db), expectedSql);
// 
// }

void JoinConstraintExpressionTest::transformGetComparisonSql()
{
  namespace Sql = Mdt::Sql;

  using Sql::JoinConstraintField;
  using Sql::TableName;
  using Sql::FieldName;
  using Sql::Expression::JoinConstraint::GetComparisonSql;

  auto db = pvDatabase;
  QVERIFY(db.isValid());
  QString expectedSql;
  GetComparisonSql transform;

  JoinConstraintField clientId(TableName("Client_tbl"), FieldName("Id_PK"));
  JoinConstraintField adrClientId(TableName("Address_tbl"), FieldName("Client_Id_FK"));
  /*
   * Check Field == Field
   */
  expectedSql = "\"Client_tbl\".\"Id_PK\"=\"Address_tbl\".\"Client_Id_FK\"";
  ///QCOMPARE(transform(clientId == adrClientId, 0, db), expectedSql);
  /*
   * Check Field == int
   */
  expectedSql = "\"Client_tbl\".\"Id_PK\"=25";
  ///QCOMPARE(transform(clientId == 25, 0, db), expectedSql);
}

void JoinConstraintExpressionTest::transforLogicalAndSqlTransformTest()
{
  namespace Sql = Mdt::Sql;

  using Sql::JoinConstraintField;
  using Sql::TableName;
  using Sql::FieldName;
  using Sql::Expression::JoinConstraint::LogicalAndSqlTransform;

  auto db = pvDatabase;
  QVERIFY(db.isValid());
  QString expectedSql;
  LogicalAndSqlTransform transform;

  JoinConstraintField clientId(TableName("Client_tbl"), FieldName("Id_PK"));
  JoinConstraintField adrClientId(TableName("Address_tbl"), FieldName("Client_Id_FK"));
  /*
   * Check (field == int) AND (field == int)
   */
//   expectedSql = "(\"Client_tbl\".\"Id_PK\"=25)AND(\"Address_tbl\".\"Client_Id_FK\"=44)";
//   QCOMPARE(transform( (clientId == 25) && (adrClientId == 44), 0, db), expectedSql);
  /*
   * Check (field == int) AND (field == int) AND (field == int)
   */
//   expectedSql = "(\"Client_tbl\".\"Id_PK\"=25)AND(\"Address_tbl\".\"Client_Id_FK\"=44)AND(\"Client_tbl\".\"Id_PK\"=36)";
//   QCOMPARE(transform( (clientId == 25) && (adrClientId == 44) && (clientId == 36), 0, db), expectedSql);

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
  JoinConstraintExpression exp1( A == 2 );
  expectedSql = "\"A\".\"a\"=2";
  QCOMPARE(exp1.toSql(db), expectedSql);

}

void JoinConstraintExpressionTest::expressionAssignSqliteTest()
{
  auto db = pvDatabase;
  QVERIFY(db.isValid());

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
