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
#include "Mdt/ItemModel/ParentModelColumn.h"

#include "Mdt/ItemModel/Expression/LeftTerminal.h"
#include "Mdt/ItemModel/Expression/RightTerminal.h"

#include "Mdt/ItemModel/Expression/FilterExpressionGrammar.h"
#include "Mdt/ItemModel/Expression/RelationFilterExpressionGrammar.h"
#include "Mdt/ItemModel/Expression/ComparisonEval.h"
#include "Mdt/ItemModel/Expression/FilterEval.h"
#include "Mdt/ItemModel/Expression/ParentModelEvalData.h"
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
using ItemModel::FilterColumn;
using ItemModel::ParentModelColumn;

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

void FilterExpressionTest::filterComparisonTest()
{
  using ItemModel::Expression::FilterComparison;
  using ItemModel::FilterColumn;
  using Like = ItemModel::LikeExpression;

  FilterColumn A(1);
  FilterColumn B(2);
  ParentModelColumn P(5);

  // Not a comparison
  static_assert( !expressionMatchesGrammar< decltype( A ) , FilterComparison >() , "" );
  // ==
  static_assert(  expressionMatchesGrammar< decltype( A == 25 ) , FilterComparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A == Like("25*") ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 == A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A == B ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A == P ) , FilterComparison >() , "" );
  // !=
  static_assert(  expressionMatchesGrammar< decltype( A != 25 ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A != Like("25*") ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 != A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A != B ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A != P ) , FilterComparison >() , "" );
  // <
  static_assert(  expressionMatchesGrammar< decltype( A < 25 ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A < Like("25*") ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 < A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A < B ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A < P ) , FilterComparison >() , "" );
  // <=
  static_assert(  expressionMatchesGrammar< decltype( A <= 25 ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A <= Like("25*") ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 <= A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A <= B ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A <= P ) , FilterComparison >() , "" );
  // >
  static_assert(  expressionMatchesGrammar< decltype( A > 25 ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A > Like("25*") ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 > A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A > B ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A > P ) , FilterComparison >() , "" );
  // >=
  static_assert(  expressionMatchesGrammar< decltype( A >= 25 ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A >= Like("25*") ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 >= A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A >= B ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( A >= P ) , FilterComparison >() , "" );
  // Invalid expressions
  static_assert( !expressionMatchesGrammar< decltype( 25 + A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A + 25 ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A + B ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 - A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A - 25 ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A - B ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 * A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A * 25 ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A * B ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 / A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A / 25 ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A / B ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 % A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A % 25 ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A % B ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 << A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A << 25 ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A << B ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 >> A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A >> 25 ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A >> B ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 || A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A || 25 ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A || B ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 && A ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A && 25 ) , FilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype(  A && B ) , FilterComparison >() , "" );
}

void FilterExpressionTest::filterExpressionGrammarTest()
{
  using ItemModel::Expression::FilterExpressionGrammar;
  using ItemModel::FilterColumn;
  using Like = ItemModel::LikeExpression;

  FilterColumn A(1);
  FilterColumn B(2);
  ParentModelColumn P(5);

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
  static_assert( !expressionMatchesGrammar< decltype( A > 25 && B == P ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A > 25 && B == "A" ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A > 25 && B == Like("*s") ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A > 25 && B == Like("*s") && B < 46 ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A > 25 && A < 50 && B == Like("*s") && B < 46 ) , FilterExpressionGrammar >() , "" );
  // OR
  static_assert( !expressionMatchesGrammar< decltype( A || 25 ) , FilterExpressionGrammar >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( (A == 25) || (B == P) ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (A == 25) || (B == 44) ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (A == 25) || (B == 44) || (B == 77) ) , FilterExpressionGrammar >() , "" );
  // AND , OR
  static_assert(  expressionMatchesGrammar< decltype( (A == 25) || (B == 26 && A < 50) ) , FilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (A == 25) && ((B == 26) || (A < 50)) ) , FilterExpressionGrammar >() , "" );
}

