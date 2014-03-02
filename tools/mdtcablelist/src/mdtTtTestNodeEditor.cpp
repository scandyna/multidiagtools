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
#include "mdtTtTestNodeEditor.h"
#include "mdtTtTestNode.h"
#include "ui_mdtTtTestNodeEditor.h"
#include "mdtSqlFormOld.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlWindowOld.h"
#include "mdtSqlRelation.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtTtTestNodeUnitData.h"
#include "mdtTtTestNodeUnitEditor.h"
#include "mdtSqlDialog.h"
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QDataWidgetMapper>
#include <QVariant>
#include <QString>
#include <QPushButton>
#include <QSqlError>
#include <QModelIndex>
#include <QInputDialog>
#include <QMessageBox>

#include <QDebug>

mdtTtTestNodeEditor::mdtTtTestNodeEditor(QObject *parent, QSqlDatabase db)
 : mdtClEditor(parent, db)
{
}

void mdtTtTestNodeEditor::setBaseVehicleType()
{
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlQueryModel model;
  QString sql;
  QVariant vehicleTypeId;

  // Setup vehicle type selection dialog and show it to user
  sql = "SELECT * FROM VehicleType_tbl;";
  ///model.setQuery(sql, database());
  selectionDialog.setMessage("Please select vehicle type.");
  ///selectionDialog.setModel(&model, false);
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
  if(!form()->setCurrentData("TestNode_tbl", "VehicleType_Id_FK_PK", vehicleTypeId)){
    return;
  }
}

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
  /**
  if(!tn.addTestNodeUnits(unitIdList, baseVehicleTypeId, typeCode, busName)){
    pvLastError = tn.lastError();
    displayLastError();
    return;
  }
  */
  // Assign test connection to each test node unit
  if(!assignTestConnectionToTestNodeUnitLits(unitIdList, unitConnectionIdList)){
    displayLastError();
  }
  // Update TestNodeUnit_view
  form()->select("TestNodeUnit_view");
}

void mdtTtTestNodeEditor::editUnit()
{
  mdtTtTestNodeUnitEditor *tnue;
  mdtSqlDialog dialog;
  QVariant testNodeUnitId;

  testNodeUnitId = form()->currentData("TestNodeUnit_view", "Unit_Id_FK_PK");
  if(testNodeUnitId.isNull()){
    return;
  }
  tnue = new mdtTtTestNodeUnitEditor(0, database());
  if(!tnue->setupTables()){
    pvLastError = tnue->lastError();
    displayLastError();
    return;
  }
  if(!tnue->setMainTableFilter("Unit_Id_FK_PK", testNodeUnitId)){
    pvLastError = tnue->lastError();
    displayLastError();
    delete tnue;
    return;
  }
  dialog.setSqlForm(tnue);
  dialog.resize(700, 400);
  dialog.enableEdition();
  dialog.setWindowTitle(tr("Test node unit edition"));
  dialog.exec();
  // Update TestNodeUnit_view
  form()->select("TestNodeUnit_view");
}

