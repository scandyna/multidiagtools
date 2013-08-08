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
#include "mdtClTerminalBlockEditor.h"
#include "ui_mdtClTerminalBlockEditor.h"
#include "mdtSqlWindow.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtClTerminalEditorDialog.h"
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
#include <QWidget>
#include <QHBoxLayout>

#include <QDebug>

mdtClTerminalBlockEditor::mdtClTerminalBlockEditor(QObject *parent, const QSqlDatabase db)
 : QObject(parent)
{
  pvDatabase = db;
  // Setup unit
  pvUnitWidget = new mdtSqlFormWidget;
  Ui::mdtClTerminalBlockEditor ue;
  ue.setupUi(pvUnitWidget);
  pvUnitModel = new QSqlTableModel(this, pvDatabase);
  ///connect(ue.pbSetBaseArticle, SIGNAL(clicked()), this, SLOT(setBaseArticle()));
  connect(this, SIGNAL(unitEdited()), pvUnitWidget, SIGNAL(dataEdited()));
  // Setup terminal ( = unit connection) view
  pvTerminalEditWidget = new mdtSqlTableWidget;
  pvTerminalEditModel = new QSqlTableModel(this, pvDatabase);
  pvTerminalEditRelation = new mdtSqlRelation;
  // Setup terminal link view
  pvTerminalLinkEditWidget = new mdtSqlTableWidget;
  pvTerminalLinkEditModel = new QSqlTableModel(this, pvDatabase);
  pvTerminalLinkEditRelation = new mdtSqlRelation;
  // Setup VehicleType_Unit
  pvVehicleTypeWidget = new mdtSqlTableWidget;
  pvVehicleTypeModel = new QSqlTableModel(this, pvDatabase);
  pvVehicleTypeRelation = new mdtSqlRelation;
}

mdtClTerminalBlockEditor::~mdtClTerminalBlockEditor()
{
  delete pvTerminalEditRelation;
  delete pvTerminalLinkEditRelation;
  delete pvVehicleTypeRelation;
}

bool mdtClTerminalBlockEditor::setupTables()
{
  // Setup Unit table
  if(!setupUnitTable()){
    return false;
  }
  // Setup terminal edit table
  if(!setupTerminalEditTable()){
    return false;
  }
  // Setup terminal link edit table
  if(!setupTerminalLinkEditTable()){
    return false;
  }
  // Setup VehicleType_Unit
  if(!setupVehicleTable()){
    return false;
  }

  return true;
}

void mdtClTerminalBlockEditor::setupUi(mdtSqlWindow *window)
{
  Q_ASSERT(window != 0);

  window->setSqlWidget(pvUnitWidget);
  // Add child widgets
  window->addChildWidget(pvTerminalEditWidget, tr("Terminals"));
  window->addChildWidget(pvTerminalLinkEditWidget, tr("Terminal links"));
  window->addChildWidget(pvVehicleTypeWidget, tr("Vehicles"));

  window->enableNavigation();
  window->enableEdition();
  window->resize(900, 500);
}

void mdtClTerminalBlockEditor::assignVehicle()
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

void mdtClTerminalBlockEditor::removeVehicleAssignation()
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

void mdtClTerminalBlockEditor::addTerminal()
{
  Q_ASSERT(pvTerminalEditModel);

  mdtClTerminalEditorDialog d;
  int ret;
  QModelIndex index;
  int row;
  int column;
  QString terminalBlockName;

  // setup terminal edition dialog and show it to user
  terminalBlockName = pvUnitWidget->currentData("SchemaPosition").toString();
  d.setTerminalBlockName(terminalBlockName);
  d.setConnectorName(terminalBlockName);
  d.setWindowTitle(tr("Terminal edition"));
  ret = d.exec();
  if(ret != QDialog::Accepted){
    return;
  }
  // Add treminal to model
  row = pvTerminalEditModel->rowCount();
  if(!pvTerminalEditModel->insertRow(row)){
    displayError(tr("Database error"), tr("Insertion of new terminal failed"), pvTerminalEditModel->lastError(), true);
    return;
  }
  column = pvTerminalEditModel->fieldIndex("ConnectorName");
  index = pvTerminalEditModel->index(row, column);
  if(!index.isValid()){
    displayError(tr("Database error"), tr("Insertion of new terminal failed due to a invalid index for 'ConnectorName'"), pvTerminalEditModel->lastError(), true);
    return;
  }
  if(!pvTerminalEditModel->setData(index, d.connectorName())){
    displayError(tr("Database error"), tr("Insertion of new terminal failed"), pvTerminalEditModel->lastError(), true);
    return;
  }
  column = pvTerminalEditModel->fieldIndex("ContactName");
  index = pvTerminalEditModel->index(row, column);
  if(!index.isValid()){
    displayError(tr("Database error"), tr("Insertion of new terminal failed due to a invalid index for 'ContactName'"), pvTerminalEditModel->lastError(), true);
    qDebug() << "Invalid index: " << index;
    return;
  }
  if(!pvTerminalEditModel->setData(index, d.contactName())){
    displayError(tr("Database error"), tr("Insertion of new terminal failed"), pvTerminalEditModel->lastError(), true);
    return;
  }
  if(!pvTerminalEditModel->submitAll()){
    displayError(tr("Database error"), tr("Insertion of new terminal failed"), pvTerminalEditModel->lastError(), true);
    return;
  }
  /// \todo Links...

}

