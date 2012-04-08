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
#include "mdtSerialPortTest.h"
#include "mdtSerialPortManager.h"
#include "mdtSerialPort.h"
#include "mdtSerialPortConfig.h"
#include "mdtPortThread.h"
#include "mdtPortReadThread.h"
#include "mdtPortWriteThread.h"
#include "mdtSerialPortCtlThread.h"

#include <QtGlobal>
#include <QDateTime>
#include <cstdio>

#include <QByteArray>

void mdtSerialPortTest::essais()
{
 /// mdtSerialPortManager m;
  mdtSerialPort sp;
  mdtSerialPortConfig cfg;
  mdtSerialPortCtlThread ctlThd;

  ///m.setPort(&sp);
  ///m.scan();

  cfg.setBaudRate(9600);
  cfg.setDataBitsCount(8);
  cfg.setStopBitsCount(1);
  cfg.setParity(mdtSerialPortConfig::NoParity);
  cfg.enableFlowCtlXonXoff();
  cfg.enableFlowCtlRtsCts();
  qDebug() << "TEST: setAttributes() ...";
  QVERIFY(sp.setAttributes("/dev/ttyS0"));
  qDebug() << "TEST: open() ...";
  QVERIFY(sp.open(cfg));

  ctlThd.setPort(&sp);
  qDebug() << "TEST end";
  //connect(this, SIGNAL(testSignal(bool)), &sp, SLOT(setRts(bool)));
  //emit testSignal(true);
}

void mdtSerialPortTest::mdtSerialPortManagerTest()
{
  mdtSerialPortManager m;
  QStringList portsList;

  qDebug() << "* Be shure that the computer has at least one serial port, else test will fail *";

  // Verify that scan() function works ..
  qDebug() << "TEST: scan() ...";
  portsList = m.scan();
  qDebug() << "TEST: scan() END";
  QVERIFY(portsList.size() > 0);

  // Port setup
  m.config().setFrameType(mdtFrame::FT_ASCII);
  m.config().setEndOfFrameSeq("$");
  
  // Init port manager
  qDebug() << "TEST: setPortName() ...";
  QVERIFY(m.setPortName(portsList.at(0)));
  qDebug() << "TEST: setPortName() END";
  qDebug() << "TEST: openPort() ...";
  QVERIFY(m.openPort());
  qDebug() << "TEST: openPort() END";
  
  // Check that available baud rates contains 9600 (very standard, should exists on most device)
  QVERIFY(m.port().availableBaudRates().size() > 0);
  QVERIFY(m.port().availableBaudRates().contains(9600));
  // Chack that the right port is set
  QVERIFY(m.port().name() == portsList.at(0));

  // Start threads
  QVERIFY(m.start());

  // Send some data
  QVERIFY(m.writeData("Test$"));

  // Wait on answer - Timout: 500 [ms]
  QVERIFY(m.waitReadenFrame(500));

  // Verify received data
  QVERIFY(m.lastReadenFrame() == "Test");
 
  // End
  m.closePort();
}

