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
#include "mdtClLinkData.h"
#include "mdtClUnitLinkDialog.h"
#include "mdtClPathGraphDialog.h"
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QStringList>
#include <QString>
#include <QList>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QWidget>
#include <QDataWidgetMapper>
#include <QTableView>

#include <QDebug>

mdtClUnitEditor::mdtClUnitEditor(QObject *parent, QSqlDatabase db)
 : QObject(parent)
{
  pvDatabase = db;
  pvForm = new mdtSqlFormWindow;
}

mdtClUnitEditor::~mdtClUnitEditor()
{
  delete pvForm;
}

bool mdtClUnitEditor::setupTables()
{
  // Setup Unit table
  if(!setupUnitTable()){
    return false;
  }
  // Setup unit component table
  if(!setupUnitComponentTable()){
    return false;
  }
  // Setup connection table
  if(!setupUnitConnectionTable()){
    return false;
  }
  // Setup unit link table
  if(!setupUnitLinkTable()){
    return false;
  }
  // Setup VehicleType_Unit
  if(!setupVehicleTable()){
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
  QVariant unitId;
  QSqlError sqlError;
  QModelIndexList selectedVehicles;
  mdtClUnitVehicleType uvt(pvDatabase);

  // Get current unit ID
  unitId = currentUnitId();
  if(unitId.isNull()){
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
  QVariant unitId;
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
  if(unitId.isNull()){
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

  if(currentUnitId().isNull()){
    return;
  }
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
  if(unitId.isNull()){
    return;
  }
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

  if(currentUnitId().isNull()){
    return;
  }
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
  if(!data.isValid()){
    return;
  }
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
  mdtSqlTableWidget *widget;
  mdtClUnit unit(pvDatabase);
  QMessageBox msgBox;
  QModelIndexList indexes;
  QSqlError sqlError;
  QString linksMsg;

  widget = pvForm->sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  indexes = widget->indexListOfSelectedRows("UnitConnection_Id_PK");
  if(indexes.size() < 1){
    return;
  }
  // Check that selected connections are not related to some links
  linksMsg = unit.toUnitRelatedLinksListStr(currentUnitId(), indexes);
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
  if(!unit.removeUnitConnections(indexes)){
    sqlError = unit.lastError();
    ///QMessageBox msgBox;
    msgBox.setText(tr("Connections removing failed."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update connections table
  pvForm->select("UnitConnection_view");
}

void mdtClUnitEditor::addLink()
{
  mdtSqlFormWidget *widget;
  mdtClUnitLinkDialog dialog(0, pvDatabase);
  QVariant unitId;
  mdtClUnit unit(pvDatabase);

  widget = pvForm->sqlFormWidget("Unit_tbl");
  Q_ASSERT(widget != 0);
  // Setup and show dialog
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  dialog.setStartUnit(unitId, widget->currentData("SchemaPosition"), widget->currentData("Cabinet"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  if(!dialog.linkData().buildVehicleTypeStartEndIdList()){
    /// \todo MsgBox with error
    qDebug() << "ERROR building VJC list";
    return;
  }
  qDebug() << "Editor, vhcs: " << dialog.linkData().vehicleTypeStartEndIdList();
  // Add link
  if(!unit.addLink(dialog.linkData())){
    QSqlError sqlError = unit.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Link adding failed.           "));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update links view
  pvForm->select("UnitLink_view");
}

void mdtClUnitEditor::editLink()
{
  mdtSqlFormWidget *unitWidget;
  mdtSqlTableWidget *linkWidget;
  mdtClUnitLinkDialog dialog(0, pvDatabase);
  QVariant unitId, startConnectionId, endConnectionId;
  mdtClUnitConnectionData startConnectionData;
  mdtClUnitConnectionData endConnectionData;
  mdtClLinkData linkData;
  mdtClUnit unit(pvDatabase);

  unitWidget = pvForm->sqlFormWidget("Unit_tbl");
  Q_ASSERT(unitWidget != 0);
  linkWidget = pvForm->sqlTableWidget("UnitLink_view");
  Q_ASSERT(linkWidget != 0);
  // Get connection IDs
  startConnectionId = linkWidget->currentData("UnitConnectionStart_Id_FK");
  endConnectionId = linkWidget->currentData("UnitConnectionEnd_Id_FK");
  if(startConnectionId.isNull() || endConnectionId.isNull()){
    QMessageBox msgBox;
    msgBox.setText(tr("Please select a link."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
    return;
  }
  // Get current link data
  startConnectionData = unit.getUnitConnectionData(startConnectionId);
  if(!startConnectionData.isValid()){
    QSqlError sqlError = unit.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Cannot get data for start connection."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  endConnectionData = unit.getUnitConnectionData(endConnectionId);
  if(!endConnectionData.isValid()){
    QSqlError sqlError = unit.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Cannot get data for end connection."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  linkData = unit.getUnitLinkData(startConnectionId, endConnectionId);
  if(!linkData.isValid()){
    QSqlError sqlError = unit.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Cannot get data for link."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Setup and show dialog
  unitId = currentUnitId();
  dialog.setStartUnit(unitId, unitWidget->currentData("SchemaPosition"), unitWidget->currentData("Cabinet"));
  dialog.setStartConnectionData(startConnectionData);
  dialog.setEndUnit(unitId, unitWidget->currentData("SchemaPosition"), unitWidget->currentData("Cabinet"));
  dialog.setEndConnectionData(endConnectionData);
  dialog.setLinkData(linkData);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  if(!dialog.linkData().buildVehicleTypeStartEndIdList()){
    /// \todo MsgBox with error
    qDebug() << "ERROR building VJC list";
    return;
  }
  // Add link
  if(!unit.editLink(startConnectionId, endConnectionId, dialog.linkData())){
    QSqlError sqlError = unit.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Link edition failed.           "));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update links view
  pvForm->select("UnitLink_view");
}

void mdtClUnitEditor::removeLinks()
{
  mdtSqlTableWidget *widget;
  mdtClUnit unit(pvDatabase);
  QMessageBox msgBox;
  QList<QModelIndexList> indexes;
  QSqlError sqlError;
  QStringList fields;

  widget = pvForm->sqlTableWidget("UnitLink_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  fields << "UnitConnectionStart_Id_FK" << "UnitConnectionEnd_Id_FK";
  indexes = widget->indexListOfSelectedRowsByRowsList(fields);
  if(indexes.size() < 1){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove links attached to current unit."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!unit.removeLinks(indexes)){
    QSqlError sqlError = unit.lastError();
    ///QMessageBox msgBox;
    msgBox.setText(tr("Link removing failed.           "));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update connections view
  pvForm->select("UnitLink_view");
}

void mdtClUnitEditor::viewPath()
{
  mdtClPathGraphDialog dialog(0, pvDatabase);
  mdtSqlTableWidget *widget;
  QVariant startConnectionId;

  widget = pvForm->sqlTableWidget("UnitLink_view");
  Q_ASSERT(widget != 0);
  // Get selected row
  startConnectionId = widget->currentData("UnitConnectionStart_Id_FK");
  if(startConnectionId.isNull()){
    QMessageBox msgBox;
    msgBox.setText(tr("Please select a link."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
    return;
  }
  // Setup and show dialog
  if(!dialog.loadLinkList()){
    return;
  }
  if(!dialog.drawPath(startConnectionId)){
    return;
  }

  dialog.exec();
}

QVariant mdtClUnitEditor::currentUnitId()
{
  return pvForm->currentData("Unit_tbl", "Id_PK");
  /**
  QVariant var;

  var = pvForm->currentData("Unit_tbl", "Id_PK");
  if(!var.isValid()){
    return -1;
  }

  return var.toInt();
  */
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

bool mdtClUnitEditor::setupUnitComponentTable()
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
  widget->setColumnHidden("Unit_Id_PK", true);
  widget->setColumnHidden("UnitComponent_Id_PK", true);
  widget->setColumnHidden("Article_Id_FK", true);
  widget->setColumnHidden("Article_Id_PK", true);
  // Set fields a user friendly name
  widget->setHeaderData("SchemaPosition", tr("Schema\nposition"));
  widget->setHeaderData("ArticleCode", tr("Article code"));
  widget->setHeaderData("DesignationEN", tr("Designation (ENG)"));
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

bool mdtClUnitEditor::setupUnitConnectionTable()
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
  // Hide relation fields and PK
  widget->setColumnHidden("UnitConnection_Id_PK", true);
  widget->setColumnHidden("Unit_Id_FK", true);
  widget->setColumnHidden("ArticleConnection_Id_FK", true);
  // Give fields a user friendly name
  widget->setHeaderData("SchemaPage", tr("Schema\npage"));
  widget->setHeaderData("UnitFunctionEN", tr("Unit\nfunction (ENG)"));
  widget->setHeaderData("SignalName", tr("Signal name"));
  widget->setHeaderData("SwAddress", tr("SW address"));
  widget->setHeaderData("UnitConnectorName", tr("Unit\nconnector"));
  widget->setHeaderData("UnitContactName", tr("Unit\ncontact"));
  widget->setHeaderData("ArticleConnectorName", tr("Article\nconnector"));
  widget->setHeaderData("ArticleContactName", tr("Article\ncontact"));
  widget->setHeaderData("IoType", tr("I/O type"));
  widget->setHeaderData("ArticleFunctionEN", tr("Article\nfunction (ENG)"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();

  return true;
}

bool mdtClUnitEditor::setupUnitLinkTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddLink;
  QPushButton *pbEditLink;
  QPushButton *pbRemoveLinks;
  QPushButton *pbViewPath;

  if(!pvForm->addChildTable("UnitLink_view", tr("Links"), pvDatabase)){
    return false;
  }
  if(!pvForm->addRelation("Id_PK", "UnitLink_view", "StartUnit_Id_FK")){
    return false;
  }
  if(!pvForm->addRelation("Id_PK", "UnitLink_view", "EndUnit_Id_FK", "OR")){
    return false;
  }
  widget = pvForm->sqlTableWidget("UnitLink_view");
  Q_ASSERT(widget != 0);
  // Add the Add and remove buttons
  pbAddLink = new QPushButton(tr("Add link ..."));
  connect(pbAddLink, SIGNAL(clicked()), this, SLOT(addLink()));
  widget->addWidgetToLocalBar(pbAddLink);
  pbEditLink = new QPushButton(tr("Edit link"));
  connect(pbEditLink, SIGNAL(clicked()), this, SLOT(editLink()));
  widget->addWidgetToLocalBar(pbEditLink);
  pbRemoveLinks = new QPushButton(tr("Remove links"));
  connect(pbRemoveLinks, SIGNAL(clicked()), this, SLOT(removeLinks()));
  widget->addWidgetToLocalBar(pbRemoveLinks);
  pbViewPath = new QPushButton(tr("View path"));
  connect(pbViewPath, SIGNAL(clicked()), this, SLOT(viewPath()));
  widget->addWidgetToLocalBar(pbViewPath);
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
  widget->setColumnHidden("", true);
  // Give fields a user friendly name
  widget->setHeaderData("StartSchemaPosition", tr("Start\nschema pos."));
  widget->setHeaderData("StartUnitConnectorName", tr("Start\nconnector"));
  widget->setHeaderData("StartUnitContactName", tr("Start\ncontact"));
  widget->setHeaderData("EndSchemaPosition", tr("End\nschema pos."));
  widget->setHeaderData("EndUnitConnectorName", tr("End\nconnector"));
  widget->setHeaderData("EndUnitContactName", tr("End\ncontact"));
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
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();

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
