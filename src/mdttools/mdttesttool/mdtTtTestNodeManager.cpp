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

bool mdtTtTestNodeManager::checkDeviceIoMapCoherence(const QString & alias)
{
  mdtTtTestNode tn(0, pvDatabase);
  QVariant testNodeId;

  // Get device
  auto dev = device<mdtMultiIoDevice>(alias);
  if(!dev){
    pvLastError.setError(tr("No multi I/O device found with alias") + " '" + alias + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
    pvLastError.commit();
    return false;
  }
  Q_ASSERT(dev->ios());
  // Get test node ID
  testNodeId = tn.getTestNodeIdForAlias(alias);
  if(testNodeId.isNull()){
    pvLastError = tn.lastError();
    return false;
  }

  return checkDeviceIoMapCoherence(testNodeId, dev);
}

bool mdtTtTestNodeManager::setDeviceIosLabelShort(const QString & alias)
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
  if(!setAnalogInputsLabelShort(dev->ios(), testNodeId, alias)){
    return false;
  }
  // Set analog outputs labels
  if(!setAnalogOutputsLabelShort(dev->ios(), testNodeId, alias)){
    return false;
  }
  // Set digital inputs labels
  if(!setDigitalInputsLabelShort(dev->ios(), testNodeId, alias)){
    return false;
  }
  // Set digital outputs labels
  if(!setDigitalOutputsLabelShort(dev->ios(), testNodeId, alias)){
    return false;
  }

  return true;
}

vector<mdtTtTestNodeRouteRelay> mdtTtTestNodeManager::getTestNodeRouteRelays(const QString & alias, const QStringList & schemaPositionList)
{
  vector<mdtTtTestNodeRouteRelay> relays;
  mdtTtTestNode tn(pvDatabase);
  QVariant testNodeId;

  testNodeId = tn.getTestNodeIdForAlias(alias);
  if(testNodeId.isNull()){
    pvLastError = tn.lastError();
    return relays;
  }
  relays = tn.getTestNodeRouteRelays(testNodeId, schemaPositionList);
  if(relays.empty()){
    pvLastError = tn.lastError();
  }

  return relays;
}

bool mdtTtTestNodeManager::setRelaysToEnable(const vector< mdtTtTestNodeRouteRelay >& relays, shared_ptr<mdtMultiIoDevice> dev)
{
  Q_ASSERT(dev);

  dev->setDigitalOutputsValue(false, false);

  return addRelaysToEnable(relays, dev);
}

bool mdtTtTestNodeManager::addRelaysToEnable(const vector<mdtTtTestNodeRouteRelay> & relays, shared_ptr<mdtMultiIoDevice> dev)
{
  Q_ASSERT(dev);

  for(const auto & relay : relays){
    if(!dev->setDigitalOutputValueAt(relay.ioPosition, true, false)){
      pvLastError = dev->lastError();
      return false;
    }
  }

  return true;
}

