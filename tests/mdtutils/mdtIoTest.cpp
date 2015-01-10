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
#include "mdtIoTest.h"
#include "mdtApplication.h"
#include "mdtAbstractIo.h"
#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"
#include "mdtBinaryDoubleConverter.h"
#include "mdtValue.h"
#include <QVariant>

//#include <QDebug>

void mdtIoTest::binaryDoubleConverterTest()
{
  mdtBinaryDoubleConverter<int> c;
  int m;
  double x;

  /*
   * Check construction of various types with default parameters
   */
  // Signed 8 bit
  mdtBinaryDoubleConverter<qint8> c8(true);
  QCOMPARE(c8.minimum(), -128.0);
  QCOMPARE(c8.maximum(), 127.0);
  m = c8.doubleToBinary(0.0);
  QCOMPARE(m, 0);
  x = c8.binaryToDouble(m);
  QCOMPARE(x, 0.0);
  m = c8.doubleToBinary(1.0);
  QCOMPARE(m, 1);
  x = c8.binaryToDouble(m);
  QCOMPARE(x, 1.0);
  m = c8.doubleToBinary(-1.0);
  QCOMPARE(m, -1);
  x = c8.binaryToDouble(m);
  QCOMPARE(x, -1.0);
  m = c8.doubleToBinary(127.0);
  QCOMPARE(m, 127);
  x = c8.binaryToDouble(m);
  QCOMPARE(x, 127.0);
  m = c8.doubleToBinary(-128.0);
  QCOMPARE(m, -128);
  x = c8.binaryToDouble(m);
  QCOMPARE(x, -128.0);
  // Unsigned 8 bit
  mdtBinaryDoubleConverter<quint8> cu8(false);
  QCOMPARE(cu8.minimum(), 0.0);
  QCOMPARE(cu8.maximum(), 255.0);
  m = cu8.doubleToBinary(0.0);
  QCOMPARE(m, 0);
  x = cu8.binaryToDouble(m);
  QCOMPARE(x, 0.0);
  m = cu8.doubleToBinary(1.0);
  QCOMPARE(m, 1);
  x = cu8.binaryToDouble(m);
  QCOMPARE(x, 1.0);
  m = cu8.doubleToBinary(255.0);
  QCOMPARE(m, 255);
  x = cu8.binaryToDouble(m);
  QCOMPARE(x, 255.0);
  // Signed 16 bit
  mdtBinaryDoubleConverter<qint16> c16(true);
  QCOMPARE(c16.minimum(), -32768.0);
  QCOMPARE(c16.maximum(), 32767.0);
  m = c16.doubleToBinary(0.0);
  QCOMPARE(m, 0);
  x = c16.binaryToDouble(m);
  QCOMPARE(x, 0.0);
  m = c16.doubleToBinary(1.0);
  QCOMPARE(m, 1);
  x = c16.binaryToDouble(m);
  QCOMPARE(x, 1.0);
  m = c16.doubleToBinary(-1.0);
  QCOMPARE(m, -1);
  x = c16.binaryToDouble(m);
  QCOMPARE(x, -1.0);
  m = c16.doubleToBinary(32767.0);
  QCOMPARE(m, 32767);
  x = c16.binaryToDouble(m);
  QCOMPARE(x, 32767.0);
  m = c16.doubleToBinary(-32768.0);
  QCOMPARE(m, -32768);
  x = c16.binaryToDouble(m);
  QCOMPARE(x, -32768.0);
  // Unsigned 16 bit
  mdtBinaryDoubleConverter<quint16> cu16(false);
  QCOMPARE(cu16.minimum(), 0.0);
  QCOMPARE(cu16.maximum(), 65535.0);
  m = cu16.doubleToBinary(0.0);
  QCOMPARE(m, 0);
  x = cu16.binaryToDouble(m);
  QCOMPARE(x, 0.0);
  m = cu16.doubleToBinary(1.0);
  QCOMPARE(m, 1);
  x = cu16.binaryToDouble(m);
  QCOMPARE(x, 1.0);
  m = cu16.doubleToBinary(65535.0);
  QCOMPARE(m, 65535);
  x = cu16.binaryToDouble(m);
  QCOMPARE(x, 65535.0);
  /*
   * Check with int binary format and range scaling enabled
   */
  // 0...10V range with 8 bits resolution
  c.setRange(0, 10, 8);
  x = c.binaryToDouble(0);
  MDT_COMPARE(x, 0.0, 8, 0.0, 10.0);
  x = c.binaryToDouble(127);
  MDT_COMPARE(x, 5.0, 8, 0.0, 10.0);
  x = c.binaryToDouble(255);
  MDT_COMPARE(x, 10.0, 8, 0.0, 10.0);
  m = c.doubleToBinary(0.0);
  MDT_COMPARE(m, 0, 8, 0.0, 10.0);
  m = c.doubleToBinary(5.0);
  MDT_COMPARE(m, 127, 8, 0.0, 10.0);
  m = c.doubleToBinary(10.0);
  MDT_COMPARE(m, 255, 8, 0.0, 10.0);
  // 4...20mA range with 8 bits resolution
  c.setRange(4, 20, 8);
  x = c.binaryToDouble(0);
  MDT_COMPARE(x, 4.0, 8, 0.0, 10.0);
  x = c.binaryToDouble(127);
  MDT_COMPARE(x, 12.0, 8, 0.0, 10.0);
  x = c.binaryToDouble(255);
  MDT_COMPARE(x, 20.0, 8, 0.0, 10.0);
  m = c.doubleToBinary(0.0);
  MDT_COMPARE(m, 0, 8, 0.0, 10.0);
  m = c.doubleToBinary(12.0);
  MDT_COMPARE(m, 127, 8, 0.0, 10.0);
  m = c.doubleToBinary(20.0);
  MDT_COMPARE(m, 255, 8, 0.0, 10.0);
}

