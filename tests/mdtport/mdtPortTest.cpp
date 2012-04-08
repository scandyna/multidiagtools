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
#include "mdtPortTest.h"
#include "mdtPort.h"
#include "mdtPortManager.h"
#include "mdtPortConfig.h"
#include "mdtPortReadThread.h"
#include "mdtPortWriteThread.h"
#include "mdtFrame.h"
#include "mdtTcpSocket.h"
#include "mdtTcpSocketThread.h"
#include <QTemporaryFile>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QTcpSocket>
#include "mdtTcpServer.h"

#include "mdtFrameCodecModbus.h"
#include "mdtFrameModbusTcp.h"

#include <QDebug>

#include "linux/mdtUsbtmcPort.h"
#include "linux/mdtUsbtmcPortManager.h"

void mdtPortTest::initTestCase()
{
}

void mdtPortTest::startStopTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  mdtPortReadThread rdThd;
  mdtPortWriteThread wrThd;
  QTemporaryFile file;

  // Create a temporary file
  QVERIFY(file.open());

  // Setup
  QVERIFY(port.setAttributes(file.fileName()));
  QVERIFY(port.open(cfg));

  // Assign port to the threads
  rdThd.setPort(&port);
  wrThd.setPort(&port);

  // Initial, threads stopped
  QVERIFY(!rdThd.isRunning());
  QVERIFY(!wrThd.isRunning());

  // Start/stop read thread
  QVERIFY(rdThd.start());
  QVERIFY(rdThd.isRunning());
  rdThd.stop();
  QVERIFY(!rdThd.isRunning());
  // Start/stop write thread
  QVERIFY(wrThd.start());
  QVERIFY(wrThd.isRunning());
  wrThd.stop();
  QVERIFY(!wrThd.isRunning());

  // Start threads (sequencial)
  QVERIFY(rdThd.start());
  QVERIFY(rdThd.isRunning());
  QVERIFY(wrThd.start());
  QVERIFY(wrThd.isRunning());
  rdThd.stop();
  QVERIFY(!rdThd.isRunning());
  wrThd.stop();
  QVERIFY(!wrThd.isRunning());

  // Multiple start/stop - seq: rd/wr/rd/wr
  qsrand(QDateTime::currentDateTime ().toTime_t ());
  for(int i=0; i<10; i++){
    QVERIFY(rdThd.start());
    QVERIFY(rdThd.isRunning());
    QVERIFY(wrThd.start());
    QVERIFY(wrThd.isRunning());
    QTest::qWait((100.0*(double)qrand()) / RAND_MAX);
    rdThd.stop();
    QVERIFY(!rdThd.isRunning());
    wrThd.stop();
    QVERIFY(!wrThd.isRunning());
  }
}

void mdtPortTest::writeRawTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  mdtPortWriteThread wrThd;
  mdtFrame *frame;
  QTemporaryFile file;
  QString filePath;
  QByteArray fileData;

  // Get data set
  QFETCH(QString, data);

  // Create a temporary file
  QVERIFY(file.open());
  filePath = file.fileName();
  file.close();

  /* Normal write */

  // Setup
  cfg.setWriteQueueSize(1);
  cfg.setFrameType(mdtFrame::FT_RAW);
  QVERIFY(port.setAttributes(file.fileName()));
  QVERIFY(port.open(cfg));

  // Assign port to the threads
  wrThd.setPort(&port);

  // Start
  QVERIFY(wrThd.start());
  QVERIFY(wrThd.isRunning());

  // Initial: empty file
  fileData = file.readAll();
  QVERIFY(fileData.size() == 0);

  // Get a frame
  port.lockMutex();
  QVERIFY(port.writeFramesPool().size() == 1);
  frame = port.writeFramesPool().dequeue();
  port.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->append(data);
  port.lockMutex();
  port.writeFrames().enqueue(frame);
  port.unlockMutex();

  // Wait some time and verify that data was written
  QTest::qWait(100);
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);

  // Close port
  wrThd.stop();
  port.close();

  // Create a empty temporary file
  QVERIFY(file.open());
  file.remove();
  QVERIFY(!file.exists());
  file.setFileName(filePath);
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == "");
  file.close();

  /* Byte per byte write */

  // Setup
  cfg.setWriteQueueSize(1);
  QVERIFY(port.setAttributes(file.fileName()));
  cfg.setBytePerByteWrite(true, 1);
  QVERIFY(port.open(cfg));

  // Assign port to the threads
  wrThd.setPort(&port);

  // Start
  QVERIFY(wrThd.start());
  QVERIFY(wrThd.isRunning());

  // Initial: empty file
  fileData = file.readAll();
  QVERIFY(fileData.size() == 0);

  // Get a frame
  port.lockMutex();
  QVERIFY(port.writeFramesPool().size() == 1);
  frame = port.writeFramesPool().dequeue();
  port.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->append(data);
  port.lockMutex();
  port.writeFrames().enqueue(frame);
  port.unlockMutex();

  // Wait some time and verify that data was written
  QTest::qWait(3*data.size()+100);
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);

  // End
  wrThd.stop();
}

