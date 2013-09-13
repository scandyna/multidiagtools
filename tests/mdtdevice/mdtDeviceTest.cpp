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
#include "mdtDeviceTest.h"
#include "mdtApplication.h"
#include "mdtDeviceIos.h"
#include "mdtDeviceIosSegment.h"
#include "mdtDeviceIosWidget.h"
#include "mdtDeviceModbus.h"
#include "mdtModbusTcpPortManager.h"
#include "mdtDeviceModbusWago.h"
#include "mdtDeviceScpi.h"
#include "mdtDeviceU3606A.h"
#include "mdtDeviceDSO1000A.h"
#include "mdtDeviceWindow.h"
#include "mdtPortInfo.h"
#include "mdtPortManager.h"
#include "mdtPortConfig.h"

#include "mdtFrameCodecScpi.h"

#include <QString>
#include <QVariant>
#include <QList>

#include <QTime>
#include <QDebug>

void mdtDeviceTest::deviceBaseTest()
{
  mdtDevice dev;

  // Initial states
  ///QVERIFY(dev.state() == mdtDevice::Disconnected);
  ///QVERIFY(dev.state() == mdtPortManager::Disconnected);

  dev.start(100);
}

void mdtDeviceTest::deviceIosSegmentTest()
{
  mdtDeviceIosSegment seg;
  mdtAnalogIo *aIo;
  mdtDigitalIo *dIo;
  QList<mdtAnalogIo*> aIoList;
  QList<mdtDigitalIo*> dIoList;
  QList<int> expectedReadAddresses;
  QList<int> expectedWriteAddresses;
  QList<int> expectedValuesInt;
  QList<double> expectedValuesDouble;
  QList<bool> expectedValuesBool;
  QList<mdtValue> valuesMdt;
  QList<QVariant> valuesVar;

  // Check default values
  QCOMPARE(seg.startAddressRead(), 0);
  QCOMPARE(seg.startAddressWrite(), 0);
  QCOMPARE(seg.endAddressRead(), 0);
  QCOMPARE(seg.endAddressWrite(), 0);
  QCOMPARE(seg.ioCount(), 0);

  /*
   * Analog I/O
   */

  // Build a list of 9 I/O's
  expectedReadAddresses.clear();
  expectedWriteAddresses.clear();
  expectedValuesInt.clear();
  expectedValuesDouble.clear();
  for(int i = 0; i < 9; i++){
    aIo = new mdtAnalogIo;
    aIo->setAddressRead(i);
    expectedReadAddresses.append(i);
    aIo->setAddressWrite(i+100);
    expectedWriteAddresses.append(i+100);
    aIo->setValue(3*i);
    expectedValuesInt.append(3*i);
    expectedValuesDouble.append(3*i);
    aIoList.append(aIo);
  }
  // Set I/O's and check ...
  seg.setIos(aIoList);
  QCOMPARE(seg.startAddressRead(), 0);
  QCOMPARE(seg.startAddressWrite(), 100);
  QCOMPARE(seg.endAddressRead(), 8);
  QCOMPARE(seg.endAddressWrite(), 108);
  QVERIFY(!seg.containsAddressRead(-1));
  QVERIFY(seg.containsAddressRead(0));
  QVERIFY(seg.containsAddressRead(1));
  QVERIFY(seg.containsAddressRead(2));
  QVERIFY(seg.containsAddressRead(3));
  QVERIFY(seg.containsAddressRead(4));
  QVERIFY(seg.containsAddressRead(5));
  QVERIFY(seg.containsAddressRead(6));
  QVERIFY(seg.containsAddressRead(7));
  QVERIFY(seg.containsAddressRead(8));
  QVERIFY(!seg.containsAddressRead(9));
  QVERIFY(!seg.containsAddressWrite(99));
  QVERIFY(seg.containsAddressWrite(100));
  QVERIFY(seg.containsAddressWrite(101));
  QVERIFY(seg.containsAddressWrite(102));
  QVERIFY(seg.containsAddressWrite(103));
  QVERIFY(seg.containsAddressWrite(104));
  QVERIFY(seg.containsAddressWrite(105));
  QVERIFY(seg.containsAddressWrite(106));
  QVERIFY(seg.containsAddressWrite(107));
  QVERIFY(seg.containsAddressWrite(108));
  QVERIFY(!seg.containsAddressWrite(109));
  QCOMPARE(seg.ioCount(), 9);
  QCOMPARE(seg.addressesRead(), expectedReadAddresses);
  QCOMPARE(seg.addressesWrite(), expectedWriteAddresses);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  QCOMPARE(seg.valuesDouble(), expectedValuesDouble);
  // Set some values and check - correct amount of values - type: mdtValue
  valuesMdt.clear();
  expectedValuesInt.clear();
  for(int i = 0; i < 9; i++){
    valuesMdt.append(2*i);
    expectedValuesInt.append(2*i);
  }
  QCOMPARE(seg.setValues(valuesMdt), 9);
  QCOMPARE(seg.values().size(), 9);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  // Set some values and check - correct amount of values - type: QVariant
  valuesVar.clear();
  expectedValuesInt.clear();
  for(int i = 0; i < 9; i++){
    valuesVar.append(5*i);
    expectedValuesInt.append(5*i);
  }
  QCOMPARE(seg.setValues(valuesVar), 9);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  // Set some values and check - to less values - type: mdtValue
  valuesMdt.clear();
  expectedValuesInt.clear();
  for(int i = 0; i < 5; i++){
    valuesMdt.append(7*i);
    expectedValuesInt.append(7*i);
  }
  QCOMPARE(seg.setValues(valuesMdt), 5);
  QCOMPARE(seg.values().size(), 9);
  for(int i = 0; i < 5; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueInt(), expectedValuesInt.at(i));
    QCOMPARE(seg.valuesInt().at(i), expectedValuesInt.at(i));
  }
  QCOMPARE(seg.values().at(5).valueInt(), 5*5);
  QCOMPARE(seg.values().at(6).valueInt(), 5*6);
  QCOMPARE(seg.values().at(7).valueInt(), 5*7);
  QCOMPARE(seg.values().at(8).valueInt(), 5*8);
  // Set some values and check - to less values - type: QVariant
  valuesVar.clear();
  expectedValuesInt.clear();
  for(int i = 0; i < 3; i++){
    valuesVar.append(3*i);
    expectedValuesInt.append(3*i);
  }
  QCOMPARE(seg.setValues(valuesVar), 3);
  QCOMPARE(seg.values().size(), 9);
  for(int i = 0; i < 3; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueInt(), expectedValuesInt.at(i));
    QCOMPARE(seg.valuesInt().at(i), expectedValuesInt.at(i));
  }
  QCOMPARE(seg.values().at(3).valueInt(), 7*3);
  QCOMPARE(seg.values().at(4).valueInt(), 7*4);
  QCOMPARE(seg.values().at(5).valueInt(), 5*5);
  QCOMPARE(seg.values().at(6).valueInt(), 5*6);
  QCOMPARE(seg.values().at(7).valueInt(), 5*7);
  QCOMPARE(seg.values().at(8).valueInt(), 5*8);
  // Set some values and check - to many values - type: mdtValue
  valuesMdt.clear();
  expectedValuesInt.clear();
  for(int i = 0; i < 20; i++){
    valuesMdt.append(3*i);
    expectedValuesInt.append(3*i);
  }
  QCOMPARE(seg.setValues(valuesMdt), 9);
  QCOMPARE(seg.values().size(), 9);
  for(int i = 0; i < 9; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueInt(), expectedValuesInt.at(i));
    QCOMPARE(seg.valuesInt().at(i), expectedValuesInt.at(i));
  }
  // Set some values and check - to many values - type: QVariant
  valuesVar.clear();
  expectedValuesInt.clear();
  for(int i = 0; i < 30; i++){
    valuesVar.append(2*i);
    expectedValuesInt.append(2*i);
  }
  QCOMPARE(seg.setValues(valuesVar), 9);
  QCOMPARE(seg.values().size(), 9);
  for(int i = 0; i < 9; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueInt(), expectedValuesInt.at(i));
    QCOMPARE(seg.valuesInt().at(i), expectedValuesInt.at(i));
  }
  // Change ve value of last aIo and check that it is reflected
  QVERIFY(seg.valuesInt().at(8) != 11);
  aIo->setValue(11);
  QCOMPARE(seg.valuesInt().at(8), 11);
  // Update some values and check - Address read - type: mdtValue
  expectedValuesInt = seg.valuesInt();
  expectedValuesInt[0] = -10;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesMdt.clear();
  valuesMdt << -10;
  QCOMPARE(seg.updateValuesFromAddressRead(0, valuesMdt), 1);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[2] = -20;
  expectedValuesInt[3] = -30;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesMdt.clear();
  valuesMdt << -20 << -30;
  QCOMPARE(seg.updateValuesFromAddressRead(2, valuesMdt), 2);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[6] = -60;
  expectedValuesInt[7] = -70;
  expectedValuesInt[8] = -80;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesMdt.clear();
  valuesMdt << -60 << -70 << -80 << -90 << -100;
  QCOMPARE(seg.updateValuesFromAddressRead(6, valuesMdt), 3);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  valuesMdt << -2000 << -3000;
  QCOMPARE(seg.updateValuesFromAddressRead(40, valuesMdt), 0);
  // Update some values and check - Address read - type: QVariant
  expectedValuesInt = seg.valuesInt();
  expectedValuesInt[0] = -100;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesVar.clear();
  valuesVar << -100;
  QCOMPARE(seg.updateValuesFromAddressRead(0, valuesVar), 1);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[2] = -200;
  expectedValuesInt[3] = -300;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesVar.clear();
  valuesVar << -200 << -300;
  QCOMPARE(seg.updateValuesFromAddressRead(2, valuesVar), 2);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[6] = -600;
  expectedValuesInt[7] = -700;
  expectedValuesInt[8] = -800;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesVar.clear();
  valuesVar << -600 << -700 << -800 << -900 << -1000;
  QCOMPARE(seg.updateValuesFromAddressRead(6, valuesVar), 3);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  valuesVar << -2000 << -3000;
  QCOMPARE(seg.updateValuesFromAddressRead(40, valuesVar), 0);
  // Update some values and check - Address write - type: mdtValue
  expectedValuesInt = seg.valuesInt();
  expectedValuesInt[0] = -10;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesMdt.clear();
  valuesMdt << -10;
  QCOMPARE(seg.updateValuesFromAddressWrite(100, valuesMdt), 1);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[2] = -20;
  expectedValuesInt[3] = -30;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesMdt.clear();
  valuesMdt << -20 << -30;
  QCOMPARE(seg.updateValuesFromAddressWrite(102, valuesMdt), 2);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[6] = -60;
  expectedValuesInt[7] = -70;
  expectedValuesInt[8] = -80;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesMdt.clear();
  valuesMdt << -60 << -70 << -80 << -90 << -100;
  QCOMPARE(seg.updateValuesFromAddressWrite(106, valuesMdt), 3);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  valuesMdt << -2000 << -3000;
  QCOMPARE(seg.updateValuesFromAddressWrite(140, valuesMdt), 0);
  // Update some values and check - Address write - type: QVariant
  expectedValuesInt = seg.valuesInt();
  expectedValuesInt[0] = -100;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesVar.clear();
  valuesVar << -100;
  QCOMPARE(seg.updateValuesFromAddressWrite(100, valuesVar), 1);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[2] = -200;
  expectedValuesInt[3] = -300;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesVar.clear();
  valuesVar << -200 << -300;
  QCOMPARE(seg.updateValuesFromAddressWrite(102, valuesVar), 2);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[6] = -600;
  expectedValuesInt[7] = -700;
  expectedValuesInt[8] = -800;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesVar.clear();
  valuesVar << -600 << -700 << -800 << -900 << -1000;
  QCOMPARE(seg.updateValuesFromAddressWrite(106, valuesVar), 3);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  valuesVar << -2000 << -3000;
  QCOMPARE(seg.updateValuesFromAddressWrite(140, valuesVar), 0);

  /*
   * Digital I/O
   */

  // Build a list of 10 I/O's
  expectedReadAddresses.clear();
  expectedWriteAddresses.clear();
  expectedValuesBool.clear();
  for(int i = 0; i < 10; i++){
    dIo = new mdtDigitalIo;
    dIo->setAddressRead(i);
    expectedReadAddresses.append(i);
    dIo->setAddressWrite(i+100);
    expectedWriteAddresses.append(i+100);
    dIo->setValue((i%2)==0);
    expectedValuesBool.append((i%2)==0);
    dIoList.append(dIo);
  }
  // Set I/O's and check ...
  seg.setIos(dIoList);
  QCOMPARE(seg.startAddressRead(), 0);
  QCOMPARE(seg.startAddressWrite(), 100);
  QCOMPARE(seg.endAddressRead(), 9);
  QCOMPARE(seg.endAddressWrite(), 109);
  QVERIFY(!seg.containsAddressRead(-1));
  QVERIFY(seg.containsAddressRead(0));
  QVERIFY(seg.containsAddressRead(1));
  QVERIFY(seg.containsAddressRead(2));
  QVERIFY(seg.containsAddressRead(3));
  QVERIFY(seg.containsAddressRead(4));
  QVERIFY(seg.containsAddressRead(5));
  QVERIFY(seg.containsAddressRead(6));
  QVERIFY(seg.containsAddressRead(7));
  QVERIFY(seg.containsAddressRead(8));
  QVERIFY(seg.containsAddressRead(9));
  QVERIFY(!seg.containsAddressRead(10));
  QVERIFY(!seg.containsAddressWrite(99));
  QVERIFY(seg.containsAddressWrite(100));
  QVERIFY(seg.containsAddressWrite(101));
  QVERIFY(seg.containsAddressWrite(102));
  QVERIFY(seg.containsAddressWrite(103));
  QVERIFY(seg.containsAddressWrite(104));
  QVERIFY(seg.containsAddressWrite(105));
  QVERIFY(seg.containsAddressWrite(106));
  QVERIFY(seg.containsAddressWrite(107));
  QVERIFY(seg.containsAddressWrite(108));
  QVERIFY(seg.containsAddressWrite(109));
  QVERIFY(!seg.containsAddressWrite(110));
  QCOMPARE(seg.ioCount(), 10);
  QCOMPARE(seg.addressesRead(), expectedReadAddresses);
  QCOMPARE(seg.addressesWrite(), expectedWriteAddresses);
  QCOMPARE(seg.valuesBool(), expectedValuesBool);
  // Set some values and check - correct amount of values - type: mdtValue
  valuesMdt.clear();
  expectedValuesBool.clear();
  for(int i = 0; i < 10; i++){
    valuesMdt.append((i%2)!=0);
    expectedValuesBool.append((i%2)!=0);
  }
  QCOMPARE(seg.setValues(valuesMdt), 10);
  QCOMPARE(seg.values().size(), 10);
  QCOMPARE(seg.valuesBool(), expectedValuesBool);
  // Set some values and check - correct amount of values - type: QVariant
  valuesVar.clear();
  expectedValuesBool.clear();
  for(int i = 0; i < 10; i++){
    valuesVar.append((i%2)==0);
    expectedValuesBool.append((i%2)==0);
  }
  QCOMPARE(seg.setValues(valuesVar), 10);
  QCOMPARE(seg.valuesBool(), expectedValuesBool);
  // Set some values and check - to less values - type: mdtValue
  valuesMdt.clear();
  expectedValuesBool.clear();
  for(int i = 0; i < 5; i++){
    valuesMdt.append((i%2)!=0);
    expectedValuesBool.append((i%2)!=0);
  }
  QCOMPARE(seg.setValues(valuesMdt), 5);
  QCOMPARE(seg.values().size(), 10);
  for(int i = 0; i < 5; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueBool(), expectedValuesBool.at(i));
    QCOMPARE(seg.valuesBool().at(i), expectedValuesBool.at(i));
  }
  QCOMPARE(seg.values().at(6).valueBool(), (6%2)==0);
  QCOMPARE(seg.values().at(7).valueBool(), (7%2)==0);
  QCOMPARE(seg.values().at(8).valueBool(), (8%2)==0);
  // Set some values and check - to less values - type: QVariant
  valuesVar.clear();
  expectedValuesBool.clear();
  for(int i = 0; i < 3; i++){
    valuesVar.append((i%2)==0);
    expectedValuesBool.append((i%2)==0);
  }
  QCOMPARE(seg.setValues(valuesVar), 3);
  QCOMPARE(seg.values().size(), 10);
  for(int i = 0; i < 3; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueBool(), expectedValuesBool.at(i));
    QCOMPARE(seg.valuesBool().at(i), expectedValuesBool.at(i));
  }
  QCOMPARE(seg.values().at(3).valueBool(), (3%2)!= 0);
  QCOMPARE(seg.values().at(4).valueBool(), (4%2)!= 0);
  QCOMPARE(seg.values().at(5).valueBool(), (5%2)==0);
  QCOMPARE(seg.values().at(6).valueBool(), (6%2)==0);
  QCOMPARE(seg.values().at(7).valueBool(), (7%2)==0);
  QCOMPARE(seg.values().at(8).valueBool(), (8%2)==0);
  // Set some values and check - to many values - type: mdtValue
  valuesMdt.clear();
  expectedValuesBool.clear();
  for(int i = 0; i < 20; i++){
    valuesMdt.append((i%2)!=0);
    expectedValuesBool.append((i%2)!=0);
  }
  QCOMPARE(seg.setValues(valuesMdt), 10);
  QCOMPARE(seg.values().size(), 10);
  for(int i = 0; i < 9; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueBool(), expectedValuesBool.at(i));
    QCOMPARE(seg.valuesBool().at(i), expectedValuesBool.at(i));
  }
  // Set some values and check - to many values - type: QVariant
  valuesVar.clear();
  expectedValuesBool.clear();
  for(int i = 0; i < 30; i++){
    valuesVar.append((i%2)==0);
    expectedValuesBool.append((i%2)==0);
  }
  QCOMPARE(seg.setValues(valuesVar), 10);
  QCOMPARE(seg.values().size(), 10);
  for(int i = 0; i < 9; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueBool(), expectedValuesBool.at(i));
    QCOMPARE(seg.valuesBool().at(i), expectedValuesBool.at(i));
  }
  // Change value of last aIo and check that it is reflected
  dIo->setValue(true);
  QCOMPARE(seg.valuesBool().at(9), true);
  dIo->setValue(false);
  QCOMPARE(seg.valuesBool().at(9), false);
  // Update some values and check - type: mdtValue
  
  // Update some values and check - type: QVariant

}

