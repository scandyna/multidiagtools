/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
  pvBackToReadyStateTimeout = -1;
  pvBackToReadyStateTimer = new QTimer(this);
  pvBackToReadyStateTimer->setSingleShot(true);
  setName(tr("Unknown"));
  pvAutoQueryEnabled = false;
  pvQueryTimer = new QTimer(this);
  setStateDisconnected();
  ///connect(pvBackToReadyStateTimer, SIGNAL(timeout()), this, SLOT(setStateReady()));
  ///connect(pvBackToReadyStateTimer, SIGNAL(timeout()), this, SIGNAL(deviceReady()));
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

  // Clear prevous I/Os
  if(pvIos != 0){
    analogOutputs = pvIos->analogOutputs();
    for(i=0; i<analogOutputs.size(); i++){
      Q_ASSERT(analogOutputs.at(i) != 0);
      disconnect(analogOutputs.at(i), SIGNAL(valueChanged(mdtAnalogIo*)), this, SLOT(setAnalogOutputValue(mdtAnalogIo*)));
    }
    digitalOutputs = pvIos->digitalOutputs();
    for(i=0; i<digitalOutputs.size(); i++){
      Q_ASSERT(digitalOutputs.at(i) != 0);
      disconnect(digitalOutputs.at(i), SIGNAL(valueChanged(mdtDigitalIo*)), this, SLOT(setDigitalOutputValue(mdtDigitalIo*)));
    }
    if(pvIos != ios){
      pvIos->deleteIos();
      delete pvIos;
    }
  }
  // Set new I/Os
  pvIos = ios;
  if(autoOutputUpdate){
    analogOutputs = pvIos->analogOutputs();
    for(i=0; i<analogOutputs.size(); i++){
      Q_ASSERT(analogOutputs.at(i) != 0);
      connect(analogOutputs.at(i), SIGNAL(valueChanged(mdtAnalogIo*)), this, SLOT(setAnalogOutputValue(mdtAnalogIo*)));
    }
    digitalOutputs = pvIos->digitalOutputs();
    for(i=0; i<digitalOutputs.size(); i++){
      Q_ASSERT(digitalOutputs.at(i) != 0);
      connect(digitalOutputs.at(i), SIGNAL(valueChanged(mdtDigitalIo*)), this, SLOT(setDigitalOutputValue(mdtDigitalIo*)));
    }
  }
}