bool mdtTtTestNodeManager::checkDeviceIoMapCoherence(const QVariant & testNodeId, const shared_ptr<mdtMultiIoDevice> & dev)
{
  Q_ASSERT(!testNodeId.isNull());
  Q_ASSERT(dev);
  Q_ASSERT(dev->ios());

  std::vector<int> analogInputsIoPositions;
  std::vector<int> analogOutputsIoPositions;
  std::vector<int> digitalInputsIoPositions;
  std::vector<int> digitalOutputsIoPositions;
  bool ok;

  // Get I/O positions
  analogInputsIoPositions = getAnalogInputsIoPositionsList(testNodeId, ok);
  if(!ok){
    return false;
  }
  analogOutputsIoPositions = getAnalogOutputsIoPositionsList(testNodeId, ok);
  if(!ok){
    return false;
  }
  digitalInputsIoPositions = getDigitalInputsIoPositionsList(testNodeId, ok);
  if(!ok){
    return false;
  }
  digitalOutputsIoPositions = getDigitalOutputsIoPositionsList(testNodeId, ok);
  if(!ok){
    return false;
  }
  // Check analog inputs
  for(const auto & ioPosition : analogInputsIoPositions){
    if(!analogInputIoPositionExistsInDevice(ioPosition, dev->ios())){
      QString msg = tr("Device '%1': I/O position %2 does not exist in analog inputs of device.").arg(dev->alias()).arg(ioPosition);
      pvLastError.setError(msg, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
      pvLastError.commit();
      return false;
    }
  }
  // Check analog outputs
  for(const auto & ioPosition : analogOutputsIoPositions){
    if(!analogOutputIoPositionExistsInDevice(ioPosition, dev->ios())){
      QString msg = tr("Device '%1': I/O position %2 does not exist in analog outputs of device.").arg(dev->alias()).arg(ioPosition);
      pvLastError.setError(msg, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
      pvLastError.commit();
      return false;
    }
  }
  // Check digital inputs
  for(const auto & ioPosition : digitalInputsIoPositions){
    if(!digitalInputIoPositionExistsInDevice(ioPosition, dev->ios())){
      QString msg = tr("Device '%1': I/O position %2 does not exist in digital inputs of device.").arg(dev->alias()).arg(ioPosition);
      pvLastError.setError(msg, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
      pvLastError.commit();
      return false;
    }
  }
  // Check digital outputs
  for(const auto & ioPosition : digitalOutputsIoPositions){
    if(!digitalOutputIoPositionExistsInDevice(ioPosition, dev->ios())){
      QString msg = tr("Device '%1': I/O position %2 does not exist in digital outputs of device.").arg(dev->alias()).arg(ioPosition);
      pvLastError.setError(msg, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeManager");
      pvLastError.commit();
      return false;
    }
  }

  return true;
}

bool mdtTtTestNodeManager::analogInputIoPositionExistsInDevice(int ioPosition, const shared_ptr<mdtDeviceIos> & ios) const
{
  Q_ASSERT(ios);
  return (ioPosition < ios->analogInputsCount());
}

bool mdtTtTestNodeManager::analogOutputIoPositionExistsInDevice(int ioPosition, const shared_ptr< mdtDeviceIos >& ios) const
{
  Q_ASSERT(ios);
  return (ioPosition < ios->analogOutputsCount());
}

bool mdtTtTestNodeManager::digitalInputIoPositionExistsInDevice(int ioPosition, const shared_ptr<mdtDeviceIos> & ios) const
{
  Q_ASSERT(ios);
  return (ioPosition < ios->digitalInputsCount());
}

bool mdtTtTestNodeManager::digitalOutputIoPositionExistsInDevice(int ioPosition, const shared_ptr< mdtDeviceIos >& ios) const
{
  Q_ASSERT(ios);
  return (ioPosition < ios->digitalOutputsCount());
}

bool mdtTtTestNodeManager::setAnalogInputsLabelShort(shared_ptr<mdtDeviceIos> ios, const QVariant& testNodeId, const QString & alias)
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
    if(analogInputIoPositionExistsInDevice(dbIoPos, ios)){
      Q_ASSERT(dbIoPos < ios->analogInputs().size());
      io = ios->analogInputs().at(dbIoPos);
      Q_ASSERT(io != nullptr);
      io->setLabelShort(dbIo.value(0).toString());
    }
  }

  return true;
}

bool mdtTtTestNodeManager::setAnalogOutputsLabelShort(shared_ptr< mdtDeviceIos > ios, const QVariant& testNodeId, const QString & alias)
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
    if(analogOutputIoPositionExistsInDevice(dbIoPos, ios)){
      Q_ASSERT(dbIoPos < ios->analogOutputs().size());
      io = ios->analogOutputs().at(dbIoPos);
      Q_ASSERT(io != nullptr);
      io->setLabelShort(dbIo.value(0).toString());
    }
  }

  return true;
}

bool mdtTtTestNodeManager::setDigitalInputsLabelShort(std::shared_ptr<mdtDeviceIos> ios, const QVariant& testNodeId, const QString & alias)
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
    if(digitalInputIoPositionExistsInDevice(dbIoPos, ios)){
      Q_ASSERT(dbIoPos < ios->digitalInputs().size());
      io = ios->digitalInputs().at(dbIoPos);
      Q_ASSERT(io != nullptr);
      io->setLabelShort(dbIo.value(0).toString());
    }
  }

  return true;
}

