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
#ifndef MDT_USB_DEVICE_LIST_H
#define MDT_USB_DEVICE_LIST_H

#include "mdtError.h"
#include "mdtUsbDeviceDescriptor.h"
#include <QList>
#include <libusb-1.0/libusb.h>
#include <cstdint>

/*! \brief Helper class to enumerate and search USB devices that are attached on system
 *
 * Finding and opening a device can be done in several steps:
 *  - Build a list of attached devices by using scan()
 *  - Find interresting device with findDevice()
 *  - Setup system based on returned device descriptor (interfaces, endpoints, ...)
 *  - Open device with its mdtUsbDeviceDescriptor::open() function
 *  - Clear search result with clear()
 */
class mdtUsbDeviceList
{
 public:

  /*! \brief Constructor
   *
   * Given contex must be initialized (see libusb_init()).
   */
  mdtUsbDeviceList(libusb_context *ctx);

  /*! \brief Destructor
   *
   * \todo Comment about to get device handle before this class dies.
   */
  ~mdtUsbDeviceList();

  /*! \brief Enumerate any attached USB devices
   */
  bool scan();

  /*! \brief Get devices descriptors list
   *
   * Will contain valid result after successfull scan.
   */
  QList<mdtUsbDeviceDescriptor> deviceList() const
  {
    return pvDeviceDescriptors;
  }

  /*! \brief Get a list of devices of given class, sub class and protocol
   */
  QList<mdtUsbDeviceDescriptor> deviceList(uint8_t bDeviceClass, uint8_t bDeviceSubClass, uint8_t bDeviceProtocol) const;

  /*! \brief Get a list of attached USBTMC devices
   */
  QList<mdtUsbDeviceDescriptor> usbtmcDeviceList() const;

  /*! \brief Find a device with given vendor ID, product ID and serial number if not empty
   *
   * Note: serial number is not requeired for all class/subclass.
   * For USBTMC, it is required (see USBTMC 1.0 spec, chap. 5.1)
   *
   * \return A valid device descriptor on success, or a empty one on failure (use lastError() on this case).
   */
  mdtUsbDeviceDescriptor findDevice(uint16_t idVendor, uint16_t idProduct, const QString & serialNumber = QString());

  /*! \brief Get first device that is a USBTMC device
   *
   * This function is mostly used by unit testing.
   */
  mdtUsbDeviceDescriptor findFirstUsbtmcDevice() const;

  /*! \brief Clear
   *
   * Will clear devices list and call freeLibusbDeviceList()
   */
  void clear();

  /*! \brief Open a device with given vendor ID, product ID and serial number if not empty
   *
   * Note: serial number is not requeired for all class/subclass.
   * For USBTMC, it is required (see USBTMC 1.0 spec, chap. 5.1)
   *
   * \return A valid handle on success, or a null pointer on error
   *         (on failure, use lastError()).
   *         Note: caller is responsible to free the handle
   *               with libusb_close() once not longer used.
   */
  ///libusb_device_handle *openDevice(uint16_t idVendor, uint16_t idProduct, const QString & serialNumber = QString());

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  Q_DISABLE_COPY(mdtUsbDeviceList);

  /*! \brief Matche device by idVendor and idProduct
   */
  struct deviceMatchDataVidPid
  {
    bool operator() (const mdtUsbDeviceDescriptor & d){
      return ( (d.idVendor() == idVendor) && (d.idProduct() == idProduct) );
    }
    uint16_t idVendor;
    uint16_t idProduct;
  };

  /*! \brief Matche device by idVendor, idProduct and serial number
   */
  struct deviceMatchDataVidPidSn
  {
    bool operator() (const mdtUsbDeviceDescriptor & d){
      return ( (d.idVendor() == idVendor) && (d.idProduct() == idProduct) && (d.serialNumber() == serialNumber) );
    }
    uint16_t idVendor;
    uint16_t idProduct;
    QString serialNumber;
  };

  /*! \brief Build devices list
   */
  bool buildDevicesList(bool fetchDeviceActiveConfigOnly);

  /*! \brief Free libusb devices list
   */
  void freeLibusbDeviceList();

  libusb_context *pvUsbContext;
  libusb_device **pvLibusDeviceList;
  mdtError pvLastError;
  QList<mdtUsbDeviceDescriptor> pvDeviceDescriptors;
};

#endif // #ifndef MDT_USB_DEVICE_LIST_H
