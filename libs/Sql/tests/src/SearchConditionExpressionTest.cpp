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
#include "SearchConditionExpressionTest.h"
#include "Mdt/Application.h"

#include <boost/fusion/sequence.hpp>
#include <boost/fusion/include/sequence.hpp>

#include <boost/fusion/container/list.hpp>
#include <boost/fusion/include/list.hpp>
#include <boost/fusion/container/list/list_fwd.hpp>
#include <boost/fusion/include/list_fwd.hpp>

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

#include <string>
#include <vector>
#include <iterator>

#include <iostream>

#include <boost/proto/proto.hpp>
namespace proto = boost::proto;

namespace fusion = boost::fusion;

void SearchConditionExpressionTest::initTestCase()
{
}

void SearchConditionExpressionTest::cleanupTestCase()
{
}

proto::terminal<std::ostream &>::type const cout_ = {std::cout};

struct FoldToList : proto::or_<
                          proto::when<
                            proto::terminal< std::ostream & > ,
                            proto::_state
                          > ,
                          proto::when<
                            proto::terminal< proto::_ > ,
                            fusion::list< proto::_value, proto::_state >(proto::_value, proto::_state)
                          > ,
                          proto::when<
                            proto::shift_left< FoldToList, FoldToList > ,
                            proto::call< FoldToList( proto::_left, FoldToList(proto::_right, proto::_state) ) >
                            //proto::call< FoldToList(proto::_left, proto::_right, proto::_state) >
                          >
                        >
{
};

/*
struct MyStrCat : proto::or_<
                        proto::when<
                          proto::terminal< std::ostream & > ,
                          proto::_state
                        > ,
                        proto::when<
                          proto::terminal<int> ,
                          proto::_state.push_back(proto::_value)
                        >
                      >
{
};
*/

struct SomeWork
{
  template<typename T>
  void operator()(const T & x) const
  {
    std::cout << "x type: " << typeid(x).name() << " , value: " << x << std::endl;
  }
};

/*
 * Tests
 */

void SearchConditionExpressionTest::sandbox()
{
  fusion::vector<int, char, std::string, int> v(1, 'x', "str", 2);
  qDebug() << "v: " << fusion::at_c<0>(v) << " , " << fusion::at_c<1>(v) << " , " << fusion::at_c<2>(v).c_str() << " , " << fusion::at_c<3>(v);

  fusion::list<int, double, std::string, int> l(1, 2.5, "lstr", 77);
  qDebug() << "l: " << fusion::at_c<0>(l) << " , " << fusion::at_c<1>(l) << " , " << fusion::at_c<2>(l).c_str() << " , " << fusion::at_c<3>(l);

  FoldToList toList;
  auto args = toList(cout_ << 1 << 3.14 << 85 << '\n' << 77, fusion::nil());
  qDebug() << "args: " << fusion::at_c<0>(args);
  fusion::for_each(args, SomeWork());
  ///std::cout << std::endl;

//   std::vector<int> outV;
//   MyStrCat sc;
//   auto args = sc(cout_ << 1 << 3.14 << '\n' , outV);
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SearchConditionExpressionTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
