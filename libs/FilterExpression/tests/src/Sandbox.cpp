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
#include "Sandbox.h"
#include "Mdt/Application.h"
#include <QDebug>

#include <iostream>
#include <vector>
#include <boost/proto/proto.hpp>
#include <boost/typeof/std/ostream.hpp>
#include <boost/mpl/assert.hpp>

#include <boost/core/demangle.hpp>

#include <type_traits>
#include <typeinfo>
#include <utility>
#include <iostream>

namespace proto = boost::proto;

/*
 * Hello world
 */

proto::terminal< std::ostream & >::type cout_ = { std::cout };

template<typename Expr>
void evaluate( const Expr & expr)
{
  proto::default_context ctx;
  proto::eval(expr, ctx);
}

void Sandbox::helloWorld()
{
  evaluate( cout_ << "hello" << ',' << " world\n" );
}

/*
 * Calculator
 */

// Placeholder type
template<int I>
struct placeholder
{
};

// Placeholder terminals
proto::terminal< placeholder<0> >::type const _1 = {{}};
proto::terminal< placeholder<1> >::type const _2 = {{}};

struct CalculatorContext : proto::callable_context<CalculatorContext const>
{
  // Values to replace the placeholders
  std::vector<double> args;

  // Result type of calculator
  typedef double result_type;

  // Handle placeholders
  template<int I>
  double operator()(proto::tag::terminal, placeholder<I>) const
  {
    return args[I];
  }
};

void Sandbox::helloCalculator()
{
  CalculatorContext ctx;
  ctx.args.push_back(45); // Value of _1
  ctx.args.push_back(50); // Value of _2

  // Create and evaluate a expression
  double d = proto::eval( (_2 - _1) / _2 * 100 , ctx );
  qDebug() << "d: " << d;
}

// Forward declare expression wrapper
template<typename Expr>
struct Calculator;

// Calculator grammar
struct CalculatorGrammar : proto::or_<
          proto::plus< CalculatorGrammar, CalculatorGrammar > ,
          proto::minus< CalculatorGrammar, CalculatorGrammar > ,
          proto::multiplies< CalculatorGrammar, CalculatorGrammar > ,
          proto::divides< CalculatorGrammar, CalculatorGrammar > ,
          proto::terminal< proto::_ >
        >
{
};

// Calculator domain
struct CalculatorDomain : proto::domain< proto::generator<Calculator> , CalculatorGrammar >
{
};

// Expression wrapper
template<typename Expr>
struct Calculator : proto::extends<Expr, Calculator<Expr>, CalculatorDomain>
{
  typedef proto::extends<Expr, Calculator<Expr>, CalculatorDomain> base_type;

  Calculator(const Expr & expr = Expr())
   : base_type(expr)
  {
  }

  typedef double result_type;

  // Overload operator() to invoke proto::eval() with our CalculatorContext
  double operator()(double a1 = 0, double a2 = 0) const
  {
    BOOST_MPL_ASSERT((proto::matches<Expr, CalculatorGrammar>));

    CalculatorContext ctx;
    ctx.args.push_back(a1);
    ctx.args.push_back(a2);

    return proto::eval(*this, ctx);
  }
};

// Placeholders in Calculator domain
Calculator< proto::terminal< placeholder<0> >::type > const _10;
Calculator< proto::terminal< placeholder<1> >::type > const _20;

void Sandbox::calculatorSandbox()
{
  double d = ((_20 - _10) / _20 * 100)(45.0, 50.0);

  boost::proto::display_expr( ((_20 - _10) / _20 * 100) );
  qDebug() << "d: " << d;
}



template<int I>
struct ColumnPlaceHolder
{
};

// template<int I>
// using Column = typename proto::terminal< ColumnPlaceHolder<I> >::type const;
// #define Col(i) Column<i>{{}}

struct ColumnIndex
{
  ColumnIndex()
   : m_index(-1)
  {
    qDebug() << "ColumnIndex::ColumnIndex()";
  }

  ColumnIndex(int i)
   : m_index(i)
  {
    qDebug() << "ColumnIndex::ColumnIndex(" << i << ")";
  }

  int m_index;
};


/*
 * Counter...
 */

