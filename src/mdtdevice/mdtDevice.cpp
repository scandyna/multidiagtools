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
#include "mdtDevice.h"
#include "mdtError.h"
#include "mdtAnalogIo.h"
#include "mdtPortManager.h"
#include <QTimer>
#include <QList>

#include <QDebug>

mdtDevice::mdtDevice(QObject *parent)
 : QObject(parent)
{
  pvIos = 0;
  pvOutputWriteReplyTimeout = 500;
  pvAnalogOutputAddressOffset = 0;
  pvCurrentState = Unknown;
  setStateDisconnected();
  setName(tr("Unknown"));
  pvQueryTimer = new QTimer(this);
  connect(pvQueryTimer, SIGNAL(timeout()), this, SLOT(runQueries()));
}

mdtDevice::~mdtDevice()
{
  stop();
  delete pvQueryTimer;
}

void mdtDevice::setName(const QString &name)
{
  pvName = name;
}

QString mdtDevice::name() const
{
  return pvName;
}

void mdtDevice::setIos(mdtDeviceIos *ios, bool autoAnalogOutputUpdate)
{
  Q_ASSERT(ios != 0);

  int i;
  QList<mdtAnalogIo*> analogOutputs;

  pvIos = ios;
  if(autoAnalogOutputUpdate){
    analogOutputs = pvIos->analogOutputs();
    for(i=0; i<analogOutputs.size(); i++){
      connect(analogOutputs.at(i), SIGNAL(valueChanged(int, int)), this, SLOT(setAnalogOutputValue(int, int)));
    }
  }
}

void mdtDevice::setOutputWriteReplyTimeout(int timeout)
{
  pvOutputWriteReplyTimeout = timeout;
}

void mdtDevice::setAnalogOutputAddressOffset(int offset)
{
  pvAnalogOutputAddressOffset = offset;
}

void mdtDevice::start(int queryInterval)
{
  pvQueryTimer->setInterval(queryInterval);
  pvQueryTimer->start();
}

void mdtDevice::stop()
{
  pvQueryTimer->stop();
}

bool mdtDevice::readAnalogInput(int address)
{
  return false;
}

int mdtDevice::readAnalogInputs()
{
  return -1;
}

int mdtDevice::readAnalogOutputs()
{
  return -1;
}

int mdtDevice::writeAnalogOutputValue(int address, int value, int confirmationTimeout)
{
  return -1;
}

mdtDevice::state_t mdtDevice::state() const
{
  return pvCurrentState;
}

void mdtDevice::setAnalogOutputValue(int address, int value)
{
  if(pvCurrentState != Ready){
    // Device busy, cannot threat query , try later
    ///QTimer::singleShot(500, this, SLOT(setStateReady()));
    qDebug() << "Device busy, retry later ...";
    return;
  }
  if(writeAnalogOutputValue(address, value, pvOutputWriteReplyTimeout) < 0){
    setStateBusy(500);
  }
}

void mdtDevice::decodeReadenFrames(int, QByteArray)
{
}

void mdtDevice::runQueries()
{
  if(pvCurrentState != Ready){
    // Stop query timer
    ///pvQueryTimer->stop();
    return;

    // Retry and update state if Ok
    /**
    if(queriesSequence()){
      setStateReady();
      pvQueryTimer->start();
    }else{
      // Retry later
      pvQueryTimer->stop();
      QTimer::singleShot(pvQueryTimer->interval() + 500, this, SLOT(runQueries()));
      return;
    }
    */
  }
  // Re-arm query timer if it was stopped
  /**
  if(!pvQueryTimer->isActive()){
    pvQueryTimer->start();
  }
  */
  queriesSequence();
}

bool mdtDevice::queriesSequence()
{
  return false;
}

void mdtDevice::setStateReady()
{
  if(pvCurrentState != Ready){
    pvCurrentState = Ready;
    qDebug() << "mdtDevice: new state is Ready";
    emit(stateChanged(pvCurrentState));
  }
}

void mdtDevice::setStateDisconnected()
{
  if(pvCurrentState != Disconnected){
    pvCurrentState = Disconnected;
    qDebug() << "mdtDevice: new state is Disconnected";
    emit(stateChanged(pvCurrentState));
  }
}

void mdtDevice::setStateBusy(int retryTimeout)
{
  if(pvCurrentState != Busy){
    pvCurrentState = Busy;
    qDebug() << "mdtDevice: new state is Busy";
    emit(stateChanged(pvCurrentState));
  }
  // Set state ready if requested
  if(retryTimeout >= 0){
    QTimer::singleShot(retryTimeout, this, SLOT(setStateReady()));
  }
}

void mdtDevice::setStateUnknown()
{
  if(pvCurrentState != Unknown){
    pvCurrentState = Unknown;
    // Add a error
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + " goes to unknown state", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    qDebug() << "mdtDevice: new state is Unknown";
    emit(stateChanged(pvCurrentState));
  }
}

void mdtDevice::setStateFromPortError(int error)
{
  if(error == mdtAbstractPort::NoError){
    setStateReady();
  }else if(error == mdtAbstractPort::Disconnected){
    setStateDisconnected();
  }else if(error == mdtAbstractPort::WriteQueueEmpty){
    setStateBusy();
  }else{
    setStateUnknown();
  }
}

void mdtDevice::addTransaction(int id, mdtAnalogIo *io)
{
  pvPendingAioTransactions.insert(id, io);
}

void mdtDevice::addTransaction(int id, mdtDigitalIo *io)
{
  pvPendingDioTransactions.insert(id, io);
}

mdtAnalogIo *mdtDevice::pendingAioTransaction(int id)
{
  if(!pvPendingAioTransactions.contains(id)){
    return 0;
  }
  return pvPendingAioTransactions.take(id);
}

mdtDigitalIo *mdtDevice::pendingDioTransaction(int id)
{
  if(!pvPendingDioTransactions.contains(id)){
    return 0;
  }
  return pvPendingDioTransactions.take(id);
}

bool mdtDevice::waitTransactionDone(int id, int timeout, int gr)
{
  qDebug() << "mdtDevice::waitTransactionDone() TID: " << id;
  int i = timeout / gr;
  // Select correct queue
  if(pvPendingAioTransactions.contains(id)){
    qDebug() << "mdtDevice::waitTransactionDone(): queue: " << pvPendingAioTransactions.keys();
    while(pvPendingAioTransactions.contains(id)){
      // Check timeout
      if(i <= 0){
        qDebug() << "mdtDevice::waitTransactionDone(): timeout for TID " << id;
        pvPendingAioTransactions.remove(id);
        setStateBusy(2000);
        return false;
      }
      mdtPortManager::wait(gr, gr);
      i--;
    }
  }else if(pvPendingDioTransactions.contains(id)){
    qDebug() << "mdtDevice::waitTransactionDone(): queue: " << pvPendingDioTransactions.keys();
    while(pvPendingDioTransactions.contains(id)){
      // Check timeout
      if(i <= 0){
        qDebug() << "mdtDevice::waitTransactionDone(): timeout for TID " << id;
        pvPendingDioTransactions.remove(id);
        setStateBusy(2000);
        return false;
      }
      mdtPortManager::wait(gr, gr);
      i--;
    }
  }else{
    qDebug() << "mdtDevice::waitTransactionDone(): id not found";
    return false;
  }
  qDebug() << "mdtDevice::waitTransactionDone(): transaction DONE for TID " << id;

  return true;
}
