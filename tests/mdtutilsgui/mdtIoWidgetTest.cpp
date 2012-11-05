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
#include "mdtIoWidgetTest.h"
#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"
#include "mdtAnalogInWidget.h"
#include "mdtAnalogOutWidget.h"
#include "mdtDigitalInWidget.h"
#include "mdtDigitalOutWidget.h"

#include <QTest>
#include <QString>
#include "mdtApplication.h"

#include <QDebug>

void mdtIoWidgetTest::analogInWidgetTest()
{
  mdtAnalogIo ai;
  mdtAnalogInWidget w;

  w.setIo(&ai);
  w.show();

  // Initial state
  QVERIFY(!ai.hasValidData());
  QCOMPARE(ai.value(), 0.0);
  QCOMPARE(ai.valueInt(), 0);

  // 0...10V range with 8 bits resolution
  ai.setLabel("Input voltage");
  ai.setLabelShort("Vin");
  ai.setUnit("V");
  ai.setRange(0, 10, 256);
  QCOMPARE(ai.value(), 0.0);
  QCOMPARE(ai.valueInt(), 0);
  ai.setValueInt(0, true);
  QCOMPARE(ai.value(), 0.0);
  QCOMPARE(ai.valueInt(), 0);
  ai.setValueInt(127, true);
  QCOMPARE(ai.value(), (double)(10.0*127.0/255.0));
  QCOMPARE(ai.valueInt(), 127);
  ai.setValueInt(255, true);
  QCOMPARE(ai.value(), 10.0);
  QCOMPARE(ai.valueInt(), 255);

  // 4...20mA range with 8 bits resolution
  ai.setLabel("Input current");
  ai.setLabelShort("Iin");
  ai.setDetails("Input current that is used for M1 setpoint.\nShema: 10E");
  ai.setAddress(1246);
  ai.setUnit("mA");
  ai.setRange(4, 20, 256);
  QCOMPARE(ai.value(), 4.0);
  QCOMPARE(ai.valueInt(), 0);
  ai.setValueInt(0, true);
  QCOMPARE(ai.value(), 4.0);
  QCOMPARE(ai.valueInt(), 0);
  ai.setValueInt(127, true);
  QCOMPARE(ai.value(), (double)(4.0+16.0*127.0/255.0));
  QCOMPARE(ai.valueInt(), 127);
  ai.setValueInt(255, true);
  QCOMPARE(ai.value(), 20.0);
  QCOMPARE(ai.valueInt(), 255);

  /*
  while(w.isVisible()){
    QTest::qWait(1000);
  }
  */
}

