/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtUsbPortTest.h"
#include "mdtApplication.h"
#include "mdtUsbPort.h"
#include "mdtUsbDeviceDescriptor.h"
#include "mdtUsbConfigDescriptor.h"
#include "mdtUsbInterfaceDescriptor.h"
#include "mdtUsbEndpointDescriptor.h"
#include "mdtUsbPortThread.h"
#include "mdtUsbtmcPortThread.h"
#include "mdtPortConfig.h"
#include "mdtFrame.h"
#include "mdtFrameCodecK8055.h"
#include "mdtFrameUsbTmc.h"

#include "mdtUsbDeviceList.h"

// New USBTMC API
#include "mdtUsbtmcFrame.h"

#include <QByteArray>
#include <mutex>

#include <QDebug>

/*
 * Test classes to compare standard calls vs virtual calls
 */

class StandardCallTestClass
{
 public:
  int processData(int m, int n)
  {
    return m + n;
  }
};

class VirtualCallBaseTestClass
{
 public:
  virtual int processData(int m, int n)
  {
    return m + n;
  }
};

class VirtualCallTestClass : public VirtualCallBaseTestClass
{
 public:
  int processData(int m, int n)
  {
    return m + n;
  }
};

/*
 * Helper methods for alloc/free vs mutex benchamrks
 */

void mdtUsbPortTest::fillBuffer ( unsigned char* buffer, int bSize )
{
  for(int i = 0; i < bSize; ++i){
    buffer[i] = i;
  }
}

/*
 * Benchmarks
 */

void mdtUsbPortTest::basicAllocFreeBenchMarks()
{
  unsigned char *buffer;
  int N = 10;
  int bSize = 100;
  int i;

  QBENCHMARK{
    for(i = 0; i < N; ++i){
      buffer = new unsigned char[bSize];
      fillBuffer(buffer, bSize);
      delete[] buffer;
    }
  }
}

void mdtUsbPortTest::basicLockUnlockBenchmark()
{
  unsigned char *buffer;
  std::mutex mutex;
  int N = 10;
  int bSize = 100;
  int i;

  buffer = new unsigned char[bSize];
  QBENCHMARK{
    for(i = 0; i < N; ++i){
      std::lock_guard<std::mutex> lock(mutex);
      fillBuffer(buffer, bSize);
    }
  }
  delete[] buffer;
}

void mdtUsbPortTest::standardCallBenchmark()
{
  StandardCallTestClass *c = new StandardCallTestClass;
  int r;

  QBENCHMARK{
    r = c->processData(2, 5);
  }
  // Do something with r
  if(r > 5){
    r = 2;
  }
}

void mdtUsbPortTest::virtualCallBenchmark()
{
  VirtualCallBaseTestClass *c = new VirtualCallTestClass;
  int r;

  QBENCHMARK{
    r = c->processData(2, 5);
  }
  // Do something with r
  if(r > 5){
    r = 2;
  }
}

/*
 * USB tests
 */

void mdtUsbPortTest::usbEndpointDescriptorTest()
{
  QSKIP("Not implemented yet", SkipAll);
}

void mdtUsbPortTest::deviceListTest()
{
  libusb_context *usbCtx = 0;
  int ret;

  // Init lubusb
  ret = libusb_init(&usbCtx);
  QVERIFY(ret == 0);

  mdtUsbDeviceList devList(usbCtx);
  QVERIFY(devList.scan());
  
  libusb_device_handle *handle;
  handle = devList.openDevice(0x0957, 0x4d18, "MY51040034");
  ///handle = devList.openDevice(0x046d, 0xc512);
  
  // Free ressources
  if(handle != 0){
    libusb_close(handle);
  }
  libusb_exit(usbCtx);
}


/*
 * USBTMC tests
 */