void mdtIoTest::wagoAnalogInputIoConversionTest()
{
  mdtBinaryDoubleConverter<int> c;
  double x;

  // Wago 750-457 - AI -10 to +10 V
  c.setRange(-10.0, 10.0, 13, 3, true);
  x = c.binaryToDouble(0x8000);
  MDT_COMPARE(x, -10.0, 13, -10.0, 10.0);
  x = c.binaryToDouble(0x9998);
  MDT_COMPARE(x, -8.0, 13, -10.0, 10.0);
  x = c.binaryToDouble(0xB334);
  MDT_COMPARE(x, -6.0, 13, -10.0, 10.0);
  x = c.binaryToDouble(0x0000);
  MDT_COMPARE(x, 0.0, 13, -10.0, 10.0);
  x = c.binaryToDouble(0x3330);
  MDT_COMPARE(x, 4.0, 13, -10.0, 10.0);
  x = c.binaryToDouble(0x7FFC);
  MDT_COMPARE(x, 10.0, 13, -10.0, 10.0);

  // Wago 750-483 - AI 0 to 30 V
  c.setRange(0.0, 30.0, 14, 1, false);
  x = c.binaryToDouble(0x0000);
  MDT_COMPARE(x, 0.0, 14, 0.0, 30.0);
  x = c.binaryToDouble(0x1554);
  MDT_COMPARE(x, 5.0, 14, 0.0, 30.0);
  x = c.binaryToDouble(0x2AAA);
  MDT_COMPARE(x, 10.0, 14, 0.0, 30.0);
  x = c.binaryToDouble(0x4000);
  MDT_COMPARE(x, 15.0, 14, 0.0, 30.0);
  x = c.binaryToDouble(0x5554);
  MDT_COMPARE(x, 20.0, 14, 0.0, 30.0);
  x = c.binaryToDouble(0x6AAA);
  MDT_COMPARE(x, 25.0, 14, 0.0, 30.0);
  x = c.binaryToDouble(0x7FFF);
  MDT_COMPARE(x, 30.0, 14, 0.0, 30.0);

  // Wago 750-464 - AI RTD - Pt100/Pt1000, C2 format, -200 to 850°C
  c.setRange(-200.0, 850.0, 16, 0, true, false, 0.1);
  x = c.binaryToDouble(0xF830);
  MDT_COMPARE(x, -200.0, 16, -200.0, 850.0);
  x = c.binaryToDouble(0xFC18);
  MDT_COMPARE(x, -100.0, 16, -200.0, 850.0);
  x = c.binaryToDouble(0x0000);
  MDT_COMPARE(x, 0.0, 16, -200.0, 850.0);
  x = c.binaryToDouble(0x03E8);
  MDT_COMPARE(x, 100.0, 16, -200.0, 850.0);
  x = c.binaryToDouble(0x07D0);
  MDT_COMPARE(x, 200.0, 16, -200.0, 850.0);
  x = c.binaryToDouble(0x1388);
  MDT_COMPARE(x, 500.0, 16, -200.0, 850.0);
  x = c.binaryToDouble(0x1D4C);
  MDT_COMPARE(x, 750.0, 16, -200.0, 850.0);
  x = c.binaryToDouble(0x1F40);
  MDT_COMPARE(x, 800.0, 16, -200.0, 850.0);
  x = c.binaryToDouble(0x2134);
  MDT_COMPARE(x, 850.0, 16, -200.0, 850.0);
}

