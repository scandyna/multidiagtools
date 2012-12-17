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
#include "mdtPortManagerTest.h"
#include "mdtPort.h"
#include "mdtPortManager.h"
#include "mdtPortConfig.h"
#include "mdtFrame.h"
#include "mdtFrameCodecModbus.h"
#include "mdtFrameUsbControl.h"
#include "mdtPortReadThread.h"
#include "mdtPortWriteThread.h"
#include "mdtPortInfo.h"
#include <QTemporaryFile>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QHashIterator>
#include "mdtApplication.h"
#include "mdtUsbPortManager.h"
#include "mdtUsbtmcPortManager.h"
#include "mdtModbusTcpPortManager.h"

#include <QDebug>

void mdtPortManagerTest::portTest()
{
  mdtPortManager m, m2;
  QTemporaryFile file;
  mdtPortConfig cfg, cfg2;
  mdtPort *port, *port2;

  // Create a temporary file
  QVERIFY(file.open());
  file.close();

  // Port setup
  ///cfg = new mdtPortConfig;
  cfg.setFrameType(mdtFrame::FT_ASCII);
  port = new mdtPort;
  port->setConfig(&cfg);

  // Init port manager
  m.setPort(port);
  m.setEnqueueReadenFrames(true);
  m.addThread(new mdtPortWriteThread);
  m.addThread(new mdtPortReadThread);
  m.setPortName(file.fileName());
  QVERIFY(m.openPort());

  // start threads
  QVERIFY(m.start());
  QVERIFY(m.isRunning());

  // Check the wait function without available data
  QVERIFY(!m.waitReadenFrame());

  // Send some data
  QVERIFY(m.writeData("Test\n") == 0);

  // We must re-open the file, else nothing will be readen
  QTest::qWait(100);
  m.closePort();
  QVERIFY(m.openPort());
  QVERIFY(m.start());
  // Get readen data
  QVERIFY(m.waitReadenFrame());
  // Check that multiple calls of readenFrames() does not alter frames queue
  QVERIFY(m.readenFrames().size() == 1);
  QVERIFY(m.readenFrames().size() == 1);
  QVERIFY(m.readenFrames().at(0) == "Test");
  QVERIFY(m.readenFrames().size() == 1);
  m.clearReadenFrames();
  QVERIFY(m.readenFrames().size() == 0);

  // Setup a second port manager
  // Port setup
  port2 = new mdtPort;
  port2->setConfig(&cfg2);
  m2.setPort(port2);

  // Setup same port than first manager, open must fail
  m2.setPortName(file.fileName());
  QVERIFY(!m2.openPort());

  // Check that we can close port before detach without crash
  m.closePort();

  // Cleanup
  m2.detachPort(true, true);
  m.detachPort(true, true);
}

void mdtPortManagerTest::usbPortTest()
{
  mdtUsbPortManager m;
  QList<mdtPortInfo*> portInfoList;

  qDebug() << "* A USB device must be attached, else test will fail *";

  // Verify that scan() function works ..
  portInfoList = m.scan(0xFE, -1, -1);
  if(portInfoList.size() < 1){
    QSKIP("No USBT device found, or other error", SkipAll);
  }

  for(int i=0; i<portInfoList.size(); i++){
    qDebug() << "Device: " << portInfoList.at(i)->displayText();
    qDebug() << "Port: " << portInfoList.at(i)->portName();
  }
  
  // Init port manager
  ///m.setEnqueueReadenFrames(true);
  m.setPortInfo(*portInfoList.at(0));
  QVERIFY(m.openPort());
  // start threads
  ///QTest::qWait(2000);
  QVERIFY(m.start());

  
  ///QTest::qWait(2000);
  // We not need the scan result anymore, free memory
  qDeleteAll(portInfoList);
  portInfoList.clear();

  /// \note Sandbox !!!
  mdtFrameUsbControl f;
  
  
  f.setDirectionDeviceToHost(true);
  f.setRequestType(mdtFrameUsbControl::RT_CLASS);
  f.setRequestRecipient(mdtFrameUsbControl::RR_INTERFACE);
  ///f->setbRequest(7); // GET_CAPABILITIES
  f.setbRequest(128); // READ_STATUS_BYTE
  f.setwValue(5);
  f.setwIndex(0);  /// \todo interface nunber hardcoded, BAD
  f.setwLength(0x3);
  f.encode();
  m.sendControlRequest(f);
  /// End Sandbox !

  QTest::qWait(2000);

}

