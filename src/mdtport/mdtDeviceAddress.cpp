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
#include "mdtDeviceAddress.h"
#include <QFile>

/*
 * mdtDeviceAddress implementation
 */

void mdtDeviceAddress::clear()
{
  pvAddressString.clear();
  pvAlias.clear();
  pvPortType = PortType_t::NONE;
  pvDeviceType = DeviceType_t::NONE;
  pvUsbDeviceAddress.idVendor = 0;
  pvUsbDeviceAddress.idProduct = 0;
  pvUsbDeviceAddress.bInterfaceNumber = 0;
  pvTcpipDeviceAddress.port = 0;
  pvTcpipDeviceAddress.port = 0;
  pvModbusHwNodeId.clear();
  pvHostName.clear();
  pvSerialNumber.clear();
}

QString mdtDeviceAddress::portTypeStr() const
{
  switch(pvPortType){
    case PortType_t::NONE:
      return "";
    case PortType_t::ASRL:
      return "ASRL";
    case PortType_t::TCPIP:
      return "TCPIP";
    case PortType_t::USB:
      return "USB";
    // No default section here, so compiler cann tell us if we forget to handle a type
  }
  return "";
}

QString mdtDeviceAddress::deviceTypeStr() const
{
  switch(pvDeviceType){
    case DeviceType_t::NONE:
      return "";
    case DeviceType_t::INSTR:
      return "INSTR";
    case DeviceType_t::SOCKET:
      return "SOCKET";
    case DeviceType_t::MODBUS:
      return "MODBUS";
    // No default section here, so compiler cann tell us if we forget to handle a type
  }
  return "";
}

void mdtDeviceAddress::setModbusTcpIdentification(const QString & hostName, uint16_t port, const mdtModbusHwNodeId & nid)
{
  clear();
  pvPortType = PortType_t::TCPIP;
  pvDeviceType = DeviceType_t::MODBUS;
  pvHostName = hostName;
  pvTcpipDeviceAddress.port = port;
  pvModbusHwNodeId = nid;
  QString portStr;
  if(port > 0){
    portStr = QString::number(port);
  }
  pvAddressString = "TCPIP::" + pvHostName + "::" + portStr + "::MODBUS::" + \
                    QString::number(nid.id()) + "," + QString::number(nid.bitsCount()) + "," + QString::number(nid.firstBit());
}


