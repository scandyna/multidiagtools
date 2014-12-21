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

// New USB/USBTMC API
#include "mdtUsbDeviceList.h"
#include "mdtUsbTransferPool.h"
#include "mdtUsbtmcFrame.h"
#include "mdtUsbtmcMessage.h"
#include "mdtUsbtmcPort.h"
#include "mdtUsbPortThreadNew.h"
#include "mdtUsbtmcBulkTransfer.h"
#include "mdtUsbtmcControlTransfer.h"
#include "mdtUsbtmcTransferHandler.h"
#include "mdtUsbtmcPortThreadNew.h"

#include <QByteArray>
#include <mutex>
#include <cstring>

#include <unistd.h> ///  \todo remove

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
  /**
  for(int i = 0; i < bSize; ++i){
    buffer[i] = i;
  }
  */
  ::memset(buffer, 0, bSize);
}

/*
 * Benchmarks
 */

void mdtUsbPortTest::basicAllocFreeBenchMarks()
{
  unsigned char *buffer;
  int N = 10;
  int bSize = 128;
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
  int bSize = 128;
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
  mdtUsbDeviceDescriptor deviceDescriptor1, deviceDescriptor2;
  mdtUsbInterfaceDescriptor interfaceDescriptor;
  int ret;

  // Init lubusb
  ret = libusb_init(&usbCtx);
  QVERIFY(ret == 0);

  // Check descriptors initial state
  QVERIFY(deviceDescriptor1.isEmpty());
  QVERIFY(deviceDescriptor2.isEmpty());
  QVERIFY(interfaceDescriptor.isEmpty());

  // Enumerate attached USB devices
  mdtUsbDeviceList devList(usbCtx);
  QVERIFY(devList.scan());
  if(devList.deviceList().isEmpty()){
    QSKIP("Found no USB devices.", SkipAll);
  }
  deviceDescriptor1 = devList.deviceList().at(0);
  QVERIFY(!deviceDescriptor1.isEmpty());
  QVERIFY(deviceDescriptor2.isEmpty());
  // Check copy of device descriptor
  deviceDescriptor2 = deviceDescriptor1;
  QVERIFY(!deviceDescriptor2.isEmpty());
  // Hope we have 1 interface in this device
  interfaceDescriptor = deviceDescriptor1.interface(0);
  QVERIFY(!interfaceDescriptor.isEmpty());
  // Check that copy has also interface
  interfaceDescriptor = deviceDescriptor2.interface(0);
  QVERIFY(!interfaceDescriptor.isEmpty());

  // Free libusb
  libusb_exit(usbCtx);

  QSKIP("Not completly implemented yet", SkipAll);
}

void mdtUsbPortTest::usbPortThreadTest()
{
  libusb_context *usbCtx = 0;
  mdtUsbDeviceDescriptor deviceDescriptor;
  int ret;

  // Init libusb
  ret = libusb_init(&usbCtx);
  QVERIFY(ret == 0);
  libusb_set_debug(usbCtx, 2);

  // Search USBTMC device
  mdtUsbDeviceList devList(usbCtx);
  QVERIFY(devList.scan());
  deviceDescriptor = devList.findFirstUsbtmcDevice();
  if(deviceDescriptor.isEmpty()){
    QSKIP("Found no USBTMC device attached", SkipAll);
  }

  // Build thread
  mdtUsbPortThreadNew thd(usbCtx);
  // Check start/stop
  QVERIFY(thd.start(deviceDescriptor, 0));
  thd.stop();
  QVERIFY(!thd.start(deviceDescriptor, 50));
  QVERIFY(thd.start(deviceDescriptor, 0));
  devList.clear();
  // Stop..
  thd.stop();

  // Free libusb
  libusb_exit(usbCtx);
}


/*
 * USBTMC tests
 */

