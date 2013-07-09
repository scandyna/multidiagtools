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
#include "ui_mdtClUnitConnectionEditor.h"
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
#include <QWidget>
#include <QHBoxLayout>

#include <QDebug>

mdtClUnitEditor::mdtClUnitEditor(QObject *parent, const QSqlDatabase db)
 : QObject(parent)
{
  pvDatabase = db;
  // Setup unit
  pvUnitWidget = new mdtSqlFormWidget;
  Ui::mdtClUnitEditor *ue = new Ui::mdtClUnitEditor;
  ue->setupUi(pvUnitWidget);
  pvUnitModel = new QSqlTableModel(this, pvDatabase);
  connect(ue->pbSetBaseArticle, SIGNAL(clicked()), this, SLOT(setBaseArticle()));
  connect(this, SIGNAL(unitEdited()), pvUnitWidget, SIGNAL(dataEdited()));
  // Setup unit connection view
  pvUnitConnectionViewWidget = new mdtSqlTableWidget;
  pvUnitConnectionViewModel = new QSqlTableModel(this, pvDatabase);
  pvUnitConnectionViewRelation = new mdtSqlRelation;
  // Setup unit connection editor
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
  // Setup VehicleType_Unit
  pvVehicleTypeWidget = new mdtSqlTableWidget;
  pvVehicleTypeModel = new QSqlTableModel(this, pvDatabase);
  pvVehicleTypeRelation = new mdtSqlRelation;
  // Setup base article
  pvArticleWidget = new mdtSqlTableWidget;
  pvArticleModel = new QSqlTableModel(this, pvDatabase);
  pvArticleRelation = new mdtSqlRelation;
}

mdtClUnitEditor::~mdtClUnitEditor()
{
  ///delete pvUnitWidget;
  ///delete pvUnitConnectionViewWidget;
  delete pvUnitConnectionViewRelation;
  delete pvVehicleTypeRelation;
  delete pvArticleRelation;
  ///delete pvUnitModel;
  ///delete pvUnitConnectionViewModel;
  ///delete pvVehicleTypeModel;
}

