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
#include "mdtDeviceScpi.h"
#include "mdtDeviceU3606A.h"
#include "mdtDeviceWindow.h"
#include "mdtPortInfo.h"

#include "mdtFrameCodecScpi.h"

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

void mdtDeviceTest::modbusWagoTest()
{
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
  ao = new mdtAnalogIo;
  ao->setAddress(1);
  ao->setLabelShort("AO2");
  ao->setUnit("[V]");
  ao->setDetails("Module type: 750-550");
  QVERIFY(ao->setRange(0.0, 10.0, 12, 3, false));
  QVERIFY(ao->setEncodeBitSettings(15, 0));
  ios.addAnalogOutput(ao);

  // Digital inputs
  di = new mdtDigitalIo;
  di->setAddress(0);
  di->setLabelShort("DI1");
  di->setDetails("Module type: 750-428");
  ios.addDigitalInput(di);

  // Digital outputs
  dout = new mdtDigitalIo;
  dout->setAddress(0);
  dout->setLabelShort("DO1");
  dout->setDetails("Module type: 750-530");
  ios.addDigitalOutput(dout);
  dout = new mdtDigitalIo;
  dout->setAddress(1);
  dout->setLabelShort("DO2");
  dout->setDetails("Module type: 750-530");
  ios.addDigitalOutput(dout);
  dout = new mdtDigitalIo;
  dout->setAddress(2);
  dout->setLabelShort("DO3");
  dout->setDetails("Module type: 750-530");
  ios.addDigitalOutput(dout);
  dout = new mdtDigitalIo;
  dout->setAddress(3);
  dout->setLabelShort("DO4");
  dout->setDetails("Module type: 750-530");
  ios.addDigitalOutput(dout);
  dout = new mdtDigitalIo;
  dout->setAddress(4);
  dout->setLabelShort("DO5");
  dout->setDetails("Module type: 750-530");
  ios.addDigitalOutput(dout);
  dout = new mdtDigitalIo;
  dout->setAddress(5);
  dout->setLabelShort("DO6");
  dout->setDetails("Module type: 750-530");
  ios.addDigitalOutput(dout);
  dout = new mdtDigitalIo;
  dout->setAddress(6);
  dout->setLabelShort("DO7");
  dout->setDetails("Module type: 750-530");
  ios.addDigitalOutput(dout);
  dout = new mdtDigitalIo;
  dout->setAddress(7);
  dout->setLabelShort("DO8");
  dout->setDetails("Module type: 750-530");
  ios.addDigitalOutput(dout);
  dout = new mdtDigitalIo;
  dout->setAddress(8);
  dout->setLabelShort("DO9");
  dout->setDetails("Module type: 750-517");
  ios.addDigitalOutput(dout);
  dout = new mdtDigitalIo;
  dout->setAddress(9);
  dout->setLabelShort("DO10");
  dout->setDetails("Module type: 750-517");
  ios.addDigitalOutput(dout);

  // Setup I/O's widget
  iosw = new mdtDeviceIosWidget;
  iosw->setDeviceIos(&ios);

  // Setup device
  d.setIos(&ios, true);
  d.setAnalogOutputAddressOffset(0x0200);
  d.setDigitalOutputAddressOffset(0x0200);
  dw.setDevice(&d);
  dw.setIosWidget(iosw);
  dw.show();

  /*
   * Tests
   */

  // Analog inputs
  QVERIFY(d.getAnalogInputValue(0, 500, true).isValid());
  QVERIFY(!d.getAnalogInputValue(0, 0, true).isValid());
  QVERIFY(d.getAnalogInputValue(1, 500, true).isValid());
  QVERIFY(!d.getAnalogInputValue(1, 0, true).isValid());
  QVERIFY(d.getAnalogInputs(500) >= 0);

  // Analog outputs
  QVERIFY(d.setAnalogOutputValue(0, 2.5, 500) >= 0);
  QVERIFY(d.getAnalogOutputValue(0, 500, true).isValid());
  QVERIFY(d.getAnalogOutputs(500) >= 0);
  QVERIFY(d.setAnalogOutputValue(0, 2.5, 500) >= 0);
  QVERIFY(d.getAnalogOutputValue(0, 500, true).isValid());
  QVERIFY(qAbs(d.getAnalogOutputValue(0, 500, true).toDouble()-2.5) < (10.0/4050.0));
  QVERIFY(!d.getAnalogOutputValue(0, 0, true).isValid());
  // Grouped query
  QVERIFY(d.setAnalogOutputValue(0, 1.5, -1) == 0);
  QVERIFY(d.setAnalogOutputValue(1, 2.5, -1) == 0);
  QVERIFY(d.setAnalogOutputs(500) >= 0);
  QVERIFY(qAbs(d.getAnalogOutputValue(0, 500, true).toDouble()-1.5) < (10.0/4050.0));
  QVERIFY(qAbs(d.getAnalogOutputValue(1, 500, true).toDouble()-2.5) < (10.0/4050.0));

  // Digital inputs
  QVERIFY(!d.getDigitalInputState(0, 0).isValid());
  ///QVERIFY(d.getDigitalInputState(0, 500).isValid());
  ///QTest::qWait(500);
  qDebug() << "State: " << d.getDigitalInputState(0, 500);

  // Digital outputs
  QVERIFY(!d.getDigitalOutputState(0, 0).isValid());
  QVERIFY(d.getDigitalOutputState(0, 500).isValid());
  // Grouped query
  QVERIFY(d.setDigitalOutputState(0, true, -1) == 0);
  QVERIFY(d.setDigitalOutputState(8, true, -1) == 0);
  QVERIFY(d.setDigitalOutputs(500) >= 0);

  ///QVERIFY(d.getDigitalInputs(500) >= 0);
  QVERIFY(d.getAnalogOutputs(500) >= 0);
  QVERIFY(d.getDigitalOutputs(500) >= 0);
  d.start(100);
  ///while(dw.isVisible()){
    QTest::qWait(1000);
  ///}
}

