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

// #include <boost/proto/proto.hpp>

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
   * Each column stores a specific type:
   * ----------------------------
   * | String | int |
   * ----------------------------
   * |  ABC   |  3  |
   * ----------------------------
   */
  VariantTableModel model;
  model.populate(1,2);
  // Set string
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setData(index, "ABC");
  // Set int
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  model.setData(index, 3);
  // Check
  index = model.index(0, 0);
  QCOMPARE(model.data(index), QVariant("ABC"));
  index = model.index(0, 1);
  QCOMPARE(model.data(index), QVariant(3));
  /*
   * Setup columns and data and transform
   */
  FilterColumn strCol(0);
  FilterColumn intCol(1);
  FilterEvalData data(&model, 0, Qt::CaseInsensitive);
  ComparisonEval eval;
  /*
   * Tests
   */
  // == with string
  QVERIFY(  eval(strCol == "ABC" , 0, data) );
  QVERIFY(  eval(strCol == "abc" , 0, data) );
  QVERIFY( !eval(strCol == "A" , 0, data) );
  // == with int
  QVERIFY(  eval(intCol == 3 , 0, data) );
  QVERIFY( !eval(intCol == 5 , 0, data) );
  // Like
  QVERIFY( !eval(strCol == Like("A") , 0, data) );
  QVERIFY(  eval(strCol == Like("?B?") , 0, data) );
  QVERIFY(  eval(strCol == Like("?b?") , 0, data) );
  // != with string
  QVERIFY( !eval(strCol != "ABC" , 0, data) );
  QVERIFY( !eval(strCol != "abc" , 0, data) );
  QVERIFY(  eval(strCol != "A" , 0, data) );
  // != with int
  QVERIFY( !eval(intCol != 3 , 0, data) );
  QVERIFY(  eval(intCol != 5 , 0, data) );
  // < with string
  QVERIFY(  eval(strCol < "ZZZ" , 0, data) );
  QVERIFY( !eval(strCol < "AAA" , 0, data) );
  // < with int
  QVERIFY(  eval(intCol < 5 , 0, data) );
  QVERIFY( !eval(intCol < 2 , 0, data) );
  // <= with string
  QVERIFY(  eval(strCol <= "ZZZ" , 0, data) );
  QVERIFY(  eval(strCol <= "ABC" , 0, data) );
  QVERIFY( !eval(strCol <= "AAA" , 0, data) );
  // <= with int
  QVERIFY(  eval(intCol <= 5 , 0, data) );
  QVERIFY(  eval(intCol <= 3 , 0, data) );
  QVERIFY( !eval(intCol <= 2 , 0, data) );
  // > with string
  QVERIFY( !eval(strCol > "ZZZ" , 0, data) );
  QVERIFY(  eval(strCol > "AAA" , 0, data) );
  // > with int
  QVERIFY( !eval(intCol > 5 , 0, data) );
  QVERIFY(  eval(intCol > 2 , 0, data) );
  // >= with string
  QVERIFY( !eval(strCol >= "ZZZ" , 0, data) );
  QVERIFY(  eval(strCol >= "ABC" , 0, data) );
  QVERIFY(  eval(strCol >= "AAA" , 0, data) );
  // >= with int
  QVERIFY( !eval(intCol >= 5 , 0, data) );
  QVERIFY(  eval(intCol >= 3 , 0, data) );
  QVERIFY(  eval(intCol >= 2 , 0, data) );
}

