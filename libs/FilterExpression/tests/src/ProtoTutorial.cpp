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
#include "ProtoTutorial.h"
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

namespace proto = boost::proto;
namespace mpl = boost::mpl;

template<int I>
struct placeholder
{
};

const proto::terminal< placeholder<0> >::type _1 = {{}};
const proto::terminal< placeholder<1> >::type _2 = {{}};

struct CalculatorContext : proto::callable_context< const CalculatorContext >
{
  std::vector<double> args;

  typedef double result_type;

  template<int I>
  double operator()(proto::tag::terminal, const placeholder<I>) const
  {
    qDebug() << "Req arg[" << I << "]";
    return args[I];
  }
};

void ProtoTutorial::helloCalculator()
{
  CalculatorContext ctx;
  ctx.args.push_back(45);
  ctx.args.push_back(50);

  proto::display_expr( (_2 - _1) / _2 * 100 );

  double d = proto::eval( (_2 - _1) / _2 * 100 , ctx);
  qDebug() << "d: " << d;
}

template<typename Expr>
struct Calculator;

/*
 * Calculator grammar
 */
struct CalculatorGrammar;

struct CalculatorDouble : proto::terminal< proto::convertible_to<double> >
{
};

struct CalculatorPlaceholder1 : proto::terminal< placeholder<0> >
{
};

struct CalculatorPlaceholder2 : proto::terminal< placeholder<1> >
{
};

struct CalculatorTerminal : proto::or_< CalculatorDouble, CalculatorPlaceholder1, CalculatorPlaceholder2 >
{
};

struct CalculatorPlus : proto::plus< CalculatorGrammar, CalculatorGrammar >
{
};

struct CalculatorMinus : proto::minus< CalculatorGrammar, CalculatorGrammar >
{
};

struct CalculatorMultiplies : proto::multiplies< CalculatorGrammar, CalculatorGrammar >
{
};

struct CalculatorDivides : proto::divides< CalculatorGrammar, CalculatorGrammar >
{
};

struct CalculatorGrammar : proto::or_< 
                                  CalculatorTerminal ,
                                  CalculatorPlus ,
                                  CalculatorMinus ,
                                  CalculatorMultiplies ,
                                  CalculatorDivides
                                     >
{
};

// struct CalculatorGrammar : proto::or_<
//                                   proto::plus< CalculatorGrammar, CalculatorGrammar > ,
//                                   proto::minus< CalculatorGrammar, CalculatorGrammar > ,
//                                   proto::multiplies< CalculatorGrammar, CalculatorGrammar > ,
//                                   proto::divides< CalculatorGrammar, CalculatorGrammar > ,
//                                   proto::terminal< proto::_ >
//                                 >
// {
// };

/*
 * Calculator domain
 */
struct CalculatorDomain : proto::domain< proto::generator<Calculator> , CalculatorGrammar >
{
};

/*
 * Calculator arity
 */
struct CalcArity : proto::or_<
                    proto::when<
                      proto::terminal< placeholder<0> > ,
                      mpl::int_<1>()
                    > ,
                    proto::when<
                      proto::terminal< placeholder<1> > ,
                      mpl::int_<2>()
                    > ,
                    proto::when<
                      proto::terminal< proto::_ > , 
                      mpl::int_<0>()
                    > , 
                    proto::when<
                      proto::unary_expr< proto::_ , CalcArity > ,
                      CalcArity(proto::_child)
                    > ,
                    proto::when<
                      proto::binary_expr< proto::_, CalcArity, CalcArity > ,
                      mpl::max< CalcArity(proto::_left), CalcArity(proto::_right) >()
                    >
                   >
{
};


/*
 * Calculator expression wrapper
 */
template<typename Expr>
struct Calculator : proto::extends<Expr, Calculator<Expr>, CalculatorDomain>
{
  typedef proto::extends<Expr, Calculator<Expr>, CalculatorDomain> base_type;
  typedef double result_type;

  // Constructor
  Calculator(const Expr & expr = Expr())
   : base_type(expr)
  {
//     qDebug() << "Calculator::Calculator() , expr:";
//     proto::display_expr(expr);
//     qDebug() << "Calculator::Calculator() DONE";
  }

  BOOST_PROTO_EXTENDS_USING_ASSIGN(Calculator)

  static const int arity = boost::result_of<CalcArity(Expr)>::type::value;

  double operator()() const
  {
    BOOST_MPL_ASSERT_RELATION(0, ==, arity);

    qDebug() << "Calculator::operator()()";

    CalculatorContext ctx;

    return proto::eval(*this, ctx);
  }

  double operator()(double a) const
  {
    BOOST_MPL_ASSERT_RELATION(1, ==, arity);

    qDebug() << "Calculator::operator()(" << a << ")";

    CalculatorContext ctx;
    ctx.args.push_back(a);

    return proto::eval(*this, ctx);
  }

  double operator()(double a, double b) const
  {
    BOOST_MPL_ASSERT_RELATION(2, ==, arity);

    qDebug() << "Calculator::operator()(" << a << ", " << b << ")";

    CalculatorContext ctx;
    ctx.args.push_back(a);
    ctx.args.push_back(b);

    return proto::eval(*this, ctx);
  }

//   double operator()(double a = 0, double b = 0) const
//   {
// //     BOOST_MPL_ASSERT((proto::matches<Expr, CalculatorGrammar>));
//     static_assert(proto::matches<Expr, CalculatorGrammar>::value , "");
// 
//     qDebug() << "Calculator::operator()(" << a << ", " << b << ")";
// 
//     CalculatorContext ctx;
//     ctx.args.push_back(a);
//     ctx.args.push_back(b);
// 
//     return proto::eval(*this, ctx);
//   }
};

template<typename Expr>
void displayAndCheckExpression(const Expr & expr)
{
  const bool matches = proto::matches<Expr, CalculatorGrammar>::value;

  proto::display_expr(expr);
  if(matches){
    std::cout << "-----> matches ";
  }else{
    std::cout << "-----> does not match ";
  }
  std::cout << "the CalculatorGrammar" << std::endl;
}

void ProtoTutorial::calculatorWrapperMain()
{
  const Calculator< proto::terminal< placeholder<0> >::type > _a;
  const Calculator< proto::terminal< placeholder<1> >::type > _b;

//   displayAndCheckExpression( (_b - _a) / _b * 100 );

//   double x = ( (_b - _a) / _b * 100 )(45.0, 50.0);
//   qDebug() << "x: " << x;

  displayAndCheckExpression(_a+2);
  displayAndCheckExpression(_b+2);
  displayAndCheckExpression(_a+_b);

  double x = (_a + 2)(3.0);
  double y = (_b + 2)(0.0, 3.0);
  double z = (_a + _b)(2.0, 3.0);

  qDebug() << "x: " << x << " , y: " << y << " , z: " << z;
}

/*
 * Left most leaf
 */
struct LeftMostLeaf : proto::or_<
                        proto::when<
                          proto::terminal< proto::_ > ,
                          proto::_value
                        > ,
                        proto::when<
                          proto::_ ,
                          LeftMostLeaf( proto::_child0 )
                        >
                      >
{
};

void ProtoTutorial::leftMostLeafMain()
{
  proto::terminal<int>::type x = {12};

  proto::display_expr( x + 5 * x * 2 );
  LeftMostLeaf lml;
  int y = lml(x + 5 * x * 2);
  qDebug() << "y: " << y;
}

void ProtoTutorial::initTestCase()
{
}

void ProtoTutorial::cleanupTestCase()
{
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ProtoTutorial test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