void mdtDeviceTest::deviceIosTest()
{
  mdtDeviceIos ios;
  mdtAnalogIo *ai;
  QList<mdtAnalogIo*> aIoList;
  mdtAnalogIo *ao;
  mdtDigitalIo *di;
  QList<mdtDigitalIo*> dIoList;
  mdtDigitalIo *dout;
  QList<QVariant> values;

  // Add analog inputs
  ai = new mdtAnalogIo;
  ai->setAddress(12);
  ai->setRange(-100.0, 100.0, 24, 0, true);
  ios.addAnalogInput(ai);
  ai = new mdtAnalogIo;
  ai->setAddress(10);
  ai->setRange(-100.0, 100.0, 24, 0, true);
  ai->setLabelShort("AI10");
  ios.addAnalogInput(ai);

  // Add analog outputs
  ao = new mdtAnalogIo;
  ao->setAddressRead(15);
  ao->setAddressWrite(115);
  ao->setRange(-100.0, 100.0, 24, 0, true);
  ios.addAnalogOutput(ao);
  ao = new mdtAnalogIo;
  ao->setAddressRead(8);
  ao->setAddressWrite(108);
  ao->setLabelShort("AO8");
  ao->setRange(-100.0, 100.0, 24, 0, true);
  ios.addAnalogOutput(ao);

  // Add digital inputs
  di = new mdtDigitalIo;
  di->setAddress(17);
  ios.addDigitalInput(di);
  di = new mdtDigitalIo;
  di->setAddress(18);
  di->setLabelShort("DI18");
  ios.addDigitalInput(di);

  // Add digital outputs
  dout = new mdtDigitalIo;
  dout->setAddressRead(20);
  dout->setAddressWrite(120);
  ios.addDigitalOutput(dout);
  dout = new mdtDigitalIo;
  dout->setAddressRead(30);
  dout->setAddressWrite(130);
  dout->setLabelShort("DO30");
  ios.addDigitalOutput(dout);

  // Check analog inputs
  QCOMPARE(ios.analogInputsCount(), 2);
  ///QCOMPARE(ios.analogInputsFirstAddress(), 10);
  QVERIFY(ios.analogInputAt(10) != 0);
  QCOMPARE(ios.analogInputAt(10)->address() , 10);
  QVERIFY(ios.analogInputWithLabelShort("AI10") != 0);
  QCOMPARE(ios.analogInputWithLabelShort("AI10")->address(), 10);
  QCOMPARE(ios.analogInputWithLabelShort("AI10")->labelShort(), QString("AI10"));
  QVERIFY(ios.analogInputAt(11) == 0);
  QVERIFY(ios.analogInputAt(12) != 0);
  QCOMPARE(ios.analogInputAt(12)->address() , 12);
  ios.analogInputAt(12)->setLabelShort("Temp. M12");
  QVERIFY(ios.analogInputWithLabelShort("") == 0);
  QVERIFY(ios.analogInputWithLabelShort("Temp. M12") != 0);
  QCOMPARE(ios.analogInputWithLabelShort("Temp. M12")->address(), 12);
  QCOMPARE(ios.analogInputWithLabelShort("Temp. M12")->labelShort(), QString("Temp. M12"));
  aIoList = ios.analogInputs();
  QCOMPARE(aIoList.size(), 2);
  values.clear();
  values << 1.0 << 12.5;
  ios.updateAnalogInputValues(values, -1, -1);
  QCOMPARE(ios.analogInputAt(10)->value().valueDouble() , 1.0);
  QCOMPARE(ios.analogInputAt(12)->value().valueDouble() , 12.5);
  values.clear();
  values << 2.0 << 3.0;
  ios.updateAnalogInputValues(values, -1, 1);
  QCOMPARE(ios.analogInputAt(10)->value().valueDouble() , 2.0);
  QCOMPARE(ios.analogInputAt(12)->value().valueDouble() , 12.5);
  values.clear();
  values << 4.0 << 5.0;
  ios.updateAnalogInputValues(values, 10, -1);
  QCOMPARE(ios.analogInputAt(10)->value().valueDouble() , 4.0);
  QCOMPARE(ios.analogInputAt(12)->value().valueDouble() , 5.0);
  values.clear();
  values << 6.0 << 7.0;
  ios.updateAnalogInputValues(values, 10, 1);
  QCOMPARE(ios.analogInputAt(10)->value().valueDouble() , 6.0);
  QCOMPARE(ios.analogInputAt(12)->value().valueDouble() , 5.0);
  values.clear();
  values << 8.0;
  ios.updateAnalogInputValues(values, 12, 1);
  QCOMPARE(ios.analogInputAt(10)->value().valueDouble() , 6.0);
  QCOMPARE(ios.analogInputAt(12)->value().valueDouble() , 8.0);
  values.clear();
  values << 9.0 << 10.0;
  ios.updateAnalogInputValues(values, 12, 1);
  QCOMPARE(ios.analogInputAt(10)->value().valueDouble() , 6.0);
  QCOMPARE(ios.analogInputAt(12)->value().valueDouble() , 9.0);

  // Check analog outputs
  QVERIFY(ios.analogOutputAtAddressRead(13) == 0);
  QVERIFY(ios.analogOutputAtAddressWrite(13) == 0);
  QVERIFY(ios.analogOutputAtAddressRead(15) != 0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->addressRead() , 15);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->addressWrite() , 115);
  QVERIFY(ios.analogOutputAtAddressWrite(15) == 0);
  QVERIFY(ios.analogOutputAtAddressWrite(115) != 0);
  QCOMPARE(ios.analogOutputAtAddressWrite(115)->addressRead() , 15);
  QCOMPARE(ios.analogOutputAtAddressWrite(115)->addressWrite() , 115);
  ios.analogOutputAtAddressRead(15)->setLabelShort("Setpoint 15");
  QVERIFY(ios.analogOutputWithLabelShort("") == 0);
  QVERIFY(ios.analogOutputWithLabelShort("Setpoint 15") != 0);
  QCOMPARE(ios.analogOutputWithLabelShort("Setpoint 15")->addressRead(), 15);
  QCOMPARE(ios.analogOutputWithLabelShort("Setpoint 15")->addressWrite(), 115);
  QCOMPARE(ios.analogOutputWithLabelShort("Setpoint 15")->labelShort(), QString("Setpoint 15"));
  QVERIFY(ios.analogOutputAtAddressRead(8) != 0);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->addressRead() , 8);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->addressWrite() , 108);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->labelShort() , QString("AO8"));
  QVERIFY(ios.analogOutputAtAddressWrite(108) != 0);
  QCOMPARE(ios.analogOutputAtAddressWrite(108)->addressRead() , 8);
  QCOMPARE(ios.analogOutputAtAddressWrite(108)->addressWrite() , 108);
  QCOMPARE(ios.analogOutputAtAddressWrite(108)->labelShort() , QString("AO8"));
  aIoList = ios.analogOutputs();
  QCOMPARE(aIoList.size() , 2);
  QCOMPARE(ios.analogOutputsFirstAddressRead(), 8);
  QCOMPARE(ios.analogOutputsFirstAddressWrite(), 108);
  values.clear();
  values << 1.0 << 12.5;
  ios.updateAnalogOutputValues(values, -1, -1);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->value().valueDouble() , 1.0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->value().valueDouble() , 12.5);
  values.clear();
  values << 2.0 << 3.0;
  ios.updateAnalogOutputValues(values, -1, 1);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->value().valueDouble() , 2.0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->value().valueDouble() , 12.5);
  values.clear();
  values << 4.0 << 5.0;
  ios.updateAnalogOutputValues(values, 8, -1);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->value().valueDouble() , 4.0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->value().valueDouble() , 5.0);
  values.clear();
  values << 6.0 << 7.0;
  ios.updateAnalogOutputValues(values, 8, 1);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->value().valueDouble() , 6.0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->value().valueDouble() , 5.0);
  values.clear();
  values << 8.0;
  ios.updateAnalogOutputValues(values, 15, 1);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->value().valueDouble() , 6.0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->value().valueDouble() , 8.0);
  values.clear();
  values << 9.0 << 10.0;
  ios.updateAnalogOutputValues(values, 15, 1);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->value().valueDouble() , 6.0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->value().valueDouble() , 9.0);

  // Check digital inputs
  QVERIFY(ios.digitalInputAt(11) == 0);
  QVERIFY(ios.digitalInputAt(17) != 0);
  QCOMPARE(ios.digitalInputAt(17)->address(), 17);
  ios.digitalInputAt(17)->setLabelShort("DI17");
  QVERIFY(ios.digitalInputWithLabelShort("") == 0);
  QVERIFY(ios.digitalInputWithLabelShort("DI17") != 0);
  QCOMPARE(ios.digitalInputWithLabelShort("DI17")->address(), 17);
  QCOMPARE(ios.digitalInputWithLabelShort("DI17")->labelShort(), QString("DI17"));
  QVERIFY(ios.digitalInputAt(18) != 0);
  QCOMPARE(ios.digitalInputAt(18)->address(), 18);
  QVERIFY(ios.digitalInputWithLabelShort("DI18") != 0);
  QCOMPARE(ios.digitalInputWithLabelShort("DI18")->address(), 18);
  QCOMPARE(ios.digitalInputWithLabelShort("DI18")->labelShort(), QString("DI18"));
  dIoList = ios.digitalInputs();
  QCOMPARE(dIoList.size(), 2);
  QCOMPARE(ios.digitalInputsFirstAddress(), 17);
  // Check multiple digital inputs update - List contains the correct number of items
  values.clear();
  values << false << true;
  ios.updateDigitalInputValues(values, -1, -1);
  QCOMPARE(ios.digitalInputAt(17)->value().valueBool(), false);
  QCOMPARE(ios.digitalInputAt(18)->value().valueBool(), true);
  values.clear();
  values << true;
  ios.updateDigitalInputValues(values, -1, 1);
  QCOMPARE(ios.digitalInputAt(17)->value().valueBool(), true);
  QCOMPARE(ios.digitalInputAt(18)->value().valueBool(), true);
  values.clear();
  values << false;
  ios.updateDigitalInputValues(values, 18, 1);
  QCOMPARE(ios.digitalInputAt(17)->value().valueBool(), true);
  QCOMPARE(ios.digitalInputAt(18)->value().valueBool(), false);
  // Check multiple digital inputs update - List contains the to much items
  values.clear();
  values << false << true;
  ios.updateDigitalInputValues(values, -1, -1);
  QCOMPARE(ios.digitalInputAt(17)->value().valueBool(), false);
  QCOMPARE(ios.digitalInputAt(18)->value().valueBool(), true);
  values.clear();
  values << true << false;
  ios.updateDigitalInputValues(values, -1, 1);
  QCOMPARE(ios.digitalInputAt(17)->value().valueBool(), true);
  QCOMPARE(ios.digitalInputAt(18)->value().valueBool(), true);
  values.clear();
  values << false << false;
  ios.updateDigitalInputValues(values, 18, 1);
  QCOMPARE(ios.digitalInputAt(17)->value().valueBool(), true);
  QCOMPARE(ios.digitalInputAt(18)->value().valueBool(), false);

  // Check digital outputs
  QVERIFY(ios.digitalOutputAtAddressRead(19) == 0);
  QVERIFY(ios.digitalOutputAtAddressRead(20) != 0);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->addressRead(), 20);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->addressWrite(), 120);
  QVERIFY(ios.digitalOutputAtAddressWrite(19) == 0);
  QVERIFY(ios.digitalOutputAtAddressWrite(120) != 0);
  QCOMPARE(ios.digitalOutputAtAddressWrite(120)->addressRead(), 20);
  QCOMPARE(ios.digitalOutputAtAddressWrite(120)->addressWrite(), 120);
  ios.digitalOutputAtAddressRead(20)->setLabelShort("K20");
  QVERIFY(ios.digitalOutputWithLabelShort("") == 0);
  QVERIFY(ios.digitalOutputWithLabelShort("K20") != 0);
  QCOMPARE(ios.digitalOutputWithLabelShort("K20")->addressRead(), 20);
  QCOMPARE(ios.digitalOutputWithLabelShort("K20")->addressWrite(), 120);
  QCOMPARE(ios.digitalOutputWithLabelShort("K20")->labelShort(), QString("K20"));
  QVERIFY(ios.digitalOutputAtAddressRead(30) != 0);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->addressRead(), 30);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->addressWrite(), 130);
  QVERIFY(ios.digitalOutputAtAddressWrite(130) != 0);
  QCOMPARE(ios.digitalOutputAtAddressWrite(130)->addressRead(), 30);
  QCOMPARE(ios.digitalOutputAtAddressWrite(130)->addressWrite(), 130);
  QVERIFY(ios.digitalOutputWithLabelShort("DO30") != 0);
  QCOMPARE(ios.digitalOutputWithLabelShort("DO30")->addressRead(), 30);
  QCOMPARE(ios.digitalOutputWithLabelShort("DO30")->addressWrite(), 130);
  QCOMPARE(ios.digitalOutputWithLabelShort("DO30")->labelShort(), QString("DO30"));
  dIoList = ios.digitalOutputs();
  QCOMPARE(dIoList.size(), 2);
  QCOMPARE(ios.digitalOutputsFirstAddressRead(), 20);
  QCOMPARE(ios.digitalOutputsFirstAddressWrite(), 120);
  // Check multiple digital outputs update - List contains the correct number of items
  values.clear();
  values << false << true;
  ios.updateDigitalOutputValues(values, -1, -1);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->value().valueBool(), false);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->value().valueBool(), true);
  values.clear();
  values << true;
  ios.updateDigitalOutputValues(values, -1, 1);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->value().valueBool(), true);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->value().valueBool(), true);
  values.clear();
  values << false;
  ios.updateDigitalOutputValues(values, 30, 1);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->value().valueBool(), true);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->value().valueBool(), false);
  // Check multiple digital outputs update - List contains the to much items
  values.clear();
  values << false << true;
  ios.updateDigitalOutputValues(values, -1, -1);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->value().valueBool(), false);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->value().valueBool(), true);
  values.clear();
  values << true << false;
  ios.updateDigitalOutputValues(values, -1, 1);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->value().valueBool(), true);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->value().valueBool(), true);
  values.clear();
  values << false << false;
  ios.updateDigitalOutputValues(values, 30, 1);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->value().valueBool(), true);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->value().valueBool(), false);

}

