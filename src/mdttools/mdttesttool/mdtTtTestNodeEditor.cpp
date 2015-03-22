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
#include "mdtTtTestNodeEditor.h"
#include "mdtTtTestNode.h"
#include "ui_mdtTtTestNodeEditor.h"
#include "mdtSqlDataWidgetController.h"
#include "mdtSqlRelationInfo.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtTtApplicationWidgets.h"
#include "mdtTtTestNodeUnit.h"
#include "mdtTtTestNodeUnitData.h"
#include "mdtTtTestNodeUnitDialog.h"
#include "mdtClPathGraph.h"
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QDataWidgetMapper>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QPushButton>
#include <QSqlError>
#include <QModelIndex>
#include <QInputDialog>
#include <QMessageBox>
#include <memory>

#include <QDebug>

mdtTtTestNodeEditor::mdtTtTestNodeEditor(QWidget *parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

bool mdtTtTestNodeEditor::setupTables()
{
  if(!setupTestNodeTable()){
    return false;
  }
  if(!setupTestNodeUnitTable()){
    return false;
  }
  if(!setupTestNodeBusTable()){
    return false;
  }
  if(!setupTestNodeUnitConnectionTable()){
    return false;
  }
  return true;
}

void mdtTtTestNodeEditor::showTestNodeModbusIoTool()
{
  QString deviceAlias;
  bool ok;

  // Get alias of current test node
  deviceAlias = currentData("TestNode_tbl", "Alias", ok).toString();
  if(!ok){
    displayLastError();
    return;
  }
  if(deviceAlias.isEmpty()){
    pvLastError.setError(tr("No alias was defined for current test node."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeEditor");
    pvLastError.commit();
    displayLastError();
    return;
  }
  // Display tool
  mdtTtApplicationWidgets::showTestNodeModbusIoTool(deviceAlias);
}

void mdtTtTestNodeEditor::setBaseVehicleType()
{
  mdtSqlSelectionDialog selectionDialog;
  QString sql;
  QVariant vehicleTypeId;

  // Setup vehicle type selection dialog and show it to user
  sql = "SELECT * FROM VehicleType_tbl;";
  selectionDialog.setMessage("Please select vehicle type.");
  selectionDialog.setQuery(sql, database(), false);
  ///selectionDialog.setColumnHidden("Id_PK", true);
  ///selectionDialog.setHeaderData("SubType", tr("Variant"));
  ///selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);
  vehicleTypeId = selectionDialog.selectionResult().at(0);
  qDebug() << "VHC ID: " << vehicleTypeId;
  if(!setCurrentData("TestNode_tbl", "VehicleType_Id_FK_PK", vehicleTypeId)){
    displayLastError();
    return;
  }
}

void mdtTtTestNodeEditor::addUnit()
{
  mdtTtTestNodeUnit tnu(0, database());
  mdtTtTestNodeUnitDialog dialog(this, database(), mdtTtTestNodeUnitDialog::Add);
  QVariant nodeId;
  mdtTtTestNodeUnitData data;

  // Get ids
  nodeId = currentData("TestNode_tbl", "VehicleType_Id_FK_PK");
  if(nodeId.isNull()){
    return;
  }
  // Setup data
  if(!data.setup(database())){
    pvLastError = data.lastError();
    displayLastError();
    return;
  }
  data.setValue("TestNode_Id_FK", nodeId);
  dialog.setData(data);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add unit
  if(!tnu.addNodeUnit(dialog.data())){
    pvLastError = tnu.lastError();
    displayLastError();
    return;
  }
  // Update UI
  select("TestNodeUnit_view");
  select("TestNodeUnitConnection_view");
}

/**
void mdtTtTestNodeEditor::addUnits()
{
  mdtTtTestNode tn(this, database());
  QList<mdtTtTestNodeUnitData> dataList;
  int i;
  
  QVariant baseVehicleTypeId;
  QVariant typeCode;
  QVariant busName;
  QVariant firstUnitId;
  QVariant firstUnitConnectionId;
  QList<QVariant> unitConnectionIdList;
  QList<QVariant> unitIdList;

  // Get base vehicle type ID
  baseVehicleTypeId = form()->currentData("TestNode_tbl", "VehicleType_Id_FK_PK");
  if(baseVehicleTypeId.isNull()){
    return;
  }
  qDebug() << "VHTID: " << baseVehicleTypeId;
  // Get test node unit type
  typeCode = selectTestNodeUnitType();
  if(typeCode.isNull()){
    return;
  }
  qDebug() << "Type: " << typeCode;
  // Select a unit that is part of those we want to use as test node unit
  firstUnitId = selectUnitToUseAsTestNode(baseVehicleTypeId);
  if(firstUnitId.isNull()){
    return;
  }
  qDebug() << "Firs unit ID: " << firstUnitId;
  // Get bus name
  busName = getBusName();
  if(busName.isNull()){
    return;
  }
  qDebug() << "Bus name: " << busName;
  // Select connection ID that is linked to a BUS
  firstUnitConnectionId = selectUnitConnection(firstUnitId);
  if(firstUnitConnectionId.isNull()){
    return;
  }
  qDebug() << "CNN ID: " << firstUnitConnectionId;
  // Get related units
  unitConnectionIdList = tn.getIdListOfUnitConnectionsLinkedToUnitConnectionId(firstUnitConnectionId);
  if(unitConnectionIdList.isEmpty()){
    pvLastError = tn.lastError();
    displayLastError();
    return;
  }
  unitIdList = tn.getIdListOfUnitIdForUnitConnectionIdList(unitConnectionIdList);
  if(unitIdList.isEmpty()){
    pvLastError = tn.lastError();
    displayLastError();
    return;
  }
  qDebug() << "Found units: " << unitIdList;
  // Select the units to really use
  unitIdList = selectUnitIdList(unitIdList);
  if(unitIdList.isEmpty()){
    return;
  }
  qDebug() << "Selected units: " << unitIdList;
  // Build data list
  for(i = 0; i < unitIdList.size(); ++i){
    mdtTtTestNodeUnitData data(database());
    if(data.isEmpty()){
      pvLastError = data.lastError();
      displayLastError();
      return;
    }
    data.setValue("Unit_Id_FK_PK", unitIdList.at(i));
    data.setValue("TestNode_Id_FK", baseVehicleTypeId);
    data.setValue("Type_Code_FK", typeCode);
    data.setValue("Bus", busName);
    dataList.append(data);
  }
  // Add test node unit
  if(!tn.addTestNodeUnits(dataList)){
    pvLastError = tn.lastError();
    displayLastError();
    return;
  }
  
  if(!tn.addTestNodeUnits(unitIdList, baseVehicleTypeId, typeCode, busName)){
    pvLastError = tn.lastError();
    displayLastError();
    return;
  }
  
  // Assign test connection to each test node unit
  if(!assignTestConnectionToTestNodeUnitLits(unitIdList, unitConnectionIdList)){
    displayLastError();
  }
  // Update TestNodeUnit_view
  form()->select("TestNodeUnit_view");
}
*/

void mdtTtTestNodeEditor::editUnit()
{
  mdtTtTestNodeUnit tnu(0, database());
  mdtTtTestNodeUnitDialog dialog(this, database(), mdtTtTestNodeUnitDialog::Edit);
  QVariant nodeId;
  QVariant unitId;
  mdtTtTestNodeUnitData data;
  bool ok;

  // Get ids
  nodeId = currentData("TestNode_tbl", "VehicleType_Id_FK_PK");
  if(nodeId.isNull()){
    return;
  }
  unitId = currentData("TestNodeUnit_view", "Unit_Id_FK_PK");
  if(unitId.isNull()){
    return;
  }
  // Get current data
  data = tnu.getData(unitId, &ok, true);
  if(!ok){
    pvLastError = tnu.lastError();
    displayLastError();
    return;
  }
  // Setup and show dialog
  dialog.setData(data);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Update in DB
  if(!tnu.editNodeUnit(unitId, dialog.data())){
    pvLastError = tnu.lastError();
    displayLastError();
    return;
  }
  // Update UI
  select("TestNodeUnit_view");
  select("TestNodeUnitConnection_view");
}

void mdtTtTestNodeEditor::removeUnits()
{
  mdtSqlTableWidget *widget;
  mdtTtTestNodeUnit tnu(this, database());
  QMessageBox msgBox;
  mdtSqlTableSelection s;

  widget = sqlTableWidget("TestNodeUnit_view");
  Q_ASSERT(widget != 0);
  // Get selection
  s = widget->currentSelection("Unit_Id_FK_PK");
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove unit assignations."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!tnu.removeNodeUnits(s)){
    pvLastError = tnu.lastError();
    displayLastError();
    return;
  }
  // Update UI
  select("TestNodeUnit_view");
  select("TestNodeUnitConnection_view");
}

void mdtTtTestNodeEditor::updateConnections()
{
  mdtTtTestNode tn(0, database());
  QVariant testNodeId;

  // Get test node ID
  testNodeId = currentData("TestNode_tbl", "VehicleType_Id_FK_PK");
  if(testNodeId.isNull()){
    return;
  }
  // Update connections
  if(!tn.addMissingConnections(testNodeId)){
    pvLastError = tn.lastError();
    displayLastError();
    return;
  }
  // Update UI
  select("TestNodeUnitConnection_view");
}

void mdtTtTestNodeEditor::setIoPositionRange()
{
  mdtTtTestNode tn(0, database());
  mdtSqlTableWidget *widget;
  mdtSqlTableSelection s;
  QStringList fields;
  int i;
  int startIoPos;

  // Get current selection
  widget = sqlTableWidget("TestNodeUnit_view");
  Q_ASSERT(widget != 0);
  fields << "Unit_Id_FK_PK" << "Type_Code_FK";
  s = widget->currentSelection(fields);
  if(s.isEmpty()){
    return;
  }
  // Check that each item in selection is a test node unit of the same type
  for(i = 1; i < s.rowCount(); ++i){
    if(s.data(i, "Type_Code_FK") != s.data(i-1, "Type_Code_FK")){
      QMessageBox msgBox(this);
      msgBox.setText(tr("Selected units are not all of the same type."));
      msgBox.setInformativeText(tr("To number a range of I/O position, all units must be of same type (f.ex. AI, AO, Channel relays, ..."));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      return;
    }
  }
  // Ask the first I/O position to the user
  QInputDialog startIoPosDialog(this);
  startIoPosDialog.setInputMode(QInputDialog::IntInput);
  startIoPosDialog.setIntRange(0, 999);
  if(startIoPosDialog.exec() != QInputDialog::Accepted){
    return;
  }
  startIoPos = startIoPosDialog.intValue();
  // Update in database
  if(!tn.setTestNodeUnitIoRange(s, startIoPos)){
    pvLastError = tn.lastError();
    displayLastError();
    return;
  }
  // Update UI
  select("TestNodeUnit_view");
}

void mdtTtTestNodeEditor::setBusToUnitConnection()
{
  mdtTtTestNodeUnit tnu(0, database());
  mdtClPathGraph graph(database());
  mdtSqlSelectionDialog selectionDialog;
  QString sql;
  QVariant testNodeId;
  QVariant connectionId;
  QList<QVariant> connectionIdList;
  QVariant busId;
  QString msg;
  bool ok;

  // Get test node ID
  testNodeId = currentData("TestNode_tbl", "VehicleType_Id_FK_PK");
  if(testNodeId.isNull()){
    return;
  }
  // Get selected connection ID
  connectionId = currentData("TestNodeUnitConnection_view", "UnitConnection_Id_FK_PK");
  if(connectionId.isNull()){
    return;
  }
  // Setup SQL statement to list busses
  sql = "SELECT Id_PK, NameEN FROM TestNodeBus_tbl WHERE TestNode_Id_FK = " + testNodeId.toString();
  // Setup and show dialog
  msg = tr("Select the bus to affect:");
  selectionDialog.setMessage(msg);
  selectionDialog.setQuery(sql, database(), false);
  ///selectionDialog.setColumnHidden("", true);
  ///selectionDialog.setHeaderData("Unit_Id_FK", tr("Variant"));
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(selectionDialog.selection("Id_PK").rowCount() == 1);
  busId = selectionDialog.selection("Id_PK").data(0, "Id_PK");
  // Get linked connections
  if(!graph.loadLinkList()){
    pvLastError = graph.lastError();
    displayLastError();
    return;
  }
  connectionIdList = graph.getLinkedConnectionIdList(connectionId, ok);
  if(!ok){
    pvLastError = graph.lastError();
    displayLastError();
    return;
  }
  connectionIdList.append(connectionId);
  qDebug() << "CNN IDs: " << connectionIdList;
  // Update connections
  if(!tnu.setBusIdToConnections(connectionIdList, busId)){
    pvLastError = tnu.lastError();
    displayLastError();
    return;
  }
  // Update UI
  ///select("TestNodeUnit_view");
  select("TestNodeUnitConnection_view");
}

void mdtTtTestNodeEditor::removeBusFromUnitConnection()
{
  mdtTtTestNodeUnit tnu(0, database());
  QMessageBox msgBox;
  QVariant connectionId;

  // Get selected connection ID
  connectionId = currentData("TestNodeUnitConnection_view", "UnitConnection_Id_FK_PK");
  if(connectionId.isNull()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove unit from bus."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Update connection
  if(!tnu.setBusIdToConnection(connectionId, QVariant())){
    pvLastError = tnu.lastError();
    displayLastError();
    return;
  }
  // Update UI
  ///select("TestNodeUnit_view");
  select("TestNodeUnitConnection_view");
}

QVariant mdtTtTestNodeEditor::selectTestNodeUnitType()
{
  mdtSqlSelectionDialog selectionDialog;
  QString sql;
  QString msg;

  // Setup model
  sql = "SELECT * FROM TestNodeUnitType_tbl";
  // Setup and show dialog
  msg = tr("Select the type for units that will be linked.");
  selectionDialog.setMessage(msg);
  selectionDialog.setQuery(sql, database(), false);
  ///selectionDialog.setColumnHidden("", true);
  ///selectionDialog.setHeaderData("Unit_Id_FK", tr("Variant"));
  selectionDialog.addSelectionResultColumn("Code_PK");
  selectionDialog.resize(700, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtTtTestNodeEditor::selectUnitToUseAsTestNode(const QVariant & vehicleTypeId)
{
  Q_ASSERT(!vehicleTypeId.isNull());

  mdtTtTestNode tn(this, database());
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;
  QString msg;

  // Setup model
  sql = tn.sqlForUnitSelection(vehicleTypeId);
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get list of units."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeEditor");
    pvLastError.commit();
    displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  msg = tr("Select a unit that is in group of those to use.");
  selectionDialog.setMessage(msg);
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
  ///selectionDialog.setColumnHidden("", true);
  ///selectionDialog.setHeaderData("Unit_Id_FK", tr("Variant"));
  selectionDialog.addSelectionResultColumn("Unit_Id");
  selectionDialog.resize(700, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtTtTestNodeEditor::selectUnitConnection(const QVariant & unitId)
{
  mdtTtTestNode tn(this, database());
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;
  QString msg;

  // Setup model
  sql = tn.sqlForUnitConnectionSelection(unitId);
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get list of unit connections."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeEditor");
    pvLastError.commit();
    displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  msg = tr("Select a connection that is linked to a bus.");
  selectionDialog.setMessage(msg);
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
  ///selectionDialog.setColumnHidden("", true);
  ///selectionDialog.setHeaderData("Unit_Id_FK", tr("Variant"));
  selectionDialog.addSelectionResultColumn("UnitConnection_Id");
  selectionDialog.resize(700, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QList<QVariant> mdtTtTestNodeEditor::selectUnitIdList(const QList<QVariant> & unitIdList)
{
  mdtTtTestNode tn(this, database());
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;
  QString msg;
  QModelIndexList selectedItems;
  QList<QVariant> selectedUnitIdList;
  int i;

  // Setup model
  sql = tn.sqlForUnitSelectionByUnitIdList(unitIdList);
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get list of units."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeEditor");
    pvLastError.commit();
    displayLastError();
    return selectedUnitIdList;
  }
  */
  // Setup and show dialog
  msg = tr("Select units to use.");
  selectionDialog.setMessage(msg);
  ///selectionDialog.setModel(&model, true);
  selectionDialog.setQuery(sql, database(), true);
  ///selectionDialog.setColumnHidden("", true);
  ///selectionDialog.setHeaderData("Unit_Id_FK", tr("Variant"));
  selectionDialog.addSelectionResultColumn("Unit_Id");
  selectionDialog.resize(700, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return selectedUnitIdList;
  }
  selectedItems = selectionDialog.selectionResults();
  for(i = 0; i < selectedItems.size(); ++i){
    selectedUnitIdList.append(selectedItems.at(i).data());
  }

  return selectedUnitIdList;
}

QVariant mdtTtTestNodeEditor::getBusName()
{
  QInputDialog dialog;

  dialog.setLabelText(tr("Bus name:"));
  if(dialog.exec() != QDialog::Accepted){
    return QVariant();
  }

  return dialog.textValue().trimmed();
}

/**
bool mdtTtTestNodeEditor::assignTestConnectionToTestNodeUnit(const QVariant & testNodeUnitId, const QList<QVariant> & busSideTestNodeUnitConnectionIdList)
{
  mdtTtTestNode tn(this, database());
  QList<QVariant> testNodeUnitConnectionIdList;
  QList<QVariant> linkedConnectionIdList;
  QVariant testNodeUnitConnectionId;
  QVariant linkedConnectionId;
  int i;

  // Get all unit connections that are part of given unit
  testNodeUnitConnectionIdList = tn.getIdListOfUnitConnectionsPartOfUnit(testNodeUnitId);
  if(testNodeUnitConnectionIdList.isEmpty()){
    pvLastError = tn.lastError();
    return false;
  }
  qDebug() << "All unit connections: " << testNodeUnitConnectionIdList;
  // Remove connections that are on bus side
  for(i = 0; i < busSideTestNodeUnitConnectionIdList.size(); ++i){
    testNodeUnitConnectionIdList.removeAll(busSideTestNodeUnitConnectionIdList.at(i));
  }
  qDebug() << "Unit connections: " << testNodeUnitConnectionIdList;
  if(testNodeUnitConnectionIdList.isEmpty()){
    pvLastError = tn.lastError();
    return false;
  }
  /// \todo Let user choose witch one to use
  testNodeUnitConnectionId = testNodeUnitConnectionIdList.at(0);
  // Find linked connections
  linkedConnectionIdList = tn.getIdListOfUnitConnectionsLinkedToUnitConnectionId(testNodeUnitConnectionId);
  if(linkedConnectionIdList.isEmpty()){
    pvLastError = tn.lastError();
    return false;
  }
  /// \todo Let user choose if more than one connection was found
  linkedConnectionId = linkedConnectionIdList.at(0);
  // Edit test node unit
  if(!tn.setTestConnection(testNodeUnitId, linkedConnectionId)){
    pvLastError = tn.lastError();
    return false;
  }

  return true;
}
*/

  /**
bool mdtTtTestNodeEditor::assignTestConnectionToTestNodeUnitLits(const QList<QVariant> & testNodeUnitIdList, const QList<QVariant> & busSideTestNodeUnitConnectionIdList)
{
  int i;

  for(i = 0; i < testNodeUnitIdList.size(); ++i){
    if(!assignTestConnectionToTestNodeUnit(testNodeUnitIdList.at(i), busSideTestNodeUnitConnectionIdList)){
      return false;
    }
  }

  return true;
}
*/

/**
 * \todo Dans mdtAbstractSqlTableController:
 *   - Proposer auto-connection signaux/slots pour actions d'éditions lors de l'ajout d'un child controller
 *   - Serait une solution à l'édition multi-table !
 *   (- Proposer aussi reprise message handler du contrôleur parent)
 *   - Pour la reprise des PK->FK: notion de relation inverse
 */
bool mdtTtTestNodeEditor::setupTestNodeTable()
{
  Ui::mdtTtTestNodeEditor tne;
  std::shared_ptr<mdtSqlDataWidgetController> vtController;
  std::shared_ptr<mdtSqlDataWidgetController> tnController;
  mdtSqlRelationInfo relationInfo;

  // Setup main form widget
  setMainTableUi<Ui::mdtTtTestNodeEditor>(tne);
  connect(tne.pbSetVehicleType, SIGNAL(clicked()), this, SLOT(setBaseVehicleType()));
  // Setup form
  if(!setMainTable("VehicleType_tbl", "Test node", database())){
    return false;
  }
  // Get vehicle type controller and add TestNode_tbl as child to it
  vtController = tableController<mdtSqlDataWidgetController>("VehicleType_tbl");
  Q_ASSERT(vtController);
  relationInfo.setChildTableName("TestNode_tbl");
  relationInfo.addRelation("Id_PK", "VehicleType_Id_FK_PK", true);
  relationInfo.setRelationType(mdtSqlRelationInfo::OneToOne);
  if(!vtController->addChildController<mdtSqlDataWidgetController>(relationInfo, tr("Test node"))){
    pvLastError = vtController->lastError();
    return false;
  }
  // Get test node controller and map widgets to it
  tnController = vtController->childController<mdtSqlDataWidgetController>("TestNode_tbl");
  Q_ASSERT(tnController);
  tnController->addMapping(tne.fldAlias, "Alias");
  tnController->addMapping(tne.fldAddressString, "AddressString");
  /**
  if(!setMainTable("TestNode_tbl", "Test node", database())){
    return false;
  }
  */
  /*
   * Setup base vehicle type widget mapping
   */
  // Get test node controller and add it a new controller for Article_tbl
  /**
  tnController = tableController<mdtSqlDataWidgetController>("TestNode_tbl");
  Q_ASSERT(tnController);
  relationInfo.setChildTableName("VehicleType_tbl");
  relationInfo.addRelation("VehicleType_Id_FK_PK", "Id_PK", false);
  if(!tnController->addChildController<mdtSqlDataWidgetController>(relationInfo, tr("Base vehicle type"))){
    pvLastError = tnController->lastError();
    return false;
  }
  // Get freshly added vehicle type controller and map widgets to it
  vtController = tableController<mdtSqlDataWidgetController>("VehicleType_tbl");
  Q_ASSERT(vtController);
  vtController->addMapping(tne.leType, "Type");
  vtController->addMapping(tne.leSubType, "SubType");
  vtController->addMapping(tne.leSeriesNumber, "SeriesNumber");
  */

  return true;
}

bool mdtTtTestNodeEditor::setupTestNodeUnitTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddUnit;
  QPushButton *pbEditUnit;
  QPushButton *pbRemoveUnit;
  QPushButton *pbNumberIoPosRange;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("TestNodeUnit_view");
  relationInfo.addRelation("Id_PK", "TestNode_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Units"))){
    return false;
  }
  widget = sqlTableWidget("TestNodeUnit_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Unit_Id_FK_PK", true);
  widget->setColumnHidden("TestNode_Id_FK", true);
  widget->setColumnHidden("Type_Code_FK", true);
  // Set fields some user friendly name
  updateTestNodeUnitTable(QLocale());
  // Set some attributes on table view
  widget->addColumnToSortOrder("Type_Code_FK", Qt::AscendingOrder);
  widget->addColumnToSortOrder("IoPosition", Qt::AscendingOrder);
  widget->addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  widget->sort();
  widget->tableView()->resizeColumnsToContents();
  // Add buttons
  pbAddUnit = new QPushButton(tr("Add ..."));
  pbAddUnit->setIcon(QIcon::fromTheme("list-add"));
  connect(pbAddUnit, SIGNAL(clicked()), this, SLOT(addUnit()));
  widget->addWidgetToLocalBar(pbAddUnit);
  pbEditUnit = new QPushButton(tr("Edit unit"));
  connect(pbEditUnit, SIGNAL(clicked()), this, SLOT(editUnit()));
  widget->addWidgetToLocalBar(pbEditUnit);
  pbRemoveUnit = new QPushButton(tr("Remove"));
  pbRemoveUnit->setIcon(QIcon::fromTheme("list-remove"));
  connect(pbRemoveUnit, SIGNAL(clicked()), this, SLOT(removeUnits()));
  widget->addWidgetToLocalBar(pbRemoveUnit);
  // # I/O pos. range
  pbNumberIoPosRange = new QPushButton(tr("# I/O pos. range"));
  ///pbNumberIoPosRange->setIcon(QIcon::fromTheme("list-remove"));
  connect(pbNumberIoPosRange, SIGNAL(clicked()), this, SLOT(setIoPositionRange()));
  widget->addWidgetToLocalBar(pbNumberIoPosRange);
  widget->addStretchToLocalBar();

  return true;
}

void mdtTtTestNodeEditor::updateTestNodeUnitTable(const QLocale & locale)
{
  auto widget = sqlTableWidget("TestNodeUnit_view");
  Q_ASSERT(widget != nullptr);
  // Set fields a user friendly name (using tr() )
  widget->setHeaderData("SchemaPosition", tr("Schema\nposition"));
  widget->setHeaderData("IoPosition", tr("I/O\npos."));
  widget->setHeaderData("UnitConnectorName", tr("Connector"));
  widget->setHeaderData("UnitContactName", tr("Contact"));
  widget->setHeaderData("SignalName", tr("Signal name"));
  widget->setHeaderData("Manufacturer", tr("Manufacturer"));
  widget->setHeaderData("ManufacturerCode", tr("Manufacturer\narticle code"));
  // Select Designation and Name fields regarding language
  switch(locale.language()){
    case QLocale::French:
      // Designation fields
      widget->setColumnHidden("DesignationEN", true);
      widget->setColumnHidden("DesignationDE", true);
      widget->setColumnHidden("DesignationIT", true);
      widget->setHeaderData("DesignationFR", tr("Designation"));
      // Name fields
      widget->setColumnHidden("NameEN", true);
      widget->setColumnHidden("NameDE", true);
      widget->setColumnHidden("NameIT", true);
      widget->setHeaderData("NameFR", tr("Type"));
      break;
    case QLocale::German:
      // Designation fields
      widget->setColumnHidden("DesignationEN", true);
      widget->setColumnHidden("DesignationFR", true);
      widget->setColumnHidden("DesignationIT", true);
      widget->setHeaderData("DesignationDE", tr("Designation"));
      // Name fields
      widget->setColumnHidden("NameEN", true);
      widget->setColumnHidden("NameFR", true);
      widget->setColumnHidden("NameIT", true);
      widget->setHeaderData("NameDE", tr("Type"));
      break;
    case QLocale::Italian:
      // Designation fields
      widget->setColumnHidden("DesignationFR", true);
      widget->setColumnHidden("DesignationDE", true);
      widget->setColumnHidden("DesignationEN", true);
      widget->setHeaderData("DesignationIT", tr("Designation"));
      // Name fields
      widget->setColumnHidden("NameFR", true);
      widget->setColumnHidden("NameDE", true);
      widget->setColumnHidden("NameEN", true);
      widget->setHeaderData("NameIT", tr("Type"));
      break;
    default:
      // Designation fields
      widget->setColumnHidden("DesignationFR", true);
      widget->setColumnHidden("DesignationDE", true);
      widget->setColumnHidden("DesignationIT", true);
      widget->setHeaderData("DesignationEN", tr("Designation"));
      // Name fields
      widget->setColumnHidden("NameFR", true);
      widget->setColumnHidden("NameDE", true);
      widget->setColumnHidden("NameIT", true);
      widget->setHeaderData("NameEN", tr("Type"));
  }
}

bool mdtTtTestNodeEditor::setupTestNodeBusTable()
{
  mdtSqlTableWidget *widget;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("TestNodeBus_tbl");
  ///relationInfo.addRelation("VehicleType_Id_FK_PK", "TestNode_Id_FK", false);
  relationInfo.addRelation("Id_PK", "TestNode_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Bus"))){
    return false;
  }
  /**
  if(!addChildTable("TestNodeBus_tbl", tr("Bus"), database())){
    return false;
  }
  if(!addRelation("VehicleType_Id_FK_PK", "TestNodeBus_tbl", "TestNode_Id_FK")){
    return false;
  }
  */
  widget = sqlTableWidget("TestNodeBus_tbl");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("TestNode_Id_FK", true);
  // Set some attributes on table view
  widget->enableLocalEdition();
  widget->addStretchToLocalBar();
  widget->addColumnToSortOrder("NameEN", Qt::AscendingOrder);
  widget->tableView()->resizeColumnsToContents();

  return true;
}

