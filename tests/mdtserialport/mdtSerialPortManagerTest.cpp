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
#include "mdtSerialPortManagerTest.h"
#include "mdtSerialPortManager.h"
#include "mdtSerialPort.h"
#include "mdtSerialPortConfig.h"
#include "mdtPortWriteThread.h"
#include "mdtPortReadThread.h"
#include "mdtSerialPortCtlThread.h"
#include "mdtApplication.h"

#include <QtGlobal>
#include <QDateTime>
#include <cstdio>

#include <QByteArray>


void mdtSerialPortManagerTest::simpleTest()
{
  mdtSerialPortManager m;
  QStringList portsList;

  qDebug() << "* Be shure that the computer has at least one serial port, else test will fail *";

  // Verify that scan() function works ..
  portsList = m.scan();
  QVERIFY(portsList.size() > 0);

  // Init port manager
  m.port().config().setFrameType(mdtFrame::FT_ASCII);
  m.port().config().setEndOfFrameSeq("$");
  m.setPortName(portsList.at(0));
  QVERIFY(m.openPort());

  // Init port manager
  /**
  qDebug() << "TEST: setPortName() ...";
  m.setPortName(portsList.at(0));
  qDebug() << "TEST: setPortName() END";
  qDebug() << "TEST: openPort() ...";
  QVERIFY(m.openPort());
  qDebug() << "TEST: openPort() END";
  */

  // Check that available baud rates contains 9600 (very standard, should exists on most device)
  QVERIFY(m.port().availableBaudRates().size() > 0);
  QVERIFY(m.port().availableBaudRates().contains(9600));
  // Chack that the right port is set
  QVERIFY(m.port().portName() == portsList.at(0));

  // Start threads
  QVERIFY(m.start());
  QVERIFY(m.isRunning());


  // Send some data
  QVERIFY(m.writeData("Test$"));

  // Wait on answer - Timout: 500 [ms]
  QVERIFY(m.waitReadenFrame(500));

  // Verify received data
  QVERIFY(m.lastReadenFrame() == "Test");
}


int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtSerialPortManagerTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