void mdtPortTest::writeRawTest_data()
{
  QString str;

  QTest::addColumn<QString>("data");

  QTest::newRow("Empty data") << "";
  QTest::newRow("1 char") << "A";
  QTest::newRow("Short len data") << "ABCD";
  QTest::newRow("Middle len data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  // Long data
  str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  str += "abcdefghijklmnopqrstuvwxyz";
  str += "0123456789";
  QTest::newRow("Long data") << str;
  // Very long data
  str = "";
  for(int i=0; i<100; i++){
    str += "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  }
  QTest::newRow("Very long data") << str;
}

void mdtPortTest::writeAsciiTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  mdtPortWriteThread wrThd;
  mdtFrame *frame;
  QTemporaryFile file;
  QString filePath;
  QByteArray fileData;

  // Get data set
  QFETCH(QString, data);

  // Create a temporary file
  QVERIFY(file.open());
  filePath = file.fileName();
  file.close();

  /* Normal write */

  // Setup
  cfg.setWriteQueueSize(1);
  cfg.setFrameType(mdtFrame::FT_ASCII);
  QVERIFY(port.setAttributes(file.fileName()));
  QVERIFY(port.open(cfg));

  // Assign port to the threads
  wrThd.setPort(&port);

  // Start
  QVERIFY(wrThd.start());
  QVERIFY(wrThd.isRunning());

  // Initial: empty file
  fileData = file.readAll();
  QVERIFY(fileData.size() == 0);

  // Get a frame
  port.lockMutex();
  QVERIFY(port.writeFramesPool().size() == 1);
  frame = port.writeFramesPool().dequeue();
  port.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->append(data);
  port.lockMutex();
  port.writeFrames().enqueue(frame);
  port.unlockMutex();

  // Wait some time and verify that data was written
  QTest::qWait(100);
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);

  // Close port
  wrThd.stop();
  port.close();

  // Create a empty temporary file
  QVERIFY(file.open());
  file.remove();
  QVERIFY(!file.exists());
  file.setFileName(filePath);
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == "");
  file.close();

  /* Byte per byte write */

  // Setup
  cfg.setWriteQueueSize(1);
  QVERIFY(port.setAttributes(file.fileName()));
  cfg.setBytePerByteWrite(true, 1);
  QVERIFY(port.open(cfg));

  // Assign port to the threads
  wrThd.setPort(&port);

  // Start
  QVERIFY(wrThd.start());
  QVERIFY(wrThd.isRunning());

  // Initial: empty file
  fileData = file.readAll();
  QVERIFY(fileData.size() == 0);

  // Get a frame
  port.lockMutex();
  QVERIFY(port.writeFramesPool().size() == 1);
  frame = port.writeFramesPool().dequeue();
  port.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->append(data);
  port.lockMutex();
  port.writeFrames().enqueue(frame);
  port.unlockMutex();

  // Wait some time and verify that data was written
  QTest::qWait(3*data.size()+100);
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);

  // End
  wrThd.stop();
}

