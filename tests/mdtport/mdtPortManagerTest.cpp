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
#include <QTimer>

#include <QDebug>

/// Sandbox
#include <QNetworkInterface>
#include <QNetworkAddressEntry>

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
  QCOMPARE(t.ioCount(), 0);
  QCOMPARE(t.isInput(), false);
  QCOMPARE(t.isOutput(), false);
  QVERIFY(t.data().isEmpty());
  QVERIFY(!t.isQueryReplyMode());

  // Copy test AIO
  // Note: I/O pointers reference the same objects, this is wat we need.
  t.setIo(ai1, true);
  QCOMPARE(t.ioCount(), 1);
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
  // Copy test DIO
  // Note: I/O pointers reference the same objects, this is wat we need.
  t.setIo(di1, true);
  QCOMPARE(t.ioCount(), 1);
  t.setId(11);
  t.setData("tt");
  QVERIFY(t.digitalIo() == di1);
  QCOMPARE(t.id(), 11);
  QCOMPARE(t.data(), QByteArray("tt"));
  t2 = t;
  QVERIFY(t2.digitalIo() == di1);
  QCOMPARE(t2.id(), 11);
  QCOMPARE(t2.data(), QByteArray("tt"));
  t.clear();
  QCOMPARE(t.id(), 0);
  QVERIFY(t.digitalIo() == 0);
  QVERIFY(t.data().isEmpty());
  QVERIFY(t2.digitalIo() == di1);
  QCOMPARE(t2.id(), 11);
  QCOMPARE(t2.ioCount(), 1);
  QCOMPARE(t2.data(), QByteArray("tt"));

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
  mdtPortThread *thread;
  QList<QByteArray> frames;

  // Create a temporary file
  QVERIFY(file.open());
  file.close();

  // Port setup
  cfg.setFrameType(mdtFrame::FT_ASCII);
  port = new mdtPort;
  port->setConfig(&cfg);

  // Init port manager
  QVERIFY(m.readThread() == 0);
  QVERIFY(m.writeThread() == 0);
  m.setPort(port);
  m.setKeepTransactionsDone(true);
  thread = new mdtPortWriteThread;
  m.addThread(thread);
  QVERIFY(m.writeThread() == thread);
  thread = new mdtPortReadThread;
  m.addThread(thread);
  QVERIFY(m.readThread() == thread);
  m.setPortName(file.fileName());
  QVERIFY(m.openPort());

  // start threads
  QVERIFY(m.start());
  QVERIFY(m.isRunning());

  // Check the wait function without available data
  QVERIFY(!m.waitOneTransactionDone());

  // Send some data
  QCOMPARE(m.writeData("ABCD\nEFGH\n"), 1);

  // We must re-open the file, else nothing will be readen
  QTest::qWait(100);
  m.closePort();
  QVERIFY(m.openPort());
  QVERIFY(m.start());
  // Get readen data
  QVERIFY(m.waitOneTransactionDone());
  // Try to get a frame by ID (will not work, but must not crash"
  ///m.readenFrame(0);
  // Check readen data
  frames = m.readenFrames();
  QCOMPARE(frames.size(), 2);
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
  QVERIFY(m.readThread() == 0);
  QVERIFY(m.writeThread() == 0);
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
  QVERIFY(m.sendReadStatusByteRequest() >= 0);
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
  ///QVERIFY(m.waitOnFrame(bTag, 20000));
  ///QVERIFY(m.waitTransactionDone(bTag, 20000));
  QVERIFY(m.waitTransactionDone(bTag));
  frames = m.readenFrames();
  QVERIFY(frames.size() > 0);

  // Try to query device
  QVERIFY(m.sendCommand("*CLS\n") > 0);
  QVERIFY(!m.sendQuery("*IDN?\n").isEmpty());
}