void FilterExpressionTest::filterEvalTest()
{
  using ItemModel::FilterColumn;
  using ItemModel::Expression::FilterEvalData;
  using ItemModel::Expression::FilterEval;
  using Like = ItemModel::LikeExpression;

  QModelIndex index;
  /*
   * Setup table model:
   * ----------------------------
   * | Id | Name |
   * ----------------------------
   * | 1  | ABC  |
   * ----------------------------
   */
  VariantTableModel model;
  model.populate(1,2);
  // Set Id
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setData(index, 1);
  // Set Name
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  model.setData(index, "ABC");
  // Check
  index = model.index(0, 0);
  QCOMPARE(model.data(index), QVariant(1));
  index = model.index(0, 1);
  QCOMPARE(model.data(index), QVariant("ABC"));
  /*
   * Setup columns and data and transform
   */
  FilterColumn Id(0);
  FilterColumn Name(1);
  FilterEvalData data(&model, 0, Qt::CaseInsensitive);
  FilterEval eval;
  /*
   * Tests
   */
  // Only comparison
  QVERIFY(  eval(Id == 1 , 0, data) );
  QVERIFY(  eval(Id != 2 , 0, data) );
  // Comparison and &&
  QVERIFY(  eval(Id == 1 && Name == Like("?B?") , 0, data) );
  QVERIFY( !eval(Id == 1 && Name == "B" , 0, data) );
  QVERIFY( !eval(Id == 1 && Name == "B" && Name == "C" , 0, data) );
  // Comparison and ||
  QVERIFY(  eval( (Id == 2) || (Name == Like("A*")) , 0, data) );
  QVERIFY(  eval( (Id == 1) || (Name == "A") , 0, data) );
  QVERIFY( !eval( (Id == 2) || (Name == "A") , 0, data) );
  QVERIFY(  eval( (Id > 5) || (Name == "A") || (Name == "ABC") , 0, data) );
  QVERIFY( !eval( (Id > 5) || (Name == "A") || (Name == "B") , 0, data) );
  // Comparison and || and &&
  QVERIFY(  eval(Id > 10 || ((Id < 5)&&(Name == Like("?B?"))) , 0, data) );
  QVERIFY( !eval(Id > 0 && ((Name == "A")||(Name == "B")) , 0, data) );
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
  QVERIFY(!filter.eval(&model, 0, Qt::CaseInsensitive));
  filter.setExpression(col == "0A");
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));
  /*
   * Copy
   */
  // Copy construct
  FilterExpression filter2 = filter;
  QVERIFY(!filter2.isNull());
  QVERIFY(filter2.eval(&model, 0, Qt::CaseInsensitive));
  filter2.setExpression(col == 12);
  QVERIFY(!filter2.eval(&model, 0, Qt::CaseInsensitive));
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));  // Check that original filter is untouched
  // Copy assign
  filter2 = filter;
  QVERIFY(!filter2.isNull());
  QVERIFY(filter2.eval(&model, 0, Qt::CaseInsensitive));
  filter2.setExpression(col == 12);
  QVERIFY(!filter2.eval(&model, 0, Qt::CaseInsensitive));
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));  // Check that original filter is untouched
  /*
   * Move
   */
  auto tempFilter31 = filter;
  auto tempFilter32 = filter;
  // Move construct
  FilterExpression filter3 = std::move(tempFilter31);
  QVERIFY(!filter3.isNull());
  QVERIFY(filter3.eval(&model, 0, Qt::CaseInsensitive));
  filter3.setExpression(col == 12);
  QVERIFY(!filter3.eval(&model, 0, Qt::CaseInsensitive));
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));  // Check that original filter is untouched
  // Move assign
  filter3 = std::move(tempFilter32);
  QVERIFY(!filter3.isNull());
  QVERIFY(filter3.eval(&model, 0, Qt::CaseInsensitive));
  filter3.setExpression(col == 12);
  QVERIFY(!filter3.eval(&model, 0, Qt::CaseInsensitive));
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));  // Check that original filter is untouched
}

