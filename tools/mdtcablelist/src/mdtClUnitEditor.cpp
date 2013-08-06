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

#include "mdtSqlFormWindow.h"
#include "mdtClUnitVehicleType.h"

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
  mdtSqlSelectionDialog selectionDialog;
  QSqlQueryModel model;
  QString sql;
  QList<QVariant> selectedItem;
  QSqlError sqlError;
  int articleIdFkColumn;
  int currentRow;
  QModelIndex index;
  QSqlTableModel *unitModel;
  QSqlTableModel *unitConnectionModel;

  // Find unitModel
  unitModel = pvForm->model("Unit_tbl");
  Q_ASSERT(unitModel != 0);
  // Find unitConnectionModel
  unitConnectionModel = pvForm->model("UnitConnection_tbl");
  Q_ASSERT(unitConnectionModel != 0);

  // Check that no connections exists
  if(unitConnectionModel->rowCount() > 0){
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
  currentRow = pvForm->mainSqlWidget()->currentRow();
  if(currentRow < 0){
    return;
  }
  // Get column of Article FK
  articleIdFkColumn = unitModel->record().indexOf("Article_Id_FK");
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
  index = unitModel->index(currentRow, articleIdFkColumn);
  Q_ASSERT(index.isValid());
  if(!unitModel->setData(index, selectedItem.at(0))){
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

void mdtClUnitEditor::removeConnection()
{
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
  // Get column of unit connecion ID (PK) in view
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
  Ui::mdtClUnitEditor *ue;

  // Setup main form widget
  ue = new Ui::mdtClUnitEditor;
  ue->setupUi(pvForm->mainSqlWidget());
  connect(ue->pbSetBaseArticle, SIGNAL(clicked()), this, SLOT(setBaseArticle()));
  connect(this, SIGNAL(unitEdited()), pvForm->mainSqlWidget(), SIGNAL(dataEdited()));
  // Setup form
  if(!pvForm->setTable("Unit_tbl", "Units", pvDatabase)){
    return false;
  }
  pvForm->sqlWindow()->enableNavigation();
  pvForm->sqlWindow()->enableEdition();
  pvForm->sqlWindow()->resize(800, 500);
  pvForm->sqlWindow()->setWindowTitle(tr("Unit edition"));

  return true;
}

bool mdtClUnitEditor::setupUnitConnectionViewTable()
{
  QPushButton *pbAddConnection;
  QPushButton *pbRemoveConnection;

  if(!pvForm->addChildTable("UnitConnection_view", tr("Connections"), pvDatabase)){
    return false;
  }
  if(!pvForm->addRelation("Id_PK", "UnitConnection_view", "Unit_Id_FK")){
    return false;
  }
  // Add the Add and remove buttons
  pbAddConnection = new QPushButton(tr("Add"));
  pbRemoveConnection = new QPushButton(tr("Remove"));
  connect(pbAddConnection, SIGNAL(clicked()), this, SLOT(addConnection()));
  connect(pbRemoveConnection, SIGNAL(clicked()), this, SLOT(removeConnection()));
  /**
  pvUnitConnectionViewWidget->addWidgetToLocalBar(pbAddConnection);
  pvUnitConnectionViewWidget->addWidgetToLocalBar(pbRemoveConnection);
  pvUnitConnectionViewWidget->addStretchToLocalBar();
  */
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
