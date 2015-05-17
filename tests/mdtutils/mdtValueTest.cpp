/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtValueTest.h"
#include "mdtApplication.h"
#include "mdtValue.h"
#include <QVariant>
#include <limits>

//#include <QDebug>

void mdtValueTest::valueDoubleTest()
{
  mdtValueDouble x;
  
  // Initial state
  QVERIFY(x.isNull());
  QVERIFY(!x.isMinusOl());
  QVERIFY(!x.isPlusOl());
  QVERIFY(x.isNaN());

  qDebug() << "sizeof(double): " << sizeof(double);
  qDebug() << "sizeof(mdtValueDouble): " << sizeof(mdtValueDouble);

  // Set a value - No flags
  x.setValue(1.0);
  QVERIFY(!x.isNull());
  QVERIFY(!x.isMinusOl());
  QVERIFY(!x.isPlusOl());
  QVERIFY(!x.isNaN());
  QCOMPARE(x.value(), 1.0);
  // Clear
  x.clear();
  QVERIFY(x.isNull());
  QVERIFY(!x.isMinusOl());
  QVERIFY(!x.isPlusOl());
  QVERIFY(x.isNaN());
  // Set a value - -OL flag
  x.setValue(-1.0, true);
  QVERIFY(!x.isNull());
  QVERIFY(x.isMinusOl());
  QVERIFY(!x.isPlusOl());
  QVERIFY(!x.isNaN());
  QVERIFY(x.value() == -std::numeric_limits<double>::infinity());
  // Clear
  x.clear();
  QVERIFY(x.isNull());
  QVERIFY(!x.isMinusOl());
  QVERIFY(!x.isPlusOl());
  QVERIFY(x.isNaN());
  // Set a value - +OL flag
  x.setValue(1.0, true);
  QVERIFY(!x.isNull());
  QVERIFY(!x.isMinusOl());
  QVERIFY(x.isPlusOl());
  QVERIFY(!x.isNaN());
  QVERIFY(x.value() == std::numeric_limits<double>::infinity());
  // Set a value - +OL flag with 0.0 value
  x.setValue(0.0, true);
  QVERIFY(!x.isNull());
  QVERIFY(!x.isMinusOl());
  QVERIFY(x.isPlusOl());
  QVERIFY(!x.isNaN());
  QVERIFY(x.value() == std::numeric_limits<double>::infinity());
  // NaN
  x.setValue(std::numeric_limits<double>::quiet_NaN());
  QVERIFY(!x.isNull());
  QVERIFY(!x.isMinusOl());
  QVERIFY(!x.isPlusOl());
  QVERIFY(x.isNaN());

  /*
   * Construct a second value
   */
  mdtValueDouble y(2.0, true);
  QVERIFY(!y.isNull());
  QVERIFY(!y.isMinusOl());
  QVERIFY(y.isPlusOl());
  QVERIFY(y.value() == std::numeric_limits<double>::infinity());

  /*
   * Comparison
   */
  // Valid values
  x.setValue(1.0);
  y.setValue(1.0);
  QVERIFY(x == y);
  QVERIFY(!(x != y));
  QVERIFY(x <= y);
  QVERIFY(!(x < y));
  QVERIFY(x >= y);
  QVERIFY(!(x > y));
  x.setValue(1.0);
  y.setValue(1.000000001);
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  QVERIFY(x <= y);
  QVERIFY(x < y);
  QVERIFY(!(x >= y));
  QVERIFY(!(x > y));
  // null values
  x.setValue(0.0);
  y.clear();
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  QVERIFY(!(x <= y));
  QVERIFY(!(x < y));
  QVERIFY(!(x >= y));
  QVERIFY(!(x > y));
  x.clear();
  y.setValue(0.0);
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
  // +OL flag
  x.setValue(1.0);
  y.setValue(1.0, true);
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  QVERIFY(x <= y);
  QVERIFY(x < y);
  QVERIFY(!(x >= y));
  QVERIFY(!(x > y));
  x.setValue(1.0, true);
  y.setValue(1.0);
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  QVERIFY(!(x <= y));
  QVERIFY(!(x < y));
  QVERIFY(x >= y);
  QVERIFY(x > y);
  // -OL flag
  x.setValue(-1.0);
  y.setValue(-1.0, true);
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  QVERIFY(!(x <= y));
  QVERIFY(!(x < y));
  QVERIFY(x >= y);
  QVERIFY(x > y);
  x.setValue(-1.0, true);
  y.setValue(-1.0);
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  QVERIFY(x <= y);
  QVERIFY(x < y);
  QVERIFY(!(x >= y));
  QVERIFY(!(x > y));
  // Compare identical values with different OL flags
  x.setValue(1.0, true);
  y.setValue(1.0);
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  // Extrems comparison
  x.setValue(1.0, true);
  y.setValue(-1.0, true);
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  // Null , 0 comparison
  x.clear();
  y.setValue(0.0);
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  x.setValue(0.0);
  QVERIFY(x == y);
  QVERIFY(!(x != y));
  // Comparison with null
  x.clear();
  y.clear();
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  x.setValue(0.0);
  QVERIFY(!(x == y));
  QVERIFY(x != y);
  x.setValue(0.0);
  // Comparison with double
  x.setValue(1.0);
  QVERIFY(x == 1.0);
  QVERIFY(x <= 1.0);
  QVERIFY(x >= 1.0);
  QVERIFY(x < 1.0000001);
  QVERIFY(x > 0.9999999);

  /*
   * Copy constructor
   */
  x.setValue(3.0, true);
  mdtValueDouble z(x);
  QVERIFY(!z.isNull());
  QVERIFY(!z.isMinusOl());
  QVERIFY(z.isPlusOl());
  QVERIFY(z.value() == std::numeric_limits<double>::infinity());
  QVERIFY(x == z);
  z.setValue(4.0);
  QVERIFY(x.value() == std::numeric_limits<double>::infinity());
  QCOMPARE(z.value(), 4.0);
  QVERIFY(x != z);

  /*
   * Assignment operator
   */
  x.setValue(5.0, true);
  z = x;
  QVERIFY(x == z);
  QVERIFY(!z.isNull());
  QVERIFY(!z.isMinusOl());
  QVERIFY(z.isPlusOl());
  QVERIFY(z.value() == std::numeric_limits<double>::infinity());
  x.clear();
  QVERIFY(x.isNull());
  QVERIFY(!x.isMinusOl());
  QVERIFY(!x.isPlusOl());
  x = 6.0;
  QVERIFY(!x.isNull());
  QVERIFY(!x.isMinusOl());
  QVERIFY(!x.isPlusOl());
  QCOMPARE(x.value(), 6.0);
  x.setValue(7.0, true);
  QVERIFY(!x.isNull());
  QVERIFY(!x.isMinusOl());
  QVERIFY(x.isPlusOl());
  QVERIFY(x.value() == std::numeric_limits<double>::infinity());
  x = 8.0;
  QVERIFY(!x.isNull());
  QVERIFY(!x.isMinusOl());
  QVERIFY(!x.isPlusOl());
  QCOMPARE(x.value(), 8.0);

  /*
   * Additions
   */
  // common values
  x.setValue(1.0);
  y.setValue(2.5);
  z = x + y;
  QVERIFY(!z.isNull());
  QCOMPARE(z.value(), 3.5);
  // null values propagation
  x.clear();
  y.clear();
  z = x + y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x.setValue(5.0);
  y.clear();
  z = x + y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x.clear();
  y.setValue(5.0);
  z = x + y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  // Infinity values
  x.setValue(0.0);
  y.setValue(0.0, true);
  z = x + y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  QVERIFY(z.isPlusOl());
  x.setValue(-10.0);
  y.setValue(-10.0, true);
  z = x + y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  QVERIFY(z.isMinusOl());

  /*
   * Subtraction
   */
  // common values
  x.setValue(1.0);
  y.setValue(3.0);
  z = x - y;
  QVERIFY(!z.isNull());
  QCOMPARE(z.value(), -2.0);
  // null values propagation
  x.clear();
  y.clear();
  z = x - y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x.setValue(5.0);
  y.clear();
  z = x - y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x.clear();
  y.setValue(5.0);
  z = x - y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  // Infinity values
  x.setValue(0.0);
  y.setValue(0.0, true);
  z = x - y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  QVERIFY(z.isMinusOl());
  x.setValue(-10.0);
  y.setValue(-10.0, true);
  z = x - y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  //QVERIFY(z.isPlusOl());

  /*
   * Multiplication
   */
  // common values
  x.setValue(2.0);
  y.setValue(3.0);
  z = x * y;
  QVERIFY(!z.isNull());
  QCOMPARE(z.value(), 6.0);
  // null values propagation
  x.clear();
  y.clear();
  z = x * y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x.setValue(5.0);
  y.clear();
  z = x * y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x.clear();
  y.setValue(5.0);
  z = x * y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  // Infinity values
  x.setValue(1.0);
  y.setValue(1.0, true);
  z = x * y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  QVERIFY(z.isPlusOl());
  x.setValue(-10.0);
  y.setValue(-10.0, true);
  z = x * y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  //QVERIFY(z.isPlusOl());

  /*
   * Division
   */
  // common values
  x.setValue(6.0);
  y.setValue(2.0);
  z = x / y;
  QVERIFY(!z.isNull());
  QCOMPARE(z.value(), 3.0);
  // null values propagation
  x.clear();
  y.clear();
  z = x / y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x.setValue(5.0);
  y.clear();
  z = x / y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  x.clear();
  y.setValue(5.0);
  z = x / y;
  QVERIFY(z.isNull());
  QVERIFY(z.isNaN());
  // Infinity values
  x.setValue(1.0);
  y.setValue(1.0, true);
  z = x / y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  //QVERIFY(z.isPlusOl());
  x.setValue(-10.0);
  y.setValue(-10.0, true);
  z = x / y;
  QVERIFY(!z.isNull());
  QVERIFY(!z.isNaN());
  //QVERIFY(z.isPlusOl());
  /*
   * Negation
   */
  // Negate common value
  x = 5.0;
  x = -x;
  QCOMPARE(x.value(), -5.0);
  // Negate +infinity
  x.setValue(1.0, true);
  x = -x;
  QVERIFY(!x.isPlusOl());
  QVERIFY(x.isMinusOl());
  // Negate -infinity
  x.setValue(-1.0, true);
  x = -x;
  QVERIFY(x.isPlusOl());
  QVERIFY(!x.isMinusOl());
  // Negate a null value
  x.clear();
  x = -x;
  QVERIFY(x.isNull());

  /*
   * QVariant storage
   */
  QVariant var;
  x = 9.0;
  var.setValue(x);
  QCOMPARE(var.value<mdtValueDouble>().value(), 9.0);

  qDebug() << x;
}

