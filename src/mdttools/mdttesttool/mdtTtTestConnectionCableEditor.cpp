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
#include "mdtTtTestConnectionCableEditor.h"
#include "ui_mdtTtTestConnectionCableEditor.h"
#include "mdtTtTestConnectionCable.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtTtTestNode.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlTableWidget.h"
#include "mdtTtTestLinkDialog.h"
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QModelIndex>
#include <QTableView>
#include <QSqlError>
#include <QString>
#include <QPushButton>
#include <QModelIndex>

#include <QDebug>

mdtTtTestConnectionCableEditor::mdtTtTestConnectionCableEditor(QWidget *parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

bool mdtTtTestConnectionCableEditor::setupTables()
{
  if(!setupTestCableTable()){
    return false;
  }
  if(!setupTestLinkTable()){
    return false;
  }
  if(!setupTestCableTestNodeUnitTable()){
    return false;
  }
  if(!setupTestCableDutUnitTable()){
    return false;
  }
  return true;
}

void mdtTtTestConnectionCableEditor::addTestNodeUnit()
{
  mdtTtTestConnectionCable tcc(0, database());
  mdtSqlSelectionDialog selectionDialog;
  mdtSqlTableSelection s;
  QVariant testCableId;
  QVariant testNodeUnitId;
  QString sql;

  // Get current test cable ID
  testCableId = currentData("TestCable_tbl", "Id_PK");
  if(testCableId.isNull()){
    return;
  }
  // Setup and show dialog
  sql = tcc.sqlForTestNodeUnitSelection(testCableId);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setMessage(tr("Select a unit that is a test connector:"));
  selectionDialog.setColumnHidden("SeriesNumber", true);
  selectionDialog.setColumnHidden("Unit_Id_FK_PK", true);
  selectionDialog.setHeaderData("Type", tr("Test system"));
  selectionDialog.setHeaderData("SubType", tr("Test node"));
  selectionDialog.setHeaderData("NodeId", tr("Node ID"));
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema\nposition"));
  selectionDialog.setHeaderData("TestNodeUnitTypeEN", tr("Type\n(English)"));
  selectionDialog.setHeaderData("TestNodeUnitTypeFR", tr("Type\n(Frensh)"));
  selectionDialog.setHeaderData("TestNodeUnitTypeDE", tr("Type\n(German)"));
  selectionDialog.setHeaderData("TestNodeUnitTypeIT", tr("Type\n(Italian)"));
  selectionDialog.addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(800, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("Unit_Id_FK_PK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  testNodeUnitId = s.data(0, "Unit_Id_FK_PK");
  // Add unit
  if(!tcc.addTestNodeUnit(testNodeUnitId, testCableId)){
    pvLastError = tcc.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestCable_TestNodeUnit_view");
}

void mdtTtTestConnectionCableEditor::removeTestNodeUnits()
{
  mdtTtTestConnectionCable tcc(0, database());
  mdtSqlTableSelection s;
  mdtSqlTableWidget *widget;
  QMessageBox msgBox;

  // Get widget and selection
  widget = sqlTableWidget("TestCable_TestNodeUnit_view");
  Q_ASSERT(widget != 0);
  s = widget->currentSelection("TestNodeUnit_Id_FK");
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove assignations between selected node units and current test cable."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove selected units
  if(!tcc.removeTestNodeUnits(s)){
    pvLastError = tcc.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestCable_TestNodeUnit_view");
}

void mdtTtTestConnectionCableEditor::addDutUnit()
{
  mdtTtTestConnectionCable tcc(0, database());
  mdtSqlSelectionDialog selectionDialog;
  mdtSqlTableSelection s;
  QVariant testCableId;
  QVariant dutUnitId;
  QString sql;

  // Get current test cable ID
  testCableId = currentData("TestCable_tbl", "Id_PK");
  if(testCableId.isNull()){
    return;
  }
  // Setup and show dialog
  sql = tcc.sqlForDutUnitSelection(testCableId);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setMessage(tr("Select a unit (DUT):"));
  selectionDialog.setColumnHidden("VehicleType_Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_PK", true);
  selectionDialog.setColumnHidden("Coordinate", true);
  selectionDialog.setColumnHidden("Cabinet", true);
  selectionDialog.setHeaderData("Type", tr("Vehicle\ntype"));
  selectionDialog.setHeaderData("SubType", tr("Vehicle\nsub type"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Vehicle\nserie"));
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema\nposition"));
  selectionDialog.setHeaderData("ArticleCode", tr("Article\ncode"));
  selectionDialog.setHeaderData("DesignationEN", tr("Designation\n(English)"));
  selectionDialog.setHeaderData("DesignationFR", tr("Designation\n(Frensh)"));
  selectionDialog.setHeaderData("DesignationDE", tr("Designation\n(German)"));
  selectionDialog.setHeaderData("DesignationIT", tr("Designation\n(Italian)"));
  selectionDialog.addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(800, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("Unit_Id_PK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  dutUnitId = s.data(0, "Unit_Id_PK");
  // Add unit
  if(!tcc.addDutUnit(dutUnitId, testCableId)){
    pvLastError = tcc.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestCable_DutUnit_view");
}

void mdtTtTestConnectionCableEditor::removeDutUnits()
{
  mdtTtTestConnectionCable tcc(0, database());
  mdtSqlTableSelection s;
  mdtSqlTableWidget *widget;
  QMessageBox msgBox;
  // Get widget and selection
  widget = sqlTableWidget("TestCable_DutUnit_view");
  Q_ASSERT(widget != 0);
  s = widget->currentSelection("DutUnit_Id_FK");
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove assignations between selected DUT units and current test cable."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove selected units
  if(!tcc.removeDutUnits(s)){
    pvLastError = tcc.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestCable_DutUnit_view");
}

void mdtTtTestConnectionCableEditor::addLink()
{
  mdtTtTestConnectionCable tcc(this, database());
  mdtTtTestLinkDialog dialog(this, database());
  QVariant cableId;
  mdtTtTestLinkData linkData;
  QVariant testNodeUnitId;
  QList<QVariant> testNodeUnitIdList;
  QVariant dutUnitId;
  QList<QVariant> dutUnitIdList;
  QSqlTableModel *m;
  QModelIndex index;
  int row;
  int col;

  // Setup link data
  if(!linkData.setup(database())){
    pvLastError = linkData.lastError();
    displayLastError();
    return;
  }
  // Get cable ID
  cableId = currentData("TestCable_tbl", "Id_PK");
  if(cableId.isNull()){
    return;
  }
  // Get test node units
  testNodeUnitId = currentData("TestCable_TestNodeUnit_view", "TestNodeUnit_Id_FK");
  m = model("TestCable_TestNodeUnit_view");
  Q_ASSERT(m != 0);
  col = m->fieldIndex("TestNodeUnit_Id_FK");
  Q_ASSERT(col >= 0);
  for(row = 0; row < m->rowCount(); ++row){
    index = m->index(row, col);
    testNodeUnitIdList.append(m->data(index));
  }
  // Get DUT units
  dutUnitId = currentData("TestCable_DutUnit_view", "DutUnit_Id_FK");
  m = model("TestCable_DutUnit_view");
  Q_ASSERT(m != 0);
  col = m->fieldIndex("DutUnit_Id_FK");
  Q_ASSERT(col >= 0);
  for(row = 0; row < m->rowCount(); ++row){
    index = m->index(row, col);
    dutUnitIdList.append(m->data(index));
  }
  // Setup and show dialog
  linkData.setValue("TestCable_Id_FK", cableId);
  dialog.setLinkData(linkData);
  dialog.setTestNodeUnitSelectionList(testNodeUnitIdList);
  dialog.setTestNodeUnit(testNodeUnitId);
  dialog.setDutUnitSelectionList(dutUnitIdList);
  dialog.setDutUnit(dutUnitId);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Update data with cable ID
  linkData = dialog.linkData();
  // Add to DB
  if(!tcc.addLink(linkData)){
    pvLastError = tcc.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestLink_view");
}

void mdtTtTestConnectionCableEditor::editLink()
{
  mdtSqlTableWidget *widget;
  mdtTtTestConnectionCable tcc(this, database());
  mdtTtTestLinkDialog dialog(this, database());
  QVariant testNodeUnitId;
  QList<QVariant> testNodeUnitIdList;
  QVariant testConnectionId;
  QVariant dutUnitId;
  QList<QVariant> dutUnitIdList;
  QVariant dutConnectionId;
  QSqlTableModel *m;
  QModelIndex index;
  int row;
  int col;
  mdtTtTestLinkData linkData;
  bool ok;

  widget = sqlTableWidget("TestLink_view");
  Q_ASSERT(widget != 0);

  // Get test and DUT connection IDs
  testConnectionId = widget->currentData("TestConnection_Id_FK");
  if(testConnectionId.isNull()){
    return;
  }
  dutConnectionId = widget->currentData("DutConnection_Id_FK");
  if(dutConnectionId.isNull()){
    return;
  }
  // Get test node units
  testNodeUnitId = widget->currentData("Unit_Id_FK_PK");
  m = model("TestCable_TestNodeUnit_view");
  Q_ASSERT(m != 0);
  col = m->fieldIndex("TestNodeUnit_Id_FK");
  Q_ASSERT(col >= 0);
  for(row = 0; row < m->rowCount(); ++row){
    index = m->index(row, col);
    testNodeUnitIdList.append(m->data(index));
  }
  // Get DUT units
  dutUnitId = widget->currentData("DutUnitId");
  m = model("TestCable_DutUnit_view");
  Q_ASSERT(m != 0);
  col = m->fieldIndex("DutUnit_Id_FK");
  Q_ASSERT(col >= 0);
  for(row = 0; row < m->rowCount(); ++row){
    index = m->index(row, col);
    dutUnitIdList.append(m->data(index));
  }
  // Get link data
  linkData = tcc.getLinkData(testConnectionId, dutConnectionId, &ok);
  if(!ok){
    pvLastError = tcc.lastError();
    displayLastError();
    return;
  }
  // Setup and show dialog
  dialog.setTestNodeUnitSelectionList(testNodeUnitIdList);
  dialog.setTestNodeUnit(testNodeUnitId);
  dialog.setDutUnitSelectionList(dutUnitIdList);
  dialog.setDutUnit(dutUnitId);
  dialog.setLinkData(linkData);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Edit link data in DB
  if(!tcc.editLink(testConnectionId, dutConnectionId, dialog.linkData())){
    pvLastError = tcc.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestLink_view");
}

void mdtTtTestConnectionCableEditor::removeLinks()
{
  mdtSqlTableWidget *widget;
  mdtTtTestConnectionCable tcc(this, database());
  QMessageBox msgBox;
  QModelIndexList indexes;

  widget = sqlTableWidget("TestLink_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  indexes = widget->indexListOfSelectedRows("Id_PK");
  if(indexes.size() < 1){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove selected links."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!tcc.removeData("TestLink_tbl", "Id_PK", indexes)){
    pvLastError = tcc.lastError();
    displayLastError();
    return;
  }
  // Update GUI
  select("TestLink_view");
}

void mdtTtTestConnectionCableEditor::generateLinks()
{
  mdtTtTestConnectionCable tcc(this, database());
  mdtTtTestNode tn(this, database());
  QVariant cableId;
  QVariant dutVehicleId;
  QVariant dutUnitId;
  QVariant testNodeId;
  QVariant dutStartConnectorId;
  QList<QVariant> dutStartConnectionIdList;
  QList<QVariant> dutLinkedConnectorIdList;
  QList<QVariant> dutEndConnectorIdList;
  QList<QVariant> dutEndConnectionIdList;
  QList<QVariant> busAtestConnectionIdList;
  QList<QVariant> busBtestConnectionIdList;
  QList<QVariant> idList;
  int i, k;

  // Get cable ID
  cableId = currentData("TestCable_tbl", "Id_PK");
  if(cableId.isNull()){
    return;
  }
  // Let user choose DUT unit and test node
  dutVehicleId = selectDutVehicleId();
  if(dutVehicleId.isNull()){
    return;
  }
  dutUnitId = selectDutUnitId(dutVehicleId);
  if(dutUnitId.isNull()){
    return;
  }
  testNodeId = selectTestNode();
  if(testNodeId.isNull()){
    return;
  }
  // Get test node's BUS channel test connections
  busAtestConnectionIdList = tn.getChannelTestConnectionIdList(testNodeId, "BUSA");
  if(busAtestConnectionIdList.isEmpty()){
    qDebug() << "There is no channel test connection available on BUSA";
    return;
  }
  busBtestConnectionIdList = tn.getChannelTestConnectionIdList(testNodeId, "BUSB");
  if(busBtestConnectionIdList.isEmpty()){
    qDebug() << "There is no channel test connection available on BUSB";
    return;
  }
  qDebug() << "BUSA cnn: " << busAtestConnectionIdList;
  qDebug() << "BUSB cnn: " << busBtestConnectionIdList;
  // Select a start connector
  dutStartConnectorId = selectStartConnectorId(dutUnitId);
  if(dutStartConnectorId.isNull()){
    return;
  }
  qDebug() << "Selected DUT: " << dutUnitId << " , Test node: " << testNodeId;
  // Load link list
  if(!tcc.loadLinkList()){
    pvLastError = tcc.lastError();
    return;
  }
  // Get start unit connections for selected connector
  dutStartConnectionIdList = tcc.getToUnitConnectorRelatedUnitConnectionIdList(dutStartConnectorId);
  qDebug() << "Start connections: " << dutStartConnectionIdList;
  for(i = 0; i < dutStartConnectionIdList.size(); ++i){
    // Get list of unit connectors that is linked to selected start unit connection
    idList = tcc.getToUnitConnectionLinkedUnitConnectorIdList(dutStartConnectionIdList.at(i));
    for(k = 0; k < idList.size(); ++k){
      if(!dutLinkedConnectorIdList.contains(idList.at(k))){
        dutLinkedConnectorIdList.append(idList.at(k));
      }
    }
  }
  // Select end connector(s)
  dutEndConnectorIdList = selectEndConnectorIdList(dutLinkedConnectorIdList);
  qDebug() << "End connectors: " << dutEndConnectorIdList;
  // Get end connections that are part of selected end connectors
  dutEndConnectionIdList = tcc.getToUnitConnectionIdListLinkedUnitConnectionIdListPartOfUnitConnectorList(dutStartConnectionIdList, dutEndConnectorIdList);
  qDebug() << "Start connections: " << dutStartConnectionIdList;
  qDebug() << "End connections: " << dutEndConnectionIdList;
  // Check that test node has needed amount of busses lines
  if(busAtestConnectionIdList.size() < dutStartConnectionIdList.size()){
    qDebug() << "Test node has to less BUSA test connections";
    return;
  }
  if(busBtestConnectionIdList.size() < dutEndConnectionIdList.size()){
    qDebug() << "Test node has to less BUSB test connections";
    return;
  }
  // Add links to link table
  /**
  if(!tcc.createTestCable(testNodeId, busAtestConnectionIdList, dutStartConnectionIdList, busBtestConnectionIdList, dutEndConnectionIdList)){
    return;
  }
  */
  if(!tcc.addLinks(testNodeId, cableId, busAtestConnectionIdList, dutStartConnectionIdList)){
    pvLastError = tcc.lastError();
    return;
  }
  if(!tcc.addLinks(testNodeId, cableId, busBtestConnectionIdList, dutEndConnectionIdList)){
    pvLastError = tcc.lastError();
    return;
  }
  // Update GUI
  select("TestLink_view");
}


/**
void mdtTtTestConnectionCableEditor::connectTestCable()
{
  mdtTtTestConnectionCable tcc(this, database());
  QVariant testCableId;
  QVariant dutVehicleId;
  QVariant testNodeId;

  // Let user choose a test cable to use
  testCableId = selectTestCable();
  if(testCableId.isNull()){
    return;
  }
  qDebug() << "Selected test cable: " << testCableId;
  // Let user choose test node
  testNodeId = selectTestNode();
  if(testNodeId.isNull()){
    return;
  }
  // Let user choose DUT vehiclke type
  dutVehicleId = selectDutVehicleId();
  if(dutVehicleId.isNull()){
    return;
  }
  // Connect test cable
  if(!tcc.connectTestCable(testCableId, testNodeId, dutVehicleId)){
    return;
  }
}
*/

/**
void mdtTtTestConnectionCableEditor::disconnectTestCable()
{
  mdtTtTestConnectionCable tcc(this, database());
  QVariant testCableId;

  // Let user choose a test cable to disconnect
  testCableId = selectTestCable();
  if(testCableId.isNull()){
    return;
  }
  // Connect test cable
  if(!tcc.disconnectTestCable(testCableId)){
    return;
  }
}
*/

QVariant mdtTtTestConnectionCableEditor::selectDutVehicleId()
{
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;

  // Setup model to show available connectors
  sql = "SELECT * FROM VehicleType_tbl ";
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get vhicle type list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCableEditor");
    pvLastError.commit();
    ///displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  selectionDialog.setMessage("Please select vehicle type that contains unit to test.");
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Id_PK", true);
  ///selectionDialog.setColumnHidden("VehicleType_Id_FK", true);
  ///selectionDialog.setHeaderData("SubType", tr("Variant"));
  ///selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(750, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtTtTestConnectionCableEditor::selectDutUnitId(const QVariant & vehicleId)
{
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;

  // Setup model to show available connectors
  sql = "SELECT * FROM VehicleType_Unit_view WHERE VehicleType_Id_FK = " + vehicleId.toString();
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get unit list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCableEditor");
    pvLastError.commit();
    ///displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  selectionDialog.setMessage("Please select unit to test.");
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Unit_Id_FK", true);
  selectionDialog.setColumnHidden("VehicleType_Id_FK", true);
  ///selectionDialog.setHeaderData("SubType", tr("Variant"));
  ///selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("Unit_Id_FK");
  selectionDialog.resize(750, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtTtTestConnectionCableEditor::selectTestNode()
{
  mdtTtTestConnectionCable tcc(this, database());
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;

  // Setup model to show available test nodes
  ///sql = "SELECT * FROM TestNode_tbl ";
  sql = tcc.sqlForTestNodeSelection();
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get test node list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCableEditor");
    pvLastError.commit();
    ///displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  selectionDialog.setMessage("Please select test node to use.");
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("VehicleType_Id_FK_PK", true);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setHeaderData("SubType", tr("Variant"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("VehicleType_Id_FK_PK");
  selectionDialog.resize(600, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtTtTestConnectionCableEditor::selectTestCable()
{
  mdtTtTestConnectionCable tcc(this, database());
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;

  // Setup model to show available test nodes
  sql = tcc.sqlForTestCableSelection();
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get test cable list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCableEditor");
    pvLastError.commit();
    ///displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  selectionDialog.setMessage("Please select test cable to use.");
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
  ///selectionDialog.setColumnHidden("Id_PK", true);
  ///selectionDialog.setHeaderData("", tr(""));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(600, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtTtTestConnectionCableEditor::selectStartConnectorId(const QVariant & dutUnitId) 
{
  mdtTtTestConnectionCable tcc(this, database());
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;

  // Setup model to show available connectors
  sql = tcc.sqlForStartConnectorSelection(dutUnitId);
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get unit connector list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCableEditor");
    pvLastError.commit();
    ///displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  selectionDialog.setMessage("Please select a connector to test.");
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_FK", true);
  selectionDialog.setColumnHidden("Connector_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnector_Id_FK", true);
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QList<QVariant> mdtTtTestConnectionCableEditor::selectEndConnectorIdList(const QList<QVariant> & unitConnectorIdList) 
{
  mdtTtTestConnectionCable tcc(this, database());
  mdtSqlSelectionDialog selectionDialog;
  QModelIndexList selectedItems;
  QList<QVariant> idList;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;
  int i;

  // Setup model to show available connectors
  sql = tcc.sqlForUnitConnectorSelectionFromUnitConnectorIdList(unitConnectorIdList);
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get unit connector list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCableEditor");
    pvLastError.commit();
    ///displayLastError();
    return idList;
  }
  */
  // Setup and show dialog
  selectionDialog.setMessage("Please select end connector(s) to test.");
  ///selectionDialog.setModel(&model, true);
  selectionDialog.setQuery(sql, database(), true);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_FK", true);
  selectionDialog.setColumnHidden("Connector_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnector_Id_FK", true);
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return idList;
  }
  selectedItems = selectionDialog.selectionResults();
  for(i = 0; i < selectedItems.size(); ++i){
    idList.append(selectedItems.at(i).data());
  }

  return idList;  
}

bool mdtTtTestConnectionCableEditor::setupTestCableTable()
{
  Ui::mdtTtTestConnectionCableEditor tcce;

  // Setup main form widget
  tcce.setupUi(mainSqlWidget());
  // Setup form
  if(!setMainTable("TestCable_tbl", "Test cable", database())){
    return false;
  }
  // Force a update
  ///mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());

  return true;
}

bool mdtTtTestConnectionCableEditor::setupTestLinkTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddLink;
  QPushButton *pbEditLink;
  ///QPushButton *pbGenerateLinks;
  QPushButton *pbRemoveLinks;

  if(!addChildTable("TestLink_view", tr("Links"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "TestLink_view", "TestCable_Id_FK")){
    return false;
  }
  widget = sqlTableWidget("TestLink_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("TestCable_Id_FK", true);
  widget->setColumnHidden("TestConnection_Id_FK", true);
  widget->setColumnHidden("DutConnection_Id_FK", true);
  widget->setColumnHidden("VehicleType_Id_FK_PK", true);
  widget->setColumnHidden("Unit_Id_FK_PK", true);
  widget->setColumnHidden("DutUnitId", true);
  // Set fields a user friendly name
  widget->setHeaderData("TestNodeType", tr("Test node\nType"));
  widget->setHeaderData("TestNodeSubType", tr("Test node\nSub type"));
  widget->setHeaderData("TestNodeSeriesNumber", tr("Test node\nSerie"));
  widget->setHeaderData("NodeId", tr("Test node\nNode ID"));
  widget->setHeaderData("TestNodeUnitSchemaPosition", tr("Test node\nUnit"));
  widget->setHeaderData("TestConnectorName", tr("Test\nConnector"));
  widget->setHeaderData("TestContactName", tr("Test\nContact"));
  widget->setHeaderData("TestLinkIdentification", tr("Test link\nIdentification"));
  widget->setHeaderData("TestLinkValue", tr("Test link\nR [Ohm]"));
  widget->setHeaderData("DutUnitSchemaPosition", tr("DUT\nSchema pos."));
  widget->setHeaderData("DutUnitAlias", tr("DUT\nAlias"));
  widget->setHeaderData("DutConnectorName", tr("DUT\nConnector"));
  widget->setHeaderData("DutContactName", tr("DUT\nContact"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons
  /**
  pbGenerateLinks = new QPushButton(tr("Generate links ..."));
  connect(pbGenerateLinks, SIGNAL(clicked()), this, SLOT(generateLinks()));
  widget->addWidgetToLocalBar(pbGenerateLinks);
  */
  // Add link button
  pbAddLink = new QPushButton(tr("Add link ..."));
  pbAddLink->setIcon(QIcon::fromTheme("document-new"));
  connect(pbAddLink, SIGNAL(clicked()), this, SLOT(addLink()));
  widget->addWidgetToLocalBar(pbAddLink);
  // Edit link button
  pbEditLink = new QPushButton(tr("Edit link"));
  connect(pbEditLink, SIGNAL(clicked()), this, SLOT(editLink()));
  widget->addWidgetToLocalBar(pbEditLink);
  // Remove links button
  pbRemoveLinks = new QPushButton(tr("Remove links ..."));
  pbRemoveLinks->setIcon(QIcon::fromTheme("edit-delete"));
  connect(pbRemoveLinks, SIGNAL(clicked()), this, SLOT(removeLinks()));
  widget->addWidgetToLocalBar(pbRemoveLinks);
  // Add stretch
  widget->addStretchToLocalBar();

  return true;
}

bool mdtTtTestConnectionCableEditor::setupTestCableTestNodeUnitTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddUnit;
  QPushButton *pbRemoveUnits;

  // Add link table
  if(!addChildTable("TestCable_TestNodeUnit_view", tr("Test node units"), database())){
    return false;
  }
  // Setup relation
  if(!addRelation("Id_PK", "TestCable_TestNodeUnit_view", "TestCable_Id_FK")){
    return false;
  }
  // Get widget to continue setup
  widget = sqlTableWidget("TestCable_TestNodeUnit_view");
  Q_ASSERT(widget != 0);
  ///widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("TestNodeUnit_Id_FK", true);
  widget->setColumnHidden("TestCable_Id_FK", true);
  widget->setColumnHidden("Unit_Id_FK_PK", true);
  widget->setColumnHidden("TestNode_Id_FK", true);
  widget->setColumnHidden("Type_Code_FK", true);
  widget->setColumnHidden("Composite_Id_FK", true);
  widget->setColumnHidden("Article_Id_FK", true);
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("SeriesNumber", true);
  widget->setHeaderData("SchemaPosition", tr("Schema\nposition"));
  widget->setHeaderData("IoPosition", tr("I/O\nposition"));
  widget->setHeaderData("Type", tr("Test system"));
  widget->setHeaderData("SubType", tr("Test node"));
  widget->setHeaderData("TestNodeUnitTypeEN", tr("Type\n(English)"));
  widget->setHeaderData("TestNodeUnitTypeFR", tr("Type\n(Frensh)"));
  widget->setHeaderData("TestNodeUnitTypeDE", tr("Type\n(German)"));
  widget->setHeaderData("TestNodeUnitTypeIT", tr("Type\n(Italian)"));
  widget->addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  widget->sort();
  widget->tableView()->resizeColumnsToContents();
  widget->tableView()->resizeRowsToContents();
  // Add unit button
  pbAddUnit = new QPushButton(tr("Add ..."));
  pbAddUnit->setIcon(QIcon::fromTheme("list-add"));
  widget->addWidgetToLocalBar(pbAddUnit);
  connect(pbAddUnit, SIGNAL(clicked()), this, SLOT(addTestNodeUnit()));
  // Remove units button
  pbRemoveUnits = new QPushButton(tr("Remove ..."));
  pbRemoveUnits->setIcon(QIcon::fromTheme("list-remove"));
  widget->addWidgetToLocalBar(pbRemoveUnits);
  connect(pbRemoveUnits, SIGNAL(clicked()), this, SLOT(removeTestNodeUnits()));
  widget->addStretchToLocalBar();

  return true;
}

bool mdtTtTestConnectionCableEditor::setupTestCableDutUnitTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddUnit;
  QPushButton *pbRemoveUnits;

  // Add link table
  if(!addChildTable("TestCable_DutUnit_view", tr("DUT units"), database())){
    return false;
  }
  // Setup relation
  if(!addRelation("Id_PK", "TestCable_DutUnit_view", "TestCable_Id_FK")){
    return false;
  }
  // Get widget to continue setup
  widget = sqlTableWidget("TestCable_DutUnit_view");
  Q_ASSERT(widget != 0);
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("DutUnit_Id_FK", true);
  widget->setColumnHidden("TestCable_Id_FK", true);
  widget->setColumnHidden("Composite_Id_FK", true);
  widget->setColumnHidden("Article_Id_FK", true);
  widget->setHeaderData("SchemaPosition", tr("Schema\nposition"));
  widget->setHeaderData("DesignationEN", tr("Designation\n(English)"));
  widget->setHeaderData("DesignationFR", tr("Designation\n(Frensh)"));
  widget->setHeaderData("DesignationDE", tr("Designation\n(German)"));
  widget->setHeaderData("DesignationIT", tr("Designation\n(Italian)"));
  widget->addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  widget->sort();
  widget->tableView()->resizeColumnsToContents();
  widget->tableView()->resizeRowsToContents();
  // Add unit button
  pbAddUnit = new QPushButton(tr("Add ..."));
  pbAddUnit->setIcon(QIcon::fromTheme("list-add"));
  widget->addWidgetToLocalBar(pbAddUnit);
  connect(pbAddUnit, SIGNAL(clicked()), this, SLOT(addDutUnit()));
  // Remove units button
  pbRemoveUnits = new QPushButton(tr("Remove ..."));
  pbRemoveUnits->setIcon(QIcon::fromTheme("list-remove"));
  widget->addWidgetToLocalBar(pbRemoveUnits);
  connect(pbRemoveUnits, SIGNAL(clicked()), this, SLOT(removeDutUnits()));
  widget->addStretchToLocalBar();

  return true;
}
