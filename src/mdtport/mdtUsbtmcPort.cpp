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
#include "mdtUsbtmcPort.h"
#include "mdtUsbEndpointDescriptor.h"
#include "mdtUsbtmcPortThreadNew.h"
#include "mdtUsbtmcTransferHandler.h"
#include "mdtUsbDeviceList.h"
#include "mdtUsbtmcMessage.h"
#include <QString>
#include <QCoreApplication>
#include <QTimer>

#include <QDebug>

mdtUsbtmcPort::mdtUsbtmcPort(QObject* parent)
 : QObject(parent),
   pvUsbContext(0),
   pvTransferHandler(0),
   pvThread(0),
   pvDeviceList(0)
{
  qRegisterMetaType<TransactionState_t>("TransactionState_t");
  connect(this, SIGNAL(transactionFinished(TransactionState_t)), this, SLOT(setTransactionState(TransactionState_t)));
}

mdtUsbtmcPort::~mdtUsbtmcPort()
{
  close();
  if(pvUsbContext != 0){
    Q_ASSERT(pvDeviceList != 0);
    delete pvDeviceList;
    libusb_exit(pvUsbContext);
  }
  if(pvThread != 0){
    Q_ASSERT(pvTransferHandler != 0);
    delete pvThread;
    delete pvTransferHandler;
  }
}

QList<mdtUsbDeviceDescriptor> mdtUsbtmcPort::scan()
{
  QList<mdtUsbDeviceDescriptor> usbtmcDevices;

  // Init USB context if needed
  if(pvUsbContext == 0){
    if(!initLibusbConext()){
      return usbtmcDevices;
    }
  }
  // Enumerate UDBTMC devices
  pvDeviceList->clear();
  if(!pvDeviceList->scan()){
    pvLastError = pvDeviceList->lastError();
    return usbtmcDevices;
  }
  usbtmcDevices = pvDeviceList->usbtmcDeviceList();
  if(usbtmcDevices.isEmpty()){
    pvLastError.setError(tr("No USBTMC device is attached on system."), mdtError::Warning);
    // We not commit error (prevent having logs full of wrong errors)
  }

  return usbtmcDevices;
}

void mdtUsbtmcPort::setDeviceName(const QString & name)
{
  pvDeviceName = name;
  if(pvTransferHandler != 0){
    pvTransferHandler->setDeviceName(pvDeviceName);
  }
}

bool mdtUsbtmcPort::openDevice(uint16_t idVendor, uint16_t idProduct, const QString& serialNumber)
{
  // Init USB context if needed
  if(pvUsbContext == 0){
    if(!initLibusbConext()){
      return false;
    }
  }
  // Enumerate UDBTMC devices
  pvDeviceList->clear();
  if(!pvDeviceList->scan()){
    pvLastError = pvDeviceList->lastError();
    return false;
  }
  // Open requested device
  mdtUsbDeviceDescriptor descriptor = pvDeviceList->findDevice(idVendor, idProduct, serialNumber);
  if(descriptor.isEmpty()){
    pvLastError = pvDeviceList->lastError();
    return false;
  }

  return openDevice(descriptor, 0, true);
}