bool mdtClUnitEditor::setupTables(bool includeConnections)
{
  QSqlError sqlError;

  // Setup Unit table
  if(!setupUnitTable()){
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

void mdtClUnitEditor::setupUi(mdtSqlWindow *window)
{
  Q_ASSERT(window != 0);

  QWidget *unitConnectionEditionWidget;
  QHBoxLayout *unitConnectionEditionLayout;

  window->setSqlWidget(pvUnitWidget);
  ///window->addChildWidget(pvUnitConnectionViewWidget, "Connections");
  window->addChildWidget(pvVehicleTypeWidget, tr("Vehicles"));
  window->addChildWidget(pvArticleWidget, tr("Base article"));
  // setup and add unit connect edition widget
  unitConnectionEditionWidget = new QWidget;
  unitConnectionEditionLayout = new QHBoxLayout;
  unitConnectionEditionLayout->addWidget(pvUnitConnectionViewWidget);
  unitConnectionEditionLayout->addWidget(pvUnitConnectionEditWidget);
  unitConnectionEditionWidget->setLayout(unitConnectionEditionLayout);
  window->addChildWidget(unitConnectionEditionWidget, tr("Connections"));

  window->enableNavigation();
  window->enableEdition();
  window->resize(900, 500);
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

void mdtClUnitEditor::setBaseArticle()
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlQueryModel model;
  QString sql;
  QList<QVariant> selectedItem;
  QSqlError sqlError;
  int articleIdFkColumn;
  int currentRow;
  QModelIndex index;

  // Check that no connections exists
  if(pvUnitConnectionViewModel->rowCount() > 0){
    QMessageBox msgBox;
    msgBox.setText(tr("Cannot change base article."));
    msgBox.setInformativeText(tr("Please remove used connections and try again."));
    msgBox.setDetailedText(tr("Unit connections are based on article connection.") + \
                           tr("If you need to select another base article, you must re-affect connecions") );
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
    return;
  }
  // Remeber current row
  currentRow = pvUnitWidget->currentRow();
  if(currentRow < 0){
    return;
  }
  // Get column of Article FK
  articleIdFkColumn = pvUnitModel->record().indexOf("Article_Id_FK");
  if(articleIdFkColumn < 0){
    return;
  }
  // Set SQL query
  sql = "SELECT * FROM Article_tbl;";
  model.setQuery(sql, pvDatabase);
  // Setup and show dialog
  selectionDialog.setMessage("Please select a article");
  selectionDialog.setModel(&model);
  ///selectionDialog.setColumnHidden("Id_PK", true);
  ///selectionDialog.setHeaderData("SubType", tr("Variant"));
  ///selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
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
  index = pvUnitModel->index(currentRow, articleIdFkColumn);
  Q_ASSERT(index.isValid());
  if(!pvUnitModel->setData(index, selectedItem.at(0))){
    qDebug() << "ERR set data ...";
    return;
  }
  emit unitEdited();
  ///emit pvUnitWidget->dataEdited();
  /**
  if(!pvUnitModel->submitAll()){
    qDebug() << "ERR set submitAll ...";
    return;
  }
  // Return to current row
  pvUnitWidget->setCurrentIndex(currentRow);
  */
}

void mdtClUnitEditor::addConnection()
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlQueryModel model;
  QString sql;
  QList<QVariant> selectedItem;
  int articleIdFkColumn;
  int articleIdFk;
  int unitId;
  int articleConnectionId;
  QSqlError sqlError;
  QModelIndex index;

  // Get current unit ID
  unitId = currentUnitId();
  if(unitId < 0){
    return;
  }
  // Get column of Article FK
  articleIdFkColumn = pvUnitModel->record().indexOf("Article_Id_FK");
  if(articleIdFkColumn < 0){
    return;
  }
  // Get current article ID
  index = pvUnitModel->index(pvUnitWidget->currentRow(), articleIdFkColumn);
  if(!index.isValid()){
    return;
  }
  articleIdFk = pvUnitModel->data(index).toInt();
  
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
        "VALUES ("\
          + QString::number(unitId) + ", " + QString::number(articleConnectionId)\
          + ", '" + selectedItem.at(1).toString() + "', '" + selectedItem.at(2).toString() + "', '" + selectedItem.at(3).toString() + "')";
  if(!query.exec(sql)){
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
  pvUnitConnectionViewModel->select();
}

void mdtClUnitEditor::removeConnection()
{
  Q_ASSERT(pvUnitConnectionViewWidget->selectionModel() != 0);

  int i;
  int ret;
  int row;
  int unitConnectionIdColumn;
  QVariant id;
  QList<int> rows;
  QList<int> unitConnectionIds;
  QMessageBox msgBox;
  QModelIndex index;
  QModelIndexList indexes = pvUnitConnectionViewWidget->selectionModel()->selectedIndexes();

  // If nothing was selected, we do nothing
  if(indexes.size() < 1){
    return;
  }
  // Get column of unit connecion ID (PK) in view
  unitConnectionIdColumn = pvUnitConnectionViewModel->record().indexOf("UnitConnection_Id_PK");
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
    index = pvUnitConnectionViewModel->index(rows.at(i), unitConnectionIdColumn);
    if(index.isValid()){
      id = pvUnitConnectionViewModel->data(index);
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
  pvUnitConnectionViewModel->select();
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

bool mdtClUnitEditor::setupUnitConnectionViewTable()
{
  QSqlError sqlError;
  QPushButton *pbAddConnection;
  QPushButton *pbRemoveConnection;

  pvUnitConnectionViewModel->setTable("UnitConnection_view");
  if(!pvUnitConnectionViewModel->select()){
    sqlError = pvUnitConnectionViewModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'UnitConnection_view'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnitEditor");
    e.commit();
    return false;
  }
  pvUnitConnectionViewWidget->setModel(pvUnitConnectionViewModel);
  // Add the Add and remove buttons
  pbAddConnection = new QPushButton(tr("Add"));
  pbRemoveConnection = new QPushButton(tr("Remove"));
  connect(pbAddConnection, SIGNAL(clicked()), this, SLOT(addConnection()));
  connect(pbRemoveConnection, SIGNAL(clicked()), this, SLOT(removeConnection()));
  pvUnitConnectionViewWidget->addWidgetToLocalBar(pbAddConnection);
  pvUnitConnectionViewWidget->addWidgetToLocalBar(pbRemoveConnection);
  pvUnitConnectionViewWidget->addStretchToLocalBar();
  ///pvUnitConnectionViewWidget->enableLocalEdition();
  pvUnitConnectionViewWidget->setEditionEnabled(false);
  // Hide relation fields and PK
  pvUnitConnectionViewWidget->setColumnHidden("UnitConnection_Id_PK", true);
  pvUnitConnectionViewWidget->setColumnHidden("Article_Id_FK", true);
  pvUnitConnectionViewWidget->setColumnHidden("Unit_Id_FK", true);
  pvUnitConnectionViewWidget->setColumnHidden("ArticleConnection_Id_FK", true);
  // Give fields a user friendly name
  pvUnitConnectionViewWidget->setHeaderData("ConnectorName", tr("Connector"));
  pvUnitConnectionViewWidget->setHeaderData("ContactName", tr("Contact"));
  pvUnitConnectionViewWidget->setHeaderData("IoType", tr("I/O type"));
  pvUnitConnectionViewWidget->setHeaderData("FunctionEN", tr("Function ENG"));
  // Setup Unit <-> Connection relation
  pvUnitConnectionViewRelation->setParentModel(pvUnitModel);
  pvUnitConnectionViewRelation->setChildModel(pvUnitConnectionViewModel);
  if(!pvUnitConnectionViewRelation->addRelation("Id_PK", "Unit_Id_FK")){
    return false;
  }
  pvUnitWidget->addChildWidget(pvUnitConnectionViewWidget, pvUnitConnectionViewRelation);

  return true;
}

bool mdtClUnitEditor::setupUnitConnectionEditTable()
{
  QSqlError sqlError;

  pvUnitConnectionEditModel->setTable("UnitConnection_tbl");
  if(!pvUnitConnectionEditModel->select()){
    sqlError = pvUnitConnectionEditModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'UnitConnection_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnitEditor");
    e.commit();
    return false;
  }
  pvUnitConnectionEditWidget->setModel(pvUnitConnectionEditModel);
  pvUnitConnectionEditWidget->mapFormWidgets();
  // Setup relation
  pvUnitConnectionEditRelation->setParentModel(pvUnitConnectionViewModel);
  pvUnitConnectionEditRelation->setChildModel(pvUnitConnectionEditModel);
  if(!pvUnitConnectionEditRelation->addRelation("ArticleConnection_Id_FK", "ArticleConnection_Id_FK")){
    return false;
  }
  if(!pvUnitConnectionEditRelation->addRelation("Unit_Id_FK", "Unit_Id_FK")){
    return false;
  }
  ///connect(pvUnitConnectionViewWidget, SIGNAL(currentRowChanged(int)), pvUnitConnectionEditRelation, SLOT(setParentCurrentIndex(int)));
  pvUnitConnectionViewWidget->addChildWidget(pvUnitConnectionEditWidget, pvUnitConnectionEditRelation);
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

bool mdtClUnitEditor::setupArticleTable()
{
  QSqlError sqlError;
  ///QPushButton *pbSetBaseArticle;

  pvArticleModel->setTable("Article_tbl");
  if(!pvArticleModel->select()){
    sqlError = pvArticleModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'Article_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnitEditor");
    e.commit();
    return false;
  }
  pvArticleWidget->setEditionEnabled(false);
  pvArticleWidget->setModel(pvArticleModel);
  /*
   * Setup Unit <-> Article relation
   * Here we make it reversed,
   *  because the parent table is the Article table,
   *  but we need to display the article on witch
   *  current Uint is based
   */
  pvArticleRelation->setParentModel(pvUnitModel);
  pvArticleRelation->setChildModel(pvArticleModel);
  if(!pvArticleRelation->addRelation("Article_Id_FK", "Id_PK")){
    return false;
  }
  pvUnitWidget->addChildWidget(pvArticleWidget, pvArticleRelation);
  // Add base article assignation button
  /**
  pbSetBaseArticle = new QPushButton(tr("Set base article"));
  pvArticleWidget->addWidgetToLocalBar(pbSetBaseArticle);
  pvArticleWidget->addStretchToLocalBar();
  connect(pbSetBaseArticle, SIGNAL(clicked()), this, SLOT(setBaseArticle()));
  */

  return true;
}
