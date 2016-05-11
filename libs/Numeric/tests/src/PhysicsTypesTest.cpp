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
#include "PhysicsTypesTest.h"
#include "mdtApplication.h"

#include "Mdt/Numeric/Double.h"
#include "Mdt/Numeric/Resistance.h"

#include <boost/units/systems/si/resistance.hpp>
#include <boost/units/systems/si/io.hpp>
#include <iostream>

#include <QDebug>

void PhysicsTypesTest::initTestCase()
{
}

void PhysicsTypesTest::cleanupTestCase()
{
}

/*
 * Sandbox
 */

/// mode strict ?
// template<typename Derived>
// class PhysicsType
// {
//  public:
// 
//   PhysicsType() {}
//   PhysicsType(double x) : pvValue(x){}
// 
//   double value() const
//   {
//     return pvValue;
//   }
//   
//   bool isNull() const
//   {
//     return true;
//   }
// 
//   friend
//   bool operator==(const Derived & lhs, const Derived & rhs)
//   {
//     return lhs.pvValue == rhs.pvValue;
//   }
//   
//   int someDerivedImplemetedFunction()
//   {
//     return static_cast<Derived*>(this)->impl();
//   }
// 
// private:
//   double pvValue;
// };

// class Resistance : public PhysicsType<Resistance>
// {
// public:
// 
//   Resistance() : PhysicsType(){}
//   Resistance(double r) : PhysicsType(r){}
//   int impl()
//   {
//     return 10;
//   }
// };

// class Length : public PhysicsType<Length>
// {
// public:
//   
//   Length() : PhysicsType() {}
//   Length(double l) : PhysicsType(l) {}
//   int impl()
//   {
//     return 105;
//   }
// };

//typedef PhysicsType<double> Length;

/*
 * Tests
 */

// void PhysicsTypesTest::sandbox()
// {
//   using Mdt::Numeric::Double;
//   using boost::units::si::resistance;
//   using boost::units::si::ohm;
// //   using boost::units::si::ohms;
//   using boost::units::quantity;
//   
// //   using namespace boost::units;
// //   using namespace boost::units::si;
// 
//   quantity<resistance, Double> r(2.0 * ohm);
//   
//   ///std::cout << "r: " << r << std::endl;
//   
//   auto r2 = r + 2.0 * ohm;
//   
//   if(r < r2){
//     qDebug() << "(r < r2)";
//   }
//   
//   /**
//   if(r.isNull()){
//     qDebug() << "r is null";
//   }
//   */
//   
//   Resistance r10 = 10.0;
//   qDebug() << "r10 value: " << r10.value() << " impl: " <<  r10.impl();
//   Resistance r11 = 11.0;
//   qDebug() << "r11 value: " << r11.value() << " impl: " <<  r11.impl();
//   qDebug() << "r10 == r11: " << (r10 == r11);
//   qDebug() << "r10 = r11..";
//   r10 = r11;
//   qDebug() << "r10 value: " << r10.value() << " impl: " <<  r10.impl();
//   qDebug() << "r11 value: " << r11.value() << " impl: " <<  r11.impl();
//   qDebug() << "r10 == r11: " << (r10 == r11);
//   
//   Length l20 = 20.0;
//   qDebug() << "l20 value: " << l20.value() << " impl: " <<  l20.impl();
//   Length l21 = 21.0;
//   qDebug() << "l21 value: " << l21.value() << " impl: " <<  l21.impl();
//   qDebug() << "l20 == l21: " << (l20 == l21);
// 
// //   qDebug() << "l20 == r11: " << (l20 == r11);
//   
// //   r10 = l10;
//   
// //   PhysicsType<double> x22 = r10;
//   
// }

void PhysicsTypesTest::resistanceSimpleTest()
{
  using Mdt::Numeric::Resistance;
  using Mdt::Numeric::Double;

  /*
   * Constructions
   */
  // Default constructed
  Resistance r;
  QVERIFY(r.isNull());
  // Contruct with a value
  Resistance r2 = Resistance(1.5);
  QVERIFY(!r2.isNull());
  QCOMPARE(r2.value().toDouble(), 1.5);
  /*
   * Set
   */
  // Set from double
  QVERIFY(r.isNull());
  r = Resistance(2.0);
  QVERIFY(!r.isNull());
  QCOMPARE(r.value().toDouble(), 2.0);
  // Affectation of other Resistance
  r2 = Resistance(1.5);
  r = r2;
  QCOMPARE(r.value().toDouble(), 1.5);
  /*
   * Clear
   */
  QVERIFY(!r.isNull());
  r.clear();
  QVERIFY(r.isNull());
  /*
   * Get from QVariant
   */
  r = Resistance::fromQVariant(10.0);
  QVERIFY(!r.isNull());
  QCOMPARE(r.value().toDouble(), 10.0);
  r = Resistance::fromQVariant(QVariant());
  QVERIFY(r.isNull());
  /*
   * QDebug stream operator
   */
  qDebug() << "r: " << r;
  r = Resistance(2.5e3);
  qDebug() << "r: " << r;
}

void PhysicsTypesTest::resistanceComparisonTest()
{
  using Mdt::Numeric::Resistance;

  Resistance r1, r2;

  r1 = Resistance(1.0);
  r2 = Resistance(1.0);
  QVERIFY(r1 == r2);
  QVERIFY(!(r1 != r2));
  QVERIFY(!(r1 < r2));
  QVERIFY(r1 <= r2);
  QVERIFY(!(r1 > r2));
  QVERIFY(r1 >= r2);
  r1 = Resistance(1.0);
  r2 = Resistance(2.0);
  QVERIFY(!(r1 == r2));
  QVERIFY(r1 != r2);
  QVERIFY(r1 < r2);
  QVERIFY(r1 <= r2);
  QVERIFY(!(r1 > r2));
  QVERIFY(!(r1 >= r2));
}

void PhysicsTypesTest::resistanceMathOperatorsTest()
{
  using Mdt::Numeric::Resistance;

  Resistance r1, r2, r3;

  /*
   * Additions
   */
  r1 = Resistance(1.0);
  r2 = Resistance(2.0);
  r3 = r1 + r2;
  QCOMPARE(r3.value().toDouble(), 3.0);
  r3 = r1 + Resistance(5.0);
  QCOMPARE(r3.value().toDouble(), 6.0);
  /*
   * Subtraction
   */
  r1 = Resistance(10.0);
  r2 = Resistance(2.0);
  r3 = r1 - r2;
  QCOMPARE(r3.value().toDouble(), 8.0);
  r3 = r1 - Resistance(3.0);
  QCOMPARE(r3.value().toDouble(), 7.0);
  /*
   * Negation
   */
  r1 = Resistance(1.0);
  r3 = -r1;
  QCOMPARE(r3.value().toDouble(), -1.0);
  /*
   * Multiplication
   */
  r1 = Resistance(10.0);
  r2 = Resistance(2.0);
  r3 = r1 * r2;
  QCOMPARE(r3.value().toDouble(), 20.0);
  /*
   * Division
   */
  r1 = Resistance(10.0);
  r2 = Resistance(2.0);
  r3 = r1 / r2;
  QCOMPARE(r3.value().toDouble(), 5.0);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  PhysicsTypesTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
