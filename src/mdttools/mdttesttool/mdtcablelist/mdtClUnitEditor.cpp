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
#include "mdtClUnitEditor.h"
#include "mdtClUnit.h"
#include "mdtClLink.h"
#include "mdtClLinkVersion.h"
#include "mdtClModificationKeyData.h"
#include "ui_mdtClUnitEditor.h"
#include "mdtSqlTableWidget.h"
#include "mdtAbstractSqlTableController.h"
#include "mdtSqlDataWidgetController.h"
#include "mdtSqlRelationInfo.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtClArticleSelectionDialog.h"
#include "mdtClArticleConnectorSelectionDialog.h"
#include "mdtClArticleConnectionSelectionDialog.h"
#include "mdtError.h"
#include "mdtClUnitVehicleType.h"
#include "mdtClConnectorSelectionDialog.h"
#include "mdtClConnectorContactSelectionDialog.h"
#include "mdtClUnitConnectionDialog.h"
#include "mdtClLinkData.h"
#include "mdtClUnitConnection.h"
#include "mdtClUnitLinkDialog.h"
#include "mdtClUnitLinkModificationDialog.h"
#include "mdtClPathGraph.h"
#include "mdtClPathGraphDialog.h"
#include "mdtClLinkedUnitConnectionInfoDialog.h"
#include "mdtClLinkedUnitConnectorInfoDialog.h"
#include "mdtClConnectableConnectorDialog.h"
#include "mdtClAutoConnection.h"
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
#include <memory>

#include <QDebug>

mdtClUnitEditor::mdtClUnitEditor(QWidget *parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

/**
mdtClUnitEditor::~mdtClUnitEditor()
{
}
*/

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
  // Setup connector table
  if(!setupUnitConnectorTable()){
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

// bool mdtClUnitEditor::setWorkingOnVehicleTypeIdList(const QList<QVariant> & vtIdList)
// {
//   QSqlTableModel *m;
//   QString sql;
//   int i;
// 
//   pvWorkingOnVehicleTypeIdList = vtIdList;
//   // Get main table model
//   ///m = model("Unit_tbl");
//   m = 0;
//   if(m == 0){
//     pvLastError.setError(tr("Cannot set vehicle type filter because model was not set on 'Unit_tbl'."), mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtClUnitEditor");
//     pvLastError.commit();
//     return false;
//   }
//   // Build the WHERE clause for the filter
//   if(pvWorkingOnVehicleTypeIdList.isEmpty()){
//     m->setFilter("");
//     return true;
//   }
//   Q_ASSERT(pvWorkingOnVehicleTypeIdList.size() > 0);
//   sql = "Id_PK IN (";
//   sql += "SELECT Unit_Id_FK FROM VehicleType_Unit_tbl WHERE VehicleType_Id_FK = " + pvWorkingOnVehicleTypeIdList.at(0).toString();
//   for(i = 1; i < pvWorkingOnVehicleTypeIdList.size(); ++i){
//     sql += " OR VehicleType_Id_FK = " + pvWorkingOnVehicleTypeIdList.at(i).toString();
//   }
//   sql += ")";
// 
//   m->setFilter(sql);
// 
//   return true;
// }

void mdtClUnitEditor::assignVehicle()
{
  mdtSqlSelectionDialog selectionDialog;
  QVariant unitId;
  QModelIndexList selectedVehicles;
  mdtClUnitVehicleType uvt(0, database());
  QString sql;
  ///QSqlQueryModel model;

  // Get current unit ID
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  // Setup and show dialog
  sql = uvt.sqlForVehicleTypeNotAssignedToUnit(unitId);
  ///model.setQuery(sql, database());
  selectionDialog.setMessage("Please select a vehicle");
  ///selectionDialog.setModel(&model, true);
  selectionDialog.setQuery(sql, database(), true);
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
    pvLastError = uvt.lastError();
    displayLastError();
    return;
  }
  select("Unit_VehicleType_view");
}

void mdtClUnitEditor::removeVehicleAssignation()
{
  QVariant unitId;
  QSqlError sqlError;
  int ret;
  QMessageBox msgBox;
  mdtSqlTableWidget *vehicleTypeWidget;
  ///QModelIndexList indexes;
  mdtSqlTableSelection s;
  mdtClUnitVehicleType uvt(0, database());

  // Get vehicle widget
  vehicleTypeWidget = sqlTableWidget("Unit_VehicleType_view");
  Q_ASSERT(vehicleTypeWidget != 0);
  Q_ASSERT(vehicleTypeWidget->selectionModel() != 0);
  // Get current unit ID
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  // Get selected rows
  s = vehicleTypeWidget->currentSelection("VehicleType_Id_FK");
  if(s.isEmpty()){
    return;
  }
  /**
  indexes = vehicleTypeWidget->indexListOfSelectedRows("VehicleType_Id_FK");
  if(indexes.size() < 1){
    return;
  }
  */
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
  if(!uvt.removeUnitVehicleAssignments(unitId, s)){
    pvLastError = uvt.lastError();
    displayLastError();
    return;
  }
  select("Unit_VehicleType_view");
}

void mdtClUnitEditor::setBaseArticle()
{
  mdtClUnit unit(this, database());
  mdtClArticleSelectionDialog asDialog(this, database());
  int ret;
  QString sql;

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
  if(currentData("Unit_tbl", "Article_Id_FK").toInt() > 0){
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
      setCurrentData("Unit_tbl", "Article_Id_FK", QVariant());
      return;
    }
    if(msgBox.clickedButton() != pbSetBaseArticle){
      return;
    }
  }
  // Let the user select article
  if(asDialog.exec() != QDialog::Accepted){
    return;
  }
  // Set to db
  if(!setCurrentData("Unit_tbl", "Article_Id_FK", asDialog.selectedArticleId(), true)){
    displayLastError();
    return;
  }
  submit();
  // Update UI
  refresh();
}

