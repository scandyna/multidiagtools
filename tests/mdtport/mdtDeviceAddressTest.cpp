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
#include "mdtModbusHwNodeId.h"
#include "mdtApplication.h"
#include <vector>

#include <QDebug>

void mdtDeviceAddressTest::modbusHwNodeIdTest()
{
  mdtModbusHwNodeId nid;
  std::vector<bool> bits;

  qDebug() << "mdtModbusHwNodeId size: " << sizeof(nid);
  /*
   * Initial state
   */
  QVERIFY(nid.isNull());
  QCOMPARE(nid.id(), 0);
  QCOMPARE(nid.bitsCount(), 0);
  QCOMPARE(nid.firstBit(), 0);
  /*
   * Set ID
   */
  nid.setId(20, 7, 1);
  QVERIFY(!nid.isNull());
  QCOMPARE(nid.id(), 20);
  QCOMPARE(nid.bitsCount(), 7);
  QCOMPARE(nid.firstBit(), 1);
  /*
   * Clear
   */
  nid.clear();
  QVERIFY(nid.isNull());
  QCOMPARE(nid.id(), 0);
  QCOMPARE(nid.bitsCount(), 0);
  QCOMPARE(nid.firstBit(), 0);
  /*
   * Set ID
   */
  nid.setId(20, 7, 1);
  QVERIFY(!nid.isNull());
  QCOMPARE(nid.id(), 20);
  QCOMPARE(nid.bitsCount(), 7);
  QCOMPARE(nid.firstBit(), 1);
  /*
   * Set ID from bits
   */
  bits.clear();
  // Setup bits: 0b10010110
  bits.push_back(false);
  bits.push_back(true);
  bits.push_back(true);
  bits.push_back(false);
  bits.push_back(true);
  bits.push_back(false);
  bits.push_back(false);
  bits.push_back(true);
  // Set ID - All bits are to be used
  nid.setId(bits, 8);
  QVERIFY(!nid.isNull());
  QCOMPARE(nid.id(), 0b10010110);
  QCOMPARE(nid.bitsCount(), 8);
  QCOMPARE(nid.firstBit(), 1);
  // Set ID - Only 4 first bytes are to be used (note: mdtModbusHwNodeId::firstBit has no influence)
  nid.setId(bits, 4);
  QVERIFY(!nid.isNull());
  QCOMPARE(nid.id(), 0b0110);
  QCOMPARE(nid.bitsCount(), 4);
  QCOMPARE(nid.firstBit(), 1);
  // Set ID - All bits are to be used
  nid.setId(bits);
  QVERIFY(!nid.isNull());
  QCOMPARE(nid.id(), 0b10010110);
  QCOMPARE(nid.bitsCount(), 8);
  QCOMPARE(nid.firstBit(), 1);

}

void mdtDeviceAddressTest::modbusHwNodeIdBenchmark()
{
  mdtModbusHwNodeId nid(2, 8, 0);
  std::vector<bool> bits;

  // Setup bits: 0b10010110
  bits.push_back(false);
  bits.push_back(true);
  bits.push_back(true);
  bits.push_back(false);
  bits.push_back(true);
  bits.push_back(false);
  bits.push_back(false);
  bits.push_back(true);
  QBENCHMARK{
    nid.setId(bits);
  }
  QCOMPARE(nid.id(), 0b10010110);
  QCOMPARE(nid.bitsCount(), 8);
  QCOMPARE(nid.firstBit(), 0);
}

