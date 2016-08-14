#include "OtherSandbox.h"
#include "Mdt/Application.h"
#include <QDebug>
#include <type_traits>
#include <typeinfo>
#include <utility>

#include <boost/core/demangle.hpp>
#include <iostream>

// #include <boost/mpl/vector_c.hpp>
// #include <boost/mpl/plus.hpp>
// #include <boost/mpl/int.hpp>
// #include <boost/mpl/transform.hpp>
// #include <boost/mpl/equal.hpp>
// 
// namespace mpl = boost::mpl;
// 
// typedef mpl::vector_c<int,1,0,0,0,0,0,0> mass;
// typedef mpl::vector_c<int,0,1,0,0,0,0,0> length;  // Or position
// typedef mpl::vector_c<int,0,0,1,0,0,0,0> myTime;
// typedef mpl::vector_c<int,0,0,0,1,0,0,0> charge;
// typedef mpl::vector_c<int,0,0,0,0,1,0,0> temperature;
// typedef mpl::vector_c<int,0,0,0,0,0,1,0> intensity;
// typedef mpl::vector_c<int,0,0,0,0,0,0,1> angle;
// 
// // Base dimension:         m  l  t  c  t  i  a
// typedef mpl::vector_c<int, 0, 1,-1, 0, 0, 0, 0> velocity;     // l/t
// typedef mpl::vector_c<int, 0, 1,-2, 0, 0, 0, 0> acceleration; // l/(t2)
// typedef mpl::vector_c<int, 1, 1,-1, 0, 0, 0, 0> momentum;     // ml/t
// typedef mpl::vector_c<int, 1, 1,-2, 0, 0, 0, 0> force;        // ml/(t2)
// 
// typedef mpl::vector_c<int,0,0,0,0,0,0,0> scalar;
// 
// template<typename T, typename Dimension>
// struct quantity
// {
//   explicit quantity(T x)
//    : m_value(x) {}
// 
//   template<typename OtherDimension>
//   quantity(quantity<T, OtherDimension> const & rhs)
//    : m_value(rhs.value())
//   {
//     static_assert(mpl::equal<Dimension, OtherDimension>::type::value, "");
//   }
// 
//   T value() const
//   {
//     return m_value;
//   }
// 
//  private:
// 
//   T m_value;
// };
// 
// 
// /*
//  * Addition
//  */
// template<typename T, typename D>
// quantity<T, D> operator+(quantity<T,D> x, quantity<T,D> y)
// {
//   return quantity<T,D>( x.value() + y.value() );
// }
// 
// /*
//  * Substraction
//  */
// template<typename T, typename D>
// quantity<T, D> operator-(quantity<T,D> x, quantity<T,D> y)
// {
//   return quantity<T,D>( x.value() - y.value() );
// }
// 
// /*
//  * Multiplication
//  */
// 
// // Metafunction class
// struct plus_f
// {
//   template<typename T1, typename T2>
//   struct apply
//   {
//     typedef typename mpl::plus<T1,T2>::type type;
//   };
// };
// 
// // Multiplication operator function
// template<typename T, typename D1, typename D2>
// quantity<T, typename mpl::transform<D1,D2,plus_f>::type > operator*(quantity<T,D1> x, quantity<T,D2> y)
// {
//   typedef typename mpl::transform<D1,D2,plus_f>::type dim;
//   return quantity<T, dim>( x.value() * y.value() );
// }
// 
// void OtherSandbox::sandbox()
// {
//   quantity<double, length> l1(1.0) , l2(2.0);
//   quantity<double, mass> m(2.0);
//   quantity<double, acceleration> a(9.8);
//   quantity<double, force> f = m*a;
// 
//   qDebug() << "l1: " << l1.value() << ", m: " << m.value();
//   qDebug() << "l1 + l2: " << (l1+l2).value();
//   qDebug() << "l1 - l2: " << (l1-l2).value();
//   qDebug() << "l1 * l2: " << (l1*l2).value();
//   qDebug() << "m * a: " << (m*a).value();
// //   qDebug() << "l1 / l2: " << (l1/l2).value();
// 
// //   l = m;
// }

