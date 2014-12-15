/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_USB_DEVICE_DESCRIPTOR_H
#define MDT_USB_DEVICE_DESCRIPTOR_H

#include <libusb-1.0/libusb.h>
#include <QtGlobal>
#include <QList>
#include <QString>
#include "mdtError.h"
#include "mdtUsbConfigDescriptor.h"
#include "mdtUsbInterfaceDescriptor.h"
#include "mdtUsbEndpointDescriptor.h"
#include <cstdint>

/*! \brief USB device descriptor
 *
 * Internally, once fetchAttributes() is called,
 *  device configuration(s) will be scanned,
 *  and, for each configuration, interfaces and
 *  finally endpoinds will be fetched.
 *
 * This tree is represented by several classes:
 *  - Device descriptor: mdtUsbDeviceDescriptor (this class)
 *  - Configuration descriptor: mdtUsbConfigDescriptor
 *  - Interface descriptor: mdtUsbInterfaceDescriptor
 *  - Endpoint descriptor: mdtUsbEndpointDescriptor
 *
 * Each "parent" class gives access to their child with a QList<\T> method.
 * For example, configurations are available with configurations().
 *
 * For current needed attributes, some helper methods are available:
 *  - interface(int, int) : to get a interface descriptor by indexes.
 *  - endpoint(int, int, int) : to get a endpoint descriptor by index.
 *
 * When goal is to open a particular device for I/O, a simple way to get
 *  needed informations could be to call fetchAttributes() with fetchActiveConfigOnly set true,
 *  then get endpoints with firstBulkOutEndpoint() , firstBulkInEndpoint() , firstInterruptInEndpoint() 
 *  with configIndex set to 0, and a valid ifaceIndex.
 */
class mdtUsbDeviceDescriptor
{
 public:

  /*! \brief Construct a empty device descriptor
   */
  mdtUsbDeviceDescriptor();

  /*! \brief Clear
   */
  void clear();

  /*! \brief Check if descriptor is empty
   */
  bool isEmpty() const{
    return pvIsEmpty;
  }

  /*! \brief Fetch the device descriptor's attributes
   *
   * \param fetchActiveConfigOnly If true, the interfaces and endpoints informations will be fetched
   *                               only for active configuration. Else, all device's available configurations
   *                               will be scanned. See USB (2.0 or 3.1) specifications, chap. 9.6.3 .
   * \return True on success. On error, isEmpty() will be true, and lastError() contains error.
   * \pre device must be a valid pointer
   */
  bool fetchAttributes(libusb_device *device, bool fetchActiveConfigOnly);

  /*! \brief Get libusb device
   *
   * Will return the device passed to fetchAttributes().
   */
  /**
  libusb_device *libusbDevice()
  {
    return pvLibusbDevice;
  }
  */

  /*! \brief Open current device
   *
   * After successfull open, I/O can be performed
   *  with returned handle (see libusb_open() for details).
   *
   * Note: destroying current descriptor object will not invalidate handle.
   *
   * \return A valid handle on success, or a null pointer on error
   *         (on failure, use lastError()).
   *         Note: caller is responsible to free the handle
   *               with libusb_close() once no longer used.
   * \pre Current descriptor mus be valid, i.e. fetchAttributes() must be successfull before calling this function.
   */
  libusb_device_handle *open();

  /*! \brief Device descriptor type
   */
  uint8_t bDescriptorType() const
  {
    return pvDescriptor.bDescriptorType;
  }

  /*! \brief USB Specification Release Number
   *
   * Coded in BCD. F.ex. USB 2.0 will be 0x0200.
   */
  uint16_t bcdUSB() const
  {
    return pvDescriptor.bcdUSB;
  }

  /*! \brief Class code (assigned by the USB-IF)
   */
  uint8_t bDeviceClass() const
  {
    return pvDescriptor.bDeviceClass;
  }

  /*! \brief Subclass code (assigned by the USB-IF)
   */
  uint8_t bDeviceSubClass() const
  {
    return pvDescriptor.bDeviceSubClass;
  }

  /*! \brief Protocol code (assigned by the USB-IF)
   */
  uint8_t bDeviceProtocol() const
  {
    return pvDescriptor.bDeviceProtocol;
  }

  /*! \brief Maximum packet size for endpoint zero
   *
   * (only 8, 16, 32, or 64 are valid)
   */
  uint8_t bMaxPacketSize0() const
  {
    return pvDescriptor.bMaxPacketSize0;
  }

  /*! \brief Vendor ID (assigned by the USB-IF)
   */
  uint16_t idVendor() const
  {
    return pvDescriptor.idVendor;
  }

  /*! \brief Vendor name (assigned by the USB-IF)
   *
   * If vendor ID is known it is returned,
   *  else a string containing VID:0xXXXX is returned
   *
   * \todo Parsing the usb.ids (see http://www.linux-usb.org)
   */
  QString vendorName() const;

  /*! \brief Product ID
   */
  uint16_t idProduct() const
  {
    return pvDescriptor.idProduct;
  }

  /*! \brief Product name
   *
   * If vendorID:productID is known the product name is returned,
   *  else a string containing PID:0xXXXX is returned
   *
   * \todo Parsing the usb.ids (see http://www.linux-usb.org)
   */
  QString productName() const;

  /*! \brief Get serial number
   */
  QString serialNumber() const;

  /*! \brief Get a string with vendor ID, product ID and serial number if available
   */
  QString idString() const;

