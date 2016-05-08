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
#include "DoubleTest.h"
#include "mdtApplication.h"
#include "Mdt/Numeric/Double.h"

#include <QDebug>

void DoubleTest::initTestCase()
{
}

void DoubleTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void DoubleTest::simpleTest()
{
  using Mdt::Numeric::Double;

  /*
   * Construction
   */
  // Default constructed
  Double x;
  QVERIFY(x.isNaN());
  QVERIFY(x.isNull());
  // Construct a value
  Double y(1.0);
  QVERIFY(!y.isNaN());
  QVERIFY(!y.isNull());
  QCOMPARE(y.toDouble(), 1.0);
  /*
   * Set
   */
  // Affectation
  x = y;
  QVERIFY(!x.isNull());
  QCOMPARE(x.toDouble(), 1.0);
  x = 21.5;
  QVERIFY(!x.isNull());
  QCOMPARE(x.toDouble(), 21.5);
  /*
   * Infinity values
   */
  // Non infinity value
  x = 10.0;
  QVERIFY(!x.isNull());
  QVERIFY(!x.isInfinity());
  QVERIFY(!x.isMinusInfinity());
  QVERIFY(!x.isPlusInfinity());
  // -infinity value
  x.setMinusInfinity();
  QVERIFY(!x.isNull());
  QVERIFY(x.isInfinity());
  QVERIFY(x.isMinusInfinity());
  QVERIFY(!x.isPlusInfinity());
  // +infinity value
  x.setPlusInfinity();
  QVERIFY(!x.isNull());
  QVERIFY(x.isInfinity());
  QVERIFY(!x.isMinusInfinity());
  QVERIFY(x.isPlusInfinity());
  /*
   * Clear
   */
  QVERIFY(!x.isNull());
  x.clear();
  QVERIFY(x.isNaN());
  QVERIFY(x.isNull());
  QVERIFY(!x.isInfinity());
  QVERIFY(!x.isMinusInfinity());
  QVERIFY(!x.isPlusInfinity());
}

void DoubleTest::comparisonTest()
{
  using Mdt::Numeric::Double;

  Double x, y;

  // Valid values
  x = 1.0;
  y = 1.0;
  QVERIFY(x == y);
  QVERIFY(!(x != y));
  QVERIFY(x <= y);
  QVERIFY(!(x < y));
  QVERIFY(x >= y);
  QVERIFY(!(x > y));
  x = 1.0;
  y = 1.000000001;
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  QVERIFY(x <= y);
  QVERIFY(x < y);
  QVERIFY(!(x >= y));
  QVERIFY(!(x > y));
  // null values
  x = 0.0;
  y.clear();
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  QVERIFY(!(x <= y));
  QVERIFY(!(x < y));
  QVERIFY(!(x >= y));
  QVERIFY(!(x > y));
  x.clear();
  y = 0.0;
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  QVERIFY(!(x <= y));
  QVERIFY(!(x < y));
  QVERIFY(!(x >= y));
  QVERIFY(!(x > y));
  x.clear();
  y.clear();
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  QVERIFY(!(x <= y));
  QVERIFY(!(x < y));
  QVERIFY(!(x >= y));
  QVERIFY(!(x > y));
  // Null , 0 comparison
  x.clear();
  y = 0.0;
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  x = 0.0;
  QVERIFY(x == y);
  QVERIFY(!(x != y));
  // Comparison with null
  x.clear();
  y.clear();
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  x = 0.0;
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  x = 0.0;
  // Comparison with double
  x = 1.0;
  QVERIFY(x == 1.0);
  QVERIFY(x <= 1.0);
  QVERIFY(x >= 1.0);
  QVERIFY(x < 1.0000001);
  QVERIFY(x > 0.9999999);

}

void DoubleTest::mathOperatorsTest()
{
  using Mdt::Numeric::Double;

  Double x, y, z;

  /*
   * Additions
   */
  // common values
  x = 1.0;
  y = 2.5;
  z = x + y;
  QVERIFY(!z.isNull());
  QCOMPARE(z.toDouble(), 3.5);
  // null values propagation
  x.clear();
  y.clear();
  z = x + y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x = 5.0;
  y.clear();
  z = x + y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x.clear();
  y = 5.0;
  z = x + y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  // Infinity values
  x = 0.0;
  y.setPlusInfinity();
  z = x + y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  QVERIFY(z.isPlusInfinity());
  x = -10.0;
  y.setMinusInfinity();
  z = x + y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  QVERIFY(z.isMinusInfinity());
  /*
   * Subtraction
   */
  // common values
  x = 1.0;
  y = 3.0;
  z = x - y;
  QVERIFY(!z.isNull());
  QCOMPARE(z.toDouble(), -2.0);
  // null values propagation
  x.clear();
  y.clear();
  z = x - y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x = 5.0;
  y.clear();
  z = x - y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x.clear();
  y = 5.0;
  z = x - y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  // Infinity values
  x = 0.0;
  y.setPlusInfinity();
  z = x - y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  QVERIFY(z.isMinusInfinity());
  x = 10.0;
  y.setMinusInfinity();
  z = x - y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  QVERIFY(z.isPlusInfinity());
  /*
   * Negation
   */
  // Negate common value
  x = 5.0;
  x = -x;
  QCOMPARE(x.toDouble(), -5.0);
  // Negate +infinity
  x.setPlusInfinity();
  x = -x;
  QVERIFY(x.isMinusInfinity());
  // Negate -infinity
  x.setMinusInfinity();
  x = -x;
  QVERIFY(x.isPlusInfinity());
  // Negate a null value
  x.clear();
  x = -x;
  QVERIFY(x.isNull());
  /*
   * Multiplication
   */
  // common values
  x = 2.0;
  y = 3.0;
  z = x * y;
  QVERIFY(!z.isNull());
  QCOMPARE(z.toDouble(), 6.0);
  // null values propagation
  x.clear();
  y.clear();
  z = x * y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x = 5.0;
  y.clear();
  z = x * y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x.clear();
  y = 5.0;
  z = x * y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  // Infinity values
  x = 1.0;
  y.setPlusInfinity();
  z = x * y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  QVERIFY(z.isPlusInfinity());
  x = -10.0;
  y.setMinusInfinity();
  z = x * y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  QVERIFY(z.isPlusInfinity());
  /*
   * Division
   */
  // common values
  x = 6.0;
  y = 2.0;
  z = x / y;
  QVERIFY(!z.isNull());
  QCOMPARE(z.toDouble(), 3.0);
  // null values propagation
  x.clear();
  y.clear();
  z = x / y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x = 5.0;
  y.clear();
  z = x / y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x.clear();
  y = 5.0;
  z = x / y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  // Infinity values
  x = 1.0;
  y.setPlusInfinity();
  z = x / y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  QCOMPARE(z.toDouble(), 0.0);
  x = -10.0;
  y.setMinusInfinity();
  z = x / y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  QCOMPARE(z.toDouble(), 0.0);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  DoubleTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
