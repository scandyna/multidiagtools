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
#include <QDebug>
#include <iostream>
#include <vector>
#include <boost/proto/proto.hpp>
#include <boost/typeof/std/ostream.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/int.hpp>
#include <boost/core/demangle.hpp>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <QString>
#include <QVariant>
#include <string>
#include <tuple>

namespace proto = boost::proto;
namespace mpl = boost::mpl;

/*******************************************************************************
 * Terminal type discriminator here
 ******************************************************************************/
/// struct variable_tag {};

/*******************************************************************************
 * Last step, we have to redefine _x to be an analytical_expression
 ******************************************************************************/
/// analytical_expression< boost::proto::terminal< variable_tag >::type > const _x;

/*
 * Mains
 */
// int main()
// {
///   check_for_match( 3*_x + 1.f/_x);
// }

/// Mais: possible d'assigner une valeur à _x ? semblerait que pas directement

/// Evt: struct avec un setter ? (PAS de constructeur (int), pas d'opératuer() ,...
/// See steo1-01

struct ColumnIndexData
{
  ColumnIndexData(int i, const QString & tbl = QString())
   : index(i) , table(tbl)
  {
    std::cout << "ColumnIndexData(" << i << ", " << tbl.toStdString() << ")" << std::endl;
  }

  int index;
  QString table;
};

struct ColumnIndexTag
{
  ColumnIndexTag(const ColumnIndexData & d)
   : data(d)
  {
  }

  ColumnIndexData data;
//   int value;
//   QString table;
};

/*
 * Column index placeholder
 */
struct ColumnIndex : proto::terminal<ColumnIndexTag>
{
};

using ColumnIndexType = const ColumnIndex::type;

/*
 * Grammar
 */
struct FilterExpressionGrammar;

// struct MatchValue : proto::or_<
//                         proto::terminal<int>
//                       >
// {
// };

/**
 * NOTE: add notion of table ?  '.' cannot be overloaded. Evt: subscript ? client[Id] == 25 && Address[Street] == "Street 45"
 */

struct MatchValue : proto::or_<
                        proto::terminal< proto::convertible_to<int> > ,
                        proto::terminal<QString>
                      >
{
};

struct MatchEqual : proto::or_<
                        proto::equal_to< ColumnIndex, MatchValue > ,
                        proto::equal_to< FilterExpressionGrammar, FilterExpressionGrammar >
                      >
{
};

struct Compare : proto::or_<
                      MatchEqual
                    >
{
};

struct And : proto::logical_and< FilterExpressionGrammar, FilterExpressionGrammar >
{
};


// struct Terminal : proto::or_<
//                       ColumnIndex ,
//                       MatchValue
//                     >
// {
// };

struct FilterExpressionGrammar : proto::or_<
                                      Compare ,
                                      And
                                    >
{
};

struct MatchEqualAction : proto::callable
{
  typedef void result_type;

  template<typename Ci, typename V>
  void operator()(const Ci & ci, const V & v) const
  {
    std::cout << "MatchEqualAction: " << proto::value(ci).data.table.toStdString() << ".column " << proto::value(ci).data.index << " == " << proto::value(v).toStdString() << std::endl;
  }
};

struct CompareAction : proto::or_<
                            proto::when<
                              proto::equal_to< ColumnIndex, MatchValue > ,
                              proto::call<MatchEqualAction(proto::_left, proto::_right)>
                            >
                          >
{
};

struct FilterExpressionEval : proto::or_<
                                    proto::when<
                                      Compare ,
                                      CompareAction
                                    >
                                  >
{
};

template<typename Expr>
void displayAndCheckExpression(const Expr & expr)
{
//   static_assert(proto::matches<Expr, FilterExpressionGrammar>::value, "");  // NOTE: does never match with QVariant !
  const bool matches = proto::matches<Expr, FilterExpressionGrammar>::value;

  proto::display_expr(expr);
  if(matches){
    std::cout << "-----> matches ";
  }else{
    std::cout << "-----> does not match ";
  }
  std::cout << "the FilterExpressionGrammar" << std::endl;
}

template<typename Expr>
void evaluateExpression(const Expr expr)
{
  std::cout << "***** Evaluate ***********" << std::endl;
  proto::display_expr(expr);
  FilterExpressionEval eval;
  eval(expr);
  std::cout << "**************************" << std::endl;
}

int getFieldIndex(const QString & fieldName)
{
  return fieldName.toInt();
}

void FilterExpressionTest::sandbox()
{
  ColumnIndexType Name = { ColumnIndexData(2, "Client") };
  ColumnIndexType DynField = { ColumnIndexData( getFieldIndex("5") ) };

//   std::cout << "Type of ColumnIndexType: " << boost::core::demangle( typeid(ColumnIndexType).name ) << std::endl;

  displayAndCheckExpression( Name == QString("PS é ö") );
  evaluateExpression( Name == QString("PS é ö") );
  displayAndCheckExpression( 25 == Name );
  displayAndCheckExpression( Name == 25 && Name < 15 );
  displayAndCheckExpression( Name == 25 && DynField == 10 );
}

/*
 * Voir:
 * Name == 25 <- Ok
 * 25 == Name <- Nok
 * Name == 25 && Name < 15  <- Ok
 * (Name == 25) && Name < 15  <- Ok
 * Name == (25 && Name) < 15  <- Nok
 * Name == Remarks <- Nok
 * ....
 */

void FilterExpressionTest::initTestCase()
{
}

void FilterExpressionTest::cleanupTestCase()
{
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

  return QTest::qExec(&test, argc, argv);
}