void mdtSerialPortTest::mdtSerialPortConfigTest()
{
  QFETCH(int, baudRate);
  QFETCH(int, baudRateRef);
  QFETCH(int, dataBits);
  QFETCH(int, dataBitsRef);
  QFETCH(int, stopBits);
  QFETCH(int, stopBitsRef);
  QFETCH(int, parity);
  QFETCH(int, parityRef);
  QFETCH(bool, flowCtlRtsCts);
  QFETCH(bool, flowCtlRtsCtsRef);
  QFETCH(bool, flowCtlXonXoff);
  QFETCH(bool, flowCtlXonXoffRef);
  QFETCH(char, xonChar);
  QFETCH(char, xonCharRef);
  QFETCH(char, xoffChar);
  QFETCH(char, xoffCharRef);
  QFETCH(int, readFrameSize);
  QFETCH(int, readFrameSizeRef);
  QFETCH(int, readQueueSize);
  QFETCH(int, readQueueSizeRef);
  QFETCH(int, writeFrameSize);
  QFETCH(int, writeFrameSizeRef);
  QFETCH(int, writeQueueSize);
  QFETCH(int, writeQueueSizeRef);

  // Create a new config
  mdtSerialPortConfig cfg1, cfg2;

  // Verify default config
  QCOMPARE(cfg1.baudRate(), 9600);
  QCOMPARE(cfg1.dataBitsCount(), 8);
  QCOMPARE(cfg1.stopBitsCount(), 1);
  QCOMPARE(cfg1.parity(), mdtSerialPortConfig::NoParity);
  QVERIFY(!cfg1.flowCtlRtsCtsEnabled());
  QVERIFY(!cfg1.flowCtlXonXoffEnabled());
  QCOMPARE(cfg1.xonChar(), (char)MDT_DEF_XON);
  QCOMPARE(cfg1.xoffChar(), (char)MDT_DEF_XOFF);
  QVERIFY(cfg1.readFrameSize() == 1024);
  QVERIFY(cfg1.readQueueSize() == 10);
  QVERIFY(cfg1.writeFrameSize() == 1024);
  QVERIFY(cfg1.writeQueueSize() == 10);
  
  // With default config, cfg1 and cfg2 must be the same
  QVERIFY(cfg1 == cfg2);
  QVERIFY(!(cfg1 != cfg2));

  // Check veriety of combinaisons
  cfg1.setBaudRate(baudRate);
  QCOMPARE(cfg1.baudRate(), baudRateRef);
  cfg1.setDataBitsCount(dataBits);
  QCOMPARE(cfg1.dataBitsCount(), dataBitsRef);
  cfg1.setStopBitsCount(stopBits);
  QCOMPARE(cfg1.stopBitsCount(), stopBitsRef);
  cfg1.setParity((mdtSerialPortConfig::parity_t)parity);
  QCOMPARE((int)cfg1.parity(), parityRef);
  if(flowCtlRtsCts){
    cfg1.enableFlowCtlRtsCts();
  }else{
    cfg1.diseableFlowCtlRtsCts();
  }
  QCOMPARE(cfg1.flowCtlRtsCtsEnabled(), flowCtlRtsCtsRef);
  if(flowCtlXonXoff){
    cfg1.enableFlowCtlXonXoff(xonChar, xoffChar);
    QCOMPARE(cfg1.xonChar(), xonCharRef);
    QCOMPARE(cfg1.xoffChar(), xoffCharRef);
  }else{
    cfg1.diseableFlowCtlXonXoff();
  }
  QCOMPARE(cfg1.flowCtlXonXoffEnabled(), flowCtlXonXoffRef);
  cfg1.setReadFrameSize(readFrameSize);
  QCOMPARE(cfg1.readFrameSize(), readFrameSizeRef);
  cfg1.setReadQueueSize(readQueueSize);
  QCOMPARE(cfg1.readQueueSize(), readQueueSizeRef);
  cfg1.setWriteFrameSize(writeFrameSize);
  QCOMPARE(cfg1.writeFrameSize(), writeFrameSizeRef);
  cfg1.setWriteQueueSize(writeQueueSize);
  QCOMPARE(cfg1.writeQueueSize(), writeQueueSizeRef);

  // Test copy
  cfg2 = cfg1;
  QCOMPARE(cfg2.baudRate(), baudRateRef);
  QCOMPARE(cfg2.dataBitsCount(), dataBitsRef);
  QCOMPARE(cfg2.stopBitsCount(), stopBitsRef);
  QCOMPARE((int)cfg2.parity(), parityRef);
  QCOMPARE(cfg2.flowCtlRtsCtsEnabled(), flowCtlRtsCtsRef);
  QCOMPARE(cfg2.xonChar(), xonCharRef);
  QCOMPARE(cfg2.xoffChar(), xoffCharRef);
  QCOMPARE(cfg2.flowCtlXonXoffEnabled(), flowCtlXonXoffRef);
  QCOMPARE(cfg2.readFrameSize(), readFrameSizeRef);
  QCOMPARE(cfg2.readFrameSize(), readFrameSizeRef);
  QCOMPARE(cfg2.readQueueSize(), readQueueSizeRef);
  QCOMPARE(cfg2.writeFrameSize(), writeFrameSizeRef);
  QCOMPARE(cfg2.writeQueueSize(), writeQueueSizeRef);

  // cfg1 and cfg2 must match
  QVERIFY(cfg1 == cfg2);
  QVERIFY(!(cfg1 != cfg2));

  // Make some change on original
  cfg1.setBaudRate(-10);
  cfg1.setDataBitsCount(-12);
  cfg1.setStopBitsCount(-1);
  cfg1.enableFlowCtlXonXoff('m', 'n');
  cfg1.setReadFrameSize(12456);
  cfg1.setReadQueueSize(21375);
  cfg1.setWriteFrameSize(51217);
  cfg1.setWriteQueueSize(11111);
  QCOMPARE(cfg2.baudRate(), baudRateRef);
  QCOMPARE(cfg2.dataBitsCount(), dataBitsRef);
  QCOMPARE(cfg2.stopBitsCount(), stopBitsRef);
  QCOMPARE((int)cfg2.parity(), parityRef);
  QCOMPARE(cfg2.flowCtlRtsCtsEnabled(), flowCtlRtsCtsRef);
  QCOMPARE(cfg2.xonChar(), xonCharRef);
  QCOMPARE(cfg2.xoffChar(), xoffCharRef);
  QCOMPARE(cfg2.flowCtlXonXoffEnabled(), flowCtlXonXoffRef);
  QCOMPARE(cfg2.readFrameSize(), readFrameSizeRef);
  QCOMPARE(cfg2.readQueueSize(), readQueueSizeRef);
  QCOMPARE(cfg2.writeFrameSize(), writeFrameSizeRef);

  // cfg1 and cfg2 must not match
  QVERIFY(!(cfg1 == cfg2));
  QVERIFY(cfg1 != cfg2);
}

