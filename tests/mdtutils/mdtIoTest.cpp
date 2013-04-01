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

//#include <QDebug>

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
  QVERIFY(io.address() == 25);
  QVERIFY(io.labelShort() == "Label short");
  QVERIFY(io.label() == "A label");
  QVERIFY(io.details() == "Details");
  QVERIFY(!io.hasValidData());
}

void mdtIoTest::analogIoTest()
{
  mdtAnalogIo io;

  // Initial values
  QCOMPARE(io.value(), 0.0);
  QCOMPARE(io.valueInt(), 0);
  QVERIFY(!io.hasValidData());

  // 0...10V range with 8 bits resolution
  QVERIFY(io.setRange(0, 10, 8));
  QVERIFY(!io.hasValidData());
  MDT_COMPARE(io.value(), 0.0, 8, 0.0, 10.0);
  MDT_COMPARE(io.valueInt(), 0, 8, 0, 255);
  io.setValueInt(0, true);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.valueInt(), 0, 8, 0, 255);
  io.setValueInt(127, true);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value(), 5.0, 8, 0.0, 10.0);
  MDT_COMPARE(io.valueInt(), 127, 8, 0, 255);
  io.setValueInt(255, true);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value(), 10.0, 8, 0.0, 10.0);
  MDT_COMPARE(io.valueInt(), 255, 8, 0, 255);
  // Set invalid value
  io.setValue(9.0, false);
  QVERIFY(!io.hasValidData());
  MDT_COMPARE(io.value(), 0.0, 8, 0.0, 10.0);
  MDT_COMPARE(io.valueInt(), 0, 8, 0, 255);
  // Set valid value
  io.setValue(8.0, true);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value(), 8.0, 8, 0.0, 10.0);
  // Set invalid value
  io.setValueInt(123, false);
  QVERIFY(!io.hasValidData());
  MDT_COMPARE(io.value(), 0.0, 8, 0.0, 10.0);
  MDT_COMPARE(io.valueInt(), 0, 8, 0, 255);

  // Setup range (limits test)
  QVERIFY(!io.setRange(0.0, 10.0, 8*sizeof(int), 1, true));
  QVERIFY(io.setRange(0.0, 10.0, 8*sizeof(int), 0, true));
  QVERIFY(io.setRange(0.0, 10.0, 0, 8*sizeof(int), true));
  QVERIFY(!io.setRange(0.0, 10.0, 1, 8*sizeof(int), true));

  // 0...10V range with 8 bits resolution
  QVERIFY(io.setRange(0, 10, 8));
  QVERIFY(!io.hasValidData());
  MDT_COMPARE(io.value(), 0.0, 8, 0.0, 10.0);
  MDT_COMPARE(io.valueInt(), 0, 8, 0, 255);
  io.setValueInt(0, true);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.valueInt(), 0, 8, 0, 255);
  io.setValueInt(127, true);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value(), 5.0, 8, 0.0, 10.0);
  MDT_COMPARE(io.valueInt(), 127, 8, 0, 255);

  // Check setValue(QVariant)
  io.setValue(QVariant());
  QVERIFY(!io.hasValidData());
  io.setValue(QVariant(5.6));
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value(), 5.6, 8, 0.0, 10.0);
  io.setValue(QVariant(127));
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value(), 5.0, 8, 0.0, 10.0);
}

