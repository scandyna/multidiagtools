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
#include "mdtUsbtmcPortManager.h"
#include "mdtError.h"
#include "mdtPortConfig.h"
#include "mdtFrame.h"
#include "mdtFrameUsbTmc.h"
#include "mdtUsbDeviceDescriptor.h"
#include "mdtUsbConfigDescriptor.h"
#include "mdtUsbInterfaceDescriptor.h"
#include <QString>
#include <QApplication>
#include <libusb-1.0/libusb.h>

#include <QDebug>

mdtUsbtmcPortManager::mdtUsbtmcPortManager(QObject *parent)
 : mdtPortManager(parent)
{
  // Port setup
  mdtPortConfig *config = new mdtPortConfig;
  config->setFrameType(mdtFrame::FT_USBTMC);
  config->setReadFrameSize(512);
  config->setWriteFrameSize(512);
  config->setWriteQueueSize(1);
  mdtUsbPort *port = new mdtUsbPort;
  port->setConfig(config);
  setPort(port);

  // Threads setup
  addThread(new mdtUsbPortThread);

  // USBTMC specific
  pvCurrentWritebTag = 0;
}

mdtUsbtmcPortManager::~mdtUsbtmcPortManager()
{
  Q_ASSERT(pvPort != 0);

  delete &pvPort->config();
  detachPort(true, true);
}

