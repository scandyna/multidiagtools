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
#ifndef MDT_DEVICE_ADDRESS_H
#define MDT_DEVICE_ADDRESS_H

#include "mdtError.h"
#include "mdtModbusHwNodeId.h"
#include <QString>
#include <QStringList>
#include <QObject>
#include <cstdint>

/*
 * USB specific (only trivial) members
 */
struct mdtUsbDeviceAddress
{
  uint16_t idVendor;
  uint16_t idProduct;
  uint8_t bInterfaceNumber;
};

/*
 * TCPIP specific members
 */
struct mdtTcpipDeviceAddress
{
  uint16_t port;
};

/*
 * ASRL specific members
 */
struct mdtAsrlDeviceAddress
{
};

/*! \brief Common device identification
 *
 * In multiDiagTools library, each port or device class has its own
 *  methods to open a ressource.
 *
 * In some case, it's necessary to identify, f.ex. a device, in a common way.
 *  Storing this identification in a databse or a file can also be usefull.
 *
 * To resole above problems, this class is inspired from VISA (Virtual Instrument Software Architecture) library specifications.
 *  The most important part is in VPP-4.3: The VISA Library, revision 5.4 , chapiter 4.3.1 Address String.
 *  This specification is available by the IVI Foundation: http://www.ivifoundation.org
 *
 * Note: current version has only a partial support (USB and part of TCPIP)
 */
class mdtDeviceAddress
{
 public:

  /*! \brief Port type
   *
   * In VPP-4.3 revision 5.4 , chapiter 4.3.1,
   *  the term Interface is used.
   *  To avoid confusions, f.ex. with USB interface,
   *  and to match mith multiDiagTools library class names,
   *  it was choosen to name it port.
   */
  enum class PortType_t
  {
    NONE,   /*!< No port. Set by default for a invalid device address */
    TCPIP,  /*!< TCPIP port type */
    USB,    /*!< USB port type */
    ASRL    /*!< Asynchronous serial line (f.ex. RS232) */
  };

  /*! \brief Device type
   *
   * Refers to resource class described
   *  in VPP-4.3 revision 5.4 , chapiter 5.
   *
   * Some devices type are also not from VPP-4.3 specifications.
   */
  enum class DeviceType_t
  {
    NONE,   /*!< No device type. Set by default for a invalid device address */
    INSTR,  /*!< Device that supports block data exchange, clear and trigger commands,
                  status registers. Example can be a SCPI instrument.
                  See VPP-4.3 revision 5.4 , chapiter 5.1 .*/
    SOCKET, /*!< Raw socket connection using TCP/IP,
                  See VPP-4.3 revision 5.4 , chapiter 5.6 .*/
    MODBUS  /*!< Device that support MODBUS protocol. Not sepcified in VPP-4.3 revision 5.4 */
  };

  /*! \brief Construct a invalid device address
   */
  mdtDeviceAddress()
   : pvPortType(PortType_t::NONE),
     pvDeviceType(DeviceType_t::NONE)
  {
    pvUsbDeviceAddress.idVendor = 0;
    pvUsbDeviceAddress.idProduct = 0;
    pvUsbDeviceAddress.bInterfaceNumber = 0;
    pvTcpipDeviceAddress.port = 0;
  }

  /*! \brief Check if device address is valid
   */
  bool isValid() const
  {
    return (pvPortType != PortType_t::NONE);
  }

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

  /*! \brief Clear device address
   */
  void clear();

  /*! \brief Get port type
   */
  PortType_t portType() const
  {
    return pvPortType;
  }

  /*! \brief Get port type string
   */
  QString portTypeStr() const;

  /*! \brief Get device type
   */
  DeviceType_t deviceType() const
  {
    return pvDeviceType;
  }

  /*! \brief Get device type string
   */
  QString deviceTypeStr() const;