void mdtSerialPortTest::mdtSerialPortConfigTest_data()
{
  QTest::addColumn<int>("baudRate");
  QTest::addColumn<int>("baudRateRef");
  QTest::addColumn<int>("dataBits");
  QTest::addColumn<int>("dataBitsRef");
  QTest::addColumn<int>("stopBits");
  QTest::addColumn<int>("stopBitsRef");
  QTest::addColumn<int>("parity");
  QTest::addColumn<int>("parityRef");
  QTest::addColumn<bool>("flowCtlRtsCts");
  QTest::addColumn<bool>("flowCtlRtsCtsRef");
  QTest::addColumn<bool>("flowCtlXonXoff");
  QTest::addColumn<bool>("flowCtlXonXoffRef");
  QTest::addColumn<char>("xonChar");
  QTest::addColumn<char>("xonCharRef");
  QTest::addColumn<char>("xoffChar");
  QTest::addColumn<char>("xoffCharRef");
  QTest::addColumn<int>("readFrameSize");
  QTest::addColumn<int>("readFrameSizeRef");
  QTest::addColumn<int>("readQueueSize");
  QTest::addColumn<int>("readQueueSizeRef");
  QTest::addColumn<int>("writeFrameSize");
  QTest::addColumn<int>("writeFrameSizeRef");
  QTest::addColumn<int>("writeQueueSize");
  QTest::addColumn<int>("writeQueueSizeRef");

  QTest::newRow("Config 01") << 50 << 50 /* Baud rate */ \
    << 5 << 5   /* Data bits */ \
    << 1 << 1   /* Stop bits */ \
    << (int)mdtSerialPortConfig::ParityOdd << (int)mdtSerialPortConfig::ParityOdd \
    << true << true /* Flow ctl RTS/CTS */ \
    << true << true /* Flow ctl Xon/Xoff */ \
    << 'a' << 'a'   /* Xon char */ \
    << 'b' << 'b'   /* Xoff char */ \
    << 1000 << 1000 /* readFrameSize */ \
    << 5 << 5       /* readQueueSize */ \
    << 900 << 900   /* writeFrameSize */ \
    << 6 << 6       /* writeQueueSize */ ;
}