void mdtDevice::setBackToReadyStateTimeout(int timeout)
{
  pvBackToReadyStateTimeout = timeout;
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

mdtValue mdtDevice::getAnalogInputValue(mdtAnalogIo *analogInput, bool queryDevice, bool waitOnReply)
{
  Q_ASSERT(analogInput != 0);

  int transactionId;
  mdtPortTransaction *transaction;

  // Check if only cached value is requested
  if(!queryDevice){
    return analogInput->value();
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIo(analogInput, true);
  transaction->setAddress(analogInput->addressRead());
  // Send query
  if(waitOnReply){
    transaction->setQueryReplyMode(true);
    transactionId = readAnalogInput(transaction);
    if(transactionId < 0){
      analogInput->setValue(mdtValue());
      return mdtValue();
    }
    // Wait on result (use device's defined timeout)
    if(!waitTransactionDone(transactionId)){
      analogInput->setValue(mdtValue());
      return mdtValue();
    }
    // Return value
    return analogInput->value();
  }else{
    transaction->setQueryReplyMode(false);
    transactionId = readAnalogInput(transaction);
    if(transactionId < 0){
      analogInput->setValue(mdtValue());
      return mdtValue();
    }
  }

  return mdtValue();
}

mdtValue mdtDevice::getAnalogInputValue(int address, bool queryDevice, bool waitOnReply)
{
  mdtAnalogIo *ai;

  if(pvIos == 0){
    return mdtValue();
  }
  // Get internal I/O object
  ai = pvIos->analogInputAt(address);
  if(ai == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog input assigned to address " + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return mdtValue();
  }

  return getAnalogInputValue(ai, queryDevice, waitOnReply);
}

mdtValue mdtDevice::getAnalogInputValue(const QString &labelShort, bool queryDevice, bool waitOnReply)
{
  mdtAnalogIo *ai;

  if(pvIos == 0){
    return mdtValue();
  }
  // Get internal I/O object
  ai = pvIos->analogInputWithLabelShort(labelShort);
  if(ai == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog input with label short " + labelShort, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return mdtValue();
  }

  return getAnalogInputValue(ai, queryDevice, waitOnReply);

}

int mdtDevice::getAnalogInputs(bool waitOnReply)
{
  int transactionId;
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return -1;
  }
  if(pvIos->analogInputsCount() < 1){
    return 0;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIoCount(pvIos->analogInputsCount());
  transaction->setAddress(pvIos->analogInputsFirstAddress());
  // Send query and wait if requested
  if(waitOnReply){
    transaction->setQueryReplyMode(true);
    transactionId = readAnalogInputs(transaction);
    if(transactionId < 0){
      pvIos->setAnalogInputsValue(mdtValue());
      return transactionId;
    }
    // Wait on result (use device's defined timeout)
    if(!waitTransactionDone(transactionId)){
      pvIos->setAnalogInputsValue(mdtValue());
      return -1;
    }
  }else{
    transaction->setQueryReplyMode(false);
    transactionId = readAnalogInputs(transaction);
    if(transactionId < 0){
      pvIos->setAnalogInputsValue(mdtValue());
      return transactionId;
    }
  }

  return transactionId;
}

mdtValue mdtDevice::getAnalogOutputValue(mdtAnalogIo *analogOutput, bool queryDevice, bool waitOnReply)
{
  Q_ASSERT(analogOutput != 0);

  int transactionId;
  mdtPortTransaction *transaction;

  // Check if only cached value is requested
  if(!queryDevice){
    return analogOutput->value();
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIo(analogOutput, true);
  transaction->setAddress(analogOutput->addressRead());
  // Send query
  if(waitOnReply){
    transaction->setQueryReplyMode(true);
    transactionId = readAnalogOutput(transaction);
    if(transactionId < 0){
      analogOutput->setValue(mdtValue());
      return mdtValue();
    }
    // Wait on result (use device's defined timeout)
    if(!waitTransactionDone(transactionId)){
      analogOutput->setValue(mdtValue());
      return mdtValue();
    }
    // Return value
    return analogOutput->value();
  }else{
    transaction->setQueryReplyMode(false);
    transactionId = readAnalogOutput(transaction);
    if(transactionId < 0){
      analogOutput->setValue(mdtValue());
      return mdtValue();
    }
  }

  return mdtValue();
}

mdtValue mdtDevice::getAnalogOutputValue(int addressRead, bool queryDevice, bool waitOnReply)
{
  mdtAnalogIo *ao;

  if(pvIos == 0){
    return mdtValue();
  }
  // Get internal I/O object
  ao = pvIos->analogOutputAtAddressRead(addressRead);
  if(ao == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog output assigned to address " + QString::number(addressRead), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return mdtValue();
  }

  return getAnalogOutputValue(ao, queryDevice, waitOnReply);
}

mdtValue mdtDevice::getAnalogOutputValue(const QString &labelShort, bool queryDevice, bool waitOnReply)
{
  mdtAnalogIo *ao;

  if(pvIos == 0){
    return mdtValue();
  }
  // Get internal I/O object
  ao = pvIos->analogOutputWithLabelShort(labelShort);
  if(ao == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog output assigned with label " + labelShort, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return mdtValue();
  }

  return getAnalogOutputValue(ao, queryDevice, waitOnReply);
}

int mdtDevice::getAnalogOutputs(bool waitOnReply)
{
  int transactionId;
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return -1;
  }
  if(pvIos->analogOutputsCount() < 1){
    return 0;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIoCount(pvIos->analogOutputsCount());
  transaction->setAddress(pvIos->analogOutputsFirstAddressRead());
  // Send query and wait if requested
  if(waitOnReply){
    transaction->setQueryReplyMode(true);
    transactionId = readAnalogOutputs(transaction);
    if(transactionId < 0){
      pvIos->setAnalogOutputsValue(mdtValue());
      return transactionId;
    }
    // Wait on result (use device's defined timeout)
    if(!waitTransactionDone(transactionId)){
      pvIos->setAnalogOutputsValue(mdtValue());
      return -1;
    }
  }else{
    transaction->setQueryReplyMode(false);
    transactionId = readAnalogOutputs(transaction);
    if(transactionId < 0){
      pvIos->setAnalogOutputsValue(mdtValue());
      return transactionId;
    }
  }

  return transactionId;
}

int mdtDevice::setAnalogOutputValue(mdtAnalogIo *analogOutput, const mdtValue &value, bool sendToDevice, bool waitOnReply)
{
  Q_ASSERT(analogOutput != 0);

  int transactionId;
  mdtPortTransaction *transaction;

  // Store value
  analogOutput->setValue(value, false);
  if(!sendToDevice){
    return 0;
  }
  // Disable I/O - Must be re-enabled by subclass once data are available or timeout
  analogOutput->setEnabled(false);
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIo(analogOutput, false);
  transaction->setAddress(analogOutput->addressWrite());
  // Send query and wait if requested
  if(waitOnReply){
    transaction->setQueryReplyMode(true);
    transactionId = writeAnalogOutput(analogOutput->value().valueInt(), transaction);
    if(transactionId < 0){
      analogOutput->setValue(mdtValue());
      return -1;
    }
    // Wait on result (use device's defined timeout)
    if(!waitTransactionDone(transactionId)){
      analogOutput->setValue(mdtValue());
      return -1;
    }
  }else{
    transaction->setQueryReplyMode(false);
    transactionId = writeAnalogOutput(analogOutput->value().valueInt(), transaction);
    if(transactionId < 0){
      analogOutput->setValue(mdtValue());
      return -1;
    }
  }

  return transactionId;
}

int mdtDevice::setAnalogOutputValue(int addressWrite, const mdtValue &value, bool sendToDevice, bool waitOnReply)
{
  mdtAnalogIo *analogOutput;

  if(pvIos == 0){
    return -1;
  }
  // Get I/O object
  analogOutput = pvIos->analogOutputAtAddressWrite(addressWrite);
  if(analogOutput == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog output assigned to address " + QString::number(addressWrite), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return -1;
  }

  return setAnalogOutputValue(analogOutput, value, sendToDevice, waitOnReply);
}

int mdtDevice::setAnalogOutputValue(const QString &labelShort, const mdtValue &value, bool sendToDevice, bool waitOnReply)
{
  mdtAnalogIo *analogOutput;

  if(pvIos == 0){
    return -1;
  }
  // Get I/O object
  analogOutput = pvIos->analogOutputWithLabelShort(labelShort);
  if(analogOutput == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog output found with label short " + labelShort, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return -1;
  }

  return setAnalogOutputValue(analogOutput, value, sendToDevice, waitOnReply);
}

int mdtDevice::setAnalogOutputs(bool waitOnReply)
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
  transaction->setIoCount(pvIos->analogOutputsCount());
  transaction->setAddress(pvIos->analogOutputsFirstAddressWrite());
  // Send query and wait if requested
  if(waitOnReply){
    transaction->setQueryReplyMode(true);
    transactionId = writeAnalogOutputs(transaction);
    if(transactionId < 0){
      pvIos->setAnalogOutputsValue(mdtValue());
      return transactionId;
    }
    // Wait on result (use device's defined timeout)
    if(!waitTransactionDone(transactionId)){
      pvIos->setAnalogOutputsValue(mdtValue());
      return -1;
    }
  }else{
    transaction->setQueryReplyMode(false);
    transactionId = writeAnalogOutputs(transaction);
    if(transactionId < 0){
      pvIos->setAnalogOutputsValue(mdtValue());
      return transactionId;
    }
  }

  return transactionId;
}

mdtValue mdtDevice::getDigitalInputValue(mdtDigitalIo *digitalInput, bool queryDevice, bool waitOnReply)
{
  Q_ASSERT(digitalInput != 0);

  int transactionId;
  mdtPortTransaction *transaction;

  // Check if only cached value is requested
  if(!queryDevice){
    return digitalInput->value();
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIo(digitalInput, true);
  transaction->setAddress(digitalInput->addressRead());
  // Send query and wait if requested
  if(waitOnReply){
    transaction->setQueryReplyMode(true);
    transactionId = readDigitalInput(transaction);
    if(transactionId < 0){
      digitalInput->setValue(mdtValue());
      return mdtValue();
    }
    // Wait on result (use device's defined timeout)
    if(!waitTransactionDone(transactionId)){
      digitalInput->setValue(mdtValue());
      return mdtValue();
    }
    // Return value
    return digitalInput->value();
  }else{
    transaction->setQueryReplyMode(false);
    transactionId = readDigitalInput(transaction);
    if(transactionId < 0){
      digitalInput->setValue(mdtValue());
      return mdtValue();
    }
  }

  return mdtValue();
}

mdtValue mdtDevice::getDigitalInputValue(int address, bool queryDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalInput;

  if(pvIos == 0){
    return mdtValue();
  }
  // Get I/O object
  digitalInput = pvIos->digitalInputAt(address);
  if(digitalInput == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital input assigned to address " + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return mdtValue();
  }

  return getDigitalInputValue(digitalInput, queryDevice, waitOnReply);
}

mdtValue mdtDevice::getDigitalInputValue(const QString &labelShort, bool queryDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalInput;

  if(pvIos == 0){
    return mdtValue();
  }
  // Get I/O object
  digitalInput = pvIos->digitalInputWithLabelShort(labelShort);
  if(digitalInput == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital input found with label short " + labelShort, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return mdtValue();
  }

  return getDigitalInputValue(digitalInput, queryDevice, waitOnReply);
}

int mdtDevice::getDigitalInputs(bool waitOnReply)
{
  int transactionId;
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return -1;
  }
  if(pvIos->digitalInputsCount() < 1){
    return 0;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIoCount(pvIos->digitalInputsCount());
  transaction->setAddress(pvIos->digitalInputsFirstAddress());
  // Send query and wait if requested
  if(waitOnReply){
    transaction->setQueryReplyMode(true);
    transactionId = readDigitalInputs(transaction);
    if(transactionId < 0){
      pvIos->setDigitalInputsValue(mdtValue());
      return transactionId;
    }
    // Wait on result (use device's defined timeout)
    if(!waitTransactionDone(transactionId)){
      pvIos->setDigitalInputsValue(mdtValue());
      return -1;
    }
  }else{
    transaction->setQueryReplyMode(false);
    transactionId = readDigitalInputs(transaction);
    if(transactionId < 0){
      pvIos->setDigitalInputsValue(mdtValue());
      return transactionId;
    }
  }

  return transactionId;
}

mdtValue mdtDevice::getDigitalOutputValue(mdtDigitalIo *digitalOutput, bool queryDevice, bool waitOnReply)
{
  Q_ASSERT(digitalOutput != 0);

  int transactionId;
  mdtPortTransaction *transaction;

  // Check if only cached state is requested
  if(!queryDevice){
    return digitalOutput->value();
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIo(digitalOutput, false);
  transaction->setAddress(digitalOutput->addressRead());
  // Send query and wait on result if requested
  if(waitOnReply){
    transaction->setQueryReplyMode(true);
    transactionId = readDigitalOutput(transaction);
    if(transactionId < 0){
      digitalOutput->setValue(mdtValue());
      return mdtValue();
    }
    // Wait on result (use device's defined timeout)
    if(!waitTransactionDone(transactionId)){
      digitalOutput->setValue(mdtValue());
      return mdtValue();
    }
    // Return value
    return digitalOutput->value();
  }else{
    transaction->setQueryReplyMode(false);
    transactionId = readDigitalOutput(transaction);
    if(transactionId < 0){
      digitalOutput->setValue(mdtValue());
      return mdtValue();
    }
  }

  return mdtValue();
}

mdtValue mdtDevice::getDigitalOutputValue(int addressRead, bool queryDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalOutput;

  if(pvIos == 0){
    return mdtValue();
  }
  // Get I/O object
  digitalOutput = pvIos->digitalOutputAtAddressRead(addressRead);
  if(digitalOutput == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital output assigned to address " + QString::number(addressRead), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return mdtValue();
  }

  return getDigitalOutputValue(digitalOutput, queryDevice, waitOnReply);
}

mdtValue mdtDevice::getDigitalOutputValue(const QString &labelShort, bool queryDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalOutput;

  if(pvIos == 0){
    return mdtValue();
  }
  // Get I/O object
  digitalOutput = pvIos->digitalOutputWithLabelShort(labelShort);
  if(digitalOutput == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital output found with label short " + labelShort, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return mdtValue();
  }

  return getDigitalOutputValue(digitalOutput, queryDevice, waitOnReply);
}

int mdtDevice::getDigitalOutputs(bool waitOnReply)
{
  int transactionId;
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return -1;
  }
  if(pvIos->digitalOutputsCount() < 1){
    return 0;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setAddress(pvIos->digitalOutputsFirstAddressRead());
  transaction->setIoCount(pvIos->digitalOutputsCount());
  // Send query and wait if requested
  if(waitOnReply){
    transaction->setQueryReplyMode(true);
    transactionId = readDigitalOutputs(transaction);
    if(transactionId < 0){
      pvIos->setDigitalOutputsValue(mdtValue());
      return transactionId;
    }
    // Wait on result (use device's defined timeout)
    if(!waitTransactionDone(transactionId)){
      pvIos->setDigitalOutputsValue(mdtValue());
      return -1;
    }
  }else{
    transaction->setQueryReplyMode(false);
    transactionId = readDigitalOutputs(transaction);
    if(transactionId < 0){
      pvIos->setDigitalOutputsValue(mdtValue());
      return transactionId;
    }
  }

  return transactionId;
}

int mdtDevice::setDigitalOutputValue(mdtDigitalIo *digitalOutput, const mdtValue &value, bool sendToDevice, bool waitOnReply)
{
  Q_ASSERT(digitalOutput != 0);

  int transactionId;
  mdtPortTransaction *transaction;

  digitalOutput->setValue(value, false);
  if(!sendToDevice){
    return 0;
  }
  // Disable I/O - Must be re-enabled by subclass once data are available or timeout
  digitalOutput->setEnabled(false);
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIo(digitalOutput, false);
  transaction->setAddress(digitalOutput->addressWrite());
  // Send query
  if(waitOnReply){
    transaction->setQueryReplyMode(true);
    transactionId = writeDigitalOutput(value.valueBool(), transaction);
    if(transactionId < 0){
      digitalOutput->setValue(mdtValue());
      return -1;
    }
    // Wait on result (use device's defined timeout)
    if(!waitTransactionDone(transactionId)){
      digitalOutput->setValue(mdtValue());
      return -1;
    }
  }else{
    transaction->setQueryReplyMode(false);
    transactionId = writeDigitalOutput(value.valueBool(), transaction);
    if(transactionId < 0){
      digitalOutput->setValue(mdtValue());
      return -1;
    }
  }

  return transactionId;
}

int mdtDevice::setDigitalOutputValue(int addressWrite, const mdtValue &value, bool sendToDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalOutput;

  if(pvIos == 0){
    return -1;
  }
  // Get I/O object
  digitalOutput = pvIos->digitalOutputAtAddressWrite(addressWrite);
  if(digitalOutput == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital output assigned to address " + QString::number(addressWrite), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return -1;
  }

  return setDigitalOutputValue(digitalOutput, value, sendToDevice, waitOnReply);
}

int mdtDevice::setDigitalOutputValue(const QString &labelShort, const mdtValue &value, bool sendToDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalOutput;

  if(pvIos == 0){
    return -1;
  }
  // Get I/O object
  digitalOutput = pvIos->digitalOutputWithLabelShort(labelShort);
  if(digitalOutput == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital output found with label short " + labelShort, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return -1;
  }

  return setDigitalOutputValue(digitalOutput, value, sendToDevice, waitOnReply);
}

int mdtDevice::setDigitalOutputs(bool waitOnReply)
{
  int transactionId;
  mdtPortTransaction *transaction;

  if(pvIos == 0){
    return -1;
  }
  // Disable I/Os - Must be re-enabled by subclass once data are in
  pvIos->setDigitalOutputsEnabled(false);
  // Get a new transaction
  transaction = getNewTransaction();
  transaction->setIoCount(pvIos->digitalOutputsCount());
  transaction->setAddress(pvIos->digitalOutputsFirstAddressWrite());
  // Send query and wait if requested
  if(waitOnReply){
    transaction->setQueryReplyMode(true);
    transactionId = writeDigitalOutputs(transaction);
    if(transactionId < 0){
      pvIos->setDigitalOutputsValue(mdtValue());
      return transactionId;
    }
    // Wait on result (use device's defined timeout)
    if(!waitTransactionDone(transactionId)){
      pvIos->setDigitalOutputsValue(mdtValue());
      return -1;
    }
  }else{
    transaction->setQueryReplyMode(false);
    transactionId = writeDigitalOutputs(transaction);
    if(transactionId < 0){
      pvIos->setDigitalOutputsValue(mdtValue());
      return transactionId;
    }
  }

  return transactionId;
}

mdtPortManager::state_t mdtDevice::state() const
{
  return pvCurrentState;
}

void mdtDevice::setAnalogOutputValue(mdtAnalogIo* analogOutput)
{
  Q_ASSERT(analogOutput != 0);

  if(pvCurrentState != mdtPortManager::Ready){
    // Device busy, cannot threat query , try later
    return;
  }
  setAnalogOutputValue(analogOutput, analogOutput->value(), true, false);
}

void mdtDevice::setDigitalOutputValue(mdtDigitalIo* digitalOutput)
{
  Q_ASSERT(digitalOutput != 0);

  if(pvCurrentState != mdtPortManager::Ready){
    // Device busy, cannot threat query , try later
    return;
  }
  setDigitalOutputValue(digitalOutput, digitalOutput->value(), true, false);
}

void mdtDevice::runQueries()
{
  if(pvCurrentState != mdtPortManager::Ready){
    return;
  }

  queriesSequence();
}

bool mdtDevice::queriesSequence()
{
  return false;
}

void mdtDevice::decodeReadenFrame(mdtPortTransaction *transaction)
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


mdtPortTransaction *mdtDevice::getNewTransaction()
{
  Q_ASSERT(portManager() != 0);

  return portManager()->getNewTransaction();
}

void mdtDevice::restoreTransaction(mdtPortTransaction *transaction)
{
  Q_ASSERT(portManager() != 0);
  Q_ASSERT(transaction != 0);

  portManager()->restoreTransaction(transaction);
}

bool mdtDevice::waitTransactionDone(int id)
{
  Q_ASSERT(portManager() != 0);

  bool ok;

  ok = portManager()->waitTransactionDone(id);
  /*
   * Request was send, response arrived,
   * subclass has decoded response and updated I/O.
   * So, we have to remove transaction from done queue and restore it to pool
   * In mdtPortManager it was choosen to not let access
   * to transaction queues management, so we call readenFrame()
   * witch does all the needed job.
   */
  portManager()->readenFrame(id);

  return ok;
}

void mdtDevice::setStateFromPortManager(int portManagerState)
{
  switch(portManagerState){
    case mdtPortManager::Disconnected:
      setStateDisconnected();
      break;
    case mdtPortManager::Connecting:
      setStateConnecting();
      break;
    case mdtPortManager::Ready:
      setStateReady();
      break;
    case mdtPortManager::Busy:
      setStateBusy();
      break;
    case mdtPortManager::Warning:
      setStateWarning();
      break;
    case mdtPortManager::Error:
      setStateError();
      break;
    default:
      setStateError();
      emit(statusMessageChanged(tr("Received a unknown state"), "", 0));
  }
}

void mdtDevice::setStateDisconnected()
{
  if(pvCurrentState == mdtPortManager::Disconnected){
    return;
  }
  // Stop auto queries if running
  if(pvAutoQueryEnabled){
    pvQueryTimer->stop();
  }
  // Update I/Os
  if(pvIos != 0){
    pvIos->setAnalogInputsValue(mdtValue());
    pvIos->setAnalogOutputsValue(mdtValue());
    pvIos->setAnalogOutputsEnabled(false);
    pvIos->setDigitalInputsValue(mdtValue());
    pvIos->setDigitalOutputsValue(mdtValue());
    pvIos->setDigitalOutputsEnabled(false);
  }
  pvCurrentState = mdtPortManager::Disconnected;
  qDebug() << "mdtDevice: new state is Disconnected";
  emit(stateChanged(pvCurrentState));
}

void mdtDevice::setStateConnecting(/*const QString &message*/)
{
  if(pvCurrentState == mdtPortManager::Connecting){
    return;
  }
  // Stop auto queries if running
  if(pvAutoQueryEnabled){
    pvQueryTimer->stop();
  }
  // Update I/Os
  if(pvIos != 0){
    pvIos->setAnalogInputsValue(mdtValue());
    pvIos->setAnalogOutputsValue(mdtValue());
    pvIos->setAnalogOutputsEnabled(false);
    pvIos->setDigitalInputsValue(mdtValue());
    pvIos->setDigitalOutputsValue(mdtValue());
    pvIos->setDigitalOutputsEnabled(false);
  }
  // Thread will notify the ready (or disconnected) state, cancel retry timer
  pvBackToReadyStateTimer->stop();
  pvCurrentState = mdtPortManager::Connecting;
  qDebug() << "mdtDevice: new state is Connecting";
  emit(stateChanged(pvCurrentState));
}

void mdtDevice::setStateReady()
{
  if(pvCurrentState == mdtPortManager::Ready){
    return;
  }
  // Check if we have to restart query timer
  if(pvAutoQueryEnabled){
    pvQueryTimer->start();
  }
  // Update I/Os
  if(pvIos != 0){
    getAnalogInputs(true);
    getAnalogOutputs(true);
    getDigitalInputs(true);
    getDigitalOutputs(true);
  }
  pvCurrentState = mdtPortManager::Ready;
  qDebug() << "mdtDevice: new state is Ready";
  emit(stateChanged(pvCurrentState));
}

void mdtDevice::setStateBusy()
{
  if(pvCurrentState == mdtPortManager::Busy){
    return;
  }
  // Stop auto queries if running
  if(pvAutoQueryEnabled){
    pvQueryTimer->stop();
  }
  // Update I/Os
  if(pvIos != 0){
    pvIos->setAnalogInputsValue(mdtValue());
    pvIos->setAnalogOutputsValue(mdtValue());
    pvIos->setAnalogOutputsEnabled(false);
    pvIos->setDigitalInputsValue(mdtValue());
    pvIos->setDigitalOutputsValue(mdtValue());
    pvIos->setDigitalOutputsEnabled(false);
  }
  pvCurrentState = mdtPortManager::Busy;
  qDebug() << "**** mdtDevice: new state is Busy";
  emit(stateChanged(pvCurrentState));
  // Set state ready if requested
  if(pvBackToReadyStateTimeout >= 0){
    pvBackToReadyStateTimer->start(pvBackToReadyStateTimeout);
    ///QTimer::singleShot(retryTimeout, this, SLOT(setStateReady()));
  }
}

void mdtDevice::setStateWarning()
{
  if(pvCurrentState == mdtPortManager::Warning){
    return;
  }
  // Stop auto queries if running
  if(pvAutoQueryEnabled){
    pvQueryTimer->stop();
  }
  // Update I/Os
  if(pvIos != 0){
    pvIos->setAnalogInputsValue(mdtValue());
    pvIos->setAnalogOutputsValue(mdtValue());
    pvIos->setAnalogOutputsEnabled(false);
    pvIos->setDigitalInputsValue(mdtValue());
    pvIos->setDigitalOutputsValue(mdtValue());
    pvIos->setDigitalOutputsEnabled(false);
  }
  pvCurrentState = mdtPortManager::Warning;
  qDebug() << "mdtDevice: new state is Warning";
  emit(stateChanged(pvCurrentState));
}

void mdtDevice::setStateError()
{
  if(pvCurrentState == mdtPortManager::Error){
    return;
  }
  // Stop auto queries if running
  if(pvAutoQueryEnabled){
    pvQueryTimer->stop();
  }
  // Update I/Os
  if(pvIos != 0){
    pvIos->setAnalogInputsValue(mdtValue());
    pvIos->setAnalogOutputsValue(mdtValue());
    pvIos->setAnalogOutputsEnabled(false);
    pvIos->setDigitalInputsValue(mdtValue());
    pvIos->setDigitalOutputsValue(mdtValue());
    pvIos->setDigitalOutputsEnabled(false);
  }
  pvCurrentState = mdtPortManager::Error;
  // Add a error
  mdtError e(MDT_DEVICE_ERROR, "Device " + name() + " goes to error state", mdtError::Error);
  MDT_ERROR_SET_SRC(e, "mdtDevice");
  e.commit();
  qDebug() << "mdtDevice: new state is Error";
  emit(stateChanged(pvCurrentState));
}
