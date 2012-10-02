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
#include "mdtPortInfoTest.h"
#include "mdtApplication.h"
#include "mdtPortInfo.h"
#include "mdtDeviceInfo.h"
#include <QList>
#include <QVariant>

void mdtPortInfoTest::copyTest()
{
  mdtPortInfo pi1;
  mdtDeviceInfo *devi, *devi1, *devi2;
  QList<mdtDeviceInfo*> deviList;

  // Setup pi1
  pi1.setPortName("port1");
  devi = new mdtDeviceInfo;
  devi->setVendorId(1);
  devi->setProductId(11);
  pi1.addDevice(devi);
  deviList = pi1.deviceInfoList();
  QVERIFY(deviList.size() == 1);
  devi1 = deviList.at(0);
  QVERIFY(devi1->vendorId() == 1);
  QVERIFY(devi1->productId() == 11);

  // Create pi2 as copy of pi1
  mdtPortInfo pi2(pi1);
  QVERIFY(pi1.portName() == "port1");
  QVERIFY(pi2.portName() == "port1");
  deviList = pi2.deviceInfoList();
  QVERIFY(deviList.size() == 1);
  devi2 = deviList.at(0);
  QVERIFY(devi1 != devi2);
  QVERIFY(devi2->vendorId() == 1);
  QVERIFY(devi2->productId() == 11);
  // Check comparaison operators
  QVERIFY(pi1 == pi2);
  QVERIFY(!(pi1 != pi2));


  // Alter pi1
  pi1.setPortName("portA");
  QVERIFY(pi1.portName() == "portA");
  devi = new mdtDeviceInfo;
  devi->setVendorId(2);
  devi->setProductId(22);
  pi1.addDevice(devi);
  deviList = pi1.deviceInfoList();
  QVERIFY(deviList.size() == 2);
  devi = deviList.at(0);
  QVERIFY(devi->vendorId() == 1);
  QVERIFY(devi->productId() == 11);
  devi = deviList.at(1);
  QVERIFY(devi->vendorId() == 2);
  QVERIFY(devi->productId() == 22);
  // Check that pi2 is unchanged
  QVERIFY(pi2.portName() == "port1");
  deviList = pi2.deviceInfoList();
  QVERIFY(deviList.size() == 1);
  devi = deviList.at(0);
  QVERIFY(devi->vendorId() == 1);
  QVERIFY(devi->productId() == 11);
  // Check comparaison operators
  QVERIFY(pi1 != pi2);
  // Check that device info comparaison works
  pi2.setPortName("portA");
  QVERIFY(pi1 != pi2);
  devi = new mdtDeviceInfo;
  devi->setVendorId(10);
  devi->setProductId(110);
  pi2.addDevice(devi);
  QVERIFY(pi1 != pi2);

  // copy operator
  mdtPortInfo pi3;
  QVERIFY(pi3.portName() == "");
  QVERIFY(pi3.deviceInfoList().size() == 0);
  pi3 = pi1;
  QVERIFY(pi3.portName() == "portA");
  deviList = pi3.deviceInfoList();
  QVERIFY(deviList.size() == 2);
  devi = deviList.at(0);
  QVERIFY(devi->vendorId() == 1);
  QVERIFY(devi->productId() == 11);
  devi = deviList.at(1);
  QVERIFY(devi->vendorId() == 2);
  QVERIFY(devi->productId() == 22);

  // Assign same object (must not recurse or crach)
  pi1 = pi1;
}

void mdtPortInfoTest::qvariantTest()
{
  mdtPortInfo pi1, pi2;
  mdtDeviceInfo *devi;
  QList<mdtDeviceInfo*> deviList;
  QVariant var1;

  // Setup pi1
  pi1.setPortName("port1");
  devi = new mdtDeviceInfo;
  devi->setVendorId(1);
  devi->setProductId(11);
  pi1.addDevice(devi);
  deviList = pi1.deviceInfoList();
  QVERIFY(deviList.size() == 1);
  devi = deviList.at(0);
  QVERIFY(devi->vendorId() == 1);
  QVERIFY(devi->productId() == 11);

  // Store, get and check
  var1.setValue(pi1);
  pi2 = var1.value<mdtPortInfo>();
  QVERIFY(pi2 == pi1);
  QVERIFY(pi2.portName() == "port1");
  deviList = pi2.deviceInfoList();
  QVERIFY(deviList.size() == 1);
  devi = deviList.at(0);
  QVERIFY(devi->vendorId() == 1);
  QVERIFY(devi->productId() == 11);
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtPortInfoTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