bool mdtDeviceAddress::decodeUsbAddressString(const QStringList& lst)
{
  Q_ASSERT(lst.size() >= 2);
  Q_ASSERT(lst.at(0) == "USB");

  bool ok;

  pvPortType = PortType_t::USB;
  pvDeviceType = DeviceType_t::INSTR;
  // Check that we have expected number of segments
  if(lst.size() < 4){
    pvLastError.setError("'" + pvAddressString + "' " + tr("contains a invalid USB address string. "\
                         "Expected at least 4 segments (USB::manufacturerID::ProductID::SerialNumber)"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddress");
    pvLastError.commit();
    clear();
    return false;
  }
  Q_ASSERT(lst.size() >= 4);
  // Extract idVendor
  pvUsbDeviceAddress.idVendor = lst.at(1).toUInt(&ok, 16);
  if(!ok){
    pvLastError.setError(tr("Could not decode idVendor in address string '") + pvAddressString + tr("'.") , mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddress");
    pvLastError.commit();
    clear();
    return false;
  }
  // Extract idProduct
  pvUsbDeviceAddress.idProduct = lst.at(2).toUInt(&ok, 16);
  if(!ok){
    pvLastError.setError(tr("Could not decode idProduct in address string '") + pvAddressString + tr("'.") , mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddress");
    pvLastError.commit();
    clear();
    return false;
  }
  // Extract serial number
  pvSerialNumber = lst.at(3);
  // Extract bInterfaceNumber if given
  if( (lst.size() > 4) && (lst.at(4) != "INSTR") ){
    pvUsbDeviceAddress.bInterfaceNumber = lst.at(4).toUInt(&ok);
    if(!ok){
      pvLastError.setError(tr("Could not decode USB interface number in address string '") + pvAddressString + tr("'.") , mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddress");
      pvLastError.commit();
      clear();
      return false;
    }
  }

  return true;
}

bool mdtDeviceAddress::decodeTcpipAddressString(const QStringList & lst)
{
  Q_ASSERT(lst.size() >= 2);
  Q_ASSERT(lst.at(0) == "TCPIP");

  pvPortType = PortType_t::TCPIP;
  // Call appropriate tcp based device decode function
  if((lst.size() == 4) && (lst.at(3) == "SOCKET")){
    return decodeTcpipRawSocketAddressString(lst);
  }
  if((lst.size() >= 4) && (lst.at(3) == "MODBUS")){
    return decodeModbusTcpAddressString(lst);
  }
  // Here, we have a invalid or unsupported TCP/IP address string
  pvLastError.setError("'" + pvAddressString + "' " + tr("contains a invalid or not supported TCPIP address string."), mdtError::Error);
  MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddress");
  pvLastError.commit();
  clear();
  return false;
}

bool mdtDeviceAddress::decodeTcpipRawSocketAddressString(const QStringList& lst)
{
  Q_ASSERT(lst.size() == 4);
  Q_ASSERT(lst.at(3) == "SOCKET");

  bool ok;

  pvHostName = lst.at(1);
  pvTcpipDeviceAddress.port = lst.at(2).toUInt(&ok);
  if(!ok){
    pvLastError.setError(tr("Could not decode port number in address string '") + pvAddressString + tr("'.") , mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddress");
    pvLastError.commit();
    clear();
    return false;
  }
  pvDeviceType = DeviceType_t::SOCKET;

  return true;
}

bool mdtDeviceAddress::decodeModbusTcpAddressString(const QStringList& lst)
{
  Q_ASSERT(lst.size() >= 4);
  Q_ASSERT(lst.size() <= 5);
  Q_ASSERT(lst.at(3) == "MODBUS");

  bool ok;

  // Extract host name and port
  pvHostName = lst.at(1);
  if(!lst.at(2).isEmpty()){
    pvTcpipDeviceAddress.port = lst.at(2).toUInt(&ok);
    if(!ok){
      pvLastError.setError(tr("Could not decode port number in address string '") + pvAddressString + tr("'.") , mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddress");
      pvLastError.commit();
      clear();
      return false;
    }
  }
  pvDeviceType = DeviceType_t::MODBUS;
  if(lst.size() < 5){
    return true;
  }
  // Extract MODBUS hardware node address if specified
  Q_ASSERT(lst.size() == 5);
  QStringList nid = lst.at(4).split(",");
  if(nid.size() != 3){
    pvLastError.setError(tr("Could not decode MODBUS hardware node ID in address string '") + pvAddressString + \
                          tr("'. Expected 3 parameters (ID,bitsCount,firstBit)") , mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddress");
    pvLastError.commit();
    clear();
    return false;
  }
  Q_ASSERT(nid.size() == 3);
  // Node ID
  int id = nid.at(0).toInt(&ok);
  if(!ok){
    pvLastError.setError(tr("Could not decode MODBUS hardware node ID in address string '") + pvAddressString + \
                          tr("'. ID part is not a number.") , mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddress");
    pvLastError.commit();
    clear();
    return false;
  }
  // Bits count
  int bc = nid.at(1).toInt(&ok);
  if(!ok){
    pvLastError.setError(tr("Could not decode MODBUS hardware node ID in address string '") + pvAddressString + \
                          tr("'. bitsCount part is not a number.") , mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddress");
    pvLastError.commit();
    clear();
    return false;
  }
  // First bit
  int fb = nid.at(2).toInt(&ok);
  if(!ok){
    pvLastError.setError(tr("Could not decode MODBUS hardware node ID in address string '") + pvAddressString + \
                          tr("'. firstBit part is not a number.") , mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddress");
    pvLastError.commit();
    clear();
    return false;
  }
  // Ok, store node ID
  pvModbusHwNodeId.setId(id, bc, fb);

  return true;
}

/*
 * mdtDeviceAddressList implementation
 */

bool mdtDeviceAddressList::saveToFile(const QString & filePath)
{
  QFile file(filePath);
  QByteArray line;

  // Try to open file
  if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
    pvLastError.setError(tr("Cannot save device addresses to file '") + file.fileName() + tr("' (cannot open file for write)"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddressList");
    pvLastError.commit();
    return false;
  }
  // Save items
  for(const auto & da : pvDeviceAddresses){
//     line = da.addressString().toAscii();
    line = da.addressString().toLocal8Bit();
    line += "\t" + da.alias();
    line += "\n";
    if(file.write(line) < 0){
      pvLastError.setError(tr("Cannot save device addresses to file '") + file.fileName() + tr("' (write error occured)"), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddressList");
      pvLastError.commit();
      file.close();
      return false;
    }
  }
  file.close();

  return true;
}

bool mdtDeviceAddressList::readFromFile(const QString& filePath)
{
  QFile file(filePath);
  QList<QByteArray> lineData;

  // Try to open file
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
    pvLastError.setError(tr("Cannot open device addresses file '") + file.fileName() + tr("' (cannot open file for read)"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddressList");
    pvLastError.commit();
    return false;
  }
  // Read entries
  while(!file.atEnd()){
    mdtDeviceAddress da;
    lineData = file.readLine().trimmed().split('\t');
    if(!lineData.isEmpty()){
      // Extract address string
      if(!da.setAddressString(lineData.at(0))){
        pvLastError = da.lastError();
        file.close();
        return false;
      }
      // Extract alias
      if(lineData.size() > 1){
        da.setAlias(lineData.at(1));
      }
    }
    // Add device address
    append(da);
  }
  file.close();

  return true;
}
