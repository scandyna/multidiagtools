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
#include "mdtUsbtmcPortThreadNew.h"
#include "mdtUsbtmcTransferHandler.h"

#include <QDebug>

mdtUsbtmcPortThreadNew::mdtUsbtmcPortThreadNew ( mdtUsbtmcTransferHandler& th, libusb_context* usbContext, QObject* parent )
 : mdtUsbPortThreadNew (usbContext, parent),
   pvTransferHandler(th)
{
}

void mdtUsbtmcPortThreadNew::aboutToClose()
{
  qDebug() << "USBTMC: about to close ...";
  /*
   * Don't know how to handle this properly.
   * One problem is to wake-up USB thread, i.e. sending a event so that libusb_handle_events() returns.
   * One function that does it is libusb_close() (wich is called by mdtUsbPortThread once we return from here.
   * Other problem is to cancel transfers in a not to ugly way.
   * Try to submit transfers cancellation, and give thread a chance to do it in a short time.
   */
  pvTransferHandler.setCurrentState(mdtUsbtmcTransferHandler::State_t::Stopping);
  pvTransferHandler.submitControlTransfersCancel();
  if(pvTransferHandler.hasPendingTransfers()){
    waitOnState(State_t::Stopped, 1000);
  }
  qDebug() << "USBTMC: about to close DONE";
}

void mdtUsbtmcPortThreadNew::run()
{
  int err;
  libusb_device_handle *handle;

  qDebug() << "USBTMC THD: starting ...";
  // Open USB device
  if(!openDevice()){
    setCurrentState(State_t::Error);
    return;
  }
  handle = deviceHandle();
  Q_ASSERT(handle != 0);
  // Setup transfer handler
  if(!pvTransferHandler.setup(handle, deviceDescriptor(), bInterfaceNumber())){
    std::lock_guard<std::mutex> lg(pvLastErrorMutex);
    pvLastError = pvTransferHandler.lastError();
    setCurrentState(State_t::Error);
    return;
  }
  pvTransferHandler.setCurrentState(mdtUsbtmcTransferHandler::State_t::Running);
  // Tell main thread that we are now running and run..
  setCurrentState(State_t::Running);
  qDebug() << "USBTMC THD: running ...";
  while(currentState() == State_t::Running){
    err = libusb_handle_events(usbContext());
    if(err != 0){
      std::lock_guard<std::mutex> lg(pvLastErrorMutex);
      pvLastError.setError(QObject::tr("USB event handling failed (libusb_handle_events())"), mdtError::Error);
      pvLastError.setSystemError(err, libusb_strerror((libusb_error)err));
      MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcPortThreadNew");
      pvLastError.commit();
      setCurrentState(State_t::Error);
      return;
    }
    // Some event woke us up - Check that it was not a stop request
    if(currentState() != State_t::Running){
      qDebug() << "USBTMC THD: stopping ...";
      break;
    }
  }
  pvTransferHandler.setCurrentState(mdtUsbtmcTransferHandler::State_t::Stopped);
  qDebug() << "USBTMC THD: END ...";
  // Tell main thread that we are now stopped
  setCurrentState(State_t::Stopped);
}