void mdtValueTest::valueDoubleBenchmark()
{
  QBENCHMARK{
    mdtValueDouble x, y;
    x.setValue(2.5);
    y = x;
    if(x != y){
      QFAIL("Comparison failure");
    }
  }
}

void mdtValueTest::valueIntTest()
{
  mdtValueInt a;

  // Initial state
  QVERIFY(a.isNull());
  QCOMPARE(a.value(), 0);

  qDebug() << "sizeof(int): " << sizeof(int);
  qDebug() << "sizeof(mdtValueInt): " << sizeof(mdtValueInt);

  // Basic assignment
  a.setValue(1);
  QVERIFY(!a.isNull());
  QCOMPARE(a.value(), 1);
  a.clear();
  QVERIFY(a.isNull());
  QCOMPARE(a.value(), 0);
  a = 2;
  QVERIFY(!a.isNull());
  QCOMPARE(a.value(), 2);
  // Constructor
  mdtValueInt b(3);
  QVERIFY(!b.isNull());
  QCOMPARE(b.value(), 3);
  // Copy
  a = b;
  QVERIFY(!a.isNull());
  QVERIFY(!b.isNull());
  QCOMPARE(a.value(), 3);
  QCOMPARE(b.value(), 3);
  // Comparison
  QVERIFY(a == b);
  QVERIFY(!(a != b));
  QVERIFY(a <= b);
  QVERIFY(!(a < b));
  QVERIFY(a >= b);
  QVERIFY(!(a > b));
  // Null comparison
  a.clear();
  QVERIFY(!(a == b));
  QVERIFY(a != b);
  QVERIFY(!(a <= b));
  QVERIFY(!(a < b));
  QVERIFY(!(a >= b));
  QVERIFY(!(a > b));
  a.setValue(0);
  b.clear();
  QVERIFY(!(a == b));
  QVERIFY(a != b);
  QVERIFY(!(a <= b));
  QVERIFY(!(a < b));
  QVERIFY(!(a >= b));
  QVERIFY(!(a > b));
  a.clear();
  QVERIFY(!(a == b));
  QVERIFY(a != b);
  QVERIFY(!(a <= b));
  QVERIFY(!(a < b));
  QVERIFY(!(a >= b));
  QVERIFY(!(a > b));
  // Values comparison
  a.setValue(0);
  b.setValue(0);
  QVERIFY(a == b);
  QVERIFY(!(a != b));
  QVERIFY(a <= b);
  QVERIFY(!(a < b));
  QVERIFY(a >= b);
  QVERIFY(!(a > b));
  a.setValue(0);
  b.setValue(1);
  QVERIFY(!(a == b));
  QVERIFY(a != b);
  QVERIFY(a <= b);
  QVERIFY(a < b);
  QVERIFY(!(a >= b));
  QVERIFY(!(a > b));
  // Additions
  a.clear();
  b.setValue(0);
  mdtValueInt c = a + b;
  QVERIFY(c.isNull());
  a.setValue(1);
  mdtValueInt d = a + b;
  QVERIFY(!d.isNull());
  QCOMPARE(d.value(), 1);
  a.setValue(1);
  c = a + 2;
  QVERIFY(!c.isNull());
  QCOMPARE(c.value(), 3);
  // Soustractions
  a.clear();
  c = a - b;
  QVERIFY(c.isNull());
  a.setValue(5);
  b.setValue(3);
  c = a - b;
  QVERIFY(!c.isNull());
  QCOMPARE(c.value(), 2);
  c = c - b;
  QVERIFY(!c.isNull());
  QCOMPARE(c.value(), -1);
  // Multiplication
  a.clear();
  b.clear();
  c = a * b;
  QVERIFY(c.isNull());
  a.setValue(1);
  c = a * b;
  QVERIFY(c.isNull());
  b.setValue(2);
  c = a * b;
  QVERIFY(!c.isNull());
  QCOMPARE(c.value(), 2);

}

