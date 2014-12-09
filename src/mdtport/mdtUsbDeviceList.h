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

/*! \brief Helper class to enumerate and search USB devices that are attached on system
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

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  Q_DISABLE_COPY(mdtUsbDeviceList);

  /*! \brief Build devices list
   */
  bool buildDevicesList();

  libusb_context *pvUsbContext;
  libusb_device **pvDeviceList;
  mdtError pvLastError;
  QList<mdtUsbDeviceDescriptor> pvDeviceDescriptors;
};

#endif // #ifndef MDT_USB_DEVICE_LIST_H
