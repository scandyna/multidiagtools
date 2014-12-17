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
#include "mdtUsbtmcTransferHandler.h"
#include "mdtUsbtmcControlTransfer.h"
#include "mdtUsbDeviceDescriptor.h"
#include <QObject>
#include <QString>

#include <QDebug>

mdtUsbtmcTransferHandler::mdtUsbtmcTransferHandler()
 : pvDeviceHandle(0),
   pvStateMachine(State_t::Stopped)
{
}

bool mdtUsbtmcTransferHandler::setup ( libusb_device_handle* handle, const mdtUsbDeviceDescriptor & descriptor, uint8_t bInterfaceNumber )
{
  Q_ASSERT(!descriptor.isEmpty());

  // Get descriptor of requested interface
  pvInterfaceDescriptor = descriptor.interface(bInterfaceNumber);
  if(pvInterfaceDescriptor.isEmpty()){
    QString msg = QObject::tr("Device") + " '" + descriptor.idString() + "' : ";
    msg += QObject::tr("requested bInterfaceNumber ") + QString::number(bInterfaceNumber) + QObject::tr(" was not found in given device descriptor.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    return false;
  }
  /*
   * Check that interface is USBTMC compliant:
   *  - 1 bulk OUT endpoint
   *  - 1 bulk IN endpoint
   *  - Optionnaly, 1 interrupt IN endpoint
   */
  // Get bulk OUT endpoint descriptor
  if(pvInterfaceDescriptor.bulkOutEndpoints().size() != 1){
    QString msg = QObject::tr("Device") + " '" + descriptor.idString() + "' : ";
    msg += QObject::tr("interface ") + QString::number(bInterfaceNumber) + QObject::tr(" has not exactly 1 bulk OUT endpoint (not USBTMC compliant).");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    return false;
  }
  pvBulkOutDescriptor = pvInterfaceDescriptor.bulkOutEndpoints().at(0);
  // Get bulk IN endpoint descriptor
  if(pvInterfaceDescriptor.bulkInEndpoints().size() != 1){
    QString msg = QObject::tr("Device") + " '" + descriptor.idString() + "' : ";
    msg += QObject::tr("interface ") + QString::number(bInterfaceNumber) + QObject::tr(" has not exactly 1 bulk IN endpoint (not USBTMC compliant).");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    return false;
  }
  pvBulkInDescriptor = pvInterfaceDescriptor.bulkInEndpoints().at(0);
  // Setup control endpoint
  if(!pvControlTransferPool.allocTransfers(3, *this, handle)){
    QString msg = QObject::tr("Device") + " '" + descriptor.idString() + "' : ";
    msg += QObject::tr("Allocation of control transfer pool failed.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    return false;
  }
  /// \todo Setup bulk OUT endpoint
  
  /// \todo Setup bulk IN endpoint
  
  /// \todo Setup interrupt IN endpoint

  return true;
}

bool mdtUsbtmcTransferHandler::submitClearEndpointHalt(uint8_t endpointNumber, unsigned int timeout)
{
  if(currentState() != State_t::Running){
    return true;
  }
  // Get a transfer from pool
  std::lock_guard<std::mutex> lg(pvControlTransferMutex);
  mdtUsbtmcControlTransfer *transfer = pvControlTransferPool.getTransfer(*this);
  Q_ASSERT(transfer != 0);
  // Setup transfer and submit it
  qDebug() << "Submitting clear halt ...";
  transfer->setupClearEndpointHalt(endpointNumber, timeout);
  if(!transfer->submit()){
    pvLastError = transfer->lastError();  /// \todo error MUTEX
    return false;
  }
  return true;
}

bool mdtUsbtmcTransferHandler::submitControlTransfersCancel()
{
  /*
   * By little experience (with a U3606A),
   * trying to cancel control transfer just before closing give bad resulsts.
   * So, just do nothing.
   */
  if(currentState() != State_t::Running){
    return true;
  }
  // Get pending transfer, if available
  std::lock_guard<std::mutex> lg(pvControlTransferMutex);
  mdtUsbtmcControlTransfer *transfer = pvControlTransferPool.getPendingTransfer();
  if(transfer == 0){
    return true;
  }
  // Cancel transfer
  qDebug() << "Submitting a control transfer cancel ...";
  if(!transfer->cancel()){
    pvLastError = transfer->lastError();  /// \todo error MUTEX
    return false;
  }
  return true;
}

/**
bool mdtUsbtmcTransferHandler::submitTransferCancel ( mdtUsbTransfer* transfer )
{
  Q_ASSERT(transfer != 0);

  int err = libusb_cancel_transfer(transfer->transfer())
}
*/


void mdtUsbtmcTransferHandler::controlTransferCallback(libusb_transfer* transfer)
{
  Q_ASSERT(transfer != 0);

  mdtUsbtmcControlTransfer *uct = static_cast<mdtUsbtmcControlTransfer*>(transfer->user_data);
  
  qDebug() << "Control transfer callback called ! :-)";
  
  uct->transferHandler().handleControlTransferComplete(uct);
}

void mdtUsbtmcTransferHandler::handleControlTransferComplete(mdtUsbtmcControlTransfer *transfer )
{
  Q_ASSERT(transfer != 0);

  if(currentState() != State_t::Running){
    qDebug() << "No more running, do nothing more ...";
    return;
  }
  std::lock_guard<std::mutex> lg(pvControlTransferMutex);
  qDebug() << "Control transfer status: " << transfer->status();
  pvControlTransferPool.restoreTransfer(transfer);
}

bool mdtUsbtmcTransferHandler::hasPendingTransfers() const
{
  // Note: we ignore control transfers
  
  return false;
}