void mdtDeviceAddressTest::simpleTest()
{
  mdtDeviceAddress da;

  qDebug() << "mdtDeviceAddress size: " << sizeof(da);
  /*
   * Initial state
   */
  QVERIFY(!da.isValid());
  QVERIFY(da.portType() == mdtDeviceAddress::PortType_t::NONE);
  QCOMPARE(da.portTypeStr(), QString(""));
  QVERIFY(da.deviceType() == mdtDeviceAddress::DeviceType_t::NONE);
  QCOMPARE(da.deviceTypeStr(), QString(""));
  QCOMPARE((int)da.usbIdVendor(), 0);
  QCOMPARE((int)da.usbIdProduct(), 0);
  QCOMPARE((int)da.usbInterfaceNumber(), 0);
  QCOMPARE(da.usbDeviceSerialNumber(), QString(""));
  QCOMPARE(da.tcpIpHostName(), QString(""));
  QCOMPARE((int)da.tcpIpPort(), 0);
  QCOMPARE(da.modbusHwNodeId().id(), 0);
  QCOMPARE(da.modbusHwNodeId().bitsCount(), 0);
  QCOMPARE(da.modbusHwNodeId().firstBit(), 0);
  QCOMPARE(da.addressString(), QString(""));
  /*
   * Check USB identification
   */
  da.setUsbIdentification(0x1234, 0x5678, "ABCD", 1);
  QVERIFY(da.isValid());
  QVERIFY(da.portType() == mdtDeviceAddress::PortType_t::USB);
  QCOMPARE(da.portTypeStr(), QString("USB"));
  QVERIFY(da.deviceType() == mdtDeviceAddress::DeviceType_t::INSTR);
  QCOMPARE(da.deviceTypeStr(), QString("INSTR"));
  QCOMPARE((int)da.usbIdVendor(), 0x1234);
  QCOMPARE((int)da.usbIdProduct(), 0x5678);
  QCOMPARE((int)da.usbInterfaceNumber(), 1);
  QCOMPARE(da.usbDeviceSerialNumber(), QString("ABCD"));
  QCOMPARE(da.tcpIpHostName(), QString(""));
  QCOMPARE((int)da.tcpIpPort(), 0);
  QCOMPARE(da.modbusHwNodeId().id(), 0);
  QCOMPARE(da.modbusHwNodeId().bitsCount(), 0);
  QCOMPARE(da.modbusHwNodeId().firstBit(), 0);
  QCOMPARE(da.addressString(), QString("USB::0x1234::0x5678::ABCD::1::INSTR"));
  /*
   * Check raw TCP/IP socket identification
   */
  da.setTcpIpIdentification("1.2.3.4", 56789, mdtDeviceAddress::DeviceType_t::SOCKET);
  QVERIFY(da.isValid());
  QVERIFY(da.portType() == mdtDeviceAddress::PortType_t::TCPIP);
  QCOMPARE(da.portTypeStr(), QString("TCPIP"));
  QVERIFY(da.deviceType() == mdtDeviceAddress::DeviceType_t::SOCKET);
  QCOMPARE(da.deviceTypeStr(), QString("SOCKET"));
  QCOMPARE((int)da.usbIdVendor(), 0);
  QCOMPARE((int)da.usbIdProduct(), 0);
  QCOMPARE((int)da.usbInterfaceNumber(), 0);
  QCOMPARE(da.usbDeviceSerialNumber(), QString(""));
  QCOMPARE(da.tcpIpHostName(), QString("1.2.3.4"));
  QCOMPARE((int)da.tcpIpPort(), 56789);
  QCOMPARE(da.modbusHwNodeId().id(), 0);
  QCOMPARE(da.modbusHwNodeId().bitsCount(), 0);
  QCOMPARE(da.modbusHwNodeId().firstBit(), 0);
  QCOMPARE(da.addressString(), QString("TCPIP::1.2.3.4::56789::SOCKET"));
  /*
   * Check MODBUS/TCP identification
   */
  da.setModbusTcpIdentification("4.3.2.1", 8765, mdtModbusHwNodeId(5, 8, 1));
  QVERIFY(da.isValid());
  QVERIFY(da.portType() == mdtDeviceAddress::PortType_t::TCPIP);
  QCOMPARE(da.portTypeStr(), QString("TCPIP"));
  QVERIFY(da.deviceType() == mdtDeviceAddress::DeviceType_t::MODBUS);
  QCOMPARE(da.deviceTypeStr(), QString("MODBUS"));
  QCOMPARE((int)da.usbIdVendor(), 0);
  QCOMPARE((int)da.usbIdProduct(), 0);
  QCOMPARE((int)da.usbInterfaceNumber(), 0);
  QCOMPARE(da.usbDeviceSerialNumber(), QString(""));
  QCOMPARE(da.tcpIpHostName(), QString("4.3.2.1"));
  QCOMPARE((int)da.tcpIpPort(), 8765);
  QCOMPARE(da.modbusHwNodeId().id(), 5);
  QCOMPARE(da.modbusHwNodeId().bitsCount(), 8);
  QCOMPARE(da.modbusHwNodeId().firstBit(), 1);
  QCOMPARE(da.addressString(), QString("TCPIP::4.3.2.1::8765::MODBUS::5,8,1"));
  /*
   * Check clear
   */
  da.clear();
  QVERIFY(!da.isValid());
  QVERIFY(da.portType() == mdtDeviceAddress::PortType_t::NONE);
  QCOMPARE(da.portTypeStr(), QString(""));
  QVERIFY(da.deviceType() == mdtDeviceAddress::DeviceType_t::NONE);
  QCOMPARE(da.deviceTypeStr(), QString(""));
  QCOMPARE((int)da.usbIdVendor(), 0);
  QCOMPARE((int)da.usbIdProduct(), 0);
  QCOMPARE((int)da.usbInterfaceNumber(), 0);
  QCOMPARE(da.usbDeviceSerialNumber(), QString(""));
  QCOMPARE(da.tcpIpHostName(), QString(""));
  QCOMPARE((int)da.tcpIpPort(), 0);
  QCOMPARE(da.modbusHwNodeId().id(), 0);
  QCOMPARE(da.modbusHwNodeId().bitsCount(), 0);
  QCOMPARE(da.modbusHwNodeId().firstBit(), 0);
  QCOMPARE(da.addressString(), QString(""));


}