void mdtUsbPortTest::usbtmcFrameTest()
{
  mdtUsbtmcFrame f;

  // Check initial values
  QCOMPARE(f.capacity(), 4096);
  QCOMPARE(f.bufferLength(), 12);
  for(int i = 0; i < f.bufferLength(); ++i){
    QCOMPARE(f.buffer()[i], (uint8_t)0);
  }
  QCOMPARE(f.msgID(), mdtUsbtmcFrame::msgId_t::MSG_ZERO);
  /*
   * DEV_DEP_MSG_OUT encoding
   */
  // Check header encoding for DEV_DEP_MSG_OUT - case 1 (no explicit data)
  f.setMsgID(mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_OUT);
  f.setbTag(0x5B);
  QCOMPARE(f.bufferLength(), 12);
  QCOMPARE(f.buffer()[0], (uint8_t)1);      // MsgID
  QCOMPARE(f.buffer()[1], (uint8_t)0x5B);   // bTag
  QCOMPARE(f.buffer()[2], (uint8_t)0xA4);   // bTagInverse
  QCOMPARE(f.buffer()[3], (uint8_t)0);      // Reserved
  QCOMPARE(f.buffer()[4], (uint8_t)0);      // TransferSize, LLSB
  QCOMPARE(f.buffer()[5], (uint8_t)0);      // TransferSize, LSB
  QCOMPARE(f.buffer()[6], (uint8_t)0);      // TransferSize, MSB
  QCOMPARE(f.buffer()[7], (uint8_t)0);      // TransferSize, MMSB
  QCOMPARE(f.buffer()[8], (uint8_t)0);      // bmTransferAttributes (EOM not set)
  QCOMPARE(f.buffer()[9], (uint8_t)0);      // Reserved
  QCOMPARE(f.buffer()[10], (uint8_t)0);     // Reserved
  QCOMPARE(f.buffer()[11], (uint8_t)0);     // Reserved
  QCOMPARE(f.msgID(), mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_OUT);
  QCOMPARE(f.bTag(), (uint8_t)0x5B);
  QCOMPARE(f.bTagInverse(), (uint8_t)0xA4);
  QCOMPARE((int)f.transferSize(), 0);
  // Check correct encoding for DEV_DEP_MSG_OUT - case 2 (explicit data)
  f.setMsgID(mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_OUT);
  f.setbTag(0x01);
  f.setData("*IDN?\n");
  QCOMPARE(f.bufferLength(), 20);           // 12 (header) + 6 (data) + 2 (alignment bytes)
  QCOMPARE(f.buffer()[0], (uint8_t)1);      // MsgID
  QCOMPARE(f.buffer()[1], (uint8_t)0x01);   // bTag
  QCOMPARE(f.buffer()[2], (uint8_t)0xFE);   // bTagInverse
  QCOMPARE(f.buffer()[3], (uint8_t)0);      // Reserved
  QCOMPARE(f.buffer()[4], (uint8_t)6);      // TransferSize, LLSB
  QCOMPARE(f.buffer()[5], (uint8_t)0);      // TransferSize, LSB
  QCOMPARE(f.buffer()[6], (uint8_t)0);      // TransferSize, MSB
  QCOMPARE(f.buffer()[7], (uint8_t)0);      // TransferSize, MMSB
  QCOMPARE(f.buffer()[8], (uint8_t)0);      // bmTransferAttributes (EOM not set)
  QCOMPARE(f.buffer()[9], (uint8_t)0);      // Reserved
  QCOMPARE(f.buffer()[10], (uint8_t)0);     // Reserved
  QCOMPARE(f.buffer()[11], (uint8_t)0);     // Reserved
  QCOMPARE((char)f.buffer()[12], '*');      // Data
  QCOMPARE((char)f.buffer()[13], 'I');      // Data
  QCOMPARE((char)f.buffer()[14], 'D');      // Data
  QCOMPARE((char)f.buffer()[15], 'N');      // Data
  QCOMPARE((char)f.buffer()[16], '?');      // Data
  QCOMPARE((char)f.buffer()[17], '\n');     // Data
  QCOMPARE((int)f.buffer()[18], 0);         // Alignment byte
  QCOMPARE((int)f.buffer()[19], 0);         // Alignment byte
  QCOMPARE(f.msgID(), mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_OUT);
  QCOMPARE(f.bTag(), (uint8_t)0x01);
  QCOMPARE(f.bTagInverse(), (uint8_t)0xFE);
  QCOMPARE((int)f.transferSize(), 6);
  QCOMPARE(f.data(), QByteArray("*IDN?\n"));
  // Check Alignment bytes
  f.setData("A");
  QCOMPARE((int)f.transferSize(), 1);
  QCOMPARE(f.bufferLength(), 16);
  QCOMPARE((char)f.buffer()[12], 'A');  // Data
  QCOMPARE((int)f.buffer()[13], 0);     // Alignment byte
  QCOMPARE((int)f.buffer()[14], 0);     // Alignment byte
  QCOMPARE((int)f.buffer()[15], 0);     // Alignment byte
  f.setData("12");
  QCOMPARE((int)f.transferSize(), 2);
  QCOMPARE(f.bufferLength(), 16);
  QCOMPARE((char)f.buffer()[12], '1');  // Data
  QCOMPARE((char)f.buffer()[13], '2');  // Data
  QCOMPARE((int)f.buffer()[14], 0);     // Alignment byte
  QCOMPARE((int)f.buffer()[15], 0);     // Alignment byte
  f.setData("ABC");
  QCOMPARE((int)f.transferSize(), 3);
  QCOMPARE(f.bufferLength(), 16);
  QCOMPARE((char)f.buffer()[12], 'A');  // Data
  QCOMPARE((char)f.buffer()[13], 'B');  // Data
  QCOMPARE((char)f.buffer()[14], 'C');  // Data
  QCOMPARE((int)f.buffer()[15], 0);     // Alignment byte
  f.setData("1234");
  QCOMPARE((int)f.transferSize(), 4);
  QCOMPARE(f.bufferLength(), 16);
  QCOMPARE((char)f.buffer()[12], '1');  // Data
  QCOMPARE((char)f.buffer()[13], '2');  // Data
  QCOMPARE((char)f.buffer()[14], '3');  // Data
  QCOMPARE((char)f.buffer()[15], '4');  // Data
  f.setData("A");
  QCOMPARE((int)f.transferSize(), 1);
  QCOMPARE(f.bufferLength(), 16);
  QCOMPARE((char)f.buffer()[12], 'A');  // Data
  QCOMPARE((int)f.buffer()[13], 0);     // Alignment byte
  QCOMPARE((int)f.buffer()[14], 0);     // Alignment byte
  QCOMPARE((int)f.buffer()[15], 0);     // Alignment byte
  // Check clear
  f.clear();
  QCOMPARE(f.capacity(), 4096);
  QCOMPARE(f.bufferLength(), 12);
  for(int i = 0; i < f.bufferLength(); ++i){
    QCOMPARE(f.buffer()[i], (uint8_t)0);
  }
  QCOMPARE(f.msgID(), mdtUsbtmcFrame::msgId_t::MSG_ZERO);
  /*
   * DEV_DEP_MSG_IN encoding
   */
  // Check correct encoding for DEV_DEP_MSG_IN - case 1: TermChar not used
  f.setMsgID(mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_IN);
  f.setbTag(0x02);
  f.setTransferSize(0x0987);
  f.resetTermChar();
  QCOMPARE(f.bufferLength(), 12);           // 12 (header)
  QCOMPARE(f.buffer()[0], (uint8_t)2);      // MsgID
  QCOMPARE(f.buffer()[1], (uint8_t)0x02);   // bTag
  QCOMPARE(f.buffer()[2], (uint8_t)0xFD);   // bTagInverse
  QCOMPARE(f.buffer()[3], (uint8_t)0);      // Reserved
  QCOMPARE(f.buffer()[4], (uint8_t)0x87);   // TransferSize, LLSB
  QCOMPARE(f.buffer()[5], (uint8_t)0x09);   // TransferSize, LSB
  QCOMPARE(f.buffer()[6], (uint8_t)0);      // TransferSize, MSB
  QCOMPARE(f.buffer()[7], (uint8_t)0);      // TransferSize, MMSB
  QCOMPARE(f.buffer()[8], (uint8_t)0);      // bmTransferAttributes (TermCharEnabled not set)
  QCOMPARE(f.buffer()[9], (uint8_t)0);      // TermChar
  QCOMPARE(f.buffer()[10], (uint8_t)0);     // Reserved
  QCOMPARE(f.buffer()[11], (uint8_t)0);     // Reserved
  QCOMPARE(f.msgID(), mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_IN);
  QCOMPARE(f.bTag(), (uint8_t)0x02);
  QCOMPARE(f.bTagInverse(), (uint8_t)0xFD);
  QCOMPARE((int)f.transferSize(), 0x0987);
  // Check correct encoding for DEV_DEP_MSG_IN - case 2: TermChar: \n
  f.setMsgID(mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_IN);
  f.setbTag(0x03);
  f.setTransferSize(0x0485);
  f.setTermChar('\n');
  QCOMPARE(f.bufferLength(), 12);           // 12 (header)
  QCOMPARE(f.buffer()[0], (uint8_t)2);      // MsgID
  QCOMPARE(f.buffer()[1], (uint8_t)0x03);   // bTag
  QCOMPARE(f.buffer()[2], (uint8_t)0xFC);   // bTagInverse
  QCOMPARE(f.buffer()[3], (uint8_t)0);      // Reserved
  QCOMPARE(f.buffer()[4], (uint8_t)0x85);   // TransferSize, LLSB
  QCOMPARE(f.buffer()[5], (uint8_t)0x04);   // TransferSize, LSB
  QCOMPARE(f.buffer()[6], (uint8_t)0);      // TransferSize, MSB
  QCOMPARE(f.buffer()[7], (uint8_t)0);      // TransferSize, MMSB
  QCOMPARE(f.buffer()[8], (uint8_t)0b00000010); // bmTransferAttributes (TermCharEnabled set)
  QCOMPARE((char)f.buffer()[9], '\n');      // TermChar
  QCOMPARE(f.buffer()[10], (uint8_t)0);     // Reserved
  QCOMPARE(f.buffer()[11], (uint8_t)0);     // Reserved
  QCOMPARE(f.msgID(), mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_IN);
  QCOMPARE(f.bTag(), (uint8_t)0x03);
  QCOMPARE(f.bTagInverse(), (uint8_t)0xFC);
  QCOMPARE((int)f.transferSize(), 0x0485);
  // Check header encoding for DEV_DEP_MSG_OUT - case 3: big TransferSize, TermChar not set
  mdtUsbtmcFrame f2(0x20000);
  f2.setMsgID(mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_IN);
  f2.setbTag(0xEF);
  f2.setTransferSize(0x12345);
  QCOMPARE(f2.bufferLength(), 12);            // 12 (header)
  QCOMPARE(f2.buffer()[0], (uint8_t)2);       // MsgID
  QCOMPARE(f2.buffer()[1], (uint8_t)0xEF);    // bTag
  QCOMPARE(f2.buffer()[2], (uint8_t)0x10);    // bTagInverse
  QCOMPARE(f2.buffer()[3], (uint8_t)0);       // Reserved
  QCOMPARE(f2.buffer()[4], (uint8_t)0x45);    // TransferSize, LLSB
  QCOMPARE(f2.buffer()[5], (uint8_t)0x23);    // TransferSize, LSB
  QCOMPARE(f2.buffer()[6], (uint8_t)0x01);    // TransferSize, MSB
  QCOMPARE(f2.buffer()[7], (uint8_t)0);       // TransferSize, MMSB
  QCOMPARE(f2.buffer()[8], (uint8_t)0);       // bmTransferAttributes (TermCharEnabled not set)
  QCOMPARE(f2.buffer()[9], (uint8_t)0);       // TermChar
  QCOMPARE(f2.buffer()[10], (uint8_t)0);      // Reserved
  QCOMPARE(f2.buffer()[11], (uint8_t)0);      // Reserved
  QCOMPARE(f2.msgID(), mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_IN);
  QCOMPARE(f2.bTag(), (uint8_t)0xEF);
  QCOMPARE(f2.bTagInverse(), (uint8_t)0x10);
  QCOMPARE((int)f2.transferSize(), 0x12345);
  // Rset EOM and check
  f2.setEOM(false);
  QCOMPARE(f2.buffer()[8], (uint8_t)0b00000000); // bmTransferAttributes (EOM not set)
  /*
   * DEV_DEP_MSG_IN decoding - case 1: we not check data, TermChar was set in request, is supported and set in this frame by device. EOM set
   */
  f.clear();
  QCOMPARE(f.capacity(), 4096);
  QCOMPARE((int)f.transferSize(), 0);
  QCOMPARE(f.EOMset(), false);
  QCOMPARE(f.TermCharset(), false);
  f.buffer()[0] = 2;      // MsgID
  f.buffer()[1] = 0x1F;   // bTag
  f.buffer()[2] = 0xE0;   // bTagInverse
  f.buffer()[3] = 0;      // Reserved
  f.buffer()[4] = 0x56;   // TransferSize, LLSB
  f.buffer()[5] = 0x03;   // TransferSize, LSB
  f.buffer()[6] = 0;      // TransferSize, MSB
  f.buffer()[7] = 0;      // TransferSize, MMSB
  f.buffer()[8] = 0b00000011; // bmTransferAttributes: TermChar set, EOM set
  f.buffer()[9] = 0;      // Reserved
  f.buffer()[10] = 0;     // Reserved
  f.buffer()[11] = 0;     // Reserved
  QCOMPARE(f2.msgID(), mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_IN);
  QCOMPARE(f.bTag(), (uint8_t)0x1F);
  QCOMPARE(f.bTagInverse(), (uint8_t)0xE0);
  QCOMPARE((int)f.transferSize(), 0x0356);
  QCOMPARE(f.EOMset(), true);
  QCOMPARE(f.TermCharset(), true);
  QVERIFY(!f.isComplete(25 + 12));
  QVERIFY(f.isComplete(0x0356 + 12));
  QVERIFY(f.isComplete(0x0357 + 12));
  /*
   * DEV_DEP_MSG_IN decoding - case 2: put data, TermChar not used, EOM set
   */
  f.clear();
  QCOMPARE(f.capacity(), 4096);
  QCOMPARE((int)f.transferSize(), 0);
  QCOMPARE(f.EOMset(), false);
  QCOMPARE(f.TermCharset(), false);
  f.buffer()[0] = 2;      // MsgID
  f.buffer()[1] = 0x2F;   // bTag
  f.buffer()[2] = 0xD0;   // bTagInverse
  f.buffer()[3] = 0;      // Reserved
  f.buffer()[4] = 10;     // TransferSize, LLSB
  f.buffer()[5] = 0;      // TransferSize, LSB
  f.buffer()[6] = 0;      // TransferSize, MSB
  f.buffer()[7] = 0;      // TransferSize, MMSB
  f.buffer()[8] = 0b00000001; // bmTransferAttributes: TermChar not set (say we dindn't request it), EOM set
  f.buffer()[9] = 0;      // Reserved
  f.buffer()[10] = 0;     // Reserved
  f.buffer()[11] = 0;     // Reserved
  f.buffer()[12] = (uint8_t)'A';  // Data
  f.buffer()[13] = (uint8_t)'B';  // Data
  f.buffer()[14] = (uint8_t)'C';  // Data
  f.buffer()[15] = (uint8_t)'D';  // Data
  f.buffer()[16] = (uint8_t)',';  // Data
  f.buffer()[17] = (uint8_t)'1';  // Data
  f.buffer()[18] = (uint8_t)'2';  // Data
  f.buffer()[19] = (uint8_t)'3';  // Data
  f.buffer()[20] = (uint8_t)'4';  // Data
  f.buffer()[21] = (uint8_t)'\n'; // Data
  QCOMPARE(f2.msgID(), mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_IN);
  QCOMPARE(f.bTag(), (uint8_t)0x2F);
  QCOMPARE(f.bTagInverse(), (uint8_t)0xD0);
  QCOMPARE((int)f.transferSize(), 10);
  QCOMPARE(f.EOMset(), true);
  QCOMPARE(f.TermCharset(), false);
  QVERIFY(!f.isComplete(9 + 12));
  QVERIFY(f.isComplete(10 + 12));
  QVERIFY(f.isComplete(11 + 12));
  QCOMPARE(f.data(), QByteArray("ABCD,1234\n"));


  /*
   * TODO: VENDOR_SPECIFIC_OUT / VENDOR_SPECIFIC_IN
   */
}

