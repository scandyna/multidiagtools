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
#include "mdtTtTestSystemUnitEditor.h"
#include "ui_mdtTtTestSystemUnitEditor.h"
#include "mdtAbstractSqlTableController.h"
#include "mdtSqlDataWidgetController.h"
#include "mdtUiMessageHandler.h"
#include "mdtSqlRelationInfo.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtClUnit.h"
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QPushButton>
#include <QSqlError>
#include <QMessageBox>
#include <QInputDialog>
#include <memory>

#include <QDebug>

mdtTtTestSystemUnitEditor::mdtTtTestSystemUnitEditor(QWidget* parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

bool mdtTtTestSystemUnitEditor::setupTables()
{
  if(!setupTestSystemUnitTables()){
    return false;
  }
  if(!setupConnectorTable()){
    return false;
  }
  if(!setupConnectionTable()){
    return false;
  }
  return true;
}

void mdtTtTestSystemUnitEditor::addConnector()
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
  select("UnitConnector_view");
  select("UnitConnection_view");
  ///select("UnitLink_view");
}

void mdtTtTestSystemUnitEditor::addConnectorBasedConnector()
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
  ///select("UnitLink_view");
}

void mdtTtTestSystemUnitEditor::addArticleConnectorBasedConnector()
{
  QVariant unitId;
  QVariant connectorName;
  mdtClUnitConnectorData connectorData;
  QVariant articleConnectorId;
  QList<QVariant> articleConnectionIdList;
  mdtClUnit unit(0, database());
  QInputDialog dialog;

  // Check that vehicle type assignation was made
  /**
  if(rowCount("Unit_VehicleType_view", false) < 1){
    pvLastError.setError(tr("No vehicle type was assigned."), mdtError::Error);
    pvLastError.setInformativeText(tr("Please assign a vehicle type and try again."));
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnitEditor");
    pvLastError.commit();
    displayLastError();
    return;
  }
  */
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
  ///select("UnitLink_view");
}

void mdtTtTestSystemUnitEditor::editConnectorName()
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
  ///select("UnitLink_view");
}

