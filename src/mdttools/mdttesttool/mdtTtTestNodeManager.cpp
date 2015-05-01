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
#include <QSqlRecord>

#include <QDebug>

using namespace std;

mdtTtTestNodeManager::mdtTtTestNodeManager(QObject* parent, QSqlDatabase db)
 : QObject(parent),
   pvDatabase(db)
{
}

mdtTtTestNodeManager::mdtTtTestNodeManager(QSqlDatabase db)
 : QObject(nullptr),
   pvDatabase(db)
{
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

bool mdtTtTestNodeManager::setDeviceIosLabelShort(const QString & alias, bool failOnIoMismatch)
{
  mdtTtTestNode tn(0, pvDatabase);
  QVariant testNodeId;

  // Get test node ID
  testNodeId = tn.getTestNodeIdForAlias(alias);
  if(testNodeId.isNull()){
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
  if(!setAnalogInputsLabelShort(dev->ios(), testNodeId, alias, failOnIoMismatch)){
    return false;
  }
  // Set analog outputs labels
  if(!setAnalogOutputsLabelShort(dev->ios(), testNodeId, alias, failOnIoMismatch)){
    return false;
  }
  // Set digital inputs labels
  if(!setDigitalInputsLabelShort(dev->ios(), testNodeId, alias, failOnIoMismatch)){
    return false;
  }
  // Set digital outputs labels
  if(!setDigitalOutputsLabelShort(dev->ios(), testNodeId, alias, failOnIoMismatch)){
    return false;
  }

  return true;
}

/// \todo failOnIoMismatch implemented wrong
bool mdtTtTestNodeManager::setAnalogInputsLabelShort(shared_ptr< mdtDeviceIos > ios, const QVariant& testNodeId, const QString & alias, bool failOnIoMismatch)
{
  Q_ASSERT(ios);

  mdtTtTestNode tn(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dbIoList;
  int dbIoPos;
  mdtAbstractIo *io;
  bool ok;

  // Get test schema position of each node units that are digital outputs - Order by IoPosition Ascending
  sql = "SELECT SchemaPosition, IoPosition FROM TestNodeUnit_view WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND (Type_Code_FK = 'AI')";
  sql += " AND IoPosition IS NOT NULL";
  dbIoList = tn.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    pvLastError = tn.lastError();
    return false;
  }
  // Update I/O labels
  for(const auto & dbIo : dbIoList){
    // Get database I/O position
    dbIoPos = dbIo.value(1).toInt(&ok);
    if(!ok){
      pvLastError.setError(tr("Device '") + alias + tr("' could not get I/O position for '") + dbIo.value(0).toString(), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
      pvLastError.commit();
      return false;
    }
    Q_ASSERT(dbIoPos >= 0);
    // Update label
    if(ios->analogInputsCount() > dbIoPos){
      io = ios->analogInputs().at(dbIoPos);
      Q_ASSERT(io != nullptr);
      io->setLabelShort(dbIo.value(0).toString());
    }else{
      if(failOnIoMismatch){
        pvLastError.setError(tr("Device '") + alias + tr("' I/O position of '") + dbIo.value(0).toString() + tr("' not exists on device."), mdtError::Error);
        MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
        pvLastError.commit();
        return false;
      }
    }
  }

  return true;
}

/// \todo failOnIoMismatch implemented wrong
bool mdtTtTestNodeManager::setAnalogOutputsLabelShort(shared_ptr< mdtDeviceIos > ios, const QVariant& testNodeId, const QString & alias, bool failOnIoMismatch)
{
  Q_ASSERT(ios);

  mdtTtTestNode tn(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dbIoList;
  int dbIoPos;
  mdtAbstractIo *io;
  bool ok;

  // Get test schema position of each node units that are digital outputs - Order by IoPosition Ascending
  sql = "SELECT SchemaPosition, IoPosition FROM TestNodeUnit_view WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND (Type_Code_FK = 'AO')";
  sql += " AND IoPosition IS NOT NULL";
  dbIoList = tn.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    pvLastError = tn.lastError();
    return false;
  }
  // Update I/O labels
  for(const auto & dbIo : dbIoList){
    // Get database I/O position
    dbIoPos = dbIo.value(1).toInt(&ok);
    if(!ok){
      pvLastError.setError(tr("Device '") + alias + tr("' could not get I/O position for '") + dbIo.value(0).toString(), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
      pvLastError.commit();
      return false;
    }
    Q_ASSERT(dbIoPos >= 0);
    // Update label
    if(ios->analogOutputsCount() > dbIoPos){
      io = ios->analogOutputs().at(dbIoPos);
      Q_ASSERT(io != nullptr);
      io->setLabelShort(dbIo.value(0).toString());
    }else{
      if(failOnIoMismatch){
        pvLastError.setError(tr("Device '") + alias + tr("' I/O position of '") + dbIo.value(0).toString() + tr("' not exists on device."), mdtError::Error);
        MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
        pvLastError.commit();
        return false;
      }
    }
  }

  return true;
}

/// \todo failOnIoMismatch implemented wrong
bool mdtTtTestNodeManager::setDigitalInputsLabelShort(std::shared_ptr<mdtDeviceIos> ios, const QVariant& testNodeId, const QString & alias, bool failOnIoMismatch)
{
  Q_ASSERT(ios);

  mdtTtTestNode tn(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dbIoList;
  int dbIoPos;
  mdtAbstractIo *io;
  bool ok;

  // Get test schema position of each node units that are digital outputs - Order by IoPosition Ascending
  sql = "SELECT SchemaPosition, IoPosition FROM TestNodeUnit_view WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND (Type_Code_FK = 'DI')";
  sql += " AND IoPosition IS NOT NULL";
  dbIoList = tn.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    pvLastError = tn.lastError();
    return false;
  }
  // Update I/O labels
  for(const auto & dbIo : dbIoList){
    // Get database I/O position
    dbIoPos = dbIo.value(1).toInt(&ok);
    if(!ok){
      pvLastError.setError(tr("Device '") + alias + tr("' could not get I/O position for '") + dbIo.value(0).toString(), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
      pvLastError.commit();
      return false;
    }
    Q_ASSERT(dbIoPos >= 0);
    // Update label
    if(ios->digitalInputsCount() > dbIoPos){
      io = ios->digitalInputs().at(dbIoPos);
      Q_ASSERT(io != nullptr);
      io->setLabelShort(dbIo.value(0).toString());
    }else{
      if(failOnIoMismatch){
        pvLastError.setError(tr("Device '") + alias + tr("' I/O position of '") + dbIo.value(0).toString() + tr("' not exists on device."), mdtError::Error);
        MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
        pvLastError.commit();
        return false;
      }
    }
  }

  return true;
}

/// \todo failOnIoMismatch implemented wrong
bool mdtTtTestNodeManager::setDigitalOutputsLabelShort(std::shared_ptr<mdtDeviceIos> ios, const QVariant& testNodeId, const QString & alias, bool failOnIoMismatch)
{
  Q_ASSERT(ios);

  mdtTtTestNode tn(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dbIoList;
  int dbIoPos;
  mdtAbstractIo *io;
  bool ok;

  // Get test schema position of each node units that are digital outputs - Order by IoPosition Ascending
  sql = "SELECT SchemaPosition, IoPosition FROM TestNodeUnit_view WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND (Type_Code_FK = 'BUSCPLREL' OR Type_Code_FK = 'CHANELREL')";
  sql += " AND IoPosition IS NOT NULL";
  dbIoList = tn.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    pvLastError = tn.lastError();
    return false;
  }
  // Update I/O labels
  for(const auto & dbIo : dbIoList){
    // Get database I/O position
    dbIoPos = dbIo.value(1).toInt(&ok);
    if(!ok){
      pvLastError.setError(tr("Device '") + alias + tr("' could not get I/O position for '") + dbIo.value(0).toString(), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
      pvLastError.commit();
      return false;
    }
    Q_ASSERT(dbIoPos >= 0);
    // Update label
    if(ios->digitalOutputsCount() > dbIoPos){
      io = ios->digitalOutputs().at(dbIoPos);
      Q_ASSERT(io != nullptr);
      io->setLabelShort(dbIo.value(0).toString());
    }else{
      if(failOnIoMismatch){
        pvLastError.setError(tr("Device '") + alias + tr("' I/O position of '") + dbIo.value(0).toString() + tr("' not exists on device."), mdtError::Error);
        MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
        pvLastError.commit();
        return false;
      }
    }
  }

  return true;
}
