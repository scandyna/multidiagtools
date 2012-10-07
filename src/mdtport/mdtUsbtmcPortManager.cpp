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
#include "mdtAlgorithms.h"
#include "mdtError.h"
#include "mdtPortConfig.h"
#include "mdtFrame.h"
#include "mdtFrameUsbTmc.h"
#include "mdtUsbDeviceDescriptor.h"
#include "mdtUsbConfigDescriptor.h"
#include "mdtUsbInterfaceDescriptor.h"
#include <QDir>
#include <QFileInfoList>
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

/**
QStringList mdtUsbtmcPortManager::scan()
{
  QStringList portNames;
  QStringList availablePorts;
  mdtUsbtmcPort *port;
  QFileInfoList filesInfo;
  QDir dir;
  QStringList extList;

  // We only need system files
  dir.setFilter(QDir::Files | QDir::System);
  // USBTMC interfaces
  extList << "usbtmc*";
  // Common device directory
  dir.setPath("/dev");
  if(!dir.exists()){
    mdtError e(MDT_UNDEFINED_ERROR, "directory '" + dir.dirName() + "' not exists", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortManagerPosix");
    e.commit();
    return availablePorts;
  }
  dir.setNameFilters(extList);
  // Gest list of found files
  filesInfo = dir.entryInfoList();
  // Construct the list of absolute file paths
  for(int i=0; i<filesInfo.size(); ++i){
    portNames << filesInfo.at(i).absoluteFilePath();
  }
  // Sort the list
  portNames = mdtAlgorithms::sortStringListWithNumericEnd(portNames);
  // For each port name, try to open the port and get some attributes (to see if it really exists)
  for(int i=0; i<portNames.size(); ++i){
    // Try to open port
    port = new mdtUsbtmcPort;
    Q_ASSERT(port != 0);
    port->setPortName(portNames.at(i));
    if(port->open() == mdtAbstractPort::NoError){
      availablePorts.append(portNames.at(i));
      port->close();
    }
    delete port;
  }

  return availablePorts;
}
*/

QList<mdtPortInfo*> mdtUsbtmcPortManager::scan22()
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
          qDebug() << "->   iface[" << k << "]";
          // USBTMC has a application specific bInterfaceClass, a bInterfaceSubClass == 0x03
          if(ifaceDescriptor->bInterfaceClass() == 0xFE){
            if(ifaceDescriptor->bInterfaceSubClass() == 0x03){
              // Here we found a USBTMC device
              deviceInfo = new mdtDeviceInfo;
              deviceInfo->setVendorId(deviceDescriptor.idVendor());
              deviceInfo->setProductId(deviceDescriptor.idProduct());
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
        portName = "0x" + QString::number((unsigned int)libusb_get_bus_number(device), 16);
        portName += ":0x";
        portName += QString::number((unsigned int)libusb_get_device_address(device), 16);
        portName += ":0x";
        portName += QString::number((unsigned int)deviceInfo->vendorId(), 16);
        portName += ":0x";
        portName += QString::number((unsigned int)deviceInfo->productId(), 16);
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

bool mdtUsbtmcPortManager::writeData(QByteArray data)
{
  Q_ASSERT(pvPort != 0);

  mdtFrameUsbTmc *frame;

  // Clear previous readen frames
  pvReadenFrames.clear();
  // Get a frame in pool
  pvPort->lockMutex();
  if(pvPort->writeFramesPool().size() < 1){
    pvPort->unlockMutex();
    mdtError e(MDT_PORT_IO_ERROR, "No frame available in write frames pool", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortManager");
    e.commit();
    return false;
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
  frame->setbTag(pvCurrentWritebTag);
  frame->setMessageData(data);
  frame->encode();
  pvPort->addFrameToWrite(frame);
  pvPort->unlockMutex();

  return true;
}

bool mdtUsbtmcPortManager::sendReadRequest()
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
    return false;
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
  frame->setbTag(pvCurrentWritebTag);
  frame->setMessageData("");
  frame->encode();
  pvPort->addFrameToWrite(frame);
  pvPort->unlockMutex();

  return true;
}

bool mdtUsbtmcPortManager::waitReadenFrame(int timeout)
{
  Q_ASSERT(pvPort != 0);

  if(!sendReadRequest()){
    return false;
  }

  return mdtPortManager::waitReadenFrame(timeout);
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
  QByteArray data;

  // Get frames in readen queue
  pvPort->lockMutex();
  while(pvPort->readenFrames().size() > 0){
    frame = dynamic_cast<mdtFrameUsbTmc*> (pvPort->readenFrames().dequeue());
    Q_ASSERT(frame != 0);
    // Check if frame is complete
    if(frame->isComplete()){
      // Copy data
      data.append(frame->messageData().data(), frame->messageData().size());
      pvReadenFrames.append(data);
    }
    // Put frame back into pool
    pvPort->readFramesPool().enqueue(frame);
  };
  pvPort->unlockMutex();
  emit(newReadenFrame());
}
