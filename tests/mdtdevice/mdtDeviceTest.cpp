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
#include "mdtDeviceTest.h"
#include "mdtApplication.h"
#include "mdtDeviceIos.h"
#include "mdtDeviceIosWidget.h"
#include "mdtDeviceModbus.h"

/**
#ifdef Q_OS_UNIX
 #include "linux/mdtDeviceU3606A.h"
#endif
*/

#include <QString>
#include <QVariant>
#include <QList>

#include <QDebug>

void mdtDeviceTest::deviceBaseTest()
{
  mdtDevice dev;

  // Initial states
  QVERIFY(dev.state() == mdtDevice::Disconnected);

  dev.start(100);
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

  // Add analog inputs
  ai = new mdtAnalogIo;
  ai->setAddress(12);
  ios.addAnalogInput(ai);

  // Add analog outputs
  ao = new mdtAnalogIo;
  ao->setAddress(15);
  ios.addAnalogOutput(ao);

  // Add digital inputs
  di = new mdtDigitalIo;
  di->setAddress(17);
  ios.addDigitalInput(di);

  // Add digital inputs
  dout = new mdtDigitalIo;
  dout->setAddress(20);
  ios.addDigitalOutput(dout);

  // Check analog inputs
  QVERIFY(ios.analogInputAt(11) == 0);
  QVERIFY(ios.analogInputAt(12) != 0);
  QVERIFY(ios.analogInputAt(12)->address() == 12);
  aIoList = ios.analogInputs();
  for(int i=0; i<aIoList.size(); i++){
    qDebug() << "Analog input, adr: " << aIoList.at(i)->address();
  }
  // Check analog outputs
  QVERIFY(ios.analogOutputAt(13) == 0);
  QVERIFY(ios.analogOutputAt(15) != 0);
  QVERIFY(ios.analogOutputAt(15)->address() == 15);
  aIoList = ios.analogOutputs();
  for(int i=0; i<aIoList.size(); i++){
    qDebug() << "Analog output, adr: " << aIoList.at(i)->address();
  }
  // Check digital inputs
  QVERIFY(ios.digitalInputAt(11) == 0);
  QVERIFY(ios.digitalInputAt(17) != 0);
  QVERIFY(ios.digitalInputAt(17)->address() == 17);
  dIoList = ios.digitalInputs();
  for(int i=0; i<dIoList.size(); i++){
    qDebug() << "Digital input, adr: " << dIoList.at(i)->address();
  }
  // Check digital outputs
  QVERIFY(ios.digitalOutputAt(19) == 0);
  QVERIFY(ios.digitalOutputAt(20) != 0);
  QVERIFY(ios.digitalOutputAt(20)->address() == 20);
  dIoList = ios.digitalOutputs();
  for(int i=0; i<dIoList.size(); i++){
    qDebug() << "Digital output, adr: " << dIoList.at(i)->address();
  }
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

  while(w.isVisible()){
    QTest::qWait(1000);
  }
  // Second setup (must clear existing widget and redo layout)
  w.setDeviceIos(&ios);
  w.show();
  while(w.isVisible()){
    QTest::qWait(1000);
  }

}

