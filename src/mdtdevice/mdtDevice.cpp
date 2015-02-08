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
#include "mdtAnalogIo.h"
///#include "mdtPortManager.h"
#include <QTimer>

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
  setName(tr("Unknown"));
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

void mdtDevice::setName(const QString &name)
{
  pvName = name;
}

QString mdtDevice::name() const
{
  return pvName;
}

void mdtDevice::setIdentification(const QVariant & id)
{
  pvIdentification = id;
}

mdtAbstractPort::error_t mdtDevice::connectToDevice(const mdtDeviceInfo &devInfo)
{
  return mdtAbstractPort::UnhandledError;
}

void mdtDevice::disconnectFromDevice()
{
  stop();
  disconnectFromDeviceEvent();
  /**
  if(portManager() != 0){
    if(!portManager()->isClosed()){
      portManager()->stop();
    }
  }
  */
}

void mdtDevice::setBackToReadyStateTimeout(int timeout)
{
  pvBackToReadyStateTimeout = timeout;
}

// mdtPortManager *mdtDevice::portManager()
// {
//   return 0;
// }

// bool mdtDevice::isReady()
// {
//   if(portManager() == 0){
//     return false;
//   }
//   return portManager()->isReady();
// }

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

/// \todo Implement !
void mdtDevice::wait(int ms)
{
  ///Q_ASSERT(portManager() != 0);

  ///portManager()->wait(ms);
}

mdtError mdtDevice::lastError() const
{
  return pvLastError;
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

// void mdtDevice::decodeReadenFrame(mdtPortTransaction *transaction)
// {
// }

// mdtPortTransaction *mdtDevice::getNewTransaction()
// {
//   Q_ASSERT(portManager() != 0);
// 
//   return portManager()->getNewTransaction();
// }
// 
// void mdtDevice::restoreTransaction(mdtPortTransaction *transaction)
// {
//   Q_ASSERT(portManager() != 0);
//   Q_ASSERT(transaction != 0);
// 
//   portManager()->restoreTransaction(transaction);
// }
// 
// bool mdtDevice::waitTransactionDone(int id)
// {
//   Q_ASSERT(portManager() != 0);
// 
//   bool ok;
// 
//   ok = portManager()->waitTransactionDone(id);
//   /*
//    * Request was send, response arrived,
//    * subclass has decoded response and updated I/O.
//    * So, we have to remove transaction from done queue and restore it to pool
//    * In mdtPortManager it was choosen to not let access
//    * to transaction queues management, so we call readenFrame()
//    * witch does all the needed job.
//    */
//   portManager()->readenFrame(id);
// 
//   return ok;
// }

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

// void mdtDevice::setStateFromPortManager(int portManagerState)
// {
//   switch((mdtPortManager::state_t)portManagerState){
//     case mdtPortManager::PortClosed:
//       ///setStatePortClosed();
//       setCurrentState(State_t::Disconnected);
//       break;
//     case mdtPortManager::Disconnected:
//       ///setStateDisconnected();
//       setCurrentState(State_t::Disconnected);
//       break;
//     case mdtPortManager::Connecting:
//       ///setStateConnecting();
//       setCurrentState(State_t::Connecting);
//       break;
//     case mdtPortManager::Ready:
//       ///setStateReady();
//       setCurrentState(State_t::Ready);
//       break;
//     case mdtPortManager::Busy:
//       //setStateBusy();
//       //break;
//     case mdtPortManager::PortError:
//       ///setStateError();
//       setCurrentState(State_t::Error);
//       break;
//     case mdtPortManager::Stopped:
//     case mdtPortManager::Starting:
//     case mdtPortManager::Stopping:
//     case mdtPortManager::Running:
//     case mdtPortManager::PortReady:
//     case mdtPortManager::Connected:
//       ///emit stateChanged(portManagerState);
//       setCurrentState(State_t::Error);
//       break;
//   }
// }

// void mdtDevice::setStatePortClosed()
// {
//   // Stop auto queries if running
//   if(pvAutoQueryEnabled){
//     pvQueryTimer->stop();
//   }
//   stateChangedEvent(State_t::Disconnected);
//   ///emit(stateChanged(currentState()));
// }
// 
// void mdtDevice::setStateDisconnected()
// {
//   // Stop auto queries if running
//   if(pvAutoQueryEnabled){
//     pvQueryTimer->stop();
//   }
//   stateChangedEvent(State_t::Disconnected);
//   qDebug() << "mdtDevice: new state is Disconnected";
//   ///emit(stateChanged(currentState()));
// }
// 
// void mdtDevice::setStateConnecting(/*const QString &message*/)
// {
//   // Stop auto queries if running
//   if(pvAutoQueryEnabled){
//     pvQueryTimer->stop();
//   }
//   stateChangedEvent(State_t::Connecting);
//   // Thread will notify the ready (or disconnected) state, cancel retry timer
//   ///pvBackToReadyStateTimer->stop();
//   qDebug() << "mdtDevice: new state is Connecting";
//   ///emit(stateChanged(currentState()));
// }
// 
// void mdtDevice::setStateReady()
// {
//   qDebug() << "mdtDevice: new state is Ready";
//   // Check if we have to restart query timer
//   if((pvAutoQueryEnabled)&&(isReady())){
//     qDebug() << "mdtDevice: starting query timer ...";
//     pvQueryTimer->start();
//   }
//   stateChangedEvent(State_t::Ready);
//   ///emit(stateChanged(currentState()));
// }

// void mdtDevice::setStateBusy()
// {
//   // Stop auto queries if running
//   if(pvAutoQueryEnabled){
//     pvQueryTimer->stop();
//   }
//   // Update I/Os
//   if(pvIos != 0){
//     pvIos->setAnalogInputsValue(mdtValue());
//     pvIos->setAnalogOutputsValue(mdtValue());
//     pvIos->setAnalogOutputsEnabled(false);
//     pvIos->setDigitalInputsValue(mdtValue());
//     pvIos->setDigitalOutputsValue(mdtValue());
//     pvIos->setDigitalOutputsEnabled(false);
//   }
//   qDebug() << "**** mdtDevice: new state is Busy";
//   emit(stateChanged(currentState()));
//   // Set state ready if requested
//   if(pvBackToReadyStateTimeout >= 0){
//     pvBackToReadyStateTimer->start(pvBackToReadyStateTimeout);
//     ///QTimer::singleShot(retryTimeout, this, SLOT(setStateReady()));
//   }
// }

// void mdtDevice::setStateError()
// {
//   // Stop auto queries if running
//   if(pvAutoQueryEnabled){
//     pvQueryTimer->stop();
//   }
//   stateChangedEvent(State_t::Error);
//   // Add a error
//   mdtError e(MDT_DEVICE_ERROR, "Device " + name() + " goes to error state", mdtError::Error);
//   MDT_ERROR_SET_SRC(e, "mdtDevice");
//   e.commit();
//   qDebug() << "mdtDevice: new state is Error";
//   ///emit(stateChanged(currentState()));
// }

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