// template<int N>
// struct flag
// {
//   friend
//   constexpr int adl_flag(flag<N>);
// };
// 
// template<int N, int = adl_flag(flag<N>{})>
// constexpr int reader(int, flag<N>)
// {
//   return N;
// }
// 
// template<int N>
// constexpr int reader(float, flag<N>, int R = reader(0, flag<N-1>{}))
// {
//   return R;
// }
// 
// constexpr int reader(float, flag<0>)
// {
//   return 0;
// }
// 
// template<int N>
// struct writer
// {
//   friend
//   constexpr int adl_flag(flag<N>)
//   {
//     return N;
//   }
//   static constexpr int value = N;
// };
// 
// template<int N = 1>
// constexpr int next(int R = writer<reader(0, flag<32>{}) + N>::value)
// {
//   return R;
// }

/*
 * Counter try 2 .....
 */

/*
struct MyCounter
{
  constexpr MyCounter() = default;

  constexpr int getNext() const
  {
    ++m_idx;
    return m_idx;
  }

  mutable int m_idx = 0;
};*/


/*
 * Counter..
 * http://b.atch.se/posts/constexpr-counter/
 */

// template<int N>
// struct flag
// {
//   friend constexpr int adl_flag(flag<N>);
// };
// 
// template<int N>
// struct writer
// {
//   friend constexpr int adl_flag(flag<N>)
//   {
//     return N;
//   }
// 
//   static constexpr int value = N;
// };
// 
// template<int N, int = adl_flag(flag<N>{})>
// constexpr int reader(int, flag<N>)
// {
//   return N;
// }
// 
// template<int N>
// constexpr int reader(float, flag<N>, int R = reader(0, flag<N-1>{}))
// {
//   return R;
// }
// 
// constexpr int reader(float, flag<0>)
// {
//   return 0;
// }
// 
// template<int N = 1>
// constexpr int next(int R = writer<reader(0, flag<32>{}) + N>::value)
// {
//   return R;
// }

void Sandbox::staticCounter()
{
//   constexpr int a = next();
//   constexpr int b = next();
//   constexpr int c = next();
// 
//   qDebug() << "a: " << a << " , b: " << b << " , c: " << c;
}

/*
 * Counter...
 * http://stackoverflow.com/questions/6166337/does-c-support-compile-time-counters/6174263#6174263
 */

/*
 * This type returns a number trough function lookup
 */
template<int N>
struct constant_index : std::integral_constant<int, N>
{
};

/*
 * Default falback counter_crumb() function
 */
template<typename Id, int Rank, int Acc>
constexpr constant_index<Acc> counter_crumb(Id, constant_index<Rank>, constant_index<Acc>)
{
  return {};
}

#define COUNTER_READ_CRUMB( TAG, RANK, ACC ) counter_crumb( TAG(), constant_index<RANK>(), constant_index<ACC>() )
#define COUNTER_READ( TAG ) COUNTER_READ_CRUMB( TAG, 1, \
                            COUNTER_READ_CRUMB( TAG, 2, \
                            COUNTER_READ_CRUMB( TAG, 4, \
                            COUNTER_READ_CRUMB( TAG, 8, \
                            COUNTER_READ_CRUMB( TAG, 16, \
                            COUNTER_READ_CRUMB( TAG, 32, \
                            COUNTER_READ_CRUMB( TAG, 64, \
                            COUNTER_READ_CRUMB( TAG, 128, 0 ) ) ) ) ) ) ) )

#define COUNTER_INC( TAG ) constant_index< COUNTER_READ( TAG ) + 1 > \
                           constexpr counter_crumb( TAG, constant_index< ( COUNTER_READ(TAG) + 1 ) & ~COUNTER_READ(TAG) > ,\
                                                         constant_index< ( COUNTER_READ(TAG) + 1 ) &  COUNTER_READ(TAG) > )\
                           { return {}; }

/*
 * Evaluate the counter by finding the last defined overload.
 * Each function, when defined, alters the lookup sequence for lower functions.
 */
#define COUNTER_READ_2( TAG ) counter_crumb( TAG() , constant_index<1>() , constant_index< \
                              counter_crumb( TAG() , constant_index<2>() , constant_index<0>() ) \
                              >() )

