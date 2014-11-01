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
#include "mdtTtTestCableEditor.h"
#include "ui_mdtTtTestCableEditor.h"
#include "mdtSqlDataWidgetController.h"
#include "mdtSqlTableViewController.h"
#include "mdtSqlRelationInfo.h"
#include "mdtSqlTableWidget.h"
#include "mdtClUnit.h"
#include "mdtClUnitConnectionDialog.h"
#include "mdtClUnitVehicleType.h"
#include "mdtClUnitLinkDialog.h"
#include "mdtClLink.h"
#include "mdtSqlTableSelection.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtTtLogicalTestCableEditor.h"
#include "mdtTtLogicalTestCable.h"
#include "mdtSqlDialog.h"
#include "mdtTtTestCableOffsetTool.h"
#include "mdtTtTestLinkData.h"

#include "mdtTtLogicalTestCableDialog.h"

#include <QPushButton>
#include <QInputDialog>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <memory>

#include <QDebug>

mdtTtTestCableEditor::mdtTtTestCableEditor(QWidget* parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
  pvCableOffsetTool = new mdtTtTestCableOffsetTool(db, this);
}

bool mdtTtTestCableEditor::setupTables()
{
  if(!setupTestCableTable()){
    return false;
  }
  if(!setupTestNodeTable()){
    return false;
  }
  if(!setupUnitConnectorTable()){
    return false;
  }
  if(!setupUnitConnectionTable()){
    return false;
  }
  if(!setupUnitLinkTable()){
    return false;
  }
  if(!setupLogicalTestCableTable()){
    return false;
  }
  return true;
}

void mdtTtTestCableEditor::addMenus(QMenuBar* menuBar)
{
  Q_ASSERT(menuBar != 0);

  QMenu *offsetResetMenu;
  QAction *actRunOffsetReset;
  QAction *actSelectOffsetResetTestModel;

  offsetResetMenu = menuBar->addMenu(tr("&Offset reset"));
  // Run offset reset action
  actRunOffsetReset = offsetResetMenu->addAction(QIcon::fromTheme("system-run"), tr("&Run"));
  connect(actRunOffsetReset, SIGNAL(triggered(bool)), this, SLOT(runOffsetReset()));
  // Select offset reset action
  actSelectOffsetResetTestModel = offsetResetMenu->addAction(tr("&Select test"));
  connect(actSelectOffsetResetTestModel, SIGNAL(triggered(bool)), this, SLOT(selectOffsetResetTestModel()));
}

