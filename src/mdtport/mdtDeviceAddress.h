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
#include <QString>
#include <cstdint>
#include <string>

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
   * In VPP-4.3: revision 5.4 , chapiter 4.3.1,
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

  /*! \brief Construct a invalid device address
   */
  mdtDeviceAddress()
   : pvPortType(PortType_t::NONE),
     pvPortNumber(0)
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

  /*! \brief Clear device address
   */
  void clear();

  /// \todo Set/Get address string

  /*! \brief Set USB identification
   */
  void setUsbIdentification(uint16_t idVendor, uint16_t idProduct, const QString & deviceSerialNumber, uint8_t bInterfaceNumber)
  {
    clear();
    pvPortType = PortType_t::USB;
    pvUsbDeviceAddress.idVendor = idVendor;
    pvUsbDeviceAddress.idProduct = idProduct;
    pvUsbDeviceAddress.bInterfaceNumber = bInterfaceNumber;
    pvSerialNumber = deviceSerialNumber;
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

 private:

  // Common members
  QString pvAddressString;  // Address string as described in VPP-4.3: revision 5.4 , chapiter 4.3.1
  PortType_t pvPortType;
  int pvPortNumber;
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
  
};

void mdtDeviceAddress::clear()
{
  pvAddressString.clear();
  pvPortType = PortType_t::NONE;
  pvPortNumber = 0;
  pvUsbDeviceAddress.idVendor = 0;
  pvUsbDeviceAddress.idProduct = 0;
  pvUsbDeviceAddress.bInterfaceNumber = 0;
  pvTcpipDeviceAddress.port = 0;
  pvHostName.clear();
  pvSerialNumber.clear();
}


#endif // #ifndef MDT_DEVICE_ADDRESS_H

