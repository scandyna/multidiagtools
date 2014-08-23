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
#include <QLocale>

#include <QDebug>

mdtTtRelayPathDialog::mdtTtRelayPathDialog(QSqlDatabase db, mdtClPathGraph *pg, QWidget *parent)
 : QDialog(parent)
{
  Q_ASSERT(pg != 0);
  pvGraph = pg;
  pvDatabase = db;
  pvLoadingData = false;
  setupUi(this);
  connect(cbSourceUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSourceConnections(int)));
  connect(cbDestinationUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDestinationConnections(int)));
  connect(cbSourceConnection, SIGNAL(currentIndexChanged(int)), this, SLOT(searchPath(int)));
  connect(cbDestinationConnection, SIGNAL(currentIndexChanged(int)), this, SLOT(searchPath(int)));
}

void mdtTtRelayPathDialog::setTestNodeId(const QVariant & id)
{

}

void mdtTtRelayPathDialog::setData(const QVariant & testModelItemId, const QVariant & testNodeId)
{
  pvTestModelItemId = testModelItemId;
  pvLoadingData = true;
  displayTestNodeData(testNodeId);
  populateSourceTestNodeUnitCombobox(testNodeId);
  populateDestinationTestNodeUnitCombobox(testNodeId);
  pvGraph->loadLinkList();
  loadRelays(testNodeId);
  pvLoadingData = false;
  searchPath(0);
}

QList< QVariant > mdtTtRelayPathDialog::idListOfRelaysToEnable() const
{
  return pvRelaysToEnableIds;
}

QVariant mdtTtRelayPathDialog::selectedTestConnection() const
{
  int index = cbDestinationConnection->currentIndex();

  if(index < 0){
    return QVariant();
  }
  return cbDestinationConnection->itemData(index);
}

QVariant mdtTtRelayPathDialog::selectedMeasureConnection() const
{
  int index = cbSourceConnection->currentIndex();

  if(index < 0){
    return QVariant();
  }
  return cbSourceConnection->itemData(index);
}

void mdtTtRelayPathDialog::updateSourceConnections(int index)
{
  cbSourceConnection->clear();
  if(index < 0){
    return;
  }
  populateSourceConnectionCombobox(cbSourceUnit->itemData(index));
}

void mdtTtRelayPathDialog::updateDestinationConnections(int index)
{
  cbDestinationConnection->clear();
  if(index < 0){
    return;
  }
  populateDestinationConnectionCombobox(cbDestinationUnit->itemData(index));
}