void mdtDeviceTest::deviceIosSegmentStorageTest()
{
  mdtDeviceIos ios;
  mdtAnalogIo *ai;
  mdtDeviceIosSegment *seg;
  ///mdtAnalogIo *ao;
  ///mdtDigitalIo *di;
  ///mdtDigitalIo *dout;
  QList<int> expectedValuesInt;
  QList<QVariant> valuesVar;

  /*
   * Analog IN
   */

  // Add analog inputs
  ai = new mdtAnalogIo;
  ai->setAddress(0);
  ios.addAnalogInput(ai);
  ai = new mdtAnalogIo;
  ai->setAddress(2);
  ios.addAnalogInput(ai);
  ai = new mdtAnalogIo;
  ai->setAddress(1);
  ios.addAnalogInput(ai);
  ai = new mdtAnalogIo;
  ai->setAddress(5);
  ios.addAnalogInput(ai);
  ai = new mdtAnalogIo;
  ai->setAddress(6);
  ios.addAnalogInput(ai);
  // Check segments
  QCOMPARE(ios.analogInputsSegments().size(), 2);
  // First segment
  seg = ios.analogInputsSegments().at(0);
  QVERIFY(seg != 0);
  QCOMPARE(seg->addressesRead().size(), 3);
  QCOMPARE(seg->addressesRead().at(0), 0);
  QCOMPARE(seg->addressesRead().at(1), 1);
  QCOMPARE(seg->addressesRead().at(2), 2);
  // First segment - Set some values and check
  ai = ios.analogInputAt(0);
  QVERIFY(ai != 0);
  ai->setValue(10);
  ai = ios.analogInputAt(1);
  QVERIFY(ai != 0);
  ai->setValue(11);
  ai = ios.analogInputAt(2);
  QVERIFY(ai != 0);
  ai->setValue(12);
  expectedValuesInt.clear();
  expectedValuesInt << 10 << 11 << 12;
  QCOMPARE(seg->valuesInt(), expectedValuesInt);
  // Second segment
  seg = ios.analogInputsSegments().at(1);
  QVERIFY(seg != 0);
  QCOMPARE(seg->addressesRead().size(), 2);
  QCOMPARE(seg->addressesRead().at(0), 5);
  QCOMPARE(seg->addressesRead().at(1), 6);
  // Second segment - Set some values and check
  ai = ios.analogInputAt(5);
  QVERIFY(ai != 0);
  ai->setValue(15);
  ai = ios.analogInputAt(6);
  QVERIFY(ai != 0);
  ai->setValue(16);
  expectedValuesInt.clear();
  expectedValuesInt << 15 << 16;
  QCOMPARE(seg->valuesInt(), expectedValuesInt);
  // Check grouped updates - Only first segment
  valuesVar.clear();
  valuesVar << -10;
  QVERIFY(ios.analogInputAt(0) != 0);
  QVERIFY(ios.analogInputAt(0)->value().valueInt() != -10);
  ios.updateAnalogInputValues(valuesVar, 0, -1);
  QCOMPARE(ios.analogInputAt(0)->value().valueInt(), -10);
  ios.updateAnalogInputValues(valuesVar, 3, -1);  // Address 3 not exists
  QCOMPARE(ios.analogInputAt(0)->value().valueInt(), -10);
  // Check grouped updates - first + second segments
  // Note: considere that we only included I/O's with addresses 0,1,2,5,6 in container, but we made a query to device for addresses 2-6
  valuesVar.clear();
  valuesVar << 200 << 300 << 400 << 500 << 600;
  QVERIFY(ios.analogInputAt(2) != 0);
  QVERIFY(ios.analogInputAt(2)->value().valueInt() != 200);
  QVERIFY(ios.analogInputAt(5) != 0);
  QVERIFY(ios.analogInputAt(5)->value().valueInt() != 500);
  QVERIFY(ios.analogInputAt(6) != 0);
  QVERIFY(ios.analogInputAt(6)->value().valueInt() != 600);
  ios.updateAnalogInputValues(valuesVar, 2, -1, true);
  QCOMPARE(ios.analogInputAt(2)->value().valueInt(), 200);
  QCOMPARE(ios.analogInputAt(5)->value().valueInt(), 500);
  QCOMPARE(ios.analogInputAt(6)->value().valueInt(), 600);
  // Same story, but we queried device about addresses 0-3 (and I/O with address 3 does not exist in container)
  valuesVar.clear();
  valuesVar << 0 << 10 << 20 << 30;
  QVERIFY(ios.analogInputAt(0) != 0);
  QVERIFY(ios.analogInputAt(0)->value().valueInt() != 0);
  QVERIFY(ios.analogInputAt(1) != 0);
  QVERIFY(ios.analogInputAt(1)->value().valueInt() != 10);
  QVERIFY(ios.analogInputAt(2) != 0);
  QVERIFY(ios.analogInputAt(2)->value().valueInt() != 20);
  // We will check that 4th I/O was untouched
  QVERIFY(ios.analogInputAt(5) != 0);
  QVERIFY(ios.analogInputAt(5)->value().valueInt() == 500);
  ios.updateAnalogInputValues(valuesVar, -1, -1, true);
  QCOMPARE(ios.analogInputAt(0)->value().valueInt(), 0);
  QCOMPARE(ios.analogInputAt(1)->value().valueInt(), 10);
  QCOMPARE(ios.analogInputAt(2)->value().valueInt(), 20);
  // Check that 4th I/O was untouched
  QCOMPARE(ios.analogInputAt(5)->value().valueInt(), 500);


  
}