void mdtDeviceTest::modbusBeckhoffTest()
{
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
  d.setAnalogOutputAddressOffset(0x0800);
  d.setDigitalOutputAddressOffset(0);
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

  // Check query
  QVERIFY(!d.sendQuery("*IDN?\n").isEmpty());

  QVERIFY(d.waitOperationComplete(5000, 100));

  QVERIFY(d.sendCommand(":CHANnel1:PROBe 20X\n") >= 0);
  QVERIFY(d.sendCommand(":ACQuire:TYPE PEAKdetect\n") >= 0);
  ///QVERIFY(d.sendCommand(":ACQuire:AVERages 8\n") >= 0);
  QVERIFY(d.sendCommand(":TRIGger:MODE EDGE\n") >= 0);
  QVERIFY(d.sendCommand(":TRIGger:EDGE:SOURce CHANnel1\n") >= 0);
  QVERIFY(d.sendCommand(":TRIGger:EDGE:SWEep AUTO\n") >= 0);
  QVERIFY(d.sendCommand(":TIMebase:MAIN:SCALe 50e-3\n") >= 0);
  QVERIFY(d.sendCommand(":START\n") >= 0);
  d.portManager()->wait(10000);
  QVERIFY(d.sendCommand(":STOP\n") >= 0);
  QVERIFY(d.sendCommand(":WAVeform:SOURce CHANnel1\n") >= 0);
  QVERIFY(d.sendCommand(":WAVeform:FORMat BYTE\n") >= 0);
  QVERIFY(d.waitOperationComplete(5000, 1000));
  
  
  double y_inc, y_val, y_ref, y_origin;
  
  data = d.sendQuery(":WAVeform:PREamble?\n");
  qDebug() << codec.decodeValues(data, ",");
  qDebug() << codec.values();
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
    qDebug() << "data[" << i << "]: " << ((y_ref - y_val) * y_inc) - y_origin;
  }
  ///qDebug() << "Data: " << codec.values();
  
  

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
  QVERIFY(d.getAnalogInputValue(0, 32000).isValid());
  qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  qDebug() << "*** OPC: " << d.sendQuery("*OPC?\n");
  qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  QVERIFY(d.getAnalogInputValue(0, 32000).type() == QVariant::Double);
  qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  
  
  
  while(dw.isVisible()){
    QTest::qWait(500);
    qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  }
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