void mdtSerialPortTest::mdtSerialPortStartStopTest()
{
  mdtSerialPort sp;
  mdtSerialPortConfig cfg;
  mdtSerialPortCtlThread ctlThd;

#ifdef Q_OS_UNIX
  QVERIFY(sp.setAttributes("/dev/ttyS0"));
#elif defined Q_OS_WIN
  QVERIFY(sp.setAttributes("COM1"));
#endif
  // Setup - We keep default config
  QVERIFY(sp.open(cfg));

  // Assign sp to the control thread
  ctlThd.setPort(&sp);

  // Start control thread
  QVERIFY(ctlThd.start());
  ctlThd.stop();
  QVERIFY(!ctlThd.isRunning());
  qsrand(QDateTime::currentDateTime ().toTime_t ());
  for(int i=0; i<10; i++){
    QVERIFY(ctlThd.start());
    QVERIFY(ctlThd.isRunning());
    QTest::qWait((100.0*(double)qrand()) / RAND_MAX);
    ctlThd.stop();
    QVERIFY(!ctlThd.isRunning());
  }
}

void mdtSerialPortTest::mdtSerialPortCtlSignalsTest()
{
  mdtSerialPort sp;
  mdtSerialPortConfig cfg;
  mdtSerialPortCtlThread ctlThd;

  qDebug() << "* make shure that test terminal is plugged on serial port (ttyS0 , COM1) *";

  // Setup
#ifdef Q_OS_UNIX
  QVERIFY(sp.setAttributes("/dev/ttyS0"));
#elif defined Q_OS_WIN
  QVERIFY(sp.setAttributes("COM1"));
#endif
  cfg.setBaudRate(9600);
  cfg.setDataBitsCount(8);
  cfg.setStopBitsCount(1);
  cfg.setParity(mdtSerialPortConfig::NoParity);
  QVERIFY(sp.open(cfg));

  // Assign sp to the control thread and start
  ctlThd.setPort(&sp);
  QVERIFY(ctlThd.start());

  // Initial states NOTE: CAR et RNG ?
  //QVERIFY(!sp.carIsOn());
  //QVERIFY(!sp.dsrIsOn());
  //QVERIFY(!sp.ctsIsOn());
  //QVERIFY(!sp.rngIsOn());
  
  // RTS/CTS
  sp.setRts(true);
  QTest::qWait(50);
  QVERIFY(sp.ctsIsOn());
  sp.setRts(false);
  QTest::qWait(50);
  QVERIFY(!sp.ctsIsOn());

  // DTR/DSR
  sp.setDtr(true);
  QTest::qWait(50);
  QVERIFY(sp.dsrIsOn());
  sp.setDtr(false);
  QTest::qWait(50);
  QVERIFY(!sp.dsrIsOn());

  ctlThd.stop();
}

