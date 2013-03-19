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
#include "mdtIoWidgetTest.h"
#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"
#include "mdtAnalogInWidget.h"
#include "mdtAnalogOutWidget.h"
#include "mdtDigitalInWidget.h"
#include "mdtDigitalOutWidget.h"

#include <QTest>
#include <QString>
#include <QVariant>
#include <QPushButton>
#include <qwt_slider.h>
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
  ai.setRange(0, 10, 8);
  ///QCOMPARE(ai.value(), 0.0);
  MDT_COMPARE(ai.value(), 0.0, 8, 0.0, 10.0);
  QCOMPARE(ai.valueInt(), 0);
  ai.setValueInt(0, true);
  MDT_COMPARE(ai.value(), 0.0, 8, 0.0, 10.0);
  ///QCOMPARE(ai.value(), 0.0);
  QCOMPARE(ai.valueInt(), 0);
  ai.setValueInt(127, true);
  ///QCOMPARE(ai.value(), (double)(10.0*127.0/255.0));
  MDT_COMPARE(ai.value(), 5.0, 8, 0.0, 10.0);
  QCOMPARE(ai.valueInt(), 127);
  ai.setValueInt(255, true);
  ///QCOMPARE(ai.value(), 10.0);
  MDT_COMPARE(ai.value(), 10.0, 8, 0.0, 10.0);
  QCOMPARE(ai.valueInt(), 255);

  // 4...20mA range with 8 bits resolution
  ai.setLabel("Input current");
  ai.setLabelShort("Iin");
  ai.setDetails("Input current that is used for M1 setpoint.\nShema: 10E");
  ai.setAddress(1246);
  ai.setUnit("mA");
  ai.setRange(4, 20, 8);
  ///QCOMPARE(ai.value(), 4.0);
  MDT_COMPARE(ai.value(), 4.0, 8, 4.0, 20.0);
  QCOMPARE(ai.valueInt(), 0);
  ai.setValueInt(0, true);
  QCOMPARE(ai.value(), 4.0);
  QCOMPARE(ai.valueInt(), 0);
  ai.setValueInt(127, true);
  ///QCOMPARE(ai.value(), (double)(4.0+16.0*127.0/255.0));
  MDT_COMPARE(ai.value(), 12.0, 8, 4.0, 20.0);
  QCOMPARE(ai.valueInt(), 127);
  ai.setValueInt(255, true);
  ///QCOMPARE(ai.value(), 20.0);
  MDT_COMPARE(ai.value(), 20.0, 8, 4.0, 20.0);
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
  ao.setRange(0, 10, 8);
  ai.setLabel("Input voltage of final stage");
  ai.setLabelShort("Vin");
  ai.setUnit("V");
  ai.setRange(0, 10, 8);
  ///QCOMPARE(ao.value(), 0.0);
  MDT_COMPARE(ao.value(), 0.0, 8, 0.0, 10.0);
  QCOMPARE(ao.valueInt(), 0);
  ///QCOMPARE(ai.value(), 0.0);
  MDT_COMPARE(ai.value(), 0.0, 8, 0.0, 10.0);
  QCOMPARE(ai.valueInt(), 0);
  ao.setValueInt(0, true);
  ///QCOMPARE(ao.value(), 0.0);
  MDT_COMPARE(ao.value(), 0.0, 8, 0.0, 10.0);
  QCOMPARE(ao.valueInt(), 0);
  ///QCOMPARE(ai.value(), 0.0);
  MDT_COMPARE(ai.value(), 0.0, 8, 0.0, 10.0);
  QCOMPARE(ai.valueInt(), 0);
  ao.setValueInt(127, true);
  ///QVERIFY(qAbs(ao.value()-5.0) < (5.0/250.0));
  MDT_COMPARE(ao.value(), 5.0, 8, 0.0, 10.0);
  ///QVERIFY(qAbs(ao.valueInt()-127) <= 1);  /// ??
  MDT_COMPARE(ao.valueInt(), 127, 8, 0, 255);
  ///QVERIFY(qAbs(ai.value()-5.0) < (5.0/250.0));
  MDT_COMPARE(ai.value(), 5.0, 8, 0.0, 10.0);
  ///QCOMPARE(ai.valueInt(), 127);
  MDT_COMPARE(ai.valueInt(), 127, 8, 0, 255);
  ///QVERIFY(qAbs(ai.valueInt()-127) <= 1);
  ao.setValueInt(255, true);
  ///QCOMPARE(ao.value(), 10.0);
  MDT_COMPARE(ao.value(), 10.0, 8, 0.0, 10.0);
  ///QCOMPARE(ao.valueInt(), 255);
  MDT_COMPARE(ao.valueInt(), 255, 8, 0, 255);
  ///QCOMPARE(ai.value(), 10.0);
  MDT_COMPARE(ai.value(), 10.0, 8, 0.0, 10.0);
  ///QCOMPARE(ai.valueInt(), 255);
  MDT_COMPARE(ai.valueInt(), 255, 8, 0, 255);

  // 4...20mA range with 8 bits resolution
  ao.setLabel("Output current of final stage");
  ao.setLabelShort("Iout");
  ao.setRange(4, 20, 8);
  ai.setRange(4, 20, 8);
  ai.setLabelShort("Iin");
  ai.setUnit("mA");
  ///QCOMPARE(ao.value(), 4.0);
  MDT_COMPARE(ao.value(), 4.0, 8, 4.0, 20.0);
  ///QCOMPARE(ao.valueInt(), 0);
  MDT_COMPARE(ao.valueInt(), 0, 8, 0, 255);
  ///QCOMPARE(ai.value(), 4.0);
  MDT_COMPARE(ai.value(), 4.0, 8, 4.0, 20.0);
  ///QCOMPARE(ai.valueInt(), 0);
  MDT_COMPARE(ai.valueInt(), 0, 8, 0, 255);
  ao.setValueInt(0, true);
  ///QCOMPARE(ao.value(), 4.0);
  MDT_COMPARE(ao.value(), 4.0, 8, 4.0, 20.0);
  ///QCOMPARE(ao.valueInt(), 0);
  MDT_COMPARE(ao.valueInt(), 0, 8, 0, 255);
  ///QCOMPARE(ai.value(), 4.0);
  MDT_COMPARE(ai.value(), 4.0, 8, 4.0, 20.0);
  ///QCOMPARE(ai.valueInt(), 0);
  MDT_COMPARE(ai.valueInt(), 0, 8, 0, 255);
  ao.setValueInt(127, true);
  QVERIFY(qAbs(ao.value()-12.0) < (12.0/250.0));
  ///QCOMPARE(ao.valueInt(), 127);
  MDT_COMPARE(ao.valueInt(), 127, 8, 0, 255);
  ///QVERIFY(qAbs(ai.value()-12.0) < (12.0/250.0));
  MDT_COMPARE(ai.value(), 12.0, 8, 4.0, 20.0);
  ///QCOMPARE(ai.valueInt(), 127);
  MDT_COMPARE(ai.valueInt(), 127, 8, 0, 255);
  ao.setValueInt(255, true);
  ///QCOMPARE(ao.value(), 20.0);
  MDT_COMPARE(ao.value(), 20.0, 8, 4.0, 20.0);
  ///QCOMPARE(ao.valueInt(), 255);
  MDT_COMPARE(ao.valueInt(), 255, 8, 0, 255);
  ///QCOMPARE(ai.value(), 20.0);
  MDT_COMPARE(ai.value(), 20.0, 8, 4.0, 20.0);
  ///QCOMPARE(ai.valueInt(), 255);
  MDT_COMPARE(ai.valueInt(), 255, 8, 0, 255);
}

