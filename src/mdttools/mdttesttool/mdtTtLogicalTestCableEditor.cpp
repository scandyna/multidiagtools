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
#include "mdtTtLogicalTestCableEditor.h"
#include "ui_mdtTtLogicalTestCableEditor.h"
#include "mdtTtLogicalTestCable.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtTtTestNode.h"
#include "mdtSqlRelation.h"
#include "mdtSqlTableWidget.h"
#include "mdtTtTestLinkDialog.h"
#include "mdtSqlTableViewController.h"
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QModelIndex>
#include <QTableView>
#include <QSqlError>
#include <QString>
#include <QPushButton>
#include <QModelIndex>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <memory>

#include <QDebug>

mdtTtLogicalTestCableEditor::mdtTtLogicalTestCableEditor(QWidget *parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

bool mdtTtLogicalTestCableEditor::setupTables()
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

void mdtTtLogicalTestCableEditor::addTestNodeUnit()
{
  mdtTtLogicalTestCable tcc(0, database());
  mdtSqlSelectionDialog selectionDialog;
  mdtSqlTableSelection s;
  QVariant testCableId;
  QVariant testNodeUnitId;
  QString sql;

  // Get current test cable ID
  testCableId = currentData("LogicalTestCable_tbl", "Id_PK");
  if(testCableId.isNull()){
    return;
  }
  // Setup and show dialog
  sql = tcc.sqlForTestNodeUnitSelection(testCableId);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setMessage(tr("Select test connector to use:"));
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
  selectionDialog.addColumnToSortOrder("Type", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("SubType", Qt::AscendingOrder);
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
  select("LogicalTestCable_TestNodeUnit_view");
}

void mdtTtLogicalTestCableEditor::removeTestNodeUnits()
{
  mdtTtLogicalTestCable tcc(0, database());
  mdtSqlTableSelection s;
  mdtSqlTableWidget *widget;
  QMessageBox msgBox;

  // Get widget and selection
  widget = sqlTableWidget("LogicalTestCable_TestNodeUnit_view");
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
  select("LogicalTestCable_TestNodeUnit_view");
}

void mdtTtLogicalTestCableEditor::addDutUnit()
{
  mdtTtLogicalTestCable tcc(0, database());
  mdtSqlSelectionDialog selectionDialog;
  mdtSqlTableSelection s;
  QVariant testCableId;
  QVariant dutUnitId;
  QString sql;

  // Get current test cable ID
  testCableId = currentData("LogicalTestCable_tbl", "Id_PK");
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
  select("LogicalTestCable_DutUnit_view");
}

void mdtTtLogicalTestCableEditor::removeDutUnits()
{
  mdtTtLogicalTestCable tcc(0, database());
  mdtSqlTableSelection s;
  mdtSqlTableWidget *widget;
  QMessageBox msgBox;
  // Get widget and selection
  widget = sqlTableWidget("LogicalTestCable_DutUnit_view");
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
  select("LogicalTestCable_DutUnit_view");
}

void mdtTtLogicalTestCableEditor::addLink()
{
  mdtTtLogicalTestCable tcc(this, database());
  mdtTtTestLinkDialog dialog(this, database());
  QVariant cableId;
  mdtTtTestLinkData linkData;
  QVariant testNodeUnitId;
  QList<QVariant> testNodeUnitIdList;
  QVariant dutUnitId;
  QList<QVariant> dutUnitIdList;

  // Setup link data
  if(!linkData.setup(database())){
    pvLastError = linkData.lastError();
    displayLastError();
    return;
  }
  // Get cable ID
  cableId = currentData("LogicalTestCable_tbl", "Id_PK");
  if(cableId.isNull()){
    return;
  }
  // Get test node units
  testNodeUnitId = currentData("LogicalTestCable_TestNodeUnit_view", "TestNodeUnit_Id_FK");
  testNodeUnitIdList = dataList("LogicalTestCable_TestNodeUnit_view", "TestNodeUnit_Id_FK");
  // Get DUT units
  dutUnitId = currentData("LogicalTestCable_DutUnit_view", "DutUnit_Id_FK");
  dutUnitIdList = dataList("LogicalTestCable_DutUnit_view", "DutUnit_Id_FK");
  // Setup and show dialog
  linkData.setValue("LogicalTestCable_Id_FK", cableId);
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

void mdtTtLogicalTestCableEditor::editLink()
{
  mdtSqlTableWidget *widget;
  mdtTtLogicalTestCable tcc(this, database());
  mdtTtTestLinkDialog dialog(this, database());
  QVariant testNodeUnitId;
  QList<QVariant> testNodeUnitIdList;
  QVariant testConnectionId;
  QVariant dutUnitId;
  QList<QVariant> dutUnitIdList;
  QVariant dutConnectionId;
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
  testNodeUnitIdList = dataList("LogicalTestCable_TestNodeUnit_view", "TestNodeUnit_Id_FK");
  // Get DUT units
  dutUnitId = widget->currentData("DutUnitId");
  dutUnitIdList = dataList("LogicalTestCable_DutUnit_view", "DutUnit_Id_FK");
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

void mdtTtLogicalTestCableEditor::removeLinks()
{
  mdtSqlTableWidget *widget;
  mdtTtLogicalTestCable tcc(this, database());
  QMessageBox msgBox;
  mdtSqlTableSelection s;

  widget = sqlTableWidget("TestLink_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("Id_PK");
  if(s.isEmpty()){
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
  if(!tcc.removeData("TestLink_tbl", s, true)){
    pvLastError = tcc.lastError();
    displayLastError();
    return;
  }
  // Update GUI
  select("TestLink_view");
}

void mdtTtLogicalTestCableEditor::generateLinks()
{
  mdtTtLogicalTestCable tcc(this, database());
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
  cableId = currentData("LogicalTestCable_tbl", "Id_PK");
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
void mdtTtLogicalTestCableEditor::connectTestCable()
{
  mdtTtLogicalTestCable tcc(this, database());
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
void mdtTtLogicalTestCableEditor::disconnectTestCable()
{
  mdtTtLogicalTestCable tcc(this, database());
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

QVariant mdtTtLogicalTestCableEditor::selectDutVehicleId()
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
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtLogicalTestCableEditor");
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

QVariant mdtTtLogicalTestCableEditor::selectDutUnitId(const QVariant & vehicleId)
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
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtLogicalTestCableEditor");
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

QVariant mdtTtLogicalTestCableEditor::selectTestNode()
{
  mdtTtLogicalTestCable tcc(this, database());
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
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtLogicalTestCableEditor");
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

QVariant mdtTtLogicalTestCableEditor::selectTestCable()
{
  mdtTtLogicalTestCable tcc(this, database());
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
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtLogicalTestCableEditor");
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

QVariant mdtTtLogicalTestCableEditor::selectStartConnectorId(const QVariant & dutUnitId) 
{
  mdtTtLogicalTestCable tcc(this, database());
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
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtLogicalTestCableEditor");
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

QList<QVariant> mdtTtLogicalTestCableEditor::selectEndConnectorIdList(const QList<QVariant> & unitConnectorIdList) 
{
  mdtTtLogicalTestCable tcc(this, database());
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
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtLogicalTestCableEditor");
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

bool mdtTtLogicalTestCableEditor::setupTestCableTable()
{
  ///Ui::mdtTtLogicalTestCableEditor tcce;

  // Setup main form widget
  ///tcce.setupUi(mainSqlWidget());
  setMainTableUi<Ui::mdtTtLogicalTestCableEditor>();
  // Setup form
  if(!setMainTable("LogicalTestCable_tbl", "Test cable", database())){
    return false;
  }
  // Force a update
  ///mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());

  return true;
}

bool mdtTtLogicalTestCableEditor::setupTestLinkTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddLink;
  QPushButton *pbEditLink;
  ///QPushButton *pbGenerateLinks;
  QPushButton *pbRemoveLinks;
  mdtSqlRelationInfo relationInfo;

  // Add test link table
  relationInfo.setChildTableName("TestLink_view");
  relationInfo.addRelation("Id_PK", "LogicalTestCable_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Links"))){
    return false;
  }
  // Get added sql widget and do some setup on it
  widget = sqlTableWidget("TestLink_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("TestCableUnitConnectionStart_Id_FK", true);
  widget->setColumnHidden("TestCableUnitConnectionEnd_Id_FK", true);
  widget->setColumnHidden("TestConnection_Id_FK", true);
  widget->setColumnHidden("DutConnection_Id_FK", true);
  widget->setColumnHidden("LogicalTestCable_Id_FK", true);
  widget->setColumnHidden("LogicalTestCableKey", true);
  widget->setColumnHidden("DutUnitConnector_Id_FK", true);
  widget->setColumnHidden("DutUnit_Id_FK", true);
  widget->setColumnHidden("TestNodeUnitConnector_Id_FK", true);
  
//   widget->setColumnHidden("TestNode_Id_FK", true);


//   widget->setColumnHidden("VehicleType_Id_FK_PK", true);
//   widget->setColumnHidden("Unit_Id_FK_PK", true);
//   widget->setColumnHidden("DutUnitId", true);
//   widget->setColumnHidden("TestNodeUnitSchemaPosition", true);
//   widget->setColumnHidden("IoPosition", true);
  // Set fields a user friendly name
  widget->setHeaderData("DutSchemaPosition", tr("DUT\nschema pos."));
  widget->setHeaderData("DutAlias", tr("DUT\nalias"));
  widget->setHeaderData("DutConnectorName", tr("DUT\nconnector"));
  widget->setHeaderData("DutContactName", tr("DUT\ncontact"));
  widget->setHeaderData("TestCableDutSideConnectorName", tr("Test cable\nDUT side\nconnector"));
  widget->setHeaderData("TestCableDutSideContactName", tr("Test cable\nDUT side\ncontact"));
  widget->setHeaderData("TestCableLinkIdentification", tr("Test cable\nlink\nidentification"));
  widget->setHeaderData("TestCableLinkResistance", tr("Test cable\nlink\nresistance"));
  widget->setHeaderData("TestCableTestSideConnectorName", tr("Test cable\nTest node side\nconnector"));
  widget->setHeaderData("TestCableTestSideContactName", tr("Test cable\nTest node side\ncontact"));
  widget->setHeaderData("TestNodeConnectorName", tr("Test node\nconnector"));
  widget->setHeaderData("TestNodeContactName", tr("Test node\ncontact"));
  
  //widget->setHeaderData("LogicalTestCableKey", tr("Test node\nType"));
//   widget->setHeaderData("TestNodeType", tr("Test node\nType"));
//   widget->setHeaderData("TestNodeSubType", tr("Test node\nSub type"));
//   widget->setHeaderData("TestNodeSeriesNumber", tr("Test node\nSerie"));
//   widget->setHeaderData("NodeIdentification", tr("Test node\nNode ID"));
//   ///widget->setHeaderData("TestNodeUnitSchemaPosition", tr("Test node\nUnit"));
//   widget->setHeaderData("TestConnectorName", tr("Test\nConnector"));
//   widget->setHeaderData("TestContactName", tr("Test\nContact"));
//   widget->setHeaderData("TestLinkIdentification", tr("Test link\nIdentification"));
//   widget->setHeaderData("TestLinkValue", tr("Test link\nR [Ohm]"));
//   
//   
//   
//   
//   
//   widget->setHeaderData("Type", tr("DUT\nVehicle type"));
//   widget->setHeaderData("SubType", tr("DUT\nVehicle sub type"));
//   widget->setHeaderData("SeriesNumber", tr("DUT\nVehicle type serie"));
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

bool mdtTtLogicalTestCableEditor::setupTestCableTestNodeUnitTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddUnit;
  QPushButton *pbRemoveUnits;
  mdtSqlRelationInfo relationInfo;

  // Add link table
  relationInfo.setChildTableName("LogicalTestCable_TestNodeUnit_view");
  relationInfo.addRelation("Id_PK", "LogicalTestCable_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Test node units"))){
    return false;
  }
  /**
  if(!addChildTable("TestCable_TestNodeUnit_view", tr("Test node units"), database())){
    return false;
  }
  // Setup relation
  if(!addRelation("Id_PK", "TestCable_TestNodeUnit_view", "LogicalTestCable_Id_FK")){
    return false;
  }
  */
  // Get widget to continue setup
  widget = sqlTableWidget("LogicalTestCable_TestNodeUnit_view");
  Q_ASSERT(widget != 0);
  ///widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("TestNodeUnit_Id_FK", true);
  widget->setColumnHidden("LogicalTestCable_Id_FK", true);
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

bool mdtTtLogicalTestCableEditor::setupTestCableDutUnitTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddUnit;
  QPushButton *pbRemoveUnits;

  mdtSqlRelationInfo relationInfo;

  // Add DUT unit view
  relationInfo.setChildTableName("LogicalTestCable_DutUnit_view");
  relationInfo.addRelation("Id_PK", "LogicalTestCable_Id_FK", false);
  if(!addChildTable(relationInfo, tr("DUT units"))){
    return false;
  }
  /**
  // Add link table
  if(!addChildTable("TestCable_DutUnit_view", tr("DUT units"), database())){
    return false;
  }
  // Setup relation
  if(!addRelation("Id_PK", "TestCable_DutUnit_view", "LogicalTestCable_Id_FK")){
    return false;
  }
  */
  // Get widget to continue setup
  widget = sqlTableWidget("LogicalTestCable_DutUnit_view");
  Q_ASSERT(widget != 0);
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("DutUnit_Id_FK", true);
  widget->setColumnHidden("LogicalTestCable_Id_FK", true);
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
