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
#include "mdtClUnitEditor.h"
#include "ui_mdtClUnitEditor.h"
#include "mdtSqlWindow.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtError.h"
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QStringList>
#include <QString>
#include <QList>
#include <QVariant>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMessageBox>

#include <QDebug>

mdtClUnitEditor::mdtClUnitEditor(QObject *parent, const QSqlDatabase db)
 : QObject(parent)
{
  pvDatabase = db;
  // Setup unit
  pvUnitWidget = new mdtSqlFormWidget;
  Ui::mdtClUnitEditor *e = new Ui::mdtClUnitEditor;
  e->setupUi(pvUnitWidget);
  pvUnitModel = new QSqlTableModel(this, pvDatabase);
  // Setup connection
  pvUnitConnectionWidget = new mdtSqlTableWidget;
  pvUnitConnectionModel = new QSqlTableModel(this, pvDatabase);
  // Setup VehicleType_Unit
  pvVehicleTypeWidget = new mdtSqlTableWidget;
  pvVehicleTypeModel = new QSqlTableModel(this, pvDatabase);

  pvUnitConnectionRelation = new mdtSqlRelation;
  pvVehicleTypeRelation = new mdtSqlRelation;
}

mdtClUnitEditor::~mdtClUnitEditor()
{
  ///delete pvUnitWidget;
  ///delete pvUnitConnectionWidget;
  delete pvUnitConnectionRelation;
  delete pvVehicleTypeRelation;
  ///delete pvUnitModel;
  ///delete pvUnitConnectionModel;
  ///delete pvVehicleTypeModel;
}

bool mdtClUnitEditor::setupTables(bool includeConnections)
{
  QSqlError sqlError;

  // Setup Unit table
  if(!setupUnitTable()){
    return false;
  }
  // Setup connection table
  if(!setupUnitConnectionTable()){
    return false;
  }
  // Setup VehicleType_Unit
  if(!setupVehicleTable()){
    return false;
  }

  return true;
}

void mdtClUnitEditor::setupUi(mdtSqlWindow *window)
{
  Q_ASSERT(window != 0);

  window->setSqlWidget(pvUnitWidget);
  window->addChildWidget(pvUnitConnectionWidget, "Connections");
  window->addChildWidget(pvVehicleTypeWidget, "Vehicles");
  window->enableNavigation();
  window->enableEdition();
  window->resize(700, 500);
}

