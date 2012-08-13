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
#include "mdtUsbPort.h"
#include "mdtError.h"
#include <QString>
#include <QStringList>

#include <QDebug>

mdtUsbPort::mdtUsbPort(QObject *parent)
 : mdtAbstractPort(parent)
{
  int retVal;

  pvReadTimeout = 0;
  pvWriteTimeout = 0;
  pvHandle = 0;
  // Init libusb
  retVal = libusb_init(&pvLibusbContext);
  if(retVal != 0){
    pvLibusbContext = 0;
    mdtError e(MDT_USB_IO_ERROR, "libusb_init() failed", mdtError::Error);
    e.setSystemError(retVal, errorText(retVal));
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
  }
  Q_ASSERT(pvLibusbContext != 0);
}

mdtUsbPort::~mdtUsbPort()
{
  close();
  // Free libusb
  libusb_exit(pvLibusbContext);
}

void mdtUsbPort::setReadTimeout(int timeout)
{
  if(timeout < 0){
    pvReadTimeout = 0;  // 0 means infinite on libusb
  }else{
    pvReadTimeout = timeout;
  }
}

void mdtUsbPort::setWriteTimeout(int timeout)
{
  if(timeout < 0){
    pvWriteTimeout = 0;  // 0 means infinite on libusb
  }else{
    pvWriteTimeout = timeout;
  }
}

/// NOTE: \todo : Implement.... Note: use select ??    Mutex !!
mdtAbstractPort::error_t mdtUsbPort::waitForReadyRead()
{
  qDebug() << "mdtUsbPort::waitForReadyRead() ...";
  int retVal;

  /// NOTE: \todo : map correct error
  unlockMutex();
  retVal = libusb_handle_events(pvLibusbContext);
  lockMutex();
  if(retVal != 0){
    mdtError e(MDT_USB_IO_ERROR, "libusb_handle_events() failed", mdtError::Error);
    e.setSystemError(retVal, errorText(retVal));
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnknownError;
  }

  return NoError;
}

qint64 mdtUsbPort::read(char *data, qint64 maxSize)
{
  qDebug() << "mdtUsbPort::read() ... (Not implemented yet)";
  int retVal;
  int completed = 0;

  return 0;
}

void mdtUsbPort::flushIn()
{
  lockMutex();
  mdtAbstractPort::flushIn();
}

/// NOTE: \todo : Implement.... Note: use select ??    Mutex !!
mdtAbstractPort::error_t mdtUsbPort::waitEventWriteReady()
{
  qDebug() << "mdtUsbPort::waitEventWriteReady() ...";
  int retVal;

  /// NOTE: \todo : map correct error
  unlockMutex();
  retVal = libusb_handle_events(pvLibusbContext);
  lockMutex();
  if(retVal != 0){
    mdtError e(MDT_USB_IO_ERROR, "libusb_handle_events() failed", mdtError::Error);
    e.setSystemError(retVal, errorText(retVal));
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnknownError;
  }

  return NoError;
}

qint64 mdtUsbPort::write(const char *data, qint64 maxSize)
{
  qDebug() << "mdtUsbPort::write() ... (Not implemented yet)";
  return 0;
}

void mdtUsbPort::flushOut()
{
  lockMutex();
  mdtAbstractPort::flushOut();
}

void mdtUsbPort::transfertCallback(struct libusb_transfer *transfer)
{
  qDebug() << "Tranfert callback ...";
  int *completed = (int*)transfer->user_data;
  *completed = 1;
}

mdtAbstractPort::error_t mdtUsbPort::pvOpen()
{
  Q_ASSERT(!isOpen());

  quint16 vid;
  quint16 pid;
  QString str;
  QStringList lst;
  bool ok = false;

  ///qDebug() << "mdtUsbPort::pvOpen() ... (Not implemented yet)";

  // Extract vendor ID and product ID
  lst = pvPortName.split(":");
  if(lst.size() < 2){
    mdtError e(MDT_USB_IO_ERROR, "Error in port format for " + pvPortName + " (must be idVendor:idProduct)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return SetupError;
  }
  // Check vendor ID format and convert
  str = lst.at(0);
  if(str.left(2) == "0x"){
    vid = str.toUInt(&ok, 16);
  }else{
    vid = str.toUInt(&ok, 10);
  }
  if(!ok){
    mdtError e(MDT_USB_IO_ERROR, "Cannot extract idVendor in " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return SetupError;
  }
  // Check product ID format and convert
  str = lst.at(1);
  if(str.left(2) == "0x"){
    pid = str.toUInt(&ok, 16);
  }else{
    pid = str.toUInt(&ok, 10);
  }
  if(!ok){
    mdtError e(MDT_USB_IO_ERROR, "Cannot extract idProduct in " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return SetupError;
  }
  // Open port
  pvHandle = libusb_open_device_with_vid_pid(pvLibusbContext, vid, pid);
  if(pvHandle == 0){
    mdtError e(MDT_USB_IO_ERROR, "Cannot open device " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return PortNotFound;
  }

  return NoError;
}

void mdtUsbPort::pvClose()
{
  Q_ASSERT(isOpen());
  Q_ASSERT(pvHandle != 0);

  libusb_close(pvHandle);
}

mdtAbstractPort::error_t mdtUsbPort::pvSetup()
{
  Q_ASSERT(isOpen());
  Q_ASSERT(pvConfig != 0);
}

QString mdtUsbPort::errorText(int errorCode) const
{
  switch(errorCode){
    case LIBUSB_ERROR_IO:
      return tr("Input/output error");
    case LIBUSB_ERROR_INVALID_PARAM:
      return tr("Invalid parameter");
    case LIBUSB_ERROR_ACCESS:
      return tr("Access denied (insufficient permissions)");
    case LIBUSB_ERROR_NO_DEVICE:
      return tr("No such device (it may have been disconnected)");
    case LIBUSB_ERROR_NOT_FOUND:
      return tr("Entity not found");
    case LIBUSB_ERROR_BUSY:
      return tr("Resource busy");
    case LIBUSB_ERROR_TIMEOUT:
      return tr("Operation timed out");
    case LIBUSB_ERROR_OVERFLOW:
      return tr("Overflow");
    case LIBUSB_ERROR_PIPE:
      return tr("Pipe error");
    case LIBUSB_ERROR_INTERRUPTED:
      return tr("System call interrupted (perhaps due to signal)");
    case LIBUSB_ERROR_NO_MEM:
      return tr("Insufficient memory");
    case LIBUSB_ERROR_NOT_SUPPORTED:
      return tr("Operation not supported or unimplemented on this platform");
    default:
      return tr("Unknown error");
  }
}