void mdtPortManagerTest::usbTmcPortTest()
{
  mdtUsbtmcPortManager m;
  QList<mdtPortInfo*> portInfoList;

  qDebug() << "* A USBTMC compatible device must be attached, else test will fail *";

  // Verify that scan() function works ..
  portInfoList = m.scan();
  if(portInfoList.size() < 1){
    QSKIP("No USBTMC device found, or other error", SkipAll);
  }

  // Init port manager
  m.setEnqueueReadenFrames(true);
  m.setPortName(portInfoList.at(0)->portName());
  QVERIFY(m.openPort());

  // We not need the scan result anymore, free memory
  qDeleteAll(portInfoList);
  portInfoList.clear();

  // Init port manager
  ///m.setPortName(ports.at(0));
  ///port.setPortName("0x0957:0x4d18");
  ///m.setPortName("0x0957:0x0588");
  ///QVERIFY(m.openPort());

  // start threads
  QVERIFY(m.start());
  qDebug() << "TEST, threads running...";

  // Control request
  QVERIFY(m.sendReadStatusByteRequest() >= 2);
  // Query without answer
  ///QVERIFY(m.writeData("*CLS\n", false));
  qDebug() << "TEST, sending request ...";
  QVERIFY(m.writeData("*IDN?\n") > 0);
  qDebug() << "TEST, sending request DONE";
  ///QTest::qWait(500);
  qDebug() << "TEST, request read ...";
  ///m.readData();
  QVERIFY(m.waitOnWriteReady(1000));
  QVERIFY(m.sendReadRequest() > 0);
  QVERIFY(m.waitReadenFrame());
  qDebug() << "TEST, request read DONE";
  QVERIFY(m.readenFrames().size() > 0);
  qDebug() << "Data n: " << m.readenFrames().size() << ", Data[0]: " << m.readenFrames().at(0);
  m.clearReadenFrames();

  ///QTest::qWait(500);
  
  
  qDebug() << "TEST, end";
  
  // Try to query device
  /**
  for(int i=0; i<5; i++){
    // Send a command
    QVERIFY(m.writeData("*IDN?\n", true));
    // Wait and read answer
    QVERIFY(m.lastReadenFrame().size() <= 0);
    QVERIFY(m.waitReadenFrame(10000));
    QVERIFY(m.lastReadenFrame().size() > 0);
  }
  */
}

/// \todo No real check possible withous MODBUS Server available on loopback (127.0.0)
void mdtPortManagerTest::modbusTcpPortTest()
{
  mdtModbusTcpPortManager m;
  QList<mdtPortInfo*> portInfoList;
  QStringList hosts;
  mdtFrameCodecModbus codec;
  QByteArray pdu;
  QHash<quint16, QByteArray> pdus;
  int tId1, tId2, tId3;

  qDebug() << "* A MODBUS/TCP compatible device must be attached, else test will fail *";

  // Verify that scan() function works ..
  hosts << "127.0.0.1:502";
  hosts << "192.168.1.100:502";
  hosts << "192.168.1.101:502";
  hosts << "192.168.1.102:502";
  hosts << "192.168.1.103:502";
  portInfoList = m.scan(hosts);
  if(portInfoList.size() < 1){
    QSKIP("No MODBUS/TCP device found, or other error", SkipAll);
  }

  // Init port manager
  //m.setPortName(portInfoList.at(0)->portName());
  m.setEnqueueReadenFrames(true);
  m.setPortInfo(*portInfoList.at(0));
  QVERIFY(m.openPort());

  // We not need the scan result anymore, free memory
  qDeleteAll(portInfoList);
  portInfoList.clear();

  // start threads
  QVERIFY(m.start());

  // "Check" direct PDU write
  pdu = codec.encodeReadCoils(0, 3);
  tId1 = m.writeData(pdu);
  QVERIFY(tId1 >= 0);
  tId2 = m.writeData(pdu);
  QVERIFY(tId2 >= 0);
  tId3 = m.writeData(pdu);
  QVERIFY(tId3 >= 0);
  QTest::qWait(500);
  QVERIFY(!m.waitOnFrame(tId1, 500).isEmpty());
  QVERIFY(!m.waitOnFrame(tId2, 500).isEmpty());
  QVERIFY(!m.waitOnFrame(tId3, 500).isEmpty());
  m.clearReadenFrames();
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtPortManagerTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
