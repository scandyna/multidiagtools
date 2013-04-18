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
#include "mdtApplication.h"

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

  sp.setPortName("/dev/ttyS0");
  QVERIFY(sp.open() == mdtAbstractPort::NoError);
  QVERIFY(sp.uartType() != mdtAbstractSerialPort::UT_UNKNOW);
  qDebug() << "UART: " << sp.uartTypeStr();
  qDebug() << "Baudrates: " << sp.availableBaudRates();
  qDebug() << "TEST: open() ...";
  sp.setConfig(&cfg);
  QVERIFY(sp.setup() == mdtAbstractPort::NoError);

  ctlThd.setPort(&sp);
  qDebug() << "TEST end";
  //connect(this, SIGNAL(testSignal(bool)), &sp, SLOT(setRts(bool)));
  //emit testSignal(true);
}

void mdtSerialPortTest::openCloseTest()
{
  mdtSerialPort port;
  mdtSerialPortConfig cfg;
  QString portName;
  QFileInfo fileInfo;

  // Initial state
  QVERIFY(!port.isOpen());

  // Port name
#ifdef Q_OS_UNIX
  portName = "/dev/ttyS0";
#elif defined Q_OS_WIN
  portName = "COM1";
#endif

  // Open port
  port.setPortName(portName);
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  QVERIFY(port.isOpen());

#ifdef Q_OS_UNIX
  // Check about lock file
  fileInfo.setFile(portName);
  qDebug() << "mdtPortTest::openCloseTest() , lock file: LCK.." << fileInfo.fileName();
  QVERIFY(QFile::exists("/tmp/LCK.." + fileInfo.fileName()));
  QVERIFY(QFile::exists("/var/lock/LCK.." + fileInfo.fileName()));
#endif

  // Setup
  port.setConfig(&cfg);
  QVERIFY(port.setup() == mdtAbstractPort::NoError);
  QVERIFY(port.isOpen());

  // Close
  port.close();
  QVERIFY(!port.isOpen());

#ifdef Q_OS_UNIX
  // Check about lock file
  fileInfo.setFile(portName);
  QVERIFY(!QFile::exists("/tmp/LCK.." + fileInfo.fileName()));
  QVERIFY(!QFile::exists("/var/lock/LCK.." + fileInfo.fileName()));
#endif

  /*
   * When port open fails, lockfiles must not exists
   */

  // Open port
  port.setPortName("akjdhuhdhkehdhieahdahiduhuhdjshiheihdu4545ffdf23dweheiuh");
  QVERIFY(port.open() != mdtAbstractPort::NoError);
  QVERIFY(!port.isOpen());

#ifdef Q_OS_UNIX
  // Check about lock file
  QVERIFY(!QFile::exists("/tmp/LCK..akjdhuhdhkehdhieahdahiduhuhdjshiheihdu4545ffdf23dweheiuh"));
  QVERIFY(!QFile::exists("/var/lock/LCK..akjdhuhdhkehdhieahdahiduhuhdjshiheihdu4545ffdf23dweheiuh"));
#endif

  /*
   * After a fail, we must be able to re-open a port
   */

  // Open port
  port.setPortName(portName);
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  QVERIFY(port.isOpen());

#ifdef Q_OS_UNIX
  // Check about lock file
  fileInfo.setFile(portName);
  QVERIFY(QFile::exists("/tmp/LCK.." + fileInfo.fileName()));
  QVERIFY(QFile::exists("/var/lock/LCK.." + fileInfo.fileName()));
#endif

  // Setup
  port.setConfig(&cfg);
  QVERIFY(port.setup() == mdtAbstractPort::NoError);
  QVERIFY(port.isOpen());

  // Close
  port.close();
  QVERIFY(!port.isOpen());

#ifdef Q_OS_UNIX
  // Check about lock file
  fileInfo.setFile(portName);
  QVERIFY(!QFile::exists("/tmp/LCK.." + fileInfo.fileName()));
  QVERIFY(!QFile::exists("/var/lock/LCK.." + fileInfo.fileName()));
#endif

#ifdef Q_OS_UNIX
  /*
   * It can happen that device file /dev/ttyXXX exists, but that no port
   *  is really avalable.
   * In that case, library had a bug with lockfiles management.
   */

  if(!QFile::exists("/dev/ttyS30")){
    QSKIP("/dev/ttyS30 not found", SkipSingle);
  }

  // Open port
  qDebug() << "TEST: /dev/ttyS30";
  port.setPortName("/dev/ttyS30");
  QVERIFY(port.open() != mdtAbstractPort::NoError);
  QVERIFY(!port.isOpen());

  // Check about lock file
  QVERIFY(!QFile::exists("/tmp/LCK..ttyS30"));
  QVERIFY(!QFile::exists("/var/lock/LCK..ttyS30"));
#endif

  /*
   * Closing by destructor: must not crash
   */
  // Open port
  port.setPortName(portName);
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  QVERIFY(port.isOpen());
  QVERIFY(port.setup() == mdtAbstractPort::NoError);
  QVERIFY(port.isOpen());
  qDebug() << "Test END";
}

