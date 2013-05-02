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
#include <QMutableListIterator>
#include <QtGlobal>

#include <QDebug>

mdtDeviceIos::mdtDeviceIos(QObject *parent)
 : QObject(parent)
{
  pvAutoDeleteIos = true;
  pvAnalogInputsFirstAddressRead = 0;
  pvAnalogOutputsFirstAddressRead = 0;
  pvAnalogOutputsFirstAddressWrite = 0;
  pvDigitalInputsFirstAddressRead = 0;
  pvDigitalOutputsFirstAddressRead = 0;
  pvDigitalOutputsFirstAddressWrite = 0;
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
  pvAnalogInputsFirstAddressRead = 0;
  qDeleteAll(pvAnalogOutputs);
  pvAnalogOutputs.clear();
  pvAnalogOutputsByAddressRead.clear();
  pvAnalogOutputsByAddressWrite.clear();
  pvAnalogOutputsFirstAddressRead = 0;
  pvAnalogOutputsFirstAddressWrite = 0;
  qDeleteAll(pvDigitalInputs);
  pvDigitalInputs.clear();
  pvDigitalInputsByAddressRead.clear();
  pvDigitalInputsFirstAddressRead = 0;
  qDeleteAll(pvDigitalOutputs);
  pvDigitalOutputs.clear();
  pvDigitalOutputsByAddressRead.clear();
  pvDigitalOutputsByAddressWrite.clear();
  pvDigitalOutputsFirstAddressRead = 0;
  pvDigitalOutputsFirstAddressWrite = 0;
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

  return 0;
}

const QList<mdtAnalogIo*> mdtDeviceIos::analogInputs() const
{
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

void mdtDeviceIos::setAnalogInputsValue(const mdtValue &value)
{
  int i;

  for(i=0; i<pvAnalogInputs.size(); ++i){
    Q_ASSERT(pvAnalogInputs.at(i) != 0);
    pvAnalogInputs.at(i)->setValue(value);
  }
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
  pvAnalogOutputsFirstAddressWrite = pvAnalogOutputsByAddressWrite.values().at(0)->addressWrite();
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
    values.append(aos.at(i)->value().valueInt());
  }

  return values;
}

void mdtDeviceIos::setAnalogOutputsValue(const mdtValue &value)
{
  int i;

  for(i=0; i<pvAnalogOutputs.size(); ++i){
    Q_ASSERT(pvAnalogOutputs.at(i) != 0);
    pvAnalogOutputs.at(i)->setValue(value, false);
  }
}

void mdtDeviceIos::addDigitalInput(mdtDigitalIo *di)
{
  Q_ASSERT(di != 0);

  pvDigitalInputs.append(di);
  pvDigitalInputsByAddressRead.insert(di->address(), di);
  // QMap returns a list sorted by keys, ascending
  Q_ASSERT(pvDigitalInputsByAddressRead.values().size() > 0);
  Q_ASSERT(pvDigitalInputsByAddressRead.values().at(0) != 0);
  pvDigitalInputsFirstAddressRead = pvDigitalInputsByAddressRead.values().at(0)->addressRead();
}

mdtDigitalIo *mdtDeviceIos::digitalInputAt(int address)
{
  return pvDigitalInputsByAddressRead.value(address, 0);
}

mdtDigitalIo *mdtDeviceIos::digitalInputWithLabelShort(const QString &labelShort)
{
  int i;

  for(i=0; i<pvDigitalInputs.size(); ++i){
    Q_ASSERT(pvDigitalInputs.at(i) != 0);
    if(pvDigitalInputs.at(i)->labelShort() == labelShort){
      return pvDigitalInputs.at(i);
    }
  }

  return 0;
}

const QList<mdtDigitalIo*> mdtDeviceIos::digitalInputs() const
{
  return pvDigitalInputs;
}

int mdtDeviceIos::digitalInputsFirstAddress() const
{
  return pvDigitalInputsFirstAddressRead;
}

