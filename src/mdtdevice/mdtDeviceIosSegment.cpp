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

#include <QDebug>

mdtDeviceIosSegment::mdtDeviceIosSegment()
{
  pvStartAddressRead = 0;
  pvStartAddressWrite = 0;
  pvEndAddressRead = 0;
  pvEndAddressWrite = 0;
}

mdtDeviceIosSegment::~mdtDeviceIosSegment()
{
}

void mdtDeviceIosSegment::setIos(const QList<mdtAnalogIo*> & ios)
{
  int i;
  mdtAnalogIo *io;

  pvValues.clear();
  for(i = 0; i < ios.size(); ++i){
    io = ios.at(i);
    Q_ASSERT(io != 0);
    if(i == 0){
      pvStartAddressRead = io->addressRead();
      pvStartAddressWrite = io->addressWrite();
    }
    pvValues.append(io->value());
  }
  pvEndAddressRead = pvStartAddressRead + pvValues.size() - 1;
  pvEndAddressWrite = pvStartAddressWrite + pvValues.size() - 1;
  // some checks
  Q_ASSERT(pvValues.size() == ios.size());
  if(i > 0){
    // Check address
    Q_ASSERT(pvStartAddressRead >= 0);
    Q_ASSERT(pvStartAddressWrite >= 0);
    Q_ASSERT(pvEndAddressRead >= 0);
    Q_ASSERT(pvEndAddressWrite >= 0);
    Q_ASSERT(pvEndAddressRead >= pvStartAddressRead);
    Q_ASSERT(pvEndAddressWrite >= pvStartAddressWrite);
    Q_ASSERT(pvStartAddressRead == ios.at(0)->addressRead());
    Q_ASSERT(pvStartAddressWrite == ios.at(0)->addressWrite());
    Q_ASSERT(pvEndAddressRead == ios.at(pvValues.size()-1)->addressRead());
    Q_ASSERT(pvEndAddressWrite == ios.at(pvValues.size()-1)->addressWrite());
    // Check value
    Q_ASSERT(pvValues.at(0) == ios.at(0)->value());
  }
}

void mdtDeviceIosSegment::setIos(const QList<mdtDigitalIo*> & ios)
{
  int i;
  mdtDigitalIo *io;

  pvValues.clear();
  for(i = 0; i < ios.size(); ++i){
    io = ios.at(i);
    Q_ASSERT(io != 0);
    if(i == 0){
      pvStartAddressRead = io->addressRead();
      pvStartAddressWrite = io->addressWrite();
    }
    pvValues.append(io->value());
  }
  pvEndAddressRead = pvStartAddressRead + pvValues.size() - 1;
  pvEndAddressWrite = pvStartAddressWrite + pvValues.size() - 1;
  // some checks
  Q_ASSERT(pvValues.size() == ios.size());
  if(i > 0){
    // Check address
    Q_ASSERT(pvStartAddressRead >= 0);
    Q_ASSERT(pvStartAddressWrite >= 0);
    Q_ASSERT(pvEndAddressRead >= 0);
    Q_ASSERT(pvEndAddressWrite >= 0);
    Q_ASSERT(pvEndAddressRead >= pvStartAddressRead);
    Q_ASSERT(pvEndAddressWrite >= pvStartAddressWrite);
    Q_ASSERT(pvStartAddressRead == ios.at(0)->addressRead());
    Q_ASSERT(pvStartAddressWrite == ios.at(0)->addressWrite());
    Q_ASSERT(pvEndAddressRead == ios.at(pvValues.size()-1)->addressRead());
    Q_ASSERT(pvEndAddressWrite == ios.at(pvValues.size()-1)->addressWrite());
    // Check value
    Q_ASSERT(pvValues.at(0) == ios.at(0)->value());
  }
}

int mdtDeviceIosSegment::startAddressRead() const
{
  return pvStartAddressRead;
}

int mdtDeviceIosSegment::startAddressWrite() const
{
  return pvStartAddressWrite;
}

int mdtDeviceIosSegment::endAddressRead() const
{
  return pvEndAddressRead;
}

int mdtDeviceIosSegment::endAddressWrite() const
{
  return pvEndAddressWrite;
}

int mdtDeviceIosSegment::ioCount() const
{
  return pvValues.size();
}

QList<int> mdtDeviceIosSegment::addressesRead() const
{
  QList<int> addressList;
  int address;
  int i;

  address = pvStartAddressRead;
  for(i = 0; i < pvValues.size(); ++i){
    addressList.append(address);
    ++address;
  }
  Q_ASSERT(addressList.size() == pvValues.size());
  Q_ASSERT(addressList.at(pvValues.size()-1) == pvEndAddressRead);

  return addressList;
}

QList<int> mdtDeviceIosSegment::addressesWrite() const
{
  QList<int> addressList;
  int address;
  int i;

  address = pvStartAddressWrite;
  for(i = 0; i < pvValues.size(); ++i){
    addressList.append(address);
    ++address;
  }
  Q_ASSERT(addressList.size() == pvValues.size());
  Q_ASSERT(addressList.at(pvValues.size()-1) == pvEndAddressWrite);

  return addressList;
}

bool mdtDeviceIosSegment::setValues(const QList<mdtValue> & values)
{
}

bool mdtDeviceIosSegment::setValues(const QVariant & values)
{
}

const QList<mdtValue> &mdtDeviceIosSegment::values() const
{
  return pvValues;
}

QList<int> mdtDeviceIosSegment::valuesInt() const
{
  QList<int> valuesList;
  int i;

  for(i = 0; i < pvValues.size(); ++i){
    valuesList.append(pvValues.at(i).valueInt());
  }

  return valuesList;
}

QList<double> mdtDeviceIosSegment::valuesDouble() const
{
  QList<double> valuesList;
  int i;

  for(i = 0; i < pvValues.size(); ++i){
    valuesList.append(pvValues.at(i).valueDouble());
  }

  return valuesList;
}

QList<bool> mdtDeviceIosSegment::valuesBool() const
{
  QList<bool> valuesList;
  int i;

  for(i = 0; i < pvValues.size(); ++i){
    valuesList.append(pvValues.at(i).valueBool());
  }

  return valuesList;
}

#ifdef QT_DEBUG
/**
void mdtDeviceIosSegment::debugCheckIosList(const QList<mdtAbstractIo*> lst)
{
}
*/
#endif  // #ifdef QT_DEBUG