void mdtPortTest::writeAsciiTest_data()
{
  QString str;

  QTest::addColumn<QString>("data");

  QTest::newRow("Empty data") << "";
  QTest::newRow("1 char") << "A";
  QTest::newRow("Short len data") << "ABCD";
  QTest::newRow("Middle len data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  // Long data
  str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  str += "abcdefghijklmnopqrstuvwxyz";
  str += "0123456789";
  QTest::newRow("Long data") << str;
  // Very long data
  str = "";
  for(int i=0; i<100; i++){
    str += "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  }
  QTest::newRow("Very long data") << str;
}

void mdtPortTest::readRawTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  mdtPortReadThread rdThd;
  mdtFrame *frame;
  QTemporaryFile file;
  QByteArray fileData;

  // Get data set
  QFETCH(QString, data);
  QFETCH(QStringList, refData);

  // Create a temporary file
  QVERIFY(file.open());

  // Setup
  cfg.setReadQueueSize(10);
  cfg.setReadFrameSize(10);
  cfg.setFrameType(mdtFrame::FT_RAW);
  QVERIFY(port.setAttributes(file.fileName()));
  QVERIFY(port.open(cfg));

  // Assign port to the threads
  rdThd.setPort(&port);

  // Start
  QVERIFY(rdThd.start());
  QVERIFY(rdThd.isRunning());

  // Write data to file and verify that data was written
  QVERIFY(file.write(data.toAscii()) == data.size());
  QVERIFY(file.flush());
  file.close();
  file.open();
  fileData = file.readAll();
  QVERIFY(fileData == data);
  file.close();

  // Wait some time and verify that data are available
  QTest::qWait(100);
  port.lockMutex();
  // We have a special case for empty data test
  if((refData.size() == 1)&&(refData.at(0).size() == 0)){
    QVERIFY(port.readenFrames().size() == 0);
  }else{
    QVERIFY(port.readenFrames().size() == refData.size());
    // Verify each readen data frame
    for(int i=0; i<refData.size(); i++){
      frame = port.readenFrames().dequeue();
      QVERIFY(frame != 0);
      QVERIFY(frame->isComplete());
      // Verify readen data
      QVERIFY(*frame == refData.at(i));
      // Restore frame to pool
      port.readFramesPool().enqueue(frame);
    }
  }
  port.unlockMutex();

  // End
  rdThd.stop();
}

void mdtPortTest::readRawTest_data()
{
  // Note: assume that ReadFrameSize is 10
  QStringList refLst;

  QTest::addColumn<QString>("data");
  QTest::addColumn<QStringList>("refData");

  refLst.clear();
  refLst << "";
  QTest::newRow("Empty data") << "" << refLst;
  refLst.clear();
  refLst << "A";
  QTest::newRow("1 char data") << "A" << refLst;
  refLst.clear();
  refLst << "ABCD";
  QTest::newRow("1 frame short data") << "ABCD" << refLst;
  refLst.clear();
  refLst << "ABCDEFGHIJ" << "KLMNOPQRST" << "UVWXYZ";
  QTest::newRow("1 frame middle len data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << refLst;
  refLst.clear();
  refLst << "ABCDEFGHIJ" << "KLMNOPQRST" << "UVWXYZabcd" << "efghijklmn" << "opqrstuvwx" << "yz01234567" << "89";
  QTest::newRow("1 frame long data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" << refLst;
}

void mdtPortTest::readAsciiTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  mdtPortReadThread rdThd;
  mdtFrame *frame;
  QTemporaryFile file;
  QByteArray fileData;

  // Get data set
  QFETCH(QString, data);
  QFETCH(QStringList, refData);

  // Create a temporary file
  QVERIFY(file.open());

  // Setup
  cfg.setReadQueueSize(10);
  cfg.setFrameType(mdtFrame::FT_ASCII);
  cfg.setEndOfFrameSeq("*");
  QVERIFY(port.setAttributes(file.fileName()));
  QVERIFY(port.open(cfg));

  // Assign port to the threads
  rdThd.setPort(&port);

  // Start
  QVERIFY(rdThd.start());
  QVERIFY(rdThd.isRunning());

  // Write data to file and verify that data was written
  QVERIFY(file.write(data.toAscii()) == data.size());
  QVERIFY(file.flush());
  file.close();
  file.open();
  fileData = file.readAll();
  QVERIFY(fileData == data);
  file.close();

  // Wait some time and verify that data are available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == refData.size());
  // Verify each readen data frame
  for(int i=0; i<refData.size(); i++){
    frame = port.readenFrames().dequeue();
    QVERIFY(frame != 0);
    QVERIFY(frame->isComplete());
    // Verify readen data
    QVERIFY(*frame == refData.at(i));
    // Restore frame to pool
    port.readFramesPool().enqueue(frame);
  }
  port.unlockMutex();

  // End
  rdThd.stop();
}

void mdtPortTest::readAsciiTest_data()
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

void mdtPortTest::readInvalidDataAsciiTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  mdtPortReadThread rdThd;
  mdtFrame *frame;
  QTemporaryFile file;
  QString filePath;
  QByteArray fileData;
  QByteArray data;

  // Create a temporary file
  QVERIFY(file.open());
  filePath = file.fileName();

  // Setup
  cfg.setReadFrameSize(10);
  cfg.setReadQueueSize(10);
  cfg.setFrameType(mdtFrame::FT_ASCII);
  cfg.setEndOfFrameSeq("*");
  QVERIFY(port.setAttributes(filePath));

  // Assign port to the threads
  rdThd.setPort(&port);

  /*
   * Data without EOF , size < capacity : must not crash, no frame must be readen
   */

  // Write data to file and verify that data was written
  data = "01234";
  QVERIFY(file.write(data) == data.size());
  QVERIFY(file.flush());
  file.close();
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);
  file.close();

  // Start
  QVERIFY(port.open(cfg));
  QVERIFY(rdThd.start());
  QVERIFY(rdThd.isRunning());

  // Wait some time and verify that no data are available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == 0);
  port.unlockMutex();

  // Stop thread
  rdThd.stop();
  QVERIFY(!rdThd.isRunning());
  port.close();

  /*
   * Data without EOF , size == capacity : must not crash, 1 frame must be readen
   */

  // Write data to file and verify that data was written
  QVERIFY(file.remove());
  file.setFileName(filePath);
  QVERIFY(file.open());
  data = "0123456789";
  QVERIFY(file.write(data) == data.size());
  QVERIFY(file.flush());
  file.close();
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);
  file.close();

  // Start
  QVERIFY(port.open(cfg));
  QVERIFY(rdThd.start());
  QVERIFY(rdThd.isRunning());

  // Wait some time and verify that data are available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == 1);
  frame = port.readenFrames().dequeue();
  QVERIFY(!frame->isComplete());
  // Restore frame
  port.readFramesPool().enqueue(frame);
  port.unlockMutex();

  // Stop thread
  rdThd.stop();
  QVERIFY(!rdThd.isRunning());
  port.close();

  /*
   * Data without EOF , size > capacity : must not crash, 1 frame must be readen
   */

  // Write data to file and verify that data was written
  QVERIFY(file.remove());
  file.setFileName(filePath);
  QVERIFY(file.open());
  data = "0123456789ABCD";
  QVERIFY(file.write(data) == data.size());
  QVERIFY(file.flush());
  file.close();
  file.open();
  fileData = file.readAll();
  QVERIFY(fileData == data);
  file.close();

  // Start
  QVERIFY(port.open(cfg));
  QVERIFY(rdThd.start());
  QVERIFY(rdThd.isRunning());

  // Wait some time and verify that data are available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == 1);
  frame = port.readenFrames().dequeue();
  QVERIFY(!frame->isComplete());
  // Restore frame
  port.readFramesPool().enqueue(frame);
  port.unlockMutex();

  // End
  rdThd.stop();
  QVERIFY(!rdThd.isRunning());
  port.close();
  
  /*
   * Data with EOF , size == capacity : must not crash, 1 frame must be readen
   */

  // Write data to file and verify that data was written
  QVERIFY(file.remove());
  file.setFileName(filePath);
  QVERIFY(file.open());
  data = "012345678*";
  QVERIFY(file.write(data) == data.size());
  QVERIFY(file.flush());
  file.close();
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);
  file.close();

  // Start
  QVERIFY(port.open(cfg));
  QVERIFY(rdThd.start());
  QVERIFY(rdThd.isRunning());

  // Wait some time and verify that data are available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == 1);
  frame = port.readenFrames().dequeue();
  QVERIFY(frame->isComplete());
  QVERIFY(*frame == "012345678");
  // Restore frame
  port.readFramesPool().enqueue(frame);
  port.unlockMutex();

  // Stop thread
  rdThd.stop();
  QVERIFY(!rdThd.isRunning());
  port.close();

}

