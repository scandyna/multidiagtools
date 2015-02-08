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
#include "mdtMultiIoDevice.h"
#include "mdtDeviceIosSegment.h"
#include "mdtPortTransaction.h"
#include "mdtPortManager.h"

mdtMultiIoDevice::mdtMultiIoDevice(QObject* parent)
 : mdtDevice(parent),
   pvIos(new mdtDeviceIos)
{

}

mdtMultiIoDevice::~mdtMultiIoDevice()
{
}

void mdtMultiIoDevice::addInput(mdtAnalogIo *analogInput)
{
  Q_ASSERT(pvIos);
  Q_ASSERT(analogInput != 0);

  pvIos->addAnalogInput(analogInput);
}

void mdtMultiIoDevice::addInputs(const QList<mdtAnalogIo*> & analogInputs)
{
  Q_ASSERT(pvIos);

  pvIos->addAnalogInputs(analogInputs);
}

void mdtMultiIoDevice::addOutput(mdtAnalogIo *analogOutput)
{
  Q_ASSERT(pvIos);
  Q_ASSERT(analogOutput != 0);

  connect(analogOutput, SIGNAL(valueChanged(mdtAnalogIo*)), this, SLOT(setAnalogOutputValue(mdtAnalogIo*)));
  pvIos->addAnalogOutput(analogOutput);
}

void mdtMultiIoDevice::addOutputs(const QList<mdtAnalogIo*> & analogOutputs)
{
  Q_ASSERT(pvIos);

  int i;

  for(i = 0; i < analogOutputs.size(); ++i){
    Q_ASSERT(analogOutputs.at(i) != 0);
    connect(analogOutputs.at(i), SIGNAL(valueChanged(mdtAnalogIo*)), this, SLOT(setAnalogOutputValue(mdtAnalogIo*)));
  }
  pvIos->addAnalogOutputs(analogOutputs);
}

void mdtMultiIoDevice::addInput(mdtDigitalIo *digitalInput)
{
  Q_ASSERT(pvIos);
  Q_ASSERT(digitalInput != 0);

  pvIos->addDigitalInput(digitalInput);
}

void mdtMultiIoDevice::addInputs(const QList<mdtDigitalIo*> & digitalInputs)
{
  Q_ASSERT(pvIos);

  pvIos->addDigitalInputs(digitalInputs);
}

void mdtMultiIoDevice::addOutput(mdtDigitalIo *digitalOutput)
{
  Q_ASSERT(pvIos);
  Q_ASSERT(digitalOutput != 0);

  connect(digitalOutput, SIGNAL(valueChanged(mdtDigitalIo*)), this, SLOT(setDigitalOutputValue(mdtDigitalIo*)));
  pvIos->addDigitalOutput(digitalOutput);
}

void mdtMultiIoDevice::addOutputs(const QList<mdtDigitalIo*> & digitalOutputs)
{
  Q_ASSERT(pvIos);

  int i;

  for(i = 0; i < digitalOutputs.size(); ++i){
    Q_ASSERT(digitalOutputs.at(i) != 0);
    connect(digitalOutputs.at(i), SIGNAL(valueChanged(mdtDigitalIo*)), this, SLOT(setDigitalOutputValue(mdtDigitalIo*)));
  }
  pvIos->addDigitalOutputs(digitalOutputs);
}

