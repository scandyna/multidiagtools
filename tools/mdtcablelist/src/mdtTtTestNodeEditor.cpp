/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtSqlForm.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlWindow.h"
#include "mdtSqlRelation.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QDataWidgetMapper>
#include <QVariant>
#include <QString>
#include <QPushButton>
#include <QSqlError>

#include <QDebug>

mdtTtTestNodeEditor::mdtTtTestNodeEditor(QObject *parent, QSqlDatabase db)
 : mdtClEditor(parent, db)
{
}

void mdtTtTestNodeEditor::setBaseVehicleType()
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlQueryModel model;
  QString sql;
  QVariant vehicleTypeId;

  // Setup vehicle type selection dialog and show it to user
  sql = "SELECT * FROM VehicleType_tbl;";
  model.setQuery(sql, database());
  selectionDialog.setMessage("Please select vehicle type.");
  selectionDialog.setModel(&model, false);
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
  mdtTtTestNode tn(database());
  QVariant baseVehicleTypeId;
  QVariant typeCode;
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
  // Select connection ID that is linked to a BUS
  firstUnitConnectionId = selectUnitConnection(firstUnitId);
  if(firstUnitConnectionId.isNull()){
    return;
  }
  qDebug() << "CNN ID: " << firstUnitConnectionId;
  // Get related units
  unitConnectionIdList = tn.getIdListOfUnitConnectionsLinkedToUnitConnectionId(firstUnitConnectionId);
  if(unitConnectionIdList.isEmpty()){
    displayLastError();
    return;
  }
  unitIdList = tn.getIdListOfUnitIdForUnitConnectionIdList(unitConnectionIdList);
  if(unitIdList.isEmpty()){
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
}

QVariant mdtTtTestNodeEditor::selectTestNodeUnitType()
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  QString sql;
  QString msg;

  // Setup model
  sql = "SELECT * FROM TestNodeUnitType_tbl";
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
  // Setup and show dialog
  msg = tr("Select the type for units that will be linked.");
  selectionDialog.setMessage(msg);
  selectionDialog.setModel(&model, false);
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

  mdtTtTestNode tn(database());
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  QString sql;
  QString msg;

  // Setup model
  sql = tn.sqlForUnitSelection(vehicleTypeId);
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
  // Setup and show dialog
  msg = tr("Select a unit that is in group of those to use.");
  selectionDialog.setMessage(msg);
  selectionDialog.setModel(&model, false);
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
  mdtTtTestNode tn(database());
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  QString sql;
  QString msg;

  // Setup model
  sql = tn.sqlForUnitConnectionSelection(unitId);
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
  // Setup and show dialog
  msg = tr("Select a connection that is linked to a bus.");
  selectionDialog.setMessage(msg);
  selectionDialog.setModel(&model, false);
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
  mdtTtTestNode tn(database());
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  QString sql;
  QString msg;
  QList<QVariant> selectedUnitIdList;

  // Setup model
  sql = tn.sqlForUnitSelectionByUnitIdList(unitIdList);
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
  // Setup and show dialog
  msg = tr("Select units to use.");
  selectionDialog.setMessage(msg);
  selectionDialog.setModel(&model, true);
  ///selectionDialog.setColumnHidden("", true);
  ///selectionDialog.setHeaderData("Unit_Id_FK", tr("Variant"));
  selectionDialog.addSelectionResultColumn("Unit_Id");
  selectionDialog.resize(700, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return selectedUnitIdList;
  }

  return selectedUnitIdList;
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
  widget->addStretchToLocalBar();

  return true;
}
