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
#include "mdtUsbTmcPortTest.h"
#include <QString>
#include <QStringList>
#include "mdtApplication.h"
#include "linux/mdtUsbtmcPort.h"
#include "linux/mdtUsbtmcPortManager.h"

#include <QDebug>

void mdtUsbTmcPortTest::portManagerTest()
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

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtUsbTmcPortTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
