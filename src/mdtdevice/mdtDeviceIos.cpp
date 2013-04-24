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
#include "mdtDeviceIos.h"
#include <QMapIterator>
#include <QtGlobal>

#include <QDebug>

mdtDeviceIos::mdtDeviceIos(QObject *parent)
 : QObject(parent)
{
  pvAutoDeleteIos = true;
  pvAnalogInputsFirstAddressRead = 0;
  pvAnalogOutputsFirstAddressRead = 0;
  pvAnalogOutputsFirstAddressWrite = 0;
}

mdtDeviceIos::~mdtDeviceIos()
{
  // Delete I/O objects
  if(pvAutoDeleteIos){
    deleteIos();
  }
}

void mdtDeviceIos::setAutoDeleteIos(bool autoDelete)
{
  pvAutoDeleteIos = autoDelete;
}

void mdtDeviceIos::deleteIos()
{
    qDeleteAll(pvAnalogInputs);
    pvAnalogInputs.clear();
    pvAnalogInputsByAddressRead.clear();
    
    qDeleteAll(pvAnalogOutputs);
    pvAnalogOutputs.clear();
    pvAnalogOutputsByAddressRead.clear();
    pvAnalogOutputsByAddressWrite.clear();
    
    qDeleteAll(pvDigitalInputs);
    pvDigitalInputs.clear();
    qDeleteAll(pvDigitalOutputs);
    pvDigitalOutputs.clear();
}

void mdtDeviceIos::addAnalogInput(mdtAnalogIo *ai)
{
  Q_ASSERT(ai != 0);

  pvAnalogInputs.append(ai);
  pvAnalogInputsByAddressRead.insert(ai->addressRead(), ai);
  Q_ASSERT(pvAnalogInputsByAddressRead.values().size() > 0);
  Q_ASSERT(pvAnalogInputsByAddressRead.values().at(0) != 0);
  // QMap returns a list sorted by keys, ascending
  pvAnalogInputsFirstAddressRead = pvAnalogInputsByAddressRead.values().at(0)->addressRead();

  ///pvAnalogInputsByAddressWrite.insert(ai->addressWrite(), ai);
}

mdtAnalogIo *mdtDeviceIos::analogInputAt(int address)
{
  return pvAnalogInputsByAddressRead.value(address, 0);
}

mdtAnalogIo *mdtDeviceIos::analogInputWithLabelShort(const QString &labelShort)
{
  int i;

  for(i=0; i<pvAnalogInputs.size(); ++i){
    Q_ASSERT(pvAnalogInputs.at(i) != 0);
    if(pvAnalogInputs.at(i)->labelShort() == labelShort){
      return pvAnalogInputs.at(i);
    }
  }

  /**
  QMapIterator<int, mdtAnalogIo*> it(pvAnalogInputs);

  while(it.hasNext()){
    it.next();
    Q_ASSERT(it.value() != 0);
    if(it.value()->labelShort() == labelShort){
      return it.value();
    }
  }
  */

  return 0;
}

const QList<mdtAnalogIo*> mdtDeviceIos::analogInputs() const
{
  ///return pvAnalogInputs.values();
  return pvAnalogInputs;
}

int mdtDeviceIos::analogInputsFirstAddress() const
{
  return pvAnalogInputsFirstAddressRead;
}

int mdtDeviceIos::analogInputsCount() const
{
  return pvAnalogInputs.size();
}

void mdtDeviceIos::addAnalogOutput(mdtAnalogIo *ao)
{
  Q_ASSERT(ao != 0);

  pvAnalogOutputs.append(ao);
  pvAnalogOutputsByAddressRead.insert(ao->addressRead(), ao);
  pvAnalogOutputsByAddressWrite.insert(ao->addressWrite(), ao);
  // Get first addresses for read and write access
  Q_ASSERT(pvAnalogOutputsByAddressRead.values().size() > 0);
  Q_ASSERT(pvAnalogOutputsByAddressRead.values().at(0) != 0);
  // QMap returns a list sorted by keys, ascending
  pvAnalogOutputsFirstAddressRead = pvAnalogOutputsByAddressRead.values().at(0)->addressRead();
  Q_ASSERT(pvAnalogOutputsByAddressWrite.values().size() > 0);
  Q_ASSERT(pvAnalogOutputsByAddressWrite.values().at(0) != 0);
  // QMap returns a list sorted by keys, ascending
  pvAnalogOutputsFirstAddressWrite = pvAnalogOutputsByAddressWrite.values().at(0)->addressRead();

  
  /// \note : utile ??
  ///connect(ao, SIGNAL(valueChanged(int)), this, SIGNAL(analogOutputValueChanged(int)));
}