void mdtIoTest::wagoAnalogOutputIoConversionTest()
{
  mdtBinaryDoubleConverter<int> c;
  double x;
  quint16 m;

  /*
   * 750-550 - AO - 0 to 10 V
   */
  c.setRange(0.0, 10.0, 12, 3, false);
  // Read test
  x = c.binaryToDouble(0x0000);
  MDT_COMPARE(x, 0.0, 12, 0.0, 10.0);
  x = c.binaryToDouble(0x1000);
  MDT_COMPARE(x, 1.25, 12, 0.0, 10.0);
  x = c.binaryToDouble(0x2000);
  MDT_COMPARE(x, 2.5, 12, 0.0, 10.0);
  x = c.binaryToDouble(0x3000);
  MDT_COMPARE(x, 3.75, 12, 0.0, 10.0);
  x = c.binaryToDouble(0x4000);
  MDT_COMPARE(x, 5.0, 12, 0.0, 10.0);
  x = c.binaryToDouble(0x5000);
  MDT_COMPARE(x, 6.25, 12, 0.0, 10.0);
  x = c.binaryToDouble(0x6000);
  MDT_COMPARE(x, 7.5, 12, 0.0, 10.0);
  x = c.binaryToDouble(0x7000);
  MDT_COMPARE(x, 8.75, 12, 0.0, 10.0);
  x = c.binaryToDouble(0x7FFF);
  MDT_COMPARE(x, 10.0, 12, 0.0, 10.0);
  // Write test - We take values from table in manual
  m = c.doubleToBinary(0.0);
  MDT_COMPARE(m, 0x0000, 12, 0, 4095);
  m = c.doubleToBinary(1.25);
  MDT_COMPARE(m, 0x1000, 12, 0, 4095);
  m = c.doubleToBinary(2.5);
  MDT_COMPARE(m, 0x2000, 12, 0, 4095);
  m = c.doubleToBinary(3.75);
  MDT_COMPARE(m, 0x3000, 12, 0, 4095);
  m = c.doubleToBinary(5.0);
  MDT_COMPARE(m, 0x4000, 12, 0, 4095);
  m = c.doubleToBinary(6.25);
  MDT_COMPARE(m, 0x5000, 12, 0, 4095);
  m = c.doubleToBinary(7.5);
  MDT_COMPARE(m, 0x6000, 12, 0, 4095);
  m = c.doubleToBinary(8.75);
  MDT_COMPARE(m, 0x7000, 12, 0, 4095);
  m = c.doubleToBinary(10.0);
  /*
   * There is a little error in Wago datasheet: max value with 12 bits is 0xFFF.
   * Shifted by 3 bits on the right: 0xFFF*2^3=0x7FF8 (not 0x7FFF)
   */
  MDT_COMPARE(m, 0x7FF8, 12, 0, 4096);

  /*
   * 750-556 - AO - -10 to +10 V
   */
  c.setRange(-10.0, 10.0, 13, 3, true);
  // Read test
  x = c.binaryToDouble(0x8001);
  MDT_COMPARE(x, -10.0, 13, -10.0, 10.0);
  x = c.binaryToDouble(0xA000);
  MDT_COMPARE(x, -7.5, 13, -10.0, 10.0);
  x = c.binaryToDouble(0xC000);
  MDT_COMPARE(x, -5.0, 13, -10.0, 10.0);
  x = c.binaryToDouble(0xE000);
  MDT_COMPARE(x, -2.5, 13, -10.0, 10.0);
  x = c.binaryToDouble(0x0000);
  MDT_COMPARE(x, 0.0, 13, -10.0, 10.0);
  x = c.binaryToDouble(0x2000);
  MDT_COMPARE(x, 2.5, 13, -10.0, 10.0);
  x = c.binaryToDouble(0x4000);
  MDT_COMPARE(x, 5.0, 13, -10.0, 10.0);
  x = c.binaryToDouble(0x6000);
  MDT_COMPARE(x, 7.5, 13, -10.0, 10.0);
  x = c.binaryToDouble(0x7FFF);
  MDT_COMPARE(x, 10.0, 13, -10.0, 10.0);
  // Write test - We take values from table in manual (1 bit approximate)
  m = c.doubleToBinary(-10.0);
  MDT_COMPARE(m, 0x8001, 13, 0, 8192);
  m = c.doubleToBinary(-7.5);
  MDT_COMPARE(m, 0xA000, 13, 0, 8192);
  m = c.doubleToBinary(-5.0);
  MDT_COMPARE(m, 0xC000, 13, 0, 8192);
  m = c.doubleToBinary(-2.5);
  MDT_COMPARE(m, 0xE000, 13, 0, 8192);
  m = c.doubleToBinary(0.0);
  MDT_COMPARE(m, 0x0000, 13, 0, 8192);
  m = c.doubleToBinary(2.5);
  MDT_COMPARE(m, 0x2000, 13, 0, 8192);
  m = c.doubleToBinary(5.0);
  MDT_COMPARE(m, 0x4000, 13, 0, 8192);
  m = c.doubleToBinary(7.5);
  MDT_COMPARE(m, 0x6000, 13, 0, 8192);
  m = c.doubleToBinary(10.0);
  /*
   * There is a little error in Wago datasheet: max value with 12 bits is 0xFFF.
   * Shifted by 3 bits on the right: 0xFFF*2^3=0x7FF8 (not 0x7FFF)
   */
  MDT_COMPARE(m, 0x7FF8, 13, 0, 8192);
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
  ///QVERIFY(io.setRange(0, 10, 8));
  io.setRange(0, 10, 8);
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
  ///QVERIFY(io.setRange(4, 20, 8));
  io.setRange(4, 20, 8);
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
  ///QVERIFY(!io.setRange(0.0, 10.0, 8*sizeof(int), 1, true));
  ///QVERIFY(io.setRange(0.0, 10.0, 8*sizeof(int), 0, true));
  io.setRange(0.0, 10.0, 8*sizeof(int), 0, true);
  ///QVERIFY(io.setRange(0.0, 10.0, 0, 8*sizeof(int), true));
  io.setRange(0.0, 10.0, 0, 8*sizeof(int), true);
  ///QVERIFY(!io.setRange(0.0, 10.0, 1, 8*sizeof(int), true));

  // 0...10V range with 8 bits resolution
  ///QVERIFY(io.setRange(0, 10, 8));
  io.setRange(0, 10, 8);
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

  // Check that a to big value not overflows - Must be clipped to max possible value, 0xFF (255) with 8 bits
  io.setValue(11.0, false);
  MDT_COMPARE(io.value().valueInt(), 255, 8, 0.0, 255);
  // Chack that a to little value does not underflow - Must be clipped to min possible value (0x00)
  io.setValue(-1.0, false);
  MDT_COMPARE(io.value().valueInt(), 0, 8, 0.0, 255);
}

