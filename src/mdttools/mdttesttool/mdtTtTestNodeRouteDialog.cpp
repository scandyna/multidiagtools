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
#include "mdtTtTestNodeRouteDialog.h"
#include "mdtTtTestNodeRoute.h"
#include "mdtClPathGraph.h"
#include "mdtTtTestNode.h"
#include <QSqlRecord>
#include <QComboBox>
#include <QMessageBox>
#include <QLocale>

#include <QDebug>

mdtTtTestNodeRouteDialog::mdtTtTestNodeRouteDialog(QSqlDatabase db, QWidget *parent)
 : QDialog(parent)
{
  pvDatabase = db;
  pvLoadingData = false;
  setupUi(this);
  connect(cbUnitAType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateUnitA(int)));
  connect(cbUnitBType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateUnitB(int)));
  connect(cbUnitA, SIGNAL(currentIndexChanged(int)), this, SLOT(updateUnitAConnections(int)));
  connect(cbUnitB, SIGNAL(currentIndexChanged(int)), this, SLOT(updateUnitBConnections(int)));
  connect(cbConnectionA, SIGNAL(currentIndexChanged(int)), this, SLOT(searchPath(int)));
  connect(cbConnectionB, SIGNAL(currentIndexChanged(int)), this, SLOT(searchPath(int)));
}

void mdtTtTestNodeRouteDialog::setTestNodeId(const QVariant & testNodeId, const std::shared_ptr<mdtClPathGraph> & graph)
{
  Q_ASSERT(graph);

  pvGraph = graph;
  pvTestNodeId = testNodeId;
  pvLoadingData = true;
  displayTestNodeData();
  populateTestNodeUnitATypeCombobox();
  populateTestNodeUnitBTypeCombobox();
//   populateTestNodeUnitACombobox(testNodeId);
//   populateTestNodeUnitBCombobox(testNodeId);
//   loadRelays(testNodeId);
  pvLoadingData = false;
  searchPath(0);
}

// void mdtTtTestNodeRouteDialog::setData(const QVariant & testModelItemId, const QVariant & testNodeId)
// {
//   pvTestModelItemId = testModelItemId;
//   pvLoadingData = true;
//   displayTestNodeData(testNodeId);
//   populateTestNodeUnitACombobox(testNodeId);
//   populateTestNodeUnitBCombobox(testNodeId);
//   pvGraph->loadLinkList();
//   loadRelays(testNodeId);
//   pvLoadingData = false;
//   searchPath(0);
// }

// QList< QVariant > mdtTtTestNodeRouteDialog::idListOfRelaysToEnable() const
// {
//   return pvRelaysToEnableIds;
// }
// 
// QVariant mdtTtTestNodeRouteDialog::selectedTestConnection() const
// {
//   int index = cbConnectionB->currentIndex();
// 
//   if(index < 0){
//     return QVariant();
//   }
//   return cbConnectionB->itemData(index);
// }
// 
// QVariant mdtTtTestNodeRouteDialog::selectedMeasureConnection() const
// {
//   int index = cbConnectionA->currentIndex();
// 
//   if(index < 0){
//     return QVariant();
//   }
//   return cbConnectionA->itemData(index);
// }

void mdtTtTestNodeRouteDialog::updateUnitA(int index)
{
  cbUnitA->clear();
  if(index < 0){
    return;
  }
  populateTestNodeUnitACombobox(cbUnitAType->itemData(index).toString());
}

void mdtTtTestNodeRouteDialog::updateUnitAConnections(int index)
{
  cbConnectionA->clear();
  if(index < 0){
    return;
  }
  populateConnectionACombobox(cbUnitA->itemData(index));
}

void mdtTtTestNodeRouteDialog::updateUnitB(int index)
{
  cbUnitB->clear();
  if(index < 0){
    return;
  }
  populateTestNodeUnitBCombobox(cbUnitBType->itemData(index).toString());
}

void mdtTtTestNodeRouteDialog::updateUnitBConnections(int index)
{
  cbConnectionB->clear();
  if(index < 0){
    return;
  }
  populateConnectionBCombobox(cbUnitB->itemData(index));
}

