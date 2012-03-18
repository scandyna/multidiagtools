
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

#include "mdtFrameCodecModbus.h"
#include "mdtFrameModbusTcp.h"

#include <QDebug>

#include "linux/mdtUsbtmcPort.h"
#include "linux/mdtUsbtmcPortManager.h"

void mdtPortTest::initTestCase()
{
  // The slot tcpServerNewConnection()
  // will be called from QTestLib.
  // We set this flag to false, so the
  // slot knows that nothing is to do
  pvTcpServer = 0;
}

void mdtPortTest::startStopTest()
{
  /// NOTE: provisoire
  return;

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

void mdtPortTest::writeTest()
{
  /// NOTE: provisoire
  return;

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
  QTest::qWait(2*data.size()+100);
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);

  // End
  wrThd.stop();
}

void mdtPortTest::writeTest_data()
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

void mdtPortTest::readTest()
{
  /// NOTE: provisoire
  return;

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

  // Wait some time and verify that data are available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == refData.size());
  // Verify each readen data frame
  for(int i=0; i<refData.size(); i++){
    frame = port.readenFrames().dequeue();
    QVERIFY(frame != 0);
    // Verify readen data
    QVERIFY(*frame == refData.at(i));
    // Restore frame to pool
    port.readFramesPool().enqueue(frame);
  }
  port.unlockMutex();

  // End
  rdThd.stop();
}

void mdtPortTest::readTest_data()
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

void mdtPortTest::emptyQueueRecoveryTest()
{
  /// NOTE: provisoire
  return;

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
  /// NOTE: provisoire
  return;

  mdtPortManager m;
  QTemporaryFile file;

  // Create a temporary file
  QVERIFY(file.open());
  file.close();

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
}

#ifdef Q_OS_UNIX
void mdtPortTest::usbtmcPortTest()
{
  /// NOTE: provisoire
  return;

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
  //mdtFrame *frame;
  mdtFrameModbusTcp *frame;
  
  QByteArray pdu;
  mdtFrameCodecModbus c;

  // Init the tcp server
  pvTcpServer = new QTcpServer;
  QVERIFY(pvTcpServer->listen());
  connect(pvTcpServer, SIGNAL(newConnection()), this, SLOT(tcpServerNewConnection()));
  
  
  // Setup
  cfg.setFrameType(mdtFrame::FT_MODBUS_TCP);
  cfg.setWriteQueueSize(1);
  cfg.setWriteMinWaitTime(100);
  cfg.setReadMinWaitTime(100);
  cfg.setReadTimeout(500);
  cfg.setWriteTimeout(500);
  cfg.setEndOfFrameSeq('\n');
  QVERIFY(s.setAttributes("127.0.0.1:80"));
  QVERIFY(s.open(cfg));

  // Assign socket to the thread
  thd.setPort(&s);

  qDebug() << "main: start threads ...";

  // Start
  QVERIFY(thd.start());
  QVERIFY(thd.isRunning());
  
  //thd.connectToHost("127.0.0.1", pvTcpServer->serverPort());
  thd.connectToHost("127.0.0.1", 1502);
  
  qDebug() << "main: threads started";
  
  //QVERIFY(s.connectToHost("127.0.0.1" , 51168));
  
  // Get a frame
  s.lockMutex();
  QVERIFY(s.writeFramesPool().size() == 1);
  frame = static_cast<mdtFrameModbusTcp*>(s.writeFramesPool().dequeue());
  s.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  //frame->append("-> Hello !\n");
  //pdu = c.encodeReadCoils(1, 5);
  pdu = c.encodeWriteSingleCoil(16, true);
  frame->setPdu(pdu);
  frame->encode();
  for(int i=0; i<frame->size(); i++){
    qDebug() << "frame[" << i << "]: " << hex << (int)frame->at(i);
  }
  s.lockMutex();
  s.writeFrames().enqueue(frame);
  s.unlockMutex();
  thd.beginNewTransaction();

  // Wait some time and verify that data was written
  QTest::qWait(1000);

  // Get a frame
  s.lockMutex();
  QVERIFY(s.writeFramesPool().size() == 1);
  frame = static_cast<mdtFrameModbusTcp*>(s.writeFramesPool().dequeue());
  s.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  //frame->append("-> Second frame !\n");
  pdu = c.encodeReadCoils(20, 12);
  frame->setPdu(pdu);
  frame->encode();
  s.lockMutex();
  s.writeFrames().enqueue(frame);
  s.unlockMutex();
  thd.beginNewTransaction();

  // Wait some time and verify that data was written
  QTest::qWait(1000);

  // Read data
  s.lockMutex();
  QVERIFY(s.readenFrames().size() > 0);
  // Verify each readen data frame
  //frame = static_cast<mdtFrameModbusTcp*>(s.readenFrames().dequeue());
  //QVERIFY(frame != 0);
  //qDebug() << "Readen data: " << *frame;
  // Restore frame to pool
  //s.readFramesPool().enqueue(frame);
  s.unlockMutex();

  // End
  thd.stop();
  
  // free memory and diseable the tcpServerNewConnection() slot
  delete pvTcpServer;
  pvTcpServer = 0;
}

void mdtPortTest::tcpServerNewConnection()
{
  if(pvTcpServer == 0){
    return;
  }
  
  QByteArray data = "<- Hello !!\n";

  QTcpSocket *clientConnection = pvTcpServer->nextPendingConnection();
  connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
  clientConnection->write(data);
  QVERIFY(clientConnection->waitForBytesWritten());
  // Read data
  //QVERIFY(clientConnection->waitForReadyRead());
  qDebug() << "RX: " << clientConnection->readAll();
  clientConnection->disconnectFromHost();
}
