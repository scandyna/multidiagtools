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
#include "FilterExpressionTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/FilterColumn.h"
#include "Mdt/ItemEditor/Expression/LeftTerminal.h"
#include "Mdt/ItemEditor/Expression/RightTerminal.h"
#include "Mdt/ItemEditor/Expression/Comparison.h"
#include "Mdt/ItemEditor/Expression/FilterExpressionGrammar.h"
#include <QRegularExpression>
#include <boost/proto/matches.hpp>
#include <boost/proto/literal.hpp>
#include <boost/proto/transform/arg.hpp>

// #include <QDebug>

namespace ItemEditor = Mdt::ItemEditor;

void FilterExpressionTest::initTestCase()
{
}

void FilterExpressionTest::cleanupTestCase()
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

void FilterExpressionTest::literalValueTest()
{
  using Mdt::FilterExpression::LiteralValue;
  using ItemEditor::FilterColumn;

  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , LiteralValue >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( FilterColumn(1) ) , LiteralValue >() , "" );
}

void FilterExpressionTest::leftTerminalTest()
{
  using ItemEditor::Expression::LeftTerminal;
  using ItemEditor::FilterColumn;

  FilterColumn A(1);

  static_assert(  expressionMatchesGrammar< decltype( A ) , LeftTerminal >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , LeftTerminal >() , "" );
}

void FilterExpressionTest::rightTerminalTest()
{
  using ItemEditor::Expression::RightTerminal;
  using ItemEditor::FilterColumn;
  using Like = ItemEditor::LikeExpression;

  FilterColumn A(1);

  static_assert( !expressionMatchesGrammar< decltype( A ) , RightTerminal >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , RightTerminal >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( Like("A?") ) , RightTerminal >() , "" );
}

void FilterExpressionTest::comparisonTest()
{
  using ItemEditor::Expression::Comparison;
  using ItemEditor::FilterColumn;
  using Like = ItemEditor::LikeExpression;

  FilterColumn A(1);
  FilterColumn B(2);

  // ==
  static_assert(  expressionMatchesGrammar< decltype( A == 25 ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A == Like("25*") ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 == A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A == B ) , Comparison >() , "" );
  // !=
  static_assert(  expressionMatchesGrammar< decltype( A != 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 != A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A != B ) , Comparison >() , "" );
  // <
  static_assert(  expressionMatchesGrammar< decltype( A < 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 < A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A < B ) , Comparison >() , "" );
  // <=
  static_assert(  expressionMatchesGrammar< decltype( A <= 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 <= A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A <= B ) , Comparison >() , "" );
  // >
  static_assert(  expressionMatchesGrammar< decltype( A > 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 > A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A > B ) , Comparison >() , "" );
  // >=
  static_assert(  expressionMatchesGrammar< decltype( A >= 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 >= A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A >= B ) , Comparison >() , "" );
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

void FilterExpressionTest::filterExpressionGrammarTest()
{
  using ItemEditor::Expression::FilterExpressionGrammar;
  using ItemEditor::FilterColumn;
  using Like = ItemEditor::LikeExpression;

  FilterColumn A(1);
  FilterColumn B(2);

  // Comparisons
  static_assert(  expressionMatchesGrammar< decltype( A == 25 ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A != 25 ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A == Like("25?") ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A < 25 ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A <= 25 ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A > 25 ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A >= 25 ) , FilterExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( (A == 25) == 44 ) , FilterExpressionGrammar >() , "" );
  // AND
  static_assert( !expressionMatchesGrammar< decltype( A && 25 ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A > 25 && B == Like("*s") ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A > 25 && B == Like("*s") && B < 46 ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A > 25 && A < 50 && B == Like("*s") && B < 46 ) , FilterExpressionGrammar >() , "" );
  // OR
  static_assert( !expressionMatchesGrammar< decltype( A || 25 ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (A == 25) || (B == 44) ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (A == 25) || (B == 44) || (B == 77) ) , FilterExpressionGrammar >() , "" );
  // AND , OR
  static_assert(  expressionMatchesGrammar< decltype( (A == 25) || (B == 26 && A < 50) ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (A == 25) && ((B == 26) || (A < 50)) ) , FilterExpressionGrammar >() , "" );
}

/*
 * Runtime tests
 */

void FilterExpressionTest::filterColumnTest()
{
  using ItemEditor::FilterColumn;

  FilterColumn A(1);
  QCOMPARE( boost::proto::_value()(A).columnIndex() , 1 );
}

void FilterExpressionTest::sandbox()
{
//   using ItemEditor::FilterColumn;
// 
//   FilterColumn a(1);
// 
//   ///boost::proto::display_expr( a % "A" );
//   
//   QRegularExpression exp;
//   exp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
//   exp.setPattern("A");
//   checkMatches("A" , exp);
//   checkMatches("ABC" , exp);
//   checkMatches("123ABC" , exp);
//   checkMatches("123BC" , exp);
//   checkMatches("123abc" , exp);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  FilterExpressionTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