  /*! \brief Set address string
   *
   * \sa addressString()
   */
  bool setAddressString(const QString & as)
  {
    QStringList lst = as.trimmed().split("::");

    clear();
    if(lst.size() < 2){
      pvLastError.setError("'" + as + "' " + tr("contains not a valid address string. Expected at least 2 segments (Port::Something)."), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddress");
      pvLastError.commit();
      return false;
    }
    // We store address string, can be helpful for error messages (if a called decode function fails, it will call clear() ).
    pvAddressString = as.trimmed();
    // Call appropriate decode function
    QString str = lst.at(0);
    if(str == "TCPIP"){
      return decodeTcpipAddressString(lst);
    }
    if(str == "USB"){
      return decodeUsbAddressString(lst);
    }
    // Error - not supported port type
    pvLastError.setError("'" + pvAddressString + "' " + tr("contains a invalid or not supported address string."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddress");
    pvLastError.commit();
    clear();
    return false;
  }

  /*! \brief Get address string
   *
   * Return the address string as described in VPP-4.3 revision 5.4 , chapiter 4.3.1
   *  for supported port type (named interface in specification) and device type (named resource in specification).
   *
   * MODBUS device is not from VPP-4.3 specification, and has also a concept of hardware node ID
   *  that is specific to multiDiagTools.
   *  A device connected by a TCPIP port will be represented as: \"TCPIP::hostName::port::MODBUS::HWNID,bitsCount,firstBit\" .
   *  The hostName part can be a IP address or a host name. The hardware node ID is as follow:
   *  - HWNID: Hardware ID of the MODBUS node
   *  - bitsCount: number of bits, i.e. the number of digital inputs, that represent the node ID
   *  - firstBit: the first bit that represent the node ID, starting from 0 (f.ex. if digital input 1 is the first, first bit will be 0)
   */
  QString addressString() const
  {
    return pvAddressString;
  }

  /*! \brief Set USB identification
   */
  void setUsbIdentification(uint16_t idVendor, uint16_t idProduct, const QString & deviceSerialNumber,
                            uint8_t bInterfaceNumber, DeviceType_t devType = DeviceType_t::INSTR)
  {
    clear();
    pvPortType = PortType_t::USB;
    pvDeviceType = devType;
    pvUsbDeviceAddress.idVendor = idVendor;
    pvUsbDeviceAddress.idProduct = idProduct;
    pvUsbDeviceAddress.bInterfaceNumber = bInterfaceNumber;
    pvSerialNumber = deviceSerialNumber;
    pvAddressString = "USB::0x" + QString::number(idVendor, 16) + "::0x" + QString::number(idProduct, 16) + \
                      "::" + deviceSerialNumber + "::" + QString::number(bInterfaceNumber) + \
                      "::" + deviceTypeStr();
  }

  /*! \brief Get USB idVendor
   */
  uint16_t usbIdVendor() const
  {
    if(pvPortType != PortType_t::USB){
      return 0;
    }
    return pvUsbDeviceAddress.idVendor;
  }

  /*! \brief Get USB idProduct
   */
  uint16_t usbIdProduct() const
  {
    if(pvPortType != PortType_t::USB){
      return 0;
    }
    return pvUsbDeviceAddress.idProduct;
  }

  /*! \brief Get USB device serial number
   */
  QString usbDeviceSerialNumber() const
  {
    if(pvPortType != PortType_t::USB){
      return QString();
    }
    return pvSerialNumber;
  }

  /*! \brief Get USB bInterfaceNumber
   */
  uint8_t usbInterfaceNumber() const
  {
    if(pvPortType != PortType_t::USB){
      return 0;
    }
    return pvUsbDeviceAddress.bInterfaceNumber;
  }

  /*! \brief Set TCP/IP identification
   */
  void setTcpIpIdentification(const QString & hostName, uint16_t port, DeviceType_t devType)
  {
    clear();
    pvPortType = PortType_t::TCPIP;
    pvDeviceType = devType;
    pvHostName = hostName;
    pvTcpipDeviceAddress.port = port;
    pvAddressString = "TCPIP::" + pvHostName + "::" + QString::number(port) + "::" + deviceTypeStr();
  }

  /*! \brief Set MODBUS/TCP identification
   */
  void setModbusTcpIdentification(const QString & hostName, uint16_t port, const mdtModbusHwNodeId & nid)
  {
    clear();
    pvPortType = PortType_t::TCPIP;
    pvDeviceType = DeviceType_t::MODBUS;
    pvHostName = hostName;
    pvTcpipDeviceAddress.port = port;
    pvModbusHwNodeId = nid;
    pvAddressString = "TCPIP::" + pvHostName + "::" + QString::number(port) + "::MODBUS::" + \
                      QString::number(nid.id()) + "," + QString::number(nid.bitsCount()) + "," + QString::number(nid.firstBit());
  }

  /*! \brief Get TCP/IP host name
   */
  QString tcpIpHostName() const
  {
    if(pvPortType != PortType_t::TCPIP){
      return QString();
    }
    return pvHostName;
  }

  /*! \brief Get TCP/IP port
   */
  uint16_t tcpIpPort() const
  {
    if(pvPortType != PortType_t::TCPIP){
      return 0;
    }
    return pvTcpipDeviceAddress.port;
  }

  /*! \brief Get MODBUS hardware node id
   */
  mdtModbusHwNodeId modbusHwNodeId() const
  {
    return pvModbusHwNodeId;
  }

 private:

  /*! \brief Decode USB address string
   */
  bool decodeUsbAddressString(const QStringList & lst);

  /*! \brief Decode TCPIP address string
   *
   * \todo IPv6 strings not supported yet !
   */
  bool decodeTcpipAddressString(const QStringList & lst);

  /*! \brief Decode TCPIP raw socket address string
   */
  bool decodeTcpipRawSocketAddressString(const QStringList & lst);

  /*! \brief Decode MODBUS/TCP address string
   */
  bool decodeModbusTcpAddressString(const QStringList & lst);

  // tr function
  QString tr(const char *sourceText) const
  {
    return QObject::tr(sourceText);
  }

  // Common members
  QString pvAddressString;  // Address string as described in VPP-4.3: revision 5.4 , chapiter 4.3.1
  PortType_t pvPortType;
  DeviceType_t pvDeviceType;
  mdtError pvLastError;
  // Port specific members
  union
  {
    mdtUsbDeviceAddress pvUsbDeviceAddress;
    mdtTcpipDeviceAddress pvTcpipDeviceAddress;
    mdtAsrlDeviceAddress pvAsrlDeviceAddress;
  };
  // Non trivial port specific members
  QString pvHostName;
  QString pvSerialNumber;
  // Common port specific members
  mdtModbusHwNodeId pvModbusHwNodeId;
};

/*
 * Implementation of long functions
 */

void mdtDeviceAddress::clear()
{
  pvAddressString.clear();
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
  pvTcpipDeviceAddress.port = lst.at(2).toUInt(&ok);
  if(!ok){
    pvLastError.setError(tr("Could not decode port number in address string '") + pvAddressString + tr("'.") , mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDeviceAddress");
    pvLastError.commit();
    clear();
    return false;
  }
  pvDeviceType = DeviceType_t::MODBUS;
  if(lst.size() < 5){
    return true;
  }
  // Extract MODBUS hardware node address is specified
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

#endif // #ifndef MDT_DEVICE_ADDRESS_H

