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
#include "mdtIoTest.h"
#include "mdtApplication.h"
#include "mdtAbstractIo.h"
#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"

#include <QDebug>

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
  QCOMPARE(io.value(), 0.0);
  QCOMPARE(io.valueInt(), 0);
  io.setValueInt(0, true);
  QVERIFY(io.hasValidData());
  QCOMPARE(io.valueInt(), 0);
  io.setValueInt(127, true);
  QVERIFY(io.hasValidData());
  QCOMPARE(io.value(), (double)(10.0*127.0/255.0));
  QCOMPARE(io.valueInt(), 127);
  io.setValueInt(255, true);
  QVERIFY(io.hasValidData());
  QCOMPARE(io.value(), 10.0);
  QCOMPARE(io.valueInt(), 255);
  // Set invalid value
  io.setValue(9.0, false);
  QVERIFY(!io.hasValidData());
  QCOMPARE(io.value(), 0.0);
  QCOMPARE(io.valueInt(), 0);
  // Set valid value
  io.setValue(8.0, true);
  QVERIFY(io.hasValidData());
  QCOMPARE(io.value(), 8.0);
  // Set invalid value
  io.setValueInt(123, false);
  QVERIFY(!io.hasValidData());
  QCOMPARE(io.value(), 0.0);
  QCOMPARE(io.valueInt(), 0);

  // Setup range (limits test)
  QVERIFY(!io.setRange(0.0, 10.0, 8*sizeof(int), 1, true));
  QVERIFY(io.setRange(0.0, 10.0, 8*sizeof(int), 0, true));
  QVERIFY(io.setRange(0.0, 10.0, 0, 8*sizeof(int), true));
  QVERIFY(!io.setRange(0.0, 10.0, 1, 8*sizeof(int), true));

  // 0...10V range with 8 bits resolution
  QVERIFY(io.setRange(0, 10, 8));
  QVERIFY(!io.hasValidData());
  QCOMPARE(io.value(), 0.0);
  QCOMPARE(io.valueInt(), 0);
  io.setValueInt(0, true);
  QVERIFY(io.hasValidData());
  QCOMPARE(io.valueInt(), 0);
  io.setValueInt(127, true);
  QVERIFY(io.hasValidData());
  QCOMPARE(io.value(), (double)(10.0*127.0/255.0));
  QCOMPARE(io.valueInt(), 127);
}

void mdtIoTest::wagoAnalogInputTest()
{
  mdtAnalogIo io;

  // Wago 750-457 - AI -10 to +10 V
  QVERIFY(io.setRange(-10.0, 10.0, 13, 3, true));
  io.setValueInt(0x8000, true);
  QVERIFY(io.hasValidData());
  QCOMPARE(io.value(), -10.0);
  io.setValueInt(0x9998, true);
  QCOMPARE(io.value(), -8.0);
  io.setValueInt(0xB334, true);
  QCOMPARE(io.value(), -6.0);
  io.setValueInt(0x0000, true);
  QCOMPARE(io.value(), 0.0);
  io.setValueInt(0x3330, true);
  QCOMPARE(io.value(), 4.0);
  io.setValueInt(0x7FFC, true);
  QCOMPARE(io.value(), 10.0);

  // Wago 750-483 - AI 0 to 30 V
  QVERIFY(io.setRange(0.0, 30.0, 14, 1, false));
  io.setValueInt(0x0000, true);
  QVERIFY(io.hasValidData());
  QVERIFY(qAbs(io.value()) < 0.001);
  io.setValueInt(0x1554, true);
  QVERIFY(qAbs(io.value()-5.0) < 0.001);
  io.setValueInt(0x2AAA, true);
  QCOMPARE(io.value(), 10.0);
  io.setValueInt(0x4000, true);
  QVERIFY(qAbs(io.value()-15.0) < 0.001);
  io.setValueInt(0x5554, true);
  QCOMPARE(io.value(), 20.0);
  io.setValueInt(0x6AAA, true);
  QVERIFY(qAbs(io.value()-25.0) < 0.001);
  io.setValueInt(0x7FFF, true);
  QCOMPARE(io.value(), 30.0);

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