void mdtTtTestCableEditor::assignTestNode()
{
  mdtSqlSelectionDialog selectionDialog;
  QVariant unitId;
  mdtClUnitVehicleType uvt(0, database());
  QString sql;
  mdtSqlTableSelection s;

  // Get current unit ID
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  // Build SQL query to get the list of test nodes that are not allready assigned
  sql = "SELECT VehicleType_Id_FK_PK, Type, SubType, NodeIdentification, DeviceIdentification, SeriesNumber "\
        "FROM TestNode_view "\
        "WHERE VehicleType_Id_FK_PK NOT IN ( "\
        " SELECT VehicleType_Id_FK "\
        " FROM VehicleType_Unit_tbl "\
        " WHERE Unit_Id_FK = " + unitId.toString() + " ) ";
  // Setup and show dialog
  selectionDialog.setMessage("Please select a test node (or group):");
  selectionDialog.setQuery(sql, database(), true);
  selectionDialog.setColumnHidden("VehicleType_Id_FK_PK", true);
  selectionDialog.setHeaderData("Type", tr("Test system"));
  selectionDialog.setHeaderData("SubType", tr("Test node or group"));
  selectionDialog.addColumnToSortOrder("Type", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("SubType", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(700, 300);
  selectionDialog.setWindowTitle(tr("Test node assignation"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  // Proceed insertion
  s = selectionDialog.selection("VehicleType_Id_FK_PK");
  if(!uvt.addUnitVehicleTypeAssignments(unitId, s.dataList("VehicleType_Id_FK_PK"))){
    pvLastError = uvt.lastError();
    displayLastError();
    return;
  }
  select("Unit_TestNode_view");
}

void mdtTtTestCableEditor::removeTestNodeAssignation()
{
  QVariant unitId;
  QSqlError sqlError;
  int ret;
  QMessageBox msgBox;
  mdtSqlTableWidget *widget;
  mdtSqlTableSelection s;
  mdtClUnitVehicleType uvt(0, database());

  // Get test node widget
  widget = sqlTableWidget("Unit_TestNode_view");
  Q_ASSERT(widget != 0);
  Q_ASSERT(widget->selectionModel() != 0);
  // Get current unit ID
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  // Get selected rows
  s = widget->currentSelection("VehicleType_Id_FK_PK");
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove assignations between selected test system and current test cable."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  msgBox.setWindowTitle(tr("Test node assignation removal"));
  ret = msgBox.exec();
  if(ret != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!uvt.removeUnitVehicleAssignments(unitId, s.dataList("VehicleType_Id_FK_PK"))){
    pvLastError = uvt.lastError();
    displayLastError();
    return;
  }
  select("Unit_TestNode_view");
}

void mdtTtTestCableEditor::addConnectorBasedConnector()
{
  mdtClUnit unit(this, database());
  mdtClUnitConnectorData connectorData;
  QVariant unitId;
  QVariant baseConnectorId;
  QVariant connectorName;
  QList<QVariant> contactList;
  QInputDialog dialog;

  // Get unit ID
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  // Select connector from Connector_tbl
  baseConnectorId = selectBaseConnector();
  if(baseConnectorId.isNull()){
    return;
  }
  // Set connector name
  dialog.setLabelText(tr("Connector name:"));
  dialog.setTextValue(connectorName.toString());
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  connectorName = dialog.textValue().trimmed();
  if(connectorName.isNull()){
    return;
  }
  // Select contacts
  contactList = selectBaseConnectorContactIdList(baseConnectorId);
  if(contactList.isEmpty()){
    return;
  }
  // Setup unit connector data
  if(!connectorData.setup(database(), true, false)){
    pvLastError = connectorData.lastError();
    displayLastError();
    return;
  }
  connectorData.setValue("Unit_Id_FK", unitId);
  connectorData.setValue("Connector_Id_FK", baseConnectorId);
  connectorData.setValue("Name", connectorName);
  // Add contacts to unit connector data
  if(!unit.addConnectionDataListFromConnectorContactIdList(connectorData, contactList)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Add connector
  if(!unit.addConnector(connectorData)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("UnitConnector_view");
  select("UnitConnection_view");
  ///select("UnitLink_view");
}

void mdtTtTestCableEditor::editConnectorName()
{
  QVariant unitConnectorId;
  QVariant connectorName;
  QString str;
  QInputDialog dialog;
  mdtClUnit unit(this, database());

  // Get current data
  unitConnectorId = currentData("UnitConnector_view", "Id_PK");
  if(unitConnectorId.isNull()){
    return;
  }
  connectorName = currentData("UnitConnector_view", "UnitConnectorName");
  // Let user set connector name
  dialog.setLabelText(tr("Connector name:"));
  dialog.setTextValue(connectorName.toString());
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  str = dialog.textValue().trimmed();
  if(!str.isEmpty()){
    connectorName = str;
  }
  // Edit connector name
  if(!unit.editConnectorName(unitConnectorId, connectorName)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("UnitConnector_view");
  select("UnitConnection_view");
  ///select("UnitLink_view");
}

void mdtTtTestCableEditor::removeConnectors()
{
  mdtSqlTableWidget *widget;
  mdtClUnit unit(this, database());
  QMessageBox msgBox;
  mdtSqlTableSelection s;

  widget = sqlTableWidget("UnitConnector_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove connectors from current unit. This will also remove related connections."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove connectors
  if(!unit.removeConnectors(s)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("UnitConnector_view");
  select("UnitConnection_view");
  ///select("UnitLink_view");
}

void mdtTtTestCableEditor::addConnection()
{
  mdtClUnitConnectionDialog dialog(0, database(), mdtClUnitConnectionDialog::Add);
  mdtClUnit unit(this, database());
  mdtClUnitConnectionData data;
  QVariant unitId;

  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  // Setup unit connection data
  if(!data.setup(database(), true)){
    pvLastError = data.lastError();
    displayLastError();
    return;
  }
  data.setValue("Unit_Id_FK", unitId);
  // Setup and show dialog
  dialog.setData(data, currentData("Unit_tbl", "Article_Id_FK"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add connection
  if(!unit.addConnection(dialog.data())){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  select("UnitConnection_view");
  ///select("UnitLink_view");
}

void mdtTtTestCableEditor::editConnection()
{
  mdtSqlTableWidget *widget;
  mdtClUnitConnectionDialog dialog(0, database(), mdtClUnitConnectionDialog::Edit);
  mdtClUnit unit(this, database());
  QVariant connectionId;
  mdtClUnitConnectionData data;
  bool ok;

  widget = sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != 0);

  // Get current unit connection data
  connectionId = widget->currentData("UnitConnection_Id_PK");
  if(connectionId.isNull()){
    return;
  }
  data = unit.getConnectionData(connectionId, true, &ok);
  if(!ok){
    pvLastError = unit.lastError();
    displayLastError();
  }
  // Setup and show dialog
  dialog.setData(data, currentData("Unit_tbl", "Article_Id_FK"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Edit connection
  if(!unit.editConnection(connectionId, dialog.data())){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  select("UnitConnection_view");
}

void mdtTtTestCableEditor::removeConnections()
{
  mdtSqlTableWidget *widget;
  mdtClUnit unit(this, database());
  QMessageBox msgBox;
  mdtSqlTableSelection s;
  QString linksMsg;
  bool ok;

  widget = sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("UnitConnection_Id_PK");
  if(s.rowCount() < 1){
    return;
  }
  // Check that selected connections are not related to some links
  linksMsg = unit.toUnitRelatedLinksListStr(currentUnitId(), s.dataList("UnitConnection_Id_PK"), &ok);
  if(!ok){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  linksMsg = linksMsg.trimmed();
  if(!linksMsg.isEmpty()){
    msgBox.setText(tr("Unable to remove selected connections."));
    msgBox.setInformativeText(tr("Some of the selected connections are related to links."));
    msgBox.setDetailedText(tr("List of related links:\n") + linksMsg);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove connections from current unit."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!unit.removeConnections(s)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  select("UnitConnection_view");
}

void mdtTtTestCableEditor::addLink()
{
  mdtClUnitLinkDialog dialog(0, database());
  QVariant unitId;
  mdtClLink lnk(0, database());
  QList<QVariant> idList;
  QList<QVariant> startConnectorIdList;
  QList<QVariant> endConnectorIdList;
  bool ok;
  int i;

  // Check that at least 1 test system was assigned
  if(rowCount("Unit_TestNode_view") < 1){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Cannot add a link to current test cable, because no test system was assigned."));
    msgBox.setInformativeText(tr("Assign a test system and try again."));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowTitle(tr("Add link error"));
    msgBox.exec();
    return;
  }
  // If a link allready exist, we can limit start and end connectors
  idList = dataList("UnitLink_view", "StartUnitConnector_Id_FK", ok, true);
  if(!ok){
    displayLastError();
    return;
  }
  for(i = 0; i < idList.size(); ++i){
    if(!startConnectorIdList.contains(idList.at(i))){
      startConnectorIdList.append(idList.at(i));
    }
  }
  dialog.setStartConnectorLimitIdList(startConnectorIdList);
  idList = dataList("UnitLink_view", "EndUnitConnector_Id_FK", ok, true);
  if(!ok){
    displayLastError();
    return;
  }
  for(i = 0; i < idList.size(); ++i){
    if(!endConnectorIdList.contains(idList.at(i))){
      endConnectorIdList.append(idList.at(i));
    }
  }
  dialog.setEndConnectorLimitIdList(endConnectorIdList);
  // Setup and show dialog
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  dialog.setStartUnit(unitId);
  dialog.setStartConnectionLabel(tr("DUT side connection"));
  dialog.setEndUnit(unitId);
  dialog.setEndConnectionLabel(tr("Test system side connection"));
  dialog.setShowOnlyUnusedStartConnection(true);
  dialog.setShowOnlyUnusedEndConnection(true);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add link
  if(!lnk.addLink(dialog.linkData())){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Update links view
  select("UnitLink_view");
}

void mdtTtTestCableEditor::editLink()
{
  mdtSqlTableWidget *linkWidget;
  mdtClUnitLinkDialog dialog(0, database());
  QVariant unitId, startConnectionId, endConnectionId;
  mdtClLinkData linkData;
  mdtClLink lnk(0, database());
  bool ok;

  linkWidget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(linkWidget != 0);
  // Get unit ID
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  // Get connection IDs
  startConnectionId = linkWidget->currentData("UnitConnectionStart_Id_FK");
  endConnectionId = linkWidget->currentData("UnitConnectionEnd_Id_FK");
  if(startConnectionId.isNull() || endConnectionId.isNull()){
    return;
  }
  // Get current link data
  linkData = lnk.getLinkData(startConnectionId, endConnectionId,true, true, ok);
  if(!ok){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Setup and show dialog
  dialog.setLinkData(linkData);
  dialog.setShowOnlyUnusedStartConnection(true);
  dialog.setShowOnlyUnusedEndConnection(true);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Edit link
  if(!lnk.editLink(startConnectionId, endConnectionId, dialog.linkData())){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Update links view
  select("UnitLink_view");
}

void mdtTtTestCableEditor::removeLinks()
{
  mdtSqlTableWidget *widget;
  mdtClLink lnk(0, database());
  QMessageBox msgBox(this);
  mdtSqlTableSelection s;
  QStringList fields;

  widget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  fields << "UnitConnectionStart_Id_FK" << "UnitConnectionEnd_Id_FK";
  s = widget->currentSelection(fields);
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove links attached from current test cable."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!lnk.removeLinks(s)){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  select("UnitLink_view");
}

void mdtTtTestCableEditor::addLogicalTestCable()
{
  mdtTtLogicalTestCable ltc(0, database());
  mdtTtLogicalTestCableDialog dialog(this, database());
  QVariant testCableId;
  QList<QSqlRecord> testCableLinkDataList;
  mdtSqlRecord logicalTestCableData;
  mdtTtTestLinkData testLinkData;
  QList<mdtTtTestLinkData> testLinkDataList;
  QVariant id;
  QString sql;
  int i;
  bool ok;

  // Get test cable ID
  testCableId = currentUnitId();
  if(testCableId.isNull()){
    return;
  }
  // Setup and show dialog
  dialog.setTestCable(testCableId);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Setup data
  if(!logicalTestCableData.addAllFields("LogicalTestCable_tbl", database())){
    pvLastError = logicalTestCableData.lastError();
    displayLastError();
    return;
  }
  if(!testLinkData.setup(database())){
    pvLastError = testLinkData.lastError();
    displayLastError();
    return;
  }
  // Build logical test cable data
  logicalTestCableData.setValue("TestCable_Id_FK", testCableId);
  logicalTestCableData.setValue("Key", dialog.key());
  // Get links from (physical) test cable
  sql = "SELECT * FROM UnitLink_view";
  sql += " WHERE StartUnit_Id_FK = " + testCableId.toString();
  sql += " AND EndUnit_Id_FK = " + testCableId.toString();
  testCableLinkDataList = ltc.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    pvLastError = ltc.lastError();
    displayLastError();
    return;
  }
  // Build test link data list based on each test cable link data
  for(i = 0; i < testCableLinkDataList.size(); ++i){
    testLinkData.clearValues();
    // Set test cable connections
    testLinkData.setValue("TestCableUnitConnectionStart_Id_FK", testCableLinkDataList.at(i).value("UnitConnectionStart_Id_FK"));
    testLinkData.setValue("TestCableUnitConnectionEnd_Id_FK", testCableLinkDataList.at(i).value("UnitConnectionEnd_Id_FK"));
    // Set DUT connection
    id = dialog.getDutConnectionId(testLinkData.value("TestCableUnitConnectionStart_Id_FK"), ok);
    if(!ok){
      /// \todo Error message
      return;
    }
    testLinkData.setValue("DutConnection_Id_FK", id);
    // Set test system connection
    id = dialog.getTsConnectionId(testLinkData.value("TestCableUnitConnectionEnd_Id_FK"), ok);
    if(!ok){
      /// \todo Error message
      return;
    }
    testLinkData.setValue("TestConnection_Id_FK", id);
    // Add data to list
    testLinkDataList.append(testLinkData);
  }
  // Create cable
  if(!ltc.createCable(logicalTestCableData, testLinkDataList)){
    pvLastError = ltc.lastError();
    displayLastError();
    return;
  }
  // Update logical test cable table
  select("LogicalTestCable_tbl");

//   mdtTtLogicalTestCableEditor *ltce;
//   mdtSqlDialog dialog;
//   QVariant unitId;
// 
//   unitId = currentUnitId();
//   if(unitId.isNull()){
//     return;
//   }
//   // Create editor and dialog
//   ltce = new mdtTtLogicalTestCableEditor(0, database());
//   if(!ltce->setupTables()){
//     pvLastError = ltce->lastError();
//     displayLastError();
//     delete ltce;
//     return;
//   }
//   dialog.setSqlForm(ltce);
//   dialog.resize(900, 700);
//   dialog.enableEdition();
//   /**
//   if(!ltce->setMainTableFilter("Id_PK=-1")){
//     pvLastError = ltce->lastError();
//     displayLastError();
//     return;
//   }
//   */
//   if(!ltce->select()){
//     pvLastError = ltce->lastError();
//     displayLastError();
//     return;
//   }
//   ltce->insert();
//   if(!ltce->setCurrentData("LogicalTestCable_tbl", "TestCable_Id_FK", unitId)){
//     pvLastError = ltce->lastError();
//     displayLastError();
//     return;
//   }
//   dialog.exec();
  // Update logical test cable table
///  select("LogicalTestCable_tbl");
}

void mdtTtTestCableEditor::editLogicalTestCable()
{
  mdtTtLogicalTestCableEditor *ltce;
  mdtSqlDialog dialog;
  QVariant logicalCableId;

  // Get current logical cable ID
  logicalCableId = currentData("LogicalTestCable_tbl", "Id_PK");
  if(logicalCableId.isNull()){
    return;
  }
  // Create editor and dialog
  ltce = new mdtTtLogicalTestCableEditor(0, database());
  if(!ltce->setupTables()){
    pvLastError = ltce->lastError();
    displayLastError();
    delete ltce;
    return;
  }
  dialog.setSqlForm(ltce);
  dialog.resize(900, 700);
  dialog.enableEdition();
  if(!ltce->setMainTableFilter("Id_PK", logicalCableId)){
    pvLastError = ltce->lastError();
    displayLastError();
    return;
  }
  if(!ltce->select()){
    pvLastError = ltce->lastError();
    displayLastError();
    return;
  }
  dialog.exec();
  // Update logical test cable table
  select("LogicalTestCable_tbl");
}

void mdtTtTestCableEditor::removeLogicalTestCables()
{
  mdtTtLogicalTestCable ltc(0, database());
  mdtSqlTableWidget *widget;
  QMessageBox msgBox(this);
  mdtSqlTableSelection s;

  widget = sqlTableWidget("LogicalTestCable_tbl");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to delete selected logical test cables."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!ltc.removeData("LogicalTestCable_tbl", s, true)){
    pvLastError = ltc.lastError();
    displayLastError();
    return;
  }
  // Update logical test cable table
  select("LogicalTestCable_tbl");
}

void mdtTtTestCableEditor::selectOffsetResetTestModel()
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  QVariant offsetResetTestModelId;

  // Build SQL query to select test models
  sql = "SELECT * FROM TestModel_tbl";
  // Setup and show selection dialog
  selectionDialog.setMessage("Please select a test:");
  selectionDialog.setQuery(sql, database(), false);
  ///selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.resize(700, 400);
  selectionDialog.setWindowTitle(tr("Offset reset test selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(selectionDialog.selection("Id_PK").rowCount() == 1);
  offsetResetTestModelId = selectionDialog.selection("Id_PK").data(0, "Id_PK");
  // Set selected test model
  if(!setCurrentData("TestCable_tbl", "OffsetResetTestModel_Id_FK", offsetResetTestModelId, true)){
    return;
  }
  // Update
  auto unitController = mainTableController<mdtSqlDataWidgetController>();
  Q_ASSERT(unitController);
  unitController->update();
}

/**
 * \todo Mettre en place test tool ici ?
 *  Actuellement, est fait dans setupUnitLinkTable()
 *  -> a méditer ...
 */
void mdtTtTestCableEditor::runOffsetReset()
{
  Q_ASSERT(pvCableOffsetTool != 0);

  QVariant offsetResetTestModelId;

  // Get current test model offset ID
  offsetResetTestModelId = currentData("TestCable_tbl", "OffsetResetTestModel_Id_FK");
  if(offsetResetTestModelId.isNull()){
    pvLastError.setError(tr("No test was affected for offset reset of this cable."), mdtError::Warning);
    displayLastError();
    return;
  }
  
  pvCableOffsetTool->runOffsetReset(offsetResetTestModelId);
  // Update
  select("UnitLink_view");
}

QVariant mdtTtTestCableEditor::selectBaseConnector()
{
  mdtSqlSelectionDialog selectionDialog;
  QString sql;

  if(currentUnitId().isNull()){
    return QVariant();
  }
  // Setup model to show available connectors
  sql = "SELECT * FROM Connector_tbl";
  // Setup and show dialog
  selectionDialog.setMessage("Please select a connector:");
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setHeaderData("ContactQty", tr("Contact\nQty"));
  selectionDialog.setHeaderData("InsertRotation", tr("Insert\nRotation"));
  selectionDialog.setHeaderData("ManufacturerConfigCode", tr("Manufacturer\nConfiguration code"));
  selectionDialog.setHeaderData("ManufacturerArticleCode", tr("Manufacturer\nArticle code"));
  selectionDialog.addColumnToSortOrder("Gender", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("ContactQty", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("ManufacturerConfigCode", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(700, 400);
  selectionDialog.setWindowTitle(tr("Connector selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selection("Id_PK").rowCount() == 1);

  return selectionDialog.selection("Id_PK").data(0, "Id_PK");
}

QList< QVariant > mdtTtTestCableEditor::selectBaseConnectorContactIdList(const QVariant& connectorId, bool multiSelection)
{
  mdtSqlSelectionDialog selectionDialog;
  QString sql;
  mdtSqlTableSelection s;
  QList<QVariant> idList;
  int i;

  if(connectorId.isNull()){
    return idList;
  }
  // Setup model to show available connectors
  sql = "SELECT * FROM ConnectorContact_tbl ";
  sql += " WHERE Connector_Id_FK = " + connectorId.toString();
  // Setup and show dialog
  selectionDialog.setMessage("Please select contacts to use:");
  selectionDialog.setQuery(sql, database(), multiSelection);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Connector_Id_FK", true);
  selectionDialog.addColumnToSortOrder("Name", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(400, 300);
  selectionDialog.setWindowTitle(tr("Contacts selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return idList;
  }
  s = selectionDialog.selection("Id_PK");
  for(i = 0; i < s.rowCount(); ++i){
    idList.append(s.data(i, "Id_PK"));
  }

  return idList;
}

bool mdtTtTestCableEditor::setupTestCableTable()
{
  Ui::mdtTtTestCableEditor tce;
  std::shared_ptr<mdtSqlDataWidgetController> unitController;
  std::shared_ptr<mdtSqlDataWidgetController> tcController;
  std::shared_ptr<mdtSqlDataWidgetController> testModelController;
  mdtSqlRelationInfo relationInfo;

  // Setup main form widget
  setMainTableUi<Ui::mdtTtTestCableEditor>(tce);
  // Setup form
  if(!setMainTable("Unit_tbl", "Test cable")){
    return false;
  }
  // Get unit controller and add TestCable_tbl as child of it
  unitController = tableController<mdtSqlDataWidgetController>("Unit_tbl");
  Q_ASSERT(unitController);
  relationInfo.setChildTableName("TestCable_tbl");
  relationInfo.addRelation("Id_PK", "Unit_Id_FK_PK", true);
  relationInfo.setRelationType(mdtSqlRelationInfo::OneToOne);
  if(!unitController->addChildController<mdtSqlDataWidgetController>(relationInfo, tr("Test cable"))){
    pvLastError = unitController->lastError();
    return false;
  }
  // Get test cable controller and map widgets to it
  tcController = unitController->childController<mdtSqlDataWidgetController>("TestCable_tbl");
  Q_ASSERT(tcController);
  tcController->addMapping(tce.fld_Identification, "Identification");
  tcController->addMapping(tce.fld_DescriptionEN, "DescriptionEN");
  tcController->addMapping(tce.fld_OffsetResetDate, "OffsetResetDate");
  // Add a child controller that displays test model data
  relationInfo.clear();
  relationInfo.setChildTableName("TestModel_tbl");
  relationInfo.addRelation("OffsetResetTestModel_Id_FK", "Id_PK", false);
  if(!tcController->addChildController<mdtSqlDataWidgetController>(relationInfo, tr("Offset reset test model"))){
    pvLastError = tcController->lastError();
    return false;
  }
  // Get test model controller and map widgets to it
  testModelController = tcController->childController<mdtSqlDataWidgetController>("TestModel_tbl");
  Q_ASSERT(testModelController);
  testModelController->addMapping(tce.lbOffsetResetTestModelDesignationEN, "DesignationEN");
  testModelController->addMapping(tce.lbOffsetResetTestModelKey, "Key");
  testModelController->setCanWriteToDatabase(false);

  return true;
}

bool mdtTtTestCableEditor::setupTestNodeTable()
{
  mdtSqlTableWidget *widget;
  mdtSqlRelationInfo relationInfo;
  QPushButton *pb;

  relationInfo.setChildTableName("Unit_TestNode_view");
  relationInfo.addRelation("Id_PK", "Unit_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Test system"))){
    return false;
  }
  widget = sqlTableWidget("Unit_TestNode_view");
  Q_ASSERT(widget != 0);
  // Hide relation fields and PK
  widget->setColumnHidden("VehicleType_Id_FK_PK", true);
  widget->setColumnHidden("Unit_Id_FK", true);
  // Give fields a user friendly name
  widget->setHeaderData("Type", tr("Test system"));
  widget->setHeaderData("SubType", tr("Node or group"));
  // Setup test node assignation button
  pb = new QPushButton(tr("Assign"));
  pb->setIcon(QIcon::fromTheme("list-add"));
  connect(pb, SIGNAL(clicked()), this, SLOT(assignTestNode()));
  widget->addWidgetToLocalBar(pb);
  // Setup test node assignation removal button
  pb = new QPushButton(tr("Remove assignation"));
  pb->setIcon(QIcon::fromTheme("list-remove"));
  connect(pb, SIGNAL(clicked()), this, SLOT(removeTestNodeAssignation()));
  widget->addWidgetToLocalBar(pb);
  // Setup sorting and other stuff
  widget->addStretchToLocalBar();
  widget->resizeViewToContents();

  return true;
}

bool mdtTtTestCableEditor::setupUnitConnectorTable()
{
  mdtSqlTableWidget *widget;
  ///QPushButton *pbAddConnector;
  QPushButton *pbAddConnectorBasedConnector;
  QPushButton *pbEditConnectorName;
  QPushButton *pbRemoveConnectors;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("UnitConnector_view");
  relationInfo.addRelation("Id_PK", "Unit_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Connectors"))){
    return false;
  }
  widget = sqlTableWidget("UnitConnector_view");
  Q_ASSERT(widget != 0);
  // Hide relation fields and PK
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("Unit_Id_FK", true);
  widget->setColumnHidden("Connector_Id_FK", true);
  widget->setColumnHidden("ArticleConnector_Id_FK", true);
  // Give fields a user friendly name
  widget->setHeaderData("UnitConnectorName", tr("Unit\nconnector"));
  widget->setHeaderData("ArticleConnectorName", tr("Article\nconnector"));
  // Add connector button
  pbAddConnectorBasedConnector = new QPushButton(tr("Add connector ..."));
  pbAddConnectorBasedConnector->setIcon(QIcon::fromTheme("document-new"));
  connect(pbAddConnectorBasedConnector, SIGNAL(clicked()), this, SLOT(addConnectorBasedConnector()));
  widget->addWidgetToLocalBar(pbAddConnectorBasedConnector);
  // Edit connector button
  pbEditConnectorName = new QPushButton(tr("Edit name"));
  connect(pbEditConnectorName, SIGNAL(clicked()), this, SLOT(editConnectorName()));
  widget->addWidgetToLocalBar(pbEditConnectorName);
  // Remove connectors button
  pbRemoveConnectors = new QPushButton(tr("Rem. connector"));
  pbRemoveConnectors->setIcon(QIcon::fromTheme("edit-delete"));
  connect(pbRemoveConnectors, SIGNAL(clicked()), this, SLOT(removeConnectors()));
  widget->addWidgetToLocalBar(pbRemoveConnectors);
  ///pbAddConnector = new QPushButton(tr("Add free connector ..."));
  ///pbAddConnector->setIcon(QIcon::fromTheme("document-new"));
  ///pbAddArticleConnectorBasedConnector = new QPushButton(tr("Add art. connector ..."));
  ///pbAddArticleConnectorBasedConnector->setIcon(QIcon::fromTheme("document-new"));
  ///connect(pbAddConnector, SIGNAL(clicked()), this, SLOT(addConnector()));
  ///connect(pbAddArticleConnectorBasedConnector, SIGNAL(clicked()), this, SLOT(addArticleConnectorBasedConnector()));
  ///widget->addWidgetToLocalBar(pbAddConnector);
  ///widget->addWidgetToLocalBar(pbAddArticleConnectorBasedConnector);
  // Stretch and other attributes
  widget->addStretchToLocalBar();
  widget->addColumnToSortOrder("UnitConnectorName", Qt::AscendingOrder);
  widget->sort();
  widget->resizeViewToContents();

  return true;
}

bool mdtTtTestCableEditor::setupUnitConnectionTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddConnection;
  ///QPushButton *pbAddArticleConnectionBasedConnection;
  QPushButton *pbEditConnection;
  ///QPushButton *pbCopyFunctions;
  QPushButton *pbRemoveConnection;
  ///QPushButton *pbViewLinkedConnections;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("UnitConnection_view");
  relationInfo.addRelation("Id_PK", "Unit_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Connections"))){
    return false;
  }
  widget = sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != 0);
  // Add add connection button
  pbAddConnection = new QPushButton(tr("Add connection ..."));
  pbAddConnection->setIcon(QIcon::fromTheme("document-new"));
  connect(pbAddConnection, SIGNAL(clicked()), this, SLOT(addConnection()));
  widget->addWidgetToLocalBar(pbAddConnection);
  // Add add art. connection button
  /**
  pbAddArticleConnectionBasedConnection = new QPushButton(tr("Add art. connections ..."));
  pbAddArticleConnectionBasedConnection->setIcon(QIcon::fromTheme("document-new"));
  connect(pbAddArticleConnectionBasedConnection, SIGNAL(clicked()), this, SLOT(addArticleConnectionsBasedConnections()));
  widget->addWidgetToLocalBar(pbAddArticleConnectionBasedConnection);
  */
  // Add edit connection button
  pbEditConnection = new QPushButton(tr("Edit connection ..."));
  connect(pbEditConnection, SIGNAL(clicked()), this, SLOT(editConnection()));
  widget->addWidgetToLocalBar(pbEditConnection);
  // Add remove connection button
  pbRemoveConnection = new QPushButton(tr("Remove connections"));
  pbRemoveConnection->setIcon(QIcon::fromTheme("edit-delete"));
  connect(pbRemoveConnection, SIGNAL(clicked()), this, SLOT(removeConnections()));
  widget->addWidgetToLocalBar(pbRemoveConnection);
  // Add copy functions connection button
  /**
  pbCopyFunctions = new QPushButton(tr("Set functions from ..."));
  connect(pbCopyFunctions, SIGNAL(clicked()), this, SLOT(setFunctionsFromOtherConnection()));
  widget->addWidgetToLocalBar(pbCopyFunctions);
  */
  // View linked connections button
  /**
  pbViewLinkedConnections = new QPushButton(tr("Linked connections"));
  connect(pbViewLinkedConnections, SIGNAL(clicked()), this, SLOT(viewLinkedConnections()));
  widget->addWidgetToLocalBar(pbViewLinkedConnections);
  */
  // Add stretch
  widget->addStretchToLocalBar();
  // On double click, we edit connection
  Q_ASSERT(widget->tableView() != 0);
  connect(widget->tableView(), SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(editConnection()));
  // Hide relation fields and PK
  widget->setColumnHidden("UnitConnection_Id_PK", true);
  widget->setColumnHidden("Unit_Id_FK", true);
  widget->setColumnHidden("ArticleConnection_Id_FK", true);
  widget->setColumnHidden("UnitConnector_Id_FK", true);
  // Give fields a user friendly name
  widget->setHeaderData("ConnectionType_Code_FK", tr("Contact\ntype"));
  widget->setHeaderData("SchemaPage", tr("Vehicle\nSchema\npage"));
  widget->setHeaderData("UnitFunctionEN", tr("Unit\nfunction\n(English)"));
  widget->setHeaderData("UnitFunctionFR", tr("Unit\nfunction\n(French)"));
  widget->setHeaderData("UnitFunctionDE", tr("Unit\nfunction\n(German)"));
  widget->setHeaderData("UnitFunctionIT", tr("Unit\nfunction\n(Italian)"));
  widget->setHeaderData("SignalName", tr("Signal name"));
  widget->setHeaderData("SwAddress", tr("SW address"));
  widget->setHeaderData("UnitConnectorName", tr("Unit\nconnector"));
  widget->setHeaderData("UnitContactName", tr("Unit\ncontact"));
  widget->setHeaderData("ArticleConnectorName", tr("Article\nconnector"));
  widget->setHeaderData("ArticleContactName", tr("Article\ncontact"));
  widget->setHeaderData("IoType", tr("I/O type"));
  widget->setHeaderData("ArticleFunctionEN", tr("Article\nfunction\n(English)"));
  widget->setHeaderData("ArticleFunctionFR", tr("Article\nfunction\n(French)"));
  widget->setHeaderData("ArticleFunctionDE", tr("Article\nfunction\n(German)"));
  widget->setHeaderData("ArticleFunctionIT", tr("Article\nfunction\n(Italian)"));
  // Enable sorting
  widget->addColumnToSortOrder("UnitConnectorName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("UnitContactName", Qt::AscendingOrder);
  widget->sort();
  // Set some attributes on table view
  widget->resizeViewToContents();

  return true;
}

bool mdtTtTestCableEditor::setupUnitLinkTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddLink;
  QPushButton *pbEditLink;
  QPushButton *pbRemoveLinks;
  mdtSqlRelationInfo relationInfo;
  std::shared_ptr<mdtSqlTableViewController> tvc;

  relationInfo.setChildTableName("UnitLink_view");
  relationInfo.addRelation("Id_PK", "StartUnit_Id_FK", false);
  relationInfo.addRelation("Id_PK", "EndUnit_Id_FK", false, "OR");
  if(!addChildTable(relationInfo, tr("Links"))){
    return false;
  }
  // Get added table controller and set it to offset tool
  tvc = tableController<mdtSqlTableViewController>("UnitLink_view");
  Q_ASSERT(tvc);
  pvCableOffsetTool->setTestLinkTableController(tvc);
  // Get added widget to do some setup
  widget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(widget != 0);
  // Setup add link button
  pbAddLink = new QPushButton(tr("Add link ..."));
  pbAddLink->setIcon(QIcon::fromTheme("document-new"));
  connect(pbAddLink, SIGNAL(clicked()), this, SLOT(addLink()));
  widget->addWidgetToLocalBar(pbAddLink);
  // Setup edit link button
  pbEditLink = new QPushButton(tr("Edit link"));
  connect(pbEditLink, SIGNAL(clicked()), this, SLOT(editLink()));
  widget->addWidgetToLocalBar(pbEditLink);
  // On double click, we edit link
  Q_ASSERT(widget->tableView() != 0);
  connect(widget->tableView(), SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(editLink()));
  // Setup remove link button
  pbRemoveLinks = new QPushButton(tr("Remove links"));
  pbRemoveLinks->setIcon(QIcon::fromTheme("edit-delete"));
  connect(pbRemoveLinks, SIGNAL(clicked()), this, SLOT(removeLinks()));
  widget->addWidgetToLocalBar(pbRemoveLinks);
  // Strech
  widget->addStretchToLocalBar();
  // Hide relation fields and PK
  widget->setColumnHidden("UnitConnectionStart_Id_FK", true);
  widget->setColumnHidden("UnitConnectionEnd_Id_FK", true);
  widget->setColumnHidden("ArticleLink_Id_FK", true);
  widget->setColumnHidden("StartUnit_Id_FK", true);
  widget->setColumnHidden("EndUnit_Id_FK", true);
  widget->setColumnHidden("LinkType_Code_FK", true);
  widget->setColumnHidden("LinkDirection_Code_FK", true);
  widget->setColumnHidden("ArticleConnectionStart_Id_FK", true);
  widget->setColumnHidden("ArticleConnectionEnd_Id_FK", true);
  widget->setColumnHidden("LinkBeam_Id_FK", true);
  widget->setColumnHidden("StartSchemaPosition", true);
  widget->setColumnHidden("EndSchemaPosition", true);
  widget->setColumnHidden("StartAlias", true);
  widget->setColumnHidden("EndAlias", true);
  // Give fields a user friendly name
  ///widget->setHeaderData("StartSchemaPosition", tr("Start\nschema pos."));
  ///widget->setHeaderData("StartAlias", tr("Start\nalias"));
  widget->setHeaderData("StartUnitConnectorName", tr("DUT\nside\nconnector"));
  widget->setHeaderData("StartUnitContactName", tr("DUT\nside\ncontact"));
  ///widget->setHeaderData("EndSchemaPosition", tr("End\nschema pos."));
  ///widget->setHeaderData("EndAlias", tr("End\nalias"));
  widget->setHeaderData("EndUnitConnectorName", tr("Test\nsys. side\nconnector"));
  widget->setHeaderData("EndUnitContactName", tr("Test\nsys. side\ncontact"));
  widget->setHeaderData("SinceVersion", tr("Since\nversion"));
  widget->setHeaderData("LinkTypeNameEN", tr("Link type"));
  widget->setHeaderData("ValueUnit", tr("Unit"));
  widget->setHeaderData("LinkDirectionPictureAscii", tr("Direction"));
  widget->setHeaderData("StartSchemaPage", tr("Start\nschema\npage"));
  widget->setHeaderData("EndSchemaPage", tr("End\nschema\npage"));
  widget->setHeaderData("StartFunctionEN", tr("Start\nfunction (ENG)"));
  widget->setHeaderData("EndFunctionEN", tr("End\nfunction (ENG)"));
  widget->setHeaderData("StartSignalName", tr("Start\nsignal"));
  widget->setHeaderData("EndSignalName", tr("End\nsignal"));
  widget->setHeaderData("StartSwAddress", tr("Start\nSW address"));
  widget->setHeaderData("EndSwAddress", tr("End\nSW address"));
  // Setup sorting
  widget->addColumnToSortOrder("StartSchemaPosition", Qt::AscendingOrder);
  widget->addColumnToSortOrder("StartUnitConnectorName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("StartUnitContactName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("EndSchemaPosition", Qt::AscendingOrder);
  widget->addColumnToSortOrder("EndUnitConnectorName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("EndUnitContactName", Qt::AscendingOrder);
  widget->sort();
  // Set some attributes on table view
  widget->resizeViewToContents();

  return true;
}

bool mdtTtTestCableEditor::setupLogicalTestCableTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pb;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("LogicalTestCable_tbl");
  relationInfo.addRelation("Id_PK", "TestCable_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Logical test cables"))){
    return false;
  }
  widget = sqlTableWidget("LogicalTestCable_tbl");
  Q_ASSERT(widget != 0);
  // Setup add logical test cable button
  pb = new QPushButton(tr("Create"));
  pb->setIcon(QIcon::fromTheme("document-new"));
  connect(pb, SIGNAL(clicked()), this, SLOT(addLogicalTestCable()));
  widget->addWidgetToLocalBar(pb);
  // Setup edit logical test cable button
  pb = new QPushButton(tr("edit"));
  connect(pb, SIGNAL(clicked()), this, SLOT(editLogicalTestCable()));
  widget->addWidgetToLocalBar(pb);
  // Setup remove logical test cable button
  pb = new QPushButton(tr("Delete"));
  pb->setIcon(QIcon::fromTheme("edit-delete"));
  connect(pb, SIGNAL(clicked()), this, SLOT(removeLogicalTestCables()));
  widget->addWidgetToLocalBar(pb);
  // Strech
  widget->addStretchToLocalBar();
  // Hide relation fields and PK
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("TestCable_Id_FK", true);
  // Give fields a user friendly name
  widget->setHeaderData("Key", tr("Key"));
  // Setup sorting and other stuff
  widget->addColumnToSortOrder("Key", Qt::AscendingOrder);
  widget->sort();
  widget->resizeViewToContents();

  return true;
}