/*
 * Define a single new function with place-value equal to the bit flipped to 1 by the increment operation.
 * This is the lowest-magnitude function yet undefined in the current context of defined higher-magnitude function.
 *
 * Define a single counter_crumb() function....
 */
#define COUNTER_INC_2( TAG ) \
constexpr constant_index<COUNTER_READ_2(TAG) + 1> counter_crumb( TAG , \
                                                                 constant_index< ( COUNTER_READ_2(TAG) + 1 ) & ~COUNTER_READ_2(TAG) > , \
                                                                 constant_index< ( COUNTER_READ_2(TAG) + 1 ) & COUNTER_READ_2(TAG) > ) \
{ \
  return {}; \
}

#define COUNTER_LINK_NAMESPACE( NS ) using NS::counter_crumb;

/*
 * All above works, but COUNTER_INC() or COUNTER_INC_2() defines a new counter_crumb() function,
 * which not works inside a function.
 */

/*
 * Counter value, defined by ... lookup
 */
template<int N>
struct CounterConstant : std::integral_constant<int, N>
{
};

/*
 * Default fallback
 */
template<typename Tag, int Rank, int Acc>
struct CounterCrumb
{
};

struct MyAdderClass
{
  struct counter{};
  
  void add()
  {
//     COUNTER_INC_2(counter);
//     addImpl<COUNTER_READ_2(counter)>();
  }

  template<int Idx>
  void addImpl()
  {
    qDebug() << "Add " << Idx;
  }
};

  struct my_cnt{};
  
  const int a = COUNTER_READ_2(my_cnt);
  
  COUNTER_INC_2(my_cnt);
  
  const int b = COUNTER_READ(my_cnt);

void Sandbox::staticCounter2()
{
  qDebug() << "a: " << a << " , b: " << b;
  
  MyAdderClass ac1;
  
  ac1.add();
  ac1.add();
}

/*
 * Another try for a static counter..
 */

template<int N>
struct CSInt : public CSInt<N-1>
{
  static constexpr const int value = N;
};

template<>
struct CSInt<0>
{
  static constexpr const int value = 0;
};

constexpr CSInt<0> cs_counter(CSInt<0>);

#define COUNTER_READ_3 decltype(cs_counter(CSInt<5>{}))::value

#define COUNTER_INC_3 constexpr CSInt< COUNTER_READ_3 + 1 > cs_counter(CSInt< COUNTER_READ_3 + 1 >);

void Sandbox::staticCounter3()
{
  qDebug() << "CSInt<5>: " << CSInt<5>::value;
  qDebug() << "decltype(cs_counter(CSInt<5>{}))::value: " << decltype(cs_counter(CSInt<5>{}))::value;
  qDebug() << "decltype(cs_counter(CSInt<5>{}))::value: " << decltype(cs_counter(CSInt<5>{}))::value;
  
  qDebug() << "Cnt: " << COUNTER_READ_3;
  static_assert( COUNTER_READ_3 == 0, "" );
  COUNTER_INC_3;
  qDebug() << "Cnt: " << COUNTER_READ_3;
  COUNTER_INC_3;
  qDebug() << "Cnt: " << COUNTER_READ_3;
  COUNTER_INC_3;
  qDebug() << "Cnt: " << COUNTER_READ_3;
  COUNTER_INC_3;
  qDebug() << "Cnt: " << COUNTER_READ_3;
  
//   constexpr const int value_0 = decltype(cs_counter(CSInt<5>{}))::value;
//   qDebug() << "value_0: " << value_0;
//   
//   constexpr CSInt<value_0+1> cs_counter(CSInt<value_0+1>);
//   constexpr const int value_1 = decltype(cs_counter(CSInt<5>{}))::value;
//   qDebug() << "value_1: " << value_1;
}

/*
 * Yet a other counter
 */

namespace Mdt{ namespace StaticCounter{

  constexpr int maxCount = 255;

  /*
   * Typed value wrapper.
   */
  template<typename Tag, int N>
  struct ValueWrapper : ValueWrapper<Tag, N-1>
  {
    static constexpr int value = N;
  };

  // To stop the recursion
  template<typename Tag>
  struct ValueWrapper<Tag, 0>
  {
    static constexpr int value = 0;
  };

//   template<typename Tag, int N>
//   struct ValueWrapper : std::integral_constant<int, N>
//   {
//   };

}} // namespace Mdt{ namespace StaticCounter{

