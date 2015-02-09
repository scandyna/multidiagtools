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
#include "mdtDeviceScpi.h"
#include "mdtError.h"
#include "mdtFrameCodecScpi.h"
#include <QByteArray>
#include <QString>
#include <QList>
#include <QTimer>
#include <QCoreApplication>

#include <QDebug>

mdtDeviceScpi::mdtDeviceScpi(QObject *parent)
 : mdtDevice(parent),
/// : QObject(parent),
   pvPort(new mdtUsbtmcPort(this))
{
  pvOperationComplete = false;
  pvOperationCompleteTryLeft = 0;
  pvOperationCompleteTimer = new QTimer(this);
  pvOperationCompleteTimer->setSingleShot(true);
  connect(pvOperationCompleteTimer, SIGNAL(timeout()), this, SLOT(queryAboutOperationComplete()));
}

mdtDeviceScpi::~mdtDeviceScpi()
{
}

bool mdtDeviceScpi::connectToDevice(uint16_t idVendor, uint16_t idProduct, const QString & serialNumber)
{
  if(!pvPort->openDevice(idVendor, idProduct, serialNumber)){
    pvLastError = pvPort->lastError();
    return false;
  }
  return true;
}

// void mdtDeviceScpi::disconnectFromDevice()
// {
//   pvPort->close();
// }

bool mdtDeviceScpi::sendCommand(const QByteArray& command, int timeout)
{
  if(!pvPort->sendCommand(command, timeout)){
    pvLastError = pvPort->lastError();
    return false;
  }
  return true;
}

QByteArray mdtDeviceScpi::sendQuery(const QByteArray & query, int timeout)
{
  QByteArray data = pvPort->sendQuery(query, timeout);
  if(data.isEmpty()){
    pvLastError = pvPort->lastError();
  }
  return data;
}

mdtError mdtDeviceScpi::getDeviceError(const QString & errorText)
{
  QByteArray data;
  mdtCodecScpi codec;

  data = sendQuery("SYST:ERR?\n");
  if(data.isEmpty()){
    return mdtError();
  }

  return codec.decodeDeviceError(data, errorText);
}

bool mdtDeviceScpi::waitOperationComplete(int timeout, int interval)
{
  Q_ASSERT(interval > 0);
  Q_ASSERT(pvOperationCompleteTimer != 0);

  ///int maxIter;
  pvOperationComplete = false;
  pvOperationCompleteTryLeft = timeout / interval;
  QByteArray response;

  ///maxIter = timeout / interval;
  // Check one time immediatly
  response = sendQuery("*OPC?\n");
  if(response.size() > 0){
    if(response.at(0) == '1'){
      return true;
    }
  }
  // Check at regular interval
  pvOperationCompleteTryLeft--;
  pvOperationCompleteTimer->setInterval(interval);
  pvOperationCompleteTimer->start();
  while(!pvOperationComplete){
    // Check that port manager is not about to close or in error
    /**
    if(!portManager()->isReady()){
      pvOperationCompleteTimer->stop();
      return false;
    }
    */
    // Check about operation complete timeout
    if(pvOperationCompleteTryLeft < 1){
      return false;
    }
    // Wait
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
  }

  return true;
}

void mdtDeviceScpi::disconnectFromDeviceEvent()
{
  qDebug() << "mdtDeviceScpi::disconnectFromDeviceEvent() ...";
  pvPort->close();
}

void mdtDeviceScpi::nameChangedEvent(const QString & newName)
{
  qDebug() << "Setting name: " << newName;
  pvPort->setDeviceName(newName);
}

void mdtDeviceScpi::queryAboutOperationComplete()
{
  QByteArray response;

  // Query device
  response = sendQuery("*OPC?\n");
  qDebug() << "mdtDeviceScpi::queryAboutOperationComplete() - response: " << response;
  if(response.size() > 0){
    if(response.at(0) == '1'){
      pvOperationComplete = true;
      return;
    }
  }
  pvOperationCompleteTryLeft--;
  if(pvOperationCompleteTryLeft > 0){
    pvOperationCompleteTimer->start();
  }
}