bool mdtTtTestNodeManager::setDigitalOutputsLabelShort(std::shared_ptr<mdtDeviceIos> ios, const QVariant& testNodeId, const QString & alias)
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
    if(digitalOutputIoPositionExistsInDevice(dbIoPos, ios)){
      Q_ASSERT(dbIoPos < ios->digitalOutputs().size());
      io = ios->digitalOutputs().at(dbIoPos);
      Q_ASSERT(io != nullptr);
      io->setLabelShort(dbIo.value(0).toString());
    }
  }

  return true;
}

vector<int> mdtTtTestNodeManager::getAnalogInputsIoPositionsList(const QVariant & testNodeId, bool & ok)
{
  std::vector<int> ioPositionList;
  mdtTtTestNode tn(pvDatabase);
  QString sql;
  QList<QVariant> dataList;

  sql = "SELECT IoPosition FROM TestNodeUnit_tbl WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND (Type_Code_FK = 'AI')";
  sql += " AND IoPosition IS NOT NULL";
  dataList = tn.getDataList<QVariant>(sql, ok);
  if(!ok){
    pvLastError = tn.lastError();
    return ioPositionList;
  }
  for(const auto & var : dataList){
    Q_ASSERT(!var.isNull());
    ioPositionList.emplace_back(var.toInt());
  }

  return ioPositionList;
}

vector<int> mdtTtTestNodeManager::getAnalogOutputsIoPositionsList(const QVariant & testNodeId, bool & ok)
{
  std::vector<int> ioPositionList;
  mdtTtTestNode tn(pvDatabase);
  QString sql;
  QList<QVariant> dataList;

  sql = "SELECT IoPosition FROM TestNodeUnit_tbl WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND (Type_Code_FK = 'AO')";
  sql += " AND IoPosition IS NOT NULL";
  dataList = tn.getDataList<QVariant>(sql, ok);
  if(!ok){
    pvLastError = tn.lastError();
    return ioPositionList;
  }
  for(const auto & var : dataList){
    Q_ASSERT(!var.isNull());
    ioPositionList.emplace_back(var.toInt());
  }

  return ioPositionList;
}

vector<int> mdtTtTestNodeManager::getDigitalInputsIoPositionsList(const QVariant & testNodeId, bool & ok)
{
  std::vector<int> ioPositionList;
  mdtTtTestNode tn(pvDatabase);
  QString sql;
  QList<QVariant> dataList;

  sql = "SELECT IoPosition FROM TestNodeUnit_tbl WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND (Type_Code_FK = 'DI')";
  sql += " AND IoPosition IS NOT NULL";
  dataList = tn.getDataList<QVariant>(sql, ok);
  if(!ok){
    pvLastError = tn.lastError();
    return ioPositionList;
  }
  for(const auto & var : dataList){
    Q_ASSERT(!var.isNull());
    ioPositionList.emplace_back(var.toInt());
  }

  return ioPositionList;
}

vector<int> mdtTtTestNodeManager::getDigitalOutputsIoPositionsList(const QVariant & testNodeId, bool & ok)
{
  std::vector<int> ioPositionList;
  mdtTtTestNode tn(pvDatabase);
  QString sql;
  QList<QVariant> dataList;

  sql = "SELECT IoPosition FROM TestNodeUnit_tbl WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND (Type_Code_FK = 'BUSCPLREL' OR Type_Code_FK = 'CHANELREL')";
  sql += " AND IoPosition IS NOT NULL";
  dataList = tn.getDataList<QVariant>(sql, ok);
  if(!ok){
    pvLastError = tn.lastError();
    return ioPositionList;
  }
  for(const auto & var : dataList){
    Q_ASSERT(!var.isNull());
    ioPositionList.emplace_back(var.toInt());
  }

  return ioPositionList;
}