/*
 * Declare counter function
 * Cannot be in a namespace,
 * because this requires MDT_STATIC_COUNTER_INC()
 * to define it in this namespace,
 * which is not allowed in a function.
 */
// template<typename Tag>
// constexpr Mdt::StaticCounter::ValueWrapper<Tag,0> mdtStaticCounter(Mdt::StaticCounter::ValueWrapper<Tag,0>);

// Generate ValueWrapper<0> counter(ValueWrapper<0>); function declaration
#define MDT_STATIC_COUNTER_REGISTER(TAG) constexpr Mdt::StaticCounter::ValueWrapper<TAG,0> mdtStaticCounter(Mdt::StaticCounter::ValueWrapper<TAG,0>)

// Generate decltype( counter(ValueWrapper<maxCount>{}) )::value getter function
#define MDT_STATIC_COUNTER_READ(TAG) decltype( mdtStaticCounter(Mdt::StaticCounter::ValueWrapper< TAG, Mdt::StaticCounter::maxCount >{}) )::value

// Generate ValueWrapper<Acc+1> counter(ValueWrapper<Acc+1>); function declaration
#define MDT_STATIC_COUNTER_INC(TAG) constexpr Mdt::StaticCounter::ValueWrapper< TAG, MDT_STATIC_COUNTER_READ(TAG) + 1 > \
                                    mdtStaticCounter(Mdt::StaticCounter::ValueWrapper< TAG, MDT_STATIC_COUNTER_READ(TAG) + 1 >)

