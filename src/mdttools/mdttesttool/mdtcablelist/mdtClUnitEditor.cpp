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
#include "mdtClUnitEditor.h"
#include "mdtClUnit.h"
#include "mdtClLink.h"
#include "ui_mdtClUnitEditor.h"
///#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
///#include "mdtSqlRelation.h"
#include "mdtAbstractSqlTableController.h"
#include "mdtSqlDataWidgetController.h"
#include "mdtSqlRelationInfo.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtError.h"
#include "mdtClUnitVehicleType.h"
#include "mdtClUnitConnectionDialog.h"
#include "mdtClLinkData.h"
#include "mdtClUnitLinkDialog.h"
#include "mdtClPathGraph.h"
#include "mdtClPathGraphDialog.h"
#include "mdtClLinkedUnitConnectionInfoDialog.h"
#include "mdtClLinkedUnitConnectorInfoDialog.h"
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

mdtClUnitEditor::~mdtClUnitEditor()
{
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

bool mdtClUnitEditor::setWorkingOnVehicleTypeIdList(const QList<QVariant> & vtIdList)
{
  QSqlTableModel *m;
  QString sql;
  int i;

  pvWorkingOnVehicleTypeIdList = vtIdList;
  // Get main table model
  ///m = model("Unit_tbl");
  m = 0;
  if(m == 0){
    pvLastError.setError(tr("Cannot set vehicle type filter because model was not set on 'Unit_tbl'."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnitEditor");
    pvLastError.commit();
    return false;
  }
  // Build the WHERE clause for the filter
  if(pvWorkingOnVehicleTypeIdList.isEmpty()){
    m->setFilter("");
    return true;
  }
  Q_ASSERT(pvWorkingOnVehicleTypeIdList.size() > 0);
  sql = "Id_PK IN (";
  sql += "SELECT Unit_Id_FK FROM VehicleType_Unit_tbl WHERE VehicleType_Id_FK = " + pvWorkingOnVehicleTypeIdList.at(0).toString();
  for(i = 1; i < pvWorkingOnVehicleTypeIdList.size(); ++i){
    sql += " OR VehicleType_Id_FK = " + pvWorkingOnVehicleTypeIdList.at(i).toString();
  }
  sql += ")";

  m->setFilter(sql);

  return true;
}

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
  mdtSqlSelectionDialog selectionDialog;
  mdtSqlTableSelection s;
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
  // Setup article selection dialog and show it to user
  sql = "SELECT * FROM Article_tbl;";
  selectionDialog.setMessage("Please select article");
  selectionDialog.setQuery(sql, database(), false);
  ///selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setHeaderData("ArticleCode", tr("Article code\n(internal)"));
  selectionDialog.addColumnToSortOrder("ArticleCode", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("DesignationEN", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(800, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("Id_PK");
  Q_ASSERT(s.rowCount() == 1);
  if(!setCurrentData("Unit_tbl", "Article_Id_FK", s.data(0, "Id_PK"))){
    return;
  }
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
  QVariant unitId;
  QVariant connectorName;
  mdtClUnit unit(this, database());
  mdtClUnitConnectorData data;
  QInputDialog dialog;

  if(!data.setup(database(), false, false)){
    pvLastError = data.lastError();
    displayLastError();
    return;
  }
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
  data.setValue("Unit_Id_FK", unitId);
  data.setValue("Name", connectorName);
  if(!unit.addConnector(data)){
    pvLastError = unit.lastError();
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
  mdtClUnit unit(this, database());
  mdtClUnitConnectorData connectorData;
  QVariant unitId;
  QVariant baseConnectorId;
  QVariant connectorName;
  QList<QVariant> contactList;
  QInputDialog dialog;

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
  if(contactList.isEmpty()){
    return;
  }
  // Setup unit connector data
  if(!connectorData.setup(database(), true, false)){
    pvLastError = connectorData.lastError();
    displayLastError();
    return;
  }
  connectorData.setValue("Unit_Id_FK", unitId);
  connectorData.setValue("Connector_Id_FK", baseConnectorId);
  connectorData.setValue("Name", connectorName);
  // Add contacts to unit connector data
  if(!unit.addConnectionDataListFromConnectorContactIdList(connectorData, contactList)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Add connector
  if(!unit.addConnector(connectorData)){
    pvLastError = unit.lastError();
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
  QVariant unitId;
  QVariant connectorName;
  mdtClUnitConnectorData connectorData;
  QVariant articleConnectorId;
  QList<QVariant> articleConnectionIdList;
  mdtClUnit unit(0, database());
  QInputDialog dialog;

  // Setup data
  if(!connectorData.setup(database(), true, true)){
    pvLastError = connectorData.lastError();
    displayLastError();
    return;
  }
  // Get unit ID
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  connectorData.setValue("Unit_Id_FK", unitId);
  // Let user choose a article connector
  articleConnectorId = selectArticleConnector();
  if(articleConnectorId.isNull()){
    return;
  }
  // Add connector data
  if(!unit.addArticleConnectorData(connectorData, articleConnectorId, true)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Set connector name
  connectorName = connectorData.value("Name");
  dialog.setLabelText(tr("Connector name:"));
  dialog.setTextValue(connectorName.toString());
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  connectorName = dialog.textValue().trimmed();
  if(connectorName.isNull()){
    return;
  }
  connectorData.setValue("Name", connectorName);
  // Select article connections to use
  articleConnectionIdList = selectByArticleConnectorIdArticleConnectionIdList(articleConnectorId, unitId);
  if(articleConnectionIdList.isEmpty()){
    return;
  }
  // Add connections
  if(!unit.addConnectionDataListFromArticleConnectionIdList(connectorData, articleConnectionIdList, true)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Add connector
  if(!unit.addConnector(connectorData)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("UnitConnector_view");
  select("UnitConnection_view");
  select("UnitLink_view");
}

void mdtClUnitEditor::editConnectorName()
{
  QVariant unitConnectorId;
  QVariant connectorName;
  QString str;
  QInputDialog dialog;
  mdtClUnit unit(this, database());

  // Get current data
  unitConnectorId = currentData("UnitConnector_view", "Id_PK");
  if(unitConnectorId.isNull()){
    return;
  }
  connectorName = currentData("UnitConnector_view", "UnitConnectorName");
  // Let user set connector name
  dialog.setLabelText(tr("Connector name:"));
  dialog.setTextValue(connectorName.toString());
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  str = dialog.textValue().trimmed();
  if(!str.isEmpty()){
    connectorName = str;
  }
  // Edit connector name
  if(!unit.editConnectorName(unitConnectorId, connectorName)){
    pvLastError = unit.lastError();
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
  mdtSqlTableWidget *widget;
  mdtClUnit unit(this, database());
  QMessageBox msgBox;
  ///QModelIndexList indexes;
  mdtSqlTableSelection s;

  widget = sqlTableWidget("UnitConnector_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  /**
  indexes = widget->indexListOfSelectedRows("Id_PK");
  if(indexes.size() < 1){
    return;
  }
  */
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove connectors from current unit. This will also remove related connections."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove connectors
  if(!unit.removeConnectors(s)){
    pvLastError = unit.lastError();
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
  mdtClUnitConnectionDialog dialog(0, database(), mdtClUnitConnectionDialog::Add);
  mdtClUnit unit(this, database());
  mdtClUnitConnectionData data;
  QVariant unitId;

  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  // Setup unit connection data
  if(!data.setup(database(), true)){
    pvLastError = data.lastError();
    displayLastError();
    return;
  }
  data.setValue("Unit_Id_FK", unitId);
  // Setup and show dialog
  dialog.setData(data, currentData("Unit_tbl", "Article_Id_FK"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add connection
  if(!unit.addConnection(dialog.data())){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  select("UnitConnection_view");
  select("UnitLink_view");
}

void mdtClUnitEditor::addArticleConnectionsBasedConnections()
{
  mdtClUnit unit(this, database());
  mdtClUnitConnectionData data;
  QVariant unitId;
  QVariant unitConnectorId;
  QVariant articleId;
  QList<QVariant> articleConnectionIdList;
  QList<mdtClUnitConnectionData> unitConnectionDataList;
  bool ok;

  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  articleId = currentData("Unit_tbl", "Article_Id_FK");
  if(articleId.isNull()){
    return;
  }
  // Setup unit connection data
  if(!data.setup(database(), true)){
    pvLastError = data.lastError();
    displayLastError();
    return;
  }
  data.setValue("Unit_Id_FK", unitId);
  // Let user select article connections
  articleConnectionIdList = selectByArticleIdArticleConnectionIdList(articleId, unitId, true);
  if(articleConnectionIdList.isEmpty()){
    return;
  }
  // Get unit connection data
  unitConnectionDataList = unit.getUnitConnectionDataListFromArticleConnectionIdList(unitId /**, QVariant()*/ , articleConnectionIdList, true, &ok);
  if(!ok){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Add connections
  if(!unit.addConnections(unitConnectionDataList)){
    pvLastError = data.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("UnitConnection_view");
  select("UnitLink_view");
}

void mdtClUnitEditor::editConnection()
{
  mdtSqlTableWidget *widget;
  mdtClUnitConnectionDialog dialog(0, database(), mdtClUnitConnectionDialog::Edit);
  mdtClUnit unit(this, database());
  QVariant connectionId;
  mdtClUnitConnectionData data;
  bool ok;

  widget = sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != 0);

  // Get current unit connection data
  connectionId = widget->currentData("UnitConnection_Id_PK");
  if(connectionId.isNull()){
    return;
  }
  data = unit.getConnectionData(connectionId, true, &ok);
  if(!ok){
    pvLastError = unit.lastError();
    displayLastError();
  }
  // Setup and show dialog
  dialog.setData(data, currentData("Unit_tbl", "Article_Id_FK"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Edit connection
  if(!unit.editConnection(connectionId, dialog.data())){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  select("UnitConnection_view");
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
  connectionId = currentData("UnitConnection_view", "UnitConnection_Id_PK");
  if(connectionId.isNull()){
    return;
  }
  // Load link list and get linked connections
  if(!graph.loadLinkList()){
    pvLastError = graph.lastError();
    displayLastError();
    return;
  }
  linkedConnectionsIdList = graph.getLinkedConnectionIdList(connectionId);
  // Get current connection data
  connectionData = unit.getConnectionData(connectionId, true, &ok);
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
  connectionData.setValue("FunctionEN", dialog.functionStringEN());
  connectionData.setValue("FunctionFR", dialog.functionStringFR());
  connectionData.setValue("FunctionDE", dialog.functionStringDE());
  connectionData.setValue("FunctionIT", dialog.functionStringIT());
  if(!unit.editConnection(connectionId, connectionData)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections view
  select("UnitConnection_view");
}

void mdtClUnitEditor::removeConnections()
{
  mdtSqlTableWidget *widget;
  mdtClUnit unit(this, database());
  QMessageBox msgBox;
  ///QModelIndexList indexes;
  mdtSqlTableSelection s;
  QString linksMsg;
  bool ok;

  widget = sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("UnitConnection_Id_PK");
  if(s.rowCount() < 1){
    return;
  }
  /**
  indexes = widget->indexListOfSelectedRows("UnitConnection_Id_PK");
  if(indexes.size() < 1){
    return;
  }
  */
  // Check that selected connections are not related to some links
  ///linksMsg = unit.toUnitRelatedLinksListStr(currentUnitId(), indexes, &ok);
  linksMsg = unit.toUnitRelatedLinksListStr(currentUnitId(), s.dataList("UnitConnection_Id_PK"), &ok);
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
  // Delete seleced rows
  if(!unit.removeConnections(s)){
    pvLastError = unit.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  select("UnitConnection_view");
}

void mdtClUnitEditor::viewLinkedConnections()
{
  mdtClLinkedUnitConnectionInfoDialog dialog(this, database());
  mdtClPathGraph graph(database());
  QVariant connectionId;
  QList<QVariant> linkedConnectionsIdList;

  // Get current unit connection ID
  connectionId = currentData("UnitConnection_view", "UnitConnection_Id_PK");
  if(connectionId.isNull()){
    return;
  }
  // Load link list and get linked connections
  if(!graph.loadLinkList()){
    pvLastError = graph.lastError();
    displayLastError();
    return;
  }
  linkedConnectionsIdList = graph.getLinkedConnectionIdList(connectionId);
  // Setup and show dialog
  dialog.setConnections(connectionId, linkedConnectionsIdList);
  dialog.exec();
}

void mdtClUnitEditor::addLink()
{
  mdtClUnitLinkDialog dialog(0, database());
  QVariant unitId;
  mdtClLink lnk(0, database());

  // Setup and show dialog
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  dialog.setWorkingOnVehicleTypeIdList(pvWorkingOnVehicleTypeIdList);
  dialog.setStartUnit(unitId);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add link
  if(!lnk.addLink(dialog.linkData())){
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
  mdtClUnitLinkDialog dialog(0, database());
  QVariant unitId, startConnectionId, endConnectionId;
  mdtClLinkData linkData;
  mdtClLink lnk(0, database());
  bool ok;

  linkWidget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(linkWidget != 0);
  // Get unit ID
  unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
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
  linkData = lnk.getLinkData(startConnectionId, endConnectionId,true, true, &ok);
  if(!ok){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Setup and show dialog
  dialog.setWorkingOnVehicleTypeIdList(pvWorkingOnVehicleTypeIdList);
  dialog.setLinkData(linkData);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Edit link
  if(!lnk.editLink(startConnectionId, endConnectionId, dialog.linkData())){
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
  mdtClLink lnk(0, database());
  QMessageBox msgBox;
  ///QList<QModelIndexList> indexes;
  mdtSqlTableSelection s;
  QSqlError sqlError;
  QStringList fields;

  widget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  fields << "UnitConnectionStart_Id_FK" << "UnitConnectionEnd_Id_FK";
  s = widget->currentSelection(fields);
  if(s.isEmpty()){
    return;
  }
  /**
  indexes = widget->indexListOfSelectedRowsByRowsList(fields);
  if(indexes.size() < 1){
    return;
  }
  */
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
  mdtClLink lnk(0, database());
  QString sql;
  QVariant startVehicleTypeId, endVehicleTypeId;
  QVariant startConnectorId, endConnectorId;
  QVariant startUnitId, endUnitId;
  bool ok;

  // Get current unit ID
  startUnitId = currentUnitId();
  if(startUnitId.isNull()){
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
  /**
  sql = "SELECT * FROM Unit_view WHERE Unit_Id_PK <> " + startUnitId.toString();
  sql += " AND VehicleType_Id_PK = " + endVehicleTypeId.toString();
  */
  sql = "SELECT * FROM Unit_view ";
  sql += " WHERE VehicleType_Id_PK = " + endVehicleTypeId.toString();
  endUnitId = selectUnit(tr("Select end unit:"), sql);
  if(endUnitId.isNull()){
    return;
  }
  // Select end connector
  sql = lnk.sqlForConnectableUnitConnectorsSelection(startConnectorId, endUnitId, &ok);
  if(!ok){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  endConnectorId = selectUnitConnector(tr("Select end connector:"), sql);
  if(endConnectorId.isNull()){
    return;
  }
  // Make connection
  if(!lnk.connectByContactName(startConnectorId, endConnectorId, startVehicleTypeId, endVehicleTypeId)){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Update viewa
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

QVariant mdtClUnitEditor::selectBaseConnector()
{
  mdtSqlSelectionDialog selectionDialog;
  QString sql;

  if(currentUnitId().isNull()){
    return QVariant();
  }
  // Setup model to show available connectors
  sql = "SELECT * FROM Connector_tbl";
  // Setup and show dialog
  selectionDialog.setMessage("Please select a connector:");
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setHeaderData("ContactQty", tr("Contact\nQty"));
  selectionDialog.setHeaderData("InsertRotation", tr("Insert\nRotation"));
  selectionDialog.setHeaderData("ManufacturerConfigCode", tr("Manufacturer\nConfiguration code"));
  selectionDialog.setHeaderData("ManufacturerArticleCode", tr("Manufacturer\nArticle code"));
  selectionDialog.addColumnToSortOrder("Gender", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("ContactQty", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("ManufacturerConfigCode", Qt::AscendingOrder);
  selectionDialog.sort();
  ///selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(700, 400);
  selectionDialog.setWindowTitle(tr("Connector selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selection("Id_PK").rowCount() == 1);
  ///Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  ///return selectionDialog.selectionResult().at(0);
  return selectionDialog.selection("Id_PK").data(0, "Id_PK");
}

QList<QVariant> mdtClUnitEditor::selectBaseConnectorContactIdList(const QVariant & connectorId, bool multiSelection)
{
  mdtSqlSelectionDialog selectionDialog;
  QString sql;
  ///QModelIndexList selectedItems;
  mdtSqlTableSelection s;
  QList<QVariant> idList;
  int i;

  if(connectorId.isNull()){
    return idList;
  }
  // Setup model to show available connectors
  sql = "SELECT * FROM ConnectorContact_tbl ";
  sql += " WHERE Connector_Id_FK = " + connectorId.toString();
  // Setup and show dialog
  selectionDialog.setMessage("Please select contacts to use:");
  selectionDialog.setQuery(sql, database(), multiSelection);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Connector_Id_FK", true);
  selectionDialog.addColumnToSortOrder("Name", Qt::AscendingOrder);
  selectionDialog.sort();
  ///selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(400, 300);
  selectionDialog.setWindowTitle(tr("Contacts selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return idList;
  }
  s = selectionDialog.selection("Id_PK");
  for(i = 0; i < s.rowCount(); ++i){
    idList.append(s.data(i, "Id_PK"));
  }
  /**
  selectedItems = selectionDialog.selectionResults();
  for(i = 0; i < selectedItems.size(); ++i){
    idList.append(selectedItems.at(i).data());
  }
  */

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

QVariant mdtClUnitEditor::selectArticleConnector()
{
  mdtSqlSelectionDialog selectionDialog;
  QString sql;
  mdtClUnit unit(this, database());
  QVariant unitId;
  QVariant articleId;

  // Get unit and article IDs
  unitId = currentUnitId();
  if(unitId.isNull()){
    return QVariant();
  }
  articleId = currentData("Unit_tbl", "Article_Id_FK");
  if(articleId.isNull()){
    return QVariant();
  }
  // Setup model to show available connectors
  sql = unit.sqlForArticleConnectorSelection(articleId, unitId);
  // Setup and show dialog
  selectionDialog.setMessage("Please select a connector:");
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.addColumnToSortOrder("Name", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(400, 300);
  selectionDialog.setWindowTitle(tr("Article connector selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QList<QVariant> mdtClUnitEditor::selectByArticleConnectorIdArticleConnectionIdList(const QVariant & articleConnectorId, const QVariant & unitId, bool multiSelection)
{
  mdtSqlSelectionDialog selectionDialog;
  mdtClUnit unit(this, database());
  QString sql;
  QModelIndexList selectedItems;
  QList<QVariant> idList;
  int i;

  if((articleConnectorId.isNull())||(unitId.isNull())){
    return idList;
  }
  // Setup and show dialog
  sql = unit.sqlForArticleConnectionLinkedToArticleConnectorSelection(articleConnectorId, unitId);
  selectionDialog.setMessage("Please select article connections to use:");
  selectionDialog.setQuery(sql, database(), multiSelection);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnector_Id_FK", true);
  selectionDialog.setHeaderData("ArticleContactName", tr("Contact"));
  selectionDialog.setHeaderData("IoType", tr("I/O type"));
  selectionDialog.setHeaderData("FunctionEN", tr("Function\n(English)"));
  selectionDialog.setHeaderData("FunctionFR", tr("Function\n(French)"));
  selectionDialog.setHeaderData("FunctionDE", tr("Function\n(German)"));
  selectionDialog.setHeaderData("FunctionIT", tr("Function\n(Italian)"));
  selectionDialog.addColumnToSortOrder("ArticleContactName", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(700, 300);
  selectionDialog.setWindowTitle(tr("Article contacts selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return idList;
  }
  selectedItems = selectionDialog.selectionResults();
  for(i = 0; i < selectedItems.size(); ++i){
    idList.append(selectedItems.at(i).data());
  }

  return idList;
}

QList<QVariant> mdtClUnitEditor::selectByArticleIdArticleConnectionIdList(const QVariant & articleId, const QVariant & unitId, bool multiSelection)
{
  mdtSqlSelectionDialog selectionDialog;
  mdtClUnit unit(this, database());
  QString sql;
  QModelIndexList selectedItems;
  QList<QVariant> idList;
  int i;

  // Setup and show dialog
  sql = unit.sqlForArticleConnectionLinkedToArticleSelection(articleId, unitId);
  selectionDialog.setMessage("Please select article connections to use:");
  selectionDialog.setQuery(sql, database(), multiSelection);
  selectionDialog.setColumnHidden("Connector_Id_FK", true);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnector_Id_FK", true);
  selectionDialog.setHeaderData("ArticleConnectorName", tr("Connector"));
  selectionDialog.setHeaderData("ArticleContactName", tr("Contact"));
  selectionDialog.setHeaderData("IoType", tr("I/O type"));
  selectionDialog.setHeaderData("FunctionEN", tr("Function\n(English)"));
  selectionDialog.setHeaderData("FunctionFR", tr("Function\n(French)"));
  selectionDialog.setHeaderData("FunctionDE", tr("Function\n(German)"));
  selectionDialog.setHeaderData("FunctionIT", tr("Function\n(Italian)"));
  selectionDialog.addColumnToSortOrder("ArticleConnectorName", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("ArticleContactName", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(700, 300);
  selectionDialog.setWindowTitle(tr("Article contacts selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return idList;
  }
  selectedItems = selectionDialog.selectionResults();
  for(i = 0; i < selectedItems.size(); ++i){
    idList.append(selectedItems.at(i).data());
  }

  return idList;
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
  widget->tableView()->resizeColumnsToContents();

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
  /**
  if(!addChildTable("UnitConnection_view", tr("Connections"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "UnitConnection_view", "Unit_Id_FK")){
    return false;
  }
  */
  widget = sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != 0);
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
  widget->setColumnHidden("UnitConnection_Id_PK", true);
  widget->setColumnHidden("Unit_Id_FK", true);
  widget->setColumnHidden("ArticleConnection_Id_FK", true);
  // Give fields a user friendly name
  widget->setHeaderData("SchemaPage", tr("Vehicle\nSchema\npage"));
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
  // Enable sorting
  widget->addColumnToSortOrder("UnitConnectorName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("UnitContactName", Qt::AscendingOrder);
  widget->sort();
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
  QPushButton *pbConnectConnectors;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("UnitLink_view");
  relationInfo.addRelation("Id_PK", "StartUnit_Id_FK", false);
  relationInfo.addRelation("Id_PK", "EndUnit_Id_FK", false, "OR");
  if(!addChildTable(relationInfo, tr("Links"))){
    return false;
  }
  /**
  if(!addChildTable("UnitLink_view", tr("Links"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "UnitLink_view", "StartUnit_Id_FK")){
    return false;
  }
  if(!addRelation("Id_PK", "UnitLink_view", "EndUnit_Id_FK", "OR")){
    return false;
  }
  */
  widget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(widget != 0);
  // Add the Add and remove buttons
  pbAddLink = new QPushButton(tr("Add link ..."));
  pbAddLink->setIcon(QIcon::fromTheme("document-new"));
  connect(pbAddLink, SIGNAL(clicked()), this, SLOT(addLink()));
  widget->addWidgetToLocalBar(pbAddLink);
  pbEditLink = new QPushButton(tr("Edit link"));
  connect(pbEditLink, SIGNAL(clicked()), this, SLOT(editLink()));
  widget->addWidgetToLocalBar(pbEditLink);
  pbRemoveLinks = new QPushButton(tr("Remove links"));
  pbRemoveLinks->setIcon(QIcon::fromTheme("edit-delete"));
  connect(pbRemoveLinks, SIGNAL(clicked()), this, SLOT(removeLinks()));
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
  // Give fields a user friendly name
  widget->setHeaderData("StartSchemaPosition", tr("Start\nschema pos."));
  widget->setHeaderData("StartAlias", tr("Start\nalias"));
  widget->setHeaderData("StartUnitConnectorName", tr("Start\nconnector"));
  widget->setHeaderData("StartUnitContactName", tr("Start\ncontact"));
  widget->setHeaderData("EndSchemaPosition", tr("End\nschema pos."));
  widget->setHeaderData("EndAlias", tr("End\nalias"));
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
  // Setup sorting
  widget->addColumnToSortOrder("StartSchemaPosition", Qt::AscendingOrder);
  widget->addColumnToSortOrder("StartUnitConnectorName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("StartUnitContactName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("EndSchemaPosition", Qt::AscendingOrder);
  widget->addColumnToSortOrder("EndUnitConnectorName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("EndUnitContactName", Qt::AscendingOrder);
  widget->sort();
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();

  return true;
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