void mdtUsbPortTest::usbtmcFrameTest()
{
  mdtUsbtmcFrame f(4096);
  QByteArray ba;
  mdtUsbtmcMessage message(ba);

  // Check initial values
  QCOMPARE(f.capacity(), 4096);
  QCOMPARE(f.bufferLength(), 12);
  for(int i = 0; i < 12; ++i){
    QCOMPARE(f.buffer()[i], (uint8_t)0);
  }
  QCOMPARE(f.msgID(), mdtUsbtmcFrame::msgId_t::MSG_ZERO);
  /*
   * DEV_DEP_MSG_OUT encoding
   */
  // Check header encoding for DEV_DEP_MSG_OUT - case 1 (no data)
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
  // Check correct encoding for DEV_DEP_MSG_OUT - case 2 (with data)
  ba = "*IDN?\n";
  message.reset();
  f.clear();
  f.setMsgID(mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_OUT);
  f.setbTag(0x01);
  ///f.setData("*IDN?\n");
  f.setData(message);
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
  message.reset();
  f.getData(message);
  QCOMPARE(ba, QByteArray("*IDN?\n"));
  ///QCOMPARE(f.data(), QByteArray("*IDN?\n"));
  // Check Alignment bytes
  ba = "A";
  message.reset();
  f.setData(message);
  ///f.setData("A");
  QCOMPARE((int)f.transferSize(), 1);
  QCOMPARE(f.bufferLength(), 16);
  QCOMPARE((char)f.buffer()[12], 'A');  // Data
  QCOMPARE((int)f.buffer()[13], 0);     // Alignment byte
  QCOMPARE((int)f.buffer()[14], 0);     // Alignment byte
  QCOMPARE((int)f.buffer()[15], 0);     // Alignment byte
  ba = "12";
  message.reset();
  f.setData(message);
  ///f.setData("12");
  QCOMPARE((int)f.transferSize(), 2);
  QCOMPARE(f.bufferLength(), 16);
  QCOMPARE((char)f.buffer()[12], '1');  // Data
  QCOMPARE((char)f.buffer()[13], '2');  // Data
  QCOMPARE((int)f.buffer()[14], 0);     // Alignment byte
  QCOMPARE((int)f.buffer()[15], 0);     // Alignment byte
  ba = "ABC";
  message.reset();
  f.setData(message);
  ///f.setData("ABC");
  QCOMPARE((int)f.transferSize(), 3);
  QCOMPARE(f.bufferLength(), 16);
  QCOMPARE((char)f.buffer()[12], 'A');  // Data
  QCOMPARE((char)f.buffer()[13], 'B');  // Data
  QCOMPARE((char)f.buffer()[14], 'C');  // Data
  QCOMPARE((int)f.buffer()[15], 0);     // Alignment byte
  ba = "1234";
  message.reset();
  f.setData(message);
  ///f.setData("1234");
  QCOMPARE((int)f.transferSize(), 4);
  QCOMPARE(f.bufferLength(), 16);
  QCOMPARE((char)f.buffer()[12], '1');  // Data
  QCOMPARE((char)f.buffer()[13], '2');  // Data
  QCOMPARE((char)f.buffer()[14], '3');  // Data
  QCOMPARE((char)f.buffer()[15], '4');  // Data
  ba = "A";
  message.reset();
  f.setData(message);
  ///f.setData("A");
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
  // Reset EOM and check
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
  ///QCOMPARE(f.data(), QByteArray("ABCD,1234\n"));
  message.reset();
  f.getData(message);
  QCOMPARE(ba, QByteArray("ABCD,1234\n"));

  /*
   * TODO: VENDOR_SPECIFIC_OUT / VENDOR_SPECIFIC_IN
   */
}