int mdtDeviceIos::digitalInputsCount() const
{
  return pvDigitalInputs.size();
}

void mdtDeviceIos::setDigitalInputsValue(const mdtValue &value)
{
  int i;

  for(i=0; i<pvDigitalInputs.size(); ++i){
    Q_ASSERT(pvDigitalInputs.at(i) != 0);
    pvDigitalInputs.at(i)->setValue(value);
  }
}

void mdtDeviceIos::addDigitalOutput(mdtDigitalIo *dout)
{
  Q_ASSERT(dout != 0);

  pvDigitalOutputs.append(dout);
  pvDigitalOutputsByAddressRead.insert(dout->addressRead(), dout);
  pvDigitalOutputsByAddressWrite.insert(dout->addressWrite(), dout);
  // Get first addresses for read and write access
  Q_ASSERT(pvDigitalOutputsByAddressRead.values().size() > 0);
  Q_ASSERT(pvDigitalOutputsByAddressRead.values().at(0) != 0);
  // QMap returns a list sorted by keys, ascending
  pvDigitalOutputsFirstAddressRead = pvDigitalOutputsByAddressRead.values().at(0)->addressRead();
  Q_ASSERT(pvDigitalOutputsByAddressWrite.values().size() > 0);
  Q_ASSERT(pvDigitalOutputsByAddressWrite.values().at(0) != 0);
  // QMap returns a list sorted by keys, ascending
  pvDigitalOutputsFirstAddressWrite = pvDigitalOutputsByAddressWrite.values().at(0)->addressWrite();
}

mdtDigitalIo *mdtDeviceIos::digitalOutputAtAddressRead(int address)
{
  return pvDigitalOutputsByAddressRead.value(address, 0);
}

mdtDigitalIo *mdtDeviceIos::digitalOutputAtAddressWrite(int address)
{
  return pvDigitalOutputsByAddressWrite.value(address, 0);
}

mdtDigitalIo *mdtDeviceIos::digitalOutputWithLabelShort(const QString &labelShort)
{
  int i;

  for(i=0; i<pvDigitalOutputs.size(); ++i){
    Q_ASSERT(pvDigitalOutputs.at(i) != 0);
    if(pvDigitalOutputs.at(i)->labelShort() == labelShort){
      return pvDigitalOutputs.at(i);
    }
  }

  return 0;
}

const QList<mdtDigitalIo*> mdtDeviceIos::digitalOutputs() const
{
  return pvDigitalOutputs;
}

int mdtDeviceIos::digitalOutputsFirstAddressRead() const
{
  return pvDigitalOutputsFirstAddressRead;
}

int mdtDeviceIos::digitalOutputsFirstAddressWrite() const
{
  return pvDigitalOutputsFirstAddressWrite;
}

int mdtDeviceIos::digitalOutputsCount() const
{
  return pvDigitalOutputs.size();
}

void mdtDeviceIos::setDigitalOutputsValue(const mdtValue &value)
{
  int i;

  for(i=0; i<pvDigitalOutputs.size(); ++i){
    Q_ASSERT(pvDigitalOutputs.at(i) != 0);
    pvDigitalOutputs.at(i)->setValue(value);
  }
}

QList<bool> mdtDeviceIos::digitalOutputsStatesByAddressWrite() const
{
  QList<bool> states;
  QList<mdtDigitalIo*> dos;
  int i;

  dos = pvDigitalOutputsByAddressWrite.values();
  for(i=0; i<dos.size(); i++){
    Q_ASSERT(dos.at(i) != 0);
    states.append(dos.at(i)->value().valueBool());
  }

  return states;
}

