/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtDeviceAddressTest.h"
#include "mdtDeviceAddress.h"
#include "mdtApplication.h"

#include <QDebug>

void mdtDeviceAddressTest::simpleTest()
{
  mdtDeviceAddress da;

  qDebug() << "mdtDeviceAddress size: " << sizeof(da);
  /*
   * Initial state
   */
  QVERIFY(!da.isValid());
  QCOMPARE((int)da.usbIdVendor(), 0);
  QCOMPARE((int)da.usbIdProduct(), 0);
  QCOMPARE((int)da.usbInterfaceNumber(), 0);
  QCOMPARE(da.usbDeviceSerialNumber(), QString(""));
  /*
   * Check USB identification
   */
  da.setUsbIdentification(0x1234, 0x5678, "ABCD", 1);
  QVERIFY(da.isValid());
  QCOMPARE((int)da.usbIdVendor(), 0x1234);
  QCOMPARE((int)da.usbIdProduct(), 0x5678);
  QCOMPARE((int)da.usbInterfaceNumber(), 1);
  QCOMPARE(da.usbDeviceSerialNumber(), QString("ABCD"));

  
  /*
   * Check clear
   */
  da.clear();
  QVERIFY(!da.isValid());
  QCOMPARE((int)da.usbIdVendor(), 0);
  QCOMPARE((int)da.usbIdProduct(), 0);
  QCOMPARE((int)da.usbInterfaceNumber(), 0);
  QCOMPARE(da.usbDeviceSerialNumber(), QString(""));

}


/*
 * Main
 */

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtDeviceAddressTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