void mdtClUnitEditor::addComponent()
{
  mdtClUnit unit(this, database());
  mdtSqlSelectionDialog dialog;
  QString sql;
  QVariant unitId;

  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  // Setup model
  sql = unit.sqlForComponentSelection(unitId);
  // Setup and show dialog
  dialog.setMessage(tr("Please select a unit:"));
  dialog.setQuery(sql, database(), false);
  dialog.setColumnHidden("Id_PK", true);
  dialog.setHeaderData("SchemaPosition", tr("Vehicle\nSchema\nPos."));
  dialog.addSelectionResultColumn("Id_PK");
  dialog.setWindowTitle(tr("Unit component selection"));
  dialog.resize(600, 400);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Store result
  Q_ASSERT(dialog.selectionResult().size() == 1);
  // Add component
  if(!unit.addComponent(currentUnitId(), dialog.selectionResult().at(0))){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update component table
  select("UnitComponent_view");
}

void mdtClUnitEditor::removeComponents()
{
  mdtSqlTableWidget *widget;
  mdtClUnit unit(this, database());
  QMessageBox msgBox;
  ///QModelIndexList indexes;
  mdtSqlTableSelection s;
  int ret;

  widget = sqlTableWidget("UnitComponent_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("UnitComponent_Id_PK");
  if(s.isEmpty()){
    return;
  }
  /**
  indexes = widget->indexListOfSelectedRows("UnitComponent_Id_PK");
  if(indexes.size() < 1){
    return;
  }
  */
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
  if(!unit.removeComponents(s)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update component table
  select("UnitComponent_view");
}

void mdtClUnitEditor::addConnector()
{
  mdtClUnitConnection ucnx(database());
  QInputDialog dialog(this);
  mdtClUnitConnectorKeyData key;
  mdtClUnitConnectorData data;

  // Get unit ID
  key.setUnitId(currentUnitId());
  if(key.unitId().isNull()){
    return;
  }
  data.setKeyData(key);
  // Set connector name
  dialog.setLabelText(tr("Connector name:"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  data.name = dialog.textValue().trimmed();
  if(data.name.toString().isEmpty()){
    return;
  }
  // Add connector
  if(ucnx.addUnitConnector(data, true).isNull()){
    pvLastError = ucnx.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("UnitConnection_view");
  select("UnitConnector_view");
  select("UnitLink_view");
}

void mdtClUnitEditor::addConnectorBasedConnector()
{
  mdtClUnitConnection ucnx(database());
  QInputDialog inputDialog(this);
  mdtClUnitConnectorKeyData unitConnectorKey;
  mdtClUnitConnectorData unitConnectorData;
  mdtClConnectorSelectionDialog csDialog(this);
  mdtClConnectorPkData connectorKey;
  mdtClConnectorData connectorData;
  mdtClConnectorContactSelectionDialog ccsDialog(this);

  // Set unit ID
  unitConnectorKey.setUnitId(currentUnitId());
  if(unitConnectorKey.unitId().isNull()){
    return;
  }
  // Select connector from Connector_tbl
  if(!csDialog.select(database())){
    pvLastError = csDialog.lastError();
    displayLastError();
    return;
  }
  if(csDialog.exec() != QDialog::Accepted){
    return;
  }
  connectorKey = csDialog.selectedConnectorPk();
  if(connectorKey.isNull()){
    return;
  }
  unitConnectorKey.setConnectorFk(connectorKey);
  unitConnectorData.setKeyData(unitConnectorKey);
  // Set connector name
  inputDialog.setLabelText(tr("Connector name:"));
  if(inputDialog.exec() != QDialog::Accepted){
    return;
  }
  unitConnectorData.name = inputDialog.textValue().trimmed();
  if(unitConnectorData.name.toString().isEmpty()){
    return;
  }
  // Select contacts and add them to unit connector
  if(!ccsDialog.select(database(), connectorKey, true)){
    pvLastError = ccsDialog.lastError();
    displayLastError();
    return;
  }
  if(ccsDialog.exec() != QDialog::Accepted){
    return;
  }
  ucnx.addConnectionsToUnitConnector(unitConnectorData, ccsDialog.selectedContactDataList());
  // Add unit connector to database
  if(ucnx.addUnitConnector(unitConnectorData, true).isNull()){
    pvLastError = ucnx.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("UnitConnector_view");
  select("UnitConnection_view");
  select("UnitLink_view");
}

void mdtClUnitEditor::addArticleConnectorBasedConnector()
{
  mdtClUnitConnection ucnx(database());
  QInputDialog inputDialog(this);
  mdtClUnitConnectorKeyData unitConnectorKey;
  mdtClUnitConnectorData unitConnectorData;
  mdtClArticleConnectorSelectionDialog acnrsDialog(this);
  mdtClArticleConnectorKeyData articleConnectorFk;
  mdtClArticleConnectionSelectionDialog acnxsDialog(this);

  // Check that vehicle type assignation was made
  if(rowCount("Unit_VehicleType_view", false) < 1){
    pvLastError.setError(tr("No vehicle type was assigned."), mdtError::Error);
    pvLastError.setInformativeText(tr("Please assign a vehicle type and try again."));
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnitEditor");
    pvLastError.commit();
    displayLastError();
    return;
  }
  // Set unit ID
  unitConnectorKey.setUnitId(currentUnitId());
  if(unitConnectorKey.unitId().isNull()){
    return;
  }
  // Get base article ID
  articleConnectorFk.setArticleId(currentData("Unit_tbl", "Article_Id_FK"));
  if(articleConnectorFk.articleId().isNull()){
    return;
  }
  // Let user select a article connector
  if(!acnrsDialog.select(database(), articleConnectorFk.articleId(), unitConnectorKey.unitId())){
    pvLastError = acnrsDialog.lastError();
    displayLastError();
    return;
  }
  if(acnrsDialog.exec() != QDialog::Accepted){
    return;
  }
  articleConnectorFk = acnrsDialog.selectedArticleConnectorKey();
  if(articleConnectorFk.isNull()){
    return;
  }
  unitConnectorKey.setArticleConnectorFk(articleConnectorFk);
  unitConnectorData.setKeyData(unitConnectorKey);
  // Set connector name
  inputDialog.setLabelText(tr("Connector name:"));
  inputDialog.setTextValue(acnrsDialog.selectedArticleConnectorName().toString());
  if(inputDialog.exec() != QDialog::Accepted){
    return;
  }
  unitConnectorData.name = inputDialog.textValue().trimmed();
  if(unitConnectorData.name.toString().isEmpty()){
    return;
  }
  // Select article connections to use
  if(!acnxsDialog.select(database(), articleConnectorFk, unitConnectorKey.unitId(), true)){
    pvLastError = acnxsDialog.lastError();
    displayLastError();
    return;
  }
  if(acnxsDialog.exec() != QDialog::Accepted){
    return;
  }
  // Build unit connections from selected article connections
  ucnx.addConnectionsToUnitConnector(unitConnectorData, acnxsDialog.selectedArticleConnectionDataList());
  // Add unit connector to database
  if(ucnx.addUnitConnector(unitConnectorData, true).isNull()){
    pvLastError = ucnx.lastError();
    displayLastError();
    return;
  }
  // If some links has been added, we tell it the user
  if(ucnx.linksHaveBeenAdded()){
    QMessageBox msgBox(this);
    bool ok;
    msgBox.setText(tr("Following links has been added:\n") + ucnx.getAddedLinksText(ok));
    msgBox.setInformativeText(tr("Please edit them to fix vehicle type assignations and modifications."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
  }
  // Update views
  select("UnitConnector_view");
  select("UnitConnection_view");
  select("UnitLink_view");
}

void mdtClUnitEditor::editConnectorName()
{
  mdtClUnitConnection ucnx(database());
  QInputDialog inputDialog(this);
  mdtClUnitConnectorPkData pk;
  QVariant connectorName;

  // Get current data
  pk.id = currentData("UnitConnector_view", "Id_PK");
  if(pk.id.isNull()){
    return;
  }
  connectorName = currentData("UnitConnector_view", "UnitConnectorName");
  inputDialog.setLabelText(tr("Connector name:"));
  inputDialog.setTextValue(connectorName.toString());
  if(inputDialog.exec() != QDialog::Accepted){
    return;
  }
  connectorName = inputDialog.textValue().trimmed();
  if(connectorName.toString().isEmpty()){
    return;
  }
  // Update unit connector name in database
  if(!ucnx.updateUnitConnectorName(pk, connectorName)){
    pvLastError = ucnx.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("UnitConnector_view");
  select("UnitConnection_view");
  select("UnitLink_view");
}

void mdtClUnitEditor::removeConnectors()
{
  mdtClUnitConnection ucnx(database());
  mdtSqlTableWidget *widget;
  QMessageBox msgBox(this);
  mdtSqlTableSelection s;

  widget = sqlTableWidget("UnitConnector_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  msgBox.setText(tr("You are about to remove connectors from current unit. This will also remove related connections."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove connectors
  if(!ucnx.removeUnitConnectors(s)){
    pvLastError = ucnx.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("UnitConnector_view");
  select("UnitConnection_view");
  select("UnitLink_view");
}

void mdtClUnitEditor::viewLinkedConnectors()
{
  mdtClPathGraph graph(database());
  mdtClLinkedUnitConnectorInfoDialog dialog(this, database(), &graph);
  QVariant connectorId;

  // Get current unit connection ID
  connectorId = currentData("UnitConnector_view", "Id_PK");
  if(connectorId.isNull()){
    return;
  }
  // Load link list
  if(!graph.loadLinkList()){
    pvLastError = graph.lastError();
    displayLastError();
    return;
  }

  // Setup and show dialog
  dialog.setConnector(connectorId);
  dialog.exec();
}

void mdtClUnitEditor::addConnection()
{
  mdtClUnitConnectionDialog dialog(this, database(), mdtClUnitConnectionDialog::Add);
  mdtClUnitConnection ucnx(database());
  mdtClUnitConnectionKeyData key;
  mdtClUnitConnectionData data;

  key.setUnitId(currentUnitId());
  if(key.unitId().isNull()){
    return;
  }
  data.setKeyData(key);
  // Setup and show dialog
  dialog.setData(data, currentData("Unit_tbl", "Article_Id_FK"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add connection
  if(ucnx.addUnitConnection(dialog.data(), true).isNull()){
    pvLastError = ucnx.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  select("UnitConnection_view");
  select("UnitLink_view");
}

void mdtClUnitEditor::addArticleConnectionsBasedConnections()
{
  mdtClUnitConnection ucnx(database());
  mdtClUnitConnectionDialog dialog(this, database(), mdtClUnitConnectionDialog::Add);
  mdtClArticleConnectionSelectionDialog selectionDialog(this);
  mdtClArticleConnectionKeyData articleConnectionFk;
  QVariant articleId;
  mdtClUnitConnectionKeyData key;
  mdtClUnitConnectorKeyData unitConnectorFk;
  mdtClUnitConnectionData data;
  bool ok;

  articleId = currentData("Unit_tbl", "Article_Id_FK");
  if(articleId.isNull()){
    return;
  }
  // Set unit ID
  key.setUnitId(currentUnitId());
  if(key.unitId().isNull()){
    return;
  }
  // Let user select article connection
  if(!selectionDialog.select(database(), articleId, key.unitId(), mdtClArticleConnectionSelectionDialog::ArticleConnectorMembership_t::All, false)){
    pvLastError = selectionDialog.lastError();
    displayLastError();
    return;
  }
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  articleConnectionFk = selectionDialog.selectedArticleConnectionKey();
  Q_ASSERT(!articleConnectionFk.isNull());
  // Get unit connector key for given article connection
  unitConnectorFk = ucnx.getUnitConnectorKeyData(articleConnectionFk, key.unitId(), ok);
  if(!ok){
    pvLastError = ucnx.lastError();
    displayLastError();
    return;
  }
  // Update unit connection key and set it to data
  if(!unitConnectorFk.isNull()){
    key.setUnitConnectorFk(unitConnectorFk);
  }
  key.setArticleConnectionFk(articleConnectionFk);
  data.setKeyData(key);
  // Setup and show unit connection edition dialog
  dialog.setData(data, articleId);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add connection
  if(ucnx.addUnitConnection(dialog.data(), true).isNull()){
    pvLastError = ucnx.lastError();
    displayLastError();
    return;
  }
  // If some links has been added, we tell it the user
  if(ucnx.linksHaveBeenAdded()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Following links has been added:\n") + ucnx.getAddedLinksText(ok));
    msgBox.setInformativeText(tr("Please edit them to fix vehicle type assignations and modifications."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
  }
  // Update views
  select("UnitConnection_view");
  select("UnitLink_view");
}

void mdtClUnitEditor::editConnection()
{
  mdtSqlTableWidget *widget;
  mdtClUnitConnectionDialog dialog(this, database(), mdtClUnitConnectionDialog::Edit);
  mdtClUnitConnection ucnx(database());
  mdtClUnitConnectionPkData pk;
  mdtClUnitConnectionData data;
  bool ok;

  widget = sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != 0);
  // Get current unit connection data
  pk.id = widget->currentData("Id_PK");
  if(pk.isNull()){
    return;
  }
  data = ucnx.getUnitConnectionData(pk, ok);
  if(!ok){
    pvLastError = ucnx.lastError();
    displayLastError();
    return;
  }
  // Setup and show dialog
  dialog.setData(data, currentData("Unit_tbl", "Article_Id_FK"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Update in database
  if(!ucnx.updateUnitConnection(data.keyData().pk, dialog.data())){
    pvLastError = ucnx.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  select("UnitConnection_view");
  select("UnitLink_view");
}

void mdtClUnitEditor::setFunctionsFromOtherConnection()
{
  mdtClLinkedUnitConnectionInfoDialog dialog(this, database());
  mdtClUnit unit(0, database());
  mdtClPathGraph graph(database());
  QVariant connectionId;
  QList<QVariant> linkedConnectionsIdList;
  mdtClUnitConnectionData connectionData;
  bool ok;

  // Get current unit connection ID
  connectionId = currentData("UnitConnection_view", "Id_PK");
  if(connectionId.isNull()){
    return;
  }
  // Load link list and get linked connections
  if(!graph.loadLinkList()){
    pvLastError = graph.lastError();
    displayLastError();
    return;
  }
  linkedConnectionsIdList = graph.getLinkedConnectionIdList(connectionId, ok);
  if(!ok){
    pvLastError = graph.lastError();
    displayLastError();
    return;
  }
  // Get current connection data
  /**
   * \todo Adapt
  connectionData = unit.getConnectionData(connectionId, true, &ok);
  */
  if(!ok){
    pvLastError = unit.lastError();
    displayLastError();
  }
  // Setup and show dialog
  dialog.setConnections(connectionId, linkedConnectionsIdList);
  dialog.setSelectionModeEnabled(true);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Update functions
  /**
   * \todo Adapt
  connectionData.setValue("FunctionEN", dialog.functionStringEN());
  connectionData.setValue("FunctionFR", dialog.functionStringFR());
  connectionData.setValue("FunctionDE", dialog.functionStringDE());
  connectionData.setValue("FunctionIT", dialog.functionStringIT());
  if(!unit.editConnection(connectionId, connectionData)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  */
  // Update connections view
  select("UnitConnection_view");
}

void mdtClUnitEditor::removeConnections()
{
  mdtClUnitConnection ucnx(database());
  mdtSqlTableWidget *widget;
  mdtClUnit unit(this, database());
  QMessageBox msgBox;
  mdtSqlTableSelection s;
  QString linksMsg;
  bool ok;

  widget = sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("Id_PK");
  if(s.rowCount() < 1){
    return;
  }
  // Check that selected connections are not related to some links
  linksMsg = unit.toUnitRelatedLinksListStr(currentUnitId(), s.dataList("Id_PK"), &ok);
  if(!ok){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
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
  // Delete seleced unit connections
  if(!ucnx.removeUnitConnections(s)){
    pvLastError = ucnx.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  select("UnitConnection_view");
  select("UnitLink_view");
}

void mdtClUnitEditor::viewLinkedConnections()
{
  mdtClLinkedUnitConnectionInfoDialog dialog(this, database());
  mdtClPathGraph graph(database());
  QVariant connectionId;
  QList<QVariant> linkedConnectionsIdList;
  bool ok;

  // Get current unit connection ID
  connectionId = currentData("UnitConnection_view", "Id_PK");
  if(connectionId.isNull()){
    return;
  }
  // Load link list and get linked connections
  if(!graph.loadLinkList()){
    pvLastError = graph.lastError();
    displayLastError();
    return;
  }
  linkedConnectionsIdList = graph.getLinkedConnectionIdList(connectionId, ok);
  if(!ok){
    pvLastError = graph.lastError();
    displayLastError();
    return;
  }
  // Setup and show dialog
  dialog.setConnections(connectionId, linkedConnectionsIdList);
  dialog.exec();
}

void mdtClUnitEditor::addLink()
{
  mdtClUnitLinkDialog dialog(this, database());
  QVariant unitId;
  mdtClLink lnk(database());

  // Setup and show dialog
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  dialog.setWorkingOnVehicleTypeIdList(pvWorkingOnVehicleTypeIdList);
  dialog.setStartUnit(unitId);
  dialog.setLinkVersion(mdtClLinkVersion::currentVersion());
  dialog.setLinkModification(mdtClModification_t::New);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add link
  if(!lnk.addLink(dialog.linkData(), dialog.linkModificationKeyData(), dialog.selectedVehicleTypeList(), true)){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Update links view
  select("UnitLink_view");
}

void mdtClUnitEditor::addLinkModification()
{
  mdtClUnitLinkModificationDialog dialog(this, database());
  mdtClLink lnk(database());
  mdtSqlTableWidget *linkWidget;
  mdtClLinkModificationKeyData modificationKey;
  mdtClLinkPkData pk;

  linkWidget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(linkWidget != nullptr);
  // Get link PK
  pk.connectionStartId = linkWidget->currentData("UnitConnectionStart_Id_FK");
  pk.connectionEndId = linkWidget->currentData("UnitConnectionEnd_Id_FK");
  if(pk.isNull()){
    QMessageBox msgBox;
    msgBox.setText(tr("Please select a link."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
    return;
  }
  // Setup and show dialog
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  /// Update modification data
  modificationKey = dialog.linkModificationKeyData();
  modificationKey.setLinkFk(pk);
  // Add to database
  if(!lnk.addModification(modificationKey)){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Update links view
  select("UnitLink_view");
}

void mdtClUnitEditor::editLink()
{
  mdtSqlTableWidget *linkWidget;
  mdtClUnitLinkDialog dialog(this, database());
  QVariant unitId;
  QVariant var;
  mdtClLinkPkData pk;
  mdtClLinkData linkData;
  mdtClLinkVersionData linkVersionData;
  mdtClModificationPkData modificationPk;
  mdtClLinkModificationKeyData oldModificationKey;
  mdtClLink lnk(database());
  bool ok;

  linkWidget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(linkWidget != 0);
  // Get unit ID
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  // Get link PK
  pk.connectionStartId = linkWidget->currentData("UnitConnectionStart_Id_FK");
  pk.connectionEndId = linkWidget->currentData("UnitConnectionEnd_Id_FK");
  if(pk.isNull()){
    QMessageBox msgBox;
    msgBox.setText(tr("Please select a link."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
    return;
  }
  oldModificationKey.setLinkFk(pk);
  // Get link version and modification
  var = linkWidget->currentData("Version_FK");
  if(!var.isNull()){
    linkVersionData.setVersionPk(var);
    oldModificationKey.setLinkVersionFk(linkVersionData.pk());
  }
  var = linkWidget->currentData("Modification_Code_FK");
  if(!var.isNull()){
    modificationPk.code = var.toString();
    oldModificationKey.setModificationFk(modificationPk);
  }
  // Get current link data
  linkData = lnk.getLinkData(pk, ok);
  if(!ok){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Setup and show dialog
  dialog.setWorkingOnVehicleTypeIdList(pvWorkingOnVehicleTypeIdList);
  dialog.setLinkData(linkData);
  if(!linkVersionData.isNull()){
    dialog.setLinkVersion(linkVersionData);
  }
  if(!modificationPk.isNull()){
    dialog.setLinkModification(modificationPk);
  }
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Update link
  if(!lnk.updateLink(pk, dialog.linkData(), oldModificationKey, dialog.linkModificationKeyData(), dialog.selectedVehicleTypeList(), true)){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Update links view
  select("UnitLink_view");
}

void mdtClUnitEditor::removeLinks()
{
  mdtSqlTableWidget *widget;
  mdtClLink lnk(database());
  QMessageBox msgBox;
  mdtSqlTableSelection s;
  QSqlError sqlError;
  QStringList fields;
  QString infoText;

  widget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  fields << "UnitConnectionStart_Id_FK" << "UnitConnectionEnd_Id_FK";
  s = widget->currentSelection(fields);
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove links attached to current unit."));
  infoText = tr("This will also remove all modification history for selected links.");
  infoText += "\n\n" + tr("Do you want to continue ?");
  msgBox.setInformativeText(infoText);
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!lnk.removeLinks(s)){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  select("UnitLink_view");
}

void mdtClUnitEditor::viewPath()
{
  mdtClPathGraphDialog dialog(0, database());
  mdtSqlTableWidget *widget;
  QVariant startConnectionId;

  widget = sqlTableWidget("UnitLink_view");
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


/// \todo Add start and end vehicle type(s) selection to finish ...
void mdtClUnitEditor::connectConnectors()
{
  ///mdtClLink lnk(0, database());
  QString sql;
  QVariant startVehicleTypeId, endVehicleTypeId;
  QVariant startConnectorId, endConnectorId;
  QVariant startUnitId, endUnitId;
  mdtClConnectableCriteria connectableCriteria;
  mdtClConnectableConnectorDialog connectorSelectionDialog(this, database());
  ///bool ok;

  // Get current unit ID
  startUnitId = currentUnitId();
  if(startUnitId.isNull()){
    return;
  }
  // Check that vehicle type assignation was made
  if(rowCount("Unit_VehicleType_view", false) < 1){
    pvLastError.setError(tr("No vehicle type was assigned."), mdtError::Error);
    pvLastError.setInformativeText(tr("Please assign a vehicle type and try again."));
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnitEditor");
    pvLastError.commit();
    return;
  }
  // Select start connector
  sql = "SELECT * FROM UnitConnector_view WHERE Unit_Id_FK = " + startUnitId.toString();
  startConnectorId = selectUnitConnector(tr("Select start connector:"), sql);
  if(startConnectorId.isNull()){
    return;
  }
  // Select start vehicle type
  sql = "SELECT DISTINCT VehicleType_Id_FK, Type, SubType, SeriesNumber FROM Unit_VehicleType_view WHERE Unit_Id_FK = " + startUnitId.toString();
  startVehicleTypeId = selectVehicleType(tr("Select start vehicle type:"), sql);
  if(startVehicleTypeId.isNull()){
    return;
  }
  // Select end vehicle type
  sql = "SELECT DISTINCT VehicleType_Id_FK, Type, SubType, SeriesNumber FROM Unit_VehicleType_view";
  endVehicleTypeId = selectVehicleType(tr("Select end vehicle type:"), sql);
  if(endVehicleTypeId.isNull()){
    return;
  }
  // Select end unit
  sql = "SELECT * FROM Unit_view ";
  sql += " WHERE VehicleType_Id_PK = " + endVehicleTypeId.toString();
  endUnitId = selectUnit(tr("Select end unit:"), sql);
  if(endUnitId.isNull()){
    return;
  }
  // Select end connector
  connectorSelectionDialog.setStartConnector(startConnectorId);
  connectorSelectionDialog.setEndUnitId(endUnitId);
  if(connectorSelectionDialog.exec() != QDialog::Accepted){
    return;
  }
  endConnectorId = connectorSelectionDialog.endUnitConnectorId();
  connectableCriteria = connectorSelectionDialog.getConnectableCriteria();
  if(endConnectorId.isNull()){
    return;
  }
  // Make connection
  pvLastError.setError("NOT implemented yet..", mdtError::Error);
  MDT_ERROR_SET_SRC(pvLastError, "mdtClUnitEditor");
  pvLastError.commit();
  displayLastError();
  return;
  /**
  if(!lnk.connectByContactName(startConnectorId, endConnectorId, startVehicleTypeId, endVehicleTypeId, connectableCriteria)){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  */
  // Update view
  select("UnitLink_view");
}

QVariant mdtClUnitEditor::currentUnitId()
{
  return currentData("Unit_tbl", "Id_PK");
}

QVariant mdtClUnitEditor::selectVehicleType(const QString & message, const QString & sql)
{
  mdtSqlSelectionDialog selectionDialog;

  // Setup and show dialog
  selectionDialog.setMessage(message);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("VehicleType_Id_FK", true);
  ///selectionDialog.setHeaderData("ManufacturerArticleCode", tr("Manufacturer\nArticle code"));
  selectionDialog.addColumnToSortOrder("Type", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.addSelectionResultColumn("VehicleType_Id_FK");
  selectionDialog.resize(700, 400);
  selectionDialog.setWindowTitle(tr("Vehicle type selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtClUnitEditor::selectUnit(const QString & message, const QString & sql)
{
  mdtSqlSelectionDialog selectionDialog;

  // Setup and show dialog
  selectionDialog.setMessage(message);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Unit_Id_PK", true);
  selectionDialog.setColumnHidden("VehicleType_Id_PK", true);
  ///selectionDialog.setHeaderData("ManufacturerArticleCode", tr("Manufacturer\nArticle code"));
  selectionDialog.addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.addSelectionResultColumn("Unit_Id_PK");
  selectionDialog.resize(700, 400);
  selectionDialog.setWindowTitle(tr("Unit selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtClUnitEditor::selectUnitConnector(const QString & message, const QString & sql)
{
  mdtSqlSelectionDialog selectionDialog;

  // Setup and show dialog
  selectionDialog.setMessage(message);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_FK", true);
  selectionDialog.setColumnHidden("Connector_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnector_Id_FK", true);
  selectionDialog.setHeaderData("UnitConnectorName", tr("Unit\nConnector name"));
  selectionDialog.setHeaderData("ArticleConnectorName", tr("Article\nConnector name"));
  ///selectionDialog.setHeaderData("ManufacturerArticleCode", tr("Manufacturer\nArticle code"));
  selectionDialog.addColumnToSortOrder("UnitConnectorName", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(700, 400);
  selectionDialog.setWindowTitle(tr("Connector selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

bool mdtClUnitEditor::setupUnitTable()
{
  Ui::mdtClUnitEditor ue;
  std::shared_ptr<mdtSqlDataWidgetController> unitController;
  std::shared_ptr<mdtSqlDataWidgetController> articleController;
  mdtSqlRelationInfo relationInfo;
  ///QSqlTableModel *unitModel;
  ///QDataWidgetMapper *baseArticleMapper;
  ///QSqlTableModel *baseArticleModel;
  ///mdtSqlRelation *baseArticleRelation;

  // Setup main form widget
  ///ue.setupUi(mainSqlWidget());
  setMainTableUi<Ui::mdtClUnitEditor>(ue);
  connect(ue.pbSetBaseArticle, SIGNAL(clicked()), this, SLOT(setBaseArticle()));
  // Setup form
  if(!setMainTable("Unit_tbl", "Units", database())){
    return false;
  }
  /*
   * Setup base article widget mapping
   */
  // Get unit controller and add it a new controller for Article_tbl
  unitController = tableController<mdtSqlDataWidgetController>("Unit_tbl");
  Q_ASSERT(unitController);
  relationInfo.setChildTableName("Article_tbl");
  relationInfo.addRelation("Article_Id_FK", "Id_PK", false);
  if(!unitController->addChildController<mdtSqlDataWidgetController>(relationInfo, tr("Base article"))){
    pvLastError = unitController->lastError();
    return false;
  }
  // Get freshly added article controller and map widgets to it
  articleController = tableController<mdtSqlDataWidgetController>("Article_tbl");
  Q_ASSERT(articleController);
  
  qDebug() << unitController->tableName();
  qDebug() << articleController->tableName();
  
  articleController->addMapping(ue.lbl_Article_Id_FK, "Id_PK");
  articleController->addMapping(ue.lbl_ArticleCode, "ArticleCode");
  articleController->addMapping(ue.lbl_ArticleDesignationEN, "DesignationEN");
  articleController->addMapping(ue.lbl_ArticleDesignationFR, "DesignationFR");
  articleController->addMapping(ue.lbl_ArticleDesignationDE, "DesignationDE");
  articleController->addMapping(ue.lbl_ArticleDesignationIT, "DesignationIT");
  /**
  ///unitModel = model("Unit_tbl");
  unitModel = 0;
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
  baseArticleMapper->addMapping(ue.leArticleDesignationFR, baseArticleModel->fieldIndex("DesignationFR"));
  baseArticleMapper->addMapping(ue.leArticleDesignationDE, baseArticleModel->fieldIndex("DesignationDE"));
  baseArticleMapper->addMapping(ue.leArticleDesignationIT, baseArticleModel->fieldIndex("DesignationIT"));
  // Setup base article relation
  baseArticleRelation = new mdtSqlRelation(this);
  baseArticleRelation->setParentModel(unitModel);
  baseArticleRelation->setChildModel(baseArticleModel);
  if(!baseArticleRelation->addRelation("Article_Id_FK", "Id_PK", false)){
    return false;
  }
  ///connect(mainSqlWidget(), SIGNAL(currentRowChanged(int)), baseArticleRelation, SLOT(setParentCurrentIndex(int)));
  connect(baseArticleRelation, SIGNAL(childModelFilterApplied()), baseArticleMapper, SLOT(toFirst()));
  connect(baseArticleRelation, SIGNAL(childModelIsEmpty()), baseArticleMapper, SLOT(revert()));
  // Force a update
  ///mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());
  */

  return true;
}

bool mdtClUnitEditor::setupUnitComponentTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddComponent;
  QPushButton *pbRemoveComponents;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("UnitComponent_view");
  relationInfo.addRelation("Id_PK", "Unit_Id_PK", false);
  if(!addChildTable(relationInfo, tr("Components"))){
    return false;
  }
  /**
  if(!addChildTable("UnitComponent_view", tr("Components"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "UnitComponent_view", "Unit_Id_PK")){
    return false;
  }
  */
  widget = sqlTableWidget("UnitComponent_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Unit_Id_PK", true);
  widget->setColumnHidden("UnitComponent_Id_PK", true);
  widget->setColumnHidden("Article_Id_FK", true);
  widget->setColumnHidden("Article_Id_PK", true);
  // Set fields a user friendly name
  widget->setHeaderData("SchemaPosition", tr("Vehicle\nSchema\nposition"));
  widget->setHeaderData("ArticleCode", tr("Article code"));
  widget->setHeaderData("DesignationEN", tr("Designation\n(English)"));
  widget->setHeaderData("DesignationFR", tr("Designation\n(French)"));
  widget->setHeaderData("DesignationDE", tr("Designation\n(German)"));
  widget->setHeaderData("DesignationIT", tr("Designation\n(Italian)"));
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

bool mdtClUnitEditor::setupUnitConnectorTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddConnector;
  QPushButton *pbAddConnectorBasedConnector;
  QPushButton *pbAddArticleConnectorBasedConnector;
  QPushButton *pbEditConnectorName;
  QPushButton *pbRemoveConnectors;
  QPushButton *pbViewLinkedConnectors;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("UnitConnector_view");
  relationInfo.addRelation("Id_PK", "Unit_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Connectors"))){
    return false;
  }
  /**
  if(!addChildTable("UnitConnector_view", tr("Connectors"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "UnitConnector_view", "Unit_Id_FK")){
    return false;
  }
  */
  widget = sqlTableWidget("UnitConnector_view");
  Q_ASSERT(widget != 0);
  // Hide relation fields and PK
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("Unit_Id_FK", true);
  widget->setColumnHidden("Connector_Id_FK", true);
  widget->setColumnHidden("ArticleConnector_Id_FK", true);
  // Give fields a user friendly name
  widget->setHeaderData("UnitConnectorName", tr("Unit\nconnector"));
  widget->setHeaderData("ArticleConnectorName", tr("Article\nconnector"));
  // Add edition buttons
  pbAddConnector = new QPushButton(tr("Add free connector ..."));
  pbAddConnector->setIcon(QIcon::fromTheme("document-new"));
  pbAddConnectorBasedConnector = new QPushButton(tr("Add connector ..."));
  pbAddConnectorBasedConnector->setIcon(QIcon::fromTheme("document-new"));
  pbAddArticleConnectorBasedConnector = new QPushButton(tr("Add art. connector ..."));
  pbAddArticleConnectorBasedConnector->setIcon(QIcon::fromTheme("document-new"));
  pbEditConnectorName = new QPushButton(tr("Edit name"));
  pbRemoveConnectors = new QPushButton(tr("Rem. connector"));
  pbRemoveConnectors->setIcon(QIcon::fromTheme("edit-delete"));
  connect(pbAddConnector, SIGNAL(clicked()), this, SLOT(addConnector()));
  connect(pbAddConnectorBasedConnector, SIGNAL(clicked()), this, SLOT(addConnectorBasedConnector()));
  connect(pbAddArticleConnectorBasedConnector, SIGNAL(clicked()), this, SLOT(addArticleConnectorBasedConnector()));
  connect(pbEditConnectorName, SIGNAL(clicked()), this, SLOT(editConnectorName()));
  connect(pbRemoveConnectors, SIGNAL(clicked()), this, SLOT(removeConnectors()));
  widget->addWidgetToLocalBar(pbAddConnector);
  widget->addWidgetToLocalBar(pbAddConnectorBasedConnector);
  widget->addWidgetToLocalBar(pbAddArticleConnectorBasedConnector);
  widget->addWidgetToLocalBar(pbEditConnectorName);
  widget->addWidgetToLocalBar(pbRemoveConnectors);
  // View linked connectors button
  pbViewLinkedConnectors = new QPushButton(tr("Linked connectors"));
  connect(pbViewLinkedConnectors, SIGNAL(clicked()), this, SLOT(viewLinkedConnectors()));
  widget->addWidgetToLocalBar(pbViewLinkedConnectors);
  // Stretch
  widget->addStretchToLocalBar();
  // Set some attributes on table view
  widget->resizeViewToContents();

  return true;
}


bool mdtClUnitEditor::setupUnitConnectionTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddConnection;
  QPushButton *pbAddArticleConnectionBasedConnection;
  QPushButton *pbEditConnection;
  QPushButton *pbCopyFunctions;
  QPushButton *pbRemoveConnection;
  QPushButton *pbViewLinkedConnections;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("UnitConnection_view");
  relationInfo.addRelation("Id_PK", "Unit_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Connections"))){
    return false;
  }
  widget = sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != nullptr);
  // Add add connection button
  pbAddConnection = new QPushButton(tr("Add connection ..."));
  pbAddConnection->setIcon(QIcon::fromTheme("document-new"));
  connect(pbAddConnection, SIGNAL(clicked()), this, SLOT(addConnection()));
  widget->addWidgetToLocalBar(pbAddConnection);
  // Add add art. connection button
  pbAddArticleConnectionBasedConnection = new QPushButton(tr("Add art. connections ..."));
  pbAddArticleConnectionBasedConnection->setIcon(QIcon::fromTheme("document-new"));
  connect(pbAddArticleConnectionBasedConnection, SIGNAL(clicked()), this, SLOT(addArticleConnectionsBasedConnections()));
  widget->addWidgetToLocalBar(pbAddArticleConnectionBasedConnection);
  // Add edit connection button
  pbEditConnection = new QPushButton(tr("Edit connection ..."));
  connect(pbEditConnection, SIGNAL(clicked()), this, SLOT(editConnection()));
  widget->addWidgetToLocalBar(pbEditConnection);
  // Add remove connection button
  pbRemoveConnection = new QPushButton(tr("Remove connections"));
  pbRemoveConnection->setIcon(QIcon::fromTheme("edit-delete"));
  connect(pbRemoveConnection, SIGNAL(clicked()), this, SLOT(removeConnections()));
  widget->addWidgetToLocalBar(pbRemoveConnection);
  // Add copy functions connection button
  pbCopyFunctions = new QPushButton(tr("Set functions from ..."));
  connect(pbCopyFunctions, SIGNAL(clicked()), this, SLOT(setFunctionsFromOtherConnection()));
  widget->addWidgetToLocalBar(pbCopyFunctions);
  // View linked connections button
  pbViewLinkedConnections = new QPushButton(tr("Linked connections"));
  connect(pbViewLinkedConnections, SIGNAL(clicked()), this, SLOT(viewLinkedConnections()));
  widget->addWidgetToLocalBar(pbViewLinkedConnections);
  // Add stretch
  widget->addStretchToLocalBar();
  // On double click, we edit connection
  Q_ASSERT(widget->tableView() != 0);
  connect(widget->tableView(), SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(editConnection()));
  // Hide relation fields and PK
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("UnitConnector_Id_FK", true);
  widget->setColumnHidden("Unit_Id_FK", true);
  widget->setColumnHidden("ArticleConnection_Id_FK", true);
  widget->setColumnHidden("ConnectionType_Code_FK", true);
  // Give fields a user friendly name
  widget->setHeaderData("SchemaPage", tr("Vehicle\nSchema\npage"));
  widget->setHeaderData("SignalName", tr("Signal name"));
  widget->setHeaderData("SwAddress", tr("SW address"));
  widget->setHeaderData("UnitConnectorName", tr("Unit\nconnector"));
  widget->setHeaderData("UnitContactName", tr("Unit\ncontact"));
  widget->setHeaderData("UnitConnectionResistance", tr("Unit\nconnection\nresistance"));
  widget->setHeaderData("ArticleConnectorName", tr("Article\nconnector"));
  widget->setHeaderData("ArticleContactName", tr("Article\ncontact"));
  widget->setHeaderData("IoType", tr("I/O type"));
  updateUnitConnectionTable();
  // Enable sorting
  widget->addColumnToSortOrder("UnitConnectorName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("UnitContactName", Qt::AscendingOrder);
  widget->sort();
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();

  return true;
}

void mdtClUnitEditor::updateUnitConnectionTable(const QLocale & locale)
{
  auto *widget = sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != nullptr);

  switch(locale.language()){
    case QLocale::French:
      widget->setColumnHidden("UnitFunctionEN", true);
      widget->setColumnHidden("UnitFunctionDE", true);
      widget->setColumnHidden("UnitFunctionIT", true);
      widget->setHeaderData("UnitFunctionFR", tr("Unit\nconnection\nfunction"));
      widget->setColumnHidden("ArticleFunctionEN", true);
      widget->setColumnHidden("ArticleFunctionDE", true);
      widget->setColumnHidden("ArticleFunctionIT", true);
      widget->setHeaderData("ArticleFunctionFR", tr("Article\nconnection\nfunction"));
      widget->setColumnHidden("ConnectionTypeEN", true);
      widget->setColumnHidden("ConnectionTypeDE", true);
      widget->setColumnHidden("ConnectionTypeIT", true);
      widget->setHeaderData("ConnectionTypeFR", tr("Connection\ntype"));
      break;
    case QLocale::German:
      widget->setColumnHidden("UnitFunctionEN", true);
      widget->setColumnHidden("UnitFunctionFR", true);
      widget->setColumnHidden("UnitFunctionIT", true);
      widget->setHeaderData("UnitFunctionDE", tr("Unit\nconnection\nfunction"));
      widget->setColumnHidden("ArticleFunctionEN", true);
      widget->setColumnHidden("ArticleFunctionFR", true);
      widget->setColumnHidden("ArticleFunctionIT", true);
      widget->setHeaderData("ArticleFunctionDE", tr("Article\nconnection\nfunction"));
      widget->setColumnHidden("ConnectionTypeEN", true);
      widget->setColumnHidden("ConnectionTypeFR", true);
      widget->setColumnHidden("ConnectionTypeIT", true);
      widget->setHeaderData("ConnectionTypeDE", tr("Connection\ntype"));
      break;
    case QLocale::Italian:
      widget->setColumnHidden("UnitFunctionEN", true);
      widget->setColumnHidden("UnitFunctionDE", true);
      widget->setColumnHidden("UnitFunctionFR", true);
      widget->setHeaderData("UnitFunctionIT", tr("Unit\nconnection\nfunction"));
      widget->setColumnHidden("ArticleFunctionEN", true);
      widget->setColumnHidden("ArticleFunctionDE", true);
      widget->setColumnHidden("ArticleFunctionFR", true);
      widget->setHeaderData("ArticleFunctionIT", tr("Article\nconnection\nfunction"));
      widget->setColumnHidden("ConnectionTypeEN", true);
      widget->setColumnHidden("ConnectionTypeDE", true);
      widget->setColumnHidden("ConnectionTypeFR", true);
      widget->setHeaderData("ConnectionTypeIT", tr("Connection\ntype"));
      break;
    default:
      widget->setColumnHidden("UnitFunctionFR", true);
      widget->setColumnHidden("UnitFunctionDE", true);
      widget->setColumnHidden("UnitFunctionIT", true);
      widget->setHeaderData("UnitFunctionEN", tr("Unit\nconnection\nfunction"));
      widget->setColumnHidden("ArticleFunctionFR", true);
      widget->setColumnHidden("ArticleFunctionDE", true);
      widget->setColumnHidden("ArticleFunctionIT", true);
      widget->setHeaderData("ArticleFunctionEN", tr("Article\nconnection\nfunction"));
      widget->setColumnHidden("ConnectionTypeFR", true);
      widget->setColumnHidden("ConnectionTypeDE", true);
      widget->setColumnHidden("ConnectionTypeIT", true);
      widget->setHeaderData("ConnectionTypeEN", tr("Connection\ntype"));
  }
}

bool mdtClUnitEditor::setupUnitLinkTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddLink;
  QPushButton *pbEditLink;
  QPushButton *pbRemoveLinks;
  QPushButton *pbViewPath;
  QPushButton *pbConnectConnectors;
  QPushButton *pb;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("UnitLink_view");
  relationInfo.addRelation("Id_PK", "StartUnit_Id_FK", false);
  relationInfo.addRelation("Id_PK", "EndUnit_Id_FK", false, "OR");
  if(!addChildTable(relationInfo, tr("Links"))){
    return false;
  }
  widget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(widget != 0);
  // Add link button
  pbAddLink = new QPushButton(tr("Add link ..."));
  pbAddLink->setIcon(QIcon::fromTheme("document-new"));
  connect(pbAddLink, SIGNAL(clicked()), this, SLOT(addLink()));
  widget->addWidgetToLocalBar(pbAddLink);
  // Add link modification button
  pb = new QPushButton(tr("Add modification ..."));
  connect(pb, SIGNAL(clicked()), this, SLOT(addLinkModification()));
  widget->addWidgetToLocalBar(pb);
  // Edit link button
  pbEditLink = new QPushButton(tr("Edit link"));
  connect(pbEditLink, SIGNAL(clicked()), this, SLOT(editLink()));
  widget->addWidgetToLocalBar(pbEditLink);
  // Remove links button
  pbRemoveLinks = new QPushButton(tr("Remove links"));
  pbRemoveLinks->setIcon(QIcon::fromTheme("edit-delete"));
  connect(pbRemoveLinks, SIGNAL(clicked()), this, SLOT(removeLinks()));
  // View path button
  widget->addWidgetToLocalBar(pbRemoveLinks);
  pbViewPath = new QPushButton(tr("View path"));
  connect(pbViewPath, SIGNAL(clicked()), this, SLOT(viewPath()));
  widget->addWidgetToLocalBar(pbViewPath);
  // Connect connectors button
  pbConnectConnectors = new QPushButton(tr("Connect connectors ..."));
  connect(pbConnectConnectors, SIGNAL(clicked()), this, SLOT(connectConnectors()));
  widget->addWidgetToLocalBar(pbConnectConnectors);
  // Strech
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
  widget->setColumnHidden("LinkBeam_Id_FK", true);
  widget->setColumnHidden("ModificationSortOrder", true);
  widget->setColumnHidden("StartUnitConnector_Id_FK", true);
  widget->setColumnHidden("EndUnitConnector_Id_FK", true);
  widget->setColumnHidden("Version_FK", true);
  widget->setColumnHidden("Modification_Code_FK", true);
  // Give fields a user friendly name
  widget->setHeaderData("StartSchemaPosition", tr("Start\nschema pos."));
  widget->setHeaderData("StartAlias", tr("Start\nalias"));
  widget->setHeaderData("StartUnitConnectorName", tr("Start\nconnector"));
  widget->setHeaderData("StartUnitContactName", tr("Start\ncontact"));
  widget->setHeaderData("StartUnitConnectionResistance", tr("Start\nconnection\nresistance"));
  widget->setHeaderData("EndSchemaPosition", tr("End\nschema pos."));
  widget->setHeaderData("EndAlias", tr("End\nalias"));
  widget->setHeaderData("EndUnitConnectorName", tr("End\nconnector"));
  widget->setHeaderData("EndUnitContactName", tr("End\ncontact"));
  widget->setHeaderData("EndUnitConnectionResistance", tr("End\nconnection\nresistance"));
  widget->setHeaderData("SinceVersion", tr("Since\nversion"));
  widget->setHeaderData("LinkTypeNameEN", tr("Link type"));
  widget->setHeaderData("Length", tr("Link\nlength"));
  widget->setHeaderData("Resistance", tr("Link\nresistance"));
  widget->setHeaderData("LinkDirectionPictureAscii", tr("Direction"));
  widget->setHeaderData("StartSchemaPage", tr("Start\nschema\npage"));
  widget->setHeaderData("EndSchemaPage", tr("End\nschema\npage"));
  widget->setHeaderData("StartFunctionEN", tr("Start\nfunction (ENG)"));
  widget->setHeaderData("EndFunctionEN", tr("End\nfunction (ENG)"));
  widget->setHeaderData("StartSignalName", tr("Start\nsignal"));
  widget->setHeaderData("EndSignalName", tr("End\nsignal"));
  widget->setHeaderData("StartSwAddress", tr("Start\nSW address"));
  widget->setHeaderData("EndSwAddress", tr("End\nSW address"));
  // Setup sorting
  widget->addColumnToSortOrder("Identification", Qt::AscendingOrder);
  widget->addColumnToSortOrder("ModificationSortOrder", Qt::AscendingOrder);
  widget->addColumnToSortOrder("StartSchemaPosition", Qt::AscendingOrder);
  widget->addColumnToSortOrder("StartUnitConnectorName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("StartUnitContactName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("EndSchemaPosition", Qt::AscendingOrder);
  widget->addColumnToSortOrder("EndUnitConnectorName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("EndUnitContactName", Qt::AscendingOrder);
  widget->sort();
  // Setup text alignments
  widget->addTextAlignmentFormat("Version", Qt::AlignCenter);
  widget->addTextAlignmentFormat("StartSchemaPosition", Qt::AlignCenter);
  widget->addTextAlignmentFormat("StartAlias", Qt::AlignCenter);
  widget->addTextAlignmentFormat("StartUnitConnectorName", Qt::AlignCenter);
  widget->addTextAlignmentFormat("StartUnitContactName", Qt::AlignCenter);
  widget->addTextAlignmentFormat("StartUnitConnectionResistance", Qt::AlignCenter);
  widget->addTextAlignmentFormat("EndSchemaPosition", Qt::AlignCenter);
  widget->addTextAlignmentFormat("EndAlias", Qt::AlignCenter);
  widget->addTextAlignmentFormat("EndUnitConnectorName", Qt::AlignCenter);
  widget->addTextAlignmentFormat("EndUnitContactName", Qt::AlignCenter);
  widget->addTextAlignmentFormat("EndUnitConnectionResistance", Qt::AlignCenter);

  // Setup conditionnal baground colors
  widget->setBackgroundFromatKeyDataColumn("Modification_Code_FK");
  widget->addBackgroundFromat("NEW", QBrush(Qt::red));
  widget->addBackgroundFromat("REM", QBrush(Qt::yellow));
  widget->addBackgroundFromat("MODREM", QBrush(QColor(170, 255, 255)));
  widget->addBackgroundFromat("MODNEW", QBrush(QColor(255, 170, 120)));
  widget->addBackgroundFromat("MOD", QBrush(QColor(85, 170, 255)));
  
  // Set some attributes on table view
  widget->resizeViewToContents();
  // Update regarding locale
  updateUnitLinkTable();

  return true;
}

void mdtClUnitEditor::updateUnitLinkTable(const QLocale & locale)
{
  auto *widget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(widget != nullptr);

  switch(locale.language()){
    case QLocale::French:
      widget->setColumnHidden("ModificationEN", true);
      widget->setColumnHidden("ModificationDE", true);
      widget->setColumnHidden("ModificationIT", true);
      widget->setHeaderData("ModificationFR", tr("Modification\ntype"));
      widget->setColumnHidden("ModificationTextEN", true);
      widget->setColumnHidden("ModificationTextDE", true);
      widget->setColumnHidden("ModificationTextIT", true);
      widget->setHeaderData("ModificationTextFR", tr("Modification\ntext"));
      break;
    case QLocale::German:
      widget->setColumnHidden("ModificationEN", true);
      widget->setColumnHidden("ModificationFR", true);
      widget->setColumnHidden("ModificationIT", true);
      widget->setHeaderData("ModificationDE", tr("Modification\ntype"));
      widget->setColumnHidden("ModificationTextEN", true);
      widget->setColumnHidden("ModificationTextFR", true);
      widget->setColumnHidden("ModificationTextIT", true);
      widget->setHeaderData("ModificationTextDE", tr("Modification\ntext"));
      break;
    case QLocale::Italian:
      widget->setColumnHidden("ModificationEN", true);
      widget->setColumnHidden("ModificationDE", true);
      widget->setColumnHidden("ModificationFR", true);
      widget->setHeaderData("ModificationIT", tr("Modification\ntype"));
      widget->setColumnHidden("ModificationTextEN", true);
      widget->setColumnHidden("ModificationTextDE", true);
      widget->setColumnHidden("ModificationTextFR", true);
      widget->setHeaderData("ModificationTextIT", tr("Modification\ntext"));
      break;
    default:
      widget->setColumnHidden("ModificationFR", true);
      widget->setColumnHidden("ModificationDE", true);
      widget->setColumnHidden("ModificationIT", true);
      widget->setHeaderData("ModificationEN", tr("Modification\ntype"));
      widget->setColumnHidden("ModificationTextFR", true);
      widget->setColumnHidden("ModificationTextDE", true);
      widget->setColumnHidden("ModificationTextIT", true);
      widget->setHeaderData("ModificationTextEN", tr("Modification\ntext"));
  }
}

bool mdtClUnitEditor::setupVehicleTable()
{
  QPushButton *pbAddVehicle;
  QPushButton *pbRemoveVehicle;
  mdtSqlTableWidget *widget;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("Unit_VehicleType_view");
  relationInfo.addRelation("Id_PK", "Unit_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Vehicles"))){
    return false;
  }
  /**
  // Add vehicle type table
  if(!addChildTable("Unit_VehicleType_view", tr("Vehicles"), database())){
    return false;
  }
  // Setup Unit <-> VehicleType relation
  if(!addRelation("Id_PK", "Unit_VehicleType_view", "Unit_Id_FK")){
    return false;
  }
  */
  // Get widget to continue setup
  widget = sqlTableWidget("Unit_VehicleType_view");
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
  pbAddVehicle->setIcon(QIcon::fromTheme("list-add"));
  widget->addWidgetToLocalBar(pbAddVehicle);
  connect(pbAddVehicle, SIGNAL(clicked()), this, SLOT(assignVehicle()));
  // Remove vehicle button
  pbRemoveVehicle = new QPushButton(tr("Remove vehicle"));
  pbRemoveVehicle->setIcon(QIcon::fromTheme("list-remove"));
  widget->addWidgetToLocalBar(pbRemoveVehicle);
  connect(pbRemoveVehicle, SIGNAL(clicked()), this, SLOT(removeVehicleAssignation()));
  widget->addStretchToLocalBar();

  return true;
}