void Sandbox::staticCounter4()
{
  struct counter1{};
  struct counter2{};

  MDT_STATIC_COUNTER_REGISTER(counter1);
  MDT_STATIC_COUNTER_REGISTER(counter2);

  static_assert(MDT_STATIC_COUNTER_READ(counter1) == 0, "");
  static_assert(MDT_STATIC_COUNTER_READ(counter2) == 0, "");

  qDebug() << "vcnt1: " << MDT_STATIC_COUNTER_READ(counter1);
  qDebug() << "vcnt2: " << MDT_STATIC_COUNTER_READ(counter2);

  MDT_STATIC_COUNTER_INC(counter1);

  static_assert(MDT_STATIC_COUNTER_READ(counter1) == 1, "");
  static_assert(MDT_STATIC_COUNTER_READ(counter2) == 0, "");

  qDebug() << "vcnt1: " << MDT_STATIC_COUNTER_READ(counter1);
  qDebug() << "vcnt2: " << MDT_STATIC_COUNTER_READ(counter2);

  MDT_STATIC_COUNTER_INC(counter1);

  qDebug() << "vcnt1: " << MDT_STATIC_COUNTER_READ(counter1);
  qDebug() << "vcnt2: " << MDT_STATIC_COUNTER_READ(counter2);

  MDT_STATIC_COUNTER_INC(counter2);

  qDebug() << "vcnt1: " << MDT_STATIC_COUNTER_READ(counter1);
  qDebug() << "vcnt2: " << MDT_STATIC_COUNTER_READ(counter2);

//   qDebug() << "vcnt1: " << decltype( mdtStaticCounter(Mdt::StaticCounter::ValueWrapper< counter1, Mdt::StaticCounter::maxCount >{}) )::value;
//   qDebug() << "vcnt2: " << decltype( mdtStaticCounter(Mdt::StaticCounter::ValueWrapper< counter2, Mdt::StaticCounter::maxCount >{}) )::value;
// 
// //   constexpr Mdt::StaticCounter::ValueWrapper<counter1,0> mdtStaticCounter(Mdt::StaticCounter::ValueWrapper<counter1,0>);
// //   constexpr Mdt::StaticCounter::ValueWrapper<counter2,0> mdtStaticCounter(Mdt::StaticCounter::ValueWrapper<counter2,0>);
//   
//   qDebug() << "vcnt1: " << decltype( mdtStaticCounter(Mdt::StaticCounter::ValueWrapper< counter1, Mdt::StaticCounter::maxCount >{}) )::value;
//   qDebug() << "vcnt2: " << decltype( mdtStaticCounter(Mdt::StaticCounter::ValueWrapper< counter2, Mdt::StaticCounter::maxCount >{}) )::value;
/*  
  constexpr Mdt::StaticCounter::ValueWrapper<counter1,1> mdtStaticCounter(Mdt::StaticCounter::ValueWrapper<counter1,1>);
  
  qDebug() << "vcnt1: " << decltype( mdtStaticCounter(Mdt::StaticCounter::ValueWrapper< counter1, Mdt::StaticCounter::maxCount >{}) )::value;
  qDebug() << "vcnt2: " << decltype( mdtStaticCounter(Mdt::StaticCounter::ValueWrapper< counter2, Mdt::StaticCounter::maxCount >{}) )::value;

  constexpr Mdt::StaticCounter::ValueWrapper<counter2,1> mdtStaticCounter(Mdt::StaticCounter::ValueWrapper<counter2,1>);

  qDebug() << "vcnt1: " << decltype( mdtStaticCounter(Mdt::StaticCounter::ValueWrapper< counter1, Mdt::StaticCounter::maxCount >{}) )::value;
  qDebug() << "vcnt2: " << decltype( mdtStaticCounter(Mdt::StaticCounter::ValueWrapper< counter2, Mdt::StaticCounter::maxCount >{}) )::value;*/

//   qDebug() << "vcnt1: " << decltype( mdtStaticCounter(Mdt::StaticCounter::ValueWrapper< counter1, Mdt::StaticCounter::maxCount >{}) )::value;
//   qDebug() << "vcnt2: " << decltype( mdtStaticCounter(Mdt::StaticCounter::ValueWrapper< counter2, Mdt::StaticCounter::maxCount >{}) )::value;

//   qDebug() << "vcnt2: " << decltype( mdtStaticCounter(Mdt::StaticCounter::ValueWrapper< counter2, Mdt::StaticCounter::maxCount >{}) )::value;

//   qDebug() << "Value counter 1: " << MDT_STATIC_COUNTER_READ(counter1);
//   MDT_STATIC_COUNTER_INC(counter1);
//   
//   qDebug() << "Value counter 2: " << MDT_STATIC_COUNTER_READ(counter2);
//   MDT_STATIC_COUNTER_INC(counter2);
  
// //   constexpr Mdt::StaticCounter::ValueWrapper<counter1,1> mdtStaticCounter(Mdt::StaticCounter::ValueWrapper<counter1,1>);
//   
//   qDebug() << "Value: " << MDT_STATIC_COUNTER_READ(counter1);
// //   MDT_STATIC_COUNTER_INC(counter1);
// //   constexpr Mdt::StaticCounter::ValueWrapper<counter2,2> mdtStaticCounter(Mdt::StaticCounter::ValueWrapper<counter2,2>);
//   MDT_STATIC_COUNTER_INC(counter1);
//   qDebug() << "Value: " << MDT_STATIC_COUNTER_READ(counter1);
  
//   qDebug() << "Value counter2: " << MDT_STATIC_COUNTER_READ(counter2);
  
//   qDebug() << "Value counter2: " << MDT_STATIC_COUNTER_READ(counter2);
//   MDT_STATIC_COUNTER_INC(counter2);
//   qDebug() << "Value counter2: " << MDT_STATIC_COUNTER_READ(counter2);
//   MDT_STATIC_COUNTER_INC(counter2);
//   qDebug() << "Value counter2: " << MDT_STATIC_COUNTER_READ(counter2);
  
  
//   static_assert(MDT_STATIC_COUNTER_READ(counter1) == 0, "");
//   MDT_STATIC_COUNTER_INC(counter1);
//   static_assert(MDT_STATIC_COUNTER_READ(counter1) == 1, "");
//   qDebug() << "Value: " << MDT_STATIC_COUNTER_READ(counter1);
//   MDT_STATIC_COUNTER_INC(counter1);
//   qDebug() << "Value: " << MDT_STATIC_COUNTER_READ(counter1);
}


// proto::terminal<ColumnIndex>::type const Column {{}};
// proto::terminal<int>::type const Name {1};

// #define COLUMN_MAP(_name, _index) proto::terminal<int>::type const _name {_index};

