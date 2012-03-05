
#include "mdtSerialPortTest.h"
#include "mdtSerialPortManager.h"
#include "mdtSerialPort.h"
#include "mdtSerialPortConfig.h"
#include "mdtPortThread.h"
#include "mdtPortReadThread.h"
#include "mdtPortWriteThread.h"
#include "mdtSerialPortCtlThread.h"

#include <QTest>
#include <QtGlobal>
#include <QDateTime>
#include <cstdio>

void mdtSerialPortTest::essais()
{
  mdtSerialPortManager m;
  mdtSerialPort sp;
  mdtSerialPortConfig cfg;
  mdtSerialPortCtlThread ctlThd;
  
  m.setSerialPortObj(&sp);
  m.scan();

  cfg.setBaudRate(9600);
  cfg.setDataBitsCount(8);
  cfg.setStopBitsCount(1);
  cfg.setParity(mdtSerialPortConfig::NoParity);
  cfg.enableFlowCtlXonXoff();
  cfg.enableFlowCtlRtsCts();
  QVERIFY(sp.setAttributes("/dev/ttyS0"));
  QVERIFY(sp.open(cfg));

  ctlThd.setPort(&sp);
  
  //connect(this, SIGNAL(testSignal(bool)), &sp, SLOT(setRts(bool)));
  //emit testSignal(true);
/*
  while(1){
    sp.waitEventCtl(2000);
  }


  char buffer[128] = {'\0'};
  qDebug() << "Wait on RX";
  while(sp.waitEventRx(30000) > 0){
    qDebug() << "Read..";
    sp.readData(buffer, 128);
    qDebug() << "Readen: " << buffer;
  }
  
  
  for(int i=0; i<100; i++){
    snprintf(buffer, 128, "Un test %d\n", i);
    qDebug() << "Wait on TX , i:" << i;
    if(sp.waitEventTxReady(3000) > 0){
      qDebug() << "Write...";
      sp.writeData(buffer, strlen(buffer));
    }
  }
*/
}

void mdtSerialPortTest::mdtSerialPortManagerTest()
{
  mdtSerialPortManager m;

  // Verify that scan() function works ..
  m.scan();
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
  qsrand(QDateTime::currentDateTime ().toTime_t ());
  for(int i=0; i<10; i++){
    QVERIFY(ctlThd.start());
    QTest::qWait((100.0*(double)qrand()) / RAND_MAX);
    ctlThd.stop();
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

/// NOTE: pas terminé !!
void mdtSerialPortTest::mdtSerialPortTxRxTest()
{
  mdtSerialPort sp;
  mdtSerialPortConfig cfg;
  mdtPortReadThread rxThd;
  mdtPortWriteThread txThd;
  mdtFrame *f;
  
  qDebug() << "* make shure that test terminal is plugged on serial port (ttyS0 , COM1) *";

  // Open port with default config
  cfg.setReadFrameSize(100);
  cfg.setReadQueueSize(25);
  cfg.setEndOfFrameSeq('*');
#ifdef Q_OS_UNIX
  QVERIFY(sp.setAttributes("/dev/ttyS0"));
#elif defined Q_OS_WIN
  QVERIFY(sp.setAttributes("COM1"));
#endif
  QVERIFY(sp.open(cfg));

  // Assign sp to the RX thread and start
  rxThd.setPort(&sp);
  rxThd.start();
  // Assign sp to the TX thread and start
  txThd.setPort(&sp);
  txThd.start();
  
  /// Note: test sans frame ...
  
  // Get a frame in TX pool WW: dangereurx (pool.size() )
  sp.lockMutex();
  f = sp.writeFramesPool().dequeue();
  f->append("A*AB*ABC*ABCD*ABCDE*0123456789*abcdefghijklmnopqrstuvwxyz0123456789*");
  sp.writeFrames().enqueue(f);
  sp.unlockMutex();

  sp.lockMutex();
  f = sp.writeFramesPool().dequeue();
  f->append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  f->append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  f->append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  f->append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  f->append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  f->append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  f->append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  f->append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  f->append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  f->append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  f->append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  f->append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  f->append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  f->append("*");
  sp.writeFrames().enqueue(f);
  sp.unlockMutex();

  QTest::qWait(1000);
  
  rxThd.stop();
  txThd.stop();
}
