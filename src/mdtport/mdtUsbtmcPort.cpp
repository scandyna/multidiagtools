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
#include "mdtUsbtmcPort.h"
#include "mdtUsbEndpointDescriptor.h"
#include <QString>

mdtUsbtmcPort::mdtUsbtmcPort ( QObject* parent )
 : QObject(parent),
   pvUsbContext(0),
   pvDeviceHandle(0)
{
}

mdtUsbtmcPort::~mdtUsbtmcPort()
{
  close();
  if(pvUsbContext != 0){
    libusb_exit(pvUsbContext);
  }
}

bool mdtUsbtmcPort::openDevice(mdtUsbDeviceDescriptor& deviceDescriptor, uint8_t bInterfaceNumber)
{
  Q_ASSERT(!deviceDescriptor.isEmpty());

  int err;

  // Get descriptor of requested interface
  pvInterfaceDescriptor = deviceDescriptor.interface(bInterfaceNumber);
  if(pvInterfaceDescriptor.isEmpty()){
    pvLastError.setError(tr("Requested bInterfaceNumber ") + QString::number(bInterfaceNumber) + tr(" was not found in given device descriptor."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcPort");
    pvLastError.commit();
    return false;
  }
  // Init libusb context if not allready done
  if(pvUsbContext == 0){
    if(!initLibusbConext()){
      return false;
    }
  }
  // Open device
  close();
  pvDeviceHandle = deviceDescriptor.open();
  if(pvDeviceHandle == 0){
    pvLastError = deviceDescriptor.lastError();
    return false;
  }
  Q_ASSERT(pvDeviceHandle != 0);
  // Tell libusb that we let him detach possibly loaded kernel driver itself
  err = libusb_set_auto_detach_kernel_driver(pvDeviceHandle, 1);
  if(err != 0){
    pvLastError.setError(tr("Your platform does not support kernel driver detach."), mdtError::Warning);
    pvLastError.setSystemError(err, libusb_strerror((libusb_error)err));
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcPort");
    pvLastError.commit();
  }
  // Claim interface
  err = libusb_claim_interface(pvDeviceHandle, pvInterfaceDescriptor.bInterfaceNumber());
  if(err != 0){
    pvLastError.setError(tr("Could not claim interface."), mdtError::Error);
    pvLastError.setSystemError(err, libusb_strerror((libusb_error)err));
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcPort");
    pvLastError.commit();
    libusb_close(pvDeviceHandle);
    return false;
  }

  return true;
}

void mdtUsbtmcPort::close()
{
  if(pvDeviceHandle != 0){
    // Release interface
    if(!pvInterfaceDescriptor.isEmpty()){
      int err = libusb_release_interface(pvDeviceHandle, pvInterfaceDescriptor.bInterfaceNumber());
      if(err != 0){
        pvLastError.setError(tr("Could not release interface."), mdtError::Warning);
        pvLastError.setSystemError(err, libusb_strerror((libusb_error)err));
        MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcPort");
        pvLastError.commit();
      }
    }
    // Close device
    libusb_close(pvDeviceHandle);
    pvDeviceHandle = 0;
  }
  pvInterfaceDescriptor.clear();
}


bool mdtUsbtmcPort::initLibusbConext()
{
  Q_ASSERT(pvUsbContext == 0);

  int err;

  err = libusb_init(&pvUsbContext);
  if(err != 0){
    pvLastError.setError(tr("Failed to init lubusb context."), mdtError::Error);
    pvLastError.setSystemError(err, libusb_strerror((libusb_error)err));
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcPort");
    pvLastError.commit();
    return false;
  }

  return true;
}