mdtAnalogIo *mdtDeviceIos::analogOutputAtAddressRead(int address)
{
  return pvAnalogOutputsByAddressRead.value(address, 0);
}

mdtAnalogIo *mdtDeviceIos::analogOutputAtAddressWrite(int address)
{
  return pvAnalogOutputsByAddressWrite.value(address, 0);
}

mdtAnalogIo *mdtDeviceIos::analogOutputWithLabelShort(const QString &labelShort)
{
  int i;

  for(i=0; i<pvAnalogOutputs.size(); ++i){
    Q_ASSERT(pvAnalogOutputs.at(i) != 0);
    if(pvAnalogOutputs.at(i)->labelShort() == labelShort){
      return pvAnalogOutputs.at(i);
    }
  }
  /**
  QMapIterator<int, mdtAnalogIo*> it(pvAnalogOutputs);

  while(it.hasNext()){
    it.next();
    Q_ASSERT(it.value() != 0);
    if(it.value()->labelShort() == labelShort){
      return it.value();
    }
  }
  */

  return 0;
}

const QList<mdtAnalogIo*> mdtDeviceIos::analogOutputs() const
{
  return pvAnalogOutputs;
}

int mdtDeviceIos::analogOutputsFirstAddressRead() const
{
  return pvAnalogOutputsFirstAddressRead;
}

int mdtDeviceIos::analogOutputsFirstAddressWrite() const
{
  return pvAnalogOutputsFirstAddressWrite;
}

int mdtDeviceIos::analogOutputsCount() const
{
  return pvAnalogOutputs.size();
}

QList<int> mdtDeviceIos::analogOutputsValuesIntByAddressWrite() const
{
  QList<int> values;
  QList<mdtAnalogIo*> aos;
  int i;

  aos = pvAnalogOutputsByAddressWrite.values();
  for(i=0; i<aos.size(); i++){
    Q_ASSERT(aos.at(i) != 0);
    values.append(aos.at(i)->valueInt());
  }

  return values;
}

void mdtDeviceIos::setAnalogOutputsValue(QVariant value)
{
  int i;

  for(i=0; i<pvAnalogOutputs.size(); ++i){
    Q_ASSERT(pvAnalogOutputs.at(i) != 0);
    pvAnalogOutputs.at(i)->setValue(value);
  }

  /**
  QList<mdtAnalogIo*> aos;
  int i;

  aos = pvAnalogOutputs.values();
  for(i=0; i<aos.size(); i++){
    Q_ASSERT(aos.at(i) != 0);
    aos.at(i)->setValue(value);
  }
  */
}

void mdtDeviceIos::addDigitalInput(mdtDigitalIo *di)
{
  Q_ASSERT(di != 0);

  pvDigitalInputs.insert(di->address(), di);
}

mdtDigitalIo *mdtDeviceIos::digitalInputAt(int address)
{
  return pvDigitalInputs.value(address, 0);
}

mdtDigitalIo *mdtDeviceIos::digitalInputWithLabelShort(const QString &labelShort)
{
  QMapIterator<int, mdtDigitalIo*> it(pvDigitalInputs);

  while(it.hasNext()){
    it.next();
    Q_ASSERT(it.value() != 0);
    if(it.value()->labelShort() == labelShort){
      return it.value();
    }
  }

  return 0;
}

QList<mdtDigitalIo*> mdtDeviceIos::digitalInputs()
{
  return pvDigitalInputs.values();
}

int mdtDeviceIos::digitalInputsCount() const
{
  return pvDigitalInputs.size();
}

void mdtDeviceIos::addDigitalOutput(mdtDigitalIo *dout)
{
  Q_ASSERT(dout != 0);

  pvDigitalOutputs.insert(dout->address(), dout);
  connect(dout, SIGNAL(stateChanged(int)), this, SIGNAL(digitalOutputStateChanged(int)));
}