void mdtTtRelayPathDialog::searchPath(int index)
{
  int sourceCbIndex, destinationCbIndex;
  QVariant sourceConnectionId, destinationConnectionId;
  QList<QVariant> pathConnectionIdList;
  QString relaysStr;
  QVariant relayId;
  bool ok;
  int i;

  if(pvLoadingData){
    return;
  }
  // Clear previous results
  lbRelays->clear();
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
  pathConnectionIdList = pvGraph->getShortestPath(sourceConnectionId, destinationConnectionId, ok);
  if(!ok){
    displayError(pvGraph->lastError());
    return;
  }
  if(pathConnectionIdList.size() < 2){
    return;
  }
  // Build relays list
  for(i = 1; i < pathConnectionIdList.size(); ++i){
    Q_ASSERT(!pathConnectionIdList.at(i-1).isNull());
    Q_ASSERT(!pathConnectionIdList.at(i).isNull());
    relayId = pvGraph->getUserData(pathConnectionIdList.at(i-1), pathConnectionIdList.at(i));
    if(!relayId.isNull()){
      Q_ASSERT(!pvRelaysToEnableIds.contains(relayId));
      pvRelaysToEnableIds.append(relayId);
      if(relaysStr.isEmpty()){
        relaysStr = pvRelayNameMap.value(relayId.toInt());
      }else{
        relaysStr += ", " + pvRelayNameMap.value(relayId.toInt());
      }
    }
  }
  // Display result
  lbRelays->setText(relaysStr);
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

bool mdtTtRelayPathDialog::populateSourceTestNodeUnitCombobox(const QVariant & testNodeId)
{
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  QString sql;
  bool ok;
  int i;

  cbSourceUnit->clear();
  sql = "SELECT\n"
        " TNU.Unit_Id_FK_PK,\n"\
        " U.SchemaPosition\n"\
        "FROM TestNodeUnit_tbl TNU\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNode_tbl TN\n"
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n";
  sql += " WHERE TN.VehicleType_Id_FK_PK = " + testNodeId.toString();
  sql += " AND (TNU.Type_Code_FK <> 'BUSCPLREL' AND TNU.Type_Code_FK <> 'CHANELREL' AND TNU.Type_Code_FK <> 'TESTCONNECTOR')";
  sql += " ORDER BY U.SchemaPosition ASC";
  dataList = tn.getData(sql, &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    cbSourceUnit->addItem(dataList.at(i).value("SchemaPosition").toString(), dataList.at(i).value("Unit_Id_FK_PK"));
  }

  return true;
}

bool mdtTtRelayPathDialog::populateSourceConnectionCombobox(const QVariant& testNodeUnitId)
{
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  QString text;
  QString function;
  bool ok;
  int i;

  dataList = tn.getData(sqlForTestConnectionData(testNodeUnitId), &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    data = dataList.at(i);
    text = data.value("UnitContactName").toString();
    function = data.value("Function").toString();
    if(!function.isEmpty()){
      text += " ( " + function + " )";
    }
    cbSourceConnection->addItem(text, data.value("Id_PK"));
  }

  return true;
}

bool mdtTtRelayPathDialog::populateDestinationTestNodeUnitCombobox(const QVariant& testNodeId)
{
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  QString sql;
  bool ok;
  int i;

  cbDestinationUnit->clear();
  sql = "SELECT\n"
        " TNU.Unit_Id_FK_PK,\n"\
        " U.SchemaPosition\n"\
        "FROM TestNodeUnit_tbl TNU\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNode_tbl TN\n"
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n";
  sql += " WHERE TN.VehicleType_Id_FK_PK = " + testNodeId.toString();
  sql += " AND (TNU.Type_Code_FK = 'TESTCONNECTOR')";
  sql += " ORDER BY U.SchemaPosition ASC";
  dataList = tn.getData(sql, &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    cbDestinationUnit->addItem(dataList.at(i).value("SchemaPosition").toString(), dataList.at(i).value("Unit_Id_FK_PK"));
  }

  return true;
}

bool mdtTtRelayPathDialog::populateDestinationConnectionCombobox(const QVariant& testNodeUnitId)
{
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  QString text;
  QString function;
  QString sql;
  QLocale locale;
  QString functionFieldName;
  bool ok;
  int i;

  // Select function field and generate SQL statement
  switch(locale.language()){
    case QLocale::French:
      functionFieldName = "FunctionFR AS Function";
      break;
    case QLocale::German:
      functionFieldName = "FunctionDE AS Function";
      break;
    case QLocale::Italian:
      functionFieldName = "FunctionIT AS Function";
      break;
    default:
      functionFieldName = "FunctionEN AS Function";
  }
  sql = "SELECT Id_PK, UnitContactName, " + functionFieldName + " FROM UnitConnection_tbl ";
  sql += " WHERE Unit_Id_FK = " + testNodeUnitId.toString();
  sql += " AND Id_PK IN (";
  sql += "  SELECT TL.TestConnection_Id_FK FROM TestLink_tbl TL JOIN TestModelItem_TestLink_tbl TMITL ON TMITL.TestLink_Id_FK = TL.Id_PK ";
  sql += "  WHERE TMITL.TestModelItem_Id_FK = " + pvTestModelItemId.toString();
  sql += ")";
  // Get data and populate combobox
  dataList = tn.getData(sql, &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    data = dataList.at(i);
    text = data.value("UnitContactName").toString();
    function = data.value("Function").toString();
    if(!function.isEmpty()){
      text += " ( " + function + " )";
    }
    cbDestinationConnection->addItem(text, data.value("Id_PK"));
  }

  return true;
}

bool mdtTtRelayPathDialog::loadRelays(const QVariant & testNodeId)
{
  QString sql;
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  ///QSqlRecord data;
  ///QList<QVariant> connectionsList;
  ///QVariant testNodeUnitId;
  bool ok;
  int i;

  // Add relays to graph
  if(!tn.addRelaysToGraph(testNodeId, *pvGraph)){
    displayError(tn.lastError());
    return false;
  }
  // Load the relays name map
  sql = "SELECT TNU.Unit_Id_FK_PK, U.SchemaPosition FROM TestNodeUnit_tbl TNU JOIN Unit_tbl U ON U.Id_PK = TNU.Unit_Id_FK_PK ";
  sql += " WHERE TNU.TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND (Type_Code_FK = 'BUSCPLREL' OR Type_Code_FK = 'CHANELREL')";
  dataList = tn.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    Q_ASSERT(!dataList.at(i).value("Unit_Id_FK_PK").isNull());
    pvRelayNameMap.insert(dataList.at(i).value("Unit_Id_FK_PK").toInt(), dataList.at(i).value("SchemaPosition").toString());
  }
  /**
  dataList = tn.getData(sql, &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    // Get connections
    data = dataList.at(i);
    sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE Unit_Id_FK = " + data.value("Unit_Id_FK_PK").toString();
    connectionsList = tn.getDataList<QVariant>(sql, ok);
    if(!ok){
      displayError(tn.lastError());
      return false;
    }
    // We only handle relays with exactly 2 connections
    if(connectionsList.size() == 2){
      testNodeUnitId = data.value("Unit_Id_FK_PK");
      Q_ASSERT(!testNodeUnitId.isNull());
      pvGraph->addLink(connectionsList.at(0), connectionsList.at(1), testNodeUnitId, true, 2);
      pvRelayNameMap.insert(testNodeUnitId.toInt(), data.value("SchemaPosition").toString());
    }else{
      mdtError e(tr("Relay ID") + data.value("Unit_Id_FK_PK").toString() + tr(" has not exactly 2 connections, will be ignored."), mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtTtRelayPathDialog");
      e.commit();
    }
  }
  */

  return true;
}

QString mdtTtRelayPathDialog::sqlForTestConnectionData(const QVariant & testNodeUnitId) const
{
  QString sql;
  QLocale locale;
  QString functionFieldName;

  switch(locale.language()){
    case QLocale::French:
      functionFieldName = "FunctionFR AS Function";
      break;
    case QLocale::German:
      functionFieldName = "FunctionDE AS Function";
      break;
    case QLocale::Italian:
      functionFieldName = "FunctionIT AS Function";
      break;
    default:
      functionFieldName = "FunctionEN AS Function";
  }
  sql = "SELECT Id_PK, UnitContactName, " + functionFieldName + " FROM UnitConnection_tbl WHERE Unit_Id_FK = " + testNodeUnitId.toString();

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