/*
 * Sources:
 *  - http://b.atch.se/posts/constexpr-counter/
 *  - http://www.elbeno.com/blog/?p=1284
 *  -
 */

// constexpr int adl_flag(flag<N>);

template<int N>
struct flag
{
  friend
  constexpr int adl_flag(flag<N>);
};

// constexpr int adl_flag(flag<N>);

// template<int N>
// constexpr int adl_flag(flag<N>)
// {
//   return N;
// }

template<int N>
struct writer
{
  friend
  constexpr int adl_flag(flag<N>)
  {
    return N;
  }

  static constexpr int value = N;
};

// The match
template<int N, int = adl_flag(flag<N>{})>
constexpr int reader(int, flag<N>)
{
  return N;
}

// The searcher
template<int N>
constexpr int reader(float, flag<N>, int R = reader(0, flag<N-1>{}) )
{
  return R;
}

// The base
constexpr int reader(float, flag<0>)
{
  return 0;
}

template<int N = 1>
constexpr int next(int R = writer<reader(0, flag<4>{}) + N>::value )
{
  return R;
}

void OtherSandbox::sandbox()
{
//   qDebug() << "reader(0, flag<2>{}): " << reader(0, flag<2>{});
//   qDebug() << "writer<2>::value: " << writer<2>::value;
//   qDebug() << "reader(0, flag<2>{}): " << reader(0, flag<2>{});

//   qDebug() << "adl_flag(flag<2>): " << adl_flag( flag<2>{} );
//   qDebug() << "writer<2>::value: " << writer<2>::value;
//   qDebug() << "reader(2,flag<2>): " << reader(2,flag<2>{});
//   qDebug() << "reader(flag<2>): " << reader(flag<2>{});
//   static_assert(next()-1 == 0, "");
//   qDebug() << "next() - 1: " << next() - 1;
//   qDebug() << "next() - 1: " << next() - 1;
//   qDebug() << "next() - 1: " << next() - 1;
}

// template<int N>
// struct A : public A<N-1>
// {
//   static constexpr int value = N;
// };
// 
// template<>
// struct A<0>
// {
//   static constexpr int value = 0;
// };
// 
// constexpr A<0> counter(A<0>);
// //constexpr A<1> counter(A<1>);
// 
// template<int N>
// constexpr A<N> counter(A<N>);
// 
// static void register_a( A< decltype( counter(A<5>{}) )::value > )
// {
//   qDebug() << "register_a(): " << decltype( counter(A<5>{}) )::value;
//   register_a( A< decltype( counter(A<5>{}) )::value + 1 >{} );
// }

void OtherSandbox::sandbox2()
{
//   qDebug() << "A<5>::value:" << A<5>::value;
//   qDebug() << "decltype( counter(A<5>{}) )::value:" << decltype( counter(A<5>{}) )::value;
// //   register_a( A<5>{} );
//   qDebug() << "decltype( counter(A<5>{}) )::value:" << decltype( counter(A<5>{}) )::value;
}

/*
 * Exemples for template instantiations:
 * http://b.atch.se/posts/non-constant-constant-expressions/
 */

// class A;
// void touch(A&);

class A
{
 public:

  A(int i)
   : member(i)
  {
    qDebug() << "A::A(" << i << ")";
  }

  friend
  void f(A);

 private:

  int member;
};

void g(A);

class B
{
  friend
  void f(A)
  {
    qDebug() << "B::f(A)";
  }

  class C
  {
    friend
    void g(A)
    {
      qDebug() << "C::g(A)";
    }
  };

};

constexpr int cf();
void indirection();

void OtherSandbox::sandbox3()
{
  A a(0);
  f(a);
  g(1);

  constexpr bool isConstExpr = noexcept(cf());
  qDebug() << "TEST: isConstExpr: " << isConstExpr;
  indirection();
}

constexpr int cf()
{
  return 0;
}

void indirection()
{
  constexpr bool isConstExpr = noexcept(cf());
  qDebug() << "indirection() - isConstExpr: " << isConstExpr;
  constexpr int n = cf();
  qDebug() << "indirection() - n: " << n;
}

namespace N
{
  struct X{};

  void func(X, int)
  {
    qDebug() << "N::func(X, int)";
  }
}