void mdtDeviceTest::deviceIosWidgetTest()
{
  mdtDeviceIosWidget w;
  mdtDeviceIos ios;
  mdtAnalogIo *ai;
  mdtAnalogIo *ao;
  mdtDigitalIo *di;
  mdtDigitalIo *dout;

  // Add analog inputs
  ai = new mdtAnalogIo;
  ai->setAddress(12);
  ai->setLabelShort("Vin");
  ai->setLabel("Input voltage");
  ai->setDetails("Input voltage measured from AG1\nSee schema 15G");
  QVERIFY(ai->setRange(0.0, 10.0, 16));
  ///ios.addAnalogInput(ai);

  ai = new mdtAnalogIo;
  ai->setAddress(13);
  ai->setLabelShort("wX_BatUBa");
  ai->setLabel("rBatSpannung");
  ai->setDetails("Batteriespannung\nSchema: 06E");
  QVERIFY(ai->setRange(0.0, 50.0, 16));
  ios.addAnalogInput(ai);

  // Add analog outputs
  ao = new mdtAnalogIo;
  ao->setAddress(15);
  ao->setLabelShort("Vout");
  ao->setLabel("Output voltage for AG speed");
  ao->setDetails("Output voltage for AG speed setpoint\nSee schema 16F");
  QVERIFY(ao->setRange(0.0, 10.0, 16));
  ios.addAnalogOutput(ao);

  // Add digital inputs
  di = new mdtDigitalIo;
  di->setAddress(17);
  di->setLabelShort("Test");
  di->setLabel("Test mode");
  di->setDetails("Test mode enabled.\nIf ON, it's possible to set some outputs from tool\nSee schema 25G");
  ios.addDigitalInput(di);

  di = new mdtDigitalIo;
  di->setAddress(18);
  di->setLabelShort("xM_HSEin");
  di->setLabel(tr("xHochspannung"));
  di->setDetails("Hoschspannung vorhanden\nSchema: 06E");
  ios.addDigitalInput(di);

  di = new mdtDigitalIo;
  di->setAddress(19);
  di->setLabelShort("xM_MgBr");
  di->setLabel(tr("xMgBremse"));
  di->setDetails("Magnetische Bremse\nSchema: 06E");
  ios.addDigitalInput(di);

  // Add digital outputs
  dout = new mdtDigitalIo;
  dout->setAddress(20);
  dout->setLabelShort("xB_BelEin");
  dout->setLabel(tr("xBeleuchtungEin"));
  dout->setDetails(tr("Drehschalter Beleuchtung Ein") + "\n" + tr("Schema: 10A"));
  ios.addDigitalOutput(dout);

  dout = new mdtDigitalIo;
  dout->setAddress(21);
  dout->setLabelShort("xB_BatMinU");
  dout->setLabel(tr("xBatMinSpg"));
  dout->setDetails(tr("Batterie Minimalspannung") + "\n" + tr("Schema: 06C"));
  ios.addDigitalOutput(dout);

  // Setup widget
  ///w.setMaxColumns(3);
  w.setDeviceIos(&ios);
  w.show();

  /**
  while(w.isVisible()){
    QTest::qWait(1000);
  }
  */
  // Second setup (must clear existing widget and redo layout)
  w.setDeviceIos(&ios);
  w.show();
  /**
  while(w.isVisible()){
    QTest::qWait(1000);
  }
  */
}