void mdtPortManagerTest::modbusTcpPortTest()
{
  mdtModbusTcpPortManager m;
  QList<mdtPortInfo*> portInfoList;
  QList<mdtPortInfo*> portInfoList2;
  mdtPortInfo validPortInfo;
  mdtPortInfo invalidPortInfo;
  QStringList hosts;
  mdtFrameCodecModbus codec;
  QByteArray pdu;
  QHash<quint16, QByteArray> pdus;
  int tId1, tId2, tId3;
  QString ipLeftPart;
  QString ipRightPart;
  QStringList ipParts;
  QString portName;
  bool found;
  mdtPortTransaction *transaction;

  // Check scan with some invalid network setup
  QVERIFY(m.scan(QNetworkInterface()).size() < 1);

  qDebug() << "* A MODBUS/TCP compatible device must be attached, else test will fail *";

  // Try to scan all network interfaces (except the loopback) + check abort
  QTimer::singleShot(100, &m, SLOT(abortScan()));
  qDebug() << "mdtPortManagerTest::modbusTcpPortTest(): scanning network ...";
  portInfoList = m.scan(QNetworkInterface::allInterfaces(), 502, 100);
  QCOMPARE(portInfoList.size(), 0);
  // Scan again
  qDebug() << "mdtPortManagerTest::modbusTcpPortTest(): scanning network ...";
  portInfoList = m.scan(QNetworkInterface::allInterfaces(), 502, 100);
  if(portInfoList.size() < 1){
    QSKIP("No MODBUS/TCP device found, or other error", SkipAll);
  }

  // Check that other scan method works
  for(int i=0; i<portInfoList.size(); i++){
    QVERIFY(portInfoList.at(i) != 0);
    hosts << portInfoList.at(i)->portName();
  }
  qDebug() << "mdtPortManagerTest::modbusTcpPortTest(): scanning network (hosts: " << hosts << ")";
  QCOMPARE(hosts.size(), portInfoList.size());
  portInfoList2 = m.scan(hosts);
  QCOMPARE(portInfoList2.size(), portInfoList.size());

  // Check tha we can save scan result
  QVERIFY(m.saveScanResult(portInfoList));
  // Rescan with cache file
  portInfoList2 = m.scan(m.readScanResult());
  QCOMPARE(portInfoList2.size(), portInfoList.size());

  // Set the valid port info
  validPortInfo = *portInfoList.at(0);
  // Set the invalid port info - Assume wa have 255.255.255.0 netmask..
  ipParts = validPortInfo.portName().split(".");
  QCOMPARE(ipParts.size(), 4);
  ipLeftPart = ipParts.at(0) + "." + ipParts.at(1) + "." + ipParts.at(2) + ".";
  for(int i=1; i<255; i++){
    ipRightPart = QString::number(i);
    portName = ipLeftPart + ipRightPart + ":502";
    // See if this IP existe in scan result
    found = false;
    for(int j=0; j<portInfoList.size(); j++){
      if(portInfoList.at(j)->portName() == portName){
        found = true;
        break;
      }
    }
    if(!found){
      invalidPortInfo.setPortName(portName);
      break;
    }
  }
  qDebug() << "Invalid port name: " << invalidPortInfo.portName();

  // We not need the scan result anymore, free memory
  qDeleteAll(portInfoList);
  portInfoList.clear();
  qDeleteAll(portInfoList2);
  portInfoList2.clear();

  /*
   * Open/close test with valid Host
   */
  m.setPortInfo(validPortInfo);
  QVERIFY(m.openPort());
  QVERIFY(m.start());
  QVERIFY(m.isRunning());
  m.stop();
  QVERIFY(!m.isRunning());
  QVERIFY(m.start());
  QVERIFY(m.isRunning());
  m.closePort();
  QVERIFY(!m.isRunning());

  /*
   * Open/close test with invalid Host
   */
  m.setPortInfo(invalidPortInfo);
  QVERIFY(!m.openPort());
  QVERIFY(!m.isRunning());
  m.stop();
  QVERIFY(!m.isRunning());
  m.closePort();
  QVERIFY(!m.isRunning());

  /*
   * Open/close test with invalid + valid Host
   */
  m.setPortInfo(invalidPortInfo);
  QVERIFY(!m.openPort());
  QVERIFY(!m.isRunning());
  m.setPortInfo(validPortInfo);
  QVERIFY(m.openPort());
  QVERIFY(m.start());
  QVERIFY(m.isRunning());
  m.stop();
  QVERIFY(!m.isRunning());

  /*
   * Communication test
   */
  m.setPortInfo(validPortInfo);
  QVERIFY(m.openPort());
  QVERIFY(m.start());
  QVERIFY(m.isRunning());

  /*
   * "Check" direct PDU write in query/reply mode
   */
  pdu = codec.encodeReadCoils(0, 3);
  // Transaction 1
  transaction = m.getNewTransaction();
  QVERIFY(transaction != 0);
  transaction->setQueryReplyMode(true);
  ///tId1 = m.writeData(pdu, true);
  ///tId1 = m.writeData(pdu, transaction);
  transaction->setData(pdu);
  tId1 = m.writeData(transaction);
  QVERIFY(tId1 >= 0);
  // Transaction 2
  transaction = m.getNewTransaction();
  QVERIFY(transaction != 0);
  transaction->setQueryReplyMode(true);
  ///tId2 = m.writeData(pdu, true);
  ///tId2 = m.writeData(pdu, transaction);
  transaction->setData(pdu);
  tId2 = m.writeData(transaction);
  QVERIFY(tId2 >= 0);
  // Transaction 3
  transaction = m.getNewTransaction();
  QVERIFY(transaction != 0);
  transaction->setQueryReplyMode(true);
  ///tId3 = m.writeData(pdu, true);
  ///tId3 = m.writeData(pdu, transaction);
  transaction->setData(pdu);
  tId3 = m.writeData(transaction);
  QVERIFY(tId3 >= 0);
  qDebug() << "TEST, tId1: " << tId1 << " , tId2: " << tId2 << " , tId3: " << tId3;
  ///QVERIFY(m.waitOnFrame(tId1));
  QVERIFY(m.waitTransactionDone(tId1));
  QVERIFY(!m.readenFrame(tId1).isEmpty());
  ///QVERIFY(m.waitOnFrame(tId2));
  QVERIFY(m.waitTransactionDone(tId2));
  ///QVERIFY(m.waitOnFrame(tId3));
  QVERIFY(m.waitTransactionDone(tId3));
  QCOMPARE(m.readenFrames().size(), 2);

  // If query/reply mode is diseabled, waitOnFrame() will timeout
  pdu = codec.encodeReadCoils(0, 3);
  transaction = m.getNewTransaction();
  QVERIFY(transaction != 0);
  transaction->setQueryReplyMode(false);
  ///tId1 = m.writeData(pdu, false);
  ///tId1 = m.writeData(pdu, transaction);
  transaction->setData(pdu);
  tId1 = m.writeData(transaction);
  ///QVERIFY(!m.waitOnFrame(tId1));
  QVERIFY(!m.waitTransactionDone(tId1));

  // All frames must be consumed
  QCOMPARE(m.readenFrames().size(), 0);

  // Check ReadDeviceIdentification
  pdu = codec.encodeReadDeviceIdentification(0, false);
  transaction = m.getNewTransaction();
  QVERIFY(transaction != 0);
  transaction->setQueryReplyMode(true);
  ///tId1 = m.writeData(pdu, true);
  ///tId1 = m.writeData(pdu, transaction);
  transaction->setData(pdu);
  tId1 = m.writeData(transaction);
  QVERIFY(tId1 >= 0);
  ///QVERIFY(m.waitOnFrame(tId1));
  QVERIFY(m.waitTransactionDone(tId1));
  QVERIFY(!m.readenFrame(tId1).isEmpty());
  // Some devices not implement this FC, so we cannot check it..

  // Try to get HW node address (we cannot check it, because we don't know witch hardware is used for tests..)
  qDebug() << m.getHardwareNodeAddress(4);
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