void FilterExpressionTest::relationFilterComparisonTest()
{
  using ItemModel::Expression::RelationFilterComparison;

  FilterColumn A(1);
  ParentModelColumn P(5);

  // ==
  static_assert(  expressionMatchesGrammar< decltype( A == 25 ) , RelationFilterComparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A == P ) , RelationFilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( P == 25 ) , RelationFilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( P == A ) , RelationFilterComparison >() , "" );
  // !=
  static_assert(  expressionMatchesGrammar< decltype( A != 25 ) , RelationFilterComparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A != P ) , RelationFilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( P != 25 ) , RelationFilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( P != A ) , RelationFilterComparison >() , "" );
  // <
  static_assert(  expressionMatchesGrammar< decltype( A < 25 ) , RelationFilterComparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A < P ) , RelationFilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( P < 25 ) , RelationFilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( P < A ) , RelationFilterComparison >() , "" );
  // <=
  static_assert(  expressionMatchesGrammar< decltype( A <= 25 ) , RelationFilterComparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A <= P ) , RelationFilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( P <= 25 ) , RelationFilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( P <= A ) , RelationFilterComparison >() , "" );
  // >
  static_assert(  expressionMatchesGrammar< decltype( A > 25 ) , RelationFilterComparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A > P ) , RelationFilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( P > 25 ) , RelationFilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( P > A ) , RelationFilterComparison >() , "" );
  // >=
  static_assert(  expressionMatchesGrammar< decltype( A >= 25 ) , RelationFilterComparison >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A >= P ) , RelationFilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( P >= 25 ) , RelationFilterComparison >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( P >= A ) , RelationFilterComparison >() , "" );

}

void FilterExpressionTest::relationExpressionGrammarTest()
{
  using ItemModel::Expression::RelationFilterExpressionGrammar;

  FilterColumn A(1);
  FilterColumn B(1);
  ParentModelColumn P(5);

  // AND
  static_assert(  expressionMatchesGrammar< decltype( A > 25 && B == P ) , RelationFilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A > 25 && B == "A" ) , RelationFilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( A > 25 && A < 50 && B == P && B < 46 ) , RelationFilterExpressionGrammar >() , "" );
  // OR
  static_assert(  expressionMatchesGrammar< decltype( (A == 25) || (B == P) ) , RelationFilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (A == 25) || (B == 44) ) , RelationFilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (A == P) || (B == 44) || (B == 77) ) , RelationFilterExpressionGrammar >() , "" );
  // AND , OR
  static_assert(  expressionMatchesGrammar< decltype( (A == P) || (B == 26 && A < 50) ) , RelationFilterExpressionGrammar >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( (A == P) && ((B == 26) || (A < 50)) ) , RelationFilterExpressionGrammar >() , "" );
}

/*
 * Runtime tests
 */

void FilterExpressionTest::filterColumnTest()
{
  FilterColumn A(1);
  QCOMPARE( boost::proto::_value()(A).columnIndex() , 1 );
}

void FilterExpressionTest::parentModelColumnTest()
{
  ParentModelColumn P(3);
  QCOMPARE( boost::proto::_value()(P).columnIndex() , 3 );
}

void FilterExpressionTest::parentModelEvalDataTest()
{
  using ItemModel::Expression::ParentModelEvalData;

  VariantTableModel model;
  model.resize(2, 2);
  /*
   * Default constructed
   */
  ParentModelEvalData data1;
  QVERIFY(data1.isNull());
  QVERIFY(data1.model() == nullptr);
  QCOMPARE(data1.row(), -1);
  /*
   * Construct valid
   */
  ParentModelEvalData data2(&model, 1);
  QVERIFY(!data2.isNull());
  QVERIFY(data2.model() == &model);
  QCOMPARE(data2.row(), 1);
}

