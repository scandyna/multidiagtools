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
#include "mdtDeviceContainer.h"
#include "mdtDevice.h"
#include "mdtDeviceU3606A.h"
#include <QObject>

//#include <QDebug>

using namespace std;

mdtDeviceContainer::mdtDeviceContainer(QObject *parent)
 : QObject(parent)
{
}

QList<shared_ptr<mdtDevice>> mdtDeviceContainer::allDevices()
{
  QList<shared_ptr<mdtDevice>> lst;
  std::vector<std::shared_ptr<mdtDevice>>::const_iterator it;

  for(it = pvDevices.begin(); it != pvDevices.end(); ++it){
    Q_ASSERT(*it);
    lst.append(*it);
  }

  return lst;
}

void mdtDeviceContainer::clear()
{
  pvDevices.clear();
  emit cleared();
}
