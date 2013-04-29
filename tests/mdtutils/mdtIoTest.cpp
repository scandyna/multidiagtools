/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtIoTest.h"
#include "mdtApplication.h"
#include "mdtAbstractIo.h"
#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"
#include "mdtValue.h"

//#include <QDebug>

void mdtIoTest::mdtValueTest()
{
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
}

void mdtIoTest::mdtAbstractIoTest()
{
  mdtAbstractIo io;

  // Initial states
  QVERIFY(io.address() == 0);
  QVERIFY(io.labelShort() == "");
  QVERIFY(io.label() == "");
  QVERIFY(io.details() == "");
  QVERIFY(!io.hasValidData());

  // Set some data and check
  io.setAddress(25);
  io.setLabelShort("Label short");
  io.setLabel("A label");
  io.setDetails("Details");
  QCOMPARE(io.addressRead(), 25);
  QCOMPARE(io.addressWrite(), 25);
  QCOMPARE(io.address(), 25);
  QVERIFY(io.labelShort() == "Label short");
  QVERIFY(io.label() == "A label");
  QVERIFY(io.details() == "Details");
  QVERIFY(!io.hasValidData());
  // Check address R/W
  io.setAddressRead(12);
  io.setAddressWrite(15);
  QCOMPARE(io.addressRead(), 12);
  QCOMPARE(io.addressWrite(), 15);
}

void mdtIoTest::analogIoTest()
{
  mdtAnalogIo io;

  // Initial values
  QCOMPARE(io.value().valueDouble(), 0.0);
  QCOMPARE(io.value().valueInt(), 0);
  QVERIFY(!io.hasValidData());

  // 0...10V range with 8 bits resolution
  QVERIFY(io.setRange(0, 10, 8));
  QVERIFY(!io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 0.0, 8, 0.0, 10.0);
  MDT_COMPARE(io.value().valueInt(), 0, 8, 0, 255);
  io.setValueInt(0, true, false);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueInt(), 0, 8, 0, 255);
  io.setValueInt(127, true, false);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 5.0, 8, 0.0, 10.0);
  MDT_COMPARE(io.value().valueInt(), 127, 8, 0, 255);
  io.setValueInt(255, true, false);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 10.0, 8, 0.0, 10.0);
  MDT_COMPARE(io.value().valueInt(), 255, 8, 0, 255);
  // Set invalid value
  ///io.setValue(9.0, false);
  io.setValue(mdtValue(), false);
  QVERIFY(!io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 0.0, 8, 0.0, 10.0);
  MDT_COMPARE(io.value().valueInt(), 0, 8, 0, 255);
  // Set valid value
  io.setValue(8.0, true);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 8.0, 8, 0.0, 10.0);
  // Set invalid value
  io.setValueInt(123, false, false);
  QVERIFY(!io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 0.0, 8, 0.0, 10.0);
  MDT_COMPARE(io.value().valueInt(), 0, 8, 0, 255);

  // 4...20mA range with 8 bits resolution
  QVERIFY(io.setRange(4, 20, 8));
  QVERIFY(!io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 4.0, 8, 4.0, 20.0);
  MDT_COMPARE(io.value().valueInt(), 0, 8, 0, 255);
  io.setValueInt(0, true, false);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueInt(), 0, 8, 0, 255);
  io.setValueInt(127, true, false);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 12.0, 8, 4.0, 20.0);
  MDT_COMPARE(io.value().valueInt(), 127, 8, 0, 255);
  io.setValueInt(255, true, false);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 20.0, 8, 4.0, 20.0);
  MDT_COMPARE(io.value().valueInt(), 255, 8, 0, 255);
  // Set invalid value
  ///io.setValue(9.0, false);
  io.setValue(mdtValue(), false);
  QVERIFY(!io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 4.0, 8, 4.0, 20.0);
  MDT_COMPARE(io.value().valueInt(), 0, 8, 0, 255);
  // Set valid value
  io.setValue(8.0, true);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 8.0, 8, 4.0, 20.0);
  // Set invalid value
  io.setValueInt(123, false, false);
  QVERIFY(!io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 4.0, 8, 4.0, 20.0);
  MDT_COMPARE(io.value().valueInt(), 0, 8, 0, 255);

  // Setup range (limits test)
  QVERIFY(!io.setRange(0.0, 10.0, 8*sizeof(int), 1, true));
  QVERIFY(io.setRange(0.0, 10.0, 8*sizeof(int), 0, true));
  QVERIFY(io.setRange(0.0, 10.0, 0, 8*sizeof(int), true));
  QVERIFY(!io.setRange(0.0, 10.0, 1, 8*sizeof(int), true));

  // 0...10V range with 8 bits resolution
  QVERIFY(io.setRange(0, 10, 8));
  QVERIFY(!io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 0.0, 8, 0.0, 10.0);
  MDT_COMPARE(io.value().valueInt(), 0, 8, 0, 255);
  io.setValueInt(0, true, false);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueInt(), 0, 8, 0, 255);
  io.setValueInt(127, true, false);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 5.0, 8, 0.0, 10.0);
  MDT_COMPARE(io.value().valueInt(), 127, 8, 0, 255);

  // Check setValue(mdtValue)
  io.setValue(mdtValue(), false);
  QVERIFY(!io.hasValidData());
  io.setValue(mdtValue(5.6), false);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 5.6, 8, 0.0, 10.0);
  io.setValue(mdtValue(127), false);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 5.0, 8, 0.0, 10.0);
}

