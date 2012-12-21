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
  qDebug() << "mdtDevice::mdtDevice() ...";
  pvIos = 0;
  pvDigitalOutputAddressOffset = 0;
  pvAnalogOutputAddressOffset = 0;
  pvBackToReadyStateTimeout = -1;
  pvBackToReadyStateTimer = new QTimer(this);
  Q_ASSERT(pvBackToReadyStateTimer != 0);
  pvBackToReadyStateTimer->setSingleShot(true);
  pvCurrentState = Unknown;
  setName(tr("Unknown"));
  pvAutoQueryEnabled = false;
  pvQueryTimer = new QTimer(this);
  Q_ASSERT(pvQueryTimer != 0);
  setStateDisconnected();
  connect(pvBackToReadyStateTimer, SIGNAL(timeout()), this, SLOT(setStateReady()));
  connect(pvQueryTimer, SIGNAL(timeout()), this, SLOT(runQueries()));
  qDebug() << "mdtDevice::mdtDevice() DONE";
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

mdtAbstractPort::error_t mdtDevice::connectToDevice(const mdtDeviceInfo &devInfo)
{
  return mdtAbstractPort::UnhandledError;
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
      connect(analogOutputs.at(i), SIGNAL(valueChanged(int)), this, SLOT(setAnalogOutputValue(int)));
    }
    digitalOutputs = pvIos->digitalOutputs();
    for(i=0; i<digitalOutputs.size(); i++){
      Q_ASSERT(digitalOutputs.at(i) != 0);
      connect(digitalOutputs.at(i), SIGNAL(stateChanged(int)), this, SLOT(setDigitalOutputState(int)));
    }
  }
}

void mdtDevice::setBackToReadyStateTimeout(int timeout)
{
  pvBackToReadyStateTimeout = timeout;
}

void mdtDevice::setAnalogOutputAddressOffset(int offset)
{
  pvAnalogOutputAddressOffset = offset;
}

void mdtDevice::setDigitalOutputAddressOffset(int offset)
{
  pvDigitalOutputAddressOffset = offset;
}

mdtPortManager *mdtDevice::portManager()
{
  return 0;
}

void mdtDevice::start(int queryInterval)
{
  pvQueryTimer->setInterval(queryInterval);
  pvAutoQueryEnabled = true;
  pvQueryTimer->start();
}

void mdtDevice::stop()
{
  pvAutoQueryEnabled = false;
  pvQueryTimer->stop();
}