void mdtTtTestNodeEditor::removeUnits()
{
  mdtSqlTableWidget *widget;
  mdtTtTestNode tn(this, database());
  QMessageBox msgBox;
  QModelIndexList indexes;

  widget = form()->sqlTableWidget("TestNodeUnit_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  indexes = widget->indexListOfSelectedRows("Unit_Id_FK_PK");
  if(indexes.size() < 1){
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
  if(!tn.removeTestNodeUnits(indexes)){
    pvLastError = tn.lastError();
    displayLastError();
    return;
  }
  // Update TestNodeUnit_view
  form()->select("TestNodeUnit_view");
}

QVariant mdtTtTestNodeEditor::selectTestNodeUnitType()
{
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;
  QString msg;

  // Setup model
  sql = "SELECT * FROM TestNodeUnitType_tbl";
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get list test node unit types."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeEditor");
    pvLastError.commit();
    displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  msg = tr("Select the type for units that will be linked.");
  selectionDialog.setMessage(msg);
  ///selectionDialog.setModel(&model, false);
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

bool mdtTtTestNodeEditor::setupTables()
{
  if(!setupTestNodeTable()){
    return false;
  }
  if(!setupTestNodeUnitTable()){
    return false;
  }
  return true;
}

bool mdtTtTestNodeEditor::setupTestNodeTable()
{
  Ui::mdtTtTestNodeEditor tne;
  QSqlTableModel *testNodeModel;
  QDataWidgetMapper *baseVehicleTypeMapper;
  QSqlTableModel *baseVehicleTypeModel;
  mdtSqlRelation *baseVehicleTypeRelation;

  // Setup main form widget
  tne.setupUi(form()->mainSqlWidget());
  connect(tne.pbSetVehicleType, SIGNAL(clicked()), this, SLOT(setBaseVehicleType()));
  ///connect(this, SIGNAL(unitEdited()), form()->mainSqlWidget(), SIGNAL(dataEdited()));
  // Setup form
  if(!form()->setTable("TestNode_tbl", "Test node", database())){
    return false;
  }
  sqlWindow()->enableNavigation();
  sqlWindow()->enableEdition();
  sqlWindow()->resize(800, 500);
  sqlWindow()->setWindowTitle(tr("Test node edition"));
  /*
   * Setup base unit widget mapping
   */
  testNodeModel = form()->model("TestNode_tbl");
  Q_ASSERT(testNodeModel != 0);
  // Setup base article model
  baseVehicleTypeModel = new QSqlTableModel(this, database());
  baseVehicleTypeModel->setTable("VehicleType_tbl");
  if(!baseVehicleTypeModel->select()){
    return false;
  }
  // Setup base article widget mapper
  baseVehicleTypeMapper = new QDataWidgetMapper(this);
  baseVehicleTypeMapper->setModel(baseVehicleTypeModel);
  baseVehicleTypeMapper->addMapping(tne.leType, baseVehicleTypeModel->fieldIndex("Type"));
  baseVehicleTypeMapper->addMapping(tne.leSubType, baseVehicleTypeModel->fieldIndex("SubType"));
  baseVehicleTypeMapper->addMapping(tne.leSeriesNumber, baseVehicleTypeModel->fieldIndex("SeriesNumber"));
  // Setup base article relation
  baseVehicleTypeRelation = new mdtSqlRelation(this);
  baseVehicleTypeRelation->setParentModel(testNodeModel);
  baseVehicleTypeRelation->setChildModel(baseVehicleTypeModel);
  if(!baseVehicleTypeRelation->addRelation("VehicleType_Id_FK_PK", "Id_PK", false)){
    return false;
  }
  connect(form()->mainSqlWidget(), SIGNAL(currentRowChanged(int)), baseVehicleTypeRelation, SLOT(setParentCurrentIndex(int)));
  connect(baseVehicleTypeRelation, SIGNAL(childModelFilterApplied()), baseVehicleTypeMapper, SLOT(toFirst()));
  connect(baseVehicleTypeRelation, SIGNAL(childModelIsEmpty()), baseVehicleTypeMapper, SLOT(revert()));
  // Force a update
  form()->mainSqlWidget()->setCurrentIndex(form()->mainSqlWidget()->currentRow());

  return true;
}

bool mdtTtTestNodeEditor::setupTestNodeUnitTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddUnit;
  QPushButton *pbEditUnit;
  QPushButton *pbRemoveUnit;

  if(!form()->addChildTable("TestNodeUnit_view", tr("Units"), database())){
    return false;
  }
  if(!form()->addRelation("VehicleType_Id_FK_PK", "TestNodeUnit_view", "TestNode_Id_FK")){
    return false;
  }
  widget = form()->sqlTableWidget("TestNodeUnit_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Unit_Id_FK_PK", true);
  widget->setColumnHidden("TestNode_Id_FK", true);
  ///widget->setColumnHidden("Type_Code_FK", true);
  widget->setColumnHidden("TestConnection_Id_FK", true);
  widget->setColumnHidden("", true);
  // Set fields a user friendly name
  widget->setHeaderData("SchemaPosition", tr("Schema\nposition"));
  widget->setHeaderData("UnitConnectorName", tr("Connector"));
  widget->setHeaderData("UnitContactName", tr("Contact"));
  widget->setHeaderData("SignalName", tr("Signal name"));
  widget->setHeaderData("FunctionEN", tr("Function (ENG)"));
  widget->setHeaderData("FunctionFR", tr("Function (FRA)"));
  widget->setHeaderData("FunctionDE", tr("Function (DEU)"));
  widget->setHeaderData("FunctionIT", tr("Function (ITA)"));
  widget->setHeaderData("NameEN", tr("Type (ENG)"));
  widget->setHeaderData("NameFR", tr("Type (FRA)"));
  widget->setHeaderData("NameDE", tr("Type (DEU)"));
  widget->setHeaderData("NameIT", tr("Type (ITA)"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons
  pbAddUnit = new QPushButton(tr("Add units ..."));
  connect(pbAddUnit, SIGNAL(clicked()), this, SLOT(addUnits()));
  widget->addWidgetToLocalBar(pbAddUnit);
  pbEditUnit = new QPushButton(tr("Edit unit"));
  connect(pbEditUnit, SIGNAL(clicked()), this, SLOT(editUnit()));
  widget->addWidgetToLocalBar(pbEditUnit);
  pbRemoveUnit = new QPushButton(tr("Remove units"));
  connect(pbRemoveUnit, SIGNAL(clicked()), this, SLOT(removeUnits()));
  widget->addWidgetToLocalBar(pbRemoveUnit);
  widget->addStretchToLocalBar();

  return true;
}