void mdtSerialPortTest::startStopTest()
{
  mdtSerialPort sp;
  mdtSerialPortConfig cfg;
  mdtPortReadThread rThd;
  mdtPortWriteThread wThd;
  mdtSerialPortCtlThread ctlThd;

#ifdef Q_OS_UNIX
  sp.setPortName("/dev/ttyS0");
#elif defined Q_OS_WIN
  sp.setPortName("COM1");
#endif
  QVERIFY(sp.open() == mdtAbstractPort::NoError);
  QVERIFY(sp.uartType() != mdtAbstractSerialPort::UT_UNKNOW);
  // Setup
  cfg.setReadTimeout(-1);   // Set infinite timeout
  cfg.setWriteTimeout(-1);   // Set infinite timeout
  sp.setConfig(&cfg);
  QVERIFY(sp.setup() == mdtAbstractPort::NoError);

  // Assign sp to threads
  rThd.setPort(&sp);
  wThd.setPort(&sp);
  ctlThd.setPort(&sp);

  // Chack initial states
  QVERIFY(!rThd.isRunning());
  QVERIFY(!wThd.isRunning());
  QVERIFY(!ctlThd.isRunning());

  // Start read and write threads and check states
  QVERIFY(rThd.start());
  QVERIFY(wThd.start());
  QVERIFY(rThd.isRunning());
  QVERIFY(wThd.isRunning());
  QVERIFY(!ctlThd.isRunning());

  // Start control thread and check states
  QVERIFY(ctlThd.start());
  QVERIFY(ctlThd.isRunning());
  QVERIFY(rThd.isRunning());
  QVERIFY(wThd.isRunning());

  // Wait some time to enshure that each thread does in wait state
  QTest::qWait(100);

  // Stop control thread and check that other still running
  ctlThd.stop();
  QVERIFY(!ctlThd.isRunning());
  QVERIFY(rThd.isRunning());
  QVERIFY(wThd.isRunning());

  // Stop all and check states
  rThd.stop();
  wThd.stop();
  QVERIFY(!rThd.isRunning());
  QVERIFY(!wThd.isRunning());
  QVERIFY(!ctlThd.isRunning());

  // Check several times start/stop of control thread
  QVERIFY(!ctlThd.isRunning());
  qsrand(QDateTime::currentDateTime ().toTime_t ());
  for(int i=0; i<10; i++){
    QVERIFY(ctlThd.start());
    QVERIFY(ctlThd.isRunning());
    QVERIFY(rThd.start());
    QVERIFY(rThd.isRunning());
    QVERIFY(wThd.start());
    QVERIFY(wThd.isRunning());
    QTest::qWait((100.0*(double)qrand()) / RAND_MAX);
    ctlThd.stop();
    QVERIFY(!ctlThd.isRunning());
    rThd.stop();
    QVERIFY(!rThd.isRunning());
    wThd.stop();
    QVERIFY(!wThd.isRunning());
  }
}

