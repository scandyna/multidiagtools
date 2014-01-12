/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtDeviceIosSegment.h"
#include "mdtPortManager.h"
#include <QTimer>

#include <QDebug>

mdtDevice::mdtDevice(QObject *parent)
 : QObject(parent)
{
  qDebug() << "mdtDevice::mdtDevice() ...";
  pvIos = new mdtDeviceIos;
  pvBackToReadyStateTimeout = -1;
  pvBackToReadyStateTimer = new QTimer(this);
  pvBackToReadyStateTimer->setSingleShot(true);
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
  delete pvIos;
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

void mdtDevice::disconnectFromDevice()
{
  stop();
  if(portManager() != 0){
    if(!portManager()->isClosed()){
      portManager()->stop();
    }
  }
}

void mdtDevice::addInput(mdtAnalogIo *analogInput)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(analogInput != 0);

  pvIos->addAnalogInput(analogInput);
}

void mdtDevice::addInputs(const QList<mdtAnalogIo*> & analogInputs)
{
  Q_ASSERT(pvIos != 0);

  pvIos->addAnalogInputs(analogInputs);
}

void mdtDevice::addOutput(mdtAnalogIo *analogOutput)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(analogOutput != 0);

  connect(analogOutput, SIGNAL(valueChanged(mdtAnalogIo*)), this, SLOT(setAnalogOutputValue(mdtAnalogIo*)));
  pvIos->addAnalogOutput(analogOutput);
}

void mdtDevice::addOutputs(const QList<mdtAnalogIo*> & analogOutputs)
{
  Q_ASSERT(pvIos != 0);

  int i;

  for(i = 0; i < analogOutputs.size(); ++i){
    Q_ASSERT(analogOutputs.at(i) != 0);
    connect(analogOutputs.at(i), SIGNAL(valueChanged(mdtAnalogIo*)), this, SLOT(setAnalogOutputValue(mdtAnalogIo*)));
  }
  pvIos->addAnalogOutputs(analogOutputs);
}

void mdtDevice::addInput(mdtDigitalIo *digitalInput)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(digitalInput != 0);

  pvIos->addDigitalInput(digitalInput);
}

void mdtDevice::addInputs(const QList<mdtDigitalIo*> & digitalInputs)
{
  Q_ASSERT(pvIos != 0);

  pvIos->addDigitalInputs(digitalInputs);
}

void mdtDevice::addOutput(mdtDigitalIo *digitalOutput)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(digitalOutput != 0);

  connect(digitalOutput, SIGNAL(valueChanged(mdtDigitalIo*)), this, SLOT(setDigitalOutputValue(mdtDigitalIo*)));
  pvIos->addDigitalOutput(digitalOutput);
}

void mdtDevice::addOutputs(const QList<mdtDigitalIo*> & digitalOutputs)
{
  Q_ASSERT(pvIos != 0);

  int i;

  for(i = 0; i < digitalOutputs.size(); ++i){
    Q_ASSERT(digitalOutputs.at(i) != 0);
    connect(digitalOutputs.at(i), SIGNAL(valueChanged(mdtDigitalIo*)), this, SLOT(setDigitalOutputValue(mdtDigitalIo*)));
  }
  pvIos->addDigitalOutputs(digitalOutputs);
}

void mdtDevice::deleteIos()
{
  Q_ASSERT(pvIos != 0);

  int i;
  QList<mdtAnalogIo*> analogOutputs;
  QList<mdtDigitalIo*> digitalOutputs;

  analogOutputs = pvIos->analogOutputs();
  for(i = 0; i < analogOutputs.size(); ++i){
    Q_ASSERT(analogOutputs.at(i) != 0);
    disconnect(analogOutputs.at(i), SIGNAL(valueChanged(mdtAnalogIo*)), this, SLOT(setAnalogOutputValue(mdtAnalogIo*)));
  }
  digitalOutputs = pvIos->digitalOutputs();
  for(i = 0; i < digitalOutputs.size(); ++i){
    Q_ASSERT(digitalOutputs.at(i) != 0);
    disconnect(digitalOutputs.at(i), SIGNAL(valueChanged(mdtDigitalIo*)), this, SLOT(setDigitalOutputValue(mdtDigitalIo*)));
  }
  pvIos->deleteIos();
}

mdtDeviceIos *mdtDevice::ios()
{
  Q_ASSERT(pvIos != 0);

  return pvIos;
}