void FilterExpressionTest::evalDataTest()
{
  using ItemModel::Expression::FilterEvalData;
  using ItemModel::Expression::ParentModelEvalData;

  VariantTableModel parentModel;
  parentModel.populate(3,1);
  VariantTableModel model;
  model.populate(2,1);

  /*
   * Construct for filter
   */
  FilterEvalData data(&model, 1, Qt::CaseInsensitive);
  QCOMPARE(data.model(), &model);
  QCOMPARE(data.row(), 1);
  QVERIFY(data.parentModel() == nullptr);
  QCOMPARE(data.parentModelRow(), -1);
  QVERIFY(data.caseSensitivity() == Qt::CaseInsensitive);
  /*
   * Construct for relation filter
   */
  FilterEvalData rData(&model, 1, ParentModelEvalData(&parentModel, 2), Qt::CaseSensitive);
  QCOMPARE(rData.model(), &model);
  QCOMPARE(rData.row(), 1);
  QVERIFY(rData.parentModel() == &parentModel);
  QCOMPARE(rData.parentModelRow(), 2);
  QVERIFY(rData.caseSensitivity() == Qt::CaseSensitive);
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
  model.resize(1, 2);
  model.populateColumn(0, {"ABC"});
  model.populateColumn(1, {3});
  // Check
  QCOMPARE(model.data(0, 0), QVariant("ABC"));
  QCOMPARE(model.data(0, 1), QVariant(3));
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

//   QModelIndex index;
  /*
   * Setup table model:
   * ----------------------------
   * | Id | Name |
   * ----------------------------
   * | 1  | ABC  |
   * ----------------------------
   */
  VariantTableModel model;
  model.resize(1, 2);
  model.populateColumn(0, {1});
  model.populateColumn(1, {"ABC"});
  // Check
  QCOMPARE(model.data(0, 0), QVariant(1));
  QCOMPARE(model.data(0, 1), QVariant("ABC"));
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

void FilterExpressionTest::relationFilterEvalTest()
{
  using ItemModel::Expression::FilterEvalData;
  using ItemModel::Expression::ParentModelEvalData;
  using ItemModel::Expression::FilterEval;

  /*
   * Setup parent table model
   * ----------------------------
   * | Id | CODE |
   * ----------------------------
   * | 2  |  B   |
   * ----------------------------
   */
  VariantTableModel parentModel;
  parentModel.resize(1, 2);
  parentModel.populateColumn(0, {2});
  parentModel.populateColumn(1, {"B"});
  /*
   * Setup (child) table model
   * ----------------------------
   * | Id | CODE |
   * ----------------------------
   * | 1  |  A   |
   * ----------------------------
   * | 2  |  B   |
   * ----------------------------
   * | 3  |  C   |
   * ----------------------------
   */
  VariantTableModel model;
  model.resize(3, 2);
  model.populateColumn(0, {1,2,3});
  model.populateColumn(1, {"A","B","C"});
  /*
   * Setup eval and columns
   */
  FilterEvalData row0EvalData(&model, 0, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive);
  FilterEvalData row1EvalData(&model, 1, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive);
  FilterEvalData row2EvalData(&model, 2, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive);
  FilterEval eval;
  ParentModelColumn parentId(0);
  ParentModelColumn parentCode(0);
  FilterColumn id(0);
  FilterColumn code(1);
  /*
   * Tests
   */
  // ==
  QVERIFY( !eval(id == parentId, 0, row0EvalData) );
  QVERIFY(  eval(id == parentId, 0, row1EvalData) );
  // !=
  QVERIFY(  eval(id != parentId, 0, row0EvalData) );
  QVERIFY( !eval(id != parentId, 0, row1EvalData) );
  // <
  QVERIFY(  eval(id < parentId, 0, row0EvalData) );
  QVERIFY( !eval(id < parentId, 0, row1EvalData) );
  // <=
  QVERIFY(  eval(id <= parentId, 0, row0EvalData) );
  QVERIFY(  eval(id <= parentId, 0, row1EvalData) );
  QVERIFY( !eval(id <= parentId, 0, row2EvalData) );
  // >
  QVERIFY( !eval(id > parentId, 0, row1EvalData) );
  QVERIFY(  eval(id > parentId, 0, row2EvalData) );
  // >=
  QVERIFY( !eval(id >= parentId, 0, row0EvalData) );
  QVERIFY(  eval(id >= parentId, 0, row1EvalData) );
  QVERIFY(  eval(id >= parentId, 0, row2EvalData) );
  // Comparison and || and &&
  QVERIFY(  eval((id == parentId) || (code == parentCode && id == parentId), 0, row1EvalData) );
  QVERIFY( !eval((id == parentId) && (code < parentCode || id < parentId), 0, row1EvalData) );
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
  model.resize(1, 2);
  model.populateColumn(0, {"ABC"});
  model.populateColumn(1, {3});
  // Check
  QCOMPARE(model.data(0, 0), QVariant("ABC"));
  QCOMPARE(model.data(0, 1), QVariant(3));
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

void FilterExpressionTest::expressionRelationTest()
{
  using ItemModel::FilterExpression;
  using ItemModel::Expression::ParentModelEvalData;

  /*
   * Setup parent table model
   * ----------------------------
   * | Id | CODE |
   * ----------------------------
   * | 2  |  B   |
   * ----------------------------
   */
  VariantTableModel parentModel;
  parentModel.resize(1, 2);
  parentModel.populateColumn(0, {2});
  parentModel.populateColumn(1, {"B"});
  /*
   * Setup (child) table model
   * ----------------------------
   * | Id | CODE |
   * ----------------------------
   * | 1  |  A   |
   * ----------------------------
   * | 2  |  B   |
   * ----------------------------
   * | 3  |  C   |
   * ----------------------------
   */
  VariantTableModel model;
  model.resize(3, 2);
  model.populateColumn(0, {1,2,3});
  model.populateColumn(1, {"A","B","C"});
  /*
   * Because boost::proto::deep_copy() changes somewhat types,
   * check with each valid operators.
   * (will validate that all compiles)
   */
  FilterExpression filter;
  ParentModelColumn parentId(0);
  ParentModelColumn parentCode(0);
  FilterColumn id(0);
  FilterColumn code(1);
  // ==
  filter.setRelationExpression(id == parentId);
  QVERIFY( !filter.eval(&model, 0, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  QVERIFY(  filter.eval(&model, 1, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  // !=
  filter.setRelationExpression(id != parentId);
  QVERIFY(  filter.eval(&model, 0, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  QVERIFY( !filter.eval(&model, 1, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  // <
  filter.setRelationExpression(id < parentId);
  QVERIFY(  filter.eval(&model, 0, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  QVERIFY( !filter.eval(&model, 1, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  QVERIFY( !filter.eval(&model, 2, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  // <=
  filter.setRelationExpression(id <= parentId);
  QVERIFY(  filter.eval(&model, 0, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  QVERIFY(  filter.eval(&model, 1, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  QVERIFY( !filter.eval(&model, 2, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  // >
  filter.setRelationExpression(id > parentId);
  QVERIFY( !filter.eval(&model, 0, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  QVERIFY( !filter.eval(&model, 1, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  QVERIFY(  filter.eval(&model, 2, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  // >=
  filter.setRelationExpression(id >= parentId);
  QVERIFY( !filter.eval(&model, 0, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  QVERIFY(  filter.eval(&model, 1, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
  QVERIFY(  filter.eval(&model, 2, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive) );
}

void FilterExpressionTest::expressionBenchmark()
{
  using ItemModel::FilterColumn;
  using ItemModel::Expression::FilterEvalData;
  using ItemModel::Expression::ParentModelEvalData;
  using ItemModel::Expression::FilterEval;
  using Like = ItemModel::LikeExpression;

  /*
   * Setup parent table model
   * -------------
   * | Id | CODE |
   * -------------
   * | 1  |  B   |
   * -------------
   */
  VariantTableModel parentModel;
  parentModel.resize(1, 2);
  parentModel.populateColumn(0, {1});
  parentModel.populateColumn(1, {"B"});
  /*
   * Setup table model:
   * ----------------------------
   * | Id | Name | Remarks
   * ----------------------------
   * | 1  | ABC  | 1234ABCD
   * ----------------------------
   */
  VariantTableModel model;
  model.resize(1, 3);
  model.populateColumn(0, {1});
  model.populateColumn(1, {"ABC"});
  model.populateColumn(2, {"1234ABCD"});
  /*
   * Setup columns and data and transform
   */
  FilterColumn id(0);
  ParentModelColumn parentId(0);
  FilterColumn name(1);
  FilterColumn rem(2);
  FilterEvalData data(&model, 0, ParentModelEvalData(&parentModel, 0), Qt::CaseInsensitive);
  FilterEval eval;
  QBENCHMARK{
    QVERIFY( eval(id == parentId && (name == Like("?B?") && rem == Like("*2*b*c?")), 0, data) );
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
