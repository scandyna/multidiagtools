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
#include "mdtTtTestNodeManager.h"
#include "mdtTtTest.h"
#include "mdtTtTestNode.h"
#include "mdtDeviceIos.h"
#include "mdtAbstractIo.h"
#include "mdtMultiIoDevice.h"

#include <QDebug>

using namespace std;

mdtTtTestNodeManager::mdtTtTestNodeManager(QObject* parent, QSqlDatabase db)
 : QObject(parent) /**, pvDevices(new mdtDeviceContainer(this))*/
{
  pvDatabase = db;
}

QList<std::shared_ptr<mdtDevice> > mdtTtTestNodeManager::allDevices()
{
  return pvDevices->allDevices();
}

std::shared_ptr< mdtDeviceContainer > mdtTtTestNodeManager::container()
{
  return pvDevices.container();
}

void mdtTtTestNodeManager::clear()
{
  pvDevices->clear();
}

bool mdtTtTestNodeManager::setDeviceIosLabelShort(const QVariant& testNodeId)
{
  Q_ASSERT(!testNodeId.isNull());

  mdtTtTestNode tn(0, pvDatabase);
  QString alias;
  bool ok;

  // Get test node device alias
  alias = tn.getTestNodeAlias(testNodeId, ok);
  if(!ok){
    pvLastError = tn.lastError();
    return false;
  }
  // Get device
  auto dev = device<mdtMultiIoDevice>(alias);
  if(!dev){
    pvLastError.setError(tr("No multi I/O device found with alias") + " '" + alias + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
    pvLastError.commit();
    return false;
  }
  Q_ASSERT(dev->ios());
  // Set analog inputs labels
  
  // Set analog outputs labels
  
  // Set digital inputs labels
  
  // Set digital outputs labels
  if(!setDigitalOutputsLabelShort(dev->ios(), testNodeId, alias)){
    return false;
  }

  return true;
}

bool mdtTtTestNodeManager::setDigitalOutputsLabelShort(std::shared_ptr<mdtDeviceIos> ios, const QVariant& testNodeId, const QString & alias)
{
  Q_ASSERT(ios);

  mdtTtTestNode tn(0, pvDatabase);
  QString sql;
  QList<QVariant> ioLabelList;
  mdtAbstractIo *io;
  bool ok;
  int i;

  // Get test schema position of each node units that are digital outputs - Order by IoPosition Ascending
  sql = "SELECT SchemaPosition FROM TestNodeUnit_view WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND (Type_Code_FK = 'BUSCPLREL' OR Type_Code_FK = 'CHANELREL')";
  sql += " ORDER BY IoPosition ASC";
  ioLabelList = tn.getDataList<QVariant>(sql, ok);
  if(!ok){
    pvLastError = tn.lastError();
    return false;
  }
  // Check count of digital outputs in ios and count of found test node units
  if(ios->digitalOutputsCount() < ioLabelList.size()){
    pvLastError.setError(tr("Device '") + alias + tr("' has less than expected digital outputs."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
    pvLastError.commit();
    return false;
  }
  // Set label for each digital output
  Q_ASSERT(ios->digitalOutputsCount() >= ioLabelList.size());
  for(i = 0; i < ioLabelList.size(); ++i){
    Q_ASSERT(i < ios->digitalOutputs().size());
    io = ios->digitalOutputs().at(i);
    Q_ASSERT(io != 0);
    io->setLabelShort(ioLabelList.at(i).toString());
  }

  return true;
}
