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
#ifndef MDT_USBTMC_PORT_H
#define MDT_USBTMC_PORT_H

#include "mdtUsbDeviceDescriptor.h"
#include "mdtUsbInterfaceDescriptor.h"
#include "mdtError.h"
#include <QObject>
#include <QList>
#include <libusb-1.0/libusb.h>
#include <cstdint>

class mdtUsbtmcPortThreadNew;
class mdtUsbtmcTransferHandler;
class mdtUsbDeviceList;

/*! \brief Port to communicate with a USBTMC device
 */
class mdtUsbtmcPort : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtUsbtmcPort(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtUsbtmcPort();

  /*! \brief Enumerate attached USBTMC devices
   */
  QList<mdtUsbDeviceDescriptor> scan();

  /*! \brief Open device represented by given descriptor
   *
   * \param deviceDescriptor USB device descriptor to open. Must be a item returned by scan().
   * \param bInterfaceNumber Interface number to use.
   * \param clearDeviceList If true, internal device descriptors list will be cleared once device was open.
   */
  bool openDevice(const mdtUsbDeviceDescriptor & deviceDescriptor, uint8_t bInterfaceNumber = 0, bool clearDeviceList = true);

  /*! \brief Close
   */
  void close();

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  /*! \brief Build port thread
   *
   * \pre must be called only once
   */
  bool initThread();

  /*! \brief Init libusb context
   *
   * \pre must be called only once
   */
  bool initLibusbConext();

  Q_DISABLE_COPY(mdtUsbtmcPort);

  libusb_context *pvUsbContext;
  ///libusb_device_handle *pvDeviceHandle;
  mdtUsbtmcTransferHandler *pvTransferHandler;
  mdtUsbtmcPortThreadNew *pvThread;
  mdtUsbDeviceList *pvDeviceList;
  mdtError pvLastError;
  ///mdtUsbInterfaceDescriptor pvInterfaceDescriptor;
};

#endif // #ifndef MDT_USBTMC_PORT_H
