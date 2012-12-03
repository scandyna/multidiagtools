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
#include "mdtDeviceU3606A.h"
#include "mdtError.h"
#include <QByteArray>
#include <QList>

#include <QDebug>

mdtDeviceU3606A::mdtDeviceU3606A(QObject *parent)
 : mdtDevice(parent)
{
  int timeout;

  pvPortManager = new mdtUsbtmcPortManager;
  pvCodec = new mdtFrameCodecScpiU3606A;
  connect(pvPortManager, SIGNAL(newReadenFrame(int, QByteArray)), this, SLOT(decodeReadenFrame(int, QByteArray)));
  connect(pvPortManager, SIGNAL(errorStateChanged(int)), this, SLOT(setStateFromPortError(int)));
  timeout = pvPortManager->config().readTimeout();
  if(pvPortManager->config().writeTimeout() > timeout){
    timeout = pvPortManager->config().writeTimeout();
  }
  setBackToReadyStateTimeout(2*timeout);
  pvPortManager->setNotifyNewReadenFrame(true);

}

mdtDeviceU3606A::~mdtDeviceU3606A()
{
  delete pvPortManager;
  delete pvCodec;
}

mdtAbstractPort::error_t mdtDeviceU3606A::connectToDevice(const mdtDeviceInfo &devInfo)
{
  mdtPortInfo *port;
  QList<mdtPortInfo*> ports;
  QList<mdtDeviceInfo*> devices;
  mdtDeviceInfo device;
  int i, j;
  mdtAbstractPort::error_t retVal;

  // Setup device info
  device = devInfo;
  device.setVendorId(0x0957);
  device.setProductId(0x4d18);
  // Scan for ports having a USBTMC device attached
  port = 0;
  ports = pvPortManager->scan();
  for(i=0; i<ports.size(); i++){
    Q_ASSERT(ports.at(i) != 0);
    // Search in devices list
    qDebug() << "port " << ports.at(i)->portName();
    devices = ports.at(i)->deviceInfoList();
    for(j=0; j<devices.size(); j++){
      Q_ASSERT(devices.at(j) != 0);
      // Check if VID and PID matches
      if((devices.at(j)->vendorId() == device.vendorId())&&(devices.at(j)->productId() == device.productId())){
        // If requested, check if serial ID matches
        if(!device.serialId().isEmpty()){
          if(devices.at(j)->serialId() == device.serialId()){
            port = ports.at(i);
            i = ports.size();
            break;
          }
        }else{
          // Only PID and VID must match
          port = ports.at(i);
          i = ports.size();
          break;
        }
      }
    }
  }
  // Open port if found
  if(port != 0){
    pvPortManager->setPortInfo(*port);
    if(pvPortManager->openPort()){
      if(pvPortManager->start()){
        retVal = mdtAbstractPort::NoError;
      }else{
        retVal = mdtAbstractPort::UnhandledError;
      }
    }else{
      retVal = mdtAbstractPort::UnhandledError;
    }
  }else{
    // Device not found
    /// \todo Add DeviceNotFound error ?
    retVal = mdtAbstractPort::PortNotFound;
  }
  // Free port info list
  qDeleteAll(ports);
  ports.clear();

  return retVal;
}

void mdtDeviceU3606A::decodeReadenFrame(int id, QByteArray data)
{
  qDebug() << "mdtDeviceU3606A::decodeReadenFrame() - ID: " << id << " , data: " << data;
}

void mdtDeviceU3606A::onStateChanged(int state)
{
  qDebug() << "mdtDeviceU3606A::onStateChanged() ...";
}

int mdtDeviceU3606A::readAnalogInput(int address)
{
  Q_ASSERT(pvIos != 0);

  int bTag;

  /// \todo Add resolution and limits (min/max)
  /// \todo Handle type (Ampere, Voltage, Resistance, AC/DC, ...)
   // Wait until data can be sent
   if(!pvPortManager->waitOnWriteReady(500)){
     return mdtAbstractPort::WriteQueueEmpty;
   }
  // Send query
   bTag = pvPortManager->writeData("MEAS:VOLT:DC?\n");
   if(bTag < 0){
     return bTag;
   }
   // Wait until more data can be sent
   if(!pvPortManager->waitOnWriteReady(500)){
     return mdtAbstractPort::WriteQueueEmpty;
   }
   // Send read request
   return pvPortManager->sendReadRequest();
}
