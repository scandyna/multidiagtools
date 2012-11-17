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
  pvDigitalOutputAddressOffset = 0;
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

void mdtDevice::setIos(mdtDeviceIos *ios, bool autoOutputUpdate)
{
  Q_ASSERT(ios != 0);

  int i;
  QList<mdtAnalogIo*> analogOutputs;
  QList<mdtDigitalIo*> digitalOutputs;

  pvIos = ios;
  if(autoOutputUpdate){
    analogOutputs = pvIos->analogOutputs();
    for(i=0; i<analogOutputs.size(); i++){
      Q_ASSERT(analogOutputs.at(i) != 0);
      connect(analogOutputs.at(i), SIGNAL(valueChanged(int, int)), this, SLOT(setAnalogOutputValue(int, int)));
    }
    digitalOutputs = pvIos->digitalOutputs();
    for(i=0; i<digitalOutputs.size(); i++){
      Q_ASSERT(digitalOutputs.at(i) != 0);
      connect(digitalOutputs.at(i), SIGNAL(stateChanged(int, bool)), this, SLOT(setDigitalOutputState(int, bool)));
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

void mdtDevice::setDigitalOutputAddressOffset(int offset)
{
  pvDigitalOutputAddressOffset = offset;
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

QVariant mdtDevice::getAnalogInputValue(int address, int timeout, bool convert)
{
  int transactionId;
  mdtAnalogIo *ai;

  if(pvIos == 0){
    return QVariant();
  }
  // Check if only cached value is requested
  if(timeout < 0){
    ai = pvIos->analogInputAt(address);
    if(ai == 0){
      mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog input assigned to address " + QString::number(address), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDevice");
      e.commit();
      return QVariant();
    }
    // Return value
    if(convert){
      return QVariant(ai->value());
    }else{
      return QVariant(ai->valueInt());
    }
  }
  // Send query
  transactionId = readAnalogInput(address);
  if(transactionId < 0){
    setStateFromPortError(transactionId);
    return QVariant();
  }
  // Wait on result if needed
  if(timeout > 0){
    ai = pvIos->analogInputAt(address);
    if(ai == 0){
      mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog input assigned to address " + QString::number(address), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDevice");
      e.commit();
      return QVariant();
    }
    addTransaction(transactionId, ai);
    if(!waitTransactionDone(transactionId, timeout)){
      return QVariant();
    }
    // Return value
    if(convert){
      return QVariant(ai->value());
    }else{
      return QVariant(ai->valueInt());
    }
  }

  return QVariant();
}

int mdtDevice::getAnalogInputs(int timeout)
{
  int transactionId;

  if(pvIos == 0){
    return -1;
  }
  // Send query
  transactionId = readAnalogInputs();
  if(transactionId < 0){
    setStateFromPortError(transactionId);
    return transactionId;
  }
  // Wait on result if needed
  if(timeout > 0){
    addTransaction(transactionId);
    if(!waitTransactionDone(transactionId, timeout)){
      return -1;
    }
  }

  return transactionId;
}

QVariant mdtDevice::getAnalogOutputValue(int address, int timeout, bool convert)
{
  int transactionId;
  mdtAnalogIo *ao;

  if(pvIos == 0){
    return QVariant();
  }
  // Check if only cached value is requested
  if(timeout < 0){
    ao = pvIos->analogOutputAt(address);
    if(ao == 0){
      mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog input assigned to address " + QString::number(address), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDevice");
      e.commit();
      return QVariant();
    }
    // Return value
    if(convert){
      return QVariant(ao->value());
    }else{
      return QVariant(ao->valueInt());
    }
  }
  // Send query
  transactionId = readAnalogOutput(address);
  if(transactionId < 0){
    setStateFromPortError(transactionId);
    return QVariant();
  }
  // Wait on result if needed
  if(timeout > 0){
    ao = pvIos->analogOutputAt(address);
    if(ao == 0){
      mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog output assigned to address " + QString::number(address), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDevice");
      e.commit();
      return QVariant();
    }
    addTransaction(transactionId, ao);
    if(!waitTransactionDone(transactionId, timeout)){
      return QVariant();
    }
    // Return value
    if(convert){
      return QVariant(ao->value());
    }else{
      return QVariant(ao->valueInt());
    }
  }

  return QVariant();
}

int mdtDevice::getAnalogOutputs(int timeout)
{
  int transactionId;

  if(pvIos == 0){
    return -1;
  }
  // Send query
  transactionId = readAnalogOutputs();
  if(transactionId < 0){
    setStateFromPortError(transactionId);
    return transactionId;
  }
  // Wait on result if needed
  if(timeout > 0){
    addTransaction(transactionId);
    if(!waitTransactionDone(transactionId, timeout)){
      return -1;
    }
  }

  return transactionId;
}