template<typename T>
struct AA
{
  using type = decltype(func(T{}, 3.14f));
};

template<typename T>
struct BB
{
  using type = decltype(func(T{}, 3.14f));
};

void gg();

void OtherSandbox::sandbox4()
{
  AA<N::X> a;

  qDebug() << "TEST - type a:" << typeid(a).name();
  gg();
}

namespace N
{
  float func(X, float)
  {
    qDebug() << "N::func(X, float)";
    return 0.0f;
  }
}

void gg()
{
//   AA<N::X>::type a;
  BB<N::X>::type b;

//   qDebug() << "gg() - type a:" << typeid(a).name();
  qDebug() << "gg() - type b:" << typeid(b).name();
}


constexpr int func5(short);
constexpr int func5(float);

template<typename T>
struct AAA
{
  friend
  constexpr int func5(T)
  {
    return 0;
  }
};

template<typename T>
AAA<T> indirection5()
{
  return {};
}

void OtherSandbox::sandbox5()
{
  indirection5<short>();
  indirection5<float>();
}

/*
 * Exemples for template instantiations:
 * http://b.atch.se/posts/non-constant-constant-expressions/
 * Solution part
 */

constexpr int flag_func6(int);

template<typename Tag>
struct writer6
{
  friend
  constexpr int flag_func6(Tag)
  {
    return 0;
  }
};

template<bool B, typename Tag = int>
struct dependent_writer : writer6<Tag>
{
};

template<
        bool B = noexcept(flag_func6(0)),
        int = sizeof(dependent_writer<B>)
        >
constexpr int func6()
{
  return B;
}

void OtherSandbox::sandbox6()
{
  constexpr int a = func6();
  constexpr int b = func6();

//   static_assert(a != b, "");
  qDebug() << "a: " << a << ", b: " << b;

  std::cout << "demangle(a): " << boost::core::demangle(typeid(a).name()) << std::endl;
}


constexpr int func7_decl();

constexpr int func7_complete()
{
  return 0;
}


void OtherSandbox::sandbox7()
{
  qDebug() << std::is_function<decltype(func7_decl)>::value;
  qDebug() << std::is_function<decltype(func7_complete)>::value;
  
  qDebug() << std::is_trivial<decltype(func7_decl)>::value;
  qDebug() << std::is_trivial<decltype(func7_complete)>::value;

  qDebug() << std::is_literal_type<decltype(func7_decl)>::value;
  qDebug() << std::is_literal_type<decltype(func7_complete)>::value;

  qDebug() << std::is_trivially_constructible<decltype(func7_decl)>::value;
  qDebug() << std::is_trivially_constructible<decltype(func7_complete)>::value;

  qDebug() << std::is_constructible<decltype(func7_decl)>::value;
  qDebug() << std::is_constructible<decltype(func7_complete)>::value;

  qDebug() << std::is_nothrow_constructible<decltype(func7_decl)>::value;
  qDebug() << std::is_nothrow_constructible<decltype(func7_complete)>::value;
}

/*
 * Variant from Matt Calabrese
 */

template<int Val = 4>
struct num_elem : num_elem<Val-1>
{
};

template<>
struct num_elem<0>
{
};

template<typename Tag, typename ThisElem = void, typename OtherHolder = void>
struct type_seq_holder
{
  static constexpr int value = OtherHolder::value + 1;
  static constexpr num_elem<value+1> next_index();
//   typedef typename push_back< typename OtherHolder::type_seq, ThisElem >::type type_seq;
};

template<typename Tag>
struct type_seq_holder<Tag>
{
  static constexpr int value = 0;
  static constexpr num_elem<1> next_index();
//   typedef vector<> type_seq;
};

template<typename Tag>
type_seq_holder<Tag> get_type_seq_holder(Tag, const num_elem<0> &);

struct list_tag{};
num_elem<> new_elem;

// type_seq_holder<list_tag, new_elem, decltype( get_type_seq_holder( list_tag(), num_elem<>() ) )>
// get_type_seq_holder(list_tag, decltype( get_type_seq_holder(list_tag(), num_elem<>()).next_index() ) const& );

void OtherSandbox::sandbox8()
{
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  OtherSandbox test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