void mdtIoTest::wagoAnalogInputTest()
{
  mdtAnalogIo io;

  // Wago 750-457 - AI -10 to +10 V
  ///QVERIFY(io.setRange(-10.0, 10.0, 13, 3, true));
  io.setRange(-10.0, 10.0, 13, 3, true);
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
  ///QVERIFY(io.setRange(0.0, 30.0, 14, 1, false));
  io.setRange(0.0, 30.0, 14, 1, false);
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

  // Wago 750-464 - AI RTD - Pt100/Pt1000, C2 format, -200 to 850°C
  ///QVERIFY(io.setRange(-200.0, 850.0, 16, 0, true, false, 0.1));
  io.setRange(-200.0, 850.0, 16, 0, true, false, 0.1);
  io.setValueInt(0xF830, true, false);
  MDT_COMPARE(io.value().valueDouble(), -200.0, 16, -200.0, 850.0);
  io.setValueInt(0xFC18, true, false);
  MDT_COMPARE(io.value().valueDouble(), -100.0, 16, -200.0, 850.0);
  io.setValueInt(0x0000, true, false);
  MDT_COMPARE(io.value().valueDouble(), 0.0, 16, -200.0, 850.0);
  io.setValueInt(0x03E8, true, false);
  MDT_COMPARE(io.value().valueDouble(), 100.0, 16, -200.0, 850.0);
  io.setValueInt(0x07D0, true, false);
  MDT_COMPARE(io.value().valueDouble(), 200.0, 16, -200.0, 850.0);
  io.setValueInt(0x1388, true, false);
  MDT_COMPARE(io.value().valueDouble(), 500.0, 16, -200.0, 850.0);
  io.setValueInt(0x1D4C, true, false);
  MDT_COMPARE(io.value().valueDouble(), 750.0, 16, -200.0, 850.0);
  io.setValueInt(0x1F40, true, false);
  MDT_COMPARE(io.value().valueDouble(), 800.0, 16, -200.0, 850.0);
  io.setValueInt(0x2134, true, false);
  MDT_COMPARE(io.value().valueDouble(), 850.0, 16, -200.0, 850.0);
}

