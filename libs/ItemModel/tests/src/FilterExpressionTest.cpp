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
#include "Mdt/ItemModel/FilterColumn.h"
#include "Mdt/ItemModel/Expression/LeftTerminal.h"
#include "Mdt/ItemModel/Expression/RightTerminal.h"
#include "Mdt/ItemModel/Expression/Comparison.h"
#include "Mdt/ItemModel/Expression/ComparisonEval.h"
#include "Mdt/ItemModel/Expression/FilterEval.h"
#include "Mdt/ItemModel/FilterExpression.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QRegularExpression>
#include <boost/proto/matches.hpp>
#include <boost/proto/literal.hpp>
#include <boost/proto/transform/arg.hpp>

#include <boost/proto/proto.hpp>

// #include <QDebug>

namespace ItemModel = Mdt::ItemModel;
using ItemModel::VariantTableModel;

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
  using ItemModel::FilterColumn;

  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , LiteralValue >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( FilterColumn(1) ) , LiteralValue >() , "" );
}

void FilterExpressionTest::leftTerminalTest()
{
  using ItemModel::Expression::LeftTerminal;
  using ItemModel::FilterColumn;

  FilterColumn A(1);

  static_assert(  expressionMatchesGrammar< decltype( A ) , LeftTerminal >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , LeftTerminal >() , "" );
}

void FilterExpressionTest::rightTerminalTest()
{
  using ItemModel::Expression::RightTerminal;
  using ItemModel::FilterColumn;
  using Like = ItemModel::LikeExpression;

  FilterColumn A(1);

  static_assert( !expressionMatchesGrammar< decltype( A ) , RightTerminal >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , RightTerminal >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( Like("A?") ) , RightTerminal >() , "" );
}

void FilterExpressionTest::comparisonTest()
{
  using ItemModel::Expression::Comparison;
  using ItemModel::FilterColumn;
  using Like = ItemModel::LikeExpression;

  FilterColumn A(1);
  FilterColumn B(2);

  // ==
  static_assert(  expressionMatchesGrammar< decltype( A == 25 ) , Comparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A == Like("25*") ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 == A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A == B ) , Comparison >() , "" );
  // !=
  static_assert(  expressionMatchesGrammar< decltype( A != 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A != Like("25*") ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 != A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A != B ) , Comparison >() , "" );
  // <
  static_assert(  expressionMatchesGrammar< decltype( A < 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A < Like("25*") ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 < A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A < B ) , Comparison >() , "" );
  // <=
  static_assert(  expressionMatchesGrammar< decltype( A <= 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A <= Like("25*") ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 <= A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A <= B ) , Comparison >() , "" );
  // >
  static_assert(  expressionMatchesGrammar< decltype( A > 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A > Like("25*") ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 > A ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A > B ) , Comparison >() , "" );
  // >=
  static_assert(  expressionMatchesGrammar< decltype( A >= 25 ) , Comparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A >= Like("25*") ) , Comparison >() , "" );
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
  using ItemModel::Expression::FilterExpressionGrammar;
  using ItemModel::FilterColumn;
  using Like = ItemModel::LikeExpression;

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
  using ItemModel::FilterColumn;

  FilterColumn A(1);
  QCOMPARE( boost::proto::_value()(A).columnIndex() , 1 );
}

void FilterExpressionTest::evalDataTest()
{
  using ItemModel::Expression::FilterEvalData;

  VariantTableModel model;
  model.populate(2,1);

  FilterEvalData data(&model, 1, Qt::CaseInsensitive);
  QCOMPARE(data.model(), &model);
  QCOMPARE(data.row(), 1);
  QVERIFY(data.caseSensitivity() == Qt::CaseInsensitive);
}

void FilterExpressionTest::comparisonEvalTest()
{
  using ItemModel::FilterColumn;
  using ItemModel::Expression::FilterEvalData;
  using ItemModel::Expression::ComparisonEval;
  using Like = ItemModel::LikeExpression;

  QModelIndex index;
  /*
   * Setup table model
   */
  VariantTableModel model;
  model.populate(1,1);
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("0A"));
  /*
   * Setup columns and data and transform
   */
  FilterColumn A(0);
  FilterEvalData data(&model, 0, Qt::CaseInsensitive);
  ComparisonEval eval;
  /*
   * Tests
   */
  // ==
  QVERIFY( !eval(A == 2 , 0, data) );
  QVERIFY( !eval(A == "A" , 0, data) );
  QVERIFY(  eval(A == "0A" , 0, data) );
  QVERIFY( !eval(A == Like("A") , 0, data) );
  QVERIFY(  eval(A == Like("?A") , 0, data) );
  // !=
  QVERIFY(  eval(A != 2 , 0, data) );
  QVERIFY(  eval(A != "A" , 0, data) );
  QVERIFY( !eval(A != "0A" , 0, data) );
  // <
  
  // <=
  
  // >
  
  // >=

  QFAIL("Not complete");
}

