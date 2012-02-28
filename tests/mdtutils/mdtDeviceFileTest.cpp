
#include "mdtDeviceFileTest.h"
#include "mdtDeviceFile.h"
#include "mdtDeviceFileConfig.h"
#include "mdtDeviceFileReadThread.h"
#include "mdtDeviceFileWriteThread.h"
#include "mdtFrame.h"
#include <QTemporaryFile>
#include <QByteArray>
#include <QString>
#include <QStringList>

#include <QDebug>

#include "linux/mdtUsbtmcFile.h"

void mdtDeviceFileTest::startStopTest()
{
  mdtDeviceFile df;
  mdtDeviceFileConfig cfg;
  mdtDeviceFileReadThread rdThd;
  mdtDeviceFileWriteThread wrThd;
  QTemporaryFile file;

  // Create a temporary file
  QVERIFY(file.open());

  // Setup
  cfg.setInterface(file.fileName());
  QVERIFY(df.open(cfg));

  // Assign df to the threads
  rdThd.setDeviceFile(&df);
  wrThd.setDeviceFile(&df);

  // Initial, threads stopped
  QVERIFY(!rdThd.isRunning());
  QVERIFY(!wrThd.isRunning());

  // Start/stop read thread
  rdThd.start();
  QVERIFY(rdThd.isRunning());
  rdThd.stop();
  QVERIFY(!rdThd.isRunning());
  // Start/stop write thread
  wrThd.start();
  QVERIFY(wrThd.isRunning());
  wrThd.stop();
  QVERIFY(!wrThd.isRunning());

  // Start threads (sequencial)
  rdThd.start();
  QVERIFY(rdThd.isRunning());
  wrThd.start();
  QVERIFY(wrThd.isRunning());
  rdThd.stop();
  QVERIFY(!rdThd.isRunning());
  wrThd.stop();
  QVERIFY(!wrThd.isRunning());

  // Multiple start/stop - seq: rd/wr/rd/wr
  qsrand(QDateTime::currentDateTime ().toTime_t ());
  for(int i=0; i<10; i++){
    rdThd.start();
    QVERIFY(rdThd.isRunning());
    wrThd.start();
    QVERIFY(wrThd.isRunning());
    QTest::qWait((100.0*(double)qrand()) / RAND_MAX);
    rdThd.stop();
    QVERIFY(!rdThd.isRunning());
    wrThd.stop();
    QVERIFY(!wrThd.isRunning());
  }
}

void mdtDeviceFileTest::writeTest()
{
  mdtDeviceFile df;
  mdtDeviceFileConfig cfg;
  mdtDeviceFileWriteThread wrThd;
  mdtFrame *frame;
  QTemporaryFile file;
  QByteArray fileData;

  // Get data set
  QFETCH(QString, data);

  // Create a temporary file
  QVERIFY(file.open());
  file.close();

  // Setup
  cfg.setInterface(file.fileName());
  cfg.setWriteQueueSize(1);
  QVERIFY(df.open(cfg));

  // Assign df to the threads
  wrThd.setDeviceFile(&df);

  // Start
  wrThd.start();
  QVERIFY(wrThd.isRunning());

  // Initial: empty file
  fileData = file.readAll();
  QVERIFY(fileData.size() == 0);

  // Get a frame
  df.lockMutex();
  QVERIFY(df.writeFramesPool().size() == 1);
  frame = df.writeFramesPool().dequeue();
  df.unlockMutex();
  QVERIFY(frame != 0);
  
  // Add some data to frame and commit
  frame->append(data);
  df.lockMutex();
  df.writeFrames().enqueue(frame);
  df.unlockMutex();
  
  // Wait some time and verify that data was written
  QTest::qWait(100);
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);

  // End
  wrThd.stop();
}

void mdtDeviceFileTest::writeTest_data()
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

