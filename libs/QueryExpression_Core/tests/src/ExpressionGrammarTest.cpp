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
#include "ExpressionGrammarTest.h"
#include "Mdt/QueryExpression/FieldName.h"
#include "Mdt/QueryExpression/EntityName.h"
#include "Mdt/QueryExpression/LiteralValue.h"
#include "Mdt/QueryExpression/Terminal.h"
#include "Mdt/QueryExpression/Comparison.h"
#include "Mdt/QueryExpression/ExpressionGrammar.h"
#include <boost/proto/matches.hpp>
#include <boost/proto/literal.hpp>

using namespace Mdt::QueryExpression;

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

void ExpressionGrammarTest::literalValueTest()
{
  struct MyType{};
  boost::proto::terminal<MyType>::type _x{{}};

  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , LiteralValue >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit("ID44") ) , LiteralValue >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(u8"éèj") ) , LiteralValue >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( _x ) , LiteralValue >() , "" );
}

void ExpressionGrammarTest::terminalTest()
{
  SelectField A( FieldName("A") );

  // LeftTerminal
  static_assert(  expressionMatchesGrammar< decltype( A ) , LeftTerminal >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , LeftTerminal >() , "" );
  // RightTerminal
  static_assert(  expressionMatchesGrammar< decltype( A ) , RightTerminal >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , RightTerminal >() , "" );
}

void ExpressionGrammarTest::comparisonTest()
{
  SelectField A(EntityName("A"), FieldName("a"));
  SelectField B(EntityName("B"), FieldName("b"));

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

void ExpressionGrammarTest::expressionTest()
{
  using Like = LikeExpression;

  SelectField clientId(EntityName("Client"), FieldName("id"));
  SelectField adrClientId(EntityName("Address"), FieldName("Client_id"));

  static_assert(  expressionMatchesGrammar< decltype( clientId == 25 ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId == adrClientId ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( clientId == Like("?25?") ) , ExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId != Like("?25?") ) , ExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId < Like("?25?") ) , ExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId > Like("?25?") ) , ExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId <= Like("?25?") ) , ExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId >= Like("?25?") ) , ExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( Like("?25?") == clientId ) , ExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId && 25 ) , ExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId && adrClientId ) , ExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId || 25 ) , ExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( clientId || adrClientId ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == 25) && (adrClientId == 52) ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (adrClientId < 20) ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId < 25) && (adrClientId == clientId) ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (clientId > 10) && (adrClientId < 100) ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (clientId > 10) && (adrClientId < 100) && (adrClientId > 2) ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && (clientId > 10) && (clientId < 1000) && (adrClientId < 100) && (adrClientId > 2) ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == 25) || (adrClientId == 52) ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (adrClientId < 20) ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId < 25) || (adrClientId == clientId) ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (clientId > 10) || (adrClientId < 100) ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (clientId > 10) || (adrClientId < 100) || (adrClientId > 2) ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || (clientId > 10) || (clientId < 1000) || (adrClientId < 100) || (adrClientId > 2) ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) && ( (adrClientId < 20) || (clientId < 50) ) ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( ( (clientId == adrClientId) || (clientId == 2) ) && ( (adrClientId < 20) || (clientId < 50) ) ) , ExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (clientId == adrClientId) || ( (adrClientId > 20) && (adrClientId < 50) ) ) , ExpressionGrammar >() , "" );
}

/*
 * Runtime tests
 */

void ExpressionGrammarTest::fieldNameTest()
{
  FieldName n("");
  QVERIFY(n.isNull());

  FieldName a("A");
  QVERIFY(!a.isNull());
  QCOMPARE(a.toString(), QString("A"));

  QVERIFY(FieldName("A") == FieldName("A"));
  QVERIFY(FieldName("A") == FieldName("a"));
  QVERIFY( !(FieldName("A") == FieldName("B")) );
}

void ExpressionGrammarTest::entityNameTest()
{
  EntityName n("");
  QVERIFY(n.isNull());

  EntityName a("A");
  QVERIFY(!a.isNull());
  QCOMPARE(a.toString(), QString("A"));
}


void ExpressionGrammarTest::fieldTerminalTest()
{
  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ExpressionGrammarTest test;

  return QTest::qExec(&test, argc, argv);
}