void mdtPortTest::emptyQueueRecoveryTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  mdtPortReadThread rdThd;
  mdtFrame *frame1, *frame2, *frame3;
  QTemporaryFile file;
  QByteArray fileData;

  // Create a temporary file
  QVERIFY(file.open());
  file.close();

  // Setup
  cfg.setReadQueueSize(3);
  cfg.setFrameType(mdtFrame::FT_ASCII);
  cfg.setEndOfFrameSeq("*");
  QVERIFY(port.setAttributes(file.fileName()));
  QVERIFY(port.open(cfg));

  // Assign port to the threads
  rdThd.setPort(&port);

  // Start
  QVERIFY(rdThd.start());
  QVERIFY(rdThd.isRunning());

  // Write data
  file.open();
  QVERIFY(file.write("ABCD*EFGH*IJKL*MNOP*"));
  QVERIFY(file.flush());
  file.close();
  file.open();
  fileData = file.readAll();
  QVERIFY(fileData == "ABCD*EFGH*IJKL*MNOP*");
  file.close();

  // Wait some time, and check that 3 frames are available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == 3);
  port.unlockMutex();

  // Get readen frames
  port.lockMutex();
  frame1 = port.readenFrames().dequeue();
  frame2 = port.readenFrames().dequeue();
  frame3 = port.readenFrames().dequeue();
  port.unlockMutex();
  // Check that frames are not null
  QVERIFY(frame1 != 0);
  QVERIFY(frame2 != 0);
  QVERIFY(frame3 != 0);
  // Verify data
  QVERIFY(*frame1 == "ABCD");
  QVERIFY(*frame2 == "EFGH");
  QVERIFY(*frame3 == "IJKL");

  // Restore frames to pool
  port.lockMutex();
  port.readFramesPool().enqueue(frame1);
  port.readFramesPool().enqueue(frame2);
  port.readFramesPool().enqueue(frame3);
  port.unlockMutex();

  // Wait some time, and check that 1 frame is available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == 1);
  frame1 = port.readenFrames().dequeue();
  QVERIFY(frame1 != 0);
  QVERIFY(*frame1 == "MNOP");
  port.unlockMutex();
}