QVariant mdtDevice::getAnalogInputValue(int address, int timeout, bool realValue)
{
  int transactionId;
  mdtAnalogIo *ai;
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return QVariant();
  }
  // Get internal I/O object
  ai = pvIos->analogInputAt(address);
  if(ai == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog input assigned to address " + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return QVariant();
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIo(ai, true);
  transaction->setAddress(address);
  // Check if only cached value is requested
  if(timeout < 0){
    // Return value
    if(realValue){
      return QVariant(ai->value());
    }else{
      return QVariant(ai->valueInt());
    }
  }
  // Send query
  if(timeout == 0){
    transaction->setQueryReplyMode(false);
    transactionId = readAnalogInput(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return QVariant();
    }
  }else{
    transaction->setQueryReplyMode(true);
    transactionId = readAnalogInput(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return QVariant();
    }
    // Wait on result
    if(!waitTransactionDone(transactionId, timeout)){
      return QVariant();
    }
    // Return value
    if(realValue){
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
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return -1;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setForMultipleIos(true);
  // Send query and wait if requested
  if(timeout <= 0){
    transaction->setQueryReplyMode(false);
    transactionId = readAnalogInputs(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return transactionId;
    }
  }else{
    transaction->setQueryReplyMode(true);
    transactionId = readAnalogInputs(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return transactionId;
    }
    if(!waitTransactionDone(transactionId, timeout)){
      return -1;
    }
  }

  return transactionId;
}

QVariant mdtDevice::getAnalogOutputValue(int address, int timeout, bool realValue)
{
  int transactionId;
  mdtAnalogIo *ao;
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return QVariant();
  }
  // Get internal I/O object
  ao = pvIos->analogOutputAt(address);
  if(ao == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog input assigned to address " + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return QVariant();
  }
  // Check if only cached value is requested
  if(timeout < 0){
    // Return value
    if(realValue){
      return QVariant(ao->value());
    }else{
      return QVariant(ao->valueInt());
    }
  }
  // Get a transaction
  transaction = getNewTransaction();
  transaction->setIo(ao, false);
  transaction->setAddress(address);
  // Send query and wait if requested
  if(timeout == 0){
    transaction->setQueryReplyMode(false);
    transactionId = readAnalogOutput(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return QVariant();
    }
  }else{
    transaction->setQueryReplyMode(true);
    transactionId = readAnalogOutput(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return QVariant();
    }
    if(!waitTransactionDone(transactionId, timeout)){
      return QVariant();
    }
    // Return value
    if(realValue){
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
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return -1;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setForMultipleIos(true);

  // Send query and wait if requested
  if(timeout <= 0){
    transaction->setQueryReplyMode(false);
    transactionId = readAnalogOutputs(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return transactionId;
    }
  }else{
    transaction->setQueryReplyMode(true);
    transactionId = readAnalogOutputs(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return transactionId;
    }
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
  mdtPortTransaction *transaction;

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
  // Disable I/O - Must be re-enabled by subclass once data are in
  ao->setEnabled(false);
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIo(ao, false);
  transaction->setAddress(address);
  // Send query and wait if requested
  if(timeout == 0){
    transaction->setQueryReplyMode(false);
    transactionId = writeAnalogOutput(ao->valueInt(), transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return -1;
    }
  }else{
    transaction->setQueryReplyMode(true);
    transactionId = writeAnalogOutput(ao->valueInt(), transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return -1;
    }
    if(!waitTransactionDone(transactionId, timeout)){
      return -1;
    }
  }

  return transactionId;
}

int mdtDevice::setAnalogOutputs(int timeout)
{
  int transactionId;
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return -1;
  }
  // Disable I/Os - Must be re-enabled by subclass once data are in
  pvIos->setAnalogOutputsEnabled(false);
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setForMultipleIos(true);
  // Send query and wait if requested
  if(timeout <= 0){
    transaction->setQueryReplyMode(false);
    transactionId = writeAnalogOutputs(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return transactionId;
    }
  }else{
    transaction->setQueryReplyMode(true);
    transactionId = writeAnalogOutputs(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return transactionId;
    }
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
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return QVariant();
  }
  // Get I/O object
  di = pvIos->digitalInputAt(address);
  if(di == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital input assigned to address " + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return QVariant();
  }
  // Check if only cached state is requested
  if(timeout < 0){
    return QVariant(di->isOn());
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIo(di, true);
  transaction->setAddress(address);
  // Send query and wait if requested
  if(timeout == 0){
    transaction->setQueryReplyMode(false);
    transactionId = readDigitalInput(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return QVariant();
    }
  }else{
    transaction->setQueryReplyMode(true);
    transactionId = readDigitalInput(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return QVariant();
    }
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
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return -1;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setForMultipleIos(true);
  // Send query and wait if requested
  if(timeout >= 0){
    transaction->setQueryReplyMode(false);
    transactionId = readDigitalInputs(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return transactionId;
    }
  }else{
    transaction->setQueryReplyMode(true);
    transactionId = readDigitalInputs(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return transactionId;
    }
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
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return QVariant();
  }
  // Get I/O object
  dout = pvIos->digitalOutputAt(address);
  if(dout == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital output assigned to address " + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return QVariant();
  }
  // Check if only cached state is requested
  if(timeout < 0){
    return QVariant(dout->isOn());
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIo(dout, false);
  transaction->setAddress(address);
  // Send query and wait on result if requested
  if(timeout == 0){
    transaction->setQueryReplyMode(false);
    transactionId = readDigitalOutput(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return QVariant();
    }
  }else{
    transaction->setQueryReplyMode(true);
    transactionId = readDigitalOutput(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return QVariant();
    }
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
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return -1;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setForMultipleIos(true);
  // Send query and wait if requested
  if(timeout <= 0){
    transaction->setQueryReplyMode(false);
    transactionId = readDigitalOutputs(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return transactionId;
    }
  }else{
    transaction->setQueryReplyMode(true);
    transactionId = readDigitalOutputs(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return transactionId;
    }
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
  mdtPortTransaction *transaction;

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
  ///qDebug() << "mdtDevice::setDigitalOutputState(): Out " << address << " : " << dout->isOn();
  dout->setOn(QVariant(state), false);
  ///qDebug() << "mdtDevice::setDigitalOutputState(): Out " << address << " : " << dout->isOn();
  // Check if query must be sent
  if(timeout < 0){
    return 0;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIo(dout, false);
  transaction->setAddress(address);
  // Disable output - subclass must enable it again wehen data comes in
  dout->setEnabled(false);
  // Send query and wait if requested
  if(timeout == 0){
    transaction->setQueryReplyMode(false);
    transactionId = writeDigitalOutput(state, transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return -1;
    }
  }else{
    transaction->setQueryReplyMode(true);
    transactionId = writeDigitalOutput(state, transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return -1;
    }
    if(!waitTransactionDone(transactionId, timeout)){
      return -1;
    }
  }

  return transactionId;
}

int mdtDevice::setDigitalOutputs(int timeout)
{
  int transactionId;
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return -1;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setForMultipleIos(true);
  // Send query and wait if requested
  if(timeout <= 0){
    transaction->setQueryReplyMode(false);
    transactionId = writeDigitalOutputs(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return transactionId;
    }
  }else{
    transaction->setQueryReplyMode(true);
    transactionId = writeDigitalOutputs(transaction);
    if(transactionId < 0){
      setStateFromPortError(transactionId);
      return transactionId;
    }
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

void mdtDevice::setAnalogOutputValue(int address)
{
  mdtAnalogIo *ao;

  if(pvIos == 0){
    return;
  }
  if(pvCurrentState != Ready){
    // Device busy, cannot threat query , try later
    return;
  }
  // Get I/O object
  ao = pvIos->analogOutputAt(address);
  if(ao == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog output assigned to address " + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return;
  }
  // Send query
  setAnalogOutputValue(address, ao->valueInt(), 0);
}

void mdtDevice::setDigitalOutputState(int address)
{
  mdtDigitalIo *dout;

  if(pvIos == 0){
    return;
  }
  if(pvCurrentState != Ready){
    // Device busy, cannot threat query , try later
    return;
  }
  // Get I/O object
  dout = pvIos->digitalOutputAt(address);
  if(dout == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital output assigned to address " + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return;
  }
  // Send query
  setDigitalOutputState(address, dout->isOn(), 0);
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

void mdtDevice::setStateFromPortError(int error)
{
  if(error == mdtAbstractPort::NoError){
    qDebug() << "mdtDevice::setStateFromPortError(): NoError";
    setStateReady();
  }else if(error == mdtAbstractPort::Disconnected){
    qDebug() << "mdtDevice::setStateFromPortError(): Disconnected";
    setStateDisconnected();
  }else if(error == mdtAbstractPort::Connecting){
    qDebug() << "mdtDevice::setStateFromPortError(): Connecting";
    setStateConnecting();
  }else if(error == mdtAbstractPort::WritePoolEmpty){
    qDebug() << "mdtDevice::setStateFromPortError(): WriteQueueEmpty";
    setStateBusy(pvBackToReadyStateTimeout);
  }else{
    qDebug() << "mdtDevice::setStateFromPortError(): ????";
    setStateUnknown();
  }
}

void mdtDevice::setStateReady()
{
  if(pvCurrentState == Ready){
    return;
  }
  // Check if we have to restart query timer
  if(pvAutoQueryEnabled){
    pvQueryTimer->start();
  }
  pvCurrentState = Ready;
  qDebug() << "mdtDevice: new state is Ready";
  emit(stateChanged(pvCurrentState));
}

void mdtDevice::decodeReadenFrame(int, QByteArray)
{
}

int mdtDevice::readAnalogInput(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtDevice::readAnalogInputs(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtDevice::readAnalogOutput(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtDevice::readAnalogOutputs(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtDevice::writeAnalogOutput(int value, mdtPortTransaction *transaction)
{
  return -1;
}

int mdtDevice::writeAnalogOutputs(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtDevice::readDigitalInput(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtDevice::readDigitalInputs(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtDevice::readDigitalOutput(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtDevice::readDigitalOutputs(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtDevice::writeDigitalOutput(bool state, mdtPortTransaction *transaction)
{
  return -1;
}

int mdtDevice::writeDigitalOutputs(mdtPortTransaction *transaction)
{
  return -1;
}

void mdtDevice::setStateDisconnected()
{
  if(pvCurrentState == Disconnected){
    return;
  }
  // Stop auto queries if running
  if(pvAutoQueryEnabled){
    pvQueryTimer->stop();
  }
  pvCurrentState = Disconnected;
  qDebug() << "mdtDevice: new state is Disconnected";
  emit(stateChanged(pvCurrentState));
}

void mdtDevice::setStateConnecting()
{
  if(pvCurrentState == Connecting){
    return;
  }
  // Stop auto queries if running
  if(pvAutoQueryEnabled){
    pvQueryTimer->stop();
  }
  // Thread will notify the ready (or disconnected) state, cancel retry timer
  pvBackToReadyStateTimer->stop();
  pvCurrentState = Connecting;
  qDebug() << "mdtDevice: new state is Connecting";
  emit(stateChanged(pvCurrentState));
}

void mdtDevice::setStateBusy(int retryTimeout)
{
  if(pvCurrentState == Busy){
    return;
  }
  // Stop auto queries if running
  if(pvAutoQueryEnabled){
    pvQueryTimer->stop();
  }
  pvCurrentState = Busy;
  qDebug() << "mdtDevice: new state is Busy";
  emit(stateChanged(pvCurrentState));
  // Set state ready if requested
  if(retryTimeout >= 0){
    pvBackToReadyStateTimer->start(retryTimeout);
    ///QTimer::singleShot(retryTimeout, this, SLOT(setStateReady()));
  }
}

void mdtDevice::setStateUnknown()
{
  if(pvCurrentState == Unknown){
    return;
  }
  // Stop auto queries if running
  if(pvAutoQueryEnabled){
    pvQueryTimer->stop();
  }
  pvCurrentState = Unknown;
  // Add a error
  mdtError e(MDT_DEVICE_ERROR, "Device " + name() + " goes to unknown state", mdtError::Error);
  MDT_ERROR_SET_SRC(e, "mdtDevice");
  e.commit();
  qDebug() << "mdtDevice: new state is Unknown";
  emit(stateChanged(pvCurrentState));
}

mdtPortTransaction *mdtDevice::getNewTransaction()
{
  Q_ASSERT(portManager() != 0);

  return portManager()->getNewTransaction();
}

void mdtDevice::addTransaction(int id, mdtAnalogIo *io)
{
  // Check that queue has not to many queries
  if(pvPendingAioTransactions.size() > 20){
    setStateBusy(pvBackToReadyStateTimeout);
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
    setStateBusy(pvBackToReadyStateTimeout);
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
    setStateBusy(pvBackToReadyStateTimeout);
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
  Q_ASSERT(portManager() != 0);

  bool ok;

  qDebug() << "mdtDevice::waitTransactionDone(), ID " << id;
  ok = portManager()->waitOnFrame(id, timeout, granularity);
  // We must remove frame from done queue
  qDebug() << "mdtDevice::waitTransactionDone(), ID " << id << " done..";
  portManager()->readenFrame(id);

  return ok;
}
/**
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
        qDebug() << "waitTransactionDone/AIO: timeout";
        ///setStateBusy(pvBackToReadyStateTimeout);
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
        qDebug() << "waitTransactionDone/DIO: timeout";
        ///setStateBusy(pvBackToReadyStateTimeout);
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
        qDebug() << "waitTransactionDone/IO: timeout";
        ///setStateBusy(pvBackToReadyStateTimeout);
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
*/
