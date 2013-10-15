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

#include "mdtClUnit.h"

#include "ui_mdtClUnitEditor.h"
#include "ui_mdtClUnitConnectionEditor.h"
#include "mdtSqlWindow.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtError.h"

#include "mdtSqlFormWindow.h"
#include "mdtClUnitVehicleType.h"

#include "mdtClUnitConnectionDialog.h"
#include "mdtClUnitConnectionData.h"

#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QStringList>
#include <QString>
#include <QList>
#include <QVariant>
///#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QWidget>
#include <QHBoxLayout>
#include <QDataWidgetMapper>

#include <QTableView>

#include <QDebug>

mdtClUnitEditor::mdtClUnitEditor(QObject *parent, QSqlDatabase db)
 : QObject(parent)
{
  pvDatabase = db;
  pvForm = new mdtSqlFormWindow;
  // Setup unit
  // Setup unit connection editor
  /**
  pvUnitConnectionEditWidget = new mdtSqlFormWidget;
  Ui::mdtClUnitConnectionEditor *uce = new Ui::mdtClUnitConnectionEditor;
  uce->setupUi(pvUnitConnectionEditWidget);
  pvUnitConnectionEditModel = new QSqlTableModel(this, pvDatabase);
  pvUnitConnectionEditRelation = new mdtSqlRelation;
  connect(pvUnitConnectionEditWidget, SIGNAL(submitEnabledStateChanged(bool)), uce->pbSave, SLOT(setEnabled(bool)));
  connect(uce->pbSave, SIGNAL(clicked()), pvUnitConnectionEditWidget, SLOT(submit()));
  uce->pbSave->setEnabled(false);
  connect(pvUnitConnectionEditWidget, SIGNAL(revertEnabledStateChanged(bool)), uce->pbCancel, SLOT(setEnabled(bool)));
  connect(uce->pbCancel, SIGNAL(clicked()), pvUnitConnectionEditWidget, SLOT(revert()));
  uce->pbCancel->setEnabled(false);
  */
}

mdtClUnitEditor::~mdtClUnitEditor()
{
  delete pvForm;
}

bool mdtClUnitEditor::setupTables(bool includeConnections)
{
  // Setup Unit table
  if(!setupUnitTable()){
    return false;
  }
  // Setup unit component view table
  if(!setupUnitComponentViewTable()){
    return false;
  }
  // Setup connection view table
  if(!setupUnitConnectionViewTable()){
    return false;
  }
  // Setup connection edit table
  if(!setupUnitConnectionEditTable()){
    return false;
  }
  // Setup VehicleType_Unit
  if(!setupVehicleTable()){
    return false;
  }
  // Setup Article
  if(!setupArticleTable()){
    return false;
  }

  return true;
}

mdtSqlFormWindow *mdtClUnitEditor::form()
{
  Q_ASSERT(pvForm != 0);

  return pvForm;
}