void mdtMultiIoDevice::deleteIos()
{
  Q_ASSERT(pvIos);

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

mdtValue mdtMultiIoDevice::getAnalogInputValue(mdtAnalogIo *analogInput, bool queryDevice, bool waitOnReply)
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

mdtValue mdtMultiIoDevice::getAnalogInputValue(int address, bool queryDevice, bool waitOnReply)
{
  mdtAnalogIo *ai;

  if(!pvIos){
    return mdtValue();
  }
  // Get internal I/O object
  ai = pvIos->analogInputAtAddress(address);
  if(ai == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no analog input assigned to address ") + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }

  return getAnalogInputValue(ai, queryDevice, waitOnReply);
}

mdtValue mdtMultiIoDevice::getAnalogInputValueAt(int position, bool queryDevice, bool waitOnReply)
{
  mdtAnalogIo *ai;

  if(!pvIos){
    return mdtValue();
  }
  // Check indexes
  if((position < 0)||(position >= pvIos->analogInputs().size())){
    pvLastError.setError(tr("Device ") + name() + tr(": position ") + QString::number(position) + tr(" is out of range"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }
  // Get internal I/O object
  ai = pvIos->analogInputs().at(position);
  if(ai == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no analog input assigned at position ") + QString::number(position), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }

  return getAnalogInputValue(ai, queryDevice, waitOnReply);
}

mdtValue mdtMultiIoDevice::getAnalogInputValue(const QString &labelShort, bool queryDevice, bool waitOnReply)
{
  mdtAnalogIo *ai;

  if(!pvIos){
    return mdtValue();
  }
  // Get internal I/O object
  ai = pvIos->analogInputWithLabelShort(labelShort);
  if(ai == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no analog input with label short ") + labelShort, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }

  return getAnalogInputValue(ai, queryDevice, waitOnReply);
}

int mdtMultiIoDevice::getAnalogInputs(bool waitOnReply)
{
  int transactionId = -1;
  int i;
  mdtPortTransaction *transaction;
  mdtDeviceIosSegment *segment;

  if(!pvIos){
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

mdtValue mdtMultiIoDevice::getAnalogOutputValue(mdtAnalogIo *analogOutput, bool queryDevice, bool waitOnReply)
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

mdtValue mdtMultiIoDevice::getAnalogOutputValue(int addressRead, bool queryDevice, bool waitOnReply)
{
  mdtAnalogIo *ao;

  if(!pvIos){
    return mdtValue();
  }
  // Get internal I/O object
  ao = pvIos->analogOutputAtAddressRead(addressRead);
  if(ao == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no analog output assigned to address ") + QString::number(addressRead), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }

  return getAnalogOutputValue(ao, queryDevice, waitOnReply);
}

mdtValue mdtMultiIoDevice::getAnalogOutputValueAt(int position, bool queryDevice, bool waitOnReply)
{
  mdtAnalogIo *ao;

  if(!pvIos){
    return mdtValue();
  }
  // Check indexes
  if((position < 0)||(position >= pvIos->analogOutputs().size())){
    pvLastError.setError(tr("Device ") + name() + tr(": position ") + QString::number(position) + tr(" is out of range"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }
  // Get internal I/O object
  ao = pvIos->analogOutputs().at(position);
  if(ao == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no analog output assigned at position ") + QString::number(position), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }

  return getAnalogOutputValue(ao, queryDevice, waitOnReply);
}

mdtValue mdtMultiIoDevice::getAnalogOutputValue(const QString &labelShort, bool queryDevice, bool waitOnReply)
{
  mdtAnalogIo *ao;

  if(!pvIos){
    return mdtValue();
  }
  // Get internal I/O object
  ao = pvIos->analogOutputWithLabelShort(labelShort);
  if(ao == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no analog output assigned with label ") + labelShort, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }

  return getAnalogOutputValue(ao, queryDevice, waitOnReply);
}

int mdtMultiIoDevice::getAnalogOutputs(bool waitOnReply)
{
  int transactionId = -1;
  int i;
  mdtPortTransaction *transaction;
  mdtDeviceIosSegment *segment;

  if(!pvIos){
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

int mdtMultiIoDevice::setAnalogOutputValue(mdtAnalogIo *analogOutput, const mdtValue &value, bool sendToDevice, bool waitOnReply)
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

int mdtMultiIoDevice::setAnalogOutputValue(int addressWrite, const mdtValue &value, bool sendToDevice, bool waitOnReply)
{
  mdtAnalogIo *analogOutput;

  if(!pvIos){
    return -1;
  }
  // Get I/O object
  analogOutput = pvIos->analogOutputAtAddressWrite(addressWrite);
  if(analogOutput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no analog output assigned to address ") + QString::number(addressWrite), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return -1;
  }

  return setAnalogOutputValue(analogOutput, value, sendToDevice, waitOnReply);
}

int mdtMultiIoDevice::setAnalogOutputValueAt(int position, const mdtValue &value, bool sendToDevice, bool waitOnReply)
{
  mdtAnalogIo *analogOutput;

  if(!pvIos){
    return -1;
  }
  // Check indexes
  if((position < 0)||(position >= pvIos->analogOutputs().size())){
    pvLastError.setError(tr("Device ") + name() + tr(": position ") + QString::number(position) + tr(" is out of range"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return -1;
  }
  // Get internal I/O object
  analogOutput = pvIos->analogOutputs().at(position);
  if(analogOutput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no analog output assigned at position ") + QString::number(position), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return -1;
  }

  return setAnalogOutputValue(analogOutput, value, sendToDevice, waitOnReply);
}

int mdtMultiIoDevice::setAnalogOutputValue(const QString &labelShort, const mdtValue &value, bool sendToDevice, bool waitOnReply)
{
  mdtAnalogIo *analogOutput;

  if(!pvIos){
    return -1;
  }
  // Get I/O object
  analogOutput = pvIos->analogOutputWithLabelShort(labelShort);
  if(analogOutput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no analog output found with label short ") + labelShort, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return -1;
  }

  return setAnalogOutputValue(analogOutput, value, sendToDevice, waitOnReply);
}

int mdtMultiIoDevice::setAnalogOutputs(bool waitOnReply)
{
  int transactionId = -1;
  int i;
  mdtPortTransaction *transaction;
  mdtDeviceIosSegment *segment;

  if(!pvIos){
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

mdtValue mdtMultiIoDevice::getDigitalInputValue(mdtDigitalIo *digitalInput, bool queryDevice, bool waitOnReply)
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

mdtValue mdtMultiIoDevice::getDigitalInputValue(int address, bool queryDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalInput;

  if(!pvIos){
    return mdtValue();
  }
  // Get I/O object
  digitalInput = pvIos->digitalInputAtAddress(address);
  if(digitalInput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no digital input assigned to address ") + QString::number(address), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }

  return getDigitalInputValue(digitalInput, queryDevice, waitOnReply);
}

mdtValue mdtMultiIoDevice::getDigitalInputValueAt(int position, bool queryDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalInput;

  if(!pvIos){
    return mdtValue();
  }
  // Check indexes
  if((position < 0)||(position >= pvIos->digitalInputs().size())){
    pvLastError.setError(tr("Device ") + name() + tr(": position ") + QString::number(position) + tr(" is out of range"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }
  // Get internal I/O object
  digitalInput = pvIos->digitalInputs().at(position);
  if(digitalInput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no digital input assigned at position ") + QString::number(position), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }

  return getDigitalInputValue(digitalInput, queryDevice, waitOnReply);
}

mdtValue mdtMultiIoDevice::getDigitalInputValue(const QString &labelShort, bool queryDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalInput;

  if(!pvIos){
    return mdtValue();
  }
  // Get I/O object
  digitalInput = pvIos->digitalInputWithLabelShort(labelShort);
  if(digitalInput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no digital input found with label short ") + labelShort, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }

  return getDigitalInputValue(digitalInput, queryDevice, waitOnReply);
}

int mdtMultiIoDevice::getDigitalInputs(bool waitOnReply)
{
  int transactionId = -1;
  int i;
  mdtPortTransaction *transaction;
  mdtDeviceIosSegment *segment;


  if(!pvIos){
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

mdtValue mdtMultiIoDevice::getDigitalOutputValue(mdtDigitalIo *digitalOutput, bool queryDevice, bool waitOnReply)
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

mdtValue mdtMultiIoDevice::getDigitalOutputValue(int addressRead, bool queryDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalOutput;

  if(!pvIos){
    return mdtValue();
  }
  // Get I/O object
  digitalOutput = pvIos->digitalOutputAtAddressRead(addressRead);
  if(digitalOutput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no digital output assigned to address ") + QString::number(addressRead), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }

  return getDigitalOutputValue(digitalOutput, queryDevice, waitOnReply);
}

mdtValue mdtMultiIoDevice::getDigitalOutputValueAt(int position, bool queryDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalOutput;

  if(!pvIos){
    return mdtValue();
  }
  // Check indexes
  if((position < 0)||(position >= pvIos->digitalOutputs().size())){
    pvLastError.setError(tr("Device ") + name() + tr(": position ") + QString::number(position) + tr(" is out of range"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }
  // Get internal I/O object
  digitalOutput = pvIos->digitalOutputs().at(position);
  if(digitalOutput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no digital output assigned at position ") + QString::number(position), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }

  return getDigitalOutputValue(digitalOutput, queryDevice, waitOnReply);
}

mdtValue mdtMultiIoDevice::getDigitalOutputValue(const QString &labelShort, bool queryDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalOutput;

  if(!pvIos){
    return mdtValue();
  }
  // Get I/O object
  digitalOutput = pvIos->digitalOutputWithLabelShort(labelShort);
  if(digitalOutput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no digital output found with label short ") + labelShort, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return mdtValue();
  }

  return getDigitalOutputValue(digitalOutput, queryDevice, waitOnReply);
}

int mdtMultiIoDevice::getDigitalOutputs(bool waitOnReply)
{
  int transactionId = -1;
  int i;
  mdtPortTransaction *transaction;
  mdtDeviceIosSegment *segment;


  if(!pvIos){
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

int mdtMultiIoDevice::setDigitalOutputValue(mdtDigitalIo *digitalOutput, const mdtValue &value, bool sendToDevice, bool waitOnReply)
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

int mdtMultiIoDevice::setDigitalOutputValue(int addressWrite, const mdtValue &value, bool sendToDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalOutput;

  if(!pvIos){
    return -1;
  }
  // Get I/O object
  digitalOutput = pvIos->digitalOutputAtAddressWrite(addressWrite);
  if(digitalOutput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no digital output assigned to address ") + QString::number(addressWrite), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return -1;
  }

  return setDigitalOutputValue(digitalOutput, value, sendToDevice, waitOnReply);
}

int mdtMultiIoDevice::setDigitalOutputValueAt(int position, const mdtValue &value, bool sendToDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalOutput;

  if(!pvIos){
    return -1;
  }
  // Check indexes
  if((position < 0)||(position >= pvIos->digitalOutputs().size())){
    pvLastError.setError(tr("Device ") + name() + tr(": position ") + QString::number(position) + tr(" is out of range"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return -1;
  }
  // Get internal I/O object
  digitalOutput = pvIos->digitalOutputs().at(position);
  if(digitalOutput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no digital output assigned at position ") + QString::number(position), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return -1;
  }

  return setDigitalOutputValue(digitalOutput, value, sendToDevice, waitOnReply);
}

int mdtMultiIoDevice::setDigitalOutputValue(const QString &labelShort, const mdtValue &value, bool sendToDevice, bool waitOnReply)
{
  mdtDigitalIo *digitalOutput;

  if(!pvIos){
    return -1;
  }
  // Get I/O object
  digitalOutput = pvIos->digitalOutputWithLabelShort(labelShort);
  if(digitalOutput == 0){
    pvLastError.setError(tr("Device ") + name() + tr(": no digital output found with label short ") + labelShort, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtMultiIoDevice");
    pvLastError.commit();
    return -1;
  }

  return setDigitalOutputValue(digitalOutput, value, sendToDevice, waitOnReply);
}

int mdtMultiIoDevice::setDigitalOutputsValue(const mdtValue& value, bool sendToDevice, bool waitOnReply)
{
  if(!pvIos){
    return -1;
  }
  if(pvIos->digitalOutputsCount() < 1){
    return -1;
  }
  // Cache value to all digital outputs
  pvIos->setDigitalOutputsValue(value);
  // Send to device if requested
  if(sendToDevice){
    return setDigitalOutputs(waitOnReply);
  }

  return 0;
}

int mdtMultiIoDevice::setDigitalOutputs(bool waitOnReply)
{
  int transactionId = -1;
  int i;
  mdtPortTransaction *transaction;
  mdtDeviceIosSegment *segment;

  if(!pvIos){
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

void mdtMultiIoDevice::setAnalogOutputValue(mdtAnalogIo* analogOutput)
{
  Q_ASSERT(analogOutput != 0);

  if(currentState() != State_t::Ready){
    // Device busy, cannot threat query , try later
    return;
  }
  setAnalogOutputValue(analogOutput, analogOutput->value(), true, false);
}

void mdtMultiIoDevice::setDigitalOutputValue(mdtDigitalIo* digitalOutput)
{
  Q_ASSERT(digitalOutput != 0);

  if(currentState() != State_t::Ready){
    // Device busy, cannot threat query , try later
    return;
  }
  setDigitalOutputValue(digitalOutput, digitalOutput->value(), true, false);
}

mdtPortTransaction *mdtMultiIoDevice::getNewTransaction()
{
  Q_ASSERT(portManager() != 0);

  return portManager()->getNewTransaction();
}

void mdtMultiIoDevice::restoreTransaction(mdtPortTransaction *transaction)
{
  Q_ASSERT(portManager() != 0);
  Q_ASSERT(transaction != 0);

  portManager()->restoreTransaction(transaction);
}

bool mdtMultiIoDevice::waitTransactionDone(int id)
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

int mdtMultiIoDevice::readAnalogInput(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtMultiIoDevice::readAnalogInputs(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtMultiIoDevice::readAnalogOutput(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtMultiIoDevice::readAnalogOutputs(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtMultiIoDevice::writeAnalogOutput(int value, mdtPortTransaction *transaction)
{
  return -1;
}

int mdtMultiIoDevice::writeAnalogOutputs(mdtPortTransaction *transaction, mdtDeviceIosSegment *segment)
{
  return -1;
}

int mdtMultiIoDevice::readDigitalInput(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtMultiIoDevice::readDigitalInputs(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtMultiIoDevice::readDigitalOutput(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtMultiIoDevice::readDigitalOutputs(mdtPortTransaction *transaction)
{
  return -1;
}

int mdtMultiIoDevice::writeDigitalOutput(bool state, mdtPortTransaction *transaction)
{
  return -1;
}

int mdtMultiIoDevice::writeDigitalOutputs(mdtPortTransaction *transaction, mdtDeviceIosSegment *segment)
{
  return -1;
}

void mdtMultiIoDevice::stateChangedEvent(mdtDevice::State_t newState)
{
  if(newState == State_t::Ready){
    enableIos();
  }else{
    disableIos();
  }
}

void mdtMultiIoDevice::enableIos()
{
  Q_ASSERT(pvIos);

  getAnalogInputs(true);
  getAnalogOutputs(true);
  pvIos->setAnalogOutputsEnabled(true);
  getDigitalInputs(true);
  getDigitalOutputs(true);
  pvIos->setDigitalOutputsEnabled(true);
}

void mdtMultiIoDevice::disableIos()
{
  Q_ASSERT(pvIos);

  pvIos->setAnalogInputsValue(mdtValue());
  pvIos->setAnalogOutputsValue(mdtValue());
  pvIos->setAnalogOutputsEnabled(false);
  pvIos->setDigitalInputsValue(mdtValue());
  pvIos->setDigitalOutputsValue(mdtValue());
  pvIos->setDigitalOutputsEnabled(false);
}
