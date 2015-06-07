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
#include "mdtClArticleEditor.h"
#include "ui_mdtClArticleEditor.h"
#include "mdtClConnectionTypeData.h"
#include "mdtClConnectorSelectionDialog.h"
#include "mdtClConnectorData.h"
#include "mdtClConnectorContactSelectionDialog.h"
#include "mdtClConnectorContactData.h"
#include "mdtClArticleConnection.h"

#include "mdtSqlTableSelection.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtError.h"
#include "mdtClArticleComponentDialog.h"
#include "mdtClArticleConnectionDialog.h"
#include "mdtClArticleLinkDialog.h"
#include "mdtClArticle.h"
#include "mdtClConnectorData.h"
#include "mdtClArticleConnectorData.h"
#include "mdtClArticleConnectionData.h"
#include "mdtSqlRecord.h"
#include "mdtSqlFieldSelectionDialog.h"
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QPushButton>
#include <QStringList>
#include <QString>
#include <QList>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QInputDialog>
#include <QTableView>

#include <QDebug>

mdtClArticleEditor::mdtClArticleEditor(QWidget *parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

mdtClArticleEditor::~mdtClArticleEditor()
{
}

void mdtClArticleEditor::addComponent()
{
  mdtClArticleComponentDialog dialog(0, database(), currentArticleId());
  mdtClArticle art(this, database());

  if(currentArticleId().isNull()){
    return;
  }
  // Setup and show dialog
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add component
  if(!art.addComponent(dialog.articleId(), dialog.componentId(), dialog.qty(), dialog.qtyUnit())){
    QMessageBox msgBox;
    msgBox.setText(tr("Component insertion failed"));
    msgBox.setInformativeText(tr("Please see details for more informations"));
    msgBox.setDetailedText(art.lastError().text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update component table
  select("ArticleComponent_view");
}

void mdtClArticleEditor::editComponent()
{
  mdtSqlTableWidget *widget;
  QVariant currentArtId;
  QVariant currentComponentId;
  mdtClArticle art(this, database());

  widget = sqlTableWidget("ArticleComponent_view");
  Q_ASSERT(widget != 0);
  // Setup and show dialog
  currentArtId = currentArticleId();
  currentComponentId = widget->currentData("Component_Id_PK");
  if(currentComponentId.isNull()){
    return;
  }
  mdtClArticleComponentDialog dialog(0, database(), currentArtId, currentComponentId);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Edit component
  if(!art.editComponent(dialog.articleId(), currentComponentId, dialog.componentId(), dialog.qty(), dialog.qtyUnit())){
    QMessageBox msgBox;
    msgBox.setText(tr("Component insertion failed"));
    msgBox.setInformativeText(tr("Please see details for more informations"));
    msgBox.setDetailedText(art.lastError().text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update component table
  select("ArticleComponent_view");
}

void mdtClArticleEditor::editComponent(const QModelIndex &index)
{
  editComponent();
}

void mdtClArticleEditor::removeComponents()
{
  mdtSqlTableWidget *widget;
  mdtClArticle art(this, database());
  QMessageBox msgBox;
  ///QModelIndexList indexes;
  mdtSqlTableSelection s;
  QSqlError sqlError;
  int ret;

  widget = sqlTableWidget("ArticleComponent_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("Component_Id_PK");
  if(s.isEmpty()){
    return;
  }
  /**
  indexes = widget->indexListOfSelectedRows("Component_Id_PK");
  if(indexes.size() < 1){
    return;
  }
  */
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove components from current article."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  ret = msgBox.exec();
  if(ret != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!art.removeComponents(currentArticleId(), s)){
    ///sqlError = art.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Components removing failed."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(art.lastError().text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update component table
  select("ArticleComponent_view");
}

/// \note Will be implemented if a day a link exists between ArticleConnection_tbl and ConnectorContact_tbl.
/**
void mdtClArticleEditor::addConnection()
{
  
  mdtClArticle art(this, database());
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  // Update connections table
  select("ArticleConnection_view");
}
*/

void mdtClArticleEditor::addConnection()
{
  mdtClArticleConnection acnx(database());
  mdtClArticleConnectionData data;
  mdtClArticleConnectionKeyData key;
  mdtClArticleConnectionDialog dialog(this, database());

  // Setup data
  key.articleId = currentArticleId();
  if(key.articleId.isNull()){
    return;
  }
  data.setKeyData(key);
  // Setup and show dialog
  dialog.setData(data);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Get and update data
  data = dialog.data();
  // Add connection
  if(acnx.addArticleConnection(data, true).isNull()){
    pvLastError = acnx.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  select("ArticleConnection_view");
}

void mdtClArticleEditor::editCurrentConnection()
{
  mdtSqlTableWidget *widget;
  mdtClArticleConnectionKeyData key;

  widget = sqlTableWidget("ArticleConnection_view");
  Q_ASSERT(widget != 0);

  // Get selected article connection ID
  key.id = widget->currentData("Id_PK");
  editConnection(key);
}

void mdtClArticleEditor::editConnection(const QSqlRecord & record)
{
  mdtClArticleConnectionKeyData key;

  key.id = record.value("Id_PK");
  editConnection(key);
}

void mdtClArticleEditor::updateRelatedUnitConnections()
{
  mdtClArticle art(this, database());
  mdtSqlFieldSelectionDialog fsDialog(this);
  QMessageBox msgBox(this);
  mdtSqlTableSelection s;
  mdtSqlTableWidget *widget;
  QStringList fields;

  widget = sqlTableWidget("ArticleConnection_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  // Setup and show dialog
  fsDialog.setMessage(tr("Select fields that must be updated in related unit connections:"));
  fsDialog.addField("ArticleContactName", tr("Contact name"), false);
  fsDialog.addField("Resistance", tr("Connection resistance"), false);
  fsDialog.addField("FunctionFR", tr("Function (French)"), false);
  fsDialog.addField("FunctionEN", tr("Function (English)"), false);
  fsDialog.addField("FunctionDE", tr("Function (German)"), false);
  fsDialog.addField("FunctionIT", tr("Function (Italian)"), false);
  fsDialog.sort();
  if(fsDialog.exec() != QDialog::Accepted){
    return;
  }
  fields = fsDialog.getSelectedFieldNames();
  if(fields.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to update all unit connections related to selected connection."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Update unit connections
  if(!art.updateUnitConnections(fields, s)){
    pvLastError = art.lastError();
    displayLastError();
    return;
  }
  msgBox.setText(tr("Unit connections update done."));
  msgBox.setInformativeText("");
  msgBox.setIcon(QMessageBox::Information);
  msgBox.setStandardButtons(QMessageBox::Yes);
  msgBox.exec();
}

void mdtClArticleEditor::removeConnections()
{
  mdtClArticleConnection acnx(database());
  mdtSqlTableWidget *widget;
  QMessageBox msgBox;
  mdtSqlTableSelection s;

  widget = sqlTableWidget("ArticleConnection_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove connections from current article."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!acnx.removeArticleConnections(s)){
    pvLastError = acnx.lastError();
    displayLastError();
    return;
  }
  // Update connection table
  select("ArticleConnection_view");
}

void mdtClArticleEditor::addConnector()
{
  mdtClArticleConnection acnx(database());
  mdtClConnectorSelectionDialog csDialog(this);
//   mdtClConnectorKeyData baseConnectorKey;
  QList<mdtClConnectorContactData> selectedContacts;
  mdtClConnectorContactSelectionDialog ccsDialog(this);
  mdtClArticleConnectorKeyData connectorKey;
  mdtClArticleConnectorData connectorData;
  
//   QVariant articleId;
  ///QVariant baseConnectorId;
//   QString connectorName;
//   QList<QVariant> selectedContacts;
//   mdtClArticle art(this, database());

  // Get current article ID
  connectorKey.articleId = currentArticleId();
  if(connectorKey.articleId.isNull()){
    return;
  }
//   articleId = currentArticleId();
//   if(articleId.isNull()){
//     return;
//   }
//   // Build article connector data
//   if(!connectorData.setup(database(), false)){
//     pvLastError = connectorData.lastError();
//     displayLastError();
//     return;
//   }
  // Let user choose a base connector
  if(!csDialog.select(database())){
    pvLastError = csDialog.lastError();
    displayLastError();
    return;
  }
  if(csDialog.exec() != QDialog::Accepted){
    return;
  }
  connectorKey.connectorFk = csDialog.selectedConnectorKey();
  if(connectorKey.connectorFk.isNull()){
    return;
  }
  connectorData.setKeyData(connectorKey);
//   baseConnectorKey = csDialog.selectedConnectorKey();
//   if(baseConnectorKey.isNull()){
//     return;
//   }
  /**
  baseConnectorId = selectConnector();
  if(baseConnectorId.isNull()){
    return;
  }
  */
  // Let user give a connector name
  QInputDialog dialog;
  dialog.setLabelText(tr("Connector name:"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  connectorData.name = dialog.textValue().trimmed();
  qDebug() << "Name: " << connectorData.name << ", " << connectorData.name.toString();
  if(connectorData.name.toString().isEmpty()){
    return;
  }
//   connectorName = dialog.textValue().trimmed();
//   if(connectorName.isEmpty()){
//     return;
//   }
//   connectorData.setValue("Article_Id_FK", articleId);
  ///connectorData.setValue("Connector_Id_FK", baseConnectorId);
//   connectorData.setValue("Name", connectorName);
  // Let user choose connector contacts
  if(!ccsDialog.select(database(), connectorKey.connectorFk)){
    pvLastError = ccsDialog.lastError();
    displayLastError();
    return;
  }
  if(ccsDialog.exec() != QDialog::Accepted){
    return;
  }
  selectedContacts = ccsDialog.selectedContactDataList();
  ///selectedContacts = selectConnectorContacts(baseConnectorId);
  qDebug() << "Selected: " << selectedContacts.size();
  if(selectedContacts.isEmpty()){
    return;
  }
  // Add connection based on selected base connector contacts
  acnx.addConnectionsToArticleConnector(connectorData, selectedContacts);
//   if(!art.addConnectionDataListFromConnectorContactIdList(connectorData, selectedContacts)){
//     pvLastError = art.lastError();
//     displayLastError();
//     return;
//   }
  // Add connector to database
  if(acnx.addArticleConnector(connectorData, true).isNull()){
    pvLastError = acnx.lastError();
    displayLastError();
    return;
  }
//   if(!art.addConnector(connectorData)){
//     pvLastError = art.lastError();
//     displayLastError();
//     return;
//   }
  // Update connections table
  select("ArticleConnector_view");
  select("ArticleConnection_view");
}

void mdtClArticleEditor::editConnectorName()
{
  QVariant articleConnectorId;
  QVariant connectorName;
  QString str;
  QInputDialog dialog;
  mdtClArticle art(this, database());

  // Get current data
  articleConnectorId = currentData("ArticleConnector_view", "Id_PK");
  if(articleConnectorId.isNull()){
    return;
  }
  connectorName = currentData("ArticleConnector_view", "ArticleConnectorName");
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
  if(!art.editConnectorName(articleConnectorId, connectorName)){
    pvLastError = art.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("ArticleConnector_view");
  select("ArticleConnection_view");
}

void mdtClArticleEditor::removeConnectors()
{
  mdtSqlTableWidget *widget;
  mdtClArticle art(this, database());
  QMessageBox msgBox;
  ///QModelIndexList indexes;
  mdtSqlTableSelection s;

  ///widget = sqlTableWidget("ArticleConnector_view");
  widget = 0;
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
  msgBox.setText(tr("You are about to remove connectors and related connections from current article."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!art.removeConnectors(s)){
    pvLastError = art.lastError();
    displayLastError();
    return;
  }
  // Update connectors and connections views
  select("ArticleConnector_view");
  select("ArticleConnection_view");
}

void mdtClArticleEditor::addLink()
{
  mdtClArticleLinkDialog dialog(0, database(), currentArticleId());
  mdtClArticle art(this, database());

  // Check if some connection exists
  if(rowCount("ArticleConnection_view") < 1){
    QMessageBox msgBox;
    msgBox.setText(tr("There is no connection available for current article"));
    msgBox.setInformativeText(tr("You must add connections to be able to link them"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
    return;
  }
  // Setup and show dialog
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add link
  if(!art.addLink(dialog.linkData())){
    pvLastError = art.lastError();
    displayLastError();
    return;
  }
  // Update link table
  select("ArticleLink_view");
}

void mdtClArticleEditor::editLink()
{
  mdtClArticleLinkDialog dialog(0, database(), currentArticleId());
  mdtSqlTableWidget *widget;
  mdtClArticle art(this, database());
  QVariant articleConnectionStartId, articleConnectionEndId;
  mdtSqlTableSelection s;
  QStringList fields;
  bool hasRelatedLinks;
  bool ok;

  if(currentArticleId().isNull()){
    return;
  }
  widget = sqlTableWidget("ArticleLink_view");
  Q_ASSERT(widget != 0);
  // Get selected links
  fields << "ArticleConnectionStart_Id_FK" << "ArticleConnectionEnd_Id_FK" << "LinkType_Code_FK" << "LinkDirection_Code_FK" << "Resistance";
  s = widget->currentSelection(fields);
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() > 0);
  // Get start and end connection ID
  /// \todo OK if Null ??
  articleConnectionStartId = s.data(0, "ArticleConnectionStart_Id_FK");
  articleConnectionEndId = s.data(0, "ArticleConnectionEnd_Id_FK");
  // Setup and show dialog
  hasRelatedLinks = art.hasRelatedLinks(articleConnectionStartId, articleConnectionEndId, ok);
  if(!ok){
    pvLastError = art.lastError();
    displayLastError();
    return;
  }
  dialog.setConnectionEditionLocked(hasRelatedLinks);
  dialog.setLinkTypeCode(s.data(0, "LinkType_Code_FK"));
  dialog.setLinkDirectionCode(s.data(0, "LinkDirection_Code_FK"));
  dialog.setValue(s.data(0, "Resistance"));
  dialog.setStartConnectionId(articleConnectionStartId);
  dialog.setEndConnectionId(articleConnectionEndId);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Edit link
  if(!art.editLink(articleConnectionStartId, articleConnectionEndId, dialog.linkData())){
    pvLastError = art.lastError();
    displayLastError();
    return;
  }
  // Update related links if they exists
  /**
  if(hasRelatedLinks){
    fields.clear();
    fields << "LinkType_Code_FK" << "LinkDirection_Code_FK" << "Resistance";
    if(!art.updateRelatedLinks(articleConnectionStartId, articleConnectionEndId, fields)){
      pvLastError = art.lastError();
      displayLastError();
      return;
    }
  }
  */
  // Update link table
  select("ArticleLink_view");
}

void mdtClArticleEditor::removeLinks()
{
  mdtSqlTableWidget *widget;
  mdtClArticle art(this, database());
  QMessageBox msgBox;
  QStringList fields;
  ///QList<QModelIndexList> indexes;
  mdtSqlTableSelection s;
  QSqlError sqlError;
  int ret;

  widget = sqlTableWidget("ArticleLink_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  fields << "ArticleConnectionStart_Id_FK" << "ArticleConnectionEnd_Id_FK";
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
  msgBox.setText(tr("You are about to remove links for current article."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  ret = msgBox.exec();
  if(ret != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!art.removeLinks(s)){
    pvLastError = art.lastError();
    displayLastError();
    return;
  }
  // Update link table
  select("ArticleLink_view");
}

void mdtClArticleEditor::editConnection(const mdtClArticleConnectionKeyData & key)
{
  mdtClArticleConnection acnx(database());
  mdtClArticleConnectionData data;
  mdtClArticleConnectionDialog dialog(this, database());
  bool ok;

  if(key.id.isNull()){
    return;
  }
  // Get connection data
  data = acnx.getArticleConnectionData(key, ok);
  if(!ok){
    pvLastError = acnx.lastError();
    displayLastError();
    return;
  }
  // Setup and show dialog
  dialog.setData(data);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Get data and update
  data = dialog.data();
  if(!acnx.updateArticleConnection(key, data)){
    pvLastError = acnx.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  select("ArticleConnection_view");
}

bool mdtClArticleEditor::setupTables()
{
  QSqlError sqlError;

  // Setup Article table
  if(!setupArticleTable()){
    return false;
  }
  // Setup component table
  if(!setupArticleComponentTable()){
    return false;
  }
  // Setup article used as component by table
  if(!setupArticleUsedByTable()){
    return false;
  }
  // Setup connector table
  if(!setupArticleConnectorTable()){
    return false;
  }
  // Setup connection table
  if(!setupArticleConnectionTable()){
    return false;
  }
  // Setup link table
  if(!setupArticleLinkTable()){
    return false;
  }

  return true;
}

QVariant mdtClArticleEditor::currentArticleId()
{
  return currentData("Article_tbl", "Id_PK");
}

/**
QVariant mdtClArticleEditor::selectConnector()
{
  mdtSqlSelectionDialog selectionDialog;
  QVariant articleId;
  QString sql;

  // Get current article ID
  articleId = currentArticleId();
  if(articleId.isNull()){
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
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(700, 300);
  selectionDialog.setWindowTitle(tr("Connector selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}
*/

QList<QVariant> mdtClArticleEditor::selectConnectorContacts(const QVariant &connectorId)
{
  QList<QVariant> contactIds;
  QModelIndexList selectedItems;
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QVariant articleId;
  QString sql;
  int i;

  if(connectorId.isNull()){
    return contactIds;
  }
  // Setup model to show available contacts
  sql = "SELECT * FROM ConnectorContact_tbl ";
  sql += "WHERE Connector_Id_FK = " + connectorId.toString();
  // Setup and show dialog
  selectionDialog.setMessage("Please select contacts:");
  selectionDialog.setQuery(sql, database(), true);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Connector_Id_FK", true);
  selectionDialog.addColumnToSortOrder("Name", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  selectionDialog.setWindowTitle(tr("Contacts selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return contactIds;
  }
  selectedItems = selectionDialog.selectionResults();
  for(i = 0; i < selectedItems.size(); ++i){
    contactIds.append(selectedItems.at(i).data());
  }

  return contactIds;
}

bool mdtClArticleEditor::setupArticleTable()
{
  ///Ui::mdtClArticleEditor ae;

  // Setup main form widget
  setMainTableUi<Ui::mdtClArticleEditor>();
  ///ae.setupUi(mainSqlWidget());
  ///connect(this, SIGNAL(unitEdited()), mainSqlWidget(), SIGNAL(dataEdited()));
  // Setup form
  if(!setMainTable("Article_tbl", "Article", database())){
    return false;
  }

  return true;
}

bool mdtClArticleEditor::setupArticleComponentTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddComponent;
  QPushButton *pbEditComponent;
  QPushButton *pbRemoveComponents;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("ArticleComponent_view");
  relationInfo.addRelation("Id_PK", "Article_Id_PK", false);
  if(!addChildTable(relationInfo, tr("Components"))){
    return false;
  }
  /**
  if(!addChildTable("ArticleComponent_view", tr("Components"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "ArticleComponent_view", "Article_Id_PK")){
    return false;
  }
  */
  widget = sqlTableWidget("ArticleComponent_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Article_Id_PK", true);
  widget->setColumnHidden("Component_Id_PK", true);
  // Set fields a user friendly name
  widget->setHeaderData("ComponentQty", tr("Qty"));
  widget->setHeaderData("ComponentQtyUnit", tr("Unit"));
  widget->setHeaderData("ArticleCode", tr("Article code"));
  widget->setHeaderData("DesignationEN", tr("Designation (ENG)"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add edition buttons
  pbAddComponent = new QPushButton(tr("Add component ..."));
  connect(pbAddComponent, SIGNAL(clicked()), this, SLOT(addComponent()));
  widget->addWidgetToLocalBar(pbAddComponent);
  pbEditComponent = new QPushButton(tr("Edit component ..."));
  connect(pbEditComponent, SIGNAL(clicked()), this, SLOT(editComponent()));
  widget->addWidgetToLocalBar(pbEditComponent);
  pbRemoveComponents = new QPushButton(tr("Remove components"));
  connect(pbRemoveComponents, SIGNAL(clicked()), this, SLOT(removeComponents()));
  widget->addWidgetToLocalBar(pbRemoveComponents);
  widget->addStretchToLocalBar();

  return true;
}

bool mdtClArticleEditor::setupArticleUsedByTable()
{
  mdtSqlTableWidget *widget;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("ArticleComponentUsage_view");
  relationInfo.addRelation("Id_PK", "Component_Id_PK", false);
  if(!addChildTable(relationInfo, tr("Used by article"))){
    return false;
  }
  /**
  if(!addChildTable("ArticleComponentUsage_view", tr("Used by"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "ArticleComponentUsage_view", "Component_Id_PK")){
    return false;
  }
  */
  widget = sqlTableWidget("ArticleComponentUsage_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Article_Id_PK", true);
  widget->setColumnHidden("Component_Id_PK", true);
  // Set fields a user friendly name
  widget->setHeaderData("ArticleCode", tr("Article code"));
  widget->setHeaderData("DesignationEN", tr("Designation (ENG)"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();

  return true;
}

bool mdtClArticleEditor::setupArticleConnectorTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddConnector;
  QPushButton *pbEditConnectorName;
  QPushButton *pbRemoveConnectors;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("ArticleConnector_view");
  relationInfo.addRelation("Id_PK", "Article_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Connectors"))){
    return false;
  }
  /**
  if(!addChildTable("ArticleConnector_view", tr("Connectors"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "ArticleConnector_view", "Article_Id_FK")){
    return false;
  }
  */
  widget = sqlTableWidget("ArticleConnector_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("Article_Id_FK", true);
  widget->setColumnHidden("Connector_Id_FK", true);
  widget->setColumnHidden("ArticleConnector_Id_FK", true);
  // Set fields a user friendly name
  widget->setHeaderData("ArticleConnectorName", tr("Connector"));
  widget->setHeaderData("ArticleContactName", tr("Contact"));
  // Add edition buttons
  pbAddConnector = new QPushButton(tr("Add connector ..."));
  connect(pbAddConnector, SIGNAL(clicked()), this, SLOT(addConnector()));
  widget->addWidgetToLocalBar(pbAddConnector);
  pbEditConnectorName = new QPushButton(tr("Edit name"));
  connect(pbEditConnectorName, SIGNAL(clicked()), this, SLOT(editConnectorName()));
  widget->addWidgetToLocalBar(pbEditConnectorName);
  pbRemoveConnectors = new QPushButton(tr("Remove connectors"));
  connect(pbRemoveConnectors, SIGNAL(clicked()), this, SLOT(removeConnectors()));
  widget->addWidgetToLocalBar(pbRemoveConnectors);
  widget->addStretchToLocalBar();

  return true;
}

bool mdtClArticleEditor::setupArticleConnectionTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddConnection;
  QPushButton *pbEditConnection;
  QPushButton *pb;
  QPushButton *pbRemoveConnections;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("ArticleConnection_view");
  relationInfo.addRelation("Id_PK", "Article_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Connections"))){
    return false;
  }
  widget = sqlTableWidget("ArticleConnection_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("Article_Id_FK", true);
  widget->setColumnHidden("Connector_Id_FK", true);
  widget->setColumnHidden("ArticleConnector_Id_FK", true);
  // Set fields a user friendly name
  widget->setHeaderData("ArticleConnectorName", tr("Connector"));
  widget->setHeaderData("ArticleContactName", tr("Contact"));
  widget->setHeaderData("IoType", tr("I/O type"));
  widget->setHeaderData("FunctionEN", tr("Function EN"));
  // Add edition buttons
  pbAddConnection = new QPushButton(tr("Add connection ..."));
  pbAddConnection->setIcon(QIcon::fromTheme("document-new"));
  connect(pbAddConnection, SIGNAL(clicked()), this, SLOT(addConnection()));
  widget->addWidgetToLocalBar(pbAddConnection);
  pbEditConnection = new QPushButton(tr("Edit connection ..."));
  connect(pbEditConnection, SIGNAL(clicked()), this, SLOT(editCurrentConnection()));
  widget->addWidgetToLocalBar(pbEditConnection);
  // Add related unit connections editions button
  pb = new QPushButton(tr("Update related unit connections ..."));
  connect(pb, SIGNAL(clicked()), this, SLOT(updateRelatedUnitConnections()));
  widget->addWidgetToLocalBar(pb);
  // Add remove connections button
  pbRemoveConnections = new QPushButton(tr("Remove connections"));
  pbRemoveConnections->setIcon(QIcon::fromTheme("edit-delete"));
  connect(pbRemoveConnections, SIGNAL(clicked()), this, SLOT(removeConnections()));
  widget->addWidgetToLocalBar(pbRemoveConnections);
  widget->addStretchToLocalBar();
  // Enable sorting
  widget->addColumnToSortOrder("ArticleConnectorName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("ArticleContactName", Qt::AscendingOrder);
  widget->sort();
  // On double click, we edit connection
  Q_ASSERT(widget->tableView() != 0);
  ///connect(widget->tableView(), SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(editConnection(const QModelIndex &)));
  connect(widget, SIGNAL(doubleClicked(const QSqlRecord &)), this, SLOT(editConnection(const QSqlRecord &)));
  widget->resizeViewToContents();

  return true;
}

bool mdtClArticleEditor::setupArticleLinkTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddLink;
  QPushButton *pbEditLink;
  QPushButton *pbRemoveLinks;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("ArticleLink_view");
  relationInfo.addRelation("Id_PK", "StartArticle_Id_FK", false);
  relationInfo.addRelation("Id_PK", "EndArticle_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Links"))){
    return false;
  }
  /**
  if(!addChildTable("ArticleLink_view", tr("Links"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "ArticleLink_view", "StartArticle_Id_FK")){
    return false;
  }
  if(!addRelation("Id_PK", "ArticleLink_view", "EndArticle_Id_FK")){
    return false;
  }
  */
  widget = sqlTableWidget("ArticleLink_view");
  Q_ASSERT(widget != 0);
  Q_ASSERT(widget->tableView() != 0);
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("StartArticle_Id_FK", true);
  widget->setColumnHidden("EndArticle_Id_FK", true);
  widget->setColumnHidden("LinkType_Code_FK", true);
  widget->setColumnHidden("LinkDirection_Code_FK", true);
  widget->setColumnHidden("ArticleConnectionStart_Id_FK", true);
  widget->setColumnHidden("ArticleConnectionEnd_Id_FK", true);
  widget->setColumnHidden("UnitConnectionStart_Id_FK", true);
  widget->setColumnHidden("UnitConnectionEnd_Id_FK", true);
  // Set fields a user friendly name
  widget->setHeaderData("SinceVersion", tr("Since\nversion"));
  widget->setHeaderData("LinkTypeNameEN", tr("Link type\n(English)"));
  widget->setHeaderData("LinkTypeNameFR", tr("Link type\n(French)"));
  widget->setHeaderData("LinkTypeNameDE", tr("Link type\n(German)"));
  widget->setHeaderData("LinkTypeNameIT", tr("Link type\n(Italian)"));
  widget->setHeaderData("ValueUnit", tr("Unit"));
  widget->setHeaderData("StartArticleConnectorName", tr("Start\nconnector"));
  widget->setHeaderData("StartArticleContactName", tr("Start\ncontact"));
  widget->setHeaderData("StartIoType", tr("Start\nI/O type"));
  widget->setHeaderData("StartFunctionEN", tr("Start\nFunction\n(English)"));
  widget->setHeaderData("StartFunctionFR", tr("Start\nFunction\n(French)"));
  widget->setHeaderData("StartFunctionDE", tr("Start\nFunction\n(German)"));
  widget->setHeaderData("StartFunctionIT", tr("Start\nFunction\n(Italian)"));
  widget->setHeaderData("LinkDirectionPictureAscii", tr("Direction"));
  widget->setHeaderData("EndArticleConnectorName", tr("End\nconnector"));
  widget->setHeaderData("EndArticleContactName", tr("End\ncontact"));
  widget->setHeaderData("EndIoType", tr("End\nI/O type"));
  widget->setHeaderData("EndFunctionEN", tr("End\nFunction\n(English)"));
  widget->setHeaderData("EndFunctionFR", tr("End\nFunction\n(French)"));
  widget->setHeaderData("EndFunctionDE", tr("End\nFunction\n(German)"));
  widget->setHeaderData("EndFunctionIT", tr("End\nFunction\n(Italian)"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add edition buttons
  pbAddLink = new QPushButton(tr("Add link ..."));
  connect(pbAddLink, SIGNAL(clicked()), this, SLOT(addLink()));
  widget->addWidgetToLocalBar(pbAddLink);
  pbEditLink = new QPushButton(tr("Edit link ..."));
  connect(pbEditLink, SIGNAL(clicked()), this, SLOT(editLink()));
  widget->addWidgetToLocalBar(pbEditLink);
  pbRemoveLinks = new QPushButton(tr("Remove links"));
  connect(pbRemoveLinks, SIGNAL(clicked()), this, SLOT(removeLinks()));
  widget->addWidgetToLocalBar(pbRemoveLinks);
  widget->addStretchToLocalBar();

  return true;
}