void mdtValueTest::valueIntBenchmark()
{
  QBENCHMARK{
    mdtValueInt a, b;
    a.setValue(2);
    b = a;
    if(a != b){
      QFAIL("Comparison failure");
    }
  }
}

void mdtValueTest::valueBoolTest()
{
  mdtValueBool a;

  // Initial state
  QVERIFY(a.isNull());
  QVERIFY(!a.value());

  // Set value
  a.setValue(false);
  QVERIFY(!a.isNull());
  QVERIFY(!a.value());
  a.setValue(true);
  QVERIFY(!a.isNull());
  QVERIFY(a.value());

  // Copy constructor
  mdtValueBool b(a);
  QVERIFY(!b.isNull());
  QVERIFY(b.value());

  // Clear
  a.clear();
  QVERIFY(a.isNull());
  QVERIFY(!a.value());

  /*
   * Assignment
   */
  b.clear();
  a.setValue(true);
  b = a;
  QVERIFY(!b.isNull());
  QVERIFY(b.value());
  a = true;
  QVERIFY(!a.isNull());
  QVERIFY(a.value() == true);
  a = false;
  QVERIFY(!a.isNull());
  QVERIFY(a.value() == false);
  /*
   * Comparaison
   */
  // mdtValueBool - bool comparison
  a = false;
  QVERIFY(a == false);
  QVERIFY(!(a != false));
  QVERIFY(!(a == true));
  QVERIFY(a != true);
  a = true;
  QVERIFY(a == true);
  QVERIFY(!(a != true));
  QVERIFY(!(a == false));
  QVERIFY(a != false);
  a.clear();
  QVERIFY(!(a == false));
  QVERIFY(!(a == true));
  QVERIFY(a != false);
  QVERIFY(a != true);
  // mdtValueBool - mdtValueBool comparison
  a = false;
  b = false;
  QVERIFY(a == b);
  QVERIFY(!(a != b));
  a = true;
  QVERIFY(!(a == b));
  QVERIFY(a != b);
  b = true;
  QVERIFY(a == b);
  QVERIFY(!(a != b));
  a = false;
  QVERIFY(!(a == b));
  QVERIFY(a != b);
  a.clear();
  QVERIFY(!(a == b));
  QVERIFY(a != b);
  b.clear();
  QVERIFY(!(a == b));
  QVERIFY(a != b);
  b = true;
  QVERIFY(!(a == b));
  QVERIFY(a != b);
  a = true;
  QVERIFY(a == b);
  QVERIFY(!(a != b));
}