void mdtTtTestNodeRouteDialog::searchPath(int index)
{
  mdtTtTestNodeRoute tnr(pvDatabase);
  int cbIndexA, cbIndexB;
  QVariant connectionIdA, connectionIdB;
  ///QList<QVariant> pathConnectionIdList;
  QString relaysStr;
//   QVariant relayId;
  bool ok;

  pvRouteData.clear();
  if(!pvGraph){
    return;
  }
  if(pvLoadingData){
    return;
  }
  // Clear previous results
  lbRelays->clear();
  pvRelaysToEnableIds.clear();
  // Get selected A and B connection IDs
  cbIndexA = cbConnectionA->currentIndex();
  cbIndexB = cbConnectionB->currentIndex();
  if((cbIndexA < 0)||(cbIndexB < 0)){
    return;
  }
  connectionIdA = cbConnectionA->itemData(cbIndexA);
  connectionIdB = cbConnectionB->itemData(cbIndexB);
  if((connectionIdA.isNull())||(connectionIdB.isNull())){
    return;
  }
  if(connectionIdA == connectionIdB){
    return;
  }
  // Build path
  Q_ASSERT(pvGraph);
  pvRouteData = tnr.buildRoute(pvTestNodeId, connectionIdA, connectionIdB, *pvGraph, ok);
  if(!ok){
    displayError(tnr.lastError());
    return;
  }
  // Build relays list string
  for(const auto & relay : pvRouteData.relaysToEnableVector()){
    if(relaysStr.isEmpty()){
      relaysStr = relay.schemaPosition.toString();
    }else{
      relaysStr += ", " + relay.schemaPosition.toString();
    }
  }

//   pathConnectionIdList = pvGraph->getShortestPath(sourceConnectionId, destinationConnectionId, ok);
//   if(!ok){
//     displayError(pvGraph->lastError());
//     return;
//   }
//   if(pathConnectionIdList.size() < 2){
//     return;
//   }
//   // Build relays list
//   for(i = 1; i < pathConnectionIdList.size(); ++i){
//     Q_ASSERT(!pathConnectionIdList.at(i-1).isNull());
//     Q_ASSERT(!pathConnectionIdList.at(i).isNull());
//     relayId = pvGraph->getUserData(pathConnectionIdList.at(i-1), pathConnectionIdList.at(i));
//     if(!relayId.isNull()){
//       Q_ASSERT(!pvRelaysToEnableIds.contains(relayId));
//       pvRelaysToEnableIds.append(relayId);
//       if(relaysStr.isEmpty()){
//         relaysStr = pvRelayNameMap.value(relayId.toInt());
//       }else{
//         relaysStr += ", " + pvRelayNameMap.value(relayId.toInt());
//       }
//     }
//   }
  // Display result
  lbRelays->setText(relaysStr);
}

void mdtTtTestNodeRouteDialog::displayTestNodeData()
{
  mdtTtTestNode tn(0, pvDatabase);
  QSqlRecord data;
  bool ok;

  data = tn.getTestNodeData(pvTestNodeId, ok);
  if(!ok){
    lbTestSystem->setText(tr("<Error!>"));
    lbTestNode->setText(tr("<Error!>"));
    return;
  }
  lbTestSystem->setText(data.value("Type").toString());
  lbTestNode->setText(data.value("SubType").toString());
}