void mdtClUnitEditor::assignVehicle()
{
  mdtSqlSelectionDialog selectionDialog;
  int unitId;
  QSqlError sqlError;
  QModelIndexList selectedVehicles;
  mdtClUnitVehicleType uvt(pvDatabase);

  // Get current unit ID
  unitId = currentUnitId();
  if(unitId < 0){
    return;
  }
  // Setup and show dialog
  selectionDialog.setMessage("Please select a vehicle");
  selectionDialog.setModel(&uvt.vehicleTypeNotAssignedToUnitModel(unitId), true);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setHeaderData("SubType", tr("Variant"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  // Proceed insertion
  selectedVehicles = selectionDialog.selectionResults();
  if(!uvt.addUnitVehicleTypeAssignments(unitId, selectedVehicles)){
    sqlError = uvt.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Could not assign vehicle"));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  pvForm->select("Unit_VehicleType_view");
}

void mdtClUnitEditor::removeVehicleAssignation()
{
  int unitId;
  QSqlError sqlError;
  int ret;
  QMessageBox msgBox;
  mdtSqlTableWidget *vehicleTypeWidget;
  QModelIndexList indexes;
  mdtClUnitVehicleType uvt(pvDatabase);

  // Get vehicle widget
  vehicleTypeWidget = pvForm->sqlTableWidget("Unit_VehicleType_view");
  Q_ASSERT(vehicleTypeWidget != 0);
  Q_ASSERT(vehicleTypeWidget->selectionModel() != 0);
  // Get current unit ID
  unitId = currentUnitId();
  if(unitId < 0){
    return;
  }
  // Get selected rows
  indexes = vehicleTypeWidget->indexListOfSelectedRows("VehicleType_Id_FK");
  if(indexes.size() < 1){
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
  // Delete seleced rows
  if(!uvt.removeUnitVehicleAssignments(unitId, indexes)){
    sqlError = uvt.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Assignation removing failed."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  pvForm->select("Unit_VehicleType_view");
}

void mdtClUnitEditor::setBaseArticle()
{
  mdtClUnit unit(pvDatabase);
  mdtSqlSelectionDialog selectionDialog;
  QSqlQueryModel model;
  int ret;
  QString sql;
  QList<QVariant> selectedItem;

  // Check if some unit connections are related to article connections
  //  If yes, we cannot change the base article
  ret = unit.toUnitRelatedArticleConnectionCount(currentUnitId());
  if(ret < 0){
    QMessageBox msgBox;
    msgBox.setText(tr("Cannot check if current unit has connections based on current article."));
    msgBox.setDetailedText(unit.lastError().text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  if(ret > 0){
    QMessageBox msgBox;
    msgBox.setText(tr("Cannot change base article because some connections are based on it."));
    msgBox.setInformativeText(tr("Please remove used connections that are related to base article and try again."));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
    return;
  }
  // If a base article  is allready set, we let choose the user between unset it or choose a new one
  if(pvForm->currentData("Unit_tbl", "Article_Id_FK").toInt() > 0){
    QMessageBox msgBox;
    msgBox.setText(tr("A base article allready exist."));
    msgBox.setInformativeText(tr("Please choose a action."));
    msgBox.setIcon(QMessageBox::Information);
    QPushButton *pbUnsetBaseArticle = msgBox.addButton(tr("Unset"), QMessageBox::AcceptRole);
    QPushButton *pbSetBaseArticle = msgBox.addButton(tr("Set other ..."), QMessageBox::AcceptRole);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    msgBox.setWindowTitle(tr("Unit base article affectation"));
    msgBox.exec();
    if(msgBox.clickedButton() == pbUnsetBaseArticle){
      pvForm->setCurrentData("Unit_tbl", "Article_Id_FK", QVariant());
      return;
    }
    if(msgBox.clickedButton() != pbSetBaseArticle){
      return;
    }
  }
  // Setup article selection dialog and show it to user
  sql = "SELECT * FROM Article_tbl;";
  model.setQuery(sql, pvDatabase);
  selectionDialog.setMessage("Please select article");
  selectionDialog.setModel(&model);
  ///selectionDialog.setColumnHidden("Id_PK", true);
  ///selectionDialog.setHeaderData("SubType", tr("Variant"));
  ///selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  selectedItem = selectionDialog.selectionResult();
  if(selectedItem.size() != 1){
    return;
  }
  if(!pvForm->setCurrentData("Unit_tbl", "Article_Id_FK", selectedItem.at(0))){
    return;
  }
}

void mdtClUnitEditor::addComponent()
{
  mdtSqlSelectionDialog dialog;
  QSqlQueryModel *unitModel;
  mdtClUnit unit(pvDatabase);
  ///QModelIndex index;
  ///QVariant data;
  ///int row;

  // Setup and show dialog
  unitModel = unit.unitModelForComponentSelection(currentUnitId());
  Q_ASSERT(unitModel != 0);
  dialog.setMessage(tr("Please select a unit"));
  dialog.setModel(unitModel, false);
  /**
  dialog.setColumnHidden("Id_PK", true);
  dialog.setHeaderData("ArticleConnectorName", "Connector");
  dialog.setHeaderData("ArticleContactName", "Contact");
  dialog.setHeaderData("IoType", "I/O type");
  dialog.setHeaderData("FunctionEN", "Function");
  */
  dialog.addSelectionResultColumn("Id_PK");
  dialog.resize(600, 400);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Store result
  Q_ASSERT(dialog.selectionResult().size() == 1);
  // Add component
  if(!unit.addComponent(currentUnitId(), dialog.selectionResult().at(0))){
    QMessageBox msgBox;
    msgBox.setText(tr("Component insertion failed"));
    msgBox.setInformativeText(tr("Please see details for more informations"));
    msgBox.setDetailedText(unit.lastError().text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update component table
  pvForm->select("UnitComponent_view");
}

void mdtClUnitEditor::removeComponents()
{
  mdtSqlTableWidget *widget;
  mdtClUnit unit(pvDatabase);
  QMessageBox msgBox;
  QModelIndexList indexes;
  QSqlError sqlError;
  int ret;

  widget = pvForm->sqlTableWidget("UnitComponent_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  indexes = widget->indexListOfSelectedRows("UnitComponent_Id_PK");
  if(indexes.size() < 1){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove components from current unit."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  ret = msgBox.exec();
  if(ret != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!unit.removeComponents(indexes)){
    sqlError = unit.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Components removing failed."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update component table
  pvForm->select("UnitComponent_view");
}

void mdtClUnitEditor::addConnection()
{
  QSqlError sqlError;
  mdtClUnitConnectionDialog connectionDialog(0, pvDatabase);
  mdtSqlSelectionDialog selectionDialog;
  mdtClUnit unit(pvDatabase);
  QVariant unitId, articleIdFk;
  QSqlQueryModel *queryModel;
  QList<QVariant> selectedItem;
  mdtClUnitConnectionData data;
  bool createAfreeConnection = false;

  // We must have current unit ID and base article ID
  unitId = currentUnitId();
  articleIdFk = pvForm->currentData("Unit_tbl", "Article_Id_FK");
  // Get qeury model for article connection selection
  queryModel = unit.modelForArticleConnectionSelection(unitId, articleIdFk);
  Q_ASSERT(queryModel != 0);
  // If either unit is not based on a article, or no more article connection is available, question the user about create a new free connection
  if((articleIdFk.isNull())||(queryModel->rowCount() < 1)){
    QMessageBox msgBox;
    if(articleIdFk.isNull()){
      msgBox.setText(tr("Current unit is not based on a article."));
      msgBox.setInformativeText(tr("Do you want to create a free connection ?"));
      msgBox.setIcon(QMessageBox::Question);
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
      msgBox.setDefaultButton(QMessageBox::No);
      if(msgBox.exec() != QMessageBox::Yes){
        return;
      }
    }else{
      msgBox.setText(tr("There is no more connection available for base article."));
      msgBox.setInformativeText(tr("Do you want to create a free connection ?"));
      msgBox.setIcon(QMessageBox::Question);
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
      msgBox.setDefaultButton(QMessageBox::No);
      if(msgBox.exec() != QMessageBox::Yes){
        return;
      }
    }
    createAfreeConnection = true;
  }
  if(!createAfreeConnection){
    // Setup and show dialog for article connection selection
    selectionDialog.setMessage("Please select a article connection to use");
    selectionDialog.setModel(queryModel);
    selectionDialog.setColumnHidden("Id_PK", true);
    selectionDialog.setColumnHidden("Article_Id_FK", true);
    selectionDialog.setHeaderData("ArticleConnectorName", tr("Connector"));
    selectionDialog.setHeaderData("ArticleContactName", tr("Contact"));
    selectionDialog.setHeaderData("IoType", tr("I/O type"));
    selectionDialog.setHeaderData("FunctionEN", tr("Function ENG"));
    selectionDialog.addSelectionResultColumn("Id_PK");
    selectionDialog.addSelectionResultColumn("Article_Id_FK");
    selectionDialog.addSelectionResultColumn("ArticleConnectorName");
    selectionDialog.addSelectionResultColumn("ArticleContactName");
    selectionDialog.addSelectionResultColumn("IoType");
    selectionDialog.addSelectionResultColumn("FunctionEN");
    selectionDialog.setWindowTitle(tr("Article connection selection"));
    selectionDialog.resize(600, 300);
    if(selectionDialog.exec() != QDialog::Accepted){
      return;
    }
    selectedItem = selectionDialog.selectionResult();
    Q_ASSERT(selectedItem.size() == 6);
    qDebug() << "Selected: " << selectedItem;
    // Set article connection data
    data.setArticleConnectionId(selectedItem.at(0));
    data.setArticleConnectorName(selectedItem.at(2));
    data.setArticleContactName(selectedItem.at(3));
    data.setArticleIoType(selectedItem.at(4));
    data.setArticleFunctionEN(selectedItem.at(5));
  }
  // Setup and show unit connection edition dialog
  data.setUnitId(unitId);
  connectionDialog.setData(data);
  if(connectionDialog.exec() != QDialog::Accepted){
    return;
  }
  // Add connection
  if(!unit.addUnitConnection(connectionDialog.data())){
    sqlError = unit.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Connection adding failed."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update connections view
  pvForm->select("UnitConnection_view");
}

void mdtClUnitEditor::addFreeConnection()
{
  QSqlError sqlError;
  mdtClUnitConnectionDialog dialog(0, pvDatabase);
  mdtClUnit unit(pvDatabase);

  // Setup and show dialog
  dialog.setUnitId(currentUnitId());
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add connection
  if(!unit.addUnitConnection(dialog.data())){
    sqlError = unit.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Connection adding failed."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update connections view
  pvForm->select("UnitConnection_view");

  return;
  
  mdtSqlSelectionDialog selectionDialog;
  QSqlQueryModel model;
  QString sql;
  QList<QVariant> selectedItem;
  int articleIdFkColumn;
  int articleIdFk;
  int unitId;
  int articleConnectionId;
  QModelIndex index;
  QSqlTableModel *unitModel;

  // Find unitModel
  unitModel = pvForm->model("Unit_tbl");
  Q_ASSERT(unitModel != 0);
  // Get current unit ID
  unitId = currentUnitId();
  if(unitId < 0){
    return;
  }
  // Get column of Article FK
  articleIdFkColumn = unitModel->record().indexOf("Article_Id_FK");
  if(articleIdFkColumn < 0){
    return;
  }
  // Get current article ID
  Q_ASSERT(pvForm->sqlWidget("Unit_tbl") != 0);
  index = unitModel->index(pvForm->sqlWidget("Unit_tbl")->currentRow(), articleIdFkColumn);
  if(!index.isValid()){
    return;
  }
  articleIdFk = unitModel->data(index).toInt();
  
  // SQL query to get the list of article connections that are not allready used
  sql = "SELECT * "\
        "FROM ArticleConnection_tbl "\
        "WHERE Article_Id_FK = " + QString::number(articleIdFk) + " "\
        "AND Id_PK NOT IN ( "\
        " SELECT ArticleConnection_Id_FK "\
        " FROM UnitConnection_tbl "\
        " WHERE Unit_Id_FK = " + QString::number(unitId) + "); ";
  /**
        "WHERE Id_PK NOT IN ( "\
        " SELECT VehicleType_Id_FK "\
        " FROM VehicleType_Unit_tbl "\
        " WHERE Unit_Id_FK = " + QString::number(unitId) + " ) "\
        "ORDER BY Type ASC, SubType ASC, SeriesNumber ASC;";
        */
  model.setQuery(sql, pvDatabase);
  // Setup and show dialog
  selectionDialog.setMessage("Please select a connection");
  selectionDialog.setModel(&model);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_FK", true);
  selectionDialog.setHeaderData("ConnectorName", tr("Connector"));
  selectionDialog.setHeaderData("ContactName", tr("Contact"));
  selectionDialog.setHeaderData("IoType", tr("I/O type"));
  selectionDialog.setHeaderData("FunctionEN", tr("Function ENG"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.addSelectionResultColumn("ConnectorName");
  selectionDialog.addSelectionResultColumn("ContactName");
  selectionDialog.addSelectionResultColumn("FunctionEN");
  selectionDialog.resize(600, 300);
  selectionDialog.exec();
  selectedItem = selectionDialog.selectionResult();
  qDebug() << "Selected: " << selectedItem;
  if(selectedItem.size() != 4){
    return;
  }
  if(!selectedItem.at(0).isValid()){
    return;
  }
  articleConnectionId = selectedItem.at(0).toInt();
  if(articleConnectionId < 0){
    return;
  }
  // Insert selected article connection into UnitConnection_tbl
  QSqlQuery query(pvDatabase);
  sql = "INSERT INTO UnitConnection_tbl "\
        "(Unit_Id_FK, ArticleConnection_Id_FK, ConnectorName, ContactName, FunctionEN) "\
        "VALUES (:Unit_Id_FK, :ArticleConnection_Id_FK, :ConnectorName, :ContactName, :FunctionEN)";
        ///"VALUES ("\
          + QString::number(unitId) + ", " + QString::number(articleConnectionId)\
          + ", '" + selectedItem.at(1).toString() + "', '" + selectedItem.at(2).toString() + "', '" + selectedItem.at(3).toString() + "')";
  query.prepare(sql);
  query.bindValue(":Unit_Id_FK", unitId);
  query.bindValue(":ArticleConnection_Id_FK", articleConnectionId);
  query.bindValue(":ConnectorName", selectedItem.at(1));
  query.bindValue(":ContactName", selectedItem.at(2));
  query.bindValue(":FunctionEN", selectedItem.at(3));
  if(!query.exec()){
    sqlError = query.lastError();
    qDebug() << "ERR insert: " << sqlError;
    QMessageBox msgBox;
    msgBox.setText(tr("Could not add connection"));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  Q_ASSERT(pvForm->model("UnitConnection_tbl") != 0);
  pvForm->model("UnitConnection_tbl")->select();
}

void mdtClUnitEditor::editConnection()
{
  mdtSqlTableWidget *widget;
  QSqlError sqlError;
  mdtClUnitConnectionDialog dialog(0, pvDatabase);
  mdtClUnit unit(pvDatabase);
  mdtClUnitConnectionData data;

  widget = pvForm->sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != 0);

  // Get current item's data
  data = unit.getUnitConnectionData(widget->currentData("UnitConnection_Id_PK"));
  /**
  if(widget->currentData("UnitConnection_Id_PK").isNull()){
    return;
  }
  data.setId(widget->currentData("UnitConnection_Id_PK"));
  data.setArticleConnectionId(widget->currentData("ArticleConnection_Id_FK"));
  data.setArticleConnectorName(widget->currentData("ArticleConnectorName"));
  data.setArticleContactName(widget->currentData("ArticleContactName"));
  data.setArticleIoType(widget->currentData("IoType"));
  data.setArticleFunctionEN(widget->currentData("ArticleFunctionEN"));
  data.setUnitId(widget->currentData("Unit_Id_FK"));
  data.setSchemaPage(widget->currentData("SchemaPage"));
  data.setFunctionEN(widget->currentData("UnitFunctionEN"));
  data.setSignalName(widget->currentData("SignalName"));
  data.setSwAddress(widget->currentData("SwAddress"));
  data.setUnitConnectorName(widget->currentData("UnitConnectorName"));
  data.setUnitContactName(widget->currentData("UnitContactName"));
  */
  // Setup and show dialog
  dialog.setData(data);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Edit connection
  if(!unit.editUnitConnection(dialog.data())){
    sqlError = unit.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Connection edition failed."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update connections view
  pvForm->select("UnitConnection_view");
}

void mdtClUnitEditor::removeConnections()
{
  return;
  
  Q_ASSERT(pvForm->sqlTableWidget("UnitConnection_view") != 0);
  Q_ASSERT(pvForm->sqlTableWidget("UnitConnection_view")->selectionModel() != 0);

  int i;
  int ret;
  int row;
  int unitConnectionIdColumn;
  QVariant id;
  QList<int> rows;
  QList<int> unitConnectionIds;
  QMessageBox msgBox;
  QModelIndex index;
  QModelIndexList indexes = pvForm->sqlTableWidget("UnitConnection_view")->selectionModel()->selectedIndexes();
  QSqlTableModel *unitConnectionViewModel;

  // Find unitConnectionViewModel
  unitConnectionViewModel = pvForm->model("UnitConnection_view");
  Q_ASSERT(unitConnectionViewModel != 0);

  // If nothing was selected, we do nothing
  if(indexes.size() < 1){
    return;
  }
  // Get column of unit connection ID (PK) in view
  unitConnectionIdColumn = unitConnectionViewModel->record().indexOf("UnitConnection_Id_PK");
  if(unitConnectionIdColumn < 0){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to delete selected connections for current unit."));
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
  // Build list of unit connections to remove
  for(i = 0; i < rows.size(); ++i){
    index = unitConnectionViewModel->index(rows.at(i), unitConnectionIdColumn);
    if(index.isValid()){
      id = unitConnectionViewModel->data(index);
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
      MDT_ERROR_SET_SRC(e, "mdtClUnitEditor");
      e.commit();
      QMessageBox msgBox;
      msgBox.setText(tr("Error occured while removing connections."));
      msgBox.setInformativeText(tr("Please see details."));
      msgBox.setDetailedText(sqlError.text());
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      return;
    }
  }
  // Refresh connections view
  unitConnectionViewModel->select();
}

int mdtClUnitEditor::currentUnitId()
{
  QVariant var;

  var = pvForm->currentData("Unit_tbl", "Id_PK");
  if(!var.isValid()){
    return -1;
  }

  return var.toInt();
}

bool mdtClUnitEditor::setupUnitTable()
{
  Ui::mdtClUnitEditor ue;
  QSqlTableModel *unitModel;
  QDataWidgetMapper *baseArticleMapper;
  QSqlTableModel *baseArticleModel;
  mdtSqlRelation *baseArticleRelation;

  // Setup main form widget
  ue.setupUi(pvForm->mainSqlWidget());
  connect(ue.pbSetBaseArticle, SIGNAL(clicked()), this, SLOT(setBaseArticle()));
  connect(this, SIGNAL(unitEdited()), pvForm->mainSqlWidget(), SIGNAL(dataEdited()));
  // Setup form
  if(!pvForm->setTable("Unit_tbl", "Units", pvDatabase)){
    return false;
  }
  pvForm->sqlWindow()->enableNavigation();
  pvForm->sqlWindow()->enableEdition();
  pvForm->sqlWindow()->resize(800, 500);
  pvForm->sqlWindow()->setWindowTitle(tr("Unit edition"));
  /*
   * Setup base article widget mapping
   */
  unitModel = pvForm->model("Unit_tbl");
  Q_ASSERT(unitModel != 0);
  // Setup base article model
  baseArticleModel = new QSqlTableModel(this, pvDatabase);
  baseArticleModel->setTable("Article_tbl");
  if(!baseArticleModel->select()){
    return false;
  }
  // Setup base article widget mapper
  baseArticleMapper = new QDataWidgetMapper(this);
  baseArticleMapper->setModel(baseArticleModel);
  baseArticleMapper->addMapping(ue.leArticle_Id_FK, baseArticleModel->fieldIndex("Id_PK"));
  baseArticleMapper->addMapping(ue.leArticleCode, baseArticleModel->fieldIndex("ArticleCode"));
  baseArticleMapper->addMapping(ue.leArticleDesignationEN, baseArticleModel->fieldIndex("DesignationEN"));
  // Setup base article relation
  baseArticleRelation = new mdtSqlRelation(this);
  baseArticleRelation->setParentModel(unitModel);
  baseArticleRelation->setChildModel(baseArticleModel);
  if(!baseArticleRelation->addRelation("Article_Id_FK", "Id_PK", false)){
    return false;
  }
  connect(pvForm->mainSqlWidget(), SIGNAL(currentRowChanged(int)), baseArticleRelation, SLOT(setParentCurrentIndex(int)));
  connect(baseArticleRelation, SIGNAL(childModelFilterApplied()), baseArticleMapper, SLOT(toFirst()));
  connect(baseArticleRelation, SIGNAL(childModelIsEmpty()), baseArticleMapper, SLOT(revert()));
  // Force a update
  pvForm->mainSqlWidget()->setCurrentIndex(pvForm->mainSqlWidget()->currentRow());

  return true;
}

bool mdtClUnitEditor::setupUnitComponentViewTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddComponent;
  QPushButton *pbRemoveComponents;

  if(!pvForm->addChildTable("UnitComponent_view", tr("Components"), pvDatabase)){
    return false;
  }
  if(!pvForm->addRelation("Id_PK", "UnitComponent_view", "Unit_Id_PK")){
    return false;
  }
  widget = pvForm->sqlTableWidget("UnitComponent_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  ///widget->setColumnHidden("", true);
  ///widget->setColumnHidden("", true);
  // Set fields a user friendly name
  /**
  widget->setHeaderData("", tr(""));
  widget->setHeaderData("", tr(""));
  widget->setHeaderData("", tr(""));
  widget->setHeaderData("", tr(""));
  */
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add edition buttons
  pbAddComponent = new QPushButton(tr("Add component ..."));
  connect(pbAddComponent, SIGNAL(clicked()), this, SLOT(addComponent()));
  widget->addWidgetToLocalBar(pbAddComponent);
  pbRemoveComponents = new QPushButton(tr("Remove components"));
  connect(pbRemoveComponents, SIGNAL(clicked()), this, SLOT(removeComponents()));
  widget->addWidgetToLocalBar(pbRemoveComponents);
  widget->addStretchToLocalBar();

  return true;
}

bool mdtClUnitEditor::setupUnitConnectionViewTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddConnection;
  QPushButton *pbAddFreeConnection;
  QPushButton *pbEditConnection;
  QPushButton *pbRemoveConnection;

  if(!pvForm->addChildTable("UnitConnection_view", tr("Connections"), pvDatabase)){
    return false;
  }
  if(!pvForm->addRelation("Id_PK", "UnitConnection_view", "Unit_Id_FK")){
    return false;
  }
  widget = pvForm->sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != 0);
  // Add the Add and remove buttons
  pbAddConnection = new QPushButton(tr("Add connection ..."));
  pbAddFreeConnection = new QPushButton(tr("Add free connection ..."));
  pbEditConnection = new QPushButton(tr("Edit connection ..."));
  pbRemoveConnection = new QPushButton(tr("Remove connections"));
  connect(pbAddConnection, SIGNAL(clicked()), this, SLOT(addConnection()));
  connect(pbAddFreeConnection, SIGNAL(clicked()), this, SLOT(addFreeConnection()));
  connect(pbEditConnection, SIGNAL(clicked()), this, SLOT(editConnection()));
  connect(pbRemoveConnection, SIGNAL(clicked()), this, SLOT(removeConnections()));
  widget->addWidgetToLocalBar(pbAddConnection);
  widget->addWidgetToLocalBar(pbAddFreeConnection);
  widget->addWidgetToLocalBar(pbEditConnection);
  widget->addWidgetToLocalBar(pbRemoveConnection);
  widget->addStretchToLocalBar();
  ///pvUnitConnectionViewWidget->enableLocalEdition();
  ///pvUnitConnectionViewWidget->setEditionEnabled(false);
  // Hide relation fields and PK
  /**
  pvUnitConnectionViewWidget->setColumnHidden("UnitConnection_Id_PK", true);
  pvUnitConnectionViewWidget->setColumnHidden("Article_Id_FK", true);
  pvUnitConnectionViewWidget->setColumnHidden("Unit_Id_FK", true);
  pvUnitConnectionViewWidget->setColumnHidden("ArticleConnection_Id_FK", true);
  */
  // Give fields a user friendly name
  /**
  pvUnitConnectionViewWidget->setHeaderData("ConnectorName", tr("Connector"));
  pvUnitConnectionViewWidget->setHeaderData("ContactName", tr("Contact"));
  pvUnitConnectionViewWidget->setHeaderData("IoType", tr("I/O type"));
  pvUnitConnectionViewWidget->setHeaderData("FunctionEN", tr("Function ENG"));
  */

  return true;
}

bool mdtClUnitEditor::setupUnitConnectionEditTable()
{
  // Setup relation
  ///pvUnitConnectionEditRelation->setParentModel(pvUnitConnectionViewModel);
  ///pvUnitConnectionEditRelation->setChildModel(pvUnitConnectionEditModel);
  /**
  if(!pvUnitConnectionEditRelation->addRelation("ArticleConnection_Id_PK", "ArticleConnection_Id_FK")){
    return false;
  }
  if(!pvUnitConnectionEditRelation->addRelation("Unit_Id_FK", "Unit_Id_FK")){
    return false;
  }
  */
  ///connect(pvUnitConnectionViewWidget, SIGNAL(currentRowChanged(int)), pvUnitConnectionEditRelation, SLOT(setParentCurrentIndex(int)));
  ///pvUnitConnectionViewWidget->addChildWidget(pvUnitConnectionEditWidget, pvUnitConnectionEditRelation);
  // Force a update of relations
  ///pvUnitConnectionEditRelation->setParentCurrentIndex(pvUnitConnectionViewWidget->currentRow());

  /// \todo provisoire
  /**
  mdtSqlWindow *w = new mdtSqlWindow;
  w->setSqlWidget(pvUnitConnectionEditWidget);
  w->enableNavigation();
  w->enableEdition();
  w->show();
  */

  return true;
}

bool mdtClUnitEditor::setupVehicleTable()
{
  QPushButton *pbAddVehicle;
  QPushButton *pbRemoveVehicle;
  mdtSqlTableWidget *widget;

  // Add vehicle type table
  if(!pvForm->addChildTable("Unit_VehicleType_view", tr("Vehicles"), pvDatabase)){
    return false;
  }
  // Setup Unit <-> VehicleType relation
  if(!pvForm->addRelation("Id_PK", "Unit_VehicleType_view", "Unit_Id_FK")){
    return false;
  }
  // Get widget to continue setup
  widget = pvForm->sqlTableWidget("Unit_VehicleType_view");
  Q_ASSERT(widget != 0);
  // Hide relation fields and PK
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("Unit_Id_FK", true);
  widget->setColumnHidden("VehicleType_Id_FK", true);
  // Give fields a user friendly name
  widget->setHeaderData("SubType", tr("Variant"));
  widget->setHeaderData("SeriesNumber", tr("Serie"));
  // Add vehicle button
  pbAddVehicle = new QPushButton(tr("Assign vehicle"));
  widget->addWidgetToLocalBar(pbAddVehicle);
  connect(pbAddVehicle, SIGNAL(clicked()), this, SLOT(assignVehicle()));
  // Remove vehicle button
  pbRemoveVehicle = new QPushButton(tr("Remove vehicle"));
  widget->addWidgetToLocalBar(pbRemoveVehicle);
  connect(pbRemoveVehicle, SIGNAL(clicked()), this, SLOT(removeVehicleAssignation()));
  widget->addStretchToLocalBar();

  return true;
}

bool mdtClUnitEditor::setupArticleTable()
{
  ///QPushButton *pbSetBaseArticle;

  /*
   * Setup Unit <-> Article relation
   * Here we make it reversed,
   *  because the parent table is the Article table,
   *  but we need to display the article on witch
   *  current Unit is based
   */
  /**
  pvArticleRelation->setChildModel(pvArticleModel);
  if(!pvArticleRelation->addRelation("Article_Id_FK", "Id_PK")){
    return false;
  }
  */
  ///pvUnitWidget->addChildWidget(pvArticleWidget, pvArticleRelation);
  // Add base article assignation button
  /**
  pbSetBaseArticle = new QPushButton(tr("Set base article"));
  pvArticleWidget->addWidgetToLocalBar(pbSetBaseArticle);
  pvArticleWidget->addStretchToLocalBar();
  connect(pbSetBaseArticle, SIGNAL(clicked()), this, SLOT(setBaseArticle()));
  */

  return true;
}