void mdtTtTestSystemUnitEditor::removeConnectors()
{
  mdtSqlTableWidget *widget;
  mdtClUnit unit(this, database());
  QMessageBox msgBox;
  mdtSqlTableSelection s;

  widget = sqlTableWidget("UnitConnector_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("Id_PK");
  if(s.isEmpty()){
    return;
  }
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
  ///select("UnitLink_view");
}

QVariant mdtTtTestSystemUnitEditor::selectBaseConnector()
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
  selectionDialog.resize(700, 400);
  selectionDialog.setWindowTitle(tr("Connector selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selection("Id_PK").rowCount() == 1);

  return selectionDialog.selection("Id_PK").data(0, "Id_PK");
}

QList< QVariant > mdtTtTestSystemUnitEditor::selectBaseConnectorContactIdList(const QVariant& connectorId, bool multiSelection)
{
  QList<QVariant> idList;
  mdtSqlSelectionDialog selectionDialog;
  QString sql;

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
  selectionDialog.resize(400, 300);
  selectionDialog.setWindowTitle(tr("Contacts selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return idList;
  }
  idList = selectionDialog.selection("Id_PK").dataList("Id_PK");

  return idList;
}

QVariant mdtTtTestSystemUnitEditor::selectArticleConnector()
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
  selectionDialog.addColumnToSortOrder("Name", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(400, 300);
  selectionDialog.setWindowTitle(tr("Article connector selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selection("Id_PK").rowCount() == 1);

  return selectionDialog.selection("Id_PK").data(0, "Id_PK");
}

QList< QVariant > mdtTtTestSystemUnitEditor::selectByArticleConnectorIdArticleConnectionIdList(const QVariant& articleConnectorId, const QVariant& unitId, bool multiSelection)
{
  QList<QVariant> idList;
  mdtSqlSelectionDialog selectionDialog;
  mdtClUnit unit(this, database());
  QString sql;
  QModelIndexList selectedItems;

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
  selectionDialog.resize(700, 300);
  selectionDialog.setWindowTitle(tr("Article contacts selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return idList;
  }
  idList = selectionDialog.selection("Id_PK").dataList("Id_PK");

  return idList;
}

bool mdtTtTestSystemUnitEditor::setupTestSystemUnitTables()
{
  Ui::mdtTtTestSystemUnitEditor tsue;
  std::shared_ptr<mdtSqlDataWidgetController> uController;
  std::shared_ptr<mdtSqlDataWidgetController> tsuController;
  std::shared_ptr<mdtSqlDataWidgetController> tscController;
  std::shared_ptr<mdtSqlDataWidgetController> baseArticleController;
  std::shared_ptr<mdtUiMessageHandler> uiMessageHandler;
  mdtSqlRelationInfo relationInfo;
  
  /// \todo Use mdtSqlSchemaTable for better mapping

  // Setup main form widget part
  setMainTableUi(tsue);
  uiMessageHandler = std::make_shared<mdtUiMessageHandler>();
  /*
   * Setup test system unit and unit part.
   * Here, we must have Unit_tbl as main controller,
   * main reason is for PK auto generation
   * (We cannot have */
  // Setup test system unit table controller
  tsuController = mainTableController<mdtSqlDataWidgetController>();
  Q_ASSERT(tsuController);
  tsuController->setTableName("TestSystemUnit_tbl", database(), tr("Test system unit"));
  tsuController->setMessageHandler(uiMessageHandler);
  // Setup unit table controller
  relationInfo.setChildTableName("Unit_tbl");
  relationInfo.addRelation("Unit_Id_FK_PK", "Id_PK", true);
  relationInfo.setRelationType(mdtSqlRelationInfo::OneToOne);
  if(!tsuController->addChildController<mdtSqlDataWidgetController>(relationInfo, tr("Test system unit"))){
    pvLastError = tsuController->lastError();
    return false;
  }
  uController = tsuController->childController<mdtSqlDataWidgetController>("Unit_tbl");
  Q_ASSERT(uController);
  uController->setMessageHandler(uiMessageHandler);
  // Setup mapping for TestSystemUnit_tbl part
  if(!tsuController->addMapping(tsue.fld_IoPosition, "IoPosition")){
    pvLastError = tsuController->lastError();
    return false;
  }
  if(!tsuController->addMapping(tsue.fld_CalibrationDate, "CalibrationDate")){
    pvLastError = tsuController->lastError();
    return false;
  }

  // Setup mapping for Unit_tbl part
  if(!uController->addMapping(tsue.fld_SchemaPosition, "SchemaPosition")){
    pvLastError = uController->lastError();
    return false;
  }
  /*
   * Setup test system component widget mapping
   */
  // Add a new child controller to test system unit controller
  Q_ASSERT(tsuController);
  relationInfo.clear();
  relationInfo.setChildTableName("TestSystemComponent_tbl");
  relationInfo.addRelation("TestSystemComponent_Id_FK", "Id_PK", false);
  if(!tsuController->addChildController<mdtSqlDataWidgetController>(relationInfo, tr("Test system component"))){
    pvLastError = tsuController->lastError();
    return false;
  }
  // Get freshly added controller and map widgets to it
  tscController = tsuController->childController<mdtSqlDataWidgetController>("TestSystemComponent_tbl");
  Q_ASSERT(tscController);
  tscController->setMessageHandler(uiMessageHandler);
  tscController->addMapping(tsue.fld_TscName, "NameEN");
  tscController->addMapping(tsue.fld_TscAlias, "Alias");
  /*
   * Setup base article widget mapping
   */
  // Add a new child controller to test system unit controller
  
  // Get freshly added controller and map widgets to it
  
  return true;
}

bool mdtTtTestSystemUnitEditor::setupConnectorTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pb;

  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("UnitConnector_view");
  relationInfo.addRelation("Unit_Id_FK_PK", "Unit_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Connectors"))){
    return false;
  }
  widget = sqlTableWidget("UnitConnector_view");
  Q_ASSERT(widget != nullptr);
  // Hide relation fields and PK
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("Unit_Id_FK", true);
  widget->setColumnHidden("Connector_Id_FK", true);
  widget->setColumnHidden("ArticleConnector_Id_FK", true);
  // Give fields a user friendly name
  widget->setHeaderData("UnitConnectorName", tr("Unit\nconnector"));
  widget->setHeaderData("ArticleConnectorName", tr("Article\nconnector"));
  // Setup add connector button
  pb = new QPushButton(tr("Add free connector"));
  pb->setIcon(QIcon::fromTheme("document-new"));
  connect(pb, SIGNAL(clicked()), this, SLOT(addConnector()));
  widget->addWidgetToLocalBar(pb);
  // Setup add connector based connector button
  pb = new QPushButton(tr("Add connector"));
  pb->setIcon(QIcon::fromTheme("document-new"));
  connect(pb, SIGNAL(clicked()), this, SLOT(addConnectorBasedConnector()));
  widget->addWidgetToLocalBar(pb);
  // Setup add article connector based connector button
  pb = new QPushButton(tr("Add art. connector"));
  pb->setIcon(QIcon::fromTheme("document-new"));
  connect(pb, SIGNAL(clicked()), this, SLOT(addArticleConnectorBasedConnector()));
  widget->addWidgetToLocalBar(pb);
  // Setup edit connector name button
  pb = new QPushButton(tr("Edit connector name"));
  connect(pb, SIGNAL(clicked()), this, SLOT(editConnectorName()));
  widget->addWidgetToLocalBar(pb);
  // Setup remove connectors button
  pb = new QPushButton(tr("Rem. connectors ..."));
  pb->setIcon(QIcon::fromTheme("edit-delete"));
  connect(pb, SIGNAL(clicked()), this, SLOT(removeConnectors()));
  widget->addWidgetToLocalBar(pb);
  // Stretch
  widget->addStretchToLocalBar();
  // Set some attributes on table view
  widget->resizeViewToContents();

  return true;
}

bool mdtTtTestSystemUnitEditor::setupConnectionTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pb;

  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("UnitConnection_view");
  relationInfo.addRelation("Unit_Id_FK_PK", "Unit_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Connections"))){
    return false;
  }
  widget = sqlTableWidget("UnitConnection_view");
  Q_ASSERT(widget != nullptr);
  // Hide relation fields and PK
  widget->setColumnHidden("UnitConnection_Id_PK", true);
  widget->setColumnHidden("UnitConnector_Id_FK", true);
  widget->setColumnHidden("Unit_Id_FK", true);
  widget->setColumnHidden("ArticleConnection_Id_FK", true);
  widget->setColumnHidden("IoType", true);
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
  widget->setHeaderData("UnitConnectionResistance", tr("Unit\nconnection\nresistance"));
  widget->setHeaderData("ArticleConnectorName", tr("Article\nconnector"));
  widget->setHeaderData("ArticleContactName", tr("Article\ncontact"));
  widget->setHeaderData("ArticleFunctionEN", tr("Article\nfunction\n(English)"));
  widget->setHeaderData("ArticleFunctionFR", tr("Article\nfunction\n(French)"));
  widget->setHeaderData("ArticleFunctionDE", tr("Article\nfunction\n(German)"));
  widget->setHeaderData("ArticleFunctionIT", tr("Article\nfunction\n(Italian)"));
  // Enable sorting
  widget->addColumnToSortOrder("UnitConnectorName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("UnitContactName", Qt::AscendingOrder);
  widget->sort();
  // Set some attributes on table view
  widget->resizeViewToContents();
  // Add add connection button


  return true;
}