void mdtDeviceTest::modbusTest()
{
  mdtDeviceModbus d;
  mdtModbusTcpPortManager *m = d.modbusTcpPortManager();
  QList<mdtPortInfo*> portInfoList;
  int hwNodeId = 3;
  mdtDeviceIos ios;
  mdtDeviceIosWidget *iosw;
  mdtDigitalIo *di;
  mdtDeviceWindow dw;

  QVERIFY(m != 0);

  // Setup some digital inputs (they are not device dependent)
  di = new mdtDigitalIo;
  di->setAddress(0);
  di->setLabelShort("DI1");
  di->setDetails("Some details about digital input 1");
  ios.addDigitalInput(di);  
  di = new mdtDigitalIo;
  di->setAddress(1);
  di->setLabelShort("DI2");
  di->setDetails("Some details about digital input 2");
  ios.addDigitalInput(di);  

  // Setup I/O's widget
  iosw = new mdtDeviceIosWidget;
  iosw->setDeviceIos(&ios);

  // Setup device
  d.setIos(&ios, true);
  dw.setDevice(&d);
  dw.setIosWidget(iosw);
  dw.show();

  // Scan looking in chache file first
  portInfoList = m->scan(m->readScanResult());
  // Try to connect ...
  if(d.connectToDevice(portInfoList, hwNodeId, 4) != mdtAbstractPort::NoError){
    // scan network an try again
    qDeleteAll(portInfoList);
    portInfoList.clear();
    qDebug() << "Scanning network ...";
    portInfoList = m->scan(QNetworkInterface::allInterfaces(), 502, 100);
    if(d.connectToDevice(portInfoList, hwNodeId, 4) != mdtAbstractPort::NoError){
      QSKIP("Modbus device with requested harware node ID not found", SkipAll);
    }
    // Ok found, save scan result
    QVERIFY(m->saveScanResult(portInfoList));
    // We no lobger need portInfoList
    qDeleteAll(portInfoList);
    portInfoList.clear();
  }

  // Make some queries
  QVERIFY(!d.getDigitalInputValue(0, false, false).isValid());
  QVERIFY(d.getDigitalInputValue(0, true, true).isValid());
  QVERIFY(d.getDigitalInputValue(0, false, false).isValid());
  QVERIFY(!d.getDigitalInputValue(0, true, false).isValid());
  ///QVERIFY(!d.getDigitalInputState(0, 0).isValid());
  QVERIFY(d.getDigitalOutputs(500) >= 0);
  d.start(100);
  while(dw.isVisible()){
    QTest::qWait(1000);
  }
}