void mdtIoTest::wagoAnalogInputTest()
{
  mdtAnalogIo io;

  // Wago 750-457 - AI -10 to +10 V
  QVERIFY(io.setRange(-10.0, 10.0, 13, 3, true));
  io.setValueInt(0x8000, true);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value(), -10.0, 13, -10.0, 10.0);
  io.setValueInt(0x9998, true);
  MDT_COMPARE(io.value(), -8.0, 13, -10.0, 10.0);
  io.setValueInt(0xB334, true);
  MDT_COMPARE(io.value(), -6.0, 13, -10.0, 10.0);
  io.setValueInt(0x0000, true);
  MDT_COMPARE(io.value(), 0.0, 13, -10.0, 10.0);
  io.setValueInt(0x3330, true);
  MDT_COMPARE(io.value(), 4.0, 13, -10.0, 10.0);
  io.setValueInt(0x7FFC, true);
  MDT_COMPARE(io.value(), 10.0, 13, -10.0, 10.0);

  // Wago 750-483 - AI 0 to 30 V
  QVERIFY(io.setRange(0.0, 30.0, 14, 1, false));
  io.setValueInt(0x0000, true);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value(), 0.0, 14, 0.0, 30.0);
  io.setValueInt(0x1554, true);
  MDT_COMPARE(io.value(), 5.0, 14, 0.0, 30.0);
  io.setValueInt(0x2AAA, true);
  MDT_COMPARE(io.value(), 10.0, 14, 0.0, 30.0);
  io.setValueInt(0x4000, true);
  MDT_COMPARE(io.value(), 15.0, 14, 0.0, 30.0);
  io.setValueInt(0x5554, true);
  MDT_COMPARE(io.value(), 20.0, 14, 0.0, 30.0);
  io.setValueInt(0x6AAA, true);
  MDT_COMPARE(io.value(), 25.0, 14, 0.0, 30.0);
  io.setValueInt(0x7FFF, true);
  MDT_COMPARE(io.value(), 30.0, 14, 0.0, 30.0);
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
  io.setValueInt(0x0000, true);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value(), 0.0, 12, 0.0, 10.0);
  io.setValueInt(0x1000, true);
  MDT_COMPARE(io.value(), 1.25, 12, 0.0, 10.0);
  io.setValueInt(0x2000, true);
  MDT_COMPARE(io.value(), 2.5, 12, 0.0, 10.0);
  io.setValueInt(0x3000, true);
  MDT_COMPARE(io.value(), 3.75, 12, 0.0, 10.0);
  io.setValueInt(0x4000, true);
  MDT_COMPARE(io.value(), 5.0, 12, 0.0, 10.0);
  io.setValueInt(0x5000, true);
  MDT_COMPARE(io.value(), 6.25, 12, 0.0, 10.0);
  io.setValueInt(0x6000, true);
  MDT_COMPARE(io.value(), 7.5, 12, 0.0, 10.0);
  io.setValueInt(0x7000, true);
  MDT_COMPARE(io.value(), 8.75, 12, 0.0, 10.0);
  io.setValueInt(0x7FFF, true);
  MDT_COMPARE(io.value(), 10.0, 12, 0.0, 10.0);
  // Write test - We take values from table in manual (1 bit approximate)
  io.setValue(0.0, true);
  MDT_COMPARE(io.valueInt(), 0x0000, 12, 0, 4096);
  io.setValue(1.25, true);
  MDT_COMPARE(io.valueInt(), 0x0FFF, 12, 0, 4096);
  io.setValue(2.5, true);
  MDT_COMPARE(io.valueInt(), 0x1FFF, 12, 0, 4096);
  io.setValue(3.75, true);
  MDT_COMPARE(io.valueInt(), 0x2FFF, 12, 0, 4096);
  io.setValue(5.0, true);
  MDT_COMPARE(io.valueInt(), 0x3FFF, 12, 0, 4096);
  io.setValue(6.25, true);
  MDT_COMPARE(io.valueInt(), 0x4FFF, 12, 0, 4096);
  io.setValue(7.5, true);
  MDT_COMPARE(io.valueInt(), 0x5FFF, 12, 0, 4096);
  io.setValue(8.75, true);
  MDT_COMPARE(io.valueInt(), 0x6FFF, 12, 0, 4096);
  io.setValue(10.0, true);
  MDT_COMPARE(io.valueInt(), 0x7FFF, 12, 0, 4096);

  /*
   * 750-556 - AO - -10 to +10 V
   */
  QVERIFY(io.setRange(-10.0, 10.0, 13, 3, true));
  QVERIFY(io.setEncodeBitSettings(16, 0));
  // Read test
  io.setValueInt(0x8001, true);
  QVERIFY(io.hasValidData());
  MDT_COMPARE(io.value(), -10.0, 13, -10.0, 10.0);
  io.setValueInt(0xA000, true);
  MDT_COMPARE(io.value(), -7.5, 13, -10.0, 10.0);
  io.setValueInt(0xC000, true);
  MDT_COMPARE(io.value(), -5.0, 13, -10.0, 10.0);
  io.setValueInt(0xE000, true);
  MDT_COMPARE(io.value(), -2.5, 13, -10.0, 10.0);
  io.setValueInt(0x0000, true);
  MDT_COMPARE(io.value(), 0.0, 13, -10.0, 10.0);
  io.setValueInt(0x2000, true);
  MDT_COMPARE(io.value(), 2.5, 13, -10.0, 10.0);
  io.setValueInt(0x4000, true);
  MDT_COMPARE(io.value(), 5.0, 13, -10.0, 10.0);
  io.setValueInt(0x6000, true);
  MDT_COMPARE(io.value(), 7.5, 13, -10.0, 10.0);
  io.setValueInt(0x7FFF, true);
  MDT_COMPARE(io.value(), 10.0, 13, -10.0, 10.0);
  // Write test - We take values from table in manual (1 bit approximate)
  io.setValue(-10.0, true);
  MDT_COMPARE(io.valueInt(), 0x8001, 13, 0, 8192);
  io.setValue(-7.5, true);
  MDT_COMPARE(io.valueInt(), 0xA000, 13, 0, 8192);
  io.setValue(-5.0, true);
  MDT_COMPARE(io.valueInt(), 0xC000, 13, 0, 8192);
  io.setValue(-2.5, true);
  MDT_COMPARE(io.valueInt(), 0xE000, 13, 0, 8192);
  io.setValue(0.0, true);
  MDT_COMPARE(io.valueInt(), 0x0000, 13, 0, 8192);
  io.setValue(2.5, true);
  MDT_COMPARE(io.valueInt(), 0x1FFF, 13, 0, 8192);
  io.setValue(5.0, true);
  MDT_COMPARE(io.valueInt(), 0x3FFF, 13, 0, 8192);
  io.setValue(7.5, true);
  MDT_COMPARE(io.valueInt(), 0x5FFF, 13, 0, 8192);
  io.setValue(10.0, true);
  MDT_COMPARE(io.valueInt(), 0x7FFF, 13, 0, 8192);
}

void mdtIoTest::digitalIoTest()
{
  mdtDigitalIo io;

  // Initial values
  QVERIFY(!io.isOn());
  QVERIFY(!io.hasValidData());

  // Toggle
  io.setOn(true, true);
  QVERIFY(io.hasValidData());
  QVERIFY(io.isOn());
  io.setOn(false, true);
  QVERIFY(io.hasValidData());
  QVERIFY(!io.isOn());
  // Set invalid state
  io.setOn(true, false);
  QVERIFY(!io.hasValidData());
  QVERIFY(!io.isOn());
  // Set valid state
  io.setOn(true, true);
  QVERIFY(io.hasValidData());
  QVERIFY(io.isOn());
  // Check setOn(QVariant)
  io.setOn(QVariant());
  QVERIFY(!io.hasValidData());
  io.setOn(QVariant(true));
  QVERIFY(io.hasValidData());
  QVERIFY(io.isOn());
  io.setOn(QVariant(false));
  QVERIFY(io.hasValidData());
  QVERIFY(!io.isOn());
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
