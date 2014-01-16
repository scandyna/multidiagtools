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
#include "mdtSqlWindowOld.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtError.h"
#include "mdtSqlFormWindow.h"
#include "mdtClUnitVehicleType.h"
#include "mdtClUnitConnectionDialog.h"
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
#include <QInputDialog>
#include <QWidget>
#include <QDataWidgetMapper>
#include <QTableView>

#include <QDebug>

/// \todo Replace form()->select() by select() , witch is defined in mdtClEditor, and is type bool .

mdtClUnitEditor::mdtClUnitEditor(QObject *parent, QSqlDatabase db)
 : mdtClEditor(parent, db)
{
}

mdtClUnitEditor::~mdtClUnitEditor()
{
}

void mdtClUnitEditor::assignVehicle()
{
  mdtSqlSelectionDialog selectionDialog;
  QVariant unitId;
  QSqlError sqlError;
  QModelIndexList selectedVehicles;
  mdtClUnitVehicleType uvt(database());

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
  form()->select("Unit_VehicleType_view");
}

void mdtClUnitEditor::removeVehicleAssignation()
{
  QVariant unitId;
  QSqlError sqlError;
  int ret;
  QMessageBox msgBox;
  mdtSqlTableWidget *vehicleTypeWidget;
  QModelIndexList indexes;
  mdtClUnitVehicleType uvt(database());

  // Get vehicle widget
  vehicleTypeWidget = form()->sqlTableWidget("Unit_VehicleType_view");
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
  form()->select("Unit_VehicleType_view");
}

void mdtClUnitEditor::setBaseArticle()
{
  mdtClUnit unit(this, database());
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
  if(form()->currentData("Unit_tbl", "Article_Id_FK").toInt() > 0){
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
      form()->setCurrentData("Unit_tbl", "Article_Id_FK", QVariant());
      return;
    }
    if(msgBox.clickedButton() != pbSetBaseArticle){
      return;
    }
  }
  // Setup article selection dialog and show it to user
  sql = "SELECT * FROM Article_tbl;";
  model.setQuery(sql, database());
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
  if(!form()->setCurrentData("Unit_tbl", "Article_Id_FK", selectedItem.at(0))){
    return;
  }
}

void mdtClUnitEditor::addComponent()
{
  mdtSqlSelectionDialog dialog;
  QSqlQueryModel *unitModel;
  mdtClUnit unit(this, database());

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
  form()->select("UnitComponent_view");
}

void mdtClUnitEditor::removeComponents()
{
  mdtSqlTableWidget *widget;
  mdtClUnit unit(this, database());
  QMessageBox msgBox;
  QModelIndexList indexes;
  int ret;

  widget = form()->sqlTableWidget("UnitComponent_view");
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
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update component table
  form()->select("UnitComponent_view");
}

