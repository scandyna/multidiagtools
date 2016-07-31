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
#include <boost/proto/proto.hpp>
#include <boost/typeof/std/ostream.hpp>

boost::proto::terminal< std::ostream & >::type cout_ = { std::cout };

template<typename Expr>
void evaluate( const Expr & expr)
{
  boost::proto::default_context ctx;
  boost::proto::eval(expr, ctx);
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

void Sandbox::sandbox()
{
  evaluate( cout_ << "hello" << ',' << " world\n" );
}



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
