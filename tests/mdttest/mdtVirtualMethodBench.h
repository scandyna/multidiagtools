/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#ifndef MDT_VIRTUAL_METHOD_BENCH
#define MDT_VIRTUAL_METHOD_BENCH

#include "mdtTest.h"

namespace mdtVirtualMethodBenchNamespace{

/*
 * Complete class, to test direct calls
 */
class complete
{
 public:

  volatile int add(int a, int b);
};

/*
 * Base class with virtual method
 * and derived class wich redifines method
 */
class base1
{
 public:

  virtual volatile int add(int a, int b);
};

class derived1 : public base1
{
 public:

  volatile int add(int a, int b);
};

/*
 * Base class with pure virtual method
 * and derived class wich redifines method
 */
class base2
{
 public:

  virtual volatile int add(int a, int b) = 0;
};

class derived2 : public base2
{
 public:

  volatile int add(int a, int b);
};

class derived22 : public base2
{
 public:

  volatile int add(int a, int b);
};

class derived23 : public base2
{
 public:

  volatile int add(int a, int b);
};

} // namescpace mdtVirtalMethodBench

/*
 * Test class
 */
class mdtVirtualMethodBench : public mdtTest
{
 Q_OBJECT

 private slots:

  void initTestCase();

  void pointerBasedBench();
  void stackBasedBench();
};

#endif // MDT_VIRTUAL_METHOD_BENCH
