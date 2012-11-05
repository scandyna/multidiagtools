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
  connect(ao, SIGNAL(valueChanged(int, double)), this, SIGNAL(analogOutputValueChanged(int, double)));
}

mdtAnalogIo *mdtDeviceIos::analogOutputAt(int address)
{
  return pvAnalogOutputs.value(address, 0);
}

QList<mdtAnalogIo*> mdtDeviceIos::analogOutputs()
{
  return pvAnalogOutputs.values();
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

QList<mdtDigitalIo*> mdtDeviceIos::digitalInputs()
{
  return pvDigitalInputs.values();
}

void mdtDeviceIos::addDigitalOutput(mdtDigitalIo *dout)
{
  Q_ASSERT(dout != 0);

  pvDigitalOutputs.insert(dout->address(), dout);
  connect(dout, SIGNAL(stateChanged(int, bool)), this, SIGNAL(digitalOutputStateChanged(int, bool)));
}

mdtDigitalIo *mdtDeviceIos::digitalOutputAt(int address)
{
  return pvDigitalOutputs.value(address, 0);
}

QList<mdtDigitalIo*> mdtDeviceIos::digitalOutputs()
{
  return pvDigitalOutputs.values();
}

void mdtDeviceIos::updateAnalogInputValue(int address, double value, bool isValid)
{
  mdtAnalogIo *ai;

  ai = analogInputAt(address);
  if(ai == 0){
    return;
  }
  ai->setValue(value, isValid);
}

void mdtDeviceIos::updateAnalogInputValueInt(int address, int value, bool isValid)
{
  mdtAnalogIo *ai;

  ai = analogInputAt(address);
  if(ai == 0){
    return;
  }
  ai->setValueInt(value, isValid);
}

void mdtDeviceIos::updateAnalogInputValues(const QList<QVariant> &values)
{
  int i;
  QVariant var;
  mdtAnalogIo *ai;

  for(i=0; i<values.size(); i++){
    // Get I/O and store value
    ai = analogInputAt(i);
    if(ai != 0){
      var = values.at(i);
      if(!var.isValid()){
        ai->setValue(0.0, false);
      }else if(var.type() == QVariant::Double){
        qDebug() << "mdtDeviceIos::updateAnalogInputValues() , have value type double: " << var;
        ai->setValue(var.toDouble(), true);
      }else if(var.type() == QVariant::Int){
        qDebug() << "mdtDeviceIos::updateAnalogInputValues() , have value type int: " << var;
        ai->setValueInt(var.toInt(), true);
      }else{
        ai->setValue(0.0, false);
      }
    }
  }
}

void mdtDeviceIos::updateAnalogInputValues(const QMap<int, QVariant> &values)
{
  QMapIterator<int, QVariant> it(values);
  QVariant var;
  mdtAnalogIo *ai;

  while(it.hasNext()){
    it.next();
    // Get I/O and store value
    ai = analogInputAt(it.key());
    if(ai != 0){
      var = it.value();
      if(!var.isValid()){
        ai->setValue(0.0, false);
      }else if(var.type() == QVariant::Double){
        ai->setValue(var.toDouble(), true);
      }else if(var.type() == QVariant::Int){
        ai->setValueInt(var.toInt(), true);
      }else{
        ai->setValue(0.0, false);
      }
    }
  }
}

void mdtDeviceIos::updateAnalogOutputValue(int address, double value, bool isValid)
{
  mdtAnalogIo *ao;

  ao = analogOutputAt(address);
  if(ao == 0){
    return;
  }
  ao->setValue(value, isValid);
}

void mdtDeviceIos::updateDigitalInputState(int address, double on, bool isValid)
{
  mdtDigitalIo *di;

  di = digitalInputAt(address);
  if(di == 0){
    return;
  }
  di->setOn(on, isValid);
}

void mdtDeviceIos::updateDigitalOutputState(int address, double on, bool isValid)
{
  mdtDigitalIo *dout;

  dout = digitalOutputAt(address);
  if(dout == 0){
    return;
  }
  dout->setOn(on, isValid);
}
