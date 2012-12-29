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
#include "mdtDeviceScpi.h"
#include "mdtError.h"
#include <QByteArray>
#include <QList>

#include <QDebug>

mdtDeviceScpi::mdtDeviceScpi(QObject *parent)
 : mdtDevice(parent)
{
  int timeout;

  pvUsbtmcPortManager = new mdtUsbtmcPortManager;
  // Setup port manager
  pvUsbtmcPortManager->config().setReadTimeout(10000);
  ///connect(pvUsbtmcPortManager, SIGNAL(newReadenFrame(mdtPortTransaction)), this, SLOT(decodeReadenFrame(mdtPortTransaction)));
  ///connect(pvUsbtmcPortManager, SIGNAL(errorStateChanged(int)), this, SLOT(setStateFromPortError(int)));
  timeout = pvUsbtmcPortManager->config().readTimeout();
  if(pvUsbtmcPortManager->config().writeTimeout() > timeout){
    timeout = pvUsbtmcPortManager->config().writeTimeout();
  }
  setBackToReadyStateTimeout(2*timeout);
  pvOperationComplete = false;
}

mdtDeviceScpi::~mdtDeviceScpi()
{
  stop();
  delete pvUsbtmcPortManager;
}

mdtPortManager *mdtDeviceScpi::portManager()
{
  Q_ASSERT(pvUsbtmcPortManager != 0);

  return pvUsbtmcPortManager;
}

mdtAbstractPort::error_t mdtDeviceScpi::connectToDevice(const mdtDeviceInfo &devInfo)
{
  mdtPortInfo *port;
  QList<mdtPortInfo*> ports;
  QList<mdtDeviceInfo*> devices;
  ///mdtDeviceInfo device;
  int i, j;
  mdtAbstractPort::error_t retVal;

  // Setup device info
  ///device = devInfo;
  ///device.setVendorId(0x0957);
  ///device.setProductId(0x4d18);
  // Scan for ports having a USBTMC device attached
  port = 0;
  ports = pvUsbtmcPortManager->scan();
  for(i=0; i<ports.size(); i++){
    Q_ASSERT(ports.at(i) != 0);
    // Search in devices list
    qDebug() << "port " << ports.at(i)->portName();
    devices = ports.at(i)->deviceInfoList();
    for(j=0; j<devices.size(); j++){
      Q_ASSERT(devices.at(j) != 0);
      // Check if VID and PID matches
      if((devices.at(j)->vendorId() == devInfo.vendorId())&&(devices.at(j)->productId() == devInfo.productId())){
        // If requested, check if serial ID matches
        if(!devInfo.serialId().isEmpty()){
          if(devices.at(j)->serialId() == devInfo.serialId()){
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
    portManager()->setPortInfo(*port);
    if(portManager()->openPort()){
      if(portManager()->start()){
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

int mdtDeviceScpi::sendCommand(const QByteArray &command, int timeout)
{
  return pvUsbtmcPortManager->sendCommand(command, timeout);
}

QByteArray mdtDeviceScpi::sendQuery(const QByteArray &query, int writeTimeout, int readTimeout)
{
  return pvUsbtmcPortManager->sendQuery(query, writeTimeout, readTimeout);
}

bool mdtDeviceScpi::waitOperationComplete(int timeout, int interval)
{
  Q_ASSERT(interval > 0);

  int maxIter = timeout / interval;
  pvOperationComplete = false;
  QByteArray response;

  while(!pvOperationComplete){
    if(maxIter <= 0){
      return false;
    }
    // Query device
    response = sendQuery("*OPC?\n");
    if(response.size() > 0){
      if(response.at(0) == '1'){
        pvOperationComplete = true;
        return true;
      }
    }
    portManager()->wait(interval, 50);
    maxIter--;
  }

  return true;
}