void mdtDeviceFileTest::readTest()
{
  mdtDeviceFile df;
  mdtDeviceFileConfig cfg;
  mdtDeviceFileReadThread rdThd;
  mdtFrame *frame;
  QTemporaryFile file;
  QByteArray fileData;

  // Get data set
  QFETCH(QString, data);
  QFETCH(QStringList, refData);

  // Create a temporary file
  QVERIFY(file.open());

  // Setup
  cfg.setInterface(file.fileName());
  cfg.setReadQueueSize(10);
  cfg.setEndOfFrameSeq("*");
  QVERIFY(df.open(cfg));

  // Assign df to the threads
  rdThd.setDeviceFile(&df);

  // Start
  rdThd.start();
  QVERIFY(rdThd.isRunning());

  // Write data to file and verify that data was written
  QVERIFY(file.write(data.toAscii()) == data.size());
  QVERIFY(file.flush());
  file.close();
  file.open();
  fileData = file.readAll();
  QVERIFY(fileData == data);

  // Wait some time and verify that data are available
  QTest::qWait(100);
  df.lockMutex();
  QVERIFY(df.readenFrames().size() == refData.size());
  // Verify each readen data frame
  for(int i=0; i<refData.size(); i++){
    frame = df.readenFrames().dequeue();
    QVERIFY(frame != 0);
    // Verify readen data
    QVERIFY(*frame == refData.at(i));
    // Restore frame to pool
    df.readFramesPool().enqueue(frame);
  }
  df.unlockMutex();

  // End
  rdThd.stop();
}

void mdtDeviceFileTest::readTest_data()
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

void mdtDeviceFileTest::writeReadTest()
{
}

void mdtDeviceFileTest::emptyQueueRecoveryTest()
{
  mdtDeviceFile df;
  mdtDeviceFileConfig cfg;
  mdtDeviceFileReadThread rdThd;
  mdtFrame *frame1, *frame2, *frame3;
  QTemporaryFile file;
  QByteArray fileData;

  // Create a temporary file
  QVERIFY(file.open());
  file.close();

  // Setup
  cfg.setInterface(file.fileName());
  cfg.setReadQueueSize(3);
  cfg.setEndOfFrameSeq("*");
  QVERIFY(df.open(cfg));

  // Assign df to the threads
  rdThd.setDeviceFile(&df);

  // Start
  rdThd.start();
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
  df.lockMutex();
  QVERIFY(df.readenFrames().size() == 3);
  df.unlockMutex();

  // Get readen frames
  df.lockMutex();
  frame1 = df.readenFrames().dequeue();
  frame2 = df.readenFrames().dequeue();
  frame3 = df.readenFrames().dequeue();
  df.unlockMutex();
  // Check that frames are not null
  QVERIFY(frame1 != 0);
  QVERIFY(frame2 != 0);
  QVERIFY(frame3 != 0);
  // Verify data
  QVERIFY(*frame1 == "ABCD");
  QVERIFY(*frame2 == "EFGH");
  QVERIFY(*frame3 == "IJKL");

  // Restore frames to pool
  df.lockMutex();
  df.readFramesPool().enqueue(frame1);
  df.readFramesPool().enqueue(frame2);
  df.readFramesPool().enqueue(frame3);
  df.unlockMutex();

  // Wait some time, and check that 1 frame is available
  QTest::qWait(100);
  df.lockMutex();
  QVERIFY(df.readenFrames().size() == 1);
  frame1 = df.readenFrames().dequeue();
  QVERIFY(frame1 != 0);
  QVERIFY(*frame1 == "MNOP");
  df.unlockMutex();
}

void mdtDeviceFileTest::essais()
{
  mdtUsbtmcFile df;
  mdtDeviceFileConfig cfg;
  mdtDeviceFileReadThread rdThd;
  mdtDeviceFileWriteThread wrThd;
  mdtFrame *frame;

  // Setup
  cfg.setInterface("/dev/usbtmc0");
  cfg.setEndOfFrameSeq("\n");
  cfg.setReadFrameSize(512);
  //cfg.setReadMinWaitTime(100);
  //cfg.setWriteMinWaitTime(100);
  cfg.setWriteFrameSize(512);
  QVERIFY(df.open(cfg));

  // Assign df to the threads and start
  rdThd.setDeviceFile(&df);
  wrThd.setDeviceFile(&df);
  wrThd.start();
  rdThd.start();

  /* Send a command
   */

  //df.readOneFrame();
  //QTest::qWait(1000);
  
  // Get a frame
  df.lockMutex();
  QVERIFY(df.writeFramesPool().size() > 0);
  frame = df.writeFramesPool().dequeue();
  df.unlockMutex();
  QVERIFY(frame != 0);

  // Send the command
  frame->clear();
  frame->append("*IDN?\n");
  //frame->append("*CLS\n");
  df.lockMutex();
  df.writeFrames().enqueue(frame);
  df.unlockMutex();
  df.writeOneFrame();

  QTest::qWait(100);
  df.readOneFrame();
  // Wait some time
  QTest::qWait(100);

  // Get answer
  df.lockMutex();
  QVERIFY(df.readenFrames().size() > 0);
  frame = df.readenFrames().dequeue();
  df.unlockMutex();
  QVERIFY(frame != 0);
  qDebug() << "ANS: " << *frame;

}
