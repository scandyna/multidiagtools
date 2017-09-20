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
#include "Mdt/CoreApplication.h"
#include "Mdt/FilterExpression/LiteralValue.h"
// #include "Mdt/FilterExpression/LikeExpressionTerminal.h"
// #include <QString>
// #include <QVariant>
#include <boost/proto/matches.hpp>
#include <boost/proto/literal.hpp>
#include <boost/proto/transform/arg.hpp>
// #include <type_traits>
// #include <typeinfo>

#include <QDebug>

namespace FilterExpression = Mdt::FilterExpression;

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

/// \todo Should also check int, float, double, const char*, QDate, ...
/**
 * \note How to distiguisch :"some string" or "Date-Time" formatted string ??
 *  - Check while generating SQL ??
 *  - Simply request user to be explicit (f.ex. QDate("xx.yy.zz") ) ?
 *  - Define some TAG for date/time/datetime literal terminal ?
 *   -> Ex: Date("xx.yy.zz")
 * --> QDate/etc.. seems to be the simplest..
 */

void FilterExpressionTest::literalValueTest()
{
  using FilterExpression::LiteralValue;

  struct MyType{};
  boost::proto::terminal<MyType>::type _x{{}};

  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(25) ) , LiteralValue >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit("ID44") ) , LiteralValue >() , "" );
  static_assert(  expressionMatchesGrammar< decltype( boost::proto::lit(u8"éèj") ) , LiteralValue >() , "" );
  static_assert( !expressionMatchesGrammar< decltype( _x ) , LiteralValue >() , "" );
}


/*
 * Runtime tests
 */

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  FilterExpressionTest test;

  return QTest::qExec(&test, argc, argv);
}