void mdtPortTest::portManagerTest()
{
  mdtPortManager m;
  QTemporaryFile file;

  // Create a temporary file
  QVERIFY(file.open());
  file.close();

  // Port setup
  m.setConfig(new mdtPortConfig);
  m.config().setFrameType(mdtFrame::FT_ASCII);

  // Init port manager
  m.setPort(new mdtPort);
  QVERIFY(m.setPortName(file.fileName()));
  QVERIFY(m.openPort());

  // start threads
  QVERIFY(m.start());
  
  // Check the wait function without available data
  QVERIFY(!m.waitReadenFrame());

  // Send some data
  QVERIFY(m.writeData("Test\n"));

  // We must re-open the file, else nothing will be readen
  QTest::qWait(100);
  m.closePort();
  QVERIFY(m.openPort());
  QVERIFY(m.start());
  // Get readen data
  QVERIFY(m.waitReadenFrame());
  QVERIFY(m.lastReadenFrame() == "Test");

  m.closePort();
}

#ifdef Q_OS_UNIX
void mdtPortTest::usbtmcPortTest()
{
  mdtUsbtmcPortManager m;
  QStringList ports;

  qDebug() << "* A USBTMC compatible device must be attached, else test will fail *";

  // Find attached devices
  ports = m.scan();
  QVERIFY(ports.size() > 0);

  // Init port manager
  QVERIFY(m.setPortName(ports.at(0)));
  QVERIFY(m.openPort());

  // start threads
  QVERIFY(m.start());

  // Query without answer
  QVERIFY(m.writeData("*CLS\n", false));

  // Try to query device
  for(int i=0; i<5; i++){
    // Send a command
    QVERIFY(m.writeData("*IDN?\n", true));
    // Wait and read answer
    QVERIFY(m.lastReadenFrame().size() <= 0);
    QVERIFY(m.waitReadenFrame());
    QVERIFY(m.lastReadenFrame().size() > 0);
  }
}
#endif // #ifdef Q_OS_UNIX

