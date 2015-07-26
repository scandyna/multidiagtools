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
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include <QSqlRecord>
#include <QStringList>
#include <QComboBox>
#include <QMessageBox>
#include <QLocale>
#include <QGroupBox>
#include <QRadioButton>
#include <QLineEdit>

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
  connect(cbConnectionA, SIGNAL(currentIndexChanged(int)), this, SLOT(buildRouteAuto(int)));
  connect(cbConnectionB, SIGNAL(currentIndexChanged(int)), this, SLOT(buildRouteAuto(int)));
  connect(tbAddRelay, SIGNAL(clicked()), this, SLOT(selectAndAddRelay()));
  connect(rbAuto, SIGNAL(toggled(bool)), this, SLOT(onRbAutoToggled(bool)));
  rbAuto->setChecked(true);
  leName->setEnabled(false);
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
  clearRoute();
}

void mdtTtTestNodeRouteDialog::onRbAutoToggled(bool checked)
{
  if(checked){
    leRelays->setEnabled(false);
    tbAddRelay->setEnabled(false);
    buildRouteAuto(0);
  }else{
    leRelays->setEnabled(true);
    tbAddRelay->setEnabled(true);
  }
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

// void mdtTtTestNodeRouteDialog::searchPath(int index)
// {
//   mdtTtTestNodeRoute tnr(pvDatabase);
//   int cbIndexA, cbIndexB;
//   QVariant connectionIdA, connectionIdB;
//   QString relaysStr;
//   bool ok;
// 
//   // If we are in manual mode, we do nothing
//   if(!rbAuto->isChecked()){
//     return;
//   }
//   // Clear previous results
//   leRelays->clear();
//   pvRouteData.clear();
//   // Check if we can proceed now
//   if(!pvGraph){
//     return;
//   }
//   if(pvLoadingData){
//     return;
//   }
//   // Get selected A and B connection IDs
//   cbIndexA = cbConnectionA->currentIndex();
//   cbIndexB = cbConnectionB->currentIndex();
//   if((cbIndexA < 0)||(cbIndexB < 0)){
//     return;
//   }
//   connectionIdA = cbConnectionA->itemData(cbIndexA);
//   connectionIdB = cbConnectionB->itemData(cbIndexB);
//   if((connectionIdA.isNull())||(connectionIdB.isNull())){
//     return;
//   }
//   if(connectionIdA == connectionIdB){
//     return;
//   }
//   // Build path
//   Q_ASSERT(pvGraph);
//   pvRouteData = tnr.buildRoute(pvTestNodeId, connectionIdA, connectionIdB, *pvGraph, ok);
//   if(!ok){
//     displayError(tnr.lastError());
//     return;
//   }
//   // Build relays list string
//   for(const auto & relay : pvRouteData.relaysToEnableVector()){
//     if(relaysStr.isEmpty()){
//       relaysStr = relay.schemaPosition.toString();
//     }else{
//       relaysStr += ", " + relay.schemaPosition.toString();
//     }
//   }
//   // Display result
//   leRelays->setText(relaysStr);
// }

void mdtTtTestNodeRouteDialog::selectAndAddRelay()
{
  mdtSqlSelectionDialog dialog(this);
  QString sql;
  QStringList existingRelays = leRelays->text().split(",");
  mdtSqlTableSelection s;

  // Setup query
  sql = "SELECT U.SchemaPosition";
  sql += " FROM TestNodeUnit_tbl TNU JOIN Unit_tbl U ON U.Id_PK = TNU.Unit_Id_FK_PK";
  sql += " WHERE TNU.TestNode_Id_FK = " + pvTestNodeId.toString();
  sql += " AND (TNU.Type_Code_FK = 'BUSCPLREL' OR TNU.Type_Code_FK = 'CHANELREL')";
  if(!existingRelays.isEmpty()){
    sql += " AND U.SchemaPosition NOT IN (";
    for(int i = 0; i < existingRelays.size(); ++i){
      const auto sp = existingRelays.at(i).trimmed();
      if(!sp.isEmpty()){
        if(i > 0){
          sql += ",";
        }
        sql += "'" + sp + "'";
      }
    }
    sql += ")";
  }
  // Setup and show dialog
  dialog.setQuery(sql, pvDatabase, false);
  dialog.setMessage(tr("Select relay to add:"));
  dialog.setHeaderData("SchemaPosition", tr("Schema position"));
  dialog.addColumnToSortOrder("SchemaPosition");
  dialog.setWindowTitle(tr("Route relay selection"));
  dialog.sort();
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Update relays line edit - Route will be rebuilt and checked when user accepts dialog
  s = dialog.selection("SchemaPosition");
  if(s.isEmpty()){
    return;
  }
  QList<QVariant> dataList = s.dataList("SchemaPosition");
  QString str = leRelays->text();
  if(!str.trimmed().isEmpty()){
    str += ", ";
  }
  for(int i = 0; i < dataList.size(); ++i){
    if(i > 0){
      str += ", ";
    }
    str += dataList.at(i).toString();
  }
  leRelays->setText(str);
}

void mdtTtTestNodeRouteDialog::accept()
{
  mdtTtTestNodeRoute tnr(pvDatabase);
  bool ok;

  // In manual mode we have to build the route
  if(!rbAuto->isChecked()){
    if(!buildRouteManual()){
      return;
    }
  }
  // If a route with same name as freshly created one allready exists, we warn the user
  if(tnr.routeExist(pvTestNodeId, pvRouteData.name().toString(), ok)){
    QMessageBox msgBox(this);
    msgBox.setText(tr("A route with same name allready exists."));
    msgBox.setInformativeText(tr("Do you want to save this route ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  QDialog::accept();
}

void mdtTtTestNodeRouteDialog::buildRouteAuto(int)
{
  mdtTtTestNodeRoute tnr(pvDatabase);
  QString relaysStr;
  bool ok;

  // If we are in manual mode, we do nothing (this slot is allways called)
  if(!rbAuto->isChecked()){
    return;
  }
  // Clear previous results
  clearRoute();
  // Check if we can proceed now
  if(!pvGraph){
    return;
  }
  if(pvLoadingData){
    return;
  }
  // Set selected connection
  if(!setSelectedConnectionsToRoute()){
    return;
  }
  // Build route
  Q_ASSERT(pvGraph);
  pvRouteData = tnr.buildRoute(pvTestNodeId, pvRouteData.connectionAId(), pvRouteData.connectionBId(), *pvGraph, ok);
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
  leRelays->setText(relaysStr);
  leName->setText(pvRouteData.name().toString());
}

bool mdtTtTestNodeRouteDialog::buildRouteManual()
{
  Q_ASSERT(!rbAuto->isChecked());
  Q_ASSERT(!pvLoadingData);

  mdtTtTestNode tn(pvDatabase);
  mdtTtTestNodeRoute tnr(pvDatabase);
  QStringList relaysSpList;
  QStringList tmpList;
  std::vector<mdtTtTestNodeRouteRelay> relays;

  // Clear previous results
  pvRouteData.clear();
  // Set selected connection and test node
  if(!setSelectedConnectionsToRoute()){
    return false;
  }
  pvRouteData.setTestNodeId(pvTestNodeId);
  // Get relays
  tmpList = leRelays->text().trimmed().split(",");
  qDebug() << "le text: " << leRelays->text();
  qDebug() << "tmpList: " << tmpList;
  relaysSpList.reserve(tmpList.size());
  for(const auto & sp : tmpList){
    relaysSpList.append(sp.trimmed());
  }
  qDebug() << "relaysSpList: " << relaysSpList;
  relays = tn.getTestNodeRouteRelays(pvTestNodeId, relaysSpList);
  if(relays.empty()){
    displayError(tn.lastError());
    pvRouteData.clear();
    return false;
  }
  for(const auto & relay : relays){
    pvRouteData.addRelayToEnable(relay.id, relay.schemaPosition, relay.ioPosition);
  }
  // Set route name
  if(!tnr.setRouteName(pvRouteData)){
    displayError(tnr.lastError());
    pvRouteData.clear();
    return false;
  }
  // Display result
  leName->setText(pvRouteData.name().toString());

  return true;
}

bool mdtTtTestNodeRouteDialog::setSelectedConnectionsToRoute()
{
  int cbIndexA, cbIndexB;
  QVariant connectionIdA, connectionIdB;

  // Get selected A and B connection IDs
  cbIndexA = cbConnectionA->currentIndex();
  cbIndexB = cbConnectionB->currentIndex();
  if((cbIndexA < 0)||(cbIndexB < 0)){
    return false;
  }
  connectionIdA = cbConnectionA->itemData(cbIndexA);
  connectionIdB = cbConnectionB->itemData(cbIndexB);
  if((connectionIdA.isNull())||(connectionIdB.isNull())){
    return false;
  }
  if(connectionIdA == connectionIdB){
    return false;
  }
  // Update route data
  pvRouteData.setConnectionAId(connectionIdA);
  pvRouteData.setConnectionBId(connectionIdB);

  return true;
}

void mdtTtTestNodeRouteDialog::clearRoute()
{
  leRelays->clear();
  pvRouteData.clear();
  leName->clear();
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
  dataList = tn.getDataList<QSqlRecord>(sqlForUnitTypeData(), ok);
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
  dataList = tn.getDataList<QSqlRecord>(sqlForUnitData(unitType), ok);
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

  dataList = tn.getDataList<QSqlRecord>(sqlForConnectionData(testNodeUnitId), ok);
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
  dataList = tn.getDataList<QSqlRecord>(sqlForUnitTypeData(), ok);
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
  dataList = tn.getDataList<QSqlRecord>(sqlForUnitData(unitType), ok);
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

  dataList = tn.getDataList<QSqlRecord>(sqlForConnectionData(testNodeUnitId), ok);
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
