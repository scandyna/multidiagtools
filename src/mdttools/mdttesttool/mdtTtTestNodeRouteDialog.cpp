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
  pvLoadingData = false;
  searchPath(0);
}

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
  QString relaysStr;
  bool ok;

  // Clear previous results
  lbRelays->clear();
  pvRouteData.clear();
  // Check if we can proceed now
  if(!pvGraph){
    return;
  }
  if(pvLoadingData){
    return;
  }
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
  bool ok;
  int i;

  cbUnitA->clear();
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
  bool ok;
  int i;

  cbUnitB->clear();
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