void mdtDeviceAddressTest::decodeAddressStringTest()
{
  mdtDeviceAddress da;

  /*
   * Check Raw TCP/IP socket addresse string parsing
   */
  QVERIFY(da.setAddressString("TCPIP::2.5.8.9::512::SOCKET"));
  QVERIFY(da.isValid());
  QVERIFY(da.portType() == mdtDeviceAddress::PortType_t::TCPIP);
  QCOMPARE(da.portTypeStr(), QString("TCPIP"));
  QVERIFY(da.deviceType() == mdtDeviceAddress::DeviceType_t::SOCKET);
  QCOMPARE(da.deviceTypeStr(), QString("SOCKET"));
  QCOMPARE((int)da.usbIdVendor(), 0);
  QCOMPARE((int)da.usbIdProduct(), 0);
  QCOMPARE((int)da.usbInterfaceNumber(), 0);
  QCOMPARE(da.usbDeviceSerialNumber(), QString(""));
  QCOMPARE(da.tcpIpHostName(), QString("2.5.8.9"));
  QCOMPARE((int)da.tcpIpPort(), 512);
  QCOMPARE(da.modbusHwNodeId().id(), 0);
  QCOMPARE(da.modbusHwNodeId().bitsCount(), 0);
  QCOMPARE(da.modbusHwNodeId().firstBit(), 0);
  QCOMPARE(da.addressString(), QString("TCPIP::2.5.8.9::512::SOCKET"));
  // Check the same with spaces at begin and end
  QVERIFY(da.setAddressString(" TCPIP::1.4.7.8::1024::SOCKET "));
  QVERIFY(da.isValid());
  QVERIFY(da.portType() == mdtDeviceAddress::PortType_t::TCPIP);
  QCOMPARE(da.portTypeStr(), QString("TCPIP"));
  QVERIFY(da.deviceType() == mdtDeviceAddress::DeviceType_t::SOCKET);
  QCOMPARE(da.deviceTypeStr(), QString("SOCKET"));
  QCOMPARE((int)da.usbIdVendor(), 0);
  QCOMPARE((int)da.usbIdProduct(), 0);
  QCOMPARE((int)da.usbInterfaceNumber(), 0);
  QCOMPARE(da.usbDeviceSerialNumber(), QString(""));
  QCOMPARE(da.tcpIpHostName(), QString("1.4.7.8"));
  QCOMPARE((int)da.tcpIpPort(), 1024);
  QCOMPARE(da.modbusHwNodeId().id(), 0);
  QCOMPARE(da.modbusHwNodeId().bitsCount(), 0);
  QCOMPARE(da.modbusHwNodeId().firstBit(), 0);
  QCOMPARE(da.addressString(), QString("TCPIP::1.4.7.8::1024::SOCKET"));
  /*
   * Check Raw MODBUS/TCP addresse string parsing
   */
  QVERIFY(da.setAddressString("TCPIP::1.5.9.8::502::MODBUS::2,8,1"));
  QVERIFY(da.isValid());
  QVERIFY(da.portType() == mdtDeviceAddress::PortType_t::TCPIP);
  QCOMPARE(da.portTypeStr(), QString("TCPIP"));
  QVERIFY(da.deviceType() == mdtDeviceAddress::DeviceType_t::MODBUS);
  QCOMPARE(da.deviceTypeStr(), QString("MODBUS"));
  QCOMPARE((int)da.usbIdVendor(), 0);
  QCOMPARE((int)da.usbIdProduct(), 0);
  QCOMPARE((int)da.usbInterfaceNumber(), 0);
  QCOMPARE(da.usbDeviceSerialNumber(), QString(""));
  QCOMPARE(da.tcpIpHostName(), QString("1.5.9.8"));
  QCOMPARE((int)da.tcpIpPort(), 502);
  QCOMPARE(da.modbusHwNodeId().id(), 2);
  QCOMPARE(da.modbusHwNodeId().bitsCount(), 8);
  QCOMPARE(da.modbusHwNodeId().firstBit(), 1);
  QCOMPARE(da.addressString(), QString("TCPIP::1.5.9.8::502::MODBUS::2,8,1"));
  /*
   * Check USB address string parsing
   *  -> Interface number not specified
   *  -> INSTR not specified
   */
  QVERIFY(da.setAddressString("USB::0x1234::0x5678::SN889"));
  QVERIFY(da.isValid());
  QVERIFY(da.portType() == mdtDeviceAddress::PortType_t::USB);
  QCOMPARE(da.portTypeStr(), QString("USB"));
  QVERIFY(da.deviceType() == mdtDeviceAddress::DeviceType_t::INSTR);
  QCOMPARE(da.deviceTypeStr(), QString("INSTR"));
  QCOMPARE((int)da.usbIdVendor(), 0x1234);
  QCOMPARE((int)da.usbIdProduct(), 0x5678);
  QCOMPARE((int)da.usbInterfaceNumber(), 0);
  QCOMPARE(da.usbDeviceSerialNumber(), QString("SN889"));
  QCOMPARE(da.tcpIpHostName(), QString(""));
  QCOMPARE((int)da.tcpIpPort(), 0);
  QCOMPARE(da.modbusHwNodeId().id(), 0);
  QCOMPARE(da.modbusHwNodeId().bitsCount(), 0);
  QCOMPARE(da.modbusHwNodeId().firstBit(), 0);
  QCOMPARE(da.addressString(), QString("USB::0x1234::0x5678::SN889"));
  /*
   * Check USB address string parsing
   *  -> Interface number not specified
   *  -> INSTR specified
   */
  QVERIFY(da.setAddressString("USB::0x1234::0x5678::SN889::INSTR"));
  QVERIFY(da.isValid());
  QVERIFY(da.portType() == mdtDeviceAddress::PortType_t::USB);
  QCOMPARE(da.portTypeStr(), QString("USB"));
  QVERIFY(da.deviceType() == mdtDeviceAddress::DeviceType_t::INSTR);
  QCOMPARE(da.deviceTypeStr(), QString("INSTR"));
  QCOMPARE((int)da.usbIdVendor(), 0x1234);
  QCOMPARE((int)da.usbIdProduct(), 0x5678);
  QCOMPARE((int)da.usbInterfaceNumber(), 0);
  QCOMPARE(da.usbDeviceSerialNumber(), QString("SN889"));
  QCOMPARE(da.tcpIpHostName(), QString(""));
  QCOMPARE((int)da.tcpIpPort(), 0);
  QCOMPARE(da.modbusHwNodeId().id(), 0);
  QCOMPARE(da.modbusHwNodeId().bitsCount(), 0);
  QCOMPARE(da.modbusHwNodeId().firstBit(), 0);
  QCOMPARE(da.addressString(), QString("USB::0x1234::0x5678::SN889::INSTR"));
  /*
   * Check USB address string parsing
   *  -> Interface number specified
   *  -> INSTR not specified
   */
  QVERIFY(da.setAddressString("USB::0x1234::0x5678::SN889::1"));
  QVERIFY(da.isValid());
  QVERIFY(da.portType() == mdtDeviceAddress::PortType_t::USB);
  QCOMPARE(da.portTypeStr(), QString("USB"));
  QVERIFY(da.deviceType() == mdtDeviceAddress::DeviceType_t::INSTR);
  QCOMPARE(da.deviceTypeStr(), QString("INSTR"));
  QCOMPARE((int)da.usbIdVendor(), 0x1234);
  QCOMPARE((int)da.usbIdProduct(), 0x5678);
  QCOMPARE((int)da.usbInterfaceNumber(), 1);
  QCOMPARE(da.usbDeviceSerialNumber(), QString("SN889"));
  QCOMPARE(da.tcpIpHostName(), QString(""));
  QCOMPARE((int)da.tcpIpPort(), 0);
  QCOMPARE(da.modbusHwNodeId().id(), 0);
  QCOMPARE(da.modbusHwNodeId().bitsCount(), 0);
  QCOMPARE(da.modbusHwNodeId().firstBit(), 0);
  QCOMPARE(da.addressString(), QString("USB::0x1234::0x5678::SN889::1"));
  /*
   * Check USB address string parsing
   *  -> Interface number specified
   *  -> INSTR specified
   */
  QVERIFY(da.setAddressString("USB::0x1234::0x5678::SN889::2::INSTR"));
  QVERIFY(da.isValid());
  QVERIFY(da.portType() == mdtDeviceAddress::PortType_t::USB);
  QCOMPARE(da.portTypeStr(), QString("USB"));
  QVERIFY(da.deviceType() == mdtDeviceAddress::DeviceType_t::INSTR);
  QCOMPARE(da.deviceTypeStr(), QString("INSTR"));
  QCOMPARE((int)da.usbIdVendor(), 0x1234);
  QCOMPARE((int)da.usbIdProduct(), 0x5678);
  QCOMPARE((int)da.usbInterfaceNumber(), 2);
  QCOMPARE(da.usbDeviceSerialNumber(), QString("SN889"));
  QCOMPARE(da.tcpIpHostName(), QString(""));
  QCOMPARE((int)da.tcpIpPort(), 0);
  QCOMPARE(da.modbusHwNodeId().id(), 0);
  QCOMPARE(da.modbusHwNodeId().bitsCount(), 0);
  QCOMPARE(da.modbusHwNodeId().firstBit(), 0);
  QCOMPARE(da.addressString(), QString("USB::0x1234::0x5678::SN889::2::INSTR"));
  /*
   * Currently not supported address strings
   */
  QVERIFY(!da.setAddressString("VXI::1::INSTR"));
  QVERIFY(!da.isValid());
  QVERIFY(!da.setAddressString("GPIB::1::0::INSTR"));
  QVERIFY(!da.isValid());
  QVERIFY(!da.setAddressString("ASRL1::INSTR"));
  QVERIFY(!da.isValid());
  QVERIFY(!da.setAddressString("TCPIP::1.2.3.4::inst0::INSTR"));
  QVERIFY(!da.isValid());

}