void mdtClUnitEditor::addConnector()
{
  QVariant unitId;
  QVariant connectorName;
  mdtClUnit unit(this, database());
  QInputDialog dialog;

  // Get unit ID
  unitId = currentUnitId();
  if(unitId.isNull()){
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
  // Add connector
  if(!unit.addConnector(unitId, QVariant(), QVariant(), connectorName)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  form()->select("UnitConnection_view");
}

void mdtClUnitEditor::addConnectorBasedConnector()
{
  QVariant unitId;
  QVariant baseConnectorId;
  QVariant connectorName;
  QList<QVariant> contactList;
  QList<mdtClUnitConnectionData> contactDataList;
  mdtClUnit unit(this, database());
  QInputDialog dialog;
  int i;

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
  for(i = 0; i < contactList.size(); ++i){
    contactDataList.append(unit.getBaseConnectorContactData(contactList.at(i)));
  }
  // Add connector
  if(!unit.addConnector(unitId, baseConnectorId, QVariant(), connectorName, contactDataList)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  form()->select("UnitConnection_view");
}

void mdtClUnitEditor::addArticleConnectorBasedConnector()
{
  QVariant unitId;
  QVariant connectorName;
  mdtClUnitConnectionData connectorData;
  QVariant articleConnectorId;
  QList<QVariant> articleConnectionList;
  QList<mdtClUnitConnectionData> connectionDataList;
  mdtClUnit unit(this, database());
  QInputDialog dialog;
  int i;

  // Get unit ID
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  // Let user choose a article connector
  articleConnectorId = selectArticleConnector();
  if(articleConnectorId.isNull()){
    return;
  }
  // Get article connector data
  connectorData = unit.getArticleConnectorData(articleConnectorId);
  // Set connector name
  ///connectorName = unit.getArticleConnectorName(articleConnectorId);
  connectorName = connectorData.articleConnectionData().connectorName();
  dialog.setLabelText(tr("Connector name:"));
  dialog.setTextValue(connectorName.toString());
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  connectorName = dialog.textValue().trimmed();
  if(connectorName.isNull()){
    return;
  }
  connectorData.setConnectorName(connectorName);
  // Select article connections to use
  articleConnectionList = selectByArticleConnectorIdArticleConnectionIdList(articleConnectorId, unitId);
  if(articleConnectionList.isEmpty()){
    return;
  }
  // Get connections data
  for(i = 0; i < articleConnectionList.size(); ++i){
    connectionDataList.append(unit.getArticleConnectionData(articleConnectionList.at(i)));
    connectionDataList[i].copyArticleConnectionAttributes();
  }
  // Add connector
  if(!unit.addConnector(unitId, QVariant(), articleConnectorId, connectorName, connectionDataList)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  form()->select("UnitConnection_view");
}

void mdtClUnitEditor::removeConnector()
{
  QVariant connectorId;
  mdtClUnit unit(this, database());

  connectorId = selectUnitConnector();
  if(connectorId.isNull()){
    return;
  }
  if(!unit.removeConnector(connectorId)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  form()->select("UnitConnection_view");
}

void mdtClUnitEditor::addConnection()
{
  mdtClUnitConnectionDialog connectionDialog(0, database());
  mdtClUnit unit(this, database());
  QVariant unitId;
  QVariant unitConnectorId;
  QVariant articleConnectionId;
  QVariant contactId;
  mdtClUnitConnectionData data;

  // We must have current unit ID
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  // Select unit connector
  unitConnectorId = selectUnitConnector();
  if(!unitConnectorId.isNull()){
    data = unit.getUnitConnectorData(unitConnectorId);
  }
  /*
   * Here we have several cases:
   *  - 1) A unit connector was choosen, and it is based on a article connector
   *  - 2) A unit connector was choosen, and it is based on a connector (from Connector_tbl)
   *  - 3) A unit connector was choosen, but it is based on nothing (free connector)
   *  - 4) No unit connector was choosen
   */
  if(!data.articleConnectionData().articleConnectorId().isNull()){
    // Case 1)
    articleConnectionId = selectByArticleConnectorIdArticleConnectionId(data.articleConnectionData().articleConnectorId() , unitId);
    data = unit.getArticleConnectionData(articleConnectionId, data);
  }else if(!data.connectorId().isNull()){
    // Case 2)
    contactId = selectBaseConnectorContactId(data.connectorId());
    data = unit.getBaseConnectorContactData(contactId, data);
  }
  /**
  // If connector is based on a article connector, let user choose article connection
  if(!data.articleConnectionData().connectorId().isNull()){
    ///articleConnectionId = selectArticleConnection(data.articleConnectionData().connectorId());
    articleConnectionId = selectArticleConnectionLinkedToUnitConnector(unitConnectorId, unitId);
    if(articleConnectionId.isNull()){
      return;
    }
    // Get article connection data
    data = unit.getArticleConnectionData(articleConnectionId, data);
    data.copyArticleConnectionAttributes();
  }
  */
  // Setup and show unit connection edition dialog
  data.setUnitId(unitId);
  connectionDialog.setData(data);
  if(connectionDialog.exec() != QDialog::Accepted){
    return;
  }
  // Add connection
  if(!unit.addUnitConnection(connectionDialog.data())){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  form()->select("UnitConnection_view");
}

void mdtClUnitEditor::addArticleConnectionBasedConnection()
{
  mdtClUnitConnectionDialog connectionDialog(0, database());
  mdtClUnit unit(this, database());
  QVariant unitId;
  QVariant articleId;
  ///QVariant unitConnectorId;
  QVariant articleConnectionId;
  mdtClUnitConnectionData data;

  // We must have current unit and article ID
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  articleId = form()->currentData("Unit_tbl", "Article_Id_FK");
  if(articleId.isNull()){
    return;
  }
  // Select article connection ID
  articleConnectionId = selectByArticleIdArticleConnectionId(articleId, unitId);
  if(articleConnectionId.isNull()){
    return;
  }
  data = unit.getArticleConnectionData(articleConnectionId, data);
  data.copyArticleConnectionAttributes();
  /*
   * Check if seleced article connection is based on a article connector,
   *  wich is used as base of a unit connector used in current unit .
   *  If true, get unit connector data .
   */
  if(!data.articleConnectionData().articleConnectorId().isNull()){
    data = unit.getUnitConnectorDataByArticleConnectorId(data.articleConnectionData().articleConnectorId(), unitId, data);
  }
  // Setup and show unit connection edition dialog
  data.setUnitId(unitId);
  connectionDialog.setData(data);
  if(connectionDialog.exec() != QDialog::Accepted){
    return;
  }
  // Add connection
  if(!unit.addUnitConnection(connectionDialog.data())){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  form()->select("UnitConnection_view");
}

/**
void mdtClUnitEditor::addFreeConnection()
{
  mdtClUnitConnectionDialog dialog(0, database());
  mdtClUnit unit(this, database());

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
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  form()->select("UnitConnection_view");
}
*/

void mdtClUnitEditor::editConnection()
{
  mdtSqlTableWidget *widget;
  mdtClUnitConnectionDialog dialog(0, database());
  mdtClUnit unit(this, database());
  mdtClUnitConnectionData data;

  widget = form()->sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != 0);

  // Get current item's data
  ///data = unit.getUnitConnectionData(widget->currentData("UnitConnection_Id_PK"));
  data = unit.getConnectionDataByUnitConnectionId(widget->currentData("UnitConnection_Id_PK"));
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
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  form()->select("UnitConnection_view");
}

void mdtClUnitEditor::removeConnections()
{
  mdtSqlTableWidget *widget;
  mdtClUnit unit(this, database());
  QMessageBox msgBox;
  QModelIndexList indexes;
  QString linksMsg;

  widget = form()->sqlTableWidget("UnitConnection_view");
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
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  form()->select("UnitConnection_view");
}

void mdtClUnitEditor::addLink()
{
  mdtSqlFormWidget *widget;
  mdtClUnitLinkDialog dialog(0, database());
  QVariant unitId;
  mdtClUnit unit(this, database());

  widget = form()->sqlFormWidget("Unit_tbl");
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
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update links view
  form()->select("UnitLink_view");
}

void mdtClUnitEditor::editLink()
{
  mdtSqlFormWidget *unitWidget;
  mdtSqlTableWidget *linkWidget;
  mdtClUnitLinkDialog dialog(0, database());
  QVariant unitId, startConnectionId, endConnectionId;
  mdtClUnitConnectionData startConnectionData;
  mdtClUnitConnectionData endConnectionData;
  mdtClLinkData linkData;
  mdtClUnit unit(this, database());

  unitWidget = form()->sqlFormWidget("Unit_tbl");
  Q_ASSERT(unitWidget != 0);
  linkWidget = form()->sqlTableWidget("UnitLink_view");
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
  startConnectionData = unit.getConnectionDataByUnitConnectionId(startConnectionId);
  if(!startConnectionData.isValid()){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  endConnectionData = unit.getConnectionDataByUnitConnectionId(endConnectionId);
  if(!endConnectionData.isValid()){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  linkData = unit.getUnitLinkData(startConnectionId, endConnectionId);
  if(!linkData.isValid()){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Setup and show dialog
  unitId = currentUnitId();
  dialog.setStartUnit(unitId, unitWidget->currentData("SchemaPosition"), unitWidget->currentData("Cabinet"));
  dialog.setStartConnectionData(startConnectionData);
  /// \bug This displays allways current unit, this is wrong !!
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
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update links view
  form()->select("UnitLink_view");
}

void mdtClUnitEditor::removeLinks()
{
  mdtSqlTableWidget *widget;
  mdtClUnit unit(this, database());
  QMessageBox msgBox;
  QList<QModelIndexList> indexes;
  QSqlError sqlError;
  QStringList fields;

  widget = form()->sqlTableWidget("UnitLink_view");
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
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  form()->select("UnitLink_view");
}

void mdtClUnitEditor::viewPath()
{
  mdtClPathGraphDialog dialog(0, database());
  mdtSqlTableWidget *widget;
  QVariant startConnectionId;

  widget = form()->sqlTableWidget("UnitLink_view");
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
  return form()->currentData("Unit_tbl", "Id_PK");
}

QVariant mdtClUnitEditor::selectBaseConnector()
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  QString sql;

  if(currentUnitId().isNull()){
    return QVariant();
  }
  // Setup model to show available connectors
  sql = "SELECT * FROM Connector_tbl";
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get connectors list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnitEditor");
    pvLastError.commit();
    displayLastError();
    return QVariant();
  }
  // Setup and show dialog
  selectionDialog.setMessage("Please select a connector.");
  selectionDialog.setModel(&model, false);
  ///selectionDialog.setColumnHidden("Id_PK", true);
  ///selectionDialog.setHeaderData("SubType", tr("Variant"));
  ///selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QList<QVariant> mdtClUnitEditor::selectBaseConnectorContactIdList(const QVariant & connectorId, bool multiSelection)
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  QString sql;
  QModelIndexList selectedItems;
  QList<QVariant> idList;
  int i;

  if(connectorId.isNull()){
    return idList;
  }
  // Setup model to show available connectors
  sql = "SELECT * FROM ConnectorContact_tbl ";
  sql += " WHERE Connector_Id_FK = " + connectorId.toString();
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get connector contacts list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnitEditor");
    pvLastError.commit();
    displayLastError();
    return idList;
  }
  // Setup and show dialog
  selectionDialog.setMessage("Please select contacts to use.");
  selectionDialog.setModel(&model, multiSelection);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Connector_Id_FK", true);
  ///selectionDialog.setHeaderData("SubType", tr("Variant"));
  ///selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return idList;
  }
  selectedItems = selectionDialog.selectionResults();
  for(i = 0; i < selectedItems.size(); ++i){
    idList.append(selectedItems.at(i).data());
  }

  return idList;
}

QVariant mdtClUnitEditor::selectBaseConnectorContactId(const QVariant & connectorId)
{
  QList<QVariant> idList;

  idList = selectBaseConnectorContactIdList(connectorId, false);
  if(idList.isEmpty()){
    return QVariant();
  }

  return idList.at(0);
}

QVariant mdtClUnitEditor::selectUnitConnector()
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  QString sql;
  QVariant unitId;

  unitId = currentUnitId();
  if(unitId.isNull()){
    return QVariant();
  }
  // Setup model to show available connectors
  sql = "SELECT * FROM UnitConnector_tbl WHERE Unit_Id_FK = " + unitId.toString();
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get unit connectors list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnitEditor");
    pvLastError.commit();
    displayLastError();
    return QVariant();
  }
  // Setup and show dialog
  selectionDialog.setMessage("Please select a connector.");
  selectionDialog.setModel(&model, false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_FK", true);
  selectionDialog.setColumnHidden("Connector_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnector_Id_FK", true);
  ///selectionDialog.setHeaderData("Unit_Id_FK", tr("Variant"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(400, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtClUnitEditor::selectArticleConnectionLinkedToUnitConnector(const QVariant & unitConnectorId, const QVariant & unitId)
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  mdtClUnit unit(this, database());
  QString sql;
  QString msg;

  if((unitConnectorId.isNull())||(unitId.isNull())){
    return QVariant();
  }
  // Setup model
  sql = unit.sqlForArticleConnectionLinkedToUnitConnectorSelection(unitConnectorId, unitId);
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get list of article connections related to unit connector."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnitEditor");
    pvLastError.commit();
    displayLastError();
    return QVariant();
  }
  // Setup and show dialog
  msg = tr("Selected connector is based on a article connector.\n");
  msg += tr("Please select a article based connection to use.");
  selectionDialog.setMessage(msg);
  selectionDialog.setModel(&model, false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_FK", true);
  selectionDialog.setColumnHidden("UnitConnector_Id_PK", true);
  selectionDialog.setColumnHidden("ArticleConnector_Id_PK", true);
  selectionDialog.setColumnHidden("ArticleConnection_Id_PK", true);
  ///selectionDialog.setHeaderData("Unit_Id_FK", tr("Variant"));
  selectionDialog.addSelectionResultColumn("ArticleConnection_Id_PK");
  selectionDialog.resize(700, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtClUnitEditor::selectArticleConnector()
{
  Q_ASSERT(form() != 0);

  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  QString sql;
  mdtClUnit unit(this, database());
  QVariant unitId;
  QVariant articleId;

  // Get unit and article IDs
  unitId = currentUnitId();
  if(unitId.isNull()){
    return QVariant();
  }
  articleId = form()->currentData("Unit_tbl", "Article_Id_FK");
  if(articleId.isNull()){
    return QVariant();
  }
  // Setup model to show available connectors
  sql = unit.sqlForArticleConnectorSelection(articleId, unitId);
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError = unit.lastError();
    displayLastError();
    return QVariant();
  }
  // Setup and show dialog
  selectionDialog.setMessage("Please select a connector.");
  selectionDialog.setModel(&model, false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QList<QVariant> mdtClUnitEditor::selectByArticleConnectorIdArticleConnectionIdList(const QVariant & articleConnectorId, const QVariant & unitId, bool multiSelection)
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  mdtClUnit unit(this, database());
  QString sql;
  QModelIndexList selectedItems;
  QList<QVariant> idList;
  int i;

  if((articleConnectorId.isNull())||(unitId.isNull())){
    return idList;
  }
  // Setup model to show available article connections
  sql = unit.sqlForArticleConnectionLinkedToArticleConnectorSelection(articleConnectorId, unitId);
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError = unit.lastError();
    displayLastError();
    return idList;
  }
  // Setup and show dialog
  selectionDialog.setMessage("Please select article connections to use.");
  selectionDialog.setModel(&model, multiSelection);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnector_Id_FK", true);
  selectionDialog.setHeaderData("ArticleContactName", tr("Contact"));
  selectionDialog.setHeaderData("IoType", tr("I/O type"));
  selectionDialog.setHeaderData("FunctionEN", tr("Function\n(English)"));
  selectionDialog.setHeaderData("FunctionFR", tr("Function\n(French)"));
  selectionDialog.setHeaderData("FunctionDE", tr("Function\n(German)"));
  selectionDialog.setHeaderData("FunctionIT", tr("Function\n(Italian)"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(700, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return idList;
  }
  selectedItems = selectionDialog.selectionResults();
  for(i = 0; i < selectedItems.size(); ++i){
    idList.append(selectedItems.at(i).data());
  }

  return idList;
}

QVariant mdtClUnitEditor::selectByArticleConnectorIdArticleConnectionId(const QVariant & articleConnectorId, const QVariant & unitId)
{
  QList<QVariant> idList;

  idList = selectByArticleConnectorIdArticleConnectionIdList(articleConnectorId, unitId, false);
  if(idList.isEmpty()){
    return QVariant();
  }

  return idList.at(0);
}

QList<QVariant> mdtClUnitEditor::selectByArticleIdArticleConnectionIdList(const QVariant & articleId, const QVariant & unitId, bool multiSelection)
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  mdtClUnit unit(this, database());
  QString sql;
  QModelIndexList selectedItems;
  QList<QVariant> idList;
  int i;

  if((articleId.isNull())||(unitId.isNull())){
    return idList;
  }
  // Setup model to show available article connections
  sql = unit.sqlForArticleConnectionLinkedToArticleSelection(articleId, unitId);
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError = unit.lastError();
    displayLastError();
    return idList;
  }
  // Setup and show dialog
  selectionDialog.setMessage("Please select article connection to use.");
  selectionDialog.setModel(&model, multiSelection);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnector_Id_FK", true);
  selectionDialog.setHeaderData("ArticleContactName", tr("Contact"));
  selectionDialog.setHeaderData("IoType", tr("I/O type"));
  selectionDialog.setHeaderData("FunctionEN", tr("Function\n(English)"));
  selectionDialog.setHeaderData("FunctionFR", tr("Function\n(French)"));
  selectionDialog.setHeaderData("FunctionDE", tr("Function\n(German)"));
  selectionDialog.setHeaderData("FunctionIT", tr("Function\n(Italian)"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(700, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return idList;
  }
  selectedItems = selectionDialog.selectionResults();
  for(i = 0; i < selectedItems.size(); ++i){
    idList.append(selectedItems.at(i).data());
  }

  return idList;
}

QVariant mdtClUnitEditor::selectByArticleIdArticleConnectionId(const QVariant & articleId, const QVariant & unitId)
{
  QList<QVariant> idList;

  idList = selectByArticleIdArticleConnectionIdList(articleId, unitId, false);
  if(idList.isEmpty()){
    return QVariant();
  }

  return idList.at(0);
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

bool mdtClUnitEditor::setupUnitTable()
{
  Q_ASSERT(sqlWindow() != 0);

  Ui::mdtClUnitEditor ue;
  QSqlTableModel *unitModel;
  QDataWidgetMapper *baseArticleMapper;
  QSqlTableModel *baseArticleModel;
  mdtSqlRelation *baseArticleRelation;

  // Setup main form widget
  ue.setupUi(form()->mainSqlWidget());
  connect(ue.pbSetBaseArticle, SIGNAL(clicked()), this, SLOT(setBaseArticle()));
  connect(this, SIGNAL(unitEdited()), form()->mainSqlWidget(), SIGNAL(dataEdited()));
  // Setup form
  if(!form()->setTable("Unit_tbl", "Units", database())){
    return false;
  }
  sqlWindow()->enableNavigation();
  sqlWindow()->enableEdition();
  sqlWindow()->resize(800, 500);
  sqlWindow()->setWindowTitle(tr("Unit edition"));
  /*
   * Setup base article widget mapping
   */
  unitModel = form()->model("Unit_tbl");
  Q_ASSERT(unitModel != 0);
  // Setup base article model
  baseArticleModel = new QSqlTableModel(this, database());
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
  connect(form()->mainSqlWidget(), SIGNAL(currentRowChanged(int)), baseArticleRelation, SLOT(setParentCurrentIndex(int)));
  connect(baseArticleRelation, SIGNAL(childModelFilterApplied()), baseArticleMapper, SLOT(toFirst()));
  connect(baseArticleRelation, SIGNAL(childModelIsEmpty()), baseArticleMapper, SLOT(revert()));
  // Force a update
  form()->mainSqlWidget()->setCurrentIndex(form()->mainSqlWidget()->currentRow());

  return true;
}

bool mdtClUnitEditor::setupUnitComponentTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddComponent;
  QPushButton *pbRemoveComponents;

  if(!form()->addChildTable("UnitComponent_view", tr("Components"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "UnitComponent_view", "Unit_Id_PK")){
    return false;
  }
  widget = form()->sqlTableWidget("UnitComponent_view");
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
  QPushButton *pbAddConnector;
  QPushButton *pbAddConnectorBasedConnector;
  QPushButton *pbAddArticleConnectorBasedConnector;
  QPushButton *pbRemoveConnector;
  QPushButton *pbAddConnection;
  QPushButton *pbAddArticleConnectionBasedConnection;
  QPushButton *pbEditConnection;
  QPushButton *pbRemoveConnection;

  if(!form()->addChildTable("UnitConnection_view", tr("Connections"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "UnitConnection_view", "Unit_Id_FK")){
    return false;
  }
  widget = form()->sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != 0);
  // Add the Add and remove buttons
  pbAddConnector = new QPushButton(tr("Add free connector ..."));
  pbAddConnectorBasedConnector = new QPushButton(tr("Add connector ..."));
  pbAddArticleConnectorBasedConnector = new QPushButton(tr("Add art. connector ..."));
  pbRemoveConnector = new QPushButton(tr("Rem. connector"));
  pbAddConnection = new QPushButton(tr("Add connection ..."));
  pbAddArticleConnectionBasedConnection = new QPushButton(tr("Add art. connection ..."));
  pbEditConnection = new QPushButton(tr("Edit connection ..."));
  pbRemoveConnection = new QPushButton(tr("Remove connections"));
  connect(pbAddConnector, SIGNAL(clicked()), this, SLOT(addConnector()));
  connect(pbAddConnectorBasedConnector, SIGNAL(clicked()), this, SLOT(addConnectorBasedConnector()));
  connect(pbAddArticleConnectorBasedConnector, SIGNAL(clicked()), this, SLOT(addArticleConnectorBasedConnector()));
  connect(pbRemoveConnector, SIGNAL(clicked()), this, SLOT(removeConnector()));
  connect(pbAddConnection, SIGNAL(clicked()), this, SLOT(addConnection()));
  connect(pbAddArticleConnectionBasedConnection, SIGNAL(clicked()), this, SLOT(addArticleConnectionBasedConnection()));
  connect(pbEditConnection, SIGNAL(clicked()), this, SLOT(editConnection()));
  connect(pbRemoveConnection, SIGNAL(clicked()), this, SLOT(removeConnections()));
  widget->addWidgetToLocalBar(pbAddConnector);
  widget->addWidgetToLocalBar(pbAddConnectorBasedConnector);
  widget->addWidgetToLocalBar(pbAddArticleConnectorBasedConnector);
  widget->addWidgetToLocalBar(pbRemoveConnector);
  widget->addWidgetToLocalBar(pbAddConnection);
  widget->addWidgetToLocalBar(pbAddArticleConnectionBasedConnection);
  widget->addWidgetToLocalBar(pbEditConnection);
  widget->addWidgetToLocalBar(pbRemoveConnection);
  widget->addStretchToLocalBar();
  // Hide relation fields and PK
  widget->setColumnHidden("UnitConnection_Id_PK", true);
  widget->setColumnHidden("Unit_Id_FK", true);
  widget->setColumnHidden("ArticleConnection_Id_FK", true);
  // Give fields a user friendly name
  widget->setHeaderData("SchemaPage", tr("Schema\npage"));
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

  if(!form()->addChildTable("UnitLink_view", tr("Links"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "UnitLink_view", "StartUnit_Id_FK")){
    return false;
  }
  if(!form()->addRelation("Id_PK", "UnitLink_view", "EndUnit_Id_FK", "OR")){
    return false;
  }
  widget = form()->sqlTableWidget("UnitLink_view");
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
  if(!form()->addChildTable("Unit_VehicleType_view", tr("Vehicles"), database())){
    return false;
  }
  // Setup Unit <-> VehicleType relation
  if(!form()->addRelation("Id_PK", "Unit_VehicleType_view", "Unit_Id_FK")){
    return false;
  }
  // Get widget to continue setup
  widget = form()->sqlTableWidget("Unit_VehicleType_view");
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