void FilterExpressionTest::filterEvalTest()
{
  using ItemModel::FilterColumn;
  using ItemModel::Expression::FilterEvalData;
  using ItemModel::Expression::FilterEval;
  using Like = ItemModel::LikeExpression;

  QModelIndex index;
  /*
   * Setup table model
   */
  VariantTableModel model;
  model.populate(1,1);
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("0A"));
  /*
   * Setup columns and data and transform
   */
  FilterColumn A(0);
  FilterEvalData data(&model, 0, Qt::CaseInsensitive);
  FilterEval eval;
  /*
   * Tests
   */
  // Only comparison
  QVERIFY(  eval(A == "0A" , 0, data) );
  // Comparison and &&
  QVERIFY(  eval(A == "0A" && A == Like("?A") , 0, data) );
  QVERIFY( !eval(A == "0A" && A == 2 , 0, data) );
  QVERIFY( !eval(A == "0A" && A == 2 && A == 3 , 0, data) );
  // Comparison and ||
  QVERIFY(  eval( (A == "0A") || (A == Like("?A")) , 0, data) );
  QVERIFY(  eval( (A == "0A") || (A == 2) , 0, data) );
  QVERIFY( !eval( (A == 1) || (A == 2) , 0, data) );
  QVERIFY(  eval( (A == "0A") || (A == 2) || (A == 3) , 0, data) );
  QVERIFY( !eval( (A == 1) || (A == 2) || (A == 3) , 0, data) );
  // Comparison and && and ||
  QVERIFY(  eval(A == "0A" || ((A == 1)&&(A == 2)) , 0, data) );
  QVERIFY(  eval(A == "0A" && ((A == 1)||(A == 2)) , 0, data) );


  QFAIL("Not complete");
}

void FilterExpressionTest::expressionCopyTest()
{
  using ItemModel::FilterColumn;
  using ItemModel::FilterExpression;

  QModelIndex index;
  FilterColumn col(0);
  /*
   * Setup table model
   */
  VariantTableModel model;
  model.populate(1,1);
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("0A"));
  /*
   * Default construct and set
   */
  FilterExpression filter;
  QVERIFY(filter.isNull());
  filter.setExpression(col == 25);
  QVERIFY(!filter.isNull());
  QVERIFY(!filter.eval(&model, 0));
  filter.setExpression(col == "0A");
  QVERIFY(filter.eval(&model, 0));
  /*
   * Copy
   */
  // Copy construct
  FilterExpression filter2 = filter;
  QVERIFY(!filter2.isNull());
  QVERIFY(filter2.eval(&model, 0));
  filter2.setExpression(col == 12);
  QVERIFY(!filter2.eval(&model, 0));
  QVERIFY(filter.eval(&model, 0));  // Check that original filter is untouched
  // Copy assign
  filter2 = filter;
  QVERIFY(!filter2.isNull());
  QVERIFY(filter2.eval(&model, 0));
  filter2.setExpression(col == 12);
  QVERIFY(!filter2.eval(&model, 0));
  QVERIFY(filter.eval(&model, 0));  // Check that original filter is untouched
  /*
   * Move
   */
  auto tempFilter31 = filter;
  auto tempFilter32 = filter;
  // Move construct
  FilterExpression filter3 = std::move(tempFilter31);
  QVERIFY(!filter3.isNull());
  QVERIFY(filter3.eval(&model, 0));
  filter3.setExpression(col == 12);
  QVERIFY(!filter3.eval(&model, 0));
  QVERIFY(filter.eval(&model, 0));  // Check that original filter is untouched
  // Move assign
  filter3 = std::move(tempFilter32);
  QVERIFY(!filter3.isNull());
  QVERIFY(filter3.eval(&model, 0));
  filter3.setExpression(col == 12);
  QVERIFY(!filter3.eval(&model, 0));
  QVERIFY(filter.eval(&model, 0));  // Check that original filter is untouched
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
