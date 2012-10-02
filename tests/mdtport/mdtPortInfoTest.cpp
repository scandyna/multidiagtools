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
#include "mdtPortInfoCbHandler.h"
#include <QList>
#include <QVariant>
#include <QComboBox>

void mdtPortInfoTest::copyTest()
{
  mdtPortInfo pi1;
  mdtDeviceInfo devi1;
  mdtDeviceInfo *pdevi, *pdevi1, *pdevi2;
  QList<mdtDeviceInfo*> deviList;

  /*
   * mdtDeviceInfo copy test
   */

  // Setup devi1
  devi1.setVendorId(1);
  devi1.setProductId(11);
  QVERIFY(devi1.vendorId() == 1);
  QVERIFY(devi1.productId() == 11);
  // Copy by constructor
  mdtDeviceInfo devi2(devi1);
  // Check copy
  QVERIFY(devi2.vendorId() == 1);
  QVERIFY(devi2.productId() == 11);
  // Check comparaison
  QVERIFY(devi1 == devi2);
  QVERIFY(!(devi1 != devi2));

  // Alter devi1
  devi1.setVendorId(3);
  devi1.setProductId(33);
  // Check that copy is leaved unchanged
  QVERIFY(devi2.vendorId() == 1);
  QVERIFY(devi2.productId() == 11);
  // Check comparaison
  QVERIFY(devi1 != devi2);
  QVERIFY(!(devi1 == devi2));

  // Check copy operator =
  devi2 = devi1;
  QVERIFY(devi2.vendorId() == 3);
  QVERIFY(devi2.productId() == 33);
  // Check comparaison
  QVERIFY(devi1 == devi2);
  QVERIFY(!(devi1 != devi2));

  /*
   * mdtPortInfo copy test
   */

  // Setup pi1
  pi1.setPortName("port1");
  pdevi = new mdtDeviceInfo;
  pdevi->setVendorId(1);
  pdevi->setProductId(11);
  pi1.addDevice(pdevi);
  deviList = pi1.deviceInfoList();
  QVERIFY(deviList.size() == 1);
  pdevi1 = deviList.at(0);
  QVERIFY(pdevi1->vendorId() == 1);
  QVERIFY(pdevi1->productId() == 11);

  // Create pi2 as copy of pi1
  mdtPortInfo pi2(pi1);
  QVERIFY(pi1.portName() == "port1");
  QVERIFY(pi2.portName() == "port1");
  deviList = pi2.deviceInfoList();
  QVERIFY(deviList.size() == 1);
  pdevi2 = deviList.at(0);
  QVERIFY(pdevi1 != pdevi2);
  QVERIFY(pdevi2->vendorId() == 1);
  QVERIFY(pdevi2->productId() == 11);
  // Check comparaison operators
  QVERIFY(pi1 == pi2);
  QVERIFY(!(pi1 != pi2));

  // Alter pi1
  pi1.setPortName("portA");
  QVERIFY(pi1.portName() == "portA");
  pdevi = new mdtDeviceInfo;
  pdevi->setVendorId(2);
  pdevi->setProductId(22);
  pi1.addDevice(pdevi);
  deviList = pi1.deviceInfoList();
  QVERIFY(deviList.size() == 2);
  pdevi = deviList.at(0);
  QVERIFY(pdevi->vendorId() == 1);
  QVERIFY(pdevi->productId() == 11);
  pdevi = deviList.at(1);
  QVERIFY(pdevi->vendorId() == 2);
  QVERIFY(pdevi->productId() == 22);
  // Check that pi2 is unchanged
  QVERIFY(pi2.portName() == "port1");
  deviList = pi2.deviceInfoList();
  QVERIFY(deviList.size() == 1);
  pdevi = deviList.at(0);
  QVERIFY(pdevi->vendorId() == 1);
  QVERIFY(pdevi->productId() == 11);
  // Check comparaison operators
  QVERIFY(pi1 != pi2);
  // Check that device info comparaison works
  pi2.setPortName("portA");
  QVERIFY(pi1 != pi2);
  pdevi = new mdtDeviceInfo;
  pdevi->setVendorId(10);
  pdevi->setProductId(110);
  pi2.addDevice(pdevi);
  QVERIFY(pi1 != pi2);

  // copy operator
  mdtPortInfo pi3;
  QVERIFY(pi3.portName() == "");
  QVERIFY(pi3.deviceInfoList().size() == 0);
  pi3 = pi1;
  QVERIFY(pi3.portName() == "portA");
  deviList = pi3.deviceInfoList();
  QVERIFY(deviList.size() == 2);
  pdevi = deviList.at(0);
  QVERIFY(pdevi->vendorId() == 1);
  QVERIFY(pdevi->productId() == 11);
  pdevi = deviList.at(1);
  QVERIFY(pdevi->vendorId() == 2);
  QVERIFY(pdevi->productId() == 22);

  // Assign same object (must not recurse or crach)
  pi1 = pi1;
}