/// \todo Links...
void mdtClTerminalBlockEditor::removeTerminal()
{
  Q_ASSERT(pvTerminalEditWidget->selectionModel() != 0);

  int i;
  int ret;
  int row;
  int unitConnectionIdColumn;
  QVariant id;
  QList<int> rows;
  QList<int> unitConnectionIds;
  QMessageBox msgBox;
  QModelIndex index;
  QModelIndexList indexes = pvTerminalEditWidget->selectionModel()->selectedIndexes();

  // If nothing was selected, we do nothing
  if(indexes.size() < 1){
    return;
  }
  // Get column of unit connecion ID (PK) in view
  unitConnectionIdColumn = pvTerminalEditModel->record().indexOf("Id_PK");
  if(unitConnectionIdColumn < 0){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to delete selected terminals for current terminal block."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  ret = msgBox.exec();
  if(ret != QMessageBox::Yes){
    return;
  }
  // Build rows list
  for(i = 0; i < indexes.size(); ++i){
    row = indexes.at(i).row();
    if(!rows.contains(row)){
      rows.append(row);
    }
  }
  // Build list of terminals to remove
  for(i = 0; i < rows.size(); ++i){
    index = pvTerminalEditModel->index(rows.at(i), unitConnectionIdColumn);
    if(index.isValid()){
      id = pvTerminalEditModel->data(index);
      if(id.isValid()){
        unitConnectionIds.append(id.toInt());
      }
    }
  }
  // Remove
  for(i = 0; i < unitConnectionIds.size(); ++i){
    QString sql;
    QSqlQuery query(pvDatabase);
    sql = "DELETE FROM UnitConnection_tbl WHERE Id_PK = " + QString::number(unitConnectionIds.at(i));
    if(!query.exec(sql)){
      QSqlError sqlError = query.lastError();
      mdtError e(MDT_DATABASE_ERROR, "Removing connections from table 'UnitConnection_tbl' failed for Id_PK " + QString::number(unitConnectionIds.at(i)) , mdtError::Error);
      e.setSystemError(sqlError.number(), sqlError.text());
      e.commit();
      QMessageBox msgBox;
      msgBox.setText(tr("Error occured while removing connecions."));
      msgBox.setInformativeText(tr("Please see details."));
      msgBox.setDetailedText(sqlError.text());
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      return;
    }
  }
  // Refresh connecions view
  pvTerminalEditModel->select();
}

int mdtClTerminalBlockEditor::currentUnitId()
{
  QVariant id;

  id = pvUnitWidget->currentData("Id_PK");
  if(!id.isValid()){
    return -1;
  }

  return id.toInt();
}

bool mdtClTerminalBlockEditor::setupUnitTable()
{
  QSqlError sqlError;

  pvUnitModel->setTable("Unit_tbl");
  if(!pvUnitModel->select()){
    sqlError = pvUnitModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'Unit_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClTerminalBlockEditor");
    e.commit();
    return false;
  }
  pvUnitWidget->setModel(pvUnitModel);
  pvUnitWidget->mapFormWidgets();

  return true;
}

bool mdtClTerminalBlockEditor::setupTerminalEditTable()
{
  QSqlError sqlError;
  QPushButton *pbAddTerminal;
  QPushButton *pbRemoveTerminal;

  pvTerminalEditModel->setTable("UnitConnection_tbl");
  if(!pvTerminalEditModel->select()){
    sqlError = pvTerminalEditModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'UnitConnection_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClTerminalBlockEditor");
    e.commit();
    return false;
  }
  pvTerminalEditWidget->setModel(pvTerminalEditModel);
  // Add the Add and remove buttons
  pbAddTerminal = new QPushButton(tr("Add..."));
  pbRemoveTerminal = new QPushButton(tr("Remove"));
  connect(pbAddTerminal, SIGNAL(clicked()), this, SLOT(addTerminal()));
  connect(pbRemoveTerminal, SIGNAL(clicked()), this, SLOT(removeTerminal()));
  pvTerminalEditWidget->addWidgetToLocalBar(pbAddTerminal);
  pvTerminalEditWidget->addWidgetToLocalBar(pbRemoveTerminal);
  pvTerminalEditWidget->addStretchToLocalBar();
  ///pvTerminalEditWidget->enableLocalEdition();
  pvTerminalEditWidget->setEditionEnabled(false);
  // Hide relation fields and PK
  pvTerminalEditWidget->setColumnHidden("Id_PK", true);
  pvTerminalEditWidget->setColumnHidden("Unit_Id_FK", true);
  pvTerminalEditWidget->setColumnHidden("ArticleConnection_Id_FK", true);
  pvTerminalEditWidget->setColumnHidden("IsATestPoint", true);
  pvTerminalEditWidget->setColumnHidden("FunctionEN", true);
  pvTerminalEditWidget->setColumnHidden("SignalName", true);
  pvTerminalEditWidget->setColumnHidden("SwAddress", true);
  // Give fields a user friendly name
  /**
  pvTerminalEditWidget->setHeaderData("ConnectorName", tr("Connector"));
  pvTerminalEditWidget->setHeaderData("ContactName", tr("Contact"));
  pvTerminalEditWidget->setHeaderData("IoType", tr("I/O type"));
  pvTerminalEditWidget->setHeaderData("FunctionEN", tr("Function ENG"));
  */
  // Setup Unit <-> Connection relation
  pvTerminalEditRelation->setParentModel(pvUnitModel);
  pvTerminalEditRelation->setChildModel(pvTerminalEditModel);
  if(!pvTerminalEditRelation->addRelation("Id_PK", "Unit_Id_FK")){
    return false;
  }
  pvUnitWidget->addChildWidget(pvTerminalEditWidget, pvTerminalEditRelation);

  return true;
}

bool mdtClTerminalBlockEditor::setupTerminalLinkEditTable()
{
  QSqlError sqlError;

  pvTerminalLinkEditModel->setTable("TerminalBlockLink_view");
  if(!pvTerminalLinkEditModel->select()){
    sqlError = pvTerminalLinkEditModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'TerminalBlockLink_view'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClTerminalBlockEditor");
    e.commit();
    return false;
  }
  pvTerminalLinkEditWidget->setModel(pvTerminalLinkEditModel);
  pvTerminalLinkEditWidget->setEditionEnabled(false);
  // Hide relation fields and PK
  
  // Give fields a user friendly name
  
  // Setup Unit <-> Terminal links relation
  pvTerminalLinkEditRelation->setParentModel(pvUnitModel);
  pvTerminalLinkEditRelation->setChildModel(pvTerminalLinkEditModel);
  if(!pvTerminalLinkEditRelation->addRelation("Id_PK", "UnitStart_Id_PK")){
    return false;
  }
  if(!pvTerminalLinkEditRelation->addRelation("Id_PK", "UnitEnd_Id_PK")){
    return false;
  }
  pvUnitWidget->addChildWidget(pvTerminalLinkEditWidget, pvTerminalLinkEditRelation);

  return true;
}

bool mdtClTerminalBlockEditor::setupVehicleTable()
{
  QSqlError sqlError;
  QPushButton *pbAddVehicle;
  QPushButton *pbRemoveVehicle;

  pvVehicleTypeModel->setTable("Unit_VehicleType_view");
  if(!pvVehicleTypeModel->select()){
    sqlError = pvVehicleTypeModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'Unit_VehicleType_view'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClTerminalBlockEditor");
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

void mdtClTerminalBlockEditor::displayError(const QString &title, const QString &text, const QSqlError &error, bool log)
{
  QMessageBox msgBox;

  if(log){
    mdtError e(MDT_DATABASE_ERROR, text, mdtError::Error);
    if(error.isValid()){
      e.setSystemError(error.number(), error.text());
    }
    MDT_ERROR_SET_SRC(e, "mdtClTerminalBlockEditor");
    e.commit();
  }
  msgBox.setWindowTitle(title);
  msgBox.setText(text);
  if(error.isValid()){
    msgBox.setDetailedText(error.text());
  }
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.exec();
  
}