void mdtIoTest::wagoAnalogOutputTest()
{
  mdtAnalogIo io;

  /*
   * 750-550 - AO - 0 to 10 V
   */
  ///QVERIFY(io.setRange(0.0, 10.0, 12, 3, false));
  io.setRange(0.0, 10.0, 12, 3, false);
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
  // Write test - We take values from table in manual
  io.setValue(0.0, true);
  MDT_COMPARE(io.value().valueDouble(), 0.0, 12, 0, 10.0);
  MDT_COMPARE(io.value().valueInt(), 0x0000, 12, 0, 4096);
  io.setValue(1.25, true);
  MDT_COMPARE(io.value().valueInt(), 0x1000, 12, 0, 4096);
  io.setValue(2.5, true);
  MDT_COMPARE(io.value().valueInt(), 0x2000, 12, 0, 4096);
  io.setValue(3.75, true);
  MDT_COMPARE(io.value().valueInt(), 0x3000, 12, 0, 4096);
  io.setValue(5.0, true);
  MDT_COMPARE(io.value().valueInt(), 0x4000, 12, 0, 4096);
  io.setValue(6.25, true);
  MDT_COMPARE(io.value().valueInt(), 0x5000, 12, 0, 4096);
  io.setValue(7.5, true);
  MDT_COMPARE(io.value().valueInt(), 0x6000, 12, 0, 4096);
  io.setValue(8.75, true);
  MDT_COMPARE(io.value().valueInt(), 0x7000, 12, 0, 4096);
  io.setValue(10.0, true);
  /*
   * There is a little error in Wago datasheet: max value with 12 bits is 0xFFF.
   * Shifted by 3 bits on the right: 0xFFF*2^3=0x7FF8 (not 0x7FFF)
   */
  MDT_COMPARE(io.value().valueInt(), 0x7FF8, 12, 0, 4096);

  /*
   * 750-556 - AO - -10 to +10 V
   */
  ///QVERIFY(io.setRange(-10.0, 10.0, 13, 3, true));
  io.setRange(-10.0, 10.0, 13, 3, true);
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
  MDT_COMPARE(io.value().valueInt(), 0x2000, 13, 0, 8192);
  io.setValue(5.0, true);
  MDT_COMPARE(io.value().valueInt(), 0x4000, 13, 0, 8192);
  io.setValue(7.5, true);
  MDT_COMPARE(io.value().valueInt(), 0x6000, 13, 0, 8192);
  io.setValue(10.0, true);
  /*
   * There is a little error in Wago datasheet: max value with 12 bits is 0xFFF.
   * Shifted by 3 bits on the right: 0xFFF*2^3=0x7FF8 (not 0x7FFF)
   */
  MDT_COMPARE(io.value().valueInt(), 0x7FF8, 13, 0, 8192);
}

void mdtIoTest::digitalIoTest()
{
  mdtDigitalIo io;

  // Initial values
  QCOMPARE(io.value().valueBool(), false);
  QVERIFY(!io.hasValidData());

  // Toggle
  io.setValue(true);
  QVERIFY(io.hasValidData());
  QCOMPARE(io.value().valueBool(), true);
  io.setValue(false);
  QVERIFY(io.hasValidData());
  QCOMPARE(io.value().valueBool(), false);
  // Set invalid state
  io.setValue(mdtValue());
  QVERIFY(!io.hasValidData());
  QCOMPARE(io.value().valueBool(), false);
  // Set valid state
  io.setValue(true);
  QVERIFY(io.hasValidData());
  QCOMPARE(io.value().valueBool(), true);
  // Check setOn(QVariant)
  io.setValue(mdtValue());
  QVERIFY(!io.hasValidData());
  io.setValue(true);
  QVERIFY(io.hasValidData());
  QCOMPARE(io.value().valueBool(), true);
  io.setValue(false);
  QVERIFY(io.hasValidData());
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