void mdtPortInfoTest::qvariantTest()
{
  mdtPortInfo pi1, pi2;
  mdtDeviceInfo *devi;
  mdtDeviceInfo devi1, devi2;
  QList<mdtDeviceInfo*> deviList;
  QVariant var;

  /*
   * mdtDeviceInfo QVariant test
   */

  // Check that devi2 is empty
  QVERIFY(devi2.vendorId() == 0);
  QVERIFY(devi2.productId() == 0);

  // Setup devi1
  devi1.setVendorId(1);
  devi1.setProductId(11);
  QVERIFY(devi1.vendorId() == 1);
  QVERIFY(devi1.productId() == 11);

  // Store devi1 as QVariant
  var.setValue(devi1);

  // restore to devi2 and check
  devi2 = var.value<mdtDeviceInfo>();
  QVERIFY(devi2.vendorId() == 1);
  QVERIFY(devi2.productId() == 11);

  /*
   * mdtPortInfo QVariant test
   */

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
  var.setValue(pi1);
  pi2 = var.value<mdtPortInfo>();
  QVERIFY(pi2 == pi1);
  QVERIFY(pi2.portName() == "port1");
  deviList = pi2.deviceInfoList();
  QVERIFY(deviList.size() == 1);
  devi = deviList.at(0);
  QVERIFY(devi->vendorId() == 1);
  QVERIFY(devi->productId() == 11);
}

