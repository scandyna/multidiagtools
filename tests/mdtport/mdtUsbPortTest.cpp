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
#include "mdtUsbPortTest.h"
#include "mdtApplication.h"
#include "mdtUsbPort.h"
#include "mdtUsbDeviceDescriptor.h"
#include "mdtUsbConfigDescriptor.h"
#include "mdtUsbInterfaceDescriptor.h"
#include "mdtUsbEndpointDescriptor.h"
#include "mdtUsbPortThread.h"
#include "mdtPortConfig.h"
#include "mdtFrame.h"
#include "mdtFrameCodecK8055.h"
#include "mdtFrameUsbTmc.h"

#include <QDebug>

void mdtUsbPortTest::agilentDso1000Test()
{
  mdtFrameUsbTmc *uf;
  mdtUsbPort port;
  mdtUsbPortThread thd;
  mdtPortConfig cfg;

  // Setup
  cfg.setReadQueueSize(500);
  cfg.setFrameType(mdtFrame::FT_USBTMC);
  port.setConfig(&cfg);
  thd.setPort(&port);

  // Try to open
  port.setPortName("0x0957:0x0588");
  if(port.open() != mdtAbstractPort::NoError){
    QSKIP("No Agilent DSO1000 attached, or other error", SkipAll);
  }
  QVERIFY(port.setup() == mdtAbstractPort::NoError);

  // Start thread
  QVERIFY(thd.start());

  // Open/close test
  thd.stop();
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
  QVERIFY(thd.start());

  /*
   * *IDN? query
   */

  // Send query
  port.lockMutex();
  uf = dynamic_cast<mdtFrameUsbTmc*> (port.writeFramesPool().dequeue());
  QVERIFY(uf != 0);
  uf->setWaitAnAnswer(false);
  uf->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  uf->setbTag(1);
  uf->setMessageData("*IDN?");
  uf->encode();
  port.addFrameToWrite(uf);
  port.unlockMutex();
  QTest::qWait(100);
  // Send DEV_DEP_MSG_IN request
  port.lockMutex();
  uf = dynamic_cast<mdtFrameUsbTmc*> (port.writeFramesPool().dequeue());
  QVERIFY(uf != 0);
  uf->setWaitAnAnswer(true);
  uf->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_IN);
  uf->setbTag(2);
  uf->setMessageData("");
  uf->encode();
  port.addFrameToWrite(uf);
  port.unlockMutex();
  QTest::qWait(100);
  // Check readen frame
  port.lockMutex();
  QVERIFY(port.readenFrames().size() > 0);
  uf = dynamic_cast<mdtFrameUsbTmc*> (port.readenFrames().dequeue());
  QVERIFY(uf != 0);
  qDebug() << "Message data: " << uf->messageData().left(26);
  QVERIFY(uf->messageData().left(26) == "Agilent Technologies,DSO10");
  port.unlockMutex();
  QTest::qWait(500);

  /*
   * *RST command
   */

  // Send command
  port.lockMutex();
  uf = dynamic_cast<mdtFrameUsbTmc*> (port.writeFramesPool().dequeue());
  QVERIFY(uf != 0);
  uf->setWaitAnAnswer(false);
  uf->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  uf->setbTag(3);
  uf->setMessageData("*RST\n");
  uf->encode();
  port.addFrameToWrite(uf);
  port.unlockMutex();
  QTest::qWait(100);

  /*
   * :AUToscale command
   */

  // Send command
  port.lockMutex();
  uf = dynamic_cast<mdtFrameUsbTmc*> (port.writeFramesPool().dequeue());
  QVERIFY(uf != 0);
  uf->setWaitAnAnswer(false);
  uf->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  uf->setbTag(4);
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
  uf->setbTag(3);
  uf->setMessageData("*RST\n");
  uf->encode();
  port.addFrameToWrite(uf);
  port.unlockMutex();
  QTest::qWait(100);

  // End
  thd.stop();
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