int mdtDevice::setAnalogOutputValue(int address, QVariant value, int timeout)
{
  int transactionId;
  mdtAnalogIo *ao;

  if(pvIos == 0){
    return -1;
  }
  // Get I/O object
  ao = pvIos->analogOutputAt(address);
  if(ao == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog output assigned to address " + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return -1;
  }
  // Check value and set it to I/O
  if(value.type() == QVariant::Int){
    ao->setValueInt(value.toInt(), true, false);
  }else if(value.type() == QVariant::Double){
    ao->setValue(value.toDouble(), true, false);
  }else{
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": invalid value for address " + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return -1;
  }
  if(timeout < 0){
    return 0;
  }
  // Send query
  transactionId = writeAnalogOutput(address, ao->valueInt());
  if(transactionId < 0){
    setStateFromPortError(transactionId);
    return -1;
  }
  // Wait on result if needed
  if(timeout > 0){
    ao->setEnabled(false);
    addTransaction(transactionId, ao);
    if(!waitTransactionDone(transactionId, timeout)){
      ao->setEnabled(true);
      return -1;
    }
    ao->setEnabled(true);
  }

  return transactionId;
}

int mdtDevice::setAnalogOutputs(int timeout)
{
  int transactionId;

  if(pvIos == 0){
    return -1;
  }
  // Send query
  transactionId = writeAnalogOutputs();
  if(transactionId < 0){
    setStateFromPortError(transactionId);
    return transactionId;
  }
  // Wait on result if needed
  if(timeout > 0){
    addTransaction(transactionId);
    if(!waitTransactionDone(transactionId, timeout)){
      return -1;
    }
  }

  return transactionId;
}

QVariant mdtDevice::getDigitalInputState(int address, int timeout)
{
  int transactionId;
  mdtDigitalIo *di;

  if(pvIos == 0){
    return QVariant();
  }
  // Check if only cached state is requested
  if(timeout < 0){
    di = pvIos->digitalInputAt(address);
    if(di == 0){
      mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital input assigned to address " + QString::number(address), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDevice");
      e.commit();
      return QVariant();
    }
    return QVariant(di->isOn());
  }
  // Send query
  transactionId = readDigitalInput(address);
  if(transactionId < 0){
    setStateFromPortError(transactionId);
    return QVariant();
  }
  // Wait on result if needed
  if(timeout > 0){
    di = pvIos->digitalInputAt(address);
    if(di == 0){
      mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital input assigned to address " + QString::number(address), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDevice");
      e.commit();
      return QVariant();
    }
    addTransaction(transactionId, di);
    if(!waitTransactionDone(transactionId, timeout)){
      return QVariant();
    }
    // Return value
    return QVariant(di->isOn());
  }

  return QVariant();
}

int mdtDevice::getDigitalInputs(int timeout)
{
  int transactionId;

  if(pvIos == 0){
    return -1;
  }
  // Send query
  transactionId = readDigitalInputs();
  if(transactionId < 0){
    setStateFromPortError(transactionId);
    return transactionId;
  }
  // Wait on result if needed
  if(timeout > 0){
    addTransaction(transactionId);
    if(!waitTransactionDone(transactionId, timeout)){
      return -1;
    }
  }

  return transactionId;
}

QVariant mdtDevice::getDigitalOutputState(int address, int timeout)
{
  int transactionId;
  mdtDigitalIo *dout;

  if(pvIos == 0){
    return QVariant();
  }
  // Check if only cached state is requested
  if(timeout < 0){
    dout = pvIos->digitalOutputAt(address);
    if(dout == 0){
      mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital output assigned to address " + QString::number(address), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDevice");
      e.commit();
      return QVariant();
    }
    return QVariant(dout->isOn());
  }
  // Send query
  transactionId = readDigitalOutput(address);
  if(transactionId < 0){
    setStateFromPortError(transactionId);
    return QVariant();
  }
  // Wait on result if needed
  if(timeout > 0){
    dout = pvIos->digitalOutputAt(address);
    if(dout == 0){
      mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital output assigned to address " + QString::number(address), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDevice");
      e.commit();
      return QVariant();
    }
    addTransaction(transactionId, dout);
    if(!waitTransactionDone(transactionId, timeout)){
      return QVariant();
    }
    // Return value
    return QVariant(dout->isOn());
  }

  return QVariant();
}

int mdtDevice::getDigitalOutputs(int timeout)
{
  int transactionId;

  if(pvIos == 0){
    return -1;
  }
  // Send query
  transactionId = readDigitalOutputs();
  if(transactionId < 0){
    setStateFromPortError(transactionId);
    return transactionId;
  }
  // Wait on result if needed
  if(timeout > 0){
    addTransaction(transactionId);
    if(!waitTransactionDone(transactionId, timeout)){
      return -1;
    }
  }

  return transactionId;
}

int mdtDevice::setDigitalOutputState(int address, bool state, int timeout)
{
  int transactionId;
  mdtDigitalIo *dout;

  if(pvIos == 0){
    return -1;
  }
  // Get I/O object
  dout = pvIos->digitalOutputAt(address);
  if(dout == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital output assigned to address " + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return -1;
  }
  // Check if query must be sent
  if(timeout < 0){
    return 0;
  }
  // Send query
  transactionId = writeDigitalOutput(address, state);
  if(transactionId < 0){
    setStateFromPortError(transactionId);
    return -1;
  }
  // Wait on result if needed
  if(timeout > 0){
    dout->setEnabled(false);
    addTransaction(transactionId, dout);
    if(!waitTransactionDone(transactionId, timeout)){
      dout->setEnabled(true);
      return -1;
    }
    dout->setEnabled(true);
  }

  return transactionId;
}

