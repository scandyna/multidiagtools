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
#include "mdtTtTestNodeManager.h"
#include "mdtTtTest.h"

#include <QDebug>

mdtTtTestNodeManager::mdtTtTestNodeManager(QObject* parent, QSqlDatabase db)
 : QObject(parent), pvDevices(new mdtDeviceContainer(this))
{
  pvDatabase = db;
}

QList<std::shared_ptr<mdtDevice> > mdtTtTestNodeManager::allDevices()
{
  return pvDevices->allDevices();
}

std::shared_ptr< mdtDeviceContainer > mdtTtTestNodeManager::container()
{
  return pvDevices;
}

void mdtTtTestNodeManager::clear()
{
  pvDevices->clear();
  ///emit cleared();
}

/**
mdtTtTestItemNodeSetupData mdtTtTestNodeManager::getSetupData(const QVariant & testItemId, bool & ok)
{
  mdtTtTest test(0, pvDatabase);
  mdtTtTestItemNodeSetupData data;

  data = test.getSetupData(testItemId, ok);
  if(!ok){
    pvLastError = test.lastError();
  }

  return data;
}
*/
