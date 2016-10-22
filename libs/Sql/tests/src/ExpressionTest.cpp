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
#include <QSqlDatabase>
#include <boost/proto/matches.hpp>
#include <boost/proto/literal.hpp>

namespace Sql = Mdt::Sql;

/*
 * Terminal type used in tests
 */
struct TestTerminalTag{};
using TestTerminal = const Sql::Expression::TableFieldTerminal<TestTerminalTag>;

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

/// \todo Should add grammarLiteralValueTest to check int, float, double, const char*, QDate, ...
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
  static_assert(  expressionMatchesGrammar< decltype( A ) , LeftTerminal<TestTerminalTag> >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , LeftTerminal<TestTerminalTag> >() , "" );
  // RightTerminal
  static_assert(  expressionMatchesGrammar< decltype( A ) , RightTerminal<TestTerminalTag> >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , RightTerminal<TestTerminalTag> >() , "" );
}

void ExpressionTest::comparisonTest()
{
  using Sql::Expression::CompareEquality;
  using Sql::Expression::LeftTerminal;
  using Sql::Expression::RightTerminal;
  using Sql::TableName;
  using Sql::FieldName;

  TestTerminal A(TableName("A"), FieldName("a"));

  static_assert(  expressionMatchesGrammar< decltype( A == 25 ) , CompareEquality<TestTerminalTag> >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( 25 == A ) , CompareEquality<TestTerminalTag> >() , "" );

}

/*
 * Rutime tests
 */



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