void mdtUsbPortTest::usbtmcFrameBenchmark()
{
  mdtUsbtmcFrame f;
  QByteArray query = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  QByteArray result;
  mdtUsbtmcFrame::msgId_t msgID;
  uint8_t bTag;
  uint8_t bTagInverse;
  uint32_t transferSize;

  QBENCHMARK{
    f.clear();
    f.setMsgID(mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_OUT);
    f.setbTag(0x5B);
    f.setData(query);
    msgID = f.msgID();
    bTag = f.bTag();
    bTagInverse = f.bTagInverse();
    transferSize = f.transferSize();
    result = f.data();
  }
  QCOMPARE((int)msgID, 1);
  QCOMPARE(bTag, (uint8_t)0x5B);
  QCOMPARE(bTagInverse, (uint8_t)0xA4);
  QCOMPARE(transferSize, (uint32_t)26);
  QCOMPARE(result, QByteArray("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
}


/*
 * Tests
 */

void mdtUsbPortTest::vellemanK8055Test()
{
  mdtFrame *f;
  QByteArray data;
  mdtUsbPort port;
  mdtUsbPortThread thd;
  mdtPortConfig cfg;
  mdtFrameCodecK8055 codec;

  // Setup
  cfg.setWriteFrameSize(8);
  cfg.setWriteQueueSize(3);
  cfg.setReadFrameSize(8);
  cfg.setReadQueueSize(10);
  cfg.setFrameType(mdtFrame::FT_RAW);
  port.setConfig(&cfg);
  thd.setPort(&port);

  // Try to open
  port.setPortName("VID=0x10cf:PID=0x5500");
  if(port.open() != mdtAbstractPort::NoError){
    port.setPortName("VID=0x10cf:PID=0x5501");
    if(port.open() != mdtAbstractPort::NoError){
      port.setPortName("VID=0x10cf:PID=0x5502");
      if(port.open() != mdtAbstractPort::NoError){
        port.setPortName("VID=0x10cf:PID=0x5503");
        if(port.open() != mdtAbstractPort::NoError){
          QSKIP("No Velleman k8055 attached, or other error", SkipAll);
        }
      }
    }
  }
  QVERIFY(port.setup() == mdtAbstractPort::NoError);

  // Start thread
  thd.start();
  thd.waitReady();

  // Open/close test
  thd.stop();
  thd.waitFinished();
  port.close();
  randomValueInit();
  for(int i=0; i<10; i++){
    QVERIFY(port.open() == mdtAbstractPort::NoError);
    QVERIFY(port.setup() == mdtAbstractPort::NoError);
    QTest::qWait(randomValue(0, 100));
    port.close();
  }
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  QVERIFY(port.setup() == mdtAbstractPort::NoError);
  thd.start();
  thd.waitReady();

  // Write test
  codec.setDigitalOut(1, true);
  codec.setDigitalOut(2, false);
  codec.setDigitalOut(3, true);
  codec.setDigitalOut(4, false);
  codec.setDigitalOut(5, true);
  codec.setDigitalOut(6, false);
  codec.setDigitalOut(7, true);
  codec.setDigitalOut(8, false);
  codec.setAnalogOut(1, 255);
  codec.setAnalogOut(2, 0);
  data = codec.encodeSetOutputs();
  port.lockMutex();
  QVERIFY(port.writeFramesPool().size() > 0);
  f = port.writeFramesPool().dequeue();
  QVERIFY(f != 0);
  f->clear();
  f->clearSub();
  f->append(data);
  f->setWaitAnAnswer(true);
  QCOMPARE(f->size(), 8);
  port.addFrameToWrite(f);
  port.unlockMutex();
  QTest::qWait(100);
  QCOMPARE(port.lastErrors().size(), 0);
  // Read
  port.lockMutex();
  QVERIFY(port.readenFrames().size() > 0);
  f = port.readenFrames().dequeue();
  data.clear();
  data.append(f->data(), f->size());
  port.readFramesPool().enqueue(f);
  port.unlockMutex();
  QCOMPARE(data.size(), 8);
  QVERIFY(codec.decode(data));
  QCOMPARE(codec.values().size(), 9);

  qDebug() << "Values: " << codec.values();
  // End
  thd.stop();
  thd.waitFinished();
}

void mdtUsbPortTest::agilentDso1000Test()
{
  mdtFrameUsbTmc *uf;
  mdtUsbPort port;
  mdtUsbtmcPortThread thd;
  mdtPortConfig cfg;

  // Setup
  cfg.setReadQueueSize(500);  /// ???? NOTE: re-enqueue readen frames to pool !!
  cfg.setFrameType(mdtFrame::FT_USBTMC);
  port.setConfig(&cfg);
  thd.setPort(&port);

  // Try to open
  port.setPortName("VID=0x0957:PID=0x0588");
  if(port.open() != mdtAbstractPort::NoError){
    QSKIP("No Agilent DSO1000 attached, or other error", SkipAll);
  }
  QVERIFY(port.setup() == mdtAbstractPort::NoError);

  // Start thread
  thd.start();
  thd.waitReady();

  // Open/close test
  thd.stop();
  thd.waitFinished();
  port.close();
  randomValueInit();
  for(int i=0; i<10; i++){
    QVERIFY(port.open() == mdtAbstractPort::NoError);
    QVERIFY(port.setup() == mdtAbstractPort::NoError);
    QTest::qWait(randomValue(0, 100));
    port.close();
  }
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  QVERIFY(port.setup() == mdtAbstractPort::NoError);
  thd.start();
  thd.waitReady();

  /*
   * *CLS command
   */
  port.lockMutex();
  uf = dynamic_cast<mdtFrameUsbTmc*> (port.writeFramesPool().dequeue());
  QVERIFY(uf != 0);
  uf->setWaitAnAnswer(false);
  uf->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  uf->setbTag(1);
  uf->setMessageData("*CLS\n");
  uf->encode();
  port.addFrameToWrite(uf);
  port.unlockMutex();
  QTest::qWait(100);

  /*
   * *RST command
   */

  // Send command
  port.lockMutex();
  uf = dynamic_cast<mdtFrameUsbTmc*> (port.writeFramesPool().dequeue());
  QVERIFY(uf != 0);
  uf->setWaitAnAnswer(false);
  uf->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  uf->setbTag(2);
  uf->setMessageData("*RST\n");
  uf->encode();
  port.addFrameToWrite(uf);
  port.unlockMutex();
  QTest::qWait(100);

  /*
   * *IDN? query
   */

  // Send query
  port.lockMutex();
  uf = dynamic_cast<mdtFrameUsbTmc*> (port.writeFramesPool().dequeue());
  QVERIFY(uf != 0);
  uf->setWaitAnAnswer(false);
  uf->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  uf->setbTag(3);
  uf->setMessageData("*IDN?\n");
  uf->encode();
  port.addFrameToWrite(uf);
  port.unlockMutex();
  QTest::qWait(100);
  // Send DEV_DEP_MSG_IN request
  port.lockMutex();
  uf = dynamic_cast<mdtFrameUsbTmc*> (port.writeFramesPool().dequeue());
  QVERIFY(uf != 0);
  uf->clear();
  uf->clearSub();
  uf->setWaitAnAnswer(true);
  uf->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_IN);
  uf->setbTag(4);
  uf->setTransferSize(cfg.readFrameSize()-12-1);
  uf->encode();
  port.addFrameToWrite(uf);
  port.unlockMutex();
  QTest::qWait(100);
  // Check readen frame
  port.lockMutex();
  QVERIFY(port.readenFrames().size() > 0);
  uf = dynamic_cast<mdtFrameUsbTmc*> (port.readenFrames().dequeue());
  QVERIFY(uf != 0);
  qDebug() << "Message data[Complete]: " << uf->messageData();
  qDebug() << "Message data[0-25]: " << uf->messageData().left(26);
  QVERIFY(uf->messageData().left(26) == "Agilent Technologies,DSO10");
  port.unlockMutex();
  QTest::qWait(500);

  /*
   * :AUToscale command
   */

  // Send command
  port.lockMutex();
  uf = dynamic_cast<mdtFrameUsbTmc*> (port.writeFramesPool().dequeue());
  QVERIFY(uf != 0);
  uf->setWaitAnAnswer(false);
  uf->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  uf->setbTag(5);
  uf->setMessageData(":AUToscale\n");
  uf->encode();
  port.addFrameToWrite(uf);
  port.unlockMutex();
  QTest::qWait(2000);

  /*
   * *RST command with wait before quit
   * Send a command and tell port that we wait
   *  a answer. This is false, but must not turn
   *  the application into a undefined state,
   *  and the stop must work.
   */

  // Send command
  port.lockMutex();
  uf = dynamic_cast<mdtFrameUsbTmc*> (port.writeFramesPool().dequeue());
  QVERIFY(uf != 0);
  uf->setWaitAnAnswer(true);
  uf->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  uf->setbTag(6);
  uf->setMessageData("*RST\n");
  uf->encode();
  port.addFrameToWrite(uf);
  port.unlockMutex();
  QTest::qWait(100);


  // End
  thd.stop();
  thd.waitFinished();
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtUsbPortTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