void mdtSerialPortTest::mdtSerialPortTxRxBinaryTest()
{
  mdtSerialPort sp;
  mdtSerialPortConfig cfg;
  mdtPortReadThread rxThd;
  mdtPortWriteThread txThd;
  mdtFrame *frame;
  QByteArray portName;
  QByteArray rxData;

  // Get data set
  QFETCH(QString, data);

  // Set port name
#ifdef Q_OS_UNIX
  portName = "/dev/ttyS0";
#elif defined Q_OS_WIN
  portName = "COM1";
#endif

  qDebug() << "* make shure that test terminal is plugged on port (" << portName << ") *";

  /* Normal write */

  // Setup
  cfg.setFrameType(mdtFrame::FT_RAW);
  cfg.setReadQueueSize(data.size()+10);    // In raw mode, we don't know how many frames that will be used at read (depend on OS, UART, ...)
  QVERIFY(sp.setAttributes(portName));
  QVERIFY(sp.open(cfg));

  // Assign sp to the RX thread and start
  rxThd.setPort(&sp);
  QVERIFY(rxThd.start());
  QVERIFY(rxThd.isRunning());
  // Assign sp to the TX thread and start
  txThd.setPort(&sp);
  QVERIFY(txThd.start());
  QVERIFY(txThd.isRunning());

  // Get a frame
  sp.lockMutex();
  QVERIFY(sp.writeFramesPool().size() > 0);
  frame = sp.writeFramesPool().dequeue();
  sp.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->clear();
  frame->append(data);
  sp.lockMutex();
  sp.writeFrames().enqueue(frame);
  sp.unlockMutex();

  // Wait some time and verify that data was transfered
  QTest::qWait(3*data.size()+100);
  sp.lockMutex();
  // We don't know how many frames that were used, so cat all together, and check the result.
  rxData = "";
  while(sp.readenFrames().size() > 0){
    frame = sp.readenFrames().dequeue();
    QVERIFY(frame != 0);
    QVERIFY(frame->isComplete());
    // Cat readen data
    rxData += *frame;
    // Restore frame to pool
    sp.readFramesPool().enqueue(frame);
  }
  sp.unlockMutex();
  QVERIFY(rxData == data);

  // Stop threads and close port
  rxThd.stop();
  txThd.stop();
  sp.close();

  /* Byte per byte write */

  // Setup and open port
  cfg.setBytePerByteWrite(true, 1);
  cfg.setReadQueueSize(data.size()+10); // In RAW mode, each transfered byte will generate a RX frame
  QVERIFY(sp.open(cfg));

  // Re-affect threads (so new setup will be read) and start it
  rxThd.setPort(&sp);
  txThd.setPort(&sp);
  QVERIFY(rxThd.start());
  QVERIFY(rxThd.isRunning());
  QVERIFY(txThd.start());
  QVERIFY(txThd.isRunning());

  // Get a frame
  sp.lockMutex();
  QVERIFY(sp.writeFramesPool().size() > 0);
  frame = sp.writeFramesPool().dequeue();
  sp.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->clear();
  frame->append(data);
  sp.lockMutex();
  sp.writeFrames().enqueue(frame);
  sp.unlockMutex();

  // Wait some time and verify that data was transfered
  QTest::qWait(3*data.size()+100);
  sp.lockMutex();
  // We don't know how many frames that were used, so cat all together, and check the result.
  rxData = "";
  while(sp.readenFrames().size() > 0){
    frame = sp.readenFrames().dequeue();
    QVERIFY(frame != 0);
    QVERIFY(frame->isComplete());
    // Cat readen data
    rxData += *frame;
    // Restore frame to pool
    sp.readFramesPool().enqueue(frame);
  }
  sp.unlockMutex();
  QVERIFY(rxData == data);

  // Stop threads and close port
  rxThd.stop();
  txThd.stop();
  sp.close();
}