void mdtPortInfoTest::comboboxTest()
{
  QComboBox cbPorts;
  QComboBox cbDevices;
  mdtPortInfoCbHandler cbHandler;
  mdtPortInfo pi;
  mdtPortInfo *portInfo;
  QList<mdtPortInfo*> portInfoList;
  mdtDeviceInfo *devi;
  mdtDeviceInfo di;

  // Init the combobox handler
  cbHandler.setPortsComboBox(&cbPorts);
  cbHandler.setDevicesComboBox(&cbDevices);

  /*
   * Scan
   */

  // Setup port 1
  portInfo = new mdtPortInfo;
  portInfo->setPortName("port1");
  // Add some devices
  devi = new mdtDeviceInfo;
  devi->setVendorId(0x101);
  devi->setProductId(0x111);
  portInfo->addDevice(devi);
  devi = new mdtDeviceInfo;
  devi->setVendorId(0x102);
  devi->setProductId(0x122);
  portInfo->addDevice(devi);
  // Add port 1 to list
  portInfoList.append(portInfo);

  // Setup port 2
  portInfo = new mdtPortInfo;
  portInfo->setPortName("port2");
  // Add some devices
  devi = new mdtDeviceInfo;
  devi->setVendorId(0x201);
  devi->setProductId(0x211);
  portInfo->addDevice(devi);
  devi = new mdtDeviceInfo;
  devi->setVendorId(0x202);
  devi->setProductId(0x222);
  portInfo->addDevice(devi);
  // Add port 2 to list
  portInfoList.append(portInfo);

  // Setup port 3
  portInfo = new mdtPortInfo;
  portInfo->setPortName("port3");
  // Add some devices
  devi = new mdtDeviceInfo;
  devi->setVendorId(0x301);
  devi->setProductId(0x311);
  portInfo->addDevice(devi);
  // Add port 3 to list
  portInfoList.append(portInfo);

  // Fill comboboxes
  cbHandler.fillComboBoxes(portInfoList);

  // After fill, we must have port 1/device 1 selected
  pi = cbHandler.currentPortInfo();
  QVERIFY(pi.portName() == "port1");
  di = cbHandler.currentDeviceInfo();
  QVERIFY(di.vendorId() == 0x101);
  QVERIFY(di.productId() == 0x111);
  // Emulate a device selection
  cbDevices.setCurrentIndex(1);
  di = cbHandler.currentDeviceInfo();
  QVERIFY(di.vendorId() == 0x102);
  QVERIFY(di.productId() == 0x122);
  // Emulate a port selection
  cbPorts.setCurrentIndex(1);
  pi = cbHandler.currentPortInfo();
  QVERIFY(pi.portName() == "port2");
  di = cbHandler.currentDeviceInfo();
  QVERIFY(di.vendorId() == 0x201);
  QVERIFY(di.productId() == 0x211);
  // Emulate a invalid selection (must not crash)
  cbPorts.setCurrentIndex(-1);
  pi = cbHandler.currentPortInfo();
  QVERIFY(pi.portName() == "");
  di = cbHandler.currentDeviceInfo();
  QVERIFY(di.vendorId() == 0);
  QVERIFY(di.productId() == 0);
  // Emulate a port selection
  cbPorts.setCurrentIndex(2);
  pi = cbHandler.currentPortInfo();
  QVERIFY(pi.portName() == "port3");
  di = cbHandler.currentDeviceInfo();
  QVERIFY(di.vendorId() == 0x301);
  QVERIFY(di.productId() == 0x311);

  /*
   * Proceed a new scan
   */

  // Clear previous list
  qDeleteAll(portInfoList);
  portInfoList.clear();

  // Setup port 1
  portInfo = new mdtPortInfo;
  portInfo->setPortName("port10");
  // Add some devices
  devi = new mdtDeviceInfo;
  devi->setVendorId(0x1101);
  devi->setProductId(0x1111);
  portInfo->addDevice(devi);
  devi = new mdtDeviceInfo;
  devi->setVendorId(0x1102);
  devi->setProductId(0x1122);
  portInfo->addDevice(devi);
  // Add port 1 to list
  portInfoList.append(portInfo);

  // Setup port 2
  portInfo = new mdtPortInfo;
  portInfo->setPortName("port20");
  // Add some devices
  devi = new mdtDeviceInfo;
  devi->setVendorId(0x2201);
  devi->setProductId(0x2211);
  portInfo->addDevice(devi);
  devi = new mdtDeviceInfo;
  devi->setVendorId(0x2202);
  devi->setProductId(0x2222);
  portInfo->addDevice(devi);
  devi = new mdtDeviceInfo;
  devi->setVendorId(0x2203);
  devi->setProductId(0x2223);
  portInfo->addDevice(devi);
  // Add port 2 to list
  portInfoList.append(portInfo);

  // Fill comboboxes
  cbHandler.fillComboBoxes(portInfoList);

  // After fill, we must have port 1/device 1 selected
  pi = cbHandler.currentPortInfo();
  QVERIFY(pi.portName() == "port10");
  di = cbHandler.currentDeviceInfo();
  QVERIFY(di.vendorId() == 0x1101);
  QVERIFY(di.productId() == 0x1111);
  // Emulate a port and device selection
  cbPorts.setCurrentIndex(1);
  cbDevices.setCurrentIndex(1);
  pi = cbHandler.currentPortInfo();
  QVERIFY(pi.portName() == "port20");
  di = cbHandler.currentDeviceInfo();
  QVERIFY(di.vendorId() == 0x2202);
  QVERIFY(di.productId() == 0x2222);

  // Free
  qDeleteAll(portInfoList);
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