void mdtIoWidgetTest::analogOutWidgetRecursifTest()
{
  // GUI (program)
  mdtAnalogIo ao;
  QwtSlider sl(0);
  // Simulate the physical device
  mdtAnalogIo plcAo;

  // Setup: range: 0-10, n = 12 bits
  ao.setLabelShort("AO1");
  ao.setRange(0.0, 10.0, 12);
  plcAo.setLabelShort("PLC");
  plcAo.setRange(0.0, 10.0, 12);
  sl.setRange(0.0, 10.0);
  QObject::connect(&sl, SIGNAL(valueChanged(double)), &ao, SLOT(setValueFromUi(double)));
  QObject::connect(&ao, SIGNAL(valueChangedForUi(double)), &sl, SLOT(setValue(double)));
  QObject::connect(&ao, SIGNAL(valueChanged(double)), &plcAo, SLOT(setValue(double)));
  sl.show();

  // Initial states
  ///QVERIFY(ao.value() < (1.0/4050.0));
  MDT_COMPARE(ao.value(), 0.0, 12, 0.0, 10.0);
  ///QVERIFY(plcAo.value() < (1.0/4050.0));
  MDT_COMPARE(plcAo.value(), 0.0, 12, 0.0, 10.0);

  // User change the value
  sl.setValue(1.0);
  ///QVERIFY(qAbs(ao.value()-1.0) < (1.0/4050.0));
  MDT_COMPARE(ao.value(), 1.0, 12, 0.0, 10.0);
  ///QVERIFY(qAbs(plcAo.value()-1.0) < (1.0/4050.0));
  MDT_COMPARE(plcAo.value(), 1.0, 12, 0.0, 10.0);
  // PLC (device) confirm the same value
  ao.setValue(1.0, true, false);
  ///QVERIFY(qAbs(ao.value()-1.0) < (1.0/4050.0));
  MDT_COMPARE(ao.value(), 1.0, 12, 0.0, 10.0);
  ///QVERIFY(qAbs(plcAo.value()-1.0) < (1.0/4050.0));
  MDT_COMPARE(plcAo.value(), 1.0, 12, 0.0, 10.0);

  // User change the value
  sl.setValue(2.0);
  ///QVERIFY(qAbs(ao.value()-2.0) < (1.0/4050.0));
  MDT_COMPARE(ao.value(), 2.0, 12, 0.0, 10.0);
  ///QVERIFY(qAbs(plcAo.value()-2.0) < (1.0/4050.0));
  MDT_COMPARE(plcAo.value(), 2.0, 12, 0.0, 10.0);
  // PLC (device) confirm a value that differs
  ao.setValue(1.5, true, false);
  ///QVERIFY(qAbs(ao.value()-1.5) < (1.0/4050.0));
  MDT_COMPARE(ao.value(), 1.5, 12, 0.0, 10.0);
  // Check that PLC not receives the confirmation as new value
  ///QVERIFY(qAbs(plcAo.value()-2.0) < (1.0/4050.0));
  MDT_COMPARE(plcAo.value(), 2.0, 12, 0.0, 10.0);

  // User change the value
  sl.setValue(3.5);
  ///QVERIFY(qAbs(ao.value()-3.5) < (1.0/4050.0));
  MDT_COMPARE(ao.value(), 3.5, 12, 0.0, 10.0);
  ///QVERIFY(qAbs(plcAo.value()-3.5) < (1.0/4050.0));
  MDT_COMPARE(plcAo.value(), 3.5, 12, 0.0, 10.0);
  // PLC (device) confirm the same value
  ao.setValue(3.5, true, false);
  ///QVERIFY(qAbs(ao.value()-3.5) < (1.0/4050.0));
  MDT_COMPARE(ao.value(), 3.5, 12, 0.0, 10.0);
  ///QVERIFY(qAbs(plcAo.value()-3.5) < (1.0/4050.0));
  MDT_COMPARE(plcAo.value(), 3.5, 12, 0.0, 10.0);

  // User change the value
  sl.setValue(4.0);
  ///QVERIFY(qAbs(ao.value()-4.0) < (1.0/4050.0));
  MDT_COMPARE(ao.value(), 4.0, 12, 0.0, 10.0);
  ///QVERIFY(qAbs(plcAo.value()-4.0) < (1.0/4050.0));
  MDT_COMPARE(plcAo.value(), 4.0, 12, 0.0, 10.0);
  // PLC (device) confirm the same value
  ao.setValue(4.0, true, false);
  ///QVERIFY(qAbs(ao.value()-4.0) < (1.0/4050.0));
  MDT_COMPARE(ao.value(), 4.0, 12, 0.0, 10.0);
  ///QVERIFY(qAbs(plcAo.value()-4.0) < (1.0/4050.0));
  MDT_COMPARE(plcAo.value(), 4.0, 12, 0.0, 10.0);

  /*
  while(sl.isVisible()){
    QTest::qWait(100);
  }
  */
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

void mdtIoWidgetTest::digitalOutWidgetRecursifTest()
{
  // GUI (program)
  mdtDigitalIo dout;
  mdtDigitalOutWidget doW;
  doW.setIo(&dout);

  doW.show();

  // Initial state
  QVERIFY(!doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "??");

  // A request was sent to PLC, and confirm arrives
  dout.setOn(false);
  QVERIFY(!doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "OFF");

  // User pressed button
  doW.internalPushButton()->setChecked(true);
  QVERIFY(doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "ON");
  // Request was sent to PLC and confirmation arrives with same state (On)
  dout.setOn(QVariant(true), false);
  QVERIFY(doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "ON");

  // User pressed button
  doW.internalPushButton()->setChecked(false);
  QVERIFY(!doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "OFF");
  // Request was sent to PLC and confirmation arrives with same state (Off)
  dout.setOn(QVariant(false), false);
  QVERIFY(!doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "OFF");

  // User pressed button
  doW.internalPushButton()->setChecked(true);
  dout.setEnabled(false);
  QVERIFY(doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "ON");
  // Request was sent to PLC and confirmation arrives with other state (Off)
  dout.setOn(QVariant(false), false);
  dout.setEnabled(true);
  QVERIFY(!doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "OFF");

  // User pressed button
  doW.internalPushButton()->setChecked(true);
  QVERIFY(doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "ON");
  // Request was sent to PLC but a error occured
  dout.setOn(QVariant(), false);
  QVERIFY(!doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "??");
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
