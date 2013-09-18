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
#include "mdtDeviceIosSegment.h"
#include "mdtAbstractIo.h"
#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"

mdtDeviceIosSegment::mdtDeviceIosSegment()
{
}

mdtDeviceIosSegment::~mdtDeviceIosSegment()
{
}

void mdtDeviceIosSegment::setIos(const QList<mdtAnalogIo*> & ios)
{
  int i;

  // We copy pointers, because direct QList<T1*> QList<T2*> is not possible
  pvIos.clear();
  for(i = 0; i < ios.size(); ++i){
    Q_ASSERT(ios.at(i) != 0);
    pvIos.append(ios.at(i));
  }
  Q_ASSERT(pvIos.size() == ios.size());
  if(i > 0){
    Q_ASSERT(pvIos.at(pvIos.size()-1)->addressRead() >= pvIos.at(0)->addressRead());
    Q_ASSERT(pvIos.at(pvIos.size()-1)->addressWrite() >= pvIos.at(0)->addressWrite());
  }
}

void mdtDeviceIosSegment::setIos(const QList<mdtDigitalIo*> & ios)
{
  int i;

  // We copy pointers, because direct QList<T1*> QList<T2*> is not possible
  pvIos.clear();
  for(i = 0; i < ios.size(); ++i){
    Q_ASSERT(ios.at(i) != 0);
    pvIos.append(ios.at(i));
  }
  Q_ASSERT(pvIos.size() == ios.size());
  if(i > 0){
    Q_ASSERT(pvIos.at(pvIos.size()-1)->addressRead() >= pvIos.at(0)->addressRead());
    Q_ASSERT(pvIos.at(pvIos.size()-1)->addressWrite() >= pvIos.at(0)->addressWrite());
  }
}

int mdtDeviceIosSegment::startAddressRead() const
{
  if(pvIos.isEmpty()){
    return 0;
  }
  Q_ASSERT(pvIos.at(0) != 0);
  return pvIos.at(0)->addressRead();
}

int mdtDeviceIosSegment::startAddressWrite() const
{
  if(pvIos.isEmpty()){
    return 0;
  }
  Q_ASSERT(pvIos.at(0) != 0);
  return pvIos.at(0)->addressWrite();
}

int mdtDeviceIosSegment::endAddressRead() const
{
  if(pvIos.isEmpty()){
    return 0;
  }
  Q_ASSERT(pvIos.at(pvIos.size()-1) != 0);
  return pvIos.at(pvIos.size()-1)->addressRead();
}

int mdtDeviceIosSegment::endAddressWrite() const
{
  if(pvIos.isEmpty()){
    return 0;
  }
  Q_ASSERT(pvIos.at(pvIos.size()-1) != 0);
  return pvIos.at(pvIos.size()-1)->addressWrite();
}

int mdtDeviceIosSegment::ioCount() const
{
  return pvIos.size();
}

bool mdtDeviceIosSegment::containsAddressRead(int address) const
{
  int firstAddress, lastAddress;

  if(pvIos.isEmpty()){
    return false;
  }
  Q_ASSERT(pvIos.at(0) != 0);
  firstAddress = pvIos.at(0)->addressRead();
  Q_ASSERT(pvIos.at(pvIos.size()-1) != 0);
  lastAddress = pvIos.at(pvIos.size()-1)->addressRead();

  return ((address >= firstAddress)&&(address <= lastAddress));
}

QList<int> mdtDeviceIosSegment::addressesRead() const
{
  QList<int> addressList;
  int address;
  int i;

  if(pvIos.isEmpty()){
    return addressList;
  }
  address = pvIos.at(0)->addressRead();
  for(i = 0; i < pvIos.size(); ++i){
    addressList.append(address);
    ++address;
  }
  Q_ASSERT(addressList.size() == pvIos.size());

  return addressList;
}

bool mdtDeviceIosSegment::containsAddressWrite(int address) const
{
  int firstAddress, lastAddress;

  if(pvIos.isEmpty()){
    return false;
  }
  Q_ASSERT(pvIos.at(0) != 0);
  firstAddress = pvIos.at(0)->addressWrite();
  Q_ASSERT(pvIos.at(pvIos.size()-1) != 0);
  lastAddress = pvIos.at(pvIos.size()-1)->addressWrite();

  return ((address >= firstAddress)&&(address <= lastAddress));
}

QList<int> mdtDeviceIosSegment::addressesWrite() const
{
  QList<int> addressList;
  int address;
  int i;

  if(pvIos.isEmpty()){
    return addressList;
  }
  address = pvIos.at(0)->addressWrite();
  for(i = 0; i < pvIos.size(); ++i){
    addressList.append(address);
    ++address;
  }
  Q_ASSERT(addressList.size() == pvIos.size());

  return addressList;
}

int mdtDeviceIosSegment::setValues(const QList<mdtValue> & values, int max)
{
  int i;

  // Define max
  if(max < 0){
    max = values.size();
  }
  max = qMin(max, pvIos.size());

  for(i = 0; i < max; ++i){
    Q_ASSERT(pvIos.at(i) != 0);
    pvIos.at(i)->setValue(values.at(i));
  }

  return max;
}

int mdtDeviceIosSegment::setValues(const QList<QVariant> & values, int max)
{
  int i;
  QVariant var;

  // Define max
  if(max < 0){
    max = values.size();
  }
  max = qMin(max, pvIos.size());

  for(i = 0; i < max; ++i){
    Q_ASSERT(pvIos.at(i) != 0);
    var = values.at(i);
    switch(var.type()){
      case QVariant::Int:
        pvIos.at(i)->setValue(var.toInt());
        break;
      case QVariant::Double:
        pvIos.at(i)->setValue(var.toDouble());
        break;
      case QVariant::Bool:
        pvIos.at(i)->setValue(var.toBool());
        break;
      default:
        pvIos.at(i)->setValue(mdtValue());
    }
  }

  return max;
}

