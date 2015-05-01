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
#include "mdtDeviceContainer.h"
#include "mdtDevice.h"
#include "mdtDeviceU3606A.h"
#include <QObject>

#include <QDebug>

using namespace std;

/*
 * mdtDeviceContainer implementation
 */

mdtDeviceContainer::mdtDeviceContainer(QObject *parent)
 : QObject(parent)
{
}

void mdtDeviceContainer::clear()
{
  pvDevices.clear();
  emit cleared();
}

QList<shared_ptr<mdtDevice>> mdtDeviceContainer::allDevices()
{
  QList<shared_ptr<mdtDevice>> lst;

  for(auto & dev : pvDevices){
    Q_ASSERT(dev);
    lst.append(dev);
  }

  return lst;
}

void mdtDeviceContainer::disconnectFromDevices()
{
  for(auto & dev : pvDevices){
    Q_ASSERT(dev);
    dev->disconnectFromDevice();
  }
}

/*
 * mdtGlobalDeviceContainer implementation
 */

std::shared_ptr<mdtDeviceContainer> mdtGlobalDeviceContainer::pvContainer;
atomic<int> mdtGlobalDeviceContainer::pvInstanceCount;

mdtGlobalDeviceContainer::mdtGlobalDeviceContainer()
{
  qDebug() << "Global DEV containre + , uses: " << pvContainer.use_count();
  qDebug() << "Global DEV containre + , instances: " << pvInstanceCount;
  ++pvInstanceCount;
  if(!pvContainer){
    pvContainer.reset(new mdtDeviceContainer);
  }
  
}

mdtGlobalDeviceContainer::~mdtGlobalDeviceContainer()
{
  qDebug() << "Global DEV containre - , uses: " << pvContainer.use_count();
  qDebug() << "Global DEV containre - , instances: " << pvInstanceCount;
  --pvInstanceCount;
  // If current object is the last that uses global container, we delete it
  if( (pvContainer) && (pvInstanceCount == 0) ){
    pvContainer->clear();
    pvContainer.reset();
  }
}

// void mdtGlobalDeviceContainer::clear()
// {
//   if(pvContainer){
//     pvContainer->clear();
//     pvContainer.reset();
//   }
// }