void mdtPortTest::tcpSocketTest()
{
  mdtTcpSocket s;
  mdtPortConfig cfg;
  mdtTcpSocketThread thd;
  mdtFrame *frame;
  mdtTcpServer tcpServer;
  QStringList responses;

  // Get data set
  QFETCH(QStringList, queries);
  QFETCH(QStringList, responsesRef);

  // Init the tcp server
  QVERIFY(tcpServer.listen());

  // Setup
  cfg.setFrameType(mdtFrame::FT_ASCII);
  cfg.setReadQueueSize(10);
  cfg.setWriteQueueSize(10);
  cfg.setReadTimeout(500);
  cfg.setWriteTimeout(500);
  cfg.setEndOfFrameSeq('*');
  //QVERIFY(s.setAttributes("127.0.0.1:80"));
  QVERIFY(s.open(cfg));

  // Assign socket to the thread
  thd.setPort(&s);

  // Start
  QVERIFY(thd.start());
  QVERIFY(thd.isRunning());

  // Responses comming from server
  responses = responsesRef;
  // Add the EOF sequence
  for(int i=0; i<responses.size(); i++){
    responses[i].append("*");
  }

  // Set TCP server data
  tcpServer.setResponseData(responses);

  // Init connection
  s.connectToHost("127.0.0.1" , tcpServer.serverPort());

  // Send data to server
  s.lockMutex();
  QVERIFY(s.writeFramesPool().size() >= queries.size());
  s.unlockMutex();
  for(int i=0; i<queries.size(); i++){
    s.lockMutex();
    // Get a frame
    frame = s.writeFramesPool().dequeue();
    QVERIFY(frame != 0);
    // Add some data to frame and commit
    frame->clear();
    frame->append(queries.at(i).toAscii());
    s.writeFrames().enqueue(frame);
    s.unlockMutex();
    s.beginNewTransaction();
  }
  s.unlockMutex();

  // Wait some time and verify that data was exchanged
  QTest::qWait(100);

  // Check received data
  s.lockMutex();
  QVERIFY(s.readenFrames().size() == responses.size());
  for(int i=0; i<responses.size(); i++){
    // Get a frame
    frame = s.readenFrames().dequeue();
    QVERIFY(frame != 0);
    QVERIFY(*frame == responsesRef.at(i));
  }
  s.unlockMutex();

  // End
  thd.stop();
}

void mdtPortTest::tcpSocketTest_data()
{
  QStringList q;
  QStringList r;

  QTest::addColumn<QStringList>("queries");
  QTest::addColumn<QStringList>("responsesRef");

  q.clear();
  r.clear();
  q << "";
  r << "";
  QTest::newRow("Empty data") << q << r;

  q.clear();
  r.clear();
  q << "A";
  r << "1";
  QTest::newRow("1 char data") << q << r;

  q.clear();
  r.clear();
  q << "ABCD";
  r << "1234";
  QTest::newRow("1 frame short data") << q << r;

  q.clear();
  r.clear();
  q << "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  r << "1234567890";
  QTest::newRow("1 frame middle len data") << q << r;

  q.clear();
  r.clear();
  q << "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  r << "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  QTest::newRow("1 frame long data") << q << r;

  q.clear();
  r.clear();
  q << "A" << "B" << "C" << "D";
  r << "Z" << "8" << "G" << "4";
  QTest::newRow("Multi frame short data") << q << r;

  q.clear();
  r.clear();
  q << "ABCD" << "1234" << "abcd";
  r << "9876" << "FGHI" << "askjdf";
  QTest::newRow("Multi frame middle len data") << q << r;

  q.clear();
  r.clear();
  q << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << "0123456789" << "abcdefghijklmnopqrstuvwxyz";
  r << "0123456789" << "ABCDEFGHIJK0055LMNOPQRSTUVWXYZ" << "abcdefghijklmnopqrstuvwxyz8545";
  QTest::newRow("Multi frame long data") << q << r;
}