void mdtDeviceTest::modbusWagoTest()
{
  mdtDeviceModbusWago d;
  mdtDeviceIos ios;
  mdtDeviceIosWidget *iosw;
  mdtDeviceWindow dw;
  int setGetWaitTime = 30;  // [ms]

  /*
   * Setup I/O's
   */

  // Setup I/O's widget
  iosw = new mdtDeviceIosWidget;
  iosw->setDeviceIos(&ios);

  // Setup device
  d.setIos(&ios, false);
  ///d.setAnalogOutputAddressOffset(0x0200);
  ///d.setDigitalOutputAddressOffset(0x0200);
  ///d.portManager()->config().setReadTimeout(30000);  // We set a long timeout in case we use valgrind
  dw.setDevice(&d);
  dw.setIosWidget(iosw);
  dw.show();

  if(d.connectToDevice(mdtDeviceInfo()) != mdtAbstractPort::NoError){;
    QSKIP("No Wago 750 device found, or other error", SkipAll);
  }
  ///QVERIFY(d.portManager()->isRunning());
  QVERIFY(d.portManager()->isReady());
  qDebug() << "Analog outputs: " << d.analogOutputsCount();
  qDebug() << "Analog inputs: " << d.analogInputsCount();
  qDebug() << "Digital outputs: " << d.digitalOutputsCount();
  qDebug() << "Digital inputs: " << d.digitalInputsCount();
  QVERIFY(d.detectIos(&ios));
  d.setIos(&ios, true);
  iosw->setDeviceIos(&ios);

  /*
   * Tests
   *
   * Note: we make set/get on outputs.
   *  Because a set or get can be done fast (2-30 ms),
   *  we let the PLC some time to update its cache (else it sometimes returns us a cached value)
   */

  // Analog inputs
  QVERIFY(!d.getAnalogInputValue(0, false, false).isValid());
  QVERIFY(!d.getAnalogInputValue(0, true, false).isValid());
  QVERIFY(d.getAnalogInputValue(0, true, true).isValid());
  QVERIFY(d.getAnalogInputValue(0, false, false).isValid());
  QVERIFY(!d.getAnalogInputValue(0, true, false).isValid());
  QVERIFY(d.getAnalogInputValue("AI1", true, true).isValid());
  QVERIFY(!d.getAnalogInputValue("AI1", true, false).isValid());
  QVERIFY(d.getAnalogInputValue(1, true, true).isValid());
  QVERIFY(d.getAnalogInputValue(1, false, false).isValid());
  QVERIFY(d.getAnalogInputValue("AI2", true, true).isValid());
  QVERIFY(!d.getAnalogInputValue("AI2", true, false).isValid());
  QVERIFY(d.getAnalogInputs(true) >= 0);

  // Analog outputs
  QVERIFY(!d.getAnalogOutputValue(0x200, false, false).isValid());
  QVERIFY(d.getAnalogOutputValue(0x200, true, true).isValid());
  QVERIFY(d.getAnalogOutputValue(0x200, false, false).isValid());
  QVERIFY(!d.getAnalogOutputValue(0x200, true, false).isValid());
  QVERIFY(d.setAnalogOutputValue(0, 2.5, true, true) >= 0);
  d.wait(setGetWaitTime);
  QVERIFY(d.getAnalogOutputValue(0x200, true, true).isValid());
  QVERIFY(d.getAnalogOutputs(true) >= 0);
  QVERIFY(d.getAnalogOutputValue(0x200, false, false).isValid());
  MDT_COMPARE(d.getAnalogOutputValue(0x200, false, false).valueDouble(), 2.5, 12, -10.0, 10.0);
  QVERIFY(d.setAnalogOutputValue("AO1", 2.5, true, true) >= 0);
  d.wait(setGetWaitTime);
  QVERIFY(d.getAnalogOutputValue("AO1", true, true).isValid());
  ///QVERIFY(qAbs(d.getAnalogOutputValue(0x200, 500, true).valueDouble()-2.5) < (10.0/4050.0));
  MDT_COMPARE(d.getAnalogOutputValue(0x200, true, true).valueDouble(), 2.5, 12, -10.0, 10.0);
  QVERIFY(d.getAnalogOutputValue(0x200, false, false).isValid());
  // Grouped query
  QVERIFY(d.setAnalogOutputValue(0, 1.5, false, false) == 0);
  QVERIFY(d.setAnalogOutputValue(1, 2.5, false, false) == 0);
  QVERIFY(d.setAnalogOutputs(true) >= 0);
  ///QVERIFY(qAbs(d.getAnalogOutputValue(0x200, 500, true).toDouble()-1.5) < (10.0/4050.0));
  ///QVERIFY(qAbs(d.getAnalogOutputValue(0x201, 500, true).toDouble()-2.5) < (10.0/4050.0));
  d.wait(setGetWaitTime);
  MDT_COMPARE(d.getAnalogOutputValue(0x200, true, true).valueDouble(), 1.5, 12, -10.0, 10.0);
  MDT_COMPARE(d.getAnalogOutputValue(0x201, true, true).valueDouble(), 2.5, 12, -10.0, 10.0);
  MDT_COMPARE(d.getAnalogOutputValue("AO1", true, true).valueDouble(), 1.5, 12, -10.0, 10.0);
  MDT_COMPARE(d.getAnalogOutputValue("AO2", true, true).valueDouble(), 2.5, 12, -10.0, 10.0);

  // Digital inputs
  QVERIFY(!d.getDigitalInputValue(0, false, false).isValid());
  QVERIFY(d.getDigitalInputValue(0, true, true).isValid());
  QVERIFY(d.getDigitalInputValue(0, false, false).isValid());
  QVERIFY(!d.getDigitalInputValue(0, true, false).isValid());
  QVERIFY(d.getDigitalInputs(true) >= 0);
  QVERIFY(d.getDigitalInputValue(0, false, false).isValid());
  
  ///QVERIFY(!d.getDigitalInputState(0x200, 0).isValid());
  ///QVERIFY(d.getDigitalInputState(0, 500).isValid());
  ///QTest::qWait(500);

  // Digital outputs
  QVERIFY(!d.getDigitalOutputValue(0x200, false, false).isValid());
  QVERIFY(d.getDigitalOutputValue(0x200, true, true).isValid());
  QVERIFY(d.getDigitalOutputValue(0x200, false, false).isValid());
  QVERIFY(!d.getDigitalOutputValue(0x200, true, false).isValid());
  QVERIFY(d.getDigitalOutputs(true) >= 0);
  QVERIFY(d.getDigitalOutputValue(0x200, false, false).isValid());
  QVERIFY(d.setDigitalOutputValue(0, true, true, true) >= 0);
  d.wait(setGetWaitTime);
  QCOMPARE(d.getDigitalOutputValue(0x200, true, true).valueBool(), true);
  QVERIFY(d.setDigitalOutputValue(0, false, true, true) >= 0);
  d.wait(setGetWaitTime);
  QCOMPARE(d.getDigitalOutputValue(0x200, true, true).valueBool(), false);
  QVERIFY(d.setDigitalOutputValue("DO1", true, true, true) >= 0);
  d.wait(setGetWaitTime);
  QCOMPARE(d.getDigitalOutputValue("DO1", true, true).valueBool(), true);
  QCOMPARE(d.getDigitalOutputValue(0x200, true, true).valueBool(), true);
  QVERIFY(d.setDigitalOutputValue("DO1", false, true, true) >= 0);
  d.wait(setGetWaitTime);
  QCOMPARE(d.getDigitalOutputValue("DO1", true, true).valueBool(), false);
  QCOMPARE(d.getDigitalOutputValue(0x200, true, true).valueBool(), false);
  ///QVERIFY(!d.getDigitalOutputState(0x200, 0).isValid());
  ///qDebug() << "TEST , getDigitalOutputState() ...";
  ///QVERIFY(d.getDigitalOutputState(0x200, 500).isValid());
  // Grouped query
  QVERIFY(d.setDigitalOutputValue(0, true, false, false) == 0);
  QVERIFY(d.setDigitalOutputValue("DO2", true, false, false) == 0);
  QVERIFY(d.setDigitalOutputs(true) >= 0);
  QCOMPARE(d.getDigitalOutputValue("DO1", true, true).valueBool(), true);
  QCOMPARE(d.getDigitalOutputValue(0x200, true, true).valueBool(), true);

  
  ///QVERIFY(d.setDigitalOutputState(0, true, false, false) == 0);
  ///QVERIFY(d.setDigitalOutputState("DO2", true, false, false) == 0);
  ///QVERIFY(d.setDigitalOutputState(8, true, false, false) == 0);

  

  ///QVERIFY(d.getDigitalInputs(500) >= 0);
  ///QVERIFY(d.getAnalogOutputs(500) >= 0);
  ///QVERIFY(d.getDigitalOutputs(500) >= 0);
  d.start(100);
  while(dw.isVisible()){
    QTest::qWait(1000);
  }
}

