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
  mdtAnalogInWidget w;

  w.show();

  // Initial state
  QCOMPARE(w.value(), 0.0);
  QCOMPARE(w.valueInt(), 0);

  // 0...10V range with 8 bits resolution
  w.setLabel("Input voltage");
  w.setLabelShort("Vin");
  w.setUnit("V");
  w.setRange(0, 10, 256);
  QCOMPARE(w.value(), 0.0);
  QCOMPARE(w.valueInt(), 0);
  w.setValueInt(0);
  QCOMPARE(w.value(), 0.0);
  QCOMPARE(w.valueInt(), 0);
  w.setValueInt(127);
  QCOMPARE(w.value(), (double)(10.0*127.0/255.0));
  QCOMPARE(w.valueInt(), 127);
  w.setValueInt(255);
  QCOMPARE(w.value(), 10.0);
  QCOMPARE(w.valueInt(), 255);

  // 4...20mA range with 8 bits resolution
  w.setLabel("Input current");
  w.setLabelShort("Iin");
  w.setDetails("Input current that is used for M1 setpoint.\nShema: 10E");
  w.setAddress(1246);
  w.setUnit("mA");
  w.setRange(4, 20, 256);
  QCOMPARE(w.value(), 4.0);
  QCOMPARE(w.valueInt(), 0);
  w.setValueInt(0);
  QCOMPARE(w.value(), 4.0);
  QCOMPARE(w.valueInt(), 0);
  w.setValueInt(127);
  QCOMPARE(w.value(), (double)(4.0+16.0*127.0/255.0));
  QCOMPARE(w.valueInt(), 127);
  w.setValueInt(255);
  QCOMPARE(w.value(), 20.0);
  QCOMPARE(w.valueInt(), 255);

  while(w.isVisible()){
    QTest::qWait(1000);
  }
}

void mdtIoWidgetTest::analogOutWidgetTest()
{
  mdtAnalogOutWidget wAo;
  mdtAnalogInWidget wAi;

  QObject::connect(&wAo, SIGNAL(valueChanged(double)), &wAi, SLOT(setValue(double)));

  wAo.show();
  wAi.show();

  // Initial state
  QCOMPARE(wAo.value(), 0.0);
  QCOMPARE(wAo.valueInt(), 0);

  // 0...10V range with 8 bits resolution
  wAo.setLabel("Output voltage of final stage");
  wAo.setLabelShort("Vout");
  wAo.setRange(0, 10, 256);
  wAi.setLabel("Input voltage of final stage");
  wAi.setLabelShort("Vin");
  wAi.setUnit("V");
  wAi.setRange(0, 10, 256);
  QCOMPARE(wAo.value(), 0.0);
  QCOMPARE(wAo.valueInt(), 0);
  QCOMPARE(wAi.value(), 0.0);
  QCOMPARE(wAi.valueInt(), 0);
  wAo.setValueInt(0);
  QCOMPARE(wAo.value(), 0.0);
  QCOMPARE(wAo.valueInt(), 0);
  QCOMPARE(wAi.value(), 0.0);
  QCOMPARE(wAi.valueInt(), 0);
  wAo.setValueInt(127);
  QCOMPARE(wAo.value(), (double)(10.0*127.0/255.0));
  QCOMPARE(wAo.valueInt(), 127);
  QCOMPARE(wAi.value(), (double)(10.0*127.0/255.0));
  QCOMPARE(wAi.valueInt(), 127);
  wAo.setValueInt(255);
  QCOMPARE(wAo.value(), 10.0);
  QCOMPARE(wAo.valueInt(), 255);
  QCOMPARE(wAi.value(), 10.0);
  QCOMPARE(wAi.valueInt(), 255);

  // 4...20mA range with 8 bits resolution
  wAo.setLabel("Output current of final stage");
  wAo.setLabelShort("Iout");
  wAo.setRange(4, 20, 256);
  wAi.setRange(4, 20, 256);
  wAi.setLabelShort("Iin");
  wAi.setUnit("mA");
  QCOMPARE(wAo.value(), 4.0);
  QCOMPARE(wAo.valueInt(), 0);
  QCOMPARE(wAi.value(), 4.0);
  QCOMPARE(wAi.valueInt(), 0);
  wAo.setValueInt(0);
  QCOMPARE(wAo.value(), 4.0);
  QCOMPARE(wAo.valueInt(), 0);
  QCOMPARE(wAi.value(), 4.0);
  QCOMPARE(wAi.valueInt(), 0);
  wAo.setValueInt(127);
  QCOMPARE(wAo.value(), (double)(4.0+16.0*127.0/255.0));
  QCOMPARE(wAo.valueInt(), 127);
  QCOMPARE(wAi.value(), (double)(4.0+16.0*127.0/255.0));
  QCOMPARE(wAi.valueInt(), 127);
  wAo.setValueInt(255);
  QCOMPARE(wAo.value(), 20.0);
  QCOMPARE(wAo.valueInt(), 255);
  QCOMPARE(wAi.value(), 20.0);
  QCOMPARE(wAi.valueInt(), 255);

  while(wAo.isVisible()){
    QTest::qWait(1000);
  }
}

void mdtIoWidgetTest::digitalInWidgetTest()
{
  mdtDigitalInWidget w;

  w.show();

  // Initial state
  QVERIFY(!w.isOn());

  // Toggle ...
  w.setLabel("Input 2 (Drive ON)");
  w.setLabelShort("I2");
  w.setOn(true);
  QVERIFY(w.isOn());
  w.setOn(false);
  QVERIFY(!w.isOn());

  while(w.isVisible()){
    QTest::qWait(1000);
  }
}

void mdtIoWidgetTest::digitalOutWidgetTest()
{
  mdtDigitalOutWidget wDo;
  mdtDigitalInWidget wDi;

  QObject::connect(&wDo, SIGNAL(stateChanged(bool)), &wDi, SLOT(setOn(bool)));

  wDo.show();
  wDi.show();

  // Initial state
  QVERIFY(!wDo.isOn());

  // Toggle ...
  wDo.setLabel("Output 2 (Diag mode)");
  wDo.setLabelShort("O2");
  wDi.setLabel("Input 2 (Diag mode)");
  wDi.setLabelShort("I2");
  wDo.setOn(true);
  QVERIFY(wDo.isOn());
  QVERIFY(wDi.isOn());
  wDo.setOn(false);
  QVERIFY(!wDo.isOn());
  QVERIFY(!wDi.isOn());

  while(wDo.isVisible()){
    QTest::qWait(1000);
  }
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