int mdtDeviceIosSegment::updateValuesFromAddressRead(int startAddress, const QList<mdtValue> & values, int max)
{
  int i, j;
  bool found = false;

  // Find I/O with startAddress
  for(i = 0; i < pvIos.size(); ++i){
    Q_ASSERT(pvIos.at(i) != 0);
    if(pvIos.at(i)->addressRead() == startAddress){
      found = true;
      break;
    }
  }
  if(!found){
    return 0;
  }
  // Update values
  if(max < 0){
    max = values.size();
  }else{
    max = qMin(max, values.size());
  }
  for(j = 0; j < max; ++j){
    if(i >= pvIos.size()){
      return j;
    }
    Q_ASSERT(pvIos.at(i) != 0);
    pvIos.at(i)->setValue(values.at(j));
    ++i;
  }

  return j;
}

int mdtDeviceIosSegment::updateValuesFromAddressRead(int startAddress, const QList<QVariant> & values, int max)
{
  int i, j;
  bool found = false;
  QVariant var;

  // Find I/O with startAddress
  for(i = 0; i < pvIos.size(); ++i){
    Q_ASSERT(pvIos.at(i) != 0);
    if(pvIos.at(i)->addressRead() == startAddress){
      found = true;
      break;
    }
  }
  if(!found){
    return 0;
  }
  // Update values
  if(max < 0){
    max = values.size();
  }else{
    max = qMin(max, values.size());
  }
  for(j = 0; j < max; ++j){
    if(i >= pvIos.size()){
      return j;
    }
    Q_ASSERT(pvIos.at(i) != 0);
    var = values.at(j);
    switch(var.type()){
      case QVariant::Int:
        pvIos.at(i)->setValue(var.toInt());
        break;
      case QVariant::Double:
        pvIos.at(i)->setValue(var.toDouble());
        break;
      case QVariant::Bool:
        pvIos.at(i)->setValue(var.toBool());
        break;
      default:
        pvIos.at(i)->setValue(mdtValue());
    }
    ++i;
  }

  return j;
}

int mdtDeviceIosSegment::updateValuesFromAddressWrite(int startAddress, const QList<mdtValue> & values, int max)
{
  int i, j;
  bool found = false;

  // Find I/O with startAddress
  for(i = 0; i < pvIos.size(); ++i){
    Q_ASSERT(pvIos.at(i) != 0);
    if(pvIos.at(i)->addressWrite() == startAddress){
      found = true;
      break;
    }
  }
  if(!found){
    return 0;
  }
  // Update values
  if(max < 0){
    max = values.size();
  }else{
    max = qMin(max, values.size());
  }
  for(j = 0; j < max; ++j){
    if(i >= pvIos.size()){
      return j;
    }
    Q_ASSERT(pvIos.at(i) != 0);
    pvIos.at(i)->setValue(values.at(j));
    ++i;
  }

  return j;
}

int mdtDeviceIosSegment::updateValuesFromAddressWrite(int startAddress, const QList<QVariant> & values, int max)
{
  int i, j;
  bool found = false;
  QVariant var;

  // Find I/O with startAddress
  for(i = 0; i < pvIos.size(); ++i){
    Q_ASSERT(pvIos.at(i) != 0);
    if(pvIos.at(i)->addressWrite() == startAddress){
      found = true;
      break;
    }
  }
  if(!found){
    return 0;
  }
  // Update values
  if(max < 0){
    max = values.size();
  }else{
    max = qMin(max, values.size());
  }
  for(j = 0; j < max; ++j){
    if(i >= pvIos.size()){
      return j;
    }
    Q_ASSERT(pvIos.at(i) != 0);
    var = values.at(j);
    switch(var.type()){
      case QVariant::Int:
        pvIos.at(i)->setValue(var.toInt());
        break;
      case QVariant::Double:
        pvIos.at(i)->setValue(var.toDouble());
        break;
      case QVariant::Bool:
        pvIos.at(i)->setValue(var.toBool());
        break;
      default:
        pvIos.at(i)->setValue(mdtValue());
    }
    ++i;
  }

  return j;
}

QList<mdtValue> mdtDeviceIosSegment::values() const
{
  QList<mdtValue> valuesList;
  int i;

  for(i = 0; i < pvIos.size(); ++i){
    Q_ASSERT(pvIos.at(i) != 0);
    valuesList.append(pvIos.at(i)->value());
  }

  return valuesList;
}

QList<int> mdtDeviceIosSegment::valuesInt() const
{
  QList<int> valuesList;
  int i;

  for(i = 0; i < pvIos.size(); ++i){
    Q_ASSERT(pvIos.at(i) != 0);
    valuesList.append(pvIos.at(i)->value().valueInt());
  }

  return valuesList;
}

QList<double> mdtDeviceIosSegment::valuesDouble() const
{
  QList<double> valuesList;
  int i;

  for(i = 0; i < pvIos.size(); ++i){
    Q_ASSERT(pvIos.at(i) != 0);
    valuesList.append(pvIos.at(i)->value().valueDouble());
  }

  return valuesList;
}

QList<bool> mdtDeviceIosSegment::valuesBool() const
{
  QList<bool> valuesList;
  int i;

  for(i = 0; i < pvIos.size(); ++i){
    Q_ASSERT(pvIos.at(i) != 0);
    valuesList.append(pvIos.at(i)->value().valueBool());
  }

  return valuesList;
}