void mdtClUnitEditor::assignVehicle()
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlQueryModel model;
  QString sql;
  QList<QVariant> selectedItem;
  int vehicleId;
  int unitId;
  QSqlError sqlError;

  // Get current unit ID
  unitId = currentUnitId();
  if(unitId < 0){
    return;
  }
  // SQL query to get the list of vehicles that are not allready assigned
  sql = "SELECT Id_PK, Type , SubType , SeriesNumber "\
        "FROM VehicleType_tbl "\
        "WHERE Id_PK NOT IN ( "\
        " SELECT VehicleType_Id_FK "\
        " FROM VehicleType_Unit_tbl "\
        " WHERE Unit_Id_FK = " + QString::number(unitId) + " ) "\
        "ORDER BY Type ASC, SubType ASC, SeriesNumber ASC;";
  model.setQuery(sql, pvDatabase);
  // Setup and show dialog
  selectionDialog.setMessage("Please select a vehicle");
  selectionDialog.setModel(&model);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setHeaderData("SubType", tr("Variant"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  selectionDialog.exec();
  selectedItem = selectionDialog.selectionResult();
  if(selectedItem.size() != 1){
    return;
  }
  if(!selectedItem.at(0).isValid()){
    return;
  }
  vehicleId = selectedItem.at(0).toInt();
  if(vehicleId < 0){
    return;
  }
  // Insert selected vehicle into VehicleType_Unit_tbl
  QSqlQuery query(pvDatabase);
  sql = "INSERT INTO VehicleType_Unit_tbl "\
        "(Unit_Id_FK, VehicleType_Id_FK) "\
        "VALUES (" + QString::number(unitId) + ", " + QString::number(vehicleId) + ")";
  if(!query.exec(sql)){
    sqlError = query.lastError();
    qDebug() << "ERR insert: " << sqlError;
    QMessageBox msgBox;
    msgBox.setText(tr("Could not assign vehicle"));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  pvVehicleTypeModel->select();
}

void mdtClUnitEditor::removeVehicleAssignation()
{
  Q_ASSERT(pvVehicleTypeWidget->selectionModel() != 0);

  QString sql;
  int unitId;
  QSqlError sqlError;
  int i;
  int ret;
  int row;
  QList<int> rows;
  QMessageBox msgBox;
  QModelIndex index;
  int vehicleIdColumn;
  QVariant vehicleId;
  QModelIndexList indexes = pvVehicleTypeWidget->selectionModel()->selectedIndexes();

  // If nothing was selected, we do nothing
  if(indexes.size() < 1){
    return;
  }
  // Get current unit ID
  unitId = currentUnitId();
  if(unitId < 0){
    return;
  }
  // Get VehicleType_Id_FK column
  vehicleIdColumn = pvVehicleTypeModel->record().indexOf("VehicleType_Id_FK");
  if(vehicleIdColumn < 0){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove assignations between selected vehicles and current unit."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  ret = msgBox.exec();
  if(ret != QMessageBox::Yes){
    return;
  }
  // Build list of selected rows
  for(i = 0; i < indexes.size(); ++i){
    row = indexes.at(i).row();
    Q_ASSERT(row >= 0);
    if(!rows.contains(row)){
      rows.append(row);
    }
  }
  // Delete selected rows
  row = -1;
  sql = "DELETE FROM VehicleType_Unit_tbl "\
        "WHERE Unit_Id_FK = " + QString::number(unitId);
  for(i = 0; i < rows.size(); ++i){
    if(i == 0){
      sql += " AND ( ";
    }else{
      sql += " OR ";
    }
    // Get vehicle ID for current row
    index = pvVehicleTypeModel->index(rows.at(i), vehicleIdColumn);
    Q_ASSERT(index.isValid());
    vehicleId = pvVehicleTypeModel->data(index);
    if(vehicleId.isValid()){
      sql += "VehicleType_Id_FK = " + QString::number(vehicleId.toInt());
    }
  }
  sql += " ) ";
  // Submit query
  QSqlQuery query(pvDatabase);
  if(!query.exec(sql)){
    sqlError = query.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Assignation removing failed."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  pvVehicleTypeModel->select();
}

int mdtClUnitEditor::currentUnitId()
{
  int unitIdColumn;
  QModelIndex index;

  if(pvUnitWidget->currentRow() < 0){
    return -1;
  }
  unitIdColumn = pvUnitModel->record().indexOf("Id_PK");
  if(unitIdColumn < 0){
    return -1;
  }
  index = pvUnitModel->index(pvUnitWidget->currentRow(), unitIdColumn);
  if(!index.isValid()){
    return -1;
  }

  return pvUnitModel->data(index).toInt();
}

bool mdtClUnitEditor::setupUnitTable()
{
  QSqlError sqlError;

  pvUnitModel->setTable("Unit_tbl");
  if(!pvUnitModel->select()){
    sqlError = pvUnitModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'Unit_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnitEditor");
    e.commit();
    return false;
  }
  pvUnitWidget->setModel(pvUnitModel);
  pvUnitWidget->mapFormWidgets();

  return true;
}

bool mdtClUnitEditor::setupUnitConnectionTable()
{
  QSqlError sqlError;

  pvUnitConnectionModel->setTable("UnitConnection_tbl");
  if(!pvUnitConnectionModel->select()){
    sqlError = pvUnitConnectionModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'UnitConnection_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnitEditor");
    e.commit();
    return false;
  }
  pvUnitConnectionWidget->setModel(pvUnitConnectionModel);
  pvUnitConnectionWidget->enableLocalEdition();
  // Hide relation fields and PK
  pvUnitConnectionWidget->setColumnHidden("Id_PK", true);
  pvUnitConnectionWidget->setColumnHidden("Unit_Id_FK", true);
  // Give fields a user friendly name
  ///pvUnitConnectionWidget->setHeaderData("ConnectorName", tr("Connector"));
  ///pvUnitConnectionWidget->setHeaderData("ContactName", tr("Contact"));
  ///pvUnitConnectionWidget->setHeaderData("SchemaPage", tr("Schema page"));
  ///pvUnitConnectionWidget->setHeaderData("IoType", tr("I/O type"));
  // Setup Unit <-> Connection relation
  pvUnitConnectionRelation->setParentModel(pvUnitModel);
  pvUnitConnectionRelation->setChildModel(pvUnitConnectionModel);
  if(!pvUnitConnectionRelation->addRelation("Id_PK", "Unit_Id_FK")){
    return false;
  }
  pvUnitWidget->addChildWidget(pvUnitConnectionWidget, pvUnitConnectionRelation);

  return true;
}

bool mdtClUnitEditor::setupVehicleTable()
{
  QSqlError sqlError;
  QPushButton *pbAddVehicle;
  QPushButton *pbRemoveVehicle;

  pvVehicleTypeModel->setTable("Unit_VehicleType_view");
  if(!pvVehicleTypeModel->select()){
    sqlError = pvVehicleTypeModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'Unit_VehicleType_view'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnitEditor");
    e.commit();
    return false;
  }
  pvVehicleTypeWidget->setEditionEnabled(false);
  pvVehicleTypeWidget->setModel(pvVehicleTypeModel);
  // Hide relation fields and PK
  ///pvVehicleTypeWidget->setColumnHidden("Id_PK", true);
  pvVehicleTypeWidget->setColumnHidden("Unit_Id_FK", true);
  pvVehicleTypeWidget->setColumnHidden("VehicleType_Id_FK", true);
  // Give fields a user friendly name
  pvVehicleTypeWidget->setHeaderData("SubType", tr("Variant"));
  pvVehicleTypeWidget->setHeaderData("SeriesNumber", tr("Serie"));
  // Setup Unit <-> VehicleType relation
  pvVehicleTypeRelation->setParentModel(pvUnitModel);
  pvVehicleTypeRelation->setChildModel(pvVehicleTypeModel);
  if(!pvVehicleTypeRelation->addRelation("Id_PK", "Unit_Id_FK")){
    return false;
  }
  pvUnitWidget->addChildWidget(pvVehicleTypeWidget, pvVehicleTypeRelation);
  // Add vehicle button
  pbAddVehicle = new QPushButton(tr("Assign vehicle"));
  pvVehicleTypeWidget->addWidgetToLocalBar(pbAddVehicle);
  connect(pbAddVehicle, SIGNAL(clicked()), this, SLOT(assignVehicle()));
  // Remove vehicle button
  pbRemoveVehicle = new QPushButton(tr("Remove vehicle"));
  pvVehicleTypeWidget->addWidgetToLocalBar(pbRemoveVehicle);
  connect(pbRemoveVehicle, SIGNAL(clicked()), this, SLOT(removeVehicleAssignation()));
  pvVehicleTypeWidget->addStretchToLocalBar();

  return true;
}