  /*! \brief Device release number id
   *
   * Coded in BCD.
   */
  uint16_t bcdDevice() const
  {
    return pvDescriptor.bcdDevice;
  }

  /*! \brief Access configurations
   *
   * Configurations are available after a call of fetchAttributes()
   */
  QList<mdtUsbConfigDescriptor> configurations() const{
    return pvConfigs;
  }

  /*! \brief Get number of interface that is of given class and sub class
   */
  int interfacesCount(uint8_t bInterfaceClass, uint8_t bInterfaceSubClass) const;

  /*! \brief Get list of bulk IN endpoints
   *
   * Note: if device contains many configuration/interfaces,
   *  returned result will probably be wrong.
   */
  QList<mdtUsbEndpointDescriptor> bulkInEndpoints() const;

  /*! \brief Get list of bulk OUT endpoints
   *
   * Note: if device contains many configuration/interfaces,
   *  returned result will probably be wrong.
   */
  QList<mdtUsbEndpointDescriptor> bulkOutEndpoints() const;

  /*! \brief Get list of interrupt IN endpoints
   *
   * Note: if device contains many configuration/interfaces,
   *  returned result will probably be wrong.
   */
  QList<mdtUsbEndpointDescriptor> interruptInEndpoints() const;

  /*! \brief Get list of interrupt OUT endpoints
   *
   * Note: if device contains many configuration/interfaces,
   *  returned result will probably be wrong.
   */
  QList<mdtUsbEndpointDescriptor> interruptOutEndpoints() const;

  /*! \brief Get a interface descriptor by bInterfaceNumber
   */
  mdtUsbInterfaceDescriptor interface(uint8_t bInterfaceNumber);

  
  
  
  /*! \brief Get a interface descriptor by index
   *
   * Index checking is done in this method, and empty descriptor
   *  is returned on wrong index.
   *
   * \param configIndex Index of configuration (unsorted, ordered as during discovery)
   * \param ifaceIndex Index of interface (unsorted, ordered as during discovery)
   * \return Interface descriptor filled with devices values, or a empty one on error.
   */
  mdtUsbInterfaceDescriptor interface(int configIndex, int ifaceIndex);

  /*! \brief Get a endpoint descriptor by index
   *
   * Index checking is done in this method, and a empty descriptor
   *  is returned on wrong index.
   *
   * \param configIndex Index of configuration (unsorted, ordered as during discovery)
   * \param ifaceIndex Index of interface (unsorted, ordered as during discovery)
   * \param endpointIndex Index of endpoint (unsorted, ordered as during discovery)
   * \return End point descriptor filled with devices values, or a empty one on error.
   */
  mdtUsbEndpointDescriptor endpoint(int configIndex, int ifaceIndex, int endpointIndex);

  /*! \brief Find the first bulk transfert endpoint with OUT direction
   *
   * \param configIndex Index of configuration (unsorted, ordered as during discovery)
   * \param ifaceIndex Index of interface (unsorted, ordered as during discovery)
   * \param dataEndpointOnly If true, only data endpoints will be considered (feedback and other are ignored)
   * \return End point descriptor filled with devices values, or a empty one on error.
   */
  mdtUsbEndpointDescriptor firstBulkOutEndpoint(int configIndex, int ifaceIndex, bool dataEndpointOnly);

  /*! \brief Find the first bulk transfert endpoint with IN direction
   *
   * \param configIndex Index of configuration (unsorted, ordered as during discovery)
   * \param ifaceIndex Index of interface (unsorted, ordered as during discovery)
   * \param dataEndpointOnly If true, only data endpoints will be considered (feedback and other are ignored)
   * \return End point descriptor filled with devices values, or a empty one on error.
   */
  mdtUsbEndpointDescriptor firstBulkInEndpoint(int configIndex, int ifaceIndex, bool dataEndpointOnly);

  /*! \brief Find the first interrupt transfert endpoint with OUT direction
   *
   * \param configIndex Index of configuration (unsorted, ordered as during discovery)
   * \param ifaceIndex Index of interface (unsorted, ordered as during discovery)
   * \param dataEndpointOnly If true, only data endpoints will be considered (feedback and other are ignored)
   * \return End point descriptor filled with devices values, or a empty one on error.
   */
  mdtUsbEndpointDescriptor firstInterruptOutEndpoint(int configIndex, int ifaceIndex, bool dataEndpointOnly);

  /*! \brief Find the first interrupt transfert endpoint with IN direction
   *
   * \param configIndex Index of configuration (unsorted, ordered as during discovery)
   * \param ifaceIndex Index of interface (unsorted, ordered as during discovery)
   * \param dataEndpointOnly If true, only data endpoints will be considered (feedback and other are ignored)
   * \return End point descriptor filled with devices values, or a empty one on error.
   */
  mdtUsbEndpointDescriptor firstInterruptInEndpoint(int configIndex, int ifaceIndex, bool dataEndpointOnly);

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  ///Q_DISABLE_COPY(mdtUsbDeviceDescriptor);

  libusb_device_descriptor pvDescriptor;
  libusb_device *pvLibusbDevice;
  bool pvIsEmpty;
  QString pvSerialNumber;
  QList<mdtUsbConfigDescriptor> pvConfigs;
  mdtError pvLastError;
};

#endif  // #ifndef MDT_USB_DEVICE_DESCRIPTOR_H