void mdtDeviceAddressTest::decodeAddressStringBenchmark()
{
  mdtDeviceAddress da;
  bool ok;

  QBENCHMARK{
    ok = da.setAddressString("TCPIP::1.5.9.8::502::MODBUS::2,8,1");
  }
  QVERIFY(ok);
  QVERIFY(da.isValid());
  QVERIFY(da.portType() == mdtDeviceAddress::PortType_t::TCPIP);
  QCOMPARE(da.portTypeStr(), QString("TCPIP"));
  QVERIFY(da.deviceType() == mdtDeviceAddress::DeviceType_t::MODBUS);
  QCOMPARE(da.deviceTypeStr(), QString("MODBUS"));
  QCOMPARE((int)da.usbIdVendor(), 0);
  QCOMPARE((int)da.usbIdProduct(), 0);
  QCOMPARE((int)da.usbInterfaceNumber(), 0);
  QCOMPARE(da.usbDeviceSerialNumber(), QString(""));
  QCOMPARE(da.tcpIpHostName(), QString("1.5.9.8"));
  QCOMPARE((int)da.tcpIpPort(), 502);
  QCOMPARE(da.modbusHwNodeId().id(), 2);
  QCOMPARE(da.modbusHwNodeId().bitsCount(), 8);
  QCOMPARE(da.modbusHwNodeId().firstBit(), 1);
  QCOMPARE(da.addressString(), QString("TCPIP::1.5.9.8::502::MODBUS::2,8,1"));
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
