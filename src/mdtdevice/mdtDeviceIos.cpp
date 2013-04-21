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
#include "mdtDeviceIos.h"
#include <QMapIterator>

#include <QDebug>

mdtDeviceIos::mdtDeviceIos(QObject *parent)
 : QObject(parent)
{
  pvAutoDeleteIos = true;
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
    qDeleteAll(pvAnalogOutputs);
    pvAnalogOutputs.clear();
    qDeleteAll(pvDigitalInputs);
    pvDigitalInputs.clear();
    qDeleteAll(pvDigitalOutputs);
    pvDigitalOutputs.clear();
}

void mdtDeviceIos::addAnalogInput(mdtAnalogIo *ai)
{
  Q_ASSERT(ai != 0);

  pvAnalogInputs.insert(ai->address(), ai);
}

mdtAnalogIo *mdtDeviceIos::analogInputAt(int address)
{
  return pvAnalogInputs.value(address, 0);
}

mdtAnalogIo *mdtDeviceIos::analogInputWithLabelShort(const QString &labelShort)
{
  QMapIterator<int, mdtAnalogIo*> it(pvAnalogInputs);

  while(it.hasNext()){
    it.next();
    Q_ASSERT(it.value() != 0);
    if(it.value()->labelShort() == labelShort){
      return it.value();
    }
  }

  return 0;
}

QList<mdtAnalogIo*> mdtDeviceIos::analogInputs()
{
  return pvAnalogInputs.values();
}

int mdtDeviceIos::analogInputsCount() const
{
  return pvAnalogInputs.size();
}

void mdtDeviceIos::addAnalogOutput(mdtAnalogIo *ao)
{
  Q_ASSERT(ao != 0);

  pvAnalogOutputs.insert(ao->address(), ao);
  connect(ao, SIGNAL(valueChanged(int)), this, SIGNAL(analogOutputValueChanged(int)));
}

mdtAnalogIo *mdtDeviceIos::analogOutputAt(int address)
{
  return pvAnalogOutputs.value(address, 0);
}

mdtAnalogIo *mdtDeviceIos::analogOutputWithLabelShort(const QString &labelShort)
{
  QMapIterator<int, mdtAnalogIo*> it(pvAnalogOutputs);

  while(it.hasNext()){
    it.next();
    Q_ASSERT(it.value() != 0);
    if(it.value()->labelShort() == labelShort){
      return it.value();
    }
  }

  return 0;
}

QList<mdtAnalogIo*> mdtDeviceIos::analogOutputs()
{
  return pvAnalogOutputs.values();
}

int mdtDeviceIos::analogOutputsCount() const
{
  return pvAnalogOutputs.size();
}

const QList<int> mdtDeviceIos::analogOutputsValuesInt() const
{
  QList<int> values;
  QList<mdtAnalogIo*> aos;
  int i;

  aos = pvAnalogOutputs.values();
  for(i=0; i<aos.size(); i++){
    Q_ASSERT(aos.at(i) != 0);
    values.append(aos.at(i)->valueInt());
  }

  return values;
}

void mdtDeviceIos::setAnalogOutputsValue(QVariant value)
{
  QList<mdtAnalogIo*> aos;
  int i;

  aos = pvAnalogOutputs.values();
  for(i=0; i<aos.size(); i++){
    Q_ASSERT(aos.at(i) != 0);
    aos.at(i)->setValue(value);
  }
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
  int i;
  mdtAnalogIo *ai;

  for(i=0; i<values.size(); i++){
    // Get I/O and store value
    ai = analogInputAt(i);
    if(ai != 0){
      ai->setValue(values.at(i), false);
    }
  }
}

void mdtDeviceIos::updateAnalogOutputValues(const QList<QVariant> &values)
{
  int i;
  mdtAnalogIo *ao;

  for(i=0; i<values.size(); i++){
    // Get I/O and store value
    ao = analogOutputAt(i);
    if(ao != 0){
      ao->setValue(values.at(i), false);
      ///ao->setEnabled(true);
    }
  }
}

void mdtDeviceIos::setAnalogOutputsEnabled(bool enabled)
{
  QMapIterator<int, mdtAnalogIo*> it(pvAnalogOutputs);

  while(it.hasNext()){
    it.next();
    Q_ASSERT(it.value() != 0);
    it.value()->setEnabled(enabled);
  }
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