void mdtIoTest::wagoAnalogInputTest()
{
  mdtAnalogIo io;

  // Wago 750-457 - AI -10 to +10 V
  QVERIFY(io.setRange(-10.0, 10.0, 13, 3, true));
  io.setValueInt(0x8000, true, false);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), -10.0, 13, -10.0, 10.0);
  io.setValueInt(0x9998, true, false);
  MDT_COMPARE(io.value().valueDouble(), -8.0, 13, -10.0, 10.0);
  io.setValueInt(0xB334, true, false);
  MDT_COMPARE(io.value().valueDouble(), -6.0, 13, -10.0, 10.0);
  io.setValueInt(0x0000, true, false);
  MDT_COMPARE(io.value().valueDouble(), 0.0, 13, -10.0, 10.0);
  io.setValueInt(0x3330, true, false);
  MDT_COMPARE(io.value().valueDouble(), 4.0, 13, -10.0, 10.0);
  io.setValueInt(0x7FFC, true, false);
  MDT_COMPARE(io.value().valueDouble(), 10.0, 13, -10.0, 10.0);

  // Wago 750-483 - AI 0 to 30 V
  QVERIFY(io.setRange(0.0, 30.0, 14, 1, false));
  io.setValueInt(0x0000, true, false);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 0.0, 14, 0.0, 30.0);
  io.setValueInt(0x1554, true, false);
  MDT_COMPARE(io.value().valueDouble(), 5.0, 14, 0.0, 30.0);
  io.setValueInt(0x2AAA, true, false);
  MDT_COMPARE(io.value().valueDouble(), 10.0, 14, 0.0, 30.0);
  io.setValueInt(0x4000, true, false);
  MDT_COMPARE(io.value().valueDouble(), 15.0, 14, 0.0, 30.0);
  io.setValueInt(0x5554, true, false);
  MDT_COMPARE(io.value().valueDouble(), 20.0, 14, 0.0, 30.0);
  io.setValueInt(0x6AAA, true, false);
  MDT_COMPARE(io.value().valueDouble(), 25.0, 14, 0.0, 30.0);
  io.setValueInt(0x7FFF, true, false);
  MDT_COMPARE(io.value().valueDouble(), 30.0, 14, 0.0, 30.0);
}