// #define MDT_FILTER_EXPRESSION_COLUMN(_alias, _index) proto::terminal<int>::type const _alias {_index};
#define MDT_FILTER_EXPRESSION_COLUMN(_alias, _index) proto::terminal<ColumnIndex>::type const _alias {_index};
/// See Joel Falcou part 2 at 57min to possibly replace this macro !
///  Or directly here: https://github.com/jfalcou/cppcon-2015/blob/master/present/proto_wrap.cpp

struct ColumnIndexContext : proto::callable_context<ColumnIndexContext const>
{
  typedef int result_type;

  result_type operator()(proto::tag::terminal, const ColumnIndex & ci) const
  {
    qDebug() << " ci: " << ci.m_index;
    return 0;
  }

  template<int Index>
  result_type operator()(proto::tag::terminal, const placeholder<Index> &) const
  {
    qDebug() << " ph: " << Index;
    return Index;
  }

  result_type operator()(proto::tag::terminal, int val) const
  {
    qDebug() << " val: " << val;
    return 0;
  }

//   result_type operator()(proto::_child_c<0>, proto::_child_c<1>::) const
//   {
//     qDebug() << " _child0:" ;
//     return 0;
//   }

  template<int Index>
  const auto addIndex()
  {
//     qDebug() << "addIndex() - i: " << MDT_STATIC_COUNTER_READ(ColumnIndexContext);
    qDebug() << "addIndex() - i: " << Index;
    return typename proto::terminal< placeholder<Index> >::type {{}};
  }

};

int getFieldIndex()
{
  return 2;
}

#define addIndexToContext(ctx) \
        ctx.addIndex<MDT_STATIC_COUNTER_READ(counter)>(); \
        MDT_STATIC_COUNTER_INC(counter);

void Sandbox::columnConstant()
{
  ///MDT_FILTER_EXPRESSION_COLUMN(Name, 4);
  ///MDT_FILTER_EXPRESSION_COLUMN(Value, 1);

//   struct counter{};
//   MDT_STATIC_COUNTER_REGISTER(counter);

  ColumnIndexContext ctx;
  auto Name = ctx.addIndex<0>();
  auto Value = ctx.addIndex<1>();
  proto::terminal<int>::type i = {37};

  qDebug() << "Name  name: " << typeid(Name).name() << " , hash: " << typeid(Name).hash_code();
  qDebug() << "Value name: " << typeid(Value).name() << " , hash: " << typeid(Value).hash_code();
  qDebug() << "i     name: " << typeid(i).name() << " , hash: " << typeid(i).hash_code();
  
  
  proto::display_expr( (Name == 20) && (Value < 25) /* || (i == 3) */ );
  proto::eval( (Name == 20)  && (Value < 25) /*|| (i == 3) */ , ctx );

}

struct MyCustom : proto::callable
{
  typedef int result_type;

//   template<typename T>
  int operator()(int a) const
  {
    qDebug() << "a: " << a;
    return a+5;
  }

  template<typename T, typename U>
  int operator()(const T & t, const U & u) const
  {
//     qDebug() << "MyCustom , t: " << typeid(T).name();
    std::cout << "MyCustom , T: " << boost::core::demangle(typeid(T).name()) << std::endl;
    std::cout << "MyCustom , U: " << boost::core::demangle(typeid(U).name()) << std::endl;

    qDebug() << "t: " << proto::value(t) << " , u: " << proto::value(u);

    return proto::value(t) + proto::value(u) + 3;
  }
};

struct MyEval : proto::or_<
                  proto::when<
                    proto::plus<MyEval, MyEval> ,
                    proto::call<MyCustom(proto::_left, proto::_right)>
//                     proto::terminal<proto::_> ,
//                     proto::call<MyCustom(proto::_value)>
                  >
                >
{
};

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

void Sandbox::transforms()
{
//   proto::terminal< placeholder<0> >::type Name{{}};
//   proto::terminal< placeholder<0> >::type Value{{}};
// 
//   proto::display_expr( (Name == 20) && (Value < 25) );

  proto::terminal<int>::type NameIndex{2};

  proto::display_expr( NameIndex == 20 );

  MyEval e;
  int i = e( NameIndex + 20 );
  qDebug() << "i: " << i;
}


void Sandbox::someOtherToying()
{
}


void Sandbox::initTestCase()
{
}

void Sandbox::cleanupTestCase()
{
}

/*
 * Tests
 */



/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  Sandbox test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