void mdtValueTest::valueBoolBenchmark()
{
  QBENCHMARK{
    mdtValueBool a, b;
    a.setValue(true);
    b = a;
    if(!b.value()){
      QFAIL("Comparison failure");
    }
  }
}

void mdtValueTest::valueTest()
{
  QVariant var;

  mdtValue v1;
  // Initial values/flags
  QVERIFY(!v1.isValid());
  QVERIFY(!v1.hasValueDouble());
  QVERIFY(!v1.hasValueInt());
  QVERIFY(!v1.isMinusOl());
  QVERIFY(!v1.isPlusOl());
  QVERIFY(!v1.hasValueBool());
  QCOMPARE(v1.valueDouble(), 0.0);
  QCOMPARE(v1.valueInt(), 0);
  QCOMPARE(v1.valueBool(), false);
  // Set a double value
  v1.setValue(1.0);
  QVERIFY(v1.isValid());
  QVERIFY(v1.hasValueDouble());
  QVERIFY(!v1.hasValueInt());
  QVERIFY(!v1.isMinusOl());
  QVERIFY(!v1.isPlusOl());
  QVERIFY(!v1.hasValueBool());
  QCOMPARE(v1.valueDouble(), 1.0);
  QCOMPARE(v1.valueInt(), 0);
  QCOMPARE(v1.valueBool(), false);
  // Check clear
  v1.setDefaultValue(56.7);
  v1.setDefaultValue(55);
  v1.setDefaultValue(true);
  v1.clear();
  QVERIFY(!v1.isValid());
  QVERIFY(!v1.hasValueDouble());
  QVERIFY(!v1.hasValueInt());
  QVERIFY(!v1.isMinusOl());
  QVERIFY(!v1.isPlusOl());
  QVERIFY(!v1.hasValueBool());
  QCOMPARE(v1.valueDouble(), 56.7);
  QCOMPARE(v1.valueInt(), 55);
  QCOMPARE(v1.valueBool(), true);
  v1.setDefaultValue(0.0);
  v1.setDefaultValue(0);
  v1.setDefaultValue(false);
  v1.clear();
  QVERIFY(!v1.isValid());
  QVERIFY(!v1.hasValueDouble());
  QVERIFY(!v1.hasValueInt());
  QVERIFY(!v1.isMinusOl());
  QVERIFY(!v1.isPlusOl());
  QVERIFY(!v1.hasValueBool());
  QCOMPARE(v1.valueDouble(), 0.0);
  QCOMPARE(v1.valueInt(), 0);
  QCOMPARE(v1.valueBool(), false);
  // Set a double value with -OL flag
  v1.setValue(-1.1, true);
  QVERIFY(v1.isValid());
  QVERIFY(v1.hasValueDouble());
  QVERIFY(!v1.hasValueInt());
  QVERIFY(v1.isMinusOl());
  QVERIFY(!v1.isPlusOl());
  QVERIFY(!v1.hasValueBool());
  QCOMPARE(v1.valueDouble(), -1.1);
  QCOMPARE(v1.valueInt(), 0);
  QCOMPARE(v1.valueBool(), false);
  // Set a double value with +OL flag
  v1.setValue(1.1, false, true);
  QVERIFY(v1.isValid());
  QVERIFY(v1.hasValueDouble());
  QVERIFY(!v1.hasValueInt());
  QVERIFY(!v1.isMinusOl());
  QVERIFY(v1.isPlusOl());
  QVERIFY(!v1.hasValueBool());
  QCOMPARE(v1.valueDouble(), 1.1);
  QCOMPARE(v1.valueInt(), 0);
  QCOMPARE(v1.valueBool(), false);
  // Set a int value , OL flags are overwritten
  v1.setValue(10);
  QVERIFY(v1.isValid());
  QVERIFY(v1.hasValueDouble());
  QVERIFY(v1.hasValueInt());
  QVERIFY(!v1.isMinusOl());
  QVERIFY(!v1.isPlusOl());
  QVERIFY(!v1.hasValueBool());
  QCOMPARE(v1.valueDouble(), 1.1);
  QCOMPARE(v1.valueInt(), 10);
  QCOMPARE(v1.valueBool(), false);
  // Set a int value with -OL flag
  v1.setValue(-11, true);
  QVERIFY(v1.isValid());
  QVERIFY(v1.hasValueDouble());
  QVERIFY(v1.hasValueInt());
  QVERIFY(v1.isMinusOl());
  QVERIFY(!v1.isPlusOl());
  QVERIFY(!v1.hasValueBool());
  QCOMPARE(v1.valueDouble(), 1.1);
  QCOMPARE(v1.valueInt(), -11);
  QCOMPARE(v1.valueBool(), false);
  // Set a int value with +OL flag
  v1.setValue(11, false, true);
  QVERIFY(v1.isValid());
  QVERIFY(v1.hasValueDouble());
  QVERIFY(v1.hasValueInt());
  QVERIFY(!v1.isMinusOl());
  QVERIFY(v1.isPlusOl());
  QVERIFY(!v1.hasValueBool());
  QCOMPARE(v1.valueDouble(), 1.1);
  QCOMPARE(v1.valueInt(), 11);
  QCOMPARE(v1.valueBool(), false);
  // Set a bool value , OL flags are not changed
  v1.setValue(true);
  QVERIFY(v1.isValid());
  QVERIFY(v1.hasValueDouble());
  QVERIFY(v1.hasValueInt());
  QVERIFY(!v1.isMinusOl());
  QVERIFY(v1.isPlusOl());
  QVERIFY(v1.hasValueBool());
  QCOMPARE(v1.valueDouble(), 1.1);
  QCOMPARE(v1.valueInt(), 11);
  QCOMPARE(v1.valueBool(), true);
  // Check clear
  v1.clear();
  QVERIFY(!v1.isValid());
  QVERIFY(!v1.hasValueDouble());
  QVERIFY(!v1.hasValueInt());
  QVERIFY(!v1.isMinusOl());
  QVERIFY(!v1.isPlusOl());
  QVERIFY(!v1.hasValueBool());
  QCOMPARE(v1.valueDouble(), 0.0);
  QCOMPARE(v1.valueInt(), 0);
  QCOMPARE(v1.valueBool(), false);

  mdtValue v2(2.0);
  // Initial values/flags
  QVERIFY(v2.isValid());
  QVERIFY(v2.hasValueDouble());
  QVERIFY(!v2.hasValueInt());
  QVERIFY(!v2.isMinusOl());
  QVERIFY(!v2.isPlusOl());
  QVERIFY(!v2.hasValueBool());
  QCOMPARE(v2.valueDouble(), 2.0);
  QCOMPARE(v2.valueInt(), 0);
  QCOMPARE(v2.valueBool(), false);

  mdtValue v3(3);
  // Initial values/flags
  QVERIFY(v3.isValid());
  QVERIFY(!v3.hasValueDouble());
  QVERIFY(v3.hasValueInt());
  QVERIFY(!v3.isMinusOl());
  QVERIFY(!v3.isPlusOl());
  QVERIFY(!v3.hasValueBool());
  QCOMPARE(v3.valueDouble(), 0.0);
  QCOMPARE(v3.valueInt(), 3);
  QCOMPARE(v3.valueBool(), false);

  mdtValue v4(true);
  // Initial values/flags
  QVERIFY(v4.isValid());
  QVERIFY(!v4.hasValueDouble());
  QVERIFY(!v4.hasValueInt());
  QVERIFY(!v4.isMinusOl());
  QVERIFY(!v4.isPlusOl());
  QVERIFY(v4.hasValueBool());
  QCOMPARE(v4.valueDouble(), 0.0);
  QCOMPARE(v4.valueInt(), 0);
  QCOMPARE(v4.valueBool(), true);

  // Comparaison test
  QVERIFY(v2 != v3);
  QVERIFY(!(v2 == v3));
  QVERIFY(v2 != v4);
  QVERIFY(!(v2 == v4));

  // Check copy constructor
  mdtValue v5(v4);
  QVERIFY(v5.isValid());
  QVERIFY(!v5.hasValueDouble());
  QVERIFY(!v5.hasValueInt());
  QVERIFY(!v5.isMinusOl());
  QVERIFY(!v5.isPlusOl());
  QVERIFY(v5.hasValueBool());
  QCOMPARE(v5.valueDouble(), 0.0);
  QCOMPARE(v5.valueInt(), 0);
  QCOMPARE(v5.valueBool(), true);
  QVERIFY(v4 == v5);
  QVERIFY(!(v4 != v5));
  QVERIFY(v5 != v2);

  // Check copy operator
  v5 = v2;
  QVERIFY(v5.isValid());
  QVERIFY(v5.hasValueDouble());
  QVERIFY(!v5.hasValueInt());
  QVERIFY(!v5.isMinusOl());
  QVERIFY(!v5.isPlusOl());
  QVERIFY(!v5.hasValueBool());
  QCOMPARE(v5.valueDouble(), 2.0);
  QCOMPARE(v5.valueInt(), 0);
  QCOMPARE(v5.valueBool(), false);
  QVERIFY(v5 == v2);

  // Check QVariant storage
  var.setValue(v5);
  QVERIFY(var.value<mdtValue>().isValid());
  QVERIFY(var.value<mdtValue>().hasValueDouble());
  QVERIFY(!var.value<mdtValue>().hasValueInt());
  QVERIFY(!var.value<mdtValue>().isMinusOl());
  QVERIFY(!var.value<mdtValue>().isPlusOl());
  QVERIFY(!var.value<mdtValue>().hasValueBool());
  QCOMPARE(var.value<mdtValue>().valueDouble(), 2.0);
  QCOMPARE(var.value<mdtValue>().valueInt(), 0);
  QCOMPARE(var.value<mdtValue>().valueBool(), false);
  QVERIFY(var.value<mdtValue>() == v2);

  v1.clear();
  QVERIFY(!v1.isValid());
  v1 = 5;
  QVERIFY(v1.isValid());
  QVERIFY(!v1.hasValueBool());
  QVERIFY(v1.hasValueInt());
  QVERIFY(!v1.hasValueDouble());
  QCOMPARE(v1.valueInt(), 5);
  v1 = 1.2;
  QVERIFY(v1.isValid());
  QVERIFY(!v1.hasValueBool());
  QVERIFY(!v1.hasValueInt());
  QVERIFY(v1.hasValueDouble());
  QCOMPARE(v1.valueDouble(), 1.2);
  v1 = true;
  QVERIFY(v1.isValid());
  QVERIFY(v1.hasValueBool());
  QVERIFY(!v1.hasValueInt());
  QVERIFY(!v1.hasValueDouble());
  QCOMPARE(v1.valueBool(), true);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtValueTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
