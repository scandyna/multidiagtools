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
#ifndef MDT_USB_DEVICE_DESCRIPTOR_H
#define MDT_USB_DEVICE_DESCRIPTOR_H

#include <libusb-1.0/libusb.h>
#include <QtGlobal>
#include <QList>
#include "mdtUsbConfigDescriptor.h"
#include "mdtUsbInterfaceDescriptor.h"
#include "mdtUsbEndpointDescriptor.h"

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
 * Each "parent" class gives access to their children with a QList<\T> method.
 * For example, configurations are available with configurations().
 *
 * For current needed attributes, some helper methods are available:
 *  - interface(int, int) : to get a interface descriptor by indexes.
 *  - endpoint(int, int, int) : to get a endpoint descriptor by index.
 */
class mdtUsbDeviceDescriptor
{
 public:

  mdtUsbDeviceDescriptor();
  ~mdtUsbDeviceDescriptor();

  /*! \brief Ftech the device descriptor's attributes
   *
   * \param fetchActiveConfigOnly If true, the interfaces and endpoints informations will be fetched
   *                               only for active configuration. Else, all device's avalable configurations
   *                               will be scanned.
   * \return 0 on success, libusb error code else. On error, the attributes must be considered as invalid.
   * \pre device must be a valid pointer
   */
  int fetchAttributes(libusb_device *device, bool fetchActiveConfigOnly);

  /*! \brief Device descriptor type
   */
  quint8 bDescriptorType() const;

  /*! \brief USB Specification Release Number
   *
   * Coded in BCD. F.ex. USB 2.0 will be 0x0200.
   */
  quint16 bcdUSB() const;

  /*! \brief Class code (assigned by the USB-IF)
   */
  quint8 bDeviceClass() const;

  /*! \brief Subclass code (assigned by the USB-IF)
   */
  quint8 bDeviceSubClass() const;

  /*! \brief Protocol code (assigned by the USB-IF)
   */
  quint8 bDeviceProtocol() const;

  /*! \brief Maximum packet size for endpoint zero
   *
   * (only 8, 16, 32, or 64 are valid)
   */
  quint8 bMaxPacketSize0() const;

  /*! \brief Vendor ID (assigned by the USB-IF)
   */
  quint16 idVendor() const;

  /*! \brief Product ID (assigned by the USB-IF)
   */
  quint16 idProduct() const;

  /*! \brief Device release number id
   *
   * Coded in BCD.
   */
  quint16 bcdDevice() const;

  /*! \brief Access configurations
   *
   * Configurations are available after a call of fetchAttributes()
   */
  QList<mdtUsbConfigDescriptor*> &configurations();

  /*! \brief Get a interface descriptor by index
   *
   * Index checking is done in this method, and 0 (NULL pointer)
   *  is returned on wrong index.
   *
   * \param configIndex Index of configuration (unsorted, ordered as during discovery)
   * \param ifaceIndex Index of interface (unsorted, ordered as during discovery)
   * \return Pointer to interface descriptor, or 0 on error
   */
  mdtUsbInterfaceDescriptor *interface(int configIndex, int ifaceIndex);

  /*! \brief Get a endpoint descriptor by index
   *
   * Index checking is done in this method, and 0 (NULL pointer)
   *  is returned on wrong index.
   *
   * \param configIndex Index of configuration (unsorted, ordered as during discovery)
   * \param ifaceIndex Index of interface (unsorted, ordered as during discovery)
   * \param endpointIndex Index of endpoint (unsorted, ordered as during discovery)
   * \return Pointer to endpoint descriptor, or 0 on error
   */
  mdtUsbEndpointDescriptor *endpoint(int configIndex, int ifaceIndex, int endpointIndex);

  /*! \brief 

 private:

  Q_DISABLE_COPY(mdtUsbDeviceDescriptor);

  quint8 pvbDescriptorType;
  quint16 pvbcdUSB;
  quint8 pvbDeviceClass;
  quint8 pvbDeviceSubClass;
  quint8 pvbDeviceProtocol;
  quint8 pvbMaxPacketSize0;
  quint16 pvidVendor;
  quint16 pvidProduct;
  quint16 pvbcdDevice;
  quint8 pviManufactuer;
  quint8 pviProduct;
  quint8 pviSerialNumber;
  QList<mdtUsbConfigDescriptor*> pvConfigs;
};

#endif  // #ifndef MDT_USB_DEVICE_DESCRIPTOR_H