void mdtSerialPortTest::mdtSerialPortTxRxBinaryTest_data()
{
  QString str;

  QTest::addColumn<QString>("data");

  QTest::newRow("Empty data") << "";
  QTest::newRow("1 char data") << "A";
  QTest::newRow("short len data") << "ABCD";
  QTest::newRow("middle len data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  QTest::newRow("long data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  // Very long data
  str = "";
  for(int i=0; i<50; i++){
    str += "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  }
  QTest::newRow("Very long data") << str;
}

void mdtSerialPortTest::mdtSerialPortTxRxBinaryTopTest()
{
  mdtSerialPort sp;
  mdtSerialPortConfig cfg;
  mdtPortReadThread rxThd;
  mdtPortWriteThread txThd;
  mdtFrame *frame;
  QByteArray portName;

  // Get data set
  QFETCH(QStringList, data);

  // Set port name
#ifdef Q_OS_UNIX
  portName = "/dev/ttyS0";
#elif defined Q_OS_WIN
  portName = "COM1";
#endif

  qDebug() << "* make shure that test terminal is plugged on port (" << portName << ") *";

  // Setup
  cfg.setFrameType(mdtFrame::FT_RAW_TOP);
  cfg.setWriteQueueSize(data.size()+10);
  cfg.setWriteMinWaitTime(100);
  cfg.setReadQueueSize(data.size()+10);
  cfg.setReadTimeout(50);
  cfg.setUseReadTimeoutProtocol(true);
  QVERIFY(sp.setAttributes(portName));
  QVERIFY(sp.open(cfg));

  // Assign sp to the RX thread and start
  rxThd.setPort(&sp);
  QVERIFY(rxThd.start());
  QVERIFY(rxThd.isRunning());
  // Assign sp to the TX thread and start
  txThd.setPort(&sp);
  QVERIFY(txThd.start());
  QVERIFY(txThd.isRunning());

  // Transmit data
  sp.lockMutex();
  for(int i=0; i<data.size(); i++){
    QVERIFY(sp.writeFramesPool().size() > 0);
    frame = sp.writeFramesPool().dequeue();
    frame->clear();
    frame->append(data.at(i));
    sp.writeFrames().enqueue(frame);
  }
  sp.unlockMutex();

  // Wait some time and verify that data was transfered
  QTest::qWait(150*data.size()+100);
  sp.lockMutex();
  // We have a special case for empty data test
  if((data.size() == 1)&&(data.at(0).size() == 0)){
    QVERIFY(sp.readenFrames().size() == 0);
  }else{
    QVERIFY(sp.readenFrames().size() == data.size());
    // Verify each frame
    for(int i=0; i<data.size(); i++){
      frame = sp.readenFrames().dequeue();
      QVERIFY(frame != 0);
      QVERIFY(frame->isComplete());
      // Verify readen data
      QVERIFY(*frame == data.at(i));
      // Restore frame to pool
      sp.readFramesPool().enqueue(frame);
    }
  }
  sp.unlockMutex();

  // Stop threads and close port
  rxThd.stop();
  txThd.stop();
  sp.close();
}

void mdtSerialPortTest::mdtSerialPortTxRxBinaryTopTest_data()
{
  QStringList lst;

  QTest::addColumn<QStringList>("data");

  lst.clear();
  lst << "";
  QTest::newRow("Empty data") << lst;
  lst.clear();
  lst << "Z" << "5";
  QTest::newRow("1 char data") << lst;
  lst.clear();
  lst << "ABCD" << "01234";
  QTest::newRow("short data") << lst;
  lst.clear();
  lst << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << "0123456789";
  QTest::newRow("middle len data") << lst;
  lst.clear();
  lst << "abcdefghijklmnopqrstuvwx" << "0123456789" << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  QTest::newRow("long data") << lst;
}

void mdtSerialPortTest::mdtSerialPortTxRxAsciiTest()
{
  mdtSerialPort sp;
  mdtSerialPortConfig cfg;
  mdtPortReadThread rxThd;
  mdtPortWriteThread txThd;
  mdtFrame *frame;
  QByteArray portName;
  QByteArray rxData;

  // Get data set
  QFETCH(QString, data);
  QFETCH(QStringList, refData);

  // Set port name
#ifdef Q_OS_UNIX
  portName = "/dev/ttyS0";
#elif defined Q_OS_WIN
  portName = "COM1";
#endif

  qDebug() << "* make shure that test terminal is plugged on port (" << portName << ") *";

  /* Normal write */

  // Setup
  cfg.setFrameType(mdtFrame::FT_ASCII);
  cfg.setEndOfFrameSeq("*");
  cfg.setReadQueueSize(10);
  QVERIFY(sp.setAttributes(portName));
  QVERIFY(sp.open(cfg));

  // Assign sp to the RX thread and start
  rxThd.setPort(&sp);
  QVERIFY(rxThd.start());
  QVERIFY(rxThd.isRunning());
  // Assign sp to the TX thread and start
  txThd.setPort(&sp);
  QVERIFY(txThd.start());
  QVERIFY(txThd.isRunning());

  // Get a frame
  sp.lockMutex();
  QVERIFY(sp.writeFramesPool().size() > 0);
  frame = sp.writeFramesPool().dequeue();
  sp.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->clear();
  frame->append(data);
  sp.lockMutex();
  sp.writeFrames().enqueue(frame);
  sp.unlockMutex();

  // Wait some time and verify that data was transfered
  QTest::qWait(3*data.size()+100);
  sp.lockMutex();
  QVERIFY(sp.readenFrames().size() == refData.size());
  // Verify each readen data frame
  for(int i=0; i<refData.size(); i++){
    frame = sp.readenFrames().dequeue();
    QVERIFY(frame != 0);
    QVERIFY(frame->isComplete());
    // Verify readen data
    QVERIFY(*frame == refData.at(i));
    // Restore frame to pool
    sp.readFramesPool().enqueue(frame);
  }
  sp.unlockMutex();

  // Stop threads and close port
  rxThd.stop();
  txThd.stop();
  sp.close();

  /* Byte per byte write */

  // Setup and open port
  cfg.setBytePerByteWrite(true, 1);
  cfg.setReadQueueSize(data.size()+10); // In RAW mode, each transfered byte can use a RX frame
  QVERIFY(sp.open(cfg));

  // Re-affect threads (so new setup will be read) and start it
  rxThd.setPort(&sp);
  txThd.setPort(&sp);
  QVERIFY(rxThd.start());
  QVERIFY(rxThd.isRunning());
  QVERIFY(txThd.start());
  QVERIFY(txThd.isRunning());

  // Get a frame
  sp.lockMutex();
  QVERIFY(sp.writeFramesPool().size() > 0);
  frame = sp.writeFramesPool().dequeue();
  sp.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->clear();
  frame->append(data);
  sp.lockMutex();
  sp.writeFrames().enqueue(frame);
  sp.unlockMutex();

  // Wait some time and verify that data was transfered
  QTest::qWait(3*data.size()+100);
  sp.lockMutex();
  // We don't know how many frames that were used, so cat all together, and check the result.
  rxData = "";
  while(sp.readenFrames().size() > 0){
    frame = sp.readenFrames().dequeue();
    QVERIFY(frame != 0);
    QVERIFY(frame->isComplete());
    // Cat readen data
    rxData += *frame;
    // Restore frame to pool
    sp.readFramesPool().enqueue(frame);
  }
  sp.unlockMutex();
  // Verify that rx data matches
  data.remove("*");
  QVERIFY(rxData == data);

  // Stop threads and close port
  rxThd.stop();
  txThd.stop();
  sp.close();
}

void mdtSerialPortTest::mdtSerialPortTxRxAsciiTest_data()
{
  QString str;
  QStringList lst;

  QTest::addColumn<QString>("data");
  QTest::addColumn<QStringList>("refData");

  lst.clear();
  lst << "";
  QTest::newRow("Empty data") << "*" << lst;
  lst.clear();
  lst << "A";
  QTest::newRow("1 char data") << "A*" << lst;
  lst.clear();
  lst << "ABCD";
  QTest::newRow("1 frame short data") << "ABCD*" << lst;
  lst.clear();
  lst << "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  QTest::newRow("1 frame middle len data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ*" << lst;
  lst.clear();
  lst << "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  QTest::newRow("1 frame long data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789*" << lst;
  lst.clear();
  lst << "A" << "B" << "C" << "D";
  QTest::newRow("Multi frame short data") << "A*B*C*D*" << lst;
  lst.clear();
  lst << "ABCD" << "1234" << "abcd";
  QTest::newRow("Multi frame middle len data") << "ABCD*1234*abcd*" << lst;
  lst.clear();
  lst << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << "0123456789" << "abcdefghijklmnopqrstuvwxyz";
  QTest::newRow("Multi frame long data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ*0123456789*abcdefghijklmnopqrstuvwxyz*" << lst;
}
