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
#include "mdtDevice.h"
#include <QTimer>
#include <QCoreApplication>

#include <QDebug>

mdtDevice::mdtDevice(QObject *parent)
 : QObject(parent),
   pvCurrentState(State_t::Disconnected)
{
  qDebug() << "mdtDevice::mdtDevice() ...";
  //pvIos = new mdtDeviceIos;
  pvBackToReadyStateTimeout = -1;
//   pvBackToReadyStateTimer = new QTimer(this);
//   pvBackToReadyStateTimer->setSingleShot(true);
  ///setName(tr("Unknown"));
  pvAutoQueryEnabled = false;
  pvQueryTimer = new QTimer(this);
  ///currentState() = mdtPortManager::Disconnected;
  ///setStateDisconnected();
  ///connect(pvBackToReadyStateTimer, SIGNAL(timeout()), this, SLOT(setStateReady()));
  ///connect(pvBackToReadyStateTimer, SIGNAL(timeout()), this, SIGNAL(deviceReady()));
  connect(pvQueryTimer, SIGNAL(timeout()), this, SLOT(runQueries()));
  qDebug() << "mdtDevice::mdtDevice() DONE";
}

mdtDevice::~mdtDevice()
{
  disconnectFromDevice();
  delete pvQueryTimer;
}

/**
void mdtDevice::setName(const QString &name)
{
  pvName = name;
  nameChangedEvent(name);
}

QString mdtDevice::name() const
{
  return pvName;
}
*/
/**
void mdtDevice::setIdentification(const QVariant & id)
{
  pvIdentification = id;
}
*/

void mdtDevice::setDeviceAddress(const mdtDeviceAddress & address, const QString & alias)
{
  Q_ASSERT(pvCurrentState == State_t::Disconnected);

  pvDeviceAddress = address;
  if(!alias.isEmpty()){
    pvDeviceAddress.setAlias(alias);
  }
  // Tell subclass that address changed
  deviceAddressChangedEvent(pvDeviceAddress);
}


// mdtAbstractPort::error_t mdtDevice::connectToDevice(const mdtDeviceInfo &devInfo)
// {
//   return mdtAbstractPort::UnhandledError;
// }

// bool mdtDevice::connectToDevice(const mdtDeviceAddress & address)
// {
//   pvDeviceAddress = address;
//   return connectToDeviceImpl(pvDeviceAddress);
// }


void mdtDevice::disconnectFromDevice()
{
  stop();
  disconnectFromDeviceEvent();
}

void mdtDevice::setBackToReadyStateTimeout(int timeout)
{
  pvBackToReadyStateTimeout = timeout;
}

void mdtDevice::start(int queryInterval)
{
  pvQueryTimer->setInterval(queryInterval);
  pvAutoQueryEnabled = true;
  if(isReady()){
    pvQueryTimer->start();
  }
}

void mdtDevice::stop()
{
  pvAutoQueryEnabled = false;
  pvQueryTimer->stop();
}

void mdtDevice::wait(int t)
{
  pvWaitTimeReached = false;
  QTimer::singleShot(t, this, SLOT(setWaitTimeReached()));
  while(!pvWaitTimeReached){
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
  }
}

void mdtDevice::runQueries()
{
  if(currentState() != State_t::Ready){
    return;
  }
  queriesSequence();
}

bool mdtDevice::queriesSequence()
{
  return false;
}

void mdtDevice::setCurrentState(mdtDevice::State_t state)
{
  // Update state and signal it
  pvCurrentState = state;
  stateChangedEvent(state);
  emit stateChanged(state);
  // Do our internall stuff
  if(state == State_t::Ready){
    allowQueryTimer();
  }else{
    blockQueryTimer();
  }
}

void mdtDevice::setWaitTimeReached()
{
  pvWaitTimeReached = true;
}

void mdtDevice::allowQueryTimer()
{
  if( pvAutoQueryEnabled && isReady() ){
    qDebug() << "mdtDevice: starting query timer ...";
    pvQueryTimer->start();
  }
}

void mdtDevice::blockQueryTimer()
{
  if(pvAutoQueryEnabled){
    pvQueryTimer->stop();
  }
}
