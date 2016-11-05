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
#include "ExpressionTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/Expression/LiteralValue.h"
#include "Mdt/Sql/Expression/Terminal.h"
#include "Mdt/Sql/Expression/Comparison.h"
#include "Mdt/Sql/Expression/TerminalSqlTransform.h"
#include "Mdt/Sql/Expression/ComparisonSqlTransform.h"
#include "Schema/Client_tbl.h"
#include <QSqlDatabase>
#include <boost/proto/matches.hpp>
#include <boost/proto/literal.hpp>
#include <boost/proto/transform/arg.hpp>

namespace Sql = Mdt::Sql;

/*
 * Terminal type used in tests
 */
using TestTerminal = const Sql::Expression::TableFieldTerminal<>;

/*
 * Init / cleanup
 */

void ExpressionTest::initTestCase()
{
  // Get database instance
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!pvDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
}

void ExpressionTest::cleanupTestCase()
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

/// \todo Should also check int, float, double, const char*, QDate, ...
/**
 * \note How to distiguisch :"some string" or "Date-Time" formatted string ??
 *  - Check while generating SQL ??
 *  - Simply request user to be explicit (f.ex. QDate("xx.yy.zz") ) ?
 *  - Define some TAG for date/time/datetime literal terminal ?
 *   -> Ex: Date("xx.yy.zz")
 * --> QDate/etc.. seems to be the simplest..
 */

void ExpressionTest::literalValueTest()
{
  using Sql::Expression::LiteralValue;

  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , LiteralValue >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit("ID44") ) , LiteralValue >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(u8"éèj") ) , LiteralValue >() , "" );
}

void ExpressionTest::terminalTest()
{
  using Sql::Expression::LeftTerminal;
  using Sql::Expression::RightTerminal;
  using Sql::TableName;
  using Sql::FieldName;

  TestTerminal A(TableName("A"), FieldName("a"));

  // LeftTerminal
  static_assert(  expressionMatchesGrammar< decltype( A ) , LeftTerminal >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , LeftTerminal >() , "" );
  // RightTerminal
  static_assert(  expressionMatchesGrammar< decltype( A ) , RightTerminal >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , RightTerminal >() , "" );
}

void ExpressionTest::comparisonTest()
{
  using Sql::Expression::Comparison;
  using Sql::Expression::LeftTerminal;
  using Sql::Expression::RightTerminal;
  using Sql::TableName;
  using Sql::FieldName;

  TestTerminal A(TableName("A"), FieldName("a"));
  TestTerminal B(TableName("B"), FieldName("b"));

  // ==
  static_assert(  expressionMatchesGrammar< decltype( A == 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 == A ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A == B ) , Comparison >() , "" );
  // !=
  static_assert(  expressionMatchesGrammar< decltype( A != 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 != A ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A != B ) , Comparison >() , "" );
  // <
  static_assert(  expressionMatchesGrammar< decltype( A < 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 < A ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A < B ) , Comparison >() , "" );
  // <=
  static_assert(  expressionMatchesGrammar< decltype( A <= 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 <= A ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A <= B ) , Comparison >() , "" );
  // >
  static_assert(  expressionMatchesGrammar< decltype( A > 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 > A ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A > B ) , Comparison >() , "" );
  // >=
  static_assert(  expressionMatchesGrammar< decltype( A >= 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 >= A ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A >= B ) , Comparison >() , "" );
  // Invalid expressions
  static_assert( !expressionMatchesGrammar< decltype( 25 + A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A + 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A + B ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 - A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A - 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A - B ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 * A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A * 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A * B ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 / A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A / 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A / B ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 % A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A % 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A % B ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 << A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A << 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A << B ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 >> A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A >> 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A >> B ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 || A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A || 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A || B ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 && A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A && 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A && B ) , Comparison >() , "" );
}

/*
 * Rutime tests
 */

void ExpressionTest::fieldTest()
{
  using Sql::TableName;
  using Sql::FieldName;
  using Sql::SelectTable;

  // Must not compile
//   TestTerminal Bug("BugT", "BugF");

  // Create from table name
  TestTerminal A(TableName("A_tbl"), FieldName("a_field"));
  QCOMPARE( boost::proto::_value()(A).tableName(), QString("A_tbl"));
  QCOMPARE( boost::proto::_value()(A).fieldName(), QString("a_field"));
  // Create from a select table and field name
  SelectTable SB(TableName("B_tbl"), "B");
  TestTerminal B(SB, FieldName("b_field"));
  QCOMPARE( boost::proto::_value()(B).tableName(), QString("B"));
  QCOMPARE( boost::proto::_value()(B).fieldName(), QString("b_field"));
  // Create from a select table and a field
  Schema::Client_tbl client;
  SelectTable CLI(client, "CLI");
  TestTerminal clientId(CLI, client.Id_PK());
  QCOMPARE( boost::proto::_value()(clientId).tableName(), QString("CLI"));
  QCOMPARE( boost::proto::_value()(clientId).fieldName(), QString("Id_PK"));
  // Create from a select table without alias
  SelectTable SD(TableName("D_tbl"));
  TestTerminal D(SD, FieldName("d_field"));
  QCOMPARE( boost::proto::_value()(D).tableName(), QString("D_tbl"));
  QCOMPARE( boost::proto::_value()(D).fieldName(), QString("d_field"));
}

void ExpressionTest::terminalSqlTransformTest()
{
  using Sql::TableName;
  using Sql::FieldName;
  using Sql::Expression::LeftTerminalSqlTransform;
  using Sql::Expression::RightTerminalSqlTransform;

  auto db = pvDatabase;
  QVERIFY(db.isValid());
  QString expectedSql;
  LeftTerminalSqlTransform leftTerminalTransform;
  RightTerminalSqlTransform rightTerminalTransform;

  TestTerminal clientId(TableName("Client_tbl"), FieldName("Id_PK"));
  /*
   * Left terminal
   */
  expectedSql = "\"Client_tbl\".\"Id_PK\"";
  QCOMPARE(leftTerminalTransform(clientId, 0, db), expectedSql);
  /*
   * Right terminal with TestTerminal
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

void ExpressionTest::comparisonSqlTransformTest()
{
  using Sql::TableName;
  using Sql::FieldName;
  using Sql::Expression::ComparisonSqlTransform;

  auto db = pvDatabase;
  QVERIFY(db.isValid());
  QString expectedSql;
  ComparisonSqlTransform transform;
  TestTerminal clientId(TableName("Client_tbl"), FieldName("Id_PK"));

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


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ExpressionTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
