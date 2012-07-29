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
#include "mdtSerialPortConfigTest.h"
#include "mdtSerialPortConfig.h"
#include "mdtApplication.h"

void mdtSerialPortConfigTest::ConfigTest()
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

void mdtSerialPortConfigTest::ConfigTest_data()
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

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtSerialPortConfigTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