void mdtDevice::setBackToReadyStateTimeout(int timeout)
{
  pvBackToReadyStateTimeout = timeout;
}

mdtPortManager *mdtDevice::portManager()
{
  return 0;
}

bool mdtDevice::isReady()
{
  if(portManager() == 0){
    return false;
  }
  return portManager()->isReady();
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

void mdtDevice::wait(int ms)
{
  Q_ASSERT(portManager() != 0);

  portManager()->wait(ms);
}

mdtError mdtDevice::lastError() const
{
  return pvLastError;
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
  ai = pvIos->analogInputAtAddress(address);
  if(ai == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no analog input assigned to address " + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return mdtValue();
  }

  return getAnalogInputValue(ai, queryDevice, waitOnReply);
}

mdtValue mdtDevice::getAnalogInputValueAt(int position, bool queryDevice, bool waitOnReply)
{
  mdtAnalogIo *ai;

  if(pvIos == 0){
    return mdtValue();
  }
  // Check indexes
  if((position < 0)||(position >= pvIos->analogInputs().size())){
    pvLastError.setError(tr("Device ") + name() + tr(": position ") + QString::number(position) + tr(" is out of range"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDevice");
    pvLastError.commit();
    return mdtValue();
  }
  // Get internal I/O object
  ai = pvIos->analogInputs().at(position);
  if(ai == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no analog input assigned at position ") + QString::number(position), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDevice");
    pvLastError.commit();
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
  int transactionId = -1;
  int i;
  mdtPortTransaction *transaction;
  mdtDeviceIosSegment *segment;

  if(pvIos == 0){
    return -1;
  }
  if(pvIos->analogInputsCount() < 1){
    return 0;
  }
  for(i = 0; i < pvIos->analogInputsSegments().size(); ++i){
    segment = pvIos->analogInputsSegments().at(i);
    Q_ASSERT(segment != 0);
    // Get a new transaction
    transaction = getNewTransaction();
    transaction->setIoCount(segment->ioCount());
    transaction->setAddress(segment->startAddressRead());
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

mdtValue mdtDevice::getAnalogOutputValueAt(int position, bool queryDevice, bool waitOnReply)
{
  mdtAnalogIo *ao;

  if(pvIos == 0){
    return mdtValue();
  }
  // Check indexes
  if((position < 0)||(position >= pvIos->analogOutputs().size())){
    pvLastError.setError(tr("Device ") + name() + tr(": position ") + QString::number(position) + tr(" is out of range"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDevice");
    pvLastError.commit();
    return mdtValue();
  }
  // Get internal I/O object
  ao = pvIos->analogOutputs().at(position);
  if(ao == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no analog output assigned at position ") + QString::number(position), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDevice");
    pvLastError.commit();
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
  int transactionId = -1;
  int i;
  mdtPortTransaction *transaction;
  mdtDeviceIosSegment *segment;

  if(pvIos == 0){
    return -1;
  }
  if(pvIos->analogOutputsCount() < 1){
    return 0;
  }
  for(i = 0; i < pvIos->analogOutputsSegments().size(); ++i){
    segment = pvIos->analogOutputsSegments().at(i);
    Q_ASSERT(segment != 0);
    // Get a new transaction
    transaction = getNewTransaction();
    transaction->setIoCount(segment->ioCount());
    transaction->setAddress(segment->startAddressRead());
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

int mdtDevice::setAnalogOutputValueAt(int position, const mdtValue &value, bool sendToDevice, bool waitOnReply)
{
  mdtAnalogIo *analogOutput;

  if(pvIos == 0){
    return -1;
  }
  // Check indexes
  if((position < 0)||(position >= pvIos->analogOutputs().size())){
    pvLastError.setError(tr("Device ") + name() + tr(": position ") + QString::number(position) + tr(" is out of range"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDevice");
    pvLastError.commit();
    return -1;
  }
  // Get internal I/O object
  analogOutput = pvIos->analogOutputs().at(position);
  if(analogOutput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no analog output assigned at position ") + QString::number(position), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDevice");
    pvLastError.commit();
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
  int transactionId = -1;
  int i;
  mdtPortTransaction *transaction;
  mdtDeviceIosSegment *segment;

  if(pvIos == 0){
    return -1;
  }
  if(pvIos->analogOutputsCount() < 1){
    return 0;
  }
  // Disable I/Os - Must be re-enabled by subclass once data are in
  pvIos->setAnalogOutputsEnabled(false);
  for(i = 0; i < pvIos->analogOutputsSegments().size(); ++i){
    segment = pvIos->analogOutputsSegments().at(i);
    Q_ASSERT(segment != 0);
    // Get a new transaction
    transaction = getNewTransaction();
    transaction->setIoCount(segment->ioCount());
    transaction->setAddress(segment->startAddressWrite());
    // Send query and wait if requested
    if(waitOnReply){
      transaction->setQueryReplyMode(true);
      transactionId = writeAnalogOutputs(transaction, segment);
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
      transactionId = writeAnalogOutputs(transaction, segment);
      if(transactionId < 0){
        pvIos->setAnalogOutputsValue(mdtValue());
        return transactionId;
      }
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
  digitalInput = pvIos->digitalInputAtAddress(address);
  if(digitalInput == 0){
    mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": no digital input assigned to address " + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDevice");
    e.commit();
    return mdtValue();
  }

  return getDigitalInputValue(digitalInput, queryDevice, waitOnReply);
}

mdtValue mdtDevice::getDigitalInputValueAt(int position, bool queryDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalInput;

  if(pvIos == 0){
    return mdtValue();
  }
  // Check indexes
  if((position < 0)||(position >= pvIos->digitalInputs().size())){
    pvLastError.setError(tr("Device ") + name() + tr(": position ") + QString::number(position) + tr(" is out of range"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDevice");
    pvLastError.commit();
    return mdtValue();
  }
  // Get internal I/O object
  digitalInput = pvIos->digitalInputs().at(position);
  if(digitalInput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no digital input assigned at position ") + QString::number(position), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDevice");
    pvLastError.commit();
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
  int transactionId = -1;
  int i;
  mdtPortTransaction *transaction;
  mdtDeviceIosSegment *segment;


  if(pvIos == 0){
    return -1;
  }
  if(pvIos->digitalInputsCount() < 1){
    return 0;
  }
  for(i = 0; i < pvIos->digitalInputsSegments().size(); ++i){
    segment = pvIos->digitalInputsSegments().at(i);
    Q_ASSERT(segment != 0);
    // Get a new transaction
    transaction = getNewTransaction();
    transaction->setIoCount(segment->ioCount());
    transaction->setAddress(segment->startAddressRead());
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

mdtValue mdtDevice::getDigitalOutputValueAt(int position, bool queryDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalOutput;

  if(pvIos == 0){
    return mdtValue();
  }
  // Check indexes
  if((position < 0)||(position >= pvIos->digitalOutputs().size())){
    pvLastError.setError(tr("Device ") + name() + tr(": position ") + QString::number(position) + tr(" is out of range"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDevice");
    pvLastError.commit();
    return mdtValue();
  }
  // Get internal I/O object
  digitalOutput = pvIos->digitalOutputs().at(position);
  if(digitalOutput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no digital output assigned at position ") + QString::number(position), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDevice");
    pvLastError.commit();
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
  int transactionId = -1;
  int i;
  mdtPortTransaction *transaction;
  mdtDeviceIosSegment *segment;


  if(pvIos == 0){
    return -1;
  }
  if(pvIos->digitalOutputsCount() < 1){
    return 0;
  }
  for(i = 0; i < pvIos->digitalOutputsSegments().size(); ++i){
    segment = pvIos->digitalOutputsSegments().at(i);
    Q_ASSERT(segment != 0);
    // Get a new transaction
    transaction = getNewTransaction();
    transaction->setAddress(segment->startAddressRead());
    transaction->setIoCount(segment->ioCount());
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

int mdtDevice::setDigitalOutputValueAt(int position, const mdtValue &value, bool sendToDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalOutput;

  if(pvIos == 0){
    return -1;
  }
  // Check indexes
  if((position < 0)||(position >= pvIos->digitalOutputs().size())){
    pvLastError.setError(tr("Device ") + name() + tr(": position ") + QString::number(position) + tr(" is out of range"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDevice");
    pvLastError.commit();
    return -1;
  }
  // Get internal I/O object
  digitalOutput = pvIos->digitalOutputs().at(position);
  if(digitalOutput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no digital output assigned at position ") + QString::number(position), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDevice");
    pvLastError.commit();
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
  int transactionId = -1;
  int i;
  mdtPortTransaction *transaction;
  mdtDeviceIosSegment *segment;

  if(pvIos == 0){
    return -1;
  }
  if(pvIos->digitalOutputsCount() < 1){
    return -1;
  }
  // Disable I/Os - Must be re-enabled by subclass once data are in
  pvIos->setDigitalOutputsEnabled(false);
  for(i = 0; i < pvIos->digitalOutputsSegments().size(); ++i){
    segment = pvIos->digitalOutputsSegments().at(i);
    Q_ASSERT(segment != 0);
    // Get a new transaction
    transaction = getNewTransaction();
    transaction->setAddress(segment->startAddressWrite());
    transaction->setIoCount(segment->ioCount());
    // Send query and wait if requested
    if(waitOnReply){
      transaction->setQueryReplyMode(true);
      transactionId = writeDigitalOutputs(transaction, segment);
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
      transactionId = writeDigitalOutputs(transaction, segment);
      if(transactionId < 0){
        pvIos->setDigitalOutputsValue(mdtValue());
        return transactionId;
      }
    }
  }

  return transactionId;
}

mdtPortManager::state_t mdtDevice::currentState()
{
  if(portManager() == 0){
    return mdtPortManager::PortClosed;
  }
  return portManager()->currentState();
}

void mdtDevice::setAnalogOutputValue(mdtAnalogIo* analogOutput)
{
  Q_ASSERT(analogOutput != 0);

  if(currentState() != mdtPortManager::Ready){
    // Device busy, cannot threat query , try later
    return;
  }
  setAnalogOutputValue(analogOutput, analogOutput->value(), true, false);
}

void mdtDevice::setDigitalOutputValue(mdtDigitalIo* digitalOutput)
{
  Q_ASSERT(digitalOutput != 0);

  if(currentState() != mdtPortManager::Ready){
    // Device busy, cannot threat query , try later
    return;
  }
  setDigitalOutputValue(digitalOutput, digitalOutput->value(), true, false);
}

void mdtDevice::runQueries()
{
  if(currentState() != mdtPortManager::Ready){
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

mdtError & mdtDevice::lastErrorW()
{
  return pvLastError;
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

int mdtDevice::writeAnalogOutputs(mdtPortTransaction *transaction, mdtDeviceIosSegment *segment)
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

int mdtDevice::writeDigitalOutputs(mdtPortTransaction *transaction, mdtDeviceIosSegment *segment)
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
  switch((mdtPortManager::state_t)portManagerState){
    case mdtPortManager::PortClosed:
      setStatePortClosed();
      break;
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
    case mdtPortManager::PortError:
      setStateError();
      break;
    case mdtPortManager::Stopped:
    case mdtPortManager::Starting:
    case mdtPortManager::Stopping:
    case mdtPortManager::Running:
    case mdtPortManager::PortReady:
    case mdtPortManager::Connected:
      emit stateChanged(portManagerState);
      break;
  }
}

void mdtDevice::setStatePortClosed()
{
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
  emit(stateChanged(currentState()));
}

void mdtDevice::setStateDisconnected()
{
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
  qDebug() << "mdtDevice: new state is Disconnected";
  emit(stateChanged(currentState()));
}

void mdtDevice::setStateConnecting(/*const QString &message*/)
{
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
  qDebug() << "mdtDevice: new state is Connecting";
  emit(stateChanged(currentState()));
}

void mdtDevice::setStateReady()
{
  // Check if we have to restart query timer
  if((pvAutoQueryEnabled)&&(isReady())){
    pvQueryTimer->start();
  }
  // Update I/Os
  if(pvIos != 0){
    getAnalogInputs(true);
    getAnalogOutputs(true);
    getDigitalInputs(true);
    getDigitalOutputs(true);
  }
  emit(stateChanged(currentState()));
}

void mdtDevice::setStateBusy()
{
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
  qDebug() << "**** mdtDevice: new state is Busy";
  emit(stateChanged(currentState()));
  // Set state ready if requested
  if(pvBackToReadyStateTimeout >= 0){
    pvBackToReadyStateTimer->start(pvBackToReadyStateTimeout);
    ///QTimer::singleShot(retryTimeout, this, SLOT(setStateReady()));
  }
}

void mdtDevice::setStateError()
{
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
  // Add a error
  mdtError e(MDT_DEVICE_ERROR, "Device " + name() + " goes to error state", mdtError::Error);
  MDT_ERROR_SET_SRC(e, "mdtDevice");
  e.commit();
  qDebug() << "mdtDevice: new state is Error";
  emit(stateChanged(currentState()));
}