QList<mdtPortInfo*> mdtUsbtmcPortManager::scan()
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
    mdtError e(MDT_PORT_IO_ERROR, "Cannot init libusb", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortManager");
    e.commit();
    return portInfoList;
  }
  // Get devices list
  devicesCount = libusb_get_device_list(ctx, &devicesList);
  if(devicesCount < 0){
    mdtError e(MDT_PORT_IO_ERROR, "Cannot list devices", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortManager");
    e.commit();
    return portInfoList;
  }
  // Scan ...
  for(i=0; i<devicesCount; i++){
    device = devicesList[i];
    err = deviceDescriptor.fetchAttributes(device, false);
    if(err != 0){
      mdtError e(MDT_PORT_IO_ERROR, "Device attributes fetch failed", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortManager");
      e.commit();
      continue;
    }
    // We search USBTMC devices, bDeviceClass will be set 0x00 and defined in interface descriptor
    if(deviceDescriptor.bDeviceClass() == 0x00){
      // Create a port info object
      portInfo = new mdtPortInfo;
      // Search interfaces in various configurations
      for(j=0; j<deviceDescriptor.configurations().size(); j++){
        configDescriptor = deviceDescriptor.configurations().at(j);
        Q_ASSERT(configDescriptor != 0);
        for(k=0; k<configDescriptor->interfaces().size(); k++){
          ifaceDescriptor = configDescriptor->interfaces().at(k);
          Q_ASSERT(ifaceDescriptor != 0);
          ///qDebug() << "->   iface[" << k << "]";
          // USBTMC has a application specific bInterfaceClass, a bInterfaceSubClass == 0x03
          if(ifaceDescriptor->bInterfaceClass() == 0xFE){
            if(ifaceDescriptor->bInterfaceSubClass() == 0x03){
              // Here we found a USBTMC device
              str = deviceDescriptor.vendorName();
              str += ", ";
              str += deviceDescriptor.productName();
              ///str += " VID: 0x" + QString::number(deviceDescriptor.idProduct(), 16);
              str += " (BUS 0x" + QString::number((unsigned int)libusb_get_bus_number(device), 16);
              str += " DEV 0x" + QString::number((unsigned int)libusb_get_device_address(device), 16);
              str += ")";
              portInfo->setDisplayText(str);
              deviceInfo = new mdtDeviceInfo;
              deviceInfo->setVendorId(deviceDescriptor.idVendor());
              deviceInfo->setProductId(deviceDescriptor.idProduct());
              deviceInfo->setSerialId(deviceDescriptor.serialNumber());
              deviceInfo->setProtocolId(ifaceDescriptor->bInterfaceProtocol());
              ///str = deviceDescriptor.vendorName();
              ///str = "VID:";
              ///str += QString::number(deviceDescriptor.idVendor());
              /**
              str += " PID:";
              str += QString::number(deviceDescriptor.idProduct());
              str += " Interface: ";
              str += QString::number(ifaceDescriptor->bInterfaceProtocol());
              */
              if(ifaceDescriptor->bInterfaceProtocol() == 0x00){
                deviceInfo->setDisplayText("USBTMC");
              }else if(ifaceDescriptor->bInterfaceProtocol() == 0x01){
                deviceInfo->setDisplayText("USBTMC USB488");
              }
              portInfo->addDevice(deviceInfo);
            }
          }
        }
      }
      // If port info has no device, delete it, else add it to list
      if(portInfo->deviceInfoList().size() < 1){
        delete portInfo;
      }else{
        // Build port name
        deviceInfo = portInfo->deviceInfoList().at(0);
        Q_ASSERT(deviceInfo != 0);
        /**
        portName = "0x" + QString::number((unsigned int)libusb_get_bus_number(device), 16);
        portName += ":0x";
        portName += QString::number((unsigned int)libusb_get_device_address(device), 16);
        portName += ":0x";
        */
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
  }
  // Release ressources
  libusb_free_device_list(devicesList, 1);
  libusb_exit(ctx);

  return portInfoList;
}

int mdtUsbtmcPortManager::writeData(QByteArray data)
{
  Q_ASSERT(pvPort != 0);

  mdtFrameUsbTmc *frame;

  // Get a frame in pool
  pvPort->lockMutex();
  if(pvPort->writeFramesPool().size() < 1){
    pvPort->unlockMutex();
    mdtError e(MDT_PORT_IO_ERROR, "No frame available in write frames pool", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortManager");
    e.commit();
    return mdtAbstractPort::WritePoolEmpty;
  }
  frame = dynamic_cast<mdtFrameUsbTmc*> (pvPort->writeFramesPool().dequeue());
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();
  // Store data and add frame to write queue
  frame->setWaitAnAnswer(false);
  frame->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  // Increment bTag and enshure it's <> 0
  pvCurrentWritebTag++;
  if(pvCurrentWritebTag == 0){
    pvCurrentWritebTag++;
  }
  qDebug() << "mdtUsbtmcPortManager::writeData() - bTag: " << pvCurrentWritebTag;
  frame->setbTag(pvCurrentWritebTag);
  frame->setMessageData(data);
  frame->encode();
  pvPort->addFrameToWrite(frame);
  pvPort->unlockMutex();

  return pvCurrentWritebTag;
}

int mdtUsbtmcPortManager::sendReadRequest()
{
  Q_ASSERT(pvPort != 0);

  mdtFrameUsbTmc *frame;

  // Get a frame in pool
  pvPort->lockMutex();
  if(pvPort->writeFramesPool().size() < 1){
    pvPort->unlockMutex();
    mdtError e(MDT_PORT_IO_ERROR, "No frame available in write frames pool", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortManager");
    e.commit();
    return mdtAbstractPort::WritePoolEmpty;
  }
  frame = dynamic_cast<mdtFrameUsbTmc*> (pvPort->writeFramesPool().dequeue());
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();
  // Store data, encode and add frame to write queue
  frame->setWaitAnAnswer(true);
  frame->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_IN);
  // Increment bTag and enshure it's <> 0
  pvCurrentWritebTag++;
  if(pvCurrentWritebTag == 0){
    pvCurrentWritebTag++;
  }
  qDebug() << "mdtUsbtmcPortManager::sendReadRequest() - bTag: " << pvCurrentWritebTag;
  frame->setbTag(pvCurrentWritebTag);
  frame->setMessageData("");
  frame->encode();
  pvPort->addFrameToWrite(frame);
  pvPort->unlockMutex();

  return pvCurrentWritebTag;
}


/*
void mdtUsbtmcPortManager::abort()
{
  Q_ASSERT(pvPort != 0);

  ///mdtUsbPort *port = dynamic_cast<mdtUsbPort*> (pvPort);
  ///Q_ASSERT(port != 0);
  qDebug() << "mdtUsbtmcPortManager::abort() ...";
  pvPort->flushIn();
  pvPort->flushOut();
}
*/

void mdtUsbtmcPortManager::fromThreadNewFrameReaden()
{
  Q_ASSERT(pvPort != 0);

  mdtFrameUsbTmc *frame;

  // Get frames in readen queue
  pvPort->lockMutex();
  while(pvPort->readenFrames().size() > 0){
    frame = dynamic_cast<mdtFrameUsbTmc*> (pvPort->readenFrames().dequeue());
    Q_ASSERT(frame != 0);
    // Check if frame is complete
    /// \todo Error on incomplete frame
    if(frame->isComplete()){
      // Copy data
      QByteArray data;
      data.append(frame->messageData().data(), frame->messageData().size());
      ///pvReadenFrames.insert(frame->bTag(), data);
      commitFrame(frame->bTag(), data);
    }
    // Put frame back into pool
    pvPort->readFramesPool().enqueue(frame);
  };
  pvPort->unlockMutex();
  /**
  if(pvReadenFrames.size() > 0){
    emit(newReadenFrame());
  }
  */
}