void mdtDeviceTest::modbusBeckhoffTest()
{
  QSKIP("Beckhoff is not supported for the moment", SkipAll);

  mdtDeviceModbus d;
  mdtDeviceIos ios;
  mdtDeviceIosWidget *iosw;
  mdtAnalogIo *ai;
  mdtAnalogIo *ao;
  mdtDigitalIo *di;
  mdtDigitalIo *dout;
  mdtDeviceWindow dw;

  /*
   * Setup I/O's
   */

  // Analog inputs
  ai = new mdtAnalogIo;
  ai->setAddress(0);
  ai->setLabelShort("AI1");
  ai->setUnit("[V]");
  ai->setDetails("Module type: KL3001");
  ai->setRange(-10.0, 10.0, 13, 3, true);
  ios.addAnalogInput(ai);

  // Analog outputs
  ao = new mdtAnalogIo;
  ao->setAddress(0);
  ao->setLabelShort("AO1");
  ao->setUnit("[V]");
  ao->setDetails("Module type: KL4001");
  QVERIFY(ao->setRange(0.0, 10.0, 12, 3, false));
  QVERIFY(ao->setEncodeBitSettings(15, 0));
  ios.addAnalogOutput(ao);

  // Digital inputs
  di = new mdtDigitalIo;
  di->setAddress(0);
  di->setLabelShort("DI1");
  di->setDetails("Module type: KL1002");
  ios.addDigitalInput(di);
  di = new mdtDigitalIo;
  di->setAddress(1);
  di->setLabelShort("DI2");
  di->setDetails("Module type: KL1002");
  ios.addDigitalInput(di);

  // Digital outputs
  dout = new mdtDigitalIo;
  dout->setAddress(0);
  dout->setLabelShort("DO1");
  dout->setDetails("Module type: KL2012");
  ios.addDigitalOutput(dout);
  dout = new mdtDigitalIo;
  dout->setAddress(1);
  dout->setLabelShort("DO2");
  dout->setDetails("Module type: KL2012");
  ios.addDigitalOutput(dout);

  // Setup I/O's widget
  iosw = new mdtDeviceIosWidget;
  iosw->setDeviceIos(&ios);
  ///iosw.show();

  // Setup device
  d.setIos(&ios, true);
  ///d.setAnalogOutputAddressOffset(0x0800);
  ///d.setDigitalOutputAddressOffset(0);
  dw.setIosWidget(iosw);
  dw.show();

  /*
   * Tests
   */

  QVERIFY(d.getAnalogOutputs(500) >= 0);
  ///d.start(100);
  ///while(iosw.isVisible()){
  ///while(dw.isVisible()){
    QTest::qWait(1000);
  ///}
}