void mdtIoWidgetTest::analogOutWidgetTest()
{
  mdtAnalogIo ai, ao;
  mdtAnalogOutWidget wAo;
  mdtAnalogInWidget wAi;

  // Setup
  wAi.setIo(&ai);
  wAo.setIo(&ao);
  QObject::connect(&ao, SIGNAL(valueChanged(double)), &ai, SLOT(setValue(double)));

  wAi.show();
  wAo.show();

  // Initial state
  QVERIFY(!ai.hasValidData());
  QCOMPARE(ai.value(), 0.0);
  QCOMPARE(ai.valueInt(), 0);
  QVERIFY(!ao.hasValidData());
  QCOMPARE(ao.value(), 0.0);
  QCOMPARE(ao.valueInt(), 0);

  // 0...10V range with 8 bits resolution
  ao.setLabel("Output voltage of final stage");
  ao.setLabelShort("Vout");
  ao.setRange(0, 10, 256);
  ai.setLabel("Input voltage of final stage");
  ai.setLabelShort("Vin");
  ai.setUnit("V");
  ai.setRange(0, 10, 256);
  QCOMPARE(ao.value(), 0.0);
  QCOMPARE(ao.valueInt(), 0);
  QCOMPARE(ai.value(), 0.0);
  QCOMPARE(ai.valueInt(), 0);
  ao.setValueInt(0, true);
  QCOMPARE(ao.value(), 0.0);
  QCOMPARE(ao.valueInt(), 0);
  QCOMPARE(ai.value(), 0.0);
  QCOMPARE(ai.valueInt(), 0);
  ao.setValueInt(127, true);
  QCOMPARE(ao.value(), (double)(10.0*127.0/255.0));
  QCOMPARE(ao.valueInt(), 127);
  QCOMPARE(ai.value(), (double)(10.0*127.0/255.0));
  QCOMPARE(ai.valueInt(), 127);
  ao.setValueInt(255, true);
  QCOMPARE(ao.value(), 10.0);
  QCOMPARE(ao.valueInt(), 255);
  QCOMPARE(ai.value(), 10.0);
  QCOMPARE(ai.valueInt(), 255);

  // 4...20mA range with 8 bits resolution
  ao.setLabel("Output current of final stage");
  ao.setLabelShort("Iout");
  ao.setRange(4, 20, 256);
  ai.setRange(4, 20, 256);
  ai.setLabelShort("Iin");
  ai.setUnit("mA");
  QCOMPARE(ao.value(), 4.0);
  QCOMPARE(ao.valueInt(), 0);
  QCOMPARE(ai.value(), 4.0);
  QCOMPARE(ai.valueInt(), 0);
  ao.setValueInt(0, true);
  QCOMPARE(ao.value(), 4.0);
  QCOMPARE(ao.valueInt(), 0);
  QCOMPARE(ai.value(), 4.0);
  QCOMPARE(ai.valueInt(), 0);
  ao.setValueInt(127, true);
  QCOMPARE(ao.value(), (double)(4.0+16.0*127.0/255.0));
  QCOMPARE(ao.valueInt(), 127);
  QCOMPARE(ai.value(), (double)(4.0+16.0*127.0/255.0));
  QCOMPARE(ai.valueInt(), 127);
  ao.setValueInt(255, true);
  QCOMPARE(ao.value(), 20.0);
  QCOMPARE(ao.valueInt(), 255);
  QCOMPARE(ai.value(), 20.0);
  QCOMPARE(ai.valueInt(), 255);
}

void mdtIoWidgetTest::digitalInWidgetTest()
{
  mdtDigitalIo di;
  mdtDigitalInWidget wDi;

  wDi.show();

  // Setup
  wDi.setIo(&di);
  di.setLabelShort("I2");
  di.setLabel("Input 2 (Drive ON)");
  di.setDetails("Used for drive M on");

  // Initial state
  QVERIFY(!di.isOn());
  QVERIFY(!di.hasValidData());

  // Toggle ...
  di.setOn(true, true);
  QVERIFY(di.isOn());
  di.setOn(false, true);
  QVERIFY(!di.isOn());
}

void mdtIoWidgetTest::digitalOutWidgetTest()
{
  mdtDigitalIo di;
  mdtDigitalIo dout;
  mdtDigitalOutWidget wDo;
  mdtDigitalInWidget wDi;

  // Setup
  wDi.setIo(&di);
  di.setLabelShort("I3");
  di.setLabel("Input 3 (AC1 Ok)");
  di.setDetails("Confirm that AC1 (3P var out for comp.) is Ok");
  dout.setLabelShort("O3");
  dout.setLabel("Output 3 (AC1 Ok)");
  dout.setDetails("Send confirmation that AC1 (3P var out for comp.) is Ok");
  wDo.setIo(&dout);
  QObject::connect(&dout, SIGNAL(stateChanged(bool)), &di, SLOT(setOn(bool)));

  wDo.show();
  wDi.show();

  // Initial state
  QVERIFY(!di.isOn());
  QVERIFY(!dout.isOn());
  QVERIFY(!di.hasValidData());
  QVERIFY(!dout.hasValidData());
  wDo.show();
  wDi.show();
  // Toggle ...
  dout.setOn(true, true);
  QVERIFY(dout.isOn());
  QVERIFY(di.isOn());
  dout.setOn(false, true);
  QVERIFY(!dout.isOn());
  QVERIFY(!di.isOn());
}


int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtIoWidgetTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
