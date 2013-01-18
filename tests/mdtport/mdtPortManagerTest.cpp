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
#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"
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
#include "mdtPortTransaction.h"

#include <QDebug>

void mdtPortManagerTest::transactionIdTest()
{
  mdtPortTransaction t;
  mdtAnalogIo *ai1, *ai2;
  mdtDigitalIo *di1, *di2;

  ai1 = new mdtAnalogIo;
  ai2 = new mdtAnalogIo;
  di1 = new mdtDigitalIo;
  di2 = new mdtDigitalIo;

  // Check initial values
  QCOMPARE(t.id(), 0);
  QCOMPARE(t.type(), 0);
  QVERIFY(t.analogIo() == 0);
  QVERIFY(t.digitalIo() == 0);
  QCOMPARE(t.forMultipleIos(), false);
  QCOMPARE(t.isInput(), false);
  QCOMPARE(t.isOutput(), false);
  QVERIFY(t.data().isEmpty());
  QVERIFY(!t.isQueryReplyMode());

  // Input/Output flag
  t.setIsInput(true);
  QCOMPARE(t.isInput(), true);
  QCOMPARE(t.isOutput(), false);
  t.setIsInput(false);
  QCOMPARE(t.isInput(), false);
  QCOMPARE(t.isOutput(), true);

  // Diverse flags
  t.setId(1234);
  QCOMPARE(t.id(), 1234);
  t.setType(556);
  QCOMPARE(t.type(), 556);
  t.setData("Test");
  QCOMPARE(t.data(), QByteArray("Test"));

  // I/Os
  t.setIo(ai1, true);
  QVERIFY(t.digitalIo() == 0);
  QVERIFY(t.analogIo() == ai1);
  QCOMPARE(t.isInput(), true);

  // Clear
  t.clear();
  QCOMPARE(t.id(), 0);
  QCOMPARE(t.type(), 0);
  QVERIFY(t.analogIo() == 0);
  QVERIFY(t.digitalIo() == 0);
  QCOMPARE(t.forMultipleIos(), false);
  QCOMPARE(t.isInput(), false);
  QCOMPARE(t.isOutput(), false);
  QVERIFY(t.data().isEmpty());
  QVERIFY(!t.isQueryReplyMode());

  // Copy test
  // Note: I/O pointers reference the same objects, this is wat we need.
  t.setIo(ai1, true);
  t.setId(1);
  t.setData("t");
  QVERIFY(t.analogIo() == ai1);
  QCOMPARE(t.id(), 1);
  QCOMPARE(t.data(), QByteArray("t"));
  mdtPortTransaction t2(t);
  QVERIFY(t2.analogIo() == ai1);
  QCOMPARE(t2.id(), 1);
  QCOMPARE(t2.data(), QByteArray("t"));
  t.clear();
  QCOMPARE(t.id(), 0);
  QVERIFY(t.analogIo() == 0);
  QVERIFY(t.data().isEmpty());
  QVERIFY(t2.analogIo() == ai1);
  QCOMPARE(t2.id(), 1);
  QCOMPARE(t2.data(), QByteArray("t"));

  // Cleanup
  delete ai1;
  delete ai2;
  delete di1;
  delete di2;
}

void mdtPortManagerTest::portTest()
{
  mdtPortManager m, m2;
  QTemporaryFile file;
  mdtPortConfig cfg, cfg2;
  mdtPort *port, *port2;
  QList<QByteArray> frames;

  // Create a temporary file
  QVERIFY(file.open());
  file.close();

  // Port setup
  cfg.setFrameType(mdtFrame::FT_ASCII);
  port = new mdtPort;
  port->setConfig(&cfg);

  // Init port manager
  m.setPort(port);
  m.setTransactionsDisabled(true);
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
  QVERIFY(m.writeData("ABCD\nEFGH\n") == 0);

  // We must re-open the file, else nothing will be readen
  QTest::qWait(100);
  m.closePort();
  QVERIFY(m.openPort());
  QVERIFY(m.start());
  // Get readen data
  QVERIFY(m.waitReadenFrame());
  // Try to get a frame by ID (will not work, but must not crash"
  m.readenFrame(0);
  // Check readen data
  frames = m.readenFrames();
  QVERIFY(frames.size() == 2);
  QCOMPARE(frames.at(0), QByteArray("ABCD"));
  QCOMPARE(frames.at(1), QByteArray("EFGH"));

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
  m.setPortInfo(*portInfoList.at(0));
  QVERIFY(m.openPort());
  // start threads
  QVERIFY(m.start());

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
  QList<QByteArray> frames;
  int bTag;

  qDebug() << "* A USBTMC compatible device must be attached, else test will fail *";

  // Verify that scan() function works ..
  portInfoList = m.scan();
  if(portInfoList.size() < 1){
    QSKIP("No USBTMC device found, or other error", SkipAll);
  }

  // Init port manager
  m.setPortName(portInfoList.at(0)->portName());
  m.config().setWriteQueueSize(1);
  QVERIFY(m.openPort());

  // We not need the scan result anymore, free memory
  qDeleteAll(portInfoList);
  portInfoList.clear();

  // start threads
  QVERIFY(m.start());

  // Control request
  ///QVERIFY(m.sendReadStatusByteRequest() >= 0);
  // Query without answer
  QVERIFY(m.waitOnWriteReady(1000));
  QVERIFY(m.writeData("*CLS\n"));
  QVERIFY(m.waitOnWriteReady(1000));
  QVERIFY(m.writeData("*RST\n"));
  // Query with answer
  QVERIFY(m.waitOnWriteReady(1000));
  QVERIFY(m.writeData("*IDN?\n") >= 0);
  QVERIFY(m.waitOnWriteReady(1000));
  bTag = m.sendReadRequest(true);
  QVERIFY(bTag > 0);
  QVERIFY(m.waitOnFrame(bTag, 20000));
  frames = m.readenFrames();
  QVERIFY(frames.size() > 0);

  // Try to query device
  QVERIFY(m.sendCommand("*CLS\n") > 0);
  QVERIFY(!m.sendQuery("*IDN?\n").isEmpty());
  
  // Abort test
  qDebug() << "abortBulkIn() returned: " << m.abortBulkIn(5);
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
  m.setPortInfo(*portInfoList.at(0));
  QVERIFY(m.openPort());

  // We not need the scan result anymore, free memory
  qDeleteAll(portInfoList);
  portInfoList.clear();

  // start threads
  QVERIFY(m.start());

  // "Check" direct PDU write in query/reply mode
  pdu = codec.encodeReadCoils(0, 3);
  tId1 = m.writeData(pdu, true);
  QVERIFY(tId1 >= 0);
  tId2 = m.writeData(pdu, true);
  QVERIFY(tId2 >= 0);
  tId3 = m.writeData(pdu, true);
  QVERIFY(tId3 >= 0);
  QVERIFY(m.waitOnFrame(tId1, 500));
  QVERIFY(!m.readenFrame(tId1).isEmpty());
  QVERIFY(m.waitOnFrame(tId2, 500));
  QVERIFY(m.waitOnFrame(tId3, 500));
  QCOMPARE(m.readenFrames().size(), 2);

  // If query/reply mode is diseabled, waitOnFrame() will timeout
  pdu = codec.encodeReadCoils(0, 3);
  tId1 = m.writeData(pdu, false);
  QVERIFY(!m.waitOnFrame(tId1, 500));

  // All frames must be consumed
  QCOMPARE(m.readenFrames().size(), 0);
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