void mdtDeviceIos::updateAnalogInputValues(const QList<QVariant> &values)
{
  int i, max;
  QList<mdtAnalogIo*> lst;
  QVariant var;

  // Get the list from address conatiner, so we have it sorted by address (QMap returns a sorted list, by keys, ascending)
  lst = pvAnalogInputsByAddressRead.values();
  max = qMin(values.size(), lst.size());
  for(i=0; i<max; ++i){
    Q_ASSERT(lst.at(i) != 0);
    var = values.at(i);
    switch(var.type()){
      case QVariant::Double:
        lst.at(i)->setValue(var.toDouble(), false);
        break;
      case QVariant::Int:
        lst.at(i)->setValue(var.toInt(), false);
        break;
      case QVariant::Bool:
        lst.at(i)->setValue(var.toBool(), false);
        break;
      default:
        lst.at(i)->setValue(var.value<mdtValue>(), false);
    }
  }
}

void mdtDeviceIos::updateAnalogOutputValues(const QList<QVariant> &values)
{
  int i, max;
  QList<mdtAnalogIo*> lst;
  QVariant var;

  // Get the list from address conatiner, so we have it sorted by address (QMap returns a sorted list, by keys, ascending)
  lst = pvAnalogOutputsByAddressRead.values();  // We update (G)UI, so we read from device
  max = qMin(values.size(), lst.size());
  for(i=0; i<max; ++i){
    Q_ASSERT(lst.at(i) != 0);
    var = values.at(i);
    switch(var.type()){
      case QVariant::Double:
        lst.at(i)->setValue(var.toDouble(), false);
        break;
      case QVariant::Int:
        lst.at(i)->setValue(var.toInt(), false);
        break;
      case QVariant::Bool:
        lst.at(i)->setValue(var.toBool(), false);
        break;
      default:
        lst.at(i)->setValue(var.value<mdtValue>(), false);
    }
  }
}

void mdtDeviceIos::setAnalogOutputsEnabled(bool enabled)
{
  int i;

  for(i=0; i<pvAnalogOutputs.size(); ++i){
    Q_ASSERT(pvAnalogOutputs.at(i) != 0);
    pvAnalogOutputs.at(i)->setEnabled(enabled);
  }
}

void mdtDeviceIos::updateDigitalInputValues(const QList<QVariant> &values)
{
  int i, max;
  QList<mdtDigitalIo*> lst;

  // Get the list from address conatiner, so we have it sorted by address (QMap returns a sorted list, by keys, ascending)
  lst = pvDigitalInputsByAddressRead.values();
  max = qMin(values.size(), lst.size());
  for(i=0; i<max; ++i){
    Q_ASSERT(lst.at(i) != 0);
    lst.at(i)->setValue(values.at(i).toBool(), false);
  }
}

void mdtDeviceIos::updateDigitalOutputValues(const QList<QVariant> &values, int firstAddressRead, int n)
{
  int i, max;
  QList<mdtDigitalIo*> lst;

  // Get the list from address conatiner, so we have it sorted by address (QMap returns a sorted list, by keys, ascending)
  lst = pvDigitalOutputsByAddressRead.values();  // We update (G)UI, so we read from device
  // Remove items with addressRead < firstAddressRead
  if((firstAddressRead > -1)&&(firstAddressRead > digitalOutputsFirstAddressRead())){
    QMutableListIterator<mdtDigitalIo*> it(lst);
    while(it.hasNext()){
      it.next();
      Q_ASSERT(it.value() != 0);
      if(it.value()->addressRead() >= firstAddressRead){
        break;
      }
      it.remove();
    }
  }
  // Fix quantity of outputs and update outputs
  if(n < 0){
    n = lst.size();
  }
  max = qMin(values.size(), n);
  for(i=0; i<max; ++i){
    Q_ASSERT(lst.at(i) != 0);
    if((values.at(i).isValid())&&(values.at(i).type() == QVariant::Bool)){
      lst.at(i)->setValue(values.at(i).toBool(), false);
    }else{
      lst.at(i)->setValue(mdtValue());
    }
  }
}

void mdtDeviceIos::setDigitalOutputsEnabled(bool enabled)
{
  int i;

  for(i=0; i<pvDigitalOutputs.size(); ++i){
    Q_ASSERT(pvDigitalOutputs.at(i) != 0);
    pvDigitalOutputs.at(i)->setEnabled(enabled);
  }
}
