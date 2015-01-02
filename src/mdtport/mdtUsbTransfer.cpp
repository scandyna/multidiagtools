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
#include "mdtUsbTransfer.h"
#include <QtGlobal>

mdtUsbTransfer::mdtUsbTransfer ( libusb_device_handle* dev_handle, int isoPacketsCount )
 : pvTransfer(0),
   pvDeviceHandle(dev_handle),
   pvCompleted(0),
   pvIsSync(false)
{
  Q_ASSERT(dev_handle != 0);

  pvTransfer = libusb_alloc_transfer(isoPacketsCount);
  // Make shure that automatic free are disabled
  pvTransfer->flags &= (~LIBUSB_TRANSFER_FREE_BUFFER);
  pvTransfer->flags &= (~LIBUSB_TRANSFER_FREE_TRANSFER);
}

mdtUsbTransfer::~mdtUsbTransfer()
{
  if(pvTransfer != 0){
    libusb_free_transfer(pvTransfer);
  }
}

bool mdtUsbTransfer::submit(bool sync)
{
  Q_ASSERT(pvTransfer != 0);

  int err;

  pvIsSync = sync;
  pvCompleted = false;
  err = libusb_submit_transfer(pvTransfer);
  if(err != 0){
    pvLastError.setError(QObject::tr("Failed to submit transfer"), mdtError::Error);
    pvLastError.setSystemError(err, libusb_strerror((libusb_error)err));
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbTransfer");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtUsbTransfer::cancel(bool sync)
{
  Q_ASSERT(pvTransfer != 0);

  int err;

  pvIsSync = sync;
  pvCompleted = false;
  err = libusb_cancel_transfer(pvTransfer);
  if((err != 0) && (err != LIBUSB_ERROR_NOT_FOUND)){
    pvLastError.setError(QObject::tr("Failed to submit transfer canecl request"), mdtError::Error);
    pvLastError.setSystemError(err, libusb_strerror((libusb_error)err));
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbTransfer");
    pvLastError.commit();
    return false;
  }

  return true;
}