void mdtDeviceTest::scpiTest()
{
  mdtDeviceScpi d;
  QList<mdtPortInfo*> portInfoList;
  mdtPortInfo *portInfo;
  mdtDeviceInfo *devInfo;
  mdtFrameCodecScpi codec;
  QByteArray data;

  // Try to find a device and connect if ok
  portInfoList = d.portManager()->scan();
  if(portInfoList.size() < 1){
    QSKIP("No port found with SCPI device found, or other error", SkipAll);
  }
  portInfo = portInfoList.at(0);
  if(portInfo->deviceInfoList().size() < 1){
    QSKIP("No SCPI device found, or other error", SkipAll);
  }
  devInfo = portInfo->deviceInfoList().at(0);
  if(d.connectToDevice(*devInfo) != mdtAbstractPort::NoError){
    QSKIP("No SCPI device attached, or other error", SkipAll);
  }

  // Check commands
  QVERIFY(d.sendCommand("*RST\n") >= 0);
  QVERIFY(d.waitOperationComplete(5000, 100));
  QVERIFY(d.sendCommand("*CLS\n") >= 0);
  // Check query
  QVERIFY(!d.sendQuery("*IDN?\n").isEmpty());
}

void mdtDeviceTest::U3606ATest()
{
  mdtDeviceU3606A d;
  mdtDeviceInfo devInfo;
  mdtAnalogIo *ai;
  mdtDeviceIos ios;
  mdtDeviceIosWidget *iosw;
  mdtDeviceWindow dw;

  // Try to find a device and connect if ok
  if(d.connectToDevice(devInfo) != mdtAbstractPort::NoError){
    QSKIP("No Agilent U3606A attached, or other error", SkipAll);
  }

  /*
   * Setup I/O's
   */

  // Analog inputs
  ai = new mdtAnalogIo;
  ai->setAddress(0);
  ai->setLabelShort("Voltage");
  ai->setUnit("[V]");
  //ai->setDetails("Module type: KL3001");
  ai->setRange(0.0, 1000.0, 32);
  ios.addAnalogInput(ai);

  /*
   * Setup devie
   */

  // Setup I/O's widget
  iosw = new mdtDeviceIosWidget;
  iosw->setDeviceIos(&ios);

  // Setup device
  d.setIos(&ios, true);
  dw.setDevice(&d);
  dw.setIosWidget(iosw);
  ///dw.statusWidget()->setStateBusyText("Query running ...");
  ///dw.statusWidget()->setStateBusyColor(mdtLed::Green);
  QVERIFY(d.portManager()->writeThread() != 0);
  QVERIFY(d.portManager()->readThread() != 0);
  dw.statusWidget()->enableTxRxLeds(d.portManager()->writeThread(), d.portManager()->readThread());
  dw.show();

  ///qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");

  // Check generic command
  QVERIFY(d.sendCommand("*CLS\n") >= 0);
  QVERIFY(d.sendCommand("*RST\n") >= 0);
  
  // Enable bits in Status Byte Register
  QVERIFY(d.sendCommand("*SRE 255\n") >= 0);
  ///qDebug() << "*SRE?: " << d.sendQuery("*SRE?\n");
  
  // Enable bits in Standard Events Register
  QVERIFY(d.sendCommand("*ESE 255\n") >= 0);
  ///qDebug() << "*ESE?: " << d.sendQuery("*ESE?\n");
  
  // Enable bits in Standard Operation Regsiter
  QVERIFY(d.sendCommand(":STAT:OPER:ENAB 65535\n") >= 0);
  ///qDebug() << "STATus:OPERation:ENABle?: " << d.sendQuery("STATus:OPERation:ENABle?\n");

  qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");

  ///qDebug() << "*STB?: " << d.sendQuery("*STB?\n");
  
  ///qDebug() << "*IDN?: " << d.sendQuery("*IDN?\n");
  
  ///QTest::qWait(3000);

  
  ///d.sendQuery("*OPC?\n");
  ///QTest::qWait(2000);
  ///qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  // Check generic query
  QVERIFY(d.sendQuery("*IDN?\n").left(27) == "Agilent Technologies,U3606A");
  
  ///qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");

  // Get value
  ///QVERIFY(d.getAnalogInputValue(0, 32000).isValid());
  QVERIFY(d.getAnalogInputValue(0, true, true).isValid());
  qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  qDebug() << "*** OPC: " << d.sendQuery("*OPC?\n");
  qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  ///QVERIFY(d.getAnalogInputValue(0, 32000).type() == QVariant::Double);
  QVERIFY(d.getAnalogInputValue(0, true, true).isValid());
  ///qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  d.checkDeviceError();
  
  d.start(500);
  QTest::qWait(5000);
  
  while(dw.isVisible()){
    QTest::qWait(500);
  }
  
}

void mdtDeviceTest::DSO1000ATest()
{
  mdtDeviceDSO1000A d;
  mdtDeviceInfo devInfo;
  ///mdtAnalogIo *ai;
  ///mdtDeviceIos ios;
  ///mdtDeviceIosWidget *iosw;
  mdtDeviceWindow dw;
  mdtFrameCodecScpi codec;
  QByteArray data;

  // Try to find a device and connect if ok
  if(d.connectToDevice(devInfo) != mdtAbstractPort::NoError){
    QSKIP("No Agilent DSO1000A attached, or other error", SkipAll);
  }

  // Check generic command
  QVERIFY(d.sendCommand("*CLS\n") >= 0);
  QVERIFY(d.sendCommand("*RST\n") >= 0);
  QVERIFY(d.sendQuery("*IDN?\n").left(26) == "Agilent Technologies,DSO10");
  qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");

  QVERIFY(d.sendCommand(":CHANnel1:PROBe 20X\n") >= 0);
  QVERIFY(d.sendCommand(":ACQuire:TYPE PEAKdetect\n") >= 0);
  ///QVERIFY(d.sendCommand(":ACQuire:AVERages 8\n") >= 0);
  QVERIFY(d.sendCommand(":TRIGger:MODE EDGE\n") >= 0);
  QVERIFY(d.sendCommand(":TRIGger:EDGE:SOURce CHANnel1\n") >= 0);
  QVERIFY(d.sendCommand(":TRIGger:EDGE:SWEep AUTO\n") >= 0);
  QVERIFY(d.sendCommand(":TIMebase:MAIN:SCALe 50e-3\n") >= 0);
  QVERIFY(d.sendCommand(":START\n") >= 0);
  ///d.portManager()->wait(10000);
  QTest::qWait(10000);
  QVERIFY(d.sendCommand(":STOP\n") >= 0);
  QVERIFY(d.sendCommand(":WAVeform:SOURce CHANnel1\n") >= 0);
  QVERIFY(d.sendCommand(":WAVeform:FORMat BYTE\n") >= 0);
  
  qDebug() << "TEST: getting data ...";
  
  QVERIFY(d.waitOperationComplete(5000, 1000));
  ///d.checkDeviceError();  /// \todo re-enable once correted
  
  
  double y_inc, y_val, y_ref, y_origin;
  
  qDebug() << "TEST: getting preamble ...";
  
  data = d.sendQuery(":WAVeform:PREamble?\n");
  qDebug() << codec.decodeValues(data, ",");
  qDebug() << codec.values();
  QVERIFY(codec.values().size() > 9);
  y_inc = codec.values().at(7).toDouble();
  qDebug() << "Y inc: " << y_inc;
  y_origin = codec.values().at(8).toDouble();
  qDebug() << "Y O: " << y_origin;
  y_ref = codec.values().at(9).toDouble();
  qDebug() << "Y ref: " << y_ref;
  
  
  data = d.sendQuery(":WAVeform:DATA?\n");
  qDebug() << "Data len: " << data.size();
  
  QVERIFY(codec.decodeIEEEblock(data, mdtFrameCodecScpi::BYTE));
  for(int i=0; i<codec.values().size(); i++){
    ///qDebug() << "data[" << i << "]: " << codec.values().at(i) << " , flt: " << codec.values().at(i).toDouble()*y_inc;
    y_val = codec.values().at(i).toDouble();
    ///qDebug() << "data[" << i << "]: " << ((y_ref - y_val) * y_inc) - y_origin;
  }
  ///qDebug() << "Data: " << codec.values();

}


int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtDeviceTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

