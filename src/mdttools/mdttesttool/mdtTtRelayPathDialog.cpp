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
#include "mdtTtRelayPathDialog.h"
#include "mdtClPathGraph.h"
#include "mdtTtTestNode.h"
#include <QSqlRecord>
#include <QComboBox>
#include <QMessageBox>

#include <QDebug>

mdtTtRelayPathDialog::mdtTtRelayPathDialog(QSqlDatabase db, mdtClPathGraph *pg, QWidget *parent)
 : QDialog(parent)
{
  Q_ASSERT(pg != 0);
  pvGraph = pg;
  pvDatabase = db;
  pvLoadingData = false;
  setupUi(this);
  connect(cbSourceConnector, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSourceConnections(int)));
  connect(cbDestinationConnector, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDestinationConnections(int)));
  connect(cbSourceConnection, SIGNAL(currentIndexChanged(int)), this, SLOT(searchPath(int)));
  connect(cbDestinationConnection, SIGNAL(currentIndexChanged(int)), this, SLOT(searchPath(int)));
}

void mdtTtRelayPathDialog::setTestNodeId(const QVariant & id)
{
  qDebug() << "Setting test node ID " << id;
  pvLoadingData = true;
  displayTestNodeData(id);
  populateSourceConnectorCombobox(id);
  populateDestinationConnectorCombobox(id);
  pvGraph->loadLinkList();
  loadCouplingRelays(id);
  loadChannelRelays(id);
  pvLoadingData = false;
}

QList< QVariant > mdtTtRelayPathDialog::idListOfRelaysToEnable() const
{
  return pvRelaysToEnableIds;
}

void mdtTtRelayPathDialog::updateSourceConnections(int index)
{
  cbSourceConnection->clear();
  if(index < 0){
    return;
  }
  populateSourceConnectionCombobox(cbSourceConnector->itemData(index));
}

void mdtTtRelayPathDialog::updateDestinationConnections(int index)
{
  cbDestinationConnection->clear();
  if(index < 0){
    return;
  }
  populateDestinationConnectionCombobox(cbDestinationConnector->itemData(index));
}

void mdtTtRelayPathDialog::searchPath(int index)
{
  int sourceCbIndex, destinationCbIndex;
  QVariant sourceConnectionId, destinationConnectionId;
  QList<QVariant> pathConnectionIdList;
  mdtTtRelayPathItem item;
  QString couplingRelaysStr, channelRelaysStr;
  int i;

  if(pvLoadingData){
    return;
  }
  // Clear previous results
  lbCouplingRelays->clear();
  lbChannelRelays->clear();
  pvRelaysToEnableIds.clear();
  // Get selected source and destination connection IDs
  sourceCbIndex = cbSourceConnection->currentIndex();
  destinationCbIndex = cbDestinationConnection->currentIndex();
  if((sourceCbIndex < 0)||(destinationCbIndex < 0)){
    return;
  }
  sourceConnectionId = cbSourceConnection->itemData(sourceCbIndex);
  destinationConnectionId = cbDestinationConnection->itemData(destinationCbIndex);
  if((sourceConnectionId.isNull())||(destinationConnectionId.isNull())){
    return;
  }
  // Build path
  pathConnectionIdList = pvGraph->getShortestPath(sourceConnectionId, destinationConnectionId);
  qDebug() << "Path: " << pathConnectionIdList;
  
  if(pathConnectionIdList.size() < 2){
    return;
  }
  // Build relays list
  for(i = 1; i < pathConnectionIdList.size(); ++i){
    Q_ASSERT(!pathConnectionIdList.at(i-1).isNull());
    Q_ASSERT(!pathConnectionIdList.at(i).isNull());
    qDebug() << "Search for connection ID " << pathConnectionIdList.at(i-1) << " to " << pathConnectionIdList.at(i);
    item = getCouplingRelay(pathConnectionIdList.at(i-1), pathConnectionIdList.at(i));
    if(!item.isNull()){
      Q_ASSERT(!item.testNodeUnitId.isNull());
      qDebug() << "Coupling relay: " << item.name;
      pvRelaysToEnableIds.append(item.testNodeUnitId);
      if(couplingRelaysStr.isEmpty()){
        couplingRelaysStr = item.name;
      }else{
        couplingRelaysStr += ", " + item.name;
      }
      continue;
    }
    item = getChannleRelay(pathConnectionIdList.at(i-1), pathConnectionIdList.at(i));
    if(!item.isNull()){
      Q_ASSERT(!item.testNodeUnitId.isNull());
      qDebug() << "Channel relay: " << item.name;
      pvRelaysToEnableIds.append(item.testNodeUnitId);
      if(channelRelaysStr.isEmpty()){
        channelRelaysStr = item.name;
      }else{
        channelRelaysStr += ", " + item.name;
      }
    }
  }
  // Display result
  lbCouplingRelays->setText(couplingRelaysStr);
  lbChannelRelays->setText(channelRelaysStr);
}