void mdtPortTest::tcpSocketRreadInvalidDataTest()
{
  mdtTcpSocket s;
  mdtPortConfig cfg;
  mdtTcpSocketThread thd;
  mdtFrame *frame;
  mdtTcpServer tcpServer;
  QStringList responses;

  // Init the tcp server
  QVERIFY(tcpServer.listen());

  // Setup
  cfg.setFrameType(mdtFrame::FT_ASCII);
  cfg.setReadQueueSize(10);
  cfg.setReadFrameSize(10);
  cfg.setWriteQueueSize(10);
  cfg.setReadTimeout(500);
  cfg.setWriteTimeout(500);
  cfg.setEndOfFrameSeq('*');
  QVERIFY(s.open(cfg));

  // Assign socket to the thread
  thd.setPort(&s);

  // Start
  QVERIFY(thd.start());
  QVERIFY(thd.isRunning());

  /*
   * Data without EOF , size < capacity : must not crash, no frame must be readen
   */

  responses.clear();
  responses << "ABCDE";
  QVERIFY(responses.at(0).size() < cfg.readFrameSize());

  // Set TCP server data
  tcpServer.setResponseData(responses);
  QVERIFY(tcpServer.isListening());

  // Init connection
  s.connectToHost("127.0.0.1" , tcpServer.serverPort());

  // Wait some time and verify that data was exchanged
  QTest::qWait(100);

  // Check received data
  s.lockMutex();
  QVERIFY(s.readenFrames().size() == 0);
  s.unlockMutex();

  // Wait some time to enshure TCP server has finished ...
  tcpServer.close();
  QTest::qWait(100);

  // Restart
  thd.stop();
  QVERIFY(!thd.isRunning());
  QVERIFY(thd.start());
  QVERIFY(thd.isRunning());

  /*
   * Data with EOF , size == capacity : must not crash, 1 frame must be readen
   */

  responses.clear();
  responses << "0123456789";
  QVERIFY(responses.at(0).size() == cfg.readFrameSize());

  // Set TCP server data
  tcpServer.setResponseData(responses);
  QVERIFY(tcpServer.listen());

  // Init connection
  s.connectToHost("127.0.0.1" , tcpServer.serverPort());

  // Wait some time and verify that data was exchanged
  QTest::qWait(100);

  // Check received data
  s.lockMutex();
  QVERIFY(s.readenFrames().size() == 1);
  // Get a frame
  frame = s.readenFrames().dequeue();
  QVERIFY(frame != 0);
  QVERIFY(*frame == responses.at(0));
  // Restore frame
  s.readFramesPool().enqueue(frame);
  s.unlockMutex();

  // Wait some time to enshure TCP server has finished ...
  tcpServer.close();
  QTest::qWait(100);

  // Restart
  thd.stop();
  QVERIFY(!thd.isRunning());
  QVERIFY(thd.start());
  QVERIFY(thd.isRunning());

  /*
   * Data with EOF , size > capacity (but < 2*capacity) : must not crash, 1 frame must be readen
   */

  responses.clear();
  responses << "0123456789abcdefg";
  QVERIFY(responses.at(0).size() > cfg.readFrameSize());
  QVERIFY(responses.at(0).size() < 2*cfg.readFrameSize());

  // Set TCP server data
  tcpServer.setResponseData(responses);
  QVERIFY(tcpServer.listen());

  // Init connection
  s.connectToHost("127.0.0.1" , tcpServer.serverPort());

  // Wait some time and verify that data was exchanged
  QTest::qWait(100);

  // Check received data
  s.lockMutex();
  QVERIFY(s.readenFrames().size() == 1);
  // Get a frame
  frame = s.readenFrames().dequeue();
  QVERIFY(frame != 0);
  QVERIFY(*frame == "0123456789");
  // Restore frame
  s.readFramesPool().enqueue(frame);
  s.unlockMutex();

  // End
  thd.stop();
}