void mdtUsbPortTest::usbtmcFrameBenchmark()
{
  mdtUsbtmcFrame f(4096);
  QByteArray query = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  mdtUsbtmcMessage queryMessage(query);
  QByteArray result;
  mdtUsbtmcMessage resultMessage(result);
  mdtUsbtmcFrame::msgId_t msgID;
  uint8_t bTag;
  uint8_t bTagInverse;
  uint32_t transferSize;

  QBENCHMARK{
    f.clear();
    queryMessage.reset();
    resultMessage.reset();
    f.setMsgID(mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_OUT);
    f.setbTag(0x5B);
    ///f.setData(query);
    f.setData(queryMessage);
    msgID = f.msgID();
    bTag = f.bTag();
    bTagInverse = f.bTagInverse();
    transferSize = f.transferSize();
    ///result = f.data();
    f.getData(resultMessage);
  }
  QCOMPARE((int)msgID, 1);
  QCOMPARE(bTag, (uint8_t)0x5B);
  QCOMPARE(bTagInverse, (uint8_t)0xA4);
  QCOMPARE(transferSize, (uint32_t)26);
  QCOMPARE(result, QByteArray("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
}

void mdtUsbPortTest::usbtmcMessageTest()
{
  QByteArray ba;
  std::vector<uint8_t> buffer;
  int n;

  // Init
  buffer.reserve(4);
  ba = "123456789";
  mdtUsbtmcMessage message(ba);
  QCOMPARE(ba, QByteArray("123456789"));
  QCOMPARE(message.bytesToRead(), 9);
  QVERIFY(message.hasBytesToRead());
  // Read 1
  n = message.read(buffer.data(), buffer.capacity());
  QCOMPARE(n, 4);
  QCOMPARE((char)buffer[0], '1');
  QCOMPARE((char)buffer[1], '2');
  QCOMPARE((char)buffer[2], '3');
  QCOMPARE((char)buffer[3], '4');
  QCOMPARE(message.bytesToRead(), 5);
  QVERIFY(message.hasBytesToRead());
  QCOMPARE(ba, QByteArray("123456789"));
  // Read 2
  n = message.read(buffer.data(), buffer.capacity());
  QCOMPARE(n, 4);
  QCOMPARE((char)buffer[0], '5');
  QCOMPARE((char)buffer[1], '6');
  QCOMPARE((char)buffer[2], '7');
  QCOMPARE((char)buffer[3], '8');
  QCOMPARE(message.bytesToRead(), 1);
  QVERIFY(message.hasBytesToRead());
  QCOMPARE(ba, QByteArray("123456789"));
  // Read 3
  n = message.read(buffer.data(), buffer.capacity());
  QCOMPARE(n, 1);
  QCOMPARE((char)buffer[0], '9');
  QCOMPARE(message.bytesToRead(), 0);
  QVERIFY(!message.hasBytesToRead());
  QCOMPARE(ba, QByteArray("123456789"));
  // Read 4 - Must copy nothing and not fail
  n = message.read(buffer.data(), buffer.capacity());
  QCOMPARE(message.bytesToRead(), 0);
  QVERIFY(!message.hasBytesToRead());
  QCOMPARE(ba, QByteArray("123456789"));
  // Clear byte array and check that nothing fails
  ba.clear();
  QVERIFY(ba.isEmpty());
  n = message.read(buffer.data(), buffer.capacity());
  QCOMPARE(message.bytesToRead(), 0);
  QVERIFY(!message.hasBytesToRead());
  // Write 1
  buffer[0] = 'A';
  buffer[1] = 'B';
  buffer[2] = 'C';
  buffer[3] = 'D';
  message.write(buffer.data(), buffer.capacity());
  QCOMPARE(ba, QByteArray("ABCD"));
  // Write 2
  buffer[0] = 'E';
  buffer[1] = 'F';
  buffer[2] = 'G';
  buffer[3] = 'H';
  message.write(buffer.data(), buffer.capacity());
  QCOMPARE(ba, QByteArray("ABCDEFGH"));
  // Write 3
  buffer[0] = 'I';
  message.write(buffer.data(), 1);
  QCOMPARE(ba, QByteArray("ABCDEFGHI"));
  // Write 4
  message.write(buffer.data(), 0);
  QCOMPARE(ba, QByteArray("ABCDEFGHI"));
  // Reset and write
  message.reset();
  buffer[0] = '1';
  message.write(buffer.data(), 1);
  QCOMPARE(ba, QByteArray("1"));
}

void mdtUsbPortTest::usbtmcMessageBenchmark()
{
  std::vector<uint8_t> buffer;

  // Init
  buffer.reserve(4096);
  QByteArray ba(10000, 'A');
  mdtUsbtmcMessage message(ba);

  QBENCHMARK{
    message.read(buffer.data(), buffer.capacity());
    message.reset();
    message.write(buffer.data(), buffer.capacity());
    message.reset();
  }
}

void mdtUsbPortTest::usbtmcBulkTransferTest()
{
  libusb_context *usbCtx = 0;
  libusb_device_handle *handle;
  mdtUsbDeviceDescriptor deviceDescriptor;
  mdtUsbInterfaceDescriptor interface;
  mdtUsbEndpointDescriptor bulkOutEpd;
  mdtUsbEndpointDescriptor bulkInEpd;
  QByteArray ba;
  mdtUsbtmcMessage message(ba);
  int ret;

  // Init libusb
  ret = libusb_init(&usbCtx);
  QVERIFY(ret == 0);

  // Search USBTMC device
  mdtUsbDeviceList devList(usbCtx);
  QVERIFY(devList.scan());
  deviceDescriptor = devList.findFirstUsbtmcDevice();
  if(deviceDescriptor.isEmpty()){
    QSKIP("Found no USBTMC device attached", SkipAll);
  }
  // Get interface
  interface = deviceDescriptor.interface(0);
  QVERIFY(!interface.isEmpty());
  // Get bulk OUT enpoint
  QCOMPARE(interface.bulkOutEndpoints().size(), 1);
  bulkOutEpd = interface.bulkOutEndpoints().at(0);
  QVERIFY(!bulkOutEpd.isEmpty());
  // Get bulk IN enpoint
  QCOMPARE(interface.bulkInEndpoints().size(), 1);
  bulkInEpd = interface.bulkInEndpoints().at(0);
  QVERIFY(!bulkInEpd.isEmpty());
  // Open device
  handle = deviceDescriptor.open();
  QVERIFY(handle != 0);
  devList.clear();

  /*
   * In this phase, we not send data to device,
   *  but a device handle is required by USBTMC transfer class.
   */
  mdtUsbtmcTransferHandler th(usbCtx);
  mdtUsbtmcBulkTransfer transfer(th, bulkOutEpd, handle);
  QVERIFY(transfer.allocOk());
  /*
   * Check some flags
   */
  QVERIFY(!(transfer.flags() & LIBUSB_TRANSFER_FREE_BUFFER));
  QVERIFY(!(transfer.flags() & LIBUSB_TRANSFER_FREE_TRANSFER));
  QVERIFY(!transfer.responseExpected());
  QVERIFY(transfer.splitAction() == mdtUsbtmcBulkTransfer::SplitAction_t::NoAction);
  QVERIFY(!transfer.splitActionRequired());
  // Check completed flag
  QVERIFY(transfer.completedPointer() != 0);
  QVERIFY(*transfer.completedPointer() == 0);
  QVERIFY(!transfer.isCompleted());
  *transfer.completedPointer() = 1;
  QVERIFY(transfer.isCompleted());
  *transfer.completedPointer() = 0;
  QVERIFY(!transfer.isCompleted());
  transfer.setCompleted();
  QVERIFY(transfer.isCompleted());
  QVERIFY(*transfer.completedPointer() == 1);
  // Check sync flag (can only check default without calling submit() or cancel())
  QVERIFY(!transfer.isSync());
  /*
   * Check Split action
   */
  transfer.setSplitAction(mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_ABORT);
  QVERIFY(transfer.splitAction() == mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_ABORT);
  QVERIFY(transfer.splitActionRequired());
  transfer.clearSplitAction();
  QVERIFY(transfer.splitAction() == mdtUsbtmcBulkTransfer::SplitAction_t::NoAction);
  QVERIFY(!transfer.splitActionRequired());

  /*
   * Setup DEV_DEP_MSG_OUT transfer
   */
  ba = "*IDN?\n";
  message.reset();
  transfer.setupDevDepMsgOut(1, message, true, 30000);
  QVERIFY(transfer.responseExpected());
  QCOMPARE(transfer.bTag(), (uint8_t)1);
  ba = "*CLS\n";
  message.reset();
  transfer.setupDevDepMsgOut(2, message, false, 30000);
  QVERIFY(!transfer.responseExpected());
  QCOMPARE(transfer.bTag(), (uint8_t)2);
  ba = "*IDN?\n";
  message.reset();
  transfer.setupDevDepMsgOut(3, message, true, 30000);
  QVERIFY(transfer.responseExpected());
  QCOMPARE(transfer.bTag(), (uint8_t)3);
  // Check Split action reset
  ba = "*CLS\n";
  message.reset();
  transfer.setSplitAction(mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_ABORT);
  QVERIFY(transfer.splitActionRequired());
  transfer.setupDevDepMsgOut(2, message, false, 30000);
  QVERIFY(!transfer.splitActionRequired());

  /*
   * Setup REQUEST_DEV_DEP_MSG_IN
   */
  transfer.setupRequestDevDepMsgIn(4, false, '\0', 30000);
  QVERIFY(!transfer.responseExpected());
  QCOMPARE(transfer.bTag(), (uint8_t)4);
  // Check Split action reset
  transfer.setSplitAction(mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_ABORT);
  QVERIFY(transfer.splitActionRequired());
  transfer.setupRequestDevDepMsgIn(4, false, '\0', 30000);
  QVERIFY(!transfer.splitActionRequired());

  // Free ressources
  if(handle != 0){
    libusb_close(handle);
  }
  libusb_exit(usbCtx);
}

void mdtUsbPortTest::usbtmcControlTransferTest()
{
  libusb_context *usbCtx = 0;
  libusb_device_handle *handle;
  mdtUsbDeviceDescriptor deviceDescriptor;
  mdtUsbInterfaceDescriptor interface;
  mdtUsbEndpointDescriptor bulkOutEpd;
  mdtUsbEndpointDescriptor bulkInEpd;
  mdtUsbEndpointDescriptor interruptInEpd;
  int ret;

  // Init libusb
  ret = libusb_init(&usbCtx);
  QVERIFY(ret == 0);

  // Search USBTMC device
  mdtUsbDeviceList devList(usbCtx);
  QVERIFY(devList.scan());
  deviceDescriptor = devList.findFirstUsbtmcDevice();
  if(deviceDescriptor.isEmpty()){
    QSKIP("Found no USBTMC device attached", SkipAll);
  }
  // Get interface
  interface = deviceDescriptor.interface(0);
  QVERIFY(!interface.isEmpty());
  // Get device's attributes
  qDebug() << "Found USBTMC device: " << deviceDescriptor.idString();
  qDebug() << "Bulk IN enpoints: " << interface.bulkInEndpoints().size();
  qDebug() << "Bulk OUT enpoints: " << interface.bulkOutEndpoints().size();
  qDebug() << "Interrupt IN enpoints: " << interface.interruptInEndpoints().size();
  qDebug() << "Interrupt OUT enpoints: " << interface.interruptOutEndpoints().size();
  // Get bulk IN enpoint
  QCOMPARE(interface.bulkInEndpoints().size(), 1);
  bulkInEpd = interface.bulkInEndpoints().at(0);
  QVERIFY(!bulkInEpd.isEmpty());
  // Get bulk OUT enpoint
  QCOMPARE(interface.bulkOutEndpoints().size(), 1);
  bulkOutEpd = interface.bulkOutEndpoints().at(0);
  QVERIFY(!bulkOutEpd.isEmpty());
  // Get Interrupt IN enpoint
  if(interface.interruptInEndpoints().size() == 1){
    interruptInEpd = interface.interruptInEndpoints().at(0);
  }
  
  qDebug() << "Bulk OUT - address (RAW): " << bulkOutEpd.address() << " , number: " << bulkOutEpd.number();
  qDebug() << "Bulk IN - address (RAW): " << bulkInEpd.address() << " , number: " << bulkInEpd.number();
  qDebug() << "Interrupt IN - address (RAW): " << interruptInEpd.address() << " , number: " << interruptInEpd.number();
  
  // Open device
  handle = deviceDescriptor.open();
  QVERIFY(handle != 0);
  devList.clear();
  /**
  QCOMPARE(libusb_set_auto_detach_kernel_driver(handle, 1), 0);
  QCOMPARE(libusb_claim_interface(handle, interface.bInterfaceNumber()), 0);
  */

  /*
   * In this phase, we not send data to device,
   *  but a device handle is required by USBTMC transfer class.
   */
  mdtUsbtmcTransferHandler th(usbCtx);
  mdtUsbtmcControlTransfer transfer(th, handle);
  QVERIFY(transfer.allocOk());
  QCOMPARE(transfer.actualLength(), 0);
  // Check some flags
  QVERIFY(!(transfer.flags() & LIBUSB_TRANSFER_FREE_BUFFER));
  QVERIFY(!(transfer.flags() & LIBUSB_TRANSFER_FREE_TRANSFER));
  // Check completed flag
  QVERIFY(transfer.completedPointer() != 0);
  QVERIFY(*transfer.completedPointer() == 0);
  QVERIFY(!transfer.isCompleted());
  *transfer.completedPointer() = 1;
  QVERIFY(transfer.isCompleted());
  *transfer.completedPointer() = 0;
  QVERIFY(!transfer.isCompleted());
  transfer.setCompleted();
  QVERIFY(transfer.isCompleted());
  QVERIFY(*transfer.completedPointer() == 1);
  // Check sync flag (can only check default without calling submit() or cancel())
  QVERIFY(!transfer.isSync());
  /*
   * Check simple set/get
   */
  transfer.setByteValue(0, 0x12);
  QCOMPARE(transfer.byteValue(0), (uint8_t)0x12);
  transfer.setWordValue(0, 0x1234);
  QCOMPARE(transfer.wordValue(0), (uint16_t)0x1234);
  /*
   * Setup a CLEAR_FEATURE request with wValue = ENDPOINT_HALT for enpoint number 2
   */
  transfer.setupClearEndpointHalt(2, 30000);
  // Check bmRequestType: D7=0(host->dev) , D6..D5=00(Standard) , D4..D0:00010(Endpoint) -> 0000 0010
  QCOMPARE(transfer.bmRequestType(), (uint8_t)0b00000010);
  QCOMPARE(transfer.bRequest(), (uint8_t)1);  // 1 (CLEAR_FEATURE)
  QCOMPARE(transfer.wValue(), (uint16_t)0);   // 0 (Feature: ENDPOINT_HALT)
  QCOMPARE(transfer.wIndex(), (uint16_t)2);   // 2 (endpoint 2)
  QCOMPARE(transfer.wLength(), (uint16_t)0);  // 0 (No data)
  /*
   * Setup INITIATE_ABORT_BULK_OUT request for endpoint 1, bTag 235
   */
  transfer.setupInitiateAbortBulkOut(235, 1, 30000);
  QCOMPARE(transfer.bmRequestType(), (uint8_t)0xA2);  // Directly given in USBTMC 1.0, table 18
  QCOMPARE(transfer.bRequest(), (uint8_t)1);          // 1 (INITIATE_ABORT_BULK_OUT)
  QCOMPARE(transfer.wValue(), (uint16_t)235);         // 235 (bTag)
  QCOMPARE(transfer.wIndex(), (uint16_t)1);           // 1 (Dir=OUT, endpoint 1)
  QCOMPARE(transfer.wLength(), (uint16_t)2);          // 2 (Expected data of length 2)
  QVERIFY(transfer.usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_ABORT_BULK_OUT);
  /*
   * Setup CHECK_ABORT_BULK_OUT_STATUS request for endpoint 1
   */
  transfer.setupCheckAbortBulkOutStatus(1, 30000);
  QCOMPARE(transfer.bmRequestType(), (uint8_t)0xA2);  // Directly given in USBTMC 1.0, table 21
  QCOMPARE(transfer.bRequest(), (uint8_t)2);          // 2 (CHECK_ABORT_BULK_OUT_STATUS)
  QCOMPARE(transfer.wValue(), (uint16_t)0);           // 0
  QCOMPARE(transfer.wIndex(), (uint16_t)1);           // 1 (Dir=OUT, endpoint 1)
  QCOMPARE(transfer.wLength(), (uint16_t)8);          // 8 (Expected data of length 8)
  QVERIFY(transfer.usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::CHECK_ABORT_BULK_OUT_STATUS);
  /*
   * Setup INITIATE_ABORT_BULK_IN request for endpoint 2, bTag 246
   */
  transfer.setupInitiateAbortBulkIn(246, 2, 30000);
  QCOMPARE(transfer.bmRequestType(), (uint8_t)0xA2);    // Directly given in USBTMC 1.0, table 18
  QCOMPARE(transfer.bRequest(), (uint8_t)3);            // 3 (INITIATE_ABORT_BULK_IN)
  QCOMPARE(transfer.wValue(), (uint16_t)246);           // 246 (bTag)
  QCOMPARE(transfer.wIndex(), (uint16_t)(0b10000000+2)); // 1 (Dir=IN, endpoint 2)
  QCOMPARE(transfer.wLength(), (uint16_t)2);            // 2 (Expected data of length 2)
  QVERIFY(transfer.usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_ABORT_BULK_IN);
  /*
   * Setup CHECK_ABORT_BULK_IN_STATUS request for endpoint 2
   */
  transfer.setupCheckAbortBulkInStatus(2, 30000);
  QCOMPARE(transfer.bmRequestType(), (uint8_t)0xA2);  // Directly given in USBTMC 1.0, table 27
  QCOMPARE(transfer.bRequest(), (uint8_t)4);          // 4 (CHECK_ABORT_BULK_IN_STATUS)
  QCOMPARE(transfer.wValue(), (uint16_t)0);           // 0
  QCOMPARE(transfer.wIndex(), (uint16_t)(0b10000000+2)); // 1 (Dir=IN, endpoint 2)
  QCOMPARE(transfer.wLength(), (uint16_t)8);          // 8 (Expected data of length 8)
  QVERIFY(transfer.usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::CHECK_ABORT_BULK_IN_STATUS);
  /*
   * Setup INITIATE_CLEAR request for interface 1
   */
  transfer.setupInitiateClear(1, 30000);
  QCOMPARE(transfer.bmRequestType(), (uint8_t)0xA1);  // Directly given in USBTMC 1.0, table 30
  QCOMPARE(transfer.bRequest(), (uint8_t)5);          // 4 (INITIATE_CLEAR)
  QCOMPARE(transfer.wValue(), (uint16_t)0);           // 0
  QCOMPARE(transfer.wIndex(), (uint16_t)1);           // 1 (interface 1)
  QCOMPARE(transfer.wLength(), (uint16_t)1);          // 1 (Expected data of length 1)
  QVERIFY(transfer.usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_CLEAR);
  /*
   * Setup CHECK_CLEAR_STATUS request for interface 1
   */
  transfer.setupCheckClearStatus(1, 30000);
  QCOMPARE(transfer.bmRequestType(), (uint8_t)0xA1);  // Directly given in USBTMC 1.0, table 33
  QCOMPARE(transfer.bRequest(), (uint8_t)6);          // 6 (CHECK_CLEAR_STATUS)
  QCOMPARE(transfer.wValue(), (uint16_t)0);           // 0
  QCOMPARE(transfer.wIndex(), (uint16_t)1);           // 1 (interface 1)
  QCOMPARE(transfer.wLength(), (uint16_t)2);          // 2 (Expected data of length 2)
  QVERIFY(transfer.usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::CHECK_CLEAR_STATUS);
  /*
   * Setup GET_CAPABILITIES request for interface 1
   */
  transfer.setupGetCapabilities(1, 30000);
  QCOMPARE(transfer.bmRequestType(), (uint8_t)0xA1);  // Directly given in USBTMC 1.0, table 36
  QCOMPARE(transfer.bRequest(), (uint8_t)7);          // 7 (GET_CAPABILITIES)
  QCOMPARE(transfer.wValue(), (uint16_t)0);           // 0
  QCOMPARE(transfer.wIndex(), (uint16_t)1);           // 1 (interface 1)
  QCOMPARE(transfer.wLength(), (uint16_t)0x18);       // 0x18 (Expected data of length 24)
  QVERIFY(transfer.usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::GET_CAPABILITIES);
  /*
   * Setup INDICATOR_PULSE request for interface 1
   */
  transfer.setupIndicatorPulse(1, 30000);
  QCOMPARE(transfer.bmRequestType(), (uint8_t)0xA1);  // Directly given in USBTMC 1.0, table 38
  QCOMPARE(transfer.bRequest(), (uint8_t)64);         // 64 (INDICATOR_PULSE)
  QCOMPARE(transfer.wValue(), (uint16_t)0);           // 0
  QCOMPARE(transfer.wIndex(), (uint16_t)1);           // 1 (interface 1)
  QCOMPARE(transfer.wLength(), (uint16_t)1);          // 1 (Expected data of length 1)
  QVERIFY(transfer.usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::INDICATOR_PULSE);
  /*
   * Check USBTMC_status response value
   * Note: data part is written starting at offset 8 by libusb.
   */
  transfer.setByteValue(8, 0x01);
  QVERIFY(transfer.usbtmcStatus() == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SUCCESS);
  transfer.setByteValue(8, 0x02);
  QVERIFY(transfer.usbtmcStatus() == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_PENDING);
  transfer.setByteValue(8, 0x80);
  QVERIFY(transfer.usbtmcStatus() == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_FAILED);
  transfer.setByteValue(8, 0x81);
  QVERIFY(transfer.usbtmcStatus() == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_TRANSFER_NOT_IN_PROGRESS);
  transfer.setByteValue(8, 0x82);
  QVERIFY(transfer.usbtmcStatus() == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SPLIT_NOT_IN_PROGRESS);
  transfer.setByteValue(8, 0x83);
  QVERIFY(transfer.usbtmcStatus() == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SPLIT_IN_PROGRESS);
  transfer.setByteValue(8, 0x00);

  
  ///QVERIFY(transfer.submit());
  
  /**
  ret = libusb_handle_events(usbCtx);
  if(ret != 0){
    qDebug() << "Handle event error: " << ret << " (" << libusb_strerror((libusb_error)ret) << ")";
  }
  */
  
  
  // Free ressources
  if(handle != 0){
    libusb_release_interface(handle, interface.bInterfaceNumber());
    libusb_close(handle);
  }
  libusb_exit(usbCtx);
}

void mdtUsbPortTest::usbtmcTransferPoolTest()
{
  int ret;
  libusb_context *usbCtx = 0;
  libusb_device_handle *handle;
  mdtUsbDeviceDescriptor deviceDescriptor;
  mdtUsbtmcControlTransfer *controlTransfer1, *controlTransfer2, *controlTransfer3;

  // Init libusb
  ret = libusb_init(&usbCtx);
  QVERIFY(ret == 0);
  // Search USBTMC device
  mdtUsbDeviceList devList(usbCtx);
  QVERIFY(devList.scan());
  deviceDescriptor = devList.findFirstUsbtmcDevice();
  if(deviceDescriptor.isEmpty()){
    QSKIP("Found no USBTMC device attached", SkipAll);
  }
  // Open device
  handle = deviceDescriptor.open();
  QVERIFY(handle != 0);
  devList.clear();

  /*
   * Build a USBTMC control transfer pool.
   *  - Guard size: 3
   *  - Alloc 2
   */
  mdtUsbtmcTransferHandler th(usbCtx);
  mdtUsbTransferPool<mdtUsbtmcControlTransfer> controlTransferPool(3);
  QVERIFY(controlTransferPool.allocTransfers(2, th, handle));
  QCOMPARE(controlTransferPool.pendingTransferCount(), 0);
  // Get 1. transfer
  controlTransfer1 = controlTransferPool.getTransfer(th);
  QVERIFY(controlTransfer1 != 0);
  QCOMPARE(controlTransferPool.pendingTransferCount(), 1);
  QVERIFY(controlTransferPool.getPendingTransfer() == controlTransfer1);
  QCOMPARE(controlTransferPool.pendingTransferCount(), 1);
  // Get 2. transfer
  controlTransfer2 = controlTransferPool.getTransfer(th);
  QVERIFY(controlTransfer2 != 0);
  QCOMPARE(controlTransferPool.pendingTransferCount(), 2);
  QVERIFY(controlTransferPool.getPendingTransfer() == controlTransfer2);
  QCOMPARE(controlTransferPool.pendingTransferCount(), 2);
  // Get 1 more transfer (must be allocated)
  controlTransfer3 = controlTransferPool.getTransfer(th);
  QVERIFY(controlTransfer3 != 0);
  QCOMPARE(controlTransferPool.pendingTransferCount(), 3);
  QVERIFY(controlTransferPool.getPendingTransfer() == controlTransfer3);
  QCOMPARE(controlTransferPool.pendingTransferCount(), 3);
  // The 2 following lines must fail (uncomment to try..)
  //mdtUsbtmcControlTransfer *controlTransfer4 = controlTransferPool.getTransfer(th);
  //controlTransferPool.restoreTransfer(controlTransfer4);
  // Restore transfers
  QCOMPARE(controlTransferPool.pendingTransferCount(), 3);
  controlTransferPool.restoreTransfer(controlTransfer1);
  QCOMPARE(controlTransferPool.pendingTransferCount(), 2);
  controlTransferPool.restoreTransfer(controlTransfer2);
  QCOMPARE(controlTransferPool.pendingTransferCount(), 1);
  controlTransferPool.restoreTransfer(controlTransfer3);
  QCOMPARE(controlTransferPool.pendingTransferCount(), 0);
  // Clear - must not fail - Ideal case, we restored all transfers
  controlTransferPool.clear();
  /*
   * Re-alloc pool and check that clear restores pending transfers
   */
  QVERIFY(controlTransferPool.allocTransfers(2, th, handle));
  QCOMPARE(controlTransferPool.pendingTransferCount(), 0);
  // Get 1. transfer
  controlTransfer1 = controlTransferPool.getTransfer(th);
  QVERIFY(controlTransfer1 != 0);
  QCOMPARE(controlTransferPool.pendingTransferCount(), 1);
  // Get 2. transfer
  controlTransfer2 = controlTransferPool.getTransfer(th);
  QVERIFY(controlTransfer2 != 0);
  QCOMPARE(controlTransferPool.pendingTransferCount(), 2);
  // Get 1 more transfer (must be allocated)
  controlTransfer3 = controlTransferPool.getTransfer(th);
  QVERIFY(controlTransfer3 != 0);
  QCOMPARE(controlTransferPool.pendingTransferCount(), 3);
  // Restor only 1 transfer
  controlTransferPool.restoreTransfer(controlTransfer1);
  QCOMPARE(controlTransferPool.pendingTransferCount(), 2);
  // Clear - must not fail
  controlTransferPool.clear();

  
  /*
   * Build a pool for pending control transfers
   *  - Guard size: 3
   *  - Alloc: 0
   */

  // Free ressources
  if(handle != 0){
    libusb_close(handle);
  }
  libusb_exit(usbCtx);
}

void mdtUsbPortTest::usbtmcTransferPoolBenchmark()
{
  int N = 1000000;
  int ret;
  libusb_context *usbCtx = 0;
  libusb_device_handle *handle;
  mdtUsbDeviceDescriptor deviceDescriptor;

  // Init libusb
  ret = libusb_init(&usbCtx);
  QVERIFY(ret == 0);
  // Search USBTMC device
  mdtUsbDeviceList devList(usbCtx);
  QVERIFY(devList.scan());
  deviceDescriptor = devList.findFirstUsbtmcDevice();
  if(deviceDescriptor.isEmpty()){
    QSKIP("Found no USBTMC device attached", SkipAll);
  }
  // Open device
  handle = deviceDescriptor.open();
  QVERIFY(handle != 0);
  devList.clear();

  /*
   * Build a USBTMC control transfer pool.
   */
  mdtUsbtmcTransferHandler th(usbCtx);
  mdtUsbTransferPool<mdtUsbtmcControlTransfer> controlTransferPool(30);
  QVERIFY(controlTransferPool.allocTransfers(10, th, handle));
  // Bench get/restore
  mdtUsbtmcControlTransfer *controlTransfer;
  int i;
  QBENCHMARK{
    for(i = 0; i < N; ++i){
      controlTransfer = controlTransferPool.getTransfer(th);
      if(controlTransfer == 0){
        qDebug() << "A null transfer ?!?";
      }
      controlTransferPool.restoreTransfer(controlTransfer);
    }
  }
  QVERIFY(controlTransfer != 0);

  // Free ressources
  if(handle != 0){
    libusb_close(handle);
  }
  libusb_exit(usbCtx);
}

void mdtUsbPortTest::usbtmcPortThreadTest()
{
  libusb_context *usbCtx = 0;
  mdtUsbDeviceDescriptor deviceDescriptor;
  QByteArray ba;
  mdtUsbtmcMessage message(ba);
  int ret;

  // Init libusb
  ret = libusb_init(&usbCtx);
  QVERIFY(ret == 0);
  libusb_set_debug(usbCtx, 0b00000011);

  // Search USBTMC device
  mdtUsbDeviceList devList(usbCtx);
  QVERIFY(devList.scan());
  deviceDescriptor = devList.findFirstUsbtmcDevice();
  if(deviceDescriptor.isEmpty()){
    QSKIP("Found no USBTMC device attached", SkipAll);
  }
  QVERIFY(!deviceDescriptor.interface(0).isEmpty());

  // Build thread
  mdtUsbtmcTransferHandler th(usbCtx);
  mdtUsbtmcPortThreadNew thd(th, usbCtx);
  // Check start/stop
  QVERIFY(thd.start(deviceDescriptor, 0));
  thd.stop();
  QVERIFY(!thd.start(deviceDescriptor, 50));
  QVERIFY(thd.start(deviceDescriptor, 0));
  devList.clear();

  // Submit a normal query
  ba = "*IDN?\n";
  message.reset();
  qDebug() << "TEST: submit bulk message ...";
  th.submitCommand(message, true, 30000);
  QTest::qWait(100);
  /// \todo Check that data was returned
  
  // Produce error: submit a command that expects no response, but tell we want one back
  ba = "*CLS\n";
  message.reset();
  qDebug() << "TEST: submit bulk message *CLS ...";
  th.submitCommand(message, true, 30000);
  QTest::qWait(100);
  // Submit a normal query
  ba = "*IDN?\n";
  message.reset();
  qDebug() << "TEST: submit bulk message ...";
  th.submitCommand(message, true, 30000);
  QTest::qWait(100);
  /// \todo Check that data was returned
  
  // Produce error: submit a query + command without waiting
  ba = "*IDN?\n";
  message.reset();
  qDebug() << "TEST: submit *IDN? + *ClS bulk message ...";
  th.submitCommand(message, true, 30000);
  ba = "*CLS\n";
  // Cancel bulk-OUT transfers
  ///th.dbgSubmitAbortOnePendingBulkOutTransfer();
  th.dbgSubmitAbortOnePendingBulkInTransfer();
  
  ///th.submitAbortBulkOutTransfer();
  ///th.submitBulkOutTransfersCancellation();
  // Cancel bulk-IN transfers
  ///th.submitBulkInTransfersCancellation();

  
  /*
   * Send a custom transfer with invalid MsgID:
   * - MsgID: 0 (invalid)
   * - bTag : 5
   * - bTagInverse: ~5
   * - transferSize: 12
   * - bmTransferAttributes: 0
   * - TermChar: 0
   */
  ///th.dbgSubmitCustomBulkOutTransfer(1, 5, ~5, 12, 0, 0, false, 12, 30000);
  ///QTest::qWait(100);

  // Submit a normal query
  ba = "*IDN?\n";
  message.reset();
  qDebug() << "TEST: submit bulk message ...";
  th.submitCommand(message, true, 30000);
  /// \todo Check that data was returned
  
  QTest::qWait(1000);

  // Send some control requests
  ///qDebug() << "TEST: submit a control transfer ...";
  ///QVERIFY(th.submitClearBulkOutEndpointHalt(30000));
  ///qDebug() << "TEST: submit a control transfer ...";
  ///th.beginClearBulkIo();

  ///usleep(100000);

  // Stop..
  thd.stop();
  // Free libusb
  libusb_exit(usbCtx);
}

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
