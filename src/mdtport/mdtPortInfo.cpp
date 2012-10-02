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
#include "mdtPortInfo.h"
#include "mdtDeviceInfo.h"

mdtPortInfo::mdtPortInfo()
{
}

mdtPortInfo::mdtPortInfo(const mdtPortInfo &other)
{
  int i;

  pvPortName = other.pvPortName;
  // Copy devices list
  for(i=0; i<other.pvDeviceInfoList.size(); ++i){
    pvDeviceInfoList.append(new mdtDeviceInfo(*other.pvDeviceInfoList.at(i)));
  }
}

mdtPortInfo::~mdtPortInfo()
{
  qDeleteAll(pvDeviceInfoList);
}

void mdtPortInfo::setPortName(const QString &name)
{
  pvPortName = name;
}

QString mdtPortInfo::portName() const
{
  return pvPortName;
}

void mdtPortInfo::addDevice(mdtDeviceInfo *device)
{
  pvDeviceInfoList.append(device);
}

QList<mdtDeviceInfo*> &mdtPortInfo::deviceInfoList()
{
  return pvDeviceInfoList;
}

mdtPortInfo &mdtPortInfo::operator=(const mdtPortInfo &other)
{
  int i;

  if(this != &other){
    pvPortName = other.pvPortName;
    // Copy devices list
    qDeleteAll(pvDeviceInfoList);
    for(i=0; i<other.pvDeviceInfoList.size(); ++i){
      pvDeviceInfoList.append(new mdtDeviceInfo(*other.pvDeviceInfoList.at(i)));
    }
  }

  return *this;
}

bool mdtPortInfo::operator==(const mdtPortInfo &other)
{
  int i;

  if(pvPortName != other.pvPortName){
    return false;
  }
  // Check device info list
  if(pvDeviceInfoList.size() != other.pvDeviceInfoList.size()){
    return false;
  }
  for(i=0; i<pvDeviceInfoList.size(); ++i){
    Q_ASSERT(pvDeviceInfoList.at(i) != 0);
    Q_ASSERT(other.pvDeviceInfoList.at(i) != 0);
    if((*pvDeviceInfoList.at(i)) != (*other.pvDeviceInfoList.at(i))){
      return false;
    }
  }

  return true;
}

bool mdtPortInfo::operator!=(const mdtPortInfo &other)
{
  return !(*this == other);
}
