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

  // Set some data and check
  io.setAddress(25);
  io.setLabelShort("Label short");
  io.setLabel("A label");
  io.setDetails("Details");
  QVERIFY(io.address() == 25);
  QVERIFY(io.labelShort() == "Label short");
  QVERIFY(io.label() == "A label");
  QVERIFY(io.details() == "Details");
}

void mdtIoTest::analogIoTest()
{
  mdtAnalogIo io;

  // Initial values
  QCOMPARE(io.value(), 0.0);
  QCOMPARE(io.valueInt(), 0);

  // 0...10V range with 8 bits resolution
  io.setRange(0, 10, 256);
  QCOMPARE(io.value(), 0.0);
  QCOMPARE(io.valueInt(), 0);
  io.setValueInt(0);
  QCOMPARE(io.valueInt(), 0);
  io.setValueInt(127);
  QCOMPARE(io.value(), (double)(10.0*127.0/255.0));
  QCOMPARE(io.valueInt(), 127);
  io.setValueInt(255);
  QCOMPARE(io.value(), 10.0);
  QCOMPARE(io.valueInt(), 255);
}

void mdtIoTest::digitalIoTest()
{
  mdtDigitalIo io;

  // Initial values
  QVERIFY(!io.isOn());

  // Toggle
  io.setOn(true);
  QVERIFY(io.isOn());
  io.setOn(false);
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