mdtDigitalIo *mdtDeviceIos::digitalOutputAt(int address)
{
  return pvDigitalOutputs.value(address, 0);
}

mdtDigitalIo *mdtDeviceIos::digitalOutputWithLabelShort(const QString &labelShort)
{
  QMapIterator<int, mdtDigitalIo*> it(pvDigitalOutputs);

  while(it.hasNext()){
    it.next();
    Q_ASSERT(it.value() != 0);
    if(it.value()->labelShort() == labelShort){
      return it.value();
    }
  }

  return 0;
}

QList<mdtDigitalIo*> mdtDeviceIos::digitalOutputs()
{
  return pvDigitalOutputs.values();
}

int mdtDeviceIos::digitalOutputsCount() const
{
  return pvDigitalOutputs.size();
}

const QList<bool> mdtDeviceIos::digitalOutputsStates() const
{
  QList<bool> states;
  QList<mdtDigitalIo*> dos;
  int i;

  dos = pvDigitalOutputs.values();
  for(i=0; i<dos.size(); i++){
    Q_ASSERT(dos.at(i) != 0);
    states.append(dos.at(i)->isOn());
  }

  return states;
}

void mdtDeviceIos::updateAnalogInputValues(const QList<QVariant> &values)
{
  int i, max;
  ///mdtAnalogIo *ai;
  QList<mdtAnalogIo*> lst;

  // Get the list from address conatiner, so we have it sorted by address (QMap returns a sorted list, by keys, ascending)
  lst = pvAnalogInputsByAddressRead.values();
  max = qMin(values.size(), lst.size());
  for(i=0; i<max; ++i){
    Q_ASSERT(lst.at(i) != 0);
    lst.at(i)->setValue(values.at(i), false);
  }

  /**
  for(i=0; i<values.size(); i++){
    // Get I/O and store value
    ai = analogInputAt(i);
    if(ai != 0){
      ai->setValue(values.at(i), false);
    }
  }
  */
}

void mdtDeviceIos::updateAnalogOutputValues(const QList<QVariant> &values)
{
  int i, max;
  ///mdtAnalogIo *ao;
  QList<mdtAnalogIo*> lst;

  // Get the list from address conatiner, so we have it sorted by address (QMap returns a sorted list, by keys, ascending)
  lst = pvAnalogOutputsByAddressRead.values();  // We update (G)UI, so we read from device
  max = qMin(values.size(), lst.size());
  for(i=0; i<max; ++i){
    Q_ASSERT(lst.at(i) != 0);
    lst.at(i)->setValue(values.at(i), false);
  }

  /**
  for(i=0; i<values.size(); i++){
    // Get I/O and store value
    ao = analogOutputAt(i);
    if(ao != 0){
      ao->setValue(values.at(i), false);
    }
  }
  */
}

void mdtDeviceIos::setAnalogOutputsEnabled(bool enabled)
{
  int i;

  for(i=0; i<pvAnalogOutputs.size(); ++i){
    Q_ASSERT(pvAnalogOutputs.at(i) != 0);
    pvAnalogOutputs.at(i)->setEnabled(enabled);
  }
  /**
  QMapIterator<int, mdtAnalogIo*> it(pvAnalogOutputs);

  while(it.hasNext()){
    it.next();
    Q_ASSERT(it.value() != 0);
    it.value()->setEnabled(enabled);
  }
  */
}

void mdtDeviceIos::updateDigitalInputStates(const QList<QVariant> &values)
{
  int i;
  mdtDigitalIo *di;

  for(i=0; i<values.size(); i++){
    // Get I/O and store value
    di = digitalInputAt(i);
    if(di != 0){
      di->setOn(values.at(i), false);
    }
  }
}

void mdtDeviceIos::updateDigitalOutputStates(const QList<QVariant> &values)
{
  int i;
  mdtDigitalIo *dout;

  for(i=0; i<values.size(); i++){
    // Get I/O and store value
    dout = digitalOutputAt(i);
    if(dout != 0){
      dout->setOn(values.at(i), false);
      dout->setEnabled(true);
    }
  }
}