void mdtIoTest::wagoAnalogOutputTest()
{
  mdtAnalogIo io;

  /*
   * 750-550 - AO - 0 to 10 V
   */
  QVERIFY(io.setRange(0.0, 10.0, 12, 3, false));
  QVERIFY(io.setEncodeBitSettings(15, 0));
  // Read test
  io.setValueInt(0x0000, true, false);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), 0.0, 12, 0.0, 10.0);
  io.setValueInt(0x1000, true, false);
  MDT_COMPARE(io.value().valueDouble(), 1.25, 12, 0.0, 10.0);
  io.setValueInt(0x2000, true, false);
  MDT_COMPARE(io.value().valueDouble(), 2.5, 12, 0.0, 10.0);
  io.setValueInt(0x3000, true, false);
  MDT_COMPARE(io.value().valueDouble(), 3.75, 12, 0.0, 10.0);
  io.setValueInt(0x4000, true, false);
  MDT_COMPARE(io.value().valueDouble(), 5.0, 12, 0.0, 10.0);
  io.setValueInt(0x5000, true, false);
  MDT_COMPARE(io.value().valueDouble(), 6.25, 12, 0.0, 10.0);
  io.setValueInt(0x6000, true, false);
  MDT_COMPARE(io.value().valueDouble(), 7.5, 12, 0.0, 10.0);
  io.setValueInt(0x7000, true, false);
  MDT_COMPARE(io.value().valueDouble(), 8.75, 12, 0.0, 10.0);
  io.setValueInt(0x7FFF, true, false);
  MDT_COMPARE(io.value().valueDouble(), 10.0, 12, 0.0, 10.0);
  // Write test - We take values from table in manual (1 bit approximate)
  io.setValue(0.0, true);
  MDT_COMPARE(io.value().valueInt(), 0x0000, 12, 0, 4096);
  io.setValue(1.25, true);
  MDT_COMPARE(io.value().valueInt(), 0x0FFF, 12, 0, 4096);
  io.setValue(2.5, true);
  MDT_COMPARE(io.value().valueInt(), 0x1FFF, 12, 0, 4096);
  io.setValue(3.75, true);
  MDT_COMPARE(io.value().valueInt(), 0x2FFF, 12, 0, 4096);
  io.setValue(5.0, true);
  MDT_COMPARE(io.value().valueInt(), 0x3FFF, 12, 0, 4096);
  io.setValue(6.25, true);
  MDT_COMPARE(io.value().valueInt(), 0x4FFF, 12, 0, 4096);
  io.setValue(7.5, true);
  MDT_COMPARE(io.value().valueInt(), 0x5FFF, 12, 0, 4096);
  io.setValue(8.75, true);
  MDT_COMPARE(io.value().valueInt(), 0x6FFF, 12, 0, 4096);
  io.setValue(10.0, true);
  MDT_COMPARE(io.value().valueInt(), 0x7FFF, 12, 0, 4096);

  /*
   * 750-556 - AO - -10 to +10 V
   */
  QVERIFY(io.setRange(-10.0, 10.0, 13, 3, true));
  QVERIFY(io.setEncodeBitSettings(16, 0));
  // Read test
  io.setValueInt(0x8001, true, false);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value().valueDouble(), -10.0, 13, -10.0, 10.0);
  io.setValueInt(0xA000, true, false);
  MDT_COMPARE(io.value().valueDouble(), -7.5, 13, -10.0, 10.0);
  io.setValueInt(0xC000, true, false);
  MDT_COMPARE(io.value().valueDouble(), -5.0, 13, -10.0, 10.0);
  io.setValueInt(0xE000, true, false);
  MDT_COMPARE(io.value().valueDouble(), -2.5, 13, -10.0, 10.0);
  io.setValueInt(0x0000, true, false);
  MDT_COMPARE(io.value().valueDouble(), 0.0, 13, -10.0, 10.0);
  io.setValueInt(0x2000, true, false);
  MDT_COMPARE(io.value().valueDouble(), 2.5, 13, -10.0, 10.0);
  io.setValueInt(0x4000, true, false);
  MDT_COMPARE(io.value().valueDouble(), 5.0, 13, -10.0, 10.0);
  io.setValueInt(0x6000, true, false);
  MDT_COMPARE(io.value().valueDouble(), 7.5, 13, -10.0, 10.0);
  io.setValueInt(0x7FFF, true, false);
  MDT_COMPARE(io.value().valueDouble(), 10.0, 13, -10.0, 10.0);
  // Write test - We take values from table in manual (1 bit approximate)
  io.setValue(-10.0, true);
  MDT_COMPARE(io.value().valueInt(), 0x8001, 13, 0, 8192);
  io.setValue(-7.5, true);
  MDT_COMPARE(io.value().valueInt(), 0xA000, 13, 0, 8192);
  io.setValue(-5.0, true);
  MDT_COMPARE(io.value().valueInt(), 0xC000, 13, 0, 8192);
  io.setValue(-2.5, true);
  MDT_COMPARE(io.value().valueInt(), 0xE000, 13, 0, 8192);
  io.setValue(0.0, true);
  MDT_COMPARE(io.value().valueInt(), 0x0000, 13, 0, 8192);
  io.setValue(2.5, true);
  MDT_COMPARE(io.value().valueInt(), 0x1FFF, 13, 0, 8192);
  io.setValue(5.0, true);
  MDT_COMPARE(io.value().valueInt(), 0x3FFF, 13, 0, 8192);
  io.setValue(7.5, true);
  MDT_COMPARE(io.value().valueInt(), 0x5FFF, 13, 0, 8192);
  io.setValue(10.0, true);
  MDT_COMPARE(io.value().valueInt(), 0x7FFF, 13, 0, 8192);
}

void mdtIoTest::digitalIoTest()
{
  mdtDigitalIo io;

  // Initial values
  QVERIFY(!io.isOn());
  QVERIFY(!io.hasValidData());

  // Toggle
  ///io.setOn(true, true);
  io.setValue(true);
  QVERIFY(io.hasValidData());
  ///QVERIFY(io.isOn());
  QCOMPARE(io.value().valueBool(), true);
  ///io.setOn(false, true);
  io.setValue(false);
  QVERIFY(io.hasValidData());
  ///QVERIFY(!io.isOn());
  QCOMPARE(io.value().valueBool(), false);
  // Set invalid state
  ///io.setOn(true, false);
  io.setValue(mdtValue());
  QVERIFY(!io.hasValidData());
  ///QVERIFY(!io.isOn());
  QCOMPARE(io.value().valueBool(), false);
  // Set valid state
  ///io.setOn(true, true);
  io.setValue(true);
  QVERIFY(io.hasValidData());
  ///QVERIFY(io.isOn());
  QCOMPARE(io.value().valueBool(), true);
  // Check setOn(QVariant)
  ///io.setOn(QVariant());
  io.setValue(mdtValue());
  QVERIFY(!io.hasValidData());
  ///io.setOn(QVariant(true));
  io.setValue(true);
  QVERIFY(io.hasValidData());
  ///QVERIFY(io.isOn());
  QCOMPARE(io.value().valueBool(), true);
  ///io.setOn(QVariant(false));
  io.setValue(false);
  QVERIFY(io.hasValidData());
  ///QVERIFY(!io.isOn());
  QCOMPARE(io.value().valueBool(), false);
}


int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtIoTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