bool mdtUsbtmcPort::openDevice(const mdtUsbDeviceDescriptor& deviceDescriptor, uint8_t bInterfaceNumber, bool clearDeviceList)
{
  // Check given device descriptor
  if(deviceDescriptor.isEmpty()){
    pvLastError.setError(tr("Given device descriptor is empty (did you use scan() to obtain it ?)"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcPort");
    pvLastError.commit();
    return false;
  }
  // Init thread if required
  if(pvThread == 0){
    if(!initThread()){
      return false;
    }
  }
  Q_ASSERT(pvTransferHandler != 0);
  // Set device name
  if(pvDeviceName.isEmpty()){
    pvDeviceName = deviceDescriptor.idString();
  }
  pvTransferHandler->setDeviceName(pvDeviceName);
  // Start thread
  if(!pvThread->start(deviceDescriptor, bInterfaceNumber)){
    pvLastError = pvTransferHandler->lastError();
    return false;
  }
  // Free internal scan result
  if(clearDeviceList){
    pvDeviceList->clear();
  }
  // Clear Bulk I/O
  if(!clearBulkIo()){
    return false;
  }

  return true;
}

void mdtUsbtmcPort::close()
{
  if(pvThread != 0){
    pvThread->stop();
  }
}

bool mdtUsbtmcPort::sendCommand(const QByteArray & command, int timeout)
{
  Q_ASSERT(pvTransferHandler != 0);

  mdtUsbtmcTxMessage message(command);

  while(message.hasBytesToRead()){
    pvTransactionState = TransactionState_t::Pending;
    if(!pvTransferHandler->submitCommand(message, false, timeout)){
      setError(pvTransferHandler->lastError());
      /// \todo We should abort message here (CLEAR, not Bulk I/O, because bulk transfer it was not submited now)
      return false;
    }
    waitTransactionFinished();
    if(pvTransactionState != TransactionState_t::Done){
      return false;
    }
  }
  return true;
}

QByteArray mdtUsbtmcPort::sendQuery(const QByteArray& query, int timeout)
{
  Q_ASSERT(pvTransferHandler != 0);

  mdtUsbtmcTxMessage txMessage(query);
  QByteArray rxBa;
  mdtUsbtmcRxMessage rxMessage(rxBa);

  /*
   * Send query to device.
   * TODO: if we want to send a long query (approx 4000 bytes),
   *       mdtUsbtmcTransferHandler and this class must be modified
   *       so that we can distingish when Bulk-OUT transfer is done,
   *       then send DEV_DEP_MSG_IN request and finaly get received data.
   *       For most common cases, query are very short, and we assume that
   *       they can be transmitted to device in one shot.
   */
  pvTransactionState = TransactionState_t::Pending;
  if(!pvTransferHandler->submitCommand(txMessage, true, timeout)){
    setError(pvTransferHandler->lastError());
    /// \todo We should abort message here (CLEAR, not Bulk I/O, because bulk transfer it was not submited now)
    return false;
  }
  waitTransactionFinished();
  if(pvTransactionState != TransactionState_t::Done){
    return rxBa;
  }
  pvTransferHandler->getReceivedData(rxMessage);

  return rxBa;
}

// void mdtUsbtmcPort::wait(int t)
// {
//   pvWaitTimeReached = false;
//   QTimer::singleShot(t, this, SLOT(setWaitTimeReached()));
//   while(!pvWaitTimeReached){
//     QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
//   }
// }

bool mdtUsbtmcPort::clearBulkIo()
{
  Q_ASSERT(pvTransferHandler != 0);

  pvTransactionState = TransactionState_t::Pending;
  pvTransferHandler->submitBulkIoClear();
  waitTransactionFinished();
  if(pvTransactionState != TransactionState_t::Aborted){
    return false;
  }

  return true;
}

void mdtUsbtmcPort::setError(const mdtError & error)
{
  pvLastErrorMutex.lock();
  pvLastError = error;
  pvLastErrorMutex.unlock();
  emit transactionFinished(TransactionState_t::Error);
}

void mdtUsbtmcPort::setTransactionState(mdtUsbtmcPort::TransactionState_t s)
{
  pvTransactionState = s;
}

// void mdtUsbtmcPort::setWaitTimeReached()
// {
//   pvWaitTimeReached = true;
// }

void mdtUsbtmcPort::waitTransactionFinished()
{
  while(pvTransactionState == TransactionState_t::Pending){
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
  }
}

bool mdtUsbtmcPort::initThread()
{
  Q_ASSERT(pvThread == 0);
  Q_ASSERT(pvTransferHandler == 0);

  // Init libusb context if needed
  if(pvUsbContext == 0){
    if(!initLibusbConext()){
      return false;
    }
  }
  // Build transfer handler and thread
  pvTransferHandler = new mdtUsbtmcTransferHandler(pvUsbContext, *this);
  pvThread = new mdtUsbtmcPortThreadNew(*pvTransferHandler, pvUsbContext, 0);
  connect(pvThread, SIGNAL(errorOccured(const mdtError &)), this, SLOT(setError(const mdtError &)), Qt::UniqueConnection);

  return true;
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
  pvDeviceList = new mdtUsbDeviceList(pvUsbContext);
  //libusb_set_debug(pvUsbContext, 0b00000111);

  return true;
}
