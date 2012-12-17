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
#include "mdtUsbPortManager.h"
#include "mdtUsbPort.h"
#include "mdtUsbPortThread.h"
#include "mdtPortConfig.h"
#include "mdtUsbDeviceDescriptor.h"
#include "mdtUsbConfigDescriptor.h"
#include "mdtUsbInterfaceDescriptor.h"
#include <QString>
///#include <QApplication>
#include <libusb-1.0/libusb.h>

#include <QDebug>

mdtUsbPortManager::mdtUsbPortManager(QObject *parent)
 : mdtPortManager(parent)
{
  // Port setup
  mdtPortConfig *config = new mdtPortConfig;
  mdtUsbPort *port = new mdtUsbPort;
  port->setConfig(config);
  setPort(port);

  // Threads setup
  addThread(new mdtUsbPortThread);
}

mdtUsbPortManager::~mdtUsbPortManager()
{
  Q_ASSERT(pvPort != 0);

  delete &pvPort->config();
  detachPort(true, true);
}

QList<mdtPortInfo*> mdtUsbPortManager::scan()
{
  return scan(-1, -1, -1);
}

QList<mdtPortInfo*> mdtUsbPortManager::scan(int bDeviceClass, int bDeviceSubClass, int bDeviceProtocol)
{
  Q_ASSERT(!isRunning());

  QList<mdtPortInfo*> portInfoList;
  mdtDeviceInfo *deviceInfo;
  mdtPortInfo *portInfo;
  libusb_context *ctx;
  libusb_device **devicesList;
  mdtUsbDeviceDescriptor deviceDescriptor;
  mdtUsbConfigDescriptor *configDescriptor;
  mdtUsbInterfaceDescriptor *ifaceDescriptor;
  QString portName;
  libusb_device *device;
  ssize_t devicesCount;
  ssize_t i;
  int j, k;
  int err;
  QString str;

  // Init libusb
  err = libusb_init(&ctx);
  if(err != 0){
    mdtError e(MDT_USB_IO_ERROR, "Cannot init libusb", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPortManager");
    e.commit();
    return portInfoList;
  }
  // Get devices list
  devicesCount = libusb_get_device_list(ctx, &devicesList);
  if(devicesCount < 0){
    mdtError e(MDT_USB_IO_ERROR, "Cannot list devices", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPortManager");
    e.commit();
    return portInfoList;
  }
  // Scan ...
  for(i=0; i<devicesCount; i++){
    device = devicesList[i];
    err = deviceDescriptor.fetchAttributes(device, false);
    if(err != 0){
      mdtError e(MDT_USB_IO_ERROR, "Device attributes fetch failed", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPortManager");
      e.commit();
      continue;
    }
    // Create a port info object
    portInfo = new mdtPortInfo;
    // Search interfaces in various configurations
    for(j=0; j<deviceDescriptor.configurations().size(); j++){
      configDescriptor = deviceDescriptor.configurations().at(j);
      Q_ASSERT(configDescriptor != 0);
      for(k=0; k<configDescriptor->interfaces().size(); k++){
        ifaceDescriptor = configDescriptor->interfaces().at(k);
        Q_ASSERT(ifaceDescriptor != 0);
        // See if a specific bDeviceClass was requested
        if(bDeviceClass >= 0){
          if(ifaceDescriptor->bInterfaceClass() != (quint8)bDeviceClass){
            continue;
          }
        }
        // See if a specific bDeviceSubClass was requested
        if(bDeviceSubClass >= 0){
          if(ifaceDescriptor->bInterfaceSubClass() != (quint8)bDeviceSubClass){
            continue;
          }
        }
        // See if a specific bDeviceProtocol was requested
        if(bDeviceProtocol >= 0){
          if(ifaceDescriptor->bInterfaceProtocol() != (quint8)bDeviceProtocol){
            continue;
          }
        }
        // Interresting interface
        deviceInfo = new mdtDeviceInfo;
        // Build the display text
        str = deviceDescriptor.vendorName();
        str += ", ";
        str += deviceDescriptor.productName();
        if(!deviceDescriptor.serialNumber().isEmpty()){
          str += " (SN: " + deviceDescriptor.serialNumber() + ")";
        }
        portInfo->setDisplayText(str);
        // Set VID, PID, SID and protocol ID
        deviceInfo->setVendorId(deviceDescriptor.idVendor());
        deviceInfo->setProductId(deviceDescriptor.idProduct());
        deviceInfo->setSerialId(deviceDescriptor.serialNumber());
        deviceInfo->setProtocolId(ifaceDescriptor->bInterfaceProtocol());
        // Add interface to list
        portInfo->addDevice(deviceInfo);
      }
    }
    // If port info has no device, delete it, else add it to list
    if(portInfo->deviceInfoList().size() < 1){
      delete portInfo;
    }else{
      // Build port name
      deviceInfo = portInfo->deviceInfoList().at(0);
      Q_ASSERT(deviceInfo != 0);
      portName = "0x";
      portName += QString::number((unsigned int)deviceInfo->vendorId(), 16);
      portName += ":0x";
      portName += QString::number((unsigned int)deviceInfo->productId(), 16);
      if(!deviceDescriptor.serialNumber().isEmpty()){
        portName += ":" + deviceDescriptor.serialNumber();
      }
      portInfo->setPortName(portName);
      // Add port info to list
      portInfoList.append(portInfo);
    }
  }
  // Release ressources
  libusb_free_device_list(devicesList, 1);
  libusb_exit(ctx);

  return portInfoList;
}

int mdtUsbPortManager::sendControlRequest(const mdtFrameUsbControl &request)
{
  Q_ASSERT(pvPort != 0);

  mdtUsbPort *port;
  mdtFrameUsbControl *frame;

  // We need a mdtUsbPort object
  port = dynamic_cast<mdtUsbPort*>(pvPort);
  Q_ASSERT(port != 0);
  // Get a frame in pool
  port->lockMutex();
  if(port->controlFramesPool().size() < 1){
    port->unlockMutex();
    mdtError e(MDT_PORT_IO_ERROR, "No frame available in control frames pool", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPortManager");
    e.commit();
    return mdtAbstractPort::WritePoolEmpty;
  }
  frame = dynamic_cast<mdtFrameUsbControl*> (port->controlFramesPool().dequeue());
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();
  // Store data, encode and add frame to queue
  frame->setDirectionDeviceToHost(request.directionIsDeviceToHost());
  frame->setbmRequestType(request.bmRequestType());
  frame->setbRequest(request.bRequest());
  frame->setwValue(request.wValue());
  frame->setwIndex(request.wIndex());
  frame->setwLength(request.wLength());
  frame->encode();
  port->addControlRequest(frame);
  port->unlockMutex();

  return 0;
}

void mdtUsbPortManager::fromThreadControlResponseReaden()
{
}

void mdtUsbPortManager::fromThreadMessageInReaden()
{
}