bool mdtTtTestNodeEditor::setupTestNodeUnitConnectionTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbUpdateConnections;
  QPushButton *pbSetBus;
  QPushButton *pbUnsetBus;
  mdtSqlRelationInfo relationInfo;
  QStringList languageFieldsToHide;

  relationInfo.setChildTableName("TestNodeUnitConnection_view");
  ///relationInfo.addRelation("VehicleType_Id_FK_PK", "TestNode_Id_FK", false);
  relationInfo.addRelation("Id_PK", "TestNode_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Unit connections"))){
    return false;
  }
  /**
  if(!addChildTable("TestNodeUnitConnection_view", tr("Unit connections"), database())){
    return false;
  }
  if(!addRelation("VehicleType_Id_FK_PK", "TestNodeUnitConnection_view", "TestNode_Id_FK")){
    return false;
  }
  */
  widget = sqlTableWidget("TestNodeUnitConnection_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Unit_Id_FK_PK", true);
  widget->setColumnHidden("TestNode_Id_FK", true);
  widget->setColumnHidden("UnitConnection_Id_FK_PK", true);
  widget->setColumnHidden("TestNodeUnit_Id_FK", true);
  widget->setColumnHidden("TestNodeBus_Id_FK", true);
  // Hide some filds regarding current language
  

  // Set fields a user friendly name
  widget->setHeaderData("SchemaPosition", tr("Schema\nposition"));
  widget->setHeaderData("ioPosition", tr("I/O\nposition"));
  widget->setHeaderData("UnitConnectorName", tr("Connector"));
  widget->setHeaderData("UnitContactName", tr("Contact"));
  // Set some attributes on table view
  widget->addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  widget->addColumnToSortOrder("UnitConnectorName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("UnitContactName", Qt::AscendingOrder);
  widget->sort();
  widget->tableView()->resizeColumnsToContents();
  // Update connections button
  pbUpdateConnections = new QPushButton(tr("Update connections"));
  ///pbSetBus->setIcon(QIcon::fromTheme("list-add"));
  connect(pbUpdateConnections, SIGNAL(clicked()), this, SLOT(updateConnections()));
  widget->addWidgetToLocalBar(pbUpdateConnections);
  // Set bus button
  pbSetBus = new QPushButton(tr("Assign to bus ..."));
  ///pbSetBus->setIcon(QIcon::fromTheme("list-add"));
  connect(pbSetBus, SIGNAL(clicked()), this, SLOT(setBusToUnitConnection()));
  widget->addWidgetToLocalBar(pbSetBus);
  // Unset bus button
  pbUnsetBus = new QPushButton(tr("Remove from bus ..."));
  ///pbSetBus->setIcon(QIcon::fromTheme("list-add"));
  connect(pbUnsetBus, SIGNAL(clicked()), this, SLOT(removeBusFromUnitConnection()));
  widget->addWidgetToLocalBar(pbUnsetBus);
  widget->addStretchToLocalBar();

  return true;
}