void mdtTtRelayPathDialog::displayTestNodeData(const QVariant& testNodeId)
{
  mdtTtTestNode tn(0, pvDatabase);
  QSqlRecord data;
  bool ok;

  data = tn.getTestNodeData(testNodeId, ok);
  if(!ok){
    lbTestSystem->setText(tr("<Error!>"));
    lbTestNode->setText(tr("<Error!>"));
    return;
  }
  lbTestSystem->setText(data.value("Type").toString());
  lbTestNode->setText(data.value("SubType").toString());
}

bool mdtTtRelayPathDialog::populateSourceConnectorCombobox(const QVariant & testNodeId)
{
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;
  int i;

  cbSourceConnector->clear();
  dataList = tn.getData(sqlForTestConnectorData(testNodeId), &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    cbSourceConnector->addItem(dataList.at(i).value("SchemaPosition").toString(), dataList.at(i).value("Unit_Id_FK_PK"));
  }

  return true;
}

bool mdtTtRelayPathDialog::populateSourceConnectionCombobox(const QVariant& testNodeUnitId)
{
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;
  int i;

  ///cbSourceConnection->clear();
  dataList = tn.getData(sqlForTestConnectionData(testNodeUnitId), &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    cbSourceConnection->addItem(dataList.at(i).value("UnitContactName").toString(), dataList.at(i).value("Id_PK"));
  }

  return true;
}

bool mdtTtRelayPathDialog::populateDestinationConnectorCombobox(const QVariant& testNodeId)
{
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;
  int i;

  cbDestinationConnector->clear();
  dataList = tn.getData(sqlForTestConnectorData(testNodeId), &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    cbDestinationConnector->addItem(dataList.at(i).value("SchemaPosition").toString(), dataList.at(i).value("Unit_Id_FK_PK"));
  }

  return true;
}

bool mdtTtRelayPathDialog::populateDestinationConnectionCombobox(const QVariant& testNodeUnitId)
{
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;
  int i;

  ///cbDestinationConnection->clear();
  dataList = tn.getData(sqlForTestConnectionData(testNodeUnitId), &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    cbDestinationConnection->addItem(dataList.at(i).value("UnitContactName").toString(), dataList.at(i).value("Id_PK"));
  }

  return true;
}