void mdtSerialPortTest::ctlSignalsTest()
{
  mdtSerialPort sp;
  mdtSerialPortConfig cfg;
  mdtSerialPortCtlThread ctlThd;

  qDebug() << "* make shure that test terminal is plugged on serial port (ttyS0 , COM1) *";

  // Setup
#ifdef Q_OS_UNIX
  ///QVERIFY(sp.setAttributes("/dev/ttyS0"));
  sp.setPortName("/dev/ttyS0");
#elif defined Q_OS_WIN
  ///QVERIFY(sp.setAttributes("COM1"));
  sp.setPortName("COM1");
#endif
  QVERIFY(sp.open() == mdtAbstractPort::NoError);
  QVERIFY(sp.uartType() != mdtAbstractSerialPort::UT_UNKNOW);
  // Setup
  cfg.setBaudRate(9600);
  cfg.setDataBitsCount(8);
  cfg.setStopBitsCount(1);
  cfg.setParity(mdtSerialPortConfig::NoParity);
  sp.setConfig(&cfg);
  QVERIFY(sp.setup() == mdtAbstractPort::NoError);

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
  mdtSerialPortCtlThread ctlThd;
  mdtFrame *frame;
  QByteArray portName;
  ///QByteArray rxData;
  QString rxData;

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
  sp.setPortName(portName);
  QVERIFY(sp.open() == mdtAbstractPort::NoError);
  QVERIFY(sp.uartType() != mdtAbstractSerialPort::UT_UNKNOW);

  sp.setConfig(&cfg);
  QVERIFY(sp.setup() == mdtAbstractPort::NoError);

  // Assign sp to the RX thread and start
  rxThd.setPort(&sp);
  QVERIFY(rxThd.start());
  QVERIFY(rxThd.isRunning());
  // Assign sp to the TX thread and start
  txThd.setPort(&sp);
  QVERIFY(txThd.start());
  QVERIFY(txThd.isRunning());
  // Assign sp to control thread and start
  ctlThd.setPort(&sp);
  QVERIFY(ctlThd.start());
  QVERIFY(ctlThd.isRunning());

  // Get a frame
  sp.lockMutex();
  QVERIFY(sp.writeFramesPool().size() > 0);
  frame = sp.writeFramesPool().dequeue();
  ///sp.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->clear();
  frame->append(data);
  sp.addFrameToWrite(frame);
  sp.unlockMutex();
  
  /**
  sp.lockMutex();
  sp.writeFrames().enqueue(frame);
  sp.unlockMutex();
  */

  // Wait some time and verify that data was transfered
  QTest::qWait(3*data.size()+100);

  // Stop control thread, and be shure that other can continue
  // This is to check the port's stop system (with signal on Posix, and waitForMultipleObjects on Windows)
  ctlThd.stop();
  QVERIFY(!ctlThd.isRunning());
  QVERIFY(txThd.isRunning());
  QVERIFY(rxThd.isRunning());

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
  ///QVERIFY(rxData == data);
  QCOMPARE(rxData, data);

  // Stop threads and close port
  rxThd.stop();
  txThd.stop();
  sp.close();

  /* Byte per byte write */

  // Setup and open port
  cfg.setBytePerByteWrite(true, 1);
  cfg.setReadQueueSize(data.size()+10); // In RAW mode, each transfered byte will generate a RX frame
  sp.setConfig(&cfg);
  QVERIFY(sp.setup() == mdtAbstractPort::NoError);

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
  ///sp.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->clear();
  frame->append(data);
  sp.addFrameToWrite(frame);
  sp.unlockMutex();
  
  
  /**
  sp.lockMutex();
  sp.writeFrames().enqueue(frame);
  sp.unlockMutex();
  */

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

void mdtSerialPortTest::txRxBinaryTopTest()
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
  cfg.setReadQueueSize(data.size()+10);
  cfg.setReadTimeout(500);
  cfg.setUseReadTimeoutProtocol(true);
  sp.setPortName(portName);
  QVERIFY(sp.open() == mdtAbstractPort::NoError);
  QVERIFY(sp.uartType() != mdtAbstractSerialPort::UT_UNKNOW);
  sp.setConfig(&cfg);
  QVERIFY(sp.setup() == mdtAbstractPort::NoError);

  // Assign sp to the RX thread and start
  rxThd.setPort(&sp);
  QVERIFY(rxThd.start());
  QVERIFY(rxThd.isRunning());
  // Assign sp to the TX thread and start
  txThd.setPort(&sp);
  QVERIFY(txThd.start());
  QVERIFY(txThd.isRunning());

  // Transmit data too fast (interframe < read timeout)
  for(int i=0; i<data.size(); i++){
    sp.lockMutex();
    QVERIFY(sp.writeFramesPool().size() > 0);
    frame = sp.writeFramesPool().dequeue();
    frame->clear();
    frame->append(data.at(i));
    sp.writeFrames().enqueue(frame);
    qDebug() << "TEST , enq 1 frame for write";
    sp.unlockMutex();
  }
  // Wait some time
  QTest::qWait(150*data.size()+100);
  sp.lockMutex();
  // We have a special case for empty data test
  if((data.size() == 1)&&(data.at(0).size() == 0)){
    QVERIFY(sp.readenFrames().size() == 0);
  }else{
    // Here we have max 1 frame (timeout can happen in test because we stop sending data)
    QVERIFY(sp.readenFrames().size() <= 1);
    // Restore received frames to read pool
    while(sp.readenFrames().size() > 0){
      frame = sp.readenFrames().dequeue();
      QVERIFY(frame != 0);
      sp.readFramesPool().enqueue(frame);
    }
  }
  sp.unlockMutex();

  // Update setup with correct interframe time
  //cfg.setWriteInterframeTime(550);
  // Restart threads (make shure that current frame is cleared in threads + update there setup)
  /*
  rxThd.stop();
  txThd.stop();
  sp.flush();
  QVERIFY(rxThd.start());
  QVERIFY(txThd.start());
  */

  // Update setup with correct interframe time
  rxThd.stop();
  txThd.stop();
  sp.close();
  cfg.setWriteInterframeTime(550);
  QVERIFY(sp.open() == mdtAbstractPort::NoError);
  QVERIFY(sp.setup() == mdtAbstractPort::NoError);
  QVERIFY(rxThd.start());
  QVERIFY(txThd.start());

  // Transmit data with correct interframe time
  for(int i=0; i<data.size(); i++){
    sp.lockMutex();
    QVERIFY(sp.writeFramesPool().size() > 0);
    frame = sp.writeFramesPool().dequeue();
    ///sp.unlockMutex();
    frame->clear();
    frame->append(data.at(i));
    sp.addFrameToWrite(frame);
    sp.unlockMutex();
    
    
    qDebug() << "TEST , enq 1 frame for write";
  }
  // Wait some time and verify that data was transfered
  QTest::qWait(600*data.size()+600);
  sp.lockMutex();
  // We have a special case for empty data test
  if((data.size() == 1)&&(data.at(0).size() == 0)){
    QVERIFY(sp.readenFrames().size() == 0);
  }else{
    qDebug() << "data size: " << data.size() << " , sp size: " << sp.readenFrames().size();
    QVERIFY(sp.readenFrames().size() == data.size());
    // Verify each frame
    for(int i=0; i<data.size(); i++){
      frame = sp.readenFrames().dequeue();
      QVERIFY(frame != 0);
      QVERIFY(frame->isComplete());
      // Verify readen data
      qDebug() << "Ref data: " << data.at(i);
      qDebug() << "RX  data: " << *frame;
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

void mdtSerialPortTest::txRxBinaryTopTest_data()
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
  sp.setPortName(portName);
  QVERIFY(sp.open() == mdtAbstractPort::NoError);
  QVERIFY(sp.uartType() != mdtAbstractSerialPort::UT_UNKNOW);

  sp.setConfig(&cfg);
  QVERIFY(sp.setup() == mdtAbstractPort::NoError);

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
  ///sp.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->clear();
  frame->append(data);
  sp.addFrameToWrite(frame);
  sp.unlockMutex();
  
  /**
  sp.lockMutex();
  sp.writeFrames().enqueue(frame);
  sp.unlockMutex();
  */

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
  sp.setConfig(&cfg);
  QVERIFY(sp.setup() == mdtAbstractPort::NoError);

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
  ///sp.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->clear();
  frame->append(data);
  sp.addFrameToWrite(frame);
  sp.unlockMutex();
  
  
  /**
  sp.lockMutex();
  sp.writeFrames().enqueue(frame);
  sp.unlockMutex();
  */

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

void mdtSerialPortTest::flushTest()
{
  mdtSerialPort sp;
  mdtSerialPortConfig cfg;
  mdtPortReadThread rxThd;
  mdtPortWriteThread txThd;
  mdtFrame *frame;
  QByteArray portName;
  QByteArray rxData;

  // Set port name
#ifdef Q_OS_UNIX
  portName = "/dev/ttyS0";
#elif defined Q_OS_WIN
  portName = "COM1";
#endif

  qDebug() << "* make shure that test terminal is plugged on port (" << portName << ") *";

  /*
   * Tests in ASCII tranmission mode
   */

  // Setup
  cfg.setFrameType(mdtFrame::FT_ASCII);
  cfg.setEndOfFrameSeq("*");
  cfg.setReadQueueSize(10);
  cfg.setReadTimeout(-1);
  sp.setPortName(portName);
  QVERIFY(sp.open() == mdtAbstractPort::NoError);
  QVERIFY(sp.uartType() != mdtAbstractSerialPort::UT_UNKNOW);
  sp.setConfig(&cfg);
  QVERIFY(sp.setup() == mdtAbstractPort::NoError);

  // Assign sp to the RX thread and start
  rxThd.setPort(&sp);
  QVERIFY(rxThd.start());
  QVERIFY(rxThd.isRunning());
  // Assign sp to the TX thread and start
  txThd.setPort(&sp);
  QVERIFY(txThd.start());
  QVERIFY(txThd.isRunning());

  /*
   * flushOut() and flushIn() calls
   */

  // Write a frame
  sp.lockMutex();
  QVERIFY(sp.writeFramesPool().size() > 0);
  frame = sp.writeFramesPool().dequeue();
  QVERIFY(frame != 0);
  frame->clear();
  frame->append("frame 01*");
  sp.addFrameToWrite(frame);
  sp.unlockMutex();

  // Wait some time
  QTest::qWait(200);

  // Flush
  sp.flushOut();
  sp.flushIn();

  // Check that we have no incomming frame
  sp.lockMutex();
  QVERIFY(sp.readenFrames().size() < 1);
  sp.unlockMutex();

  // Write a new frame
  sp.lockMutex();
  QVERIFY(sp.writeFramesPool().size() > 0);
  frame = sp.writeFramesPool().dequeue();
  QVERIFY(frame != 0);
  frame->clear();
  frame->append("frame 02*");
  sp.addFrameToWrite(frame);
  sp.unlockMutex();

  // Wait some time
  QTest::qWait(200);

  // Check that exaclty one frame is available
  sp.lockMutex();
  qDebug() << sp.readenFrames().size();
  QVERIFY(sp.readenFrames().size() == 1);
  frame = sp.readenFrames().at(0);
  QCOMPARE(QByteArray(frame->data()), QByteArray("frame 02"));
  sp.unlockMutex();

  /*
   * flush() call
   */

  // Write a frame
  sp.lockMutex();
  QVERIFY(sp.writeFramesPool().size() > 0);
  frame = sp.writeFramesPool().dequeue();
  QVERIFY(frame != 0);
  frame->clear();
  frame->append("frame 03*");
  sp.addFrameToWrite(frame);
  sp.unlockMutex();

  // Wait some time
  QTest::qWait(200);

  // Flush
  sp.flush();

  // Check that we have no incomming frame
  sp.lockMutex();
  QVERIFY(sp.readenFrames().size() < 1);
  sp.unlockMutex();

  // Write a new frame
  sp.lockMutex();
  QVERIFY(sp.writeFramesPool().size() > 0);
  frame = sp.writeFramesPool().dequeue();
  QVERIFY(frame != 0);
  frame->clear();
  frame->append("frame 04*");
  sp.addFrameToWrite(frame);
  sp.unlockMutex();

  // Wait some time
  QTest::qWait(200);

  // Check that exaclty one frame is available
  sp.lockMutex();
  qDebug() << sp.readenFrames().size();
  QVERIFY(sp.readenFrames().size() == 1);
  frame = sp.readenFrames().at(0);
  QCOMPARE(QByteArray(frame->data()), QByteArray("frame 04"));
  sp.unlockMutex();

  // Stop threads and close port
  rxThd.stop();
  txThd.stop();
  sp.close();
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtSerialPortTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