void mdtDeviceTest::deviceModbusTest()
{
  mdtDeviceModbus d;
  mdtDeviceIos ios;
  mdtDeviceIosWidget iosw;
  mdtAnalogIo *ai;
  mdtAnalogIo *ao;

  /*
   * Setup I/O's
   */

  // Analog inputs
  ai = new mdtAnalogIo;
  ai->setAddress(0);
  ai->setLabelShort("AI1");
  ai->setUnit("[V]");
  ai->setDetails("Module type: 750-457");
  ai->setRange(-10.0, 10.0, 13, 3, true);
  ios.addAnalogInput(ai);
  ai = new mdtAnalogIo;
  ai->setAddress(1);
  ai->setLabelShort("AI2");
  ai->setUnit("[V]");
  ai->setDetails("Module type: 750-457");
  ai->setRange(-10.0, 10.0, 13, 3, true);
  ios.addAnalogInput(ai);
  ai = new mdtAnalogIo;
  ai->setAddress(2);
  ai->setLabelShort("AI3");
  ai->setDetails("Module type: 750-457");
  ai->setRange(-10.0, 10.0, 13, 3, true);
  ios.addAnalogInput(ai);
  ai = new mdtAnalogIo;
  ai->setAddress(3);
  ai->setLabelShort("AI4");
  ai->setDetails("Module type: 750-457");
  ai->setRange(-10.0, 10.0, 13, 3, true);
  ios.addAnalogInput(ai);

  // Analog outputs
  ao = new mdtAnalogIo;
  ao->setAddress(0);
  ao->setLabelShort("AO1");
  ao->setUnit("[V]");
  ao->setDetails("Module type: 750-550");
  QVERIFY(ao->setRange(0.0, 10.0, 12, 3, false));
  QVERIFY(ao->setEncodeBitSettings(15, 0));
  ios.addAnalogOutput(ao);

  // Setup I/O's widget
  iosw.setDeviceIos(&ios);
  iosw.show();

  // Setup device
  d.setIos(&ios, true);
  d.setAnalogOutputAddressOffset(0x0200);

  // First setup - Analog inputs
  /**
  d.setupAnalogInputs(11);
  QVERIFY(!d.getAnalogInputValue(-1, false).isValid());
  QVERIFY(d.getAnalogInputValue(0, false).isValid());
  QVERIFY(d.getAnalogInputValue(0, false) == 0);
  QVERIFY(d.getAnalogInputValue(10, false) == 0);
  QVERIFY(!d.getAnalogInputValue(11, false).isValid());
  */

  // Second setup - Analog inputs
  /**
  d.setupAnalogInputs(5);
  QVERIFY(!d.getAnalogInputValue(-1, false).isValid());
  QVERIFY(d.getAnalogInputValue(0, false).isValid());
  QVERIFY(d.getAnalogInputValue(0, false) == 0);
  QVERIFY(d.getAnalogInputValue(4, false) == 0);
  QVERIFY(!d.getAnalogInputValue(5, false).isValid());
  */

  // First setup - Analog outputs
  /**
  d.setupAnalogOutputs(11);
  QVERIFY(!d.getAnalogOutputValue(-1, false).isValid());
  QVERIFY(d.getAnalogOutputValue(0, false).isValid());
  QVERIFY(d.getAnalogOutputValue(0, false) == 0);
  QVERIFY(d.getAnalogOutputValue(10, false) == 0);
  QVERIFY(!d.getAnalogOutputValue(11, false).isValid());
  // Update some values
  QVERIFY(!d.setAnalogOutput(-1, 10, false));
  QVERIFY(d.setAnalogOutput(0, 10, false));
  QVERIFY(d.getAnalogOutputValue(0, false) == 10);
  QVERIFY(d.setAnalogOutput(9, 8, false));
  QVERIFY(d.getAnalogOutputValue(9, false) == 8);

  // Second setup - Analog outputs
  d.setupAnalogOutputs(5);
  QVERIFY(!d.getAnalogOutputValue(-1, false).isValid());
  QVERIFY(d.getAnalogOutputValue(0, false).isValid());
  QVERIFY(d.getAnalogOutputValue(0, false) == 0);
  QVERIFY(d.getAnalogOutputValue(4, false) == 0);
  QVERIFY(!d.getAnalogOutputValue(5, false).isValid());
  */

  // First setup - Digital inputs
  /**
  d.setupDigitalInputs(11);
  QVERIFY(!d.getDigitalInputState(-1, false).isValid());
  QVERIFY(d.getDigitalInputState(0, false).isValid());
  QVERIFY(d.getDigitalInputState(0, false) == false);
  QVERIFY(d.getDigitalInputState(10, false) == false);
  QVERIFY(!d.getDigitalInputState(11, false).isValid());

  // Second setup - Digital inputs
  d.setupDigitalInputs(5);
  QVERIFY(!d.getDigitalInputState(-1, false).isValid());
  QVERIFY(d.getDigitalInputState(0, false).isValid());
  QVERIFY(d.getDigitalInputState(0, false) == false);
  QVERIFY(d.getDigitalInputState(4, false) == false);
  QVERIFY(!d.getDigitalInputState(5, false).isValid());
  */

  // First setup - Digital outputs
  /**
  d.setupDigitalOutputs(11);
  QVERIFY(!d.getDigitalOutputState(-1, false).isValid());
  QVERIFY(d.getDigitalOutputState(0, false).isValid());
  QVERIFY(d.getDigitalOutputState(0, false) == false);
  QVERIFY(d.getDigitalOutputState(10, false) == false);
  QVERIFY(!d.getDigitalOutputState(11, false).isValid());
  // Update some values
  QVERIFY(!d.setDigitalOutput(-1, true, false));
  QVERIFY(d.setDigitalOutput(0, true, false));
  QVERIFY(d.getDigitalOutputState(0, false) == true);
  QVERIFY(d.setDigitalOutput(9, true, false));
  QVERIFY(d.getDigitalOutputState(9, false) == true);
  */

  // Second setup - Digital outputs
  /**
  d.setupDigitalOutputs(5);
  QVERIFY(!d.getDigitalOutputState(-1, false).isValid());
  QVERIFY(d.getDigitalOutputState(0, false).isValid());
  QVERIFY(d.getDigitalOutputState(0, false) == false);
  QVERIFY(d.getDigitalOutputState(4, false) == false);
  QVERIFY(!d.getDigitalOutputState(5, false).isValid());
  */

  /// Essais
  ///QVERIFY(d.setAnalogOutput(0, 0x00FF, false));
  ///QVERIFY(d.setAnalogOutput(1, 0x00FF, false));
  ///QVERIFY(d.writeAnalogOutputs());
  ///QVERIFY(d.setDigitalOutput(0, true, false));
  ///QVERIFY(d.setDigitalOutput(1, true, false));
  ///QVERIFY(d.setDigitalOutput(3, true, false));
  ///QVERIFY(d.writeDigitalOutputs());
  ///QVERIFY(d.readDigitalInputs());
  ///QVERIFY(d.readAnalogOutputs());
  ///QVERIFY(d.readDigitalOutputs());

  QVERIFY(d.readAnalogOutputs() >= 0);
  d.start(100);
  while(iosw.isVisible()){
    ///QVERIFY(d.readAnalogInputs() >= 0);
    ///QVERIFY(d.readAnalogOutputs() >= 0);
    QTest::qWait(500);
  }
}

/**
void mdtDeviceTest::deviceU3606Atest()
{
  mdtDeviceU3606A d;
  QStringList ports;
  
  qDebug() << "* A Agilent U3606A must be attached at USB port, else test will fail *";

  // Check that scan and open methods works
  ports = d.scan();
  QVERIFY(ports.size() > 0);
  QVERIFY(d.selectPort(ports, 0));

  d.essais();
}
*/

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtDeviceTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