bool mdtTtRelayPathDialog::loadCouplingRelays(const QVariant & testNodeId)
{
  QString sql;
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  QList<QSqlRecord> connectionsList;
  bool ok;
  int i;

  pvCouplingRelays.clear();
  // Get coupling relays
  sql = "SELECT TNU.Unit_Id_FK_PK, U.SchemaPosition FROM TestNodeUnit_tbl TNU JOIN Unit_tbl U ON U.Id_PK = TNU.Unit_Id_FK_PK ";
  sql += " WHERE TNU.TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND Type_Code_FK = 'BUSCPLREL'";
  dataList = tn.getData(sql, &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    mdtTtRelayPathItem item;
    // Get connections
    data = dataList.at(i);
    qDebug() << "CR: " << data;
    sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE Unit_Id_FK = " + data.value("Unit_Id_FK_PK").toString();
    connectionsList = tn.getData(sql, &ok);
    if(!ok){
      displayError(tn.lastError());
      return false;
    }
    // We only handle relays with exactly 2 connections
    if(connectionsList.size() == 2){
      item.startConnectionId = connectionsList.at(0).value(0);
      item.endConnectionId = connectionsList.at(1).value(0);
      item.testNodeUnitId = data.value("Unit_Id_FK_PK");
      item.name = data.value("SchemaPosition").toString();
      qDebug() << "Adding " << item.name;
      pvCouplingRelays.append(item);
      pvGraph->addLink(item.startConnectionId, item.endConnectionId, true, 2);
    }else{
      mdtError e(tr("Relay ID") + data.value("Unit_Id_FK_PK").toString() + tr(" has not exactly 2 connections, will be ignored."), mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtTtRelayPathDialog");
      e.commit();
    }
  }

  return true;
}

bool mdtTtRelayPathDialog::loadChannelRelays(const QVariant & testNodeId)
{
  QString sql;
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  QList<QSqlRecord> connectionsList;
  bool ok;
  int i;

  pvChannelRelays.clear();
  // Get coupling relays
  sql = "SELECT TNU.Unit_Id_FK_PK, U.SchemaPosition FROM TestNodeUnit_tbl TNU JOIN Unit_tbl U ON U.Id_PK = TNU.Unit_Id_FK_PK ";
  sql += " WHERE TNU.TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND Type_Code_FK = 'CHANELREL'";
  dataList = tn.getData(sql, &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    mdtTtRelayPathItem item;
    // Get connections
    data = dataList.at(i);
    qDebug() << "CR: " << data;
    sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE Unit_Id_FK = " + data.value("Unit_Id_FK_PK").toString();
    connectionsList = tn.getData(sql, &ok);
    if(!ok){
      displayError(tn.lastError());
      return false;
    }
    // We only handle relays with exactly 2 connections
    if(connectionsList.size() == 2){
      item.startConnectionId = connectionsList.at(0).value(0);
      item.endConnectionId = connectionsList.at(1).value(0);
      item.testNodeUnitId = data.value("Unit_Id_FK_PK");
      item.name = data.value("SchemaPosition").toString();
      qDebug() << "Adding " << item.name;
      pvChannelRelays.append(item);
      pvGraph->addLink(item.startConnectionId, item.endConnectionId, true, 2);
    }else{
      mdtError e(tr("Relay ID") + data.value("Unit_Id_FK_PK").toString() + tr(" has not exactly 2 connections, will be ignored."), mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtTtRelayPathDialog");
      e.commit();
    }
  }

  return true;
}

mdtTtRelayPathItem mdtTtRelayPathDialog::getCouplingRelay(const QVariant & A, const QVariant & B)
{
  mdtTtRelayPathItem item;
  int i;

  for(i = 0; i < pvCouplingRelays.size(); ++i){
    item = pvCouplingRelays.at(i);
    if((item.startConnectionId == A)&&(item.endConnectionId == B)){
      return item;
    }
    if((item.startConnectionId == B)&&(item.endConnectionId == A)){
      return item;
    }
  }

  return mdtTtRelayPathItem();
}

mdtTtRelayPathItem mdtTtRelayPathDialog::getChannleRelay(const QVariant & A, const QVariant & B)
{
  mdtTtRelayPathItem item;
  int i;

  for(i = 0; i < pvChannelRelays.size(); ++i){
    item = pvChannelRelays.at(i);
    if((item.startConnectionId == A)&&(item.endConnectionId == B)){
      return item;
    }
    if((item.startConnectionId == B)&&(item.endConnectionId == A)){
      return item;
    }
  }

  return mdtTtRelayPathItem();
}

QString mdtTtRelayPathDialog::sqlForTestConnectorData(const QVariant& testNodeId) const
{
  QString sql;

  sql = "SELECT\n"
        " TNU.Unit_Id_FK_PK,\n"\
        " U.SchemaPosition\n"\
        "FROM TestNodeUnit_tbl TNU\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNode_tbl TN\n"
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n";
  sql += " WHERE TN.VehicleType_Id_FK_PK = " + testNodeId.toString();
  sql += " AND TNU.Type_Code_FK = 'TESTCONNECTOR'";
  sql += " ORDER BY U.SchemaPosition ASC";

  return sql;
}

QString mdtTtRelayPathDialog::sqlForTestConnectionData(const QVariant & testNodeUnitId) const
{
  QString sql;

  sql = "SELECT Id_PK, UnitContactName FROM UnitConnection_tbl WHERE Unit_Id_FK = " + testNodeUnitId.toString();

  return sql;
}

void mdtTtRelayPathDialog::displayError(const mdtError & error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