void FilterExpressionTest::expressionTest()
{
  using ItemModel::FilterColumn;
  using ItemModel::FilterExpression;
  using Like = ItemModel::LikeExpression;

  QModelIndex index;
  /*
   * Setup table model
   * Each column stores a specific type:
   * ----------------------------
   * | String | int |
   * ----------------------------
   * |  ABC   |  3  |
   * ----------------------------
   */
  VariantTableModel model;
  model.populate(1,2);
  // Set string
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setData(index, "ABC");
  // Set int
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  model.setData(index, 3);
  // Check
  index = model.index(0, 0);
  QCOMPARE(model.data(index), QVariant("ABC"));
  index = model.index(0, 1);
  QCOMPARE(model.data(index), QVariant(3));
  /*
   * Because boost::proto::deep_copy() changes somewhat types,
   * check with each valid operators.
   * (will validate that all compiles)
   */
  FilterExpression filter;
  FilterColumn strCol(0);
  FilterColumn intCol(1);
  // == with string
  filter.setExpression(strCol == "ABC");
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));
  filter.setExpression(strCol == "abc");
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));
  filter.setExpression(strCol == "a");
  QVERIFY(!filter.eval(&model, 0, Qt::CaseSensitive));
  // == with int
  filter.setExpression(intCol == 3);
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));
  filter.setExpression(intCol == 25);
  QVERIFY(!filter.eval(&model, 0, Qt::CaseInsensitive));
  // Like
  filter.setExpression(strCol == Like("?B?"));
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));
  QVERIFY(filter.eval(&model, 0, Qt::CaseSensitive));
  filter.setExpression(strCol == Like("?b?"));
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));
  QVERIFY(!filter.eval(&model, 0, Qt::CaseSensitive));
  // != with string
  filter.setExpression(strCol != "Z");
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));
  // != with int
  filter.setExpression(intCol != 3);
  QVERIFY(!filter.eval(&model, 0, Qt::CaseInsensitive));
  filter.setExpression(intCol != 25);
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));
  // <
  filter.setExpression(intCol < 5);
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));
  filter.setExpression(intCol < 2);
  QVERIFY(!filter.eval(&model, 0, Qt::CaseInsensitive));
  // <=
  filter.setExpression(intCol <= 5);
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));
  filter.setExpression(intCol <= 3);
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));
  filter.setExpression(intCol <= 2);
  QVERIFY(!filter.eval(&model, 0, Qt::CaseInsensitive));
  // >
  filter.setExpression(intCol > 5);
  QVERIFY(!filter.eval(&model, 0, Qt::CaseInsensitive));
  filter.setExpression(intCol > 2);
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));
  // >=
  filter.setExpression(intCol >= 5);
  QVERIFY(!filter.eval(&model, 0, Qt::CaseInsensitive));
  filter.setExpression(intCol >= 3);
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));
  filter.setExpression(intCol >= 2);
  QVERIFY(filter.eval(&model, 0, Qt::CaseInsensitive));
}

void FilterExpressionTest::expressionBenchmark()
{
  using ItemModel::FilterColumn;
  using ItemModel::Expression::FilterEvalData;
  using ItemModel::Expression::FilterEval;
  using Like = ItemModel::LikeExpression;

  QModelIndex index;
  /*
   * Setup table model:
   * ----------------------------
   * | Id | Name | Remarks
   * ----------------------------
   * | 1  | ABC  | 1234ABCD
   * ----------------------------
   */
  VariantTableModel model;
  model.populate(1,3);
  // Set Id
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setData(index, 1);
  // Set Name
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  model.setData(index, "ABC");
  // Set Name
  index = model.index(0, 2);
  QVERIFY(index.isValid());
  model.setData(index, "1234ABCD");
  // Check
  index = model.index(0, 0);
  QCOMPARE(model.data(index), QVariant(1));
  index = model.index(0, 1);
  QCOMPARE(model.data(index), QVariant("ABC"));
  index = model.index(0, 2);
  QCOMPARE(model.data(index), QVariant("1234ABCD"));
  /*
   * Setup columns and data and transform
   */
  FilterColumn id(0);
  FilterColumn name(1);
  FilterColumn rem(2);
  FilterEvalData data(&model, 0, Qt::CaseInsensitive);
  FilterEval eval;
  QBENCHMARK{
    QVERIFY( eval(id > 0 && (name == Like("?B?") && rem == Like("*2*b*c?")), 0, data) );
  }
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