bool mdtTtTestNodeRouteDialog::populateTestNodeUnitATypeCombobox()
{
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

  cbUnitAType->clear();
  dataList = tn.getData(sqlForUnitTypeData(), &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(const auto & data : dataList){
    cbUnitAType->addItem(data.value("Function").toString(), data.value("Code_PK"));
  }

  return true;
}

bool mdtTtTestNodeRouteDialog::populateTestNodeUnitACombobox(const QString & unitType)
{
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
//   QString sql;
  bool ok;
  int i;

  cbUnitA->clear();
  /**
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
  */
  dataList = tn.getData(sqlForUnitData(unitType), &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    cbUnitA->addItem(dataList.at(i).value("SchemaPosition").toString(), dataList.at(i).value("Unit_Id_FK_PK"));
  }

  return true;
}

bool mdtTtTestNodeRouteDialog::populateConnectionACombobox(const QVariant & testNodeUnitId)
{
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  QString text;
  QString function;
  bool ok;

  dataList = tn.getData(sqlForConnectionData(testNodeUnitId), &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(const auto & data : dataList){
    text = data.value("UnitContactName").toString();
    function = data.value("Function").toString();
    if(!function.isEmpty()){
      text += " ( " + function + " )";
    }
    cbConnectionA->addItem(text, data.value("Id_PK"));
  }

  return true;
}

bool mdtTtTestNodeRouteDialog::populateTestNodeUnitBTypeCombobox()
{
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;

  cbUnitBType->clear();
  dataList = tn.getData(sqlForUnitTypeData(), &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(const auto & data : dataList){
    cbUnitBType->addItem(data.value("Function").toString(), data.value("Code_PK"));
  }

  return true;
}

bool mdtTtTestNodeRouteDialog::populateTestNodeUnitBCombobox(const QString & unitType)
{
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
//   QString sql;
  bool ok;
  int i;

  cbUnitB->clear();
  /**
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
  */
  dataList = tn.getData(sqlForUnitData(unitType), &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    cbUnitB->addItem(dataList.at(i).value("SchemaPosition").toString(), dataList.at(i).value("Unit_Id_FK_PK"));
  }

  return true;
}

bool mdtTtTestNodeRouteDialog::populateConnectionBCombobox(const QVariant& testNodeUnitId)
{
  mdtTtTestNode tn(0, pvDatabase);
  QList<QSqlRecord> dataList;
  QString text;
  QString function;
  bool ok;

  dataList = tn.getData(sqlForConnectionData(testNodeUnitId), &ok);
  if(!ok){
    displayError(tn.lastError());
    return false;
  }
  for(const auto & data : dataList){
    text = data.value("UnitContactName").toString();
    function = data.value("Function").toString();
    if(!function.isEmpty()){
      text += " ( " + function + " )";
    }
    cbConnectionB->addItem(text, data.value("Id_PK"));
  }

  return true;
}

// bool mdtTtTestNodeRouteDialog::populateConnectionBCombobox(const QVariant& testNodeUnitId)
// {
//   mdtTtTestNode tn(0, pvDatabase);
//   QList<QSqlRecord> dataList;
//   QSqlRecord data;
//   QString text;
//   QString function;
//   QString sql;
//   QLocale locale;
//   QString functionFieldName;
//   bool ok;
//   int i;
// 
//   // Select function field and generate SQL statement
//   switch(locale.language()){
//     case QLocale::French:
//       functionFieldName = "FunctionFR AS Function";
//       break;
//     case QLocale::German:
//       functionFieldName = "FunctionDE AS Function";
//       break;
//     case QLocale::Italian:
//       functionFieldName = "FunctionIT AS Function";
//       break;
//     default:
//       functionFieldName = "FunctionEN AS Function";
//   }
//   sql = "SELECT Id_PK, UnitContactName, " + functionFieldName + " FROM UnitConnection_tbl ";
//   sql += " WHERE Unit_Id_FK = " + testNodeUnitId.toString();
//   sql += " AND Id_PK IN (";
//   sql += "  SELECT TL.TestConnection_Id_FK FROM TestLink_tbl TL JOIN TestModelItem_TestLink_tbl TMITL ON TMITL.TestLink_Id_FK = TL.Id_PK ";
//   sql += "  WHERE TMITL.TestModelItem_Id_FK = " + pvTestModelItemId.toString();
//   sql += ")";
//   // Get data and populate combobox
//   dataList = tn.getData(sql, &ok);
//   if(!ok){
//     displayError(tn.lastError());
//     return false;
//   }
//   for(i = 0; i < dataList.size(); ++i){
//     data = dataList.at(i);
//     text = data.value("UnitContactName").toString();
//     function = data.value("Function").toString();
//     if(!function.isEmpty()){
//       text += " ( " + function + " )";
//     }
//     cbConnectionB->addItem(text, data.value("Id_PK"));
//   }
// 
//   return true;
// }

// bool mdtTtTestNodeRouteDialog::loadRelays(const QVariant & testNodeId)
// {
//   Q_ASSERT(pvGraph);
// 
//   QString sql;
//   mdtTtTestNode tn(0, pvDatabase);
//   QList<QSqlRecord> dataList;
//   ///QSqlRecord data;
//   ///QList<QVariant> connectionsList;
//   ///QVariant testNodeUnitId;
//   bool ok;
//   int i;
// 
//   // Add relays to graph
//   if(!tn.addRelaysToGraph(testNodeId, *pvGraph)){
//     displayError(tn.lastError());
//     return false;
//   }
//   // Load the relays name map
//   sql = "SELECT TNU.Unit_Id_FK_PK, U.SchemaPosition FROM TestNodeUnit_tbl TNU JOIN Unit_tbl U ON U.Id_PK = TNU.Unit_Id_FK_PK ";
//   sql += " WHERE TNU.TestNode_Id_FK = " + testNodeId.toString();
//   sql += " AND (Type_Code_FK = 'BUSCPLREL' OR Type_Code_FK = 'CHANELREL')";
//   dataList = tn.getDataList<QSqlRecord>(sql, ok);
//   if(!ok){
//     displayError(tn.lastError());
//     return false;
//   }
//   for(i = 0; i < dataList.size(); ++i){
//     Q_ASSERT(!dataList.at(i).value("Unit_Id_FK_PK").isNull());
//     pvRelayNameMap.insert(dataList.at(i).value("Unit_Id_FK_PK").toInt(), dataList.at(i).value("SchemaPosition").toString());
//   }
//   /**
//   dataList = tn.getData(sql, &ok);
//   if(!ok){
//     displayError(tn.lastError());
//     return false;
//   }
//   for(i = 0; i < dataList.size(); ++i){
//     // Get connections
//     data = dataList.at(i);
//     sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE Unit_Id_FK = " + data.value("Unit_Id_FK_PK").toString();
//     connectionsList = tn.getDataList<QVariant>(sql, ok);
//     if(!ok){
//       displayError(tn.lastError());
//       return false;
//     }
//     // We only handle relays with exactly 2 connections
//     if(connectionsList.size() == 2){
//       testNodeUnitId = data.value("Unit_Id_FK_PK");
//       Q_ASSERT(!testNodeUnitId.isNull());
//       pvGraph->addLink(connectionsList.at(0), connectionsList.at(1), testNodeUnitId, true, 2);
//       pvRelayNameMap.insert(testNodeUnitId.toInt(), data.value("SchemaPosition").toString());
//     }else{
//       mdtError e(tr("Relay ID") + data.value("Unit_Id_FK_PK").toString() + tr(" has not exactly 2 connections, will be ignored."), mdtError::Warning);
//       MDT_ERROR_SET_SRC(e, "mdtTtTestNodeRouteDialog");
//       e.commit();
//     }
//   }
//   */
// 
//   return true;
// }

QString mdtTtTestNodeRouteDialog::sqlForUnitTypeData() const
{
  QString sql;
  QLocale locale;
  QString functionFieldName;

  switch(locale.language()){
    case QLocale::French:
      functionFieldName = "NameFR AS Function";
      break;
    case QLocale::German:
      functionFieldName = "NameDE AS Function";
      break;
    case QLocale::Italian:
      functionFieldName = "NameIT AS Function";
      break;
    default:
      functionFieldName = "NameEN AS Function";
  }
  sql = "SELECT DISTINCT TNUT.Code_PK, TNUT." + functionFieldName + " FROM TestNodeUnitType_tbl TNUT";
  sql += " JOIN TestNodeUnit_tbl TNU ON TNU.Type_Code_FK = TNUT.Code_PK";
  sql += " WHERE TNU.TestNode_Id_FK = " + pvTestNodeId.toString();

  return sql;
}

QString mdtTtTestNodeRouteDialog::sqlForUnitData(const QString & unitType) const
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
  sql += " WHERE TN.VehicleType_Id_FK_PK = " + pvTestNodeId.toString();
  if(!unitType.isEmpty()){
    sql += " AND (TNU.Type_Code_FK = '" + unitType + "')";
  }
  sql += " ORDER BY U.SchemaPosition ASC";

  return sql;
}

QString mdtTtTestNodeRouteDialog::sqlForConnectionData(const QVariant & testNodeUnitId) const
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

void mdtTtTestNodeRouteDialog::displayError(const mdtError & error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