int mdtDevice::setDigitalOutputs(int timeout)
{
  int transactionId;

  if(pvIos == 0){
    return -1;
  }
  // Send query
  transactionId = writeDigitalOutputs();
  if(transactionId < 0){
    setStateFromPortError(transactionId);
    return transactionId;
  }
  // Wait on result if needed
  if(timeout > 0){
    addTransaction(transactionId);
    if(!waitTransactionDone(transactionId, timeout)){
      return -1;
    }
  }

  return transactionId;
}

mdtDevice::state_t mdtDevice::state() const
{
  return pvCurrentState;
}

void mdtDevice::setAnalogOutputValue(int address, int value)
{
  if(pvCurrentState != Ready){
    // Device busy, cannot threat query , try later
    return;
  }
  if(setAnalogOutputValue(address, value, pvOutputWriteReplyTimeout) < 0){
    setStateBusy(500);
  }
}

void mdtDevice::setDigitalOutputState(int address, bool state)
{
  if(pvCurrentState != Ready){
    // Device busy, cannot threat query , try later
    return;
  }
  if(setDigitalOutputState(address, state, pvOutputWriteReplyTimeout) < 0){
    setStateBusy(500);
  }
}

void mdtDevice::decodeReadenFrame(int, QByteArray)
{
}

void mdtDevice::runQueries()
{
  if(pvCurrentState != Ready){
    return;
  }

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

int mdtDevice::readAnalogInput(int address)
{
  return -1;
}

int mdtDevice::readAnalogInputs()
{
  return -1;
}

int mdtDevice::readAnalogOutput(int address)
{
  return -1;
}

int mdtDevice::readAnalogOutputs()
{
  return -1;
}

int mdtDevice::writeAnalogOutput(int address, int value)
{
  return -1;
}

int mdtDevice::writeAnalogOutputs()
{
  return -1;
}

int mdtDevice::readDigitalInput(int address)
{
  return -1;
}

int mdtDevice::readDigitalInputs()
{
  return -1;
}

int mdtDevice::readDigitalOutput(int address)
{
  return -1;
}

int mdtDevice::readDigitalOutputs()
{
  return -1;
}

int mdtDevice::writeDigitalOutput(int address, bool state)
{
  return -1;
}

int mdtDevice::writeDigitalOutputs()
{
  return -1;
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
  // Check that queue has not to many queries
  if(pvPendingAioTransactions.size() > 20){
    setStateBusy(500);
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": pending transactions queue has more than 20 items, will be cleared", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    pvPendingAioTransactions.clear();
    return;
  }
  pvPendingAioTransactions.insert(id, io);
}

void mdtDevice::addTransaction(int id, mdtDigitalIo *io)
{
  // Check that queue has not to many queries
  if(pvPendingDioTransactions.size() > 20){
    setStateBusy(500);
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": pending transactions queue has more than 20 items, will be cleared", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    pvPendingDioTransactions.clear();
    return;
  }
  pvPendingDioTransactions.insert(id, io);
}

void mdtDevice::addTransaction(int id)
{
  // Check that queue has not to many queries
  if(pvPendingIoTransactions.size() > 20){
    setStateBusy(500);
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": pending transactions queue has more than 20 items, will be cleared", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    pvPendingIoTransactions.clear();
    return;
  }
  pvPendingIoTransactions.append(id);
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

int mdtDevice::pendingIoTransaction(int id)
{
  int index;

  index = pvPendingIoTransactions.indexOf(id);
  if(index < 0){
    return -1;
  }
  return pvPendingIoTransactions.takeAt(index);
}

bool mdtDevice::waitTransactionDone(int id, int timeout, int granularity)
{
  Q_ASSERT(granularity > 0);

  int index;
  int i = timeout / granularity;

  // Select correct queue
  if(pvPendingAioTransactions.contains(id)){
    while(pvPendingAioTransactions.contains(id)){
      // Check timeout
      if(i <= 0){
        pvPendingAioTransactions.remove(id);
        setStateBusy(200);
        return false;
      }
      mdtPortManager::wait(granularity, granularity);
      i--;
    }
  }else if(pvPendingDioTransactions.contains(id)){
    while(pvPendingDioTransactions.contains(id)){
      // Check timeout
      if(i <= 0){
        pvPendingDioTransactions.remove(id);
        setStateBusy(200);
        return false;
      }
      mdtPortManager::wait(granularity, granularity);
      i--;
    }
  }else if((index = pvPendingIoTransactions.indexOf(id)) >= 0){
    while(index >= 0){
      index = pvPendingIoTransactions.indexOf(id);
      // Check timeout
      if(i <= 0){
        pvPendingIoTransactions.removeAt(index);
        setStateBusy(200);
        return false;
      }
      mdtPortManager::wait(granularity, granularity);
      i--;
    }
  }else{
    // Add a error
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": transaction id " + QString::number(id) + " not found", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return false;
  }

  return true;
}
