/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "CompareRowColumnListTest.h"
#include "Mdt/TestLib/CompareRowList.h"
#include "Mdt/TestLib/CompareColumnList.h"

using namespace Mdt::Container;

/*
 * Tests
 */

void CompareRowColumnListTest::compareRowListTest()
{
  RowList a{1,2,3};
  RowList b = a;

  QCOMPARE(a, b);
}

void CompareRowColumnListTest::compareColumnListTest()
{
  ColumnList a{1,2,3};
  ColumnList b = a;

  QCOMPARE(a, b);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  CompareRowColumnListTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

