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
#include "mdtAnalogIo.h"
#include <QByteArray>
#include <QList>

#include <QDebug>

mdtDeviceU3606A::mdtDeviceU3606A(QObject *parent)
 : mdtDeviceScpi(parent)
{
  int timeout;

  ///pvPortManager = new mdtUsbtmcPortManager;
  pvCodec = new mdtFrameCodecScpiU3606A;
  // Setup port manager
  portManager()->config().setReadFrameSize(512);
  portManager()->config().setReadQueueSize(10);
  portManager()->config().setWriteFrameSize(512);
  portManager()->config().setWriteQueueSize(1);

  /// \todo setup read frames sizes
  
  /**
  pvPortManager->config().setReadTimeout(10000);
  ///pvPortManager->setNotifyNewReadenFrame(true);
  connect(pvPortManager, SIGNAL(newReadenFrame(mdtPortTransaction)), this, SLOT(decodeReadenFrame(mdtPortTransaction)));
  connect(pvPortManager, SIGNAL(errorStateChanged(int)), this, SLOT(setStateFromPortError(int)));
  timeout = pvPortManager->config().readTimeout();
  if(pvPortManager->config().writeTimeout() > timeout){
    timeout = pvPortManager->config().writeTimeout();
  }
  setBackToReadyStateTimeout(2*timeout);
  */

}

mdtDeviceU3606A::~mdtDeviceU3606A()
{
  ///delete pvPortManager;
  delete pvCodec;
}

/**
mdtPortManager *mdtDeviceU3606A::portManager()
{
  return pvPortManager;
}
*/

mdtAbstractPort::error_t mdtDeviceU3606A::connectToDevice(const mdtDeviceInfo &devInfo)
{
  mdtDeviceInfo device;

  // Setup device info
  device = devInfo;
  device.setVendorId(0x0957);
  device.setProductId(0x4d18);

  return mdtDeviceScpi::connectToDevice(device);
  
  /**
  mdtPortInfo *port;
  QList<mdtPortInfo*> ports;
  QList<mdtDeviceInfo*> devices;
  int i, j;
  mdtAbstractPort::error_t retVal;

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
  */
}

/**
int mdtDeviceU3606A::sendCommand(const QByteArray &command, int timeout)
{
  return pvPortManager->sendCommand(command, timeout);
}

QByteArray mdtDeviceU3606A::sendQuery(const QByteArray &query, int writeTimeout, int readTimeout)
{
  return pvPortManager->sendQuery(query, writeTimeout, readTimeout);
}
*/

void mdtDeviceU3606A::onStateChanged(int state)
{
  qDebug() << "mdtDeviceU3606A::onStateChanged() ...";
}

void mdtDeviceU3606A::decodeReadenFrame(mdtPortTransaction transaction)
{
  bool ok;

  qDebug() << "mdtDeviceU3606A::decodeReadenFrame() , ID: " << transaction.id();
  
  switch(transaction.type()){
    case MDT_FC_SCPI_VALUE:
      if(transaction.analogIo() == 0){
        mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received value response from device but no I/O was affected to query", mdtError::Warning);
        MDT_ERROR_SET_SRC(e, "mdtDeviceU3606A");
        e.commit();
      }else{
        ok = pvCodec->decodeValues(transaction.data());
        if(ok && (pvCodec->values().size() == 1)){
          transaction.analogIo()->setValue(pvCodec->values().at(0), false);
        }else{
          transaction.analogIo()->setValue(QVariant(), false);
        }
      }
      break;
    default:
      mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received unsupported response from device (type: " + QString::number(transaction.type()) + ")", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDeviceU3606A");
      e.commit();
  }
}

int mdtDeviceU3606A::readAnalogInput(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  int bTag;

  /// \todo Add resolution and limits (min/max)
  /// \todo Handle type (Ampere, Voltage, Resistance, AC/DC, ...)
  // Wait until data can be sent
  if(!pvUsbtmcPortManager->waitOnWriteReady(500)){
    return mdtAbstractPort::WritePoolEmpty;
  }
  // Send query
  bTag = pvUsbtmcPortManager->writeData("MEAS:VOLT:DC?\n");
  if(bTag < 0){
    return bTag;
  }
  // Wait until more data can be sent
  if(!pvUsbtmcPortManager->waitOnWriteReady(500)){
    return mdtAbstractPort::WritePoolEmpty;
  }
  // Remember query type.
  transaction->setType(MDT_FC_SCPI_VALUE);
  // Send read request
  bTag = pvUsbtmcPortManager->sendReadRequest(transaction);

  return bTag;
}
