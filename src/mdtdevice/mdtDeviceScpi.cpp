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
#include "mdtFrameCodecScpi.h"
#include <QByteArray>
#include <QString>
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
  ///connect(pvUsbtmcPortManager, SIGNAL(errorStateChanged(int, const QString&, const QString&)), this, SLOT(setStateFromPortError(int, const QString&, const QString&)));
  connect(pvUsbtmcPortManager, SIGNAL(stateChanged(int)), this, SLOT(setStateFromPortManager(int)));
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

  int maxIter;
  pvOperationComplete = false;
  QByteArray response;

  maxIter = timeout / interval;
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

int mdtDeviceScpi::checkDeviceError()
{
  int bTag;
  mdtPortTransaction *transaction;

  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setQueryReplyMode(false);
  // Wait until data can be sent
  if(!pvUsbtmcPortManager->waitOnWriteReady()){
    return mdtAbstractPort::WritePoolEmpty;
  }
  // Send query
  bTag = pvUsbtmcPortManager->writeData("SYST:ERR?\n");
  if(bTag < 0){
    return bTag;
  }
  // Wait until more data can be sent
  if(!pvUsbtmcPortManager->waitOnWriteReady()){
    return mdtAbstractPort::WritePoolEmpty;
  }
  // Remember query type.
  ///transaction->setType(MDT_FC_SCPI_ERR);
  transaction->setType(mdtFrameCodecScpi::QT_ERR);
  // Send read request
  bTag = pvUsbtmcPortManager->sendReadRequest(transaction);
  if(bTag < 0){
    return bTag;
  }

  return 0;
}

void mdtDeviceScpi::handleDeviceError(const QList<QVariant> &decodedValues)
{
  int errNum;
  QString errText;
  QList<QString> deviceSpecificTexts;
  int i;

  // Check decoded values
  if(decodedValues.size() < 2){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": unexpected size of values (excpected min. 2)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceScpi");
    e.commit();
    return;
  }
  // Get error number
  if(decodedValues.at(0).type() != QVariant::Int){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": values contains not a error (First value must be type int)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceScpi");
    e.commit();
    return;
  }
  errNum = decodedValues.at(0).toInt();
  if(errNum == 0){
    return;
  }
  // Get error message
  if(decodedValues.at(1).type() != QVariant::String){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": values contains not a error (Second value must be type string)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceScpi");
    e.commit();
    return;
  }
  errText = decodedValues.at(1).toString();
  // Get device's specific messages
  for(i=1; i<decodedValues.size(); i++){
    deviceSpecificTexts.append(decodedValues.at(i).toString());
  }
  // Handling
  if(errNum > 0){
    handleDeviceSpecificError(errNum, errText, deviceSpecificTexts);
  }else{
    // Standard SCPI errors
    switch(errNum){
      default:
        logDeviceError(errNum, errText, deviceSpecificTexts, mdtError::Error);
    }
  }

}

void mdtDeviceScpi::handleDeviceSpecificError(int errNum, const QString &errText, const QList<QString> &deviceSpecificTexts)
{
  // In this implementation, we only log error as warning.
  // The specific subclass should re-implement this method, and handle error.
  logDeviceError(errNum, errText, deviceSpecificTexts, mdtError::Warning);
}

void mdtDeviceScpi::logDeviceError(int errNum, const QString &errText, const QList<QString> &deviceSpecificTexts, mdtError::level_t level)
{
  QString specificTexts;
  QString message;
  int i;

  // Cat specific texts
  for(i=0; i<deviceSpecificTexts.size(); i++){
    if(i>0){
      specificTexts += ";";
    }
    specificTexts += deviceSpecificTexts.at(i);
  }
  // Build message
  message = "Device " + name() + " returned error number ";
  message += QString::number(errNum);
  message += " , message: " + errText;
  if(specificTexts.size() > 0){
    message += " , details: " + specificTexts;
  }
  mdtError e(MDT_DEVICE_ERROR, message, level);
  MDT_ERROR_SET_SRC(e, "mdtDeviceScpi");
  e.commit();
}
