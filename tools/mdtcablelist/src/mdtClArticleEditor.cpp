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
#include "mdtClArticleEditor.h"
#include "ui_mdtClArticleEditor.h"
#include "mdtSqlWindow.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtAbstractSqlWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtError.h"
#include "mdtClArticleComponentDialog.h"
#include "mdtClArticleConnectionData.h"
#include "mdtClArticleConnectionDialog.h"
#include "mdtClArticleLinkDialog.h"
#include "mdtClArticle.h"
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
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

mdtClArticleEditor::mdtClArticleEditor(QObject *parent, QSqlDatabase db)
 : mdtClEditor(parent, db)
{
}

mdtClArticleEditor::~mdtClArticleEditor()
{
  delete form();
}

void mdtClArticleEditor::addComponent()
{
  mdtClArticleComponentDialog dialog(0, database(), currentArticleId());
  mdtClArticle art(database());

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
  form()->select("ArticleComponent_view");
}

void mdtClArticleEditor::editComponent()
{
  mdtSqlTableWidget *widget;
  QVariant currentArtId;
  QVariant currentComponentId;
  mdtClArticle art(database());

  widget = form()->sqlTableWidget("ArticleComponent_view");
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
  form()->select("ArticleComponent_view");
}

void mdtClArticleEditor::editComponent(const QModelIndex &index)
{
  editComponent();
}

void mdtClArticleEditor::removeComponents()
{
  mdtSqlTableWidget *widget;
  mdtClArticle art(database());
  QMessageBox msgBox;
  QModelIndexList indexes;
  QSqlError sqlError;
  int ret;

  widget = form()->sqlTableWidget("ArticleComponent_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  indexes = widget->indexListOfSelectedRows("Component_Id_PK");
  if(indexes.size() < 1){
    return;
  }
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
  if(!art.removeComponents(currentArticleId(), indexes)){
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
  form()->select("ArticleComponent_view");
}

void mdtClArticleEditor::addConnection()
{
  QVariant articleId;
  mdtClArticleConnectionData data;
  mdtClArticleConnectionDialog dialog;
  mdtClArticle art(database());
  QSqlQueryModel model;
  QString sql;

  articleId = currentArticleId();
  if(articleId.isNull()){
    return;
  }
  // Setup and show dialog
  sql = "SELECT Id_PK, Name FROM ArticleConnector_tbl WHERE Article_Id_FK = " + currentArticleId().toString();
  model.setQuery(sql, database());
  dialog.setArticleConnectorModel(&model);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Get and update data
  data = dialog.data();
  data.setArticleId(articleId);
  // Add connection
  if(!art.addConnection(data)){
    pvLastError = art.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  form()->select("ArticleConnection_view");
}

void mdtClArticleEditor::removeConnections()
{
  mdtSqlTableWidget *widget;
  mdtClArticle art(database());
  QMessageBox msgBox;
  QModelIndexList indexes;
  QSqlError sqlError;
  int ret;

  widget = form()->sqlTableWidget("ArticleConnection_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  indexes = widget->indexListOfSelectedRows("Id_PK");
  if(indexes.size() < 1){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove connections from current article."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  ret = msgBox.exec();
  if(ret != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!art.removeConnections(indexes)){
    pvLastError = art.lastError();
    displayLastError();
    return;
  }
  // Update component table
  form()->select("ArticleConnection_view");
}

void mdtClArticleEditor::addConnector()
{
  QVariant articleId;
  QVariant connectorId;
  QString connectorName;
  QList<QVariant> selectedContacts;
  QList<mdtClArticleConnectionData> dataList;
  mdtClArticle art(database());
  int i;

  articleId = currentArticleId();
  if(articleId.isNull()){
    return;
  }
  // Let user choose connector
  connectorId = selectConnector();
  if(connectorId.isNull()){
    return;
  }
  // Let user give a connector name
  QInputDialog dialog;
  dialog.setLabelText(tr("Connector name:"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  connectorName = dialog.textValue().trimmed();
  if(connectorName.isEmpty()){
    return;
  }
  // Let user choose connector contacts
  selectedContacts = selectConnectorContacts(connectorId);
  if(selectedContacts.isEmpty()){
    return;
  }
  // Get contact data and add connector to table
  dataList = art.connectorContactData(selectedContacts);
  for(i = 0; i < dataList.size(); ++i){
    dataList[i].setArticleId(articleId);
    dataList[i].setConnectorName(connectorName);
  }
  if(!art.addConnector(dataList)){
    pvLastError = art.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  form()->select("ArticleConnection_view");
}

void mdtClArticleEditor::removeConnectors()
{
  QList<QVariant> articleConnectorIdList;
  mdtClArticle art(database());
  QMessageBox msgBox;

  // Let user select article connectors
  articleConnectorIdList = selectArticleConnectors();
  if(articleConnectorIdList.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove connectors and all related connections from current article."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove connectors
  if(!art.removeConnectors(articleConnectorIdList)){
    pvLastError = art.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  form()->select("ArticleConnection_view");
}

void mdtClArticleEditor::addLink()
{
  mdtClArticleLinkDialog dialog(0, database(), currentArticleId());
  mdtClArticle art(database());

  // Check if some connection exists
  if(form()->rowCount("ArticleConnection_view") < 1){
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
  if(!art.addLink(dialog.startConnectionId(), dialog.endConnectionId(), dialog.value().toDouble(), dialog.linkDirectionCode(), dialog.linkTypeCode())){
    QMessageBox msgBox;
    msgBox.setText(tr("Link insertion failed"));
    msgBox.setInformativeText(tr("Please see details for more informations"));
    msgBox.setDetailedText(art.lastError().text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update link table
  form()->select("ArticleLink_view");
}

void mdtClArticleEditor::editLink()
{
  mdtClArticleLinkDialog dialog(0, database(), currentArticleId());
  int row;
  mdtAbstractSqlWidget *widget;
  mdtClArticle art(database());

  if(currentArticleId().isNull()){
    return;
  }
  widget = form()->sqlWidget("ArticleLink_view");
  Q_ASSERT(widget != 0);
  // Check that a link is selected
  row = widget->rowCount();
  if(row < 0){
    return;
  }
  // Setup and show dialog
  dialog.setLinkTypeCode(widget->currentData("LinkType_Code_FK"));
  dialog.setLinkDirectionCode(widget->currentData("LinkDirection_Code_FK"));
  dialog.setValue(widget->currentData("Value"));
  dialog.setStartConnectionId(widget->currentData("ArticleConnectionStart_Id_FK"));
  dialog.setEndConnectionId(widget->currentData("ArticleConnectionEnd_Id_FK"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Edit link
  if(!art.editLink(widget->currentData("ArticleConnectionStart_Id_FK"), widget->currentData("ArticleConnectionEnd_Id_FK"), dialog.linkData())){
    QMessageBox msgBox;
    msgBox.setText(tr("Link insertion failed"));
    msgBox.setInformativeText(tr("Please see details for more informations"));
    msgBox.setDetailedText(art.lastError().text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update link table
  form()->select("ArticleLink_view");
}

void mdtClArticleEditor::removeLinks()
{
  mdtSqlTableWidget *widget;
  mdtClArticle art(database());
  QMessageBox msgBox;
  QStringList fields;
  QList<QModelIndexList> indexes;
  ///QModelIndexList indexes;
  QSqlError sqlError;
  int ret;

  widget = form()->sqlTableWidget("ArticleLink_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  fields << "ArticleConnectionStart_Id_FK" << "ArticleConnectionEnd_Id_FK";
  indexes = widget->indexListOfSelectedRowsByRowsList(fields);
  if(indexes.size() < 1){
    return;
  }
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
  if(!art.removeLinks(indexes)){
    ///sqlError = art.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Links removing failed."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(art.lastError().text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update link table
  form()->select("ArticleLink_view");
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
  Q_ASSERT(form() != 0);

  return form()->currentData("Article_tbl", "Id_PK");
}

QVariant mdtClArticleEditor::selectConnector()
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QVariant articleId;
  QSqlQueryModel model;
  QString sql;

  // Get current article ID
  articleId = currentArticleId();
  if(articleId.isNull()){
    return QVariant();
  }
  // Setup model to show available connectors
  sql = "SELECT * FROM Connector_tbl";
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get connectors list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticleEditor");
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

QList<QVariant> mdtClArticleEditor::selectConnectorContacts(const QVariant &connectorId)
{
  QList<QVariant> contactIds;
  QModelIndexList selectedItems;
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QVariant articleId;
  QSqlQueryModel model;
  QString sql;
  int i;

  if(connectorId.isNull()){
    return contactIds;
  }
  // Setup model to show available contacts
  sql = "SELECT * FROM ConnectorContact_tbl ";
  sql += "WHERE Connector_Id_FK = " + connectorId.toString();
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get contacts list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    displayLastError();
    return contactIds;
  }
  // Setup and show dialog
  selectionDialog.setMessage("Please select contacts.");
  selectionDialog.setModel(&model, true);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Connector_Id_FK", true);
  ///selectionDialog.setHeaderData("", tr(""));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return contactIds;
  }
  selectedItems = selectionDialog.selectionResults();
  for(i = 0; i < selectedItems.size(); ++i){
    contactIds.append(selectedItems.at(i).data());
  }

  return contactIds;
}

QList<QVariant> mdtClArticleEditor::selectArticleConnectors()
{
  QString sql;
  QSqlQueryModel model;
  QSqlError sqlError;
  QVariant articleId;
  QModelIndexList selectedItems;
  QList<QVariant> articleConnectorIdList;
  mdtSqlSelectionDialog selectionDialog;
  int i;

  // Get current article ID
  articleId = currentArticleId();
  if(articleId.isNull()){
    return articleConnectorIdList;
  }
  // Setup model to list current article related connectors
  sql = "SELECT Id_PK, Name FROM ArticleConnector_tbl WHERE Article_Id_FK = " + articleId.toString();
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get to current article related connectors list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    displayLastError();
    return articleConnectorIdList;
  }
  if(model.rowCount() < 1){
    return articleConnectorIdList;
  }
  // Setup and show dialog
  selectionDialog.setMessage("Please select connectors to remove.");
  selectionDialog.setModel(&model, true);
  selectionDialog.setColumnHidden("Id_PK", true);
  ///selectionDialog.setHeaderData("", tr(""));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return articleConnectorIdList;
  }
  selectedItems = selectionDialog.selectionResults();
  for(i = 0; i < selectedItems.size(); ++i){
    articleConnectorIdList.append(selectedItems.at(i).data());
  }

  return articleConnectorIdList;
}

bool mdtClArticleEditor::setupArticleTable()
{
  Q_ASSERT(form() != 0);

  Ui::mdtClArticleEditor ae;

  // Setup main form widget
  ae.setupUi(form()->mainSqlWidget());
  ///connect(this, SIGNAL(unitEdited()), form()->mainSqlWidget(), SIGNAL(dataEdited()));
  // Setup form
  if(!form()->setTable("Article_tbl", "Article", database())){
    return false;
  }
  if(sqlWindow() != 0){
    sqlWindow()->enableNavigation();
    sqlWindow()->enableEdition();
    sqlWindow()->resize(800, 500);
    sqlWindow()->setWindowTitle(tr("Article edition"));
  }

  return true;
}

bool mdtClArticleEditor::setupArticleComponentTable()
{
  Q_ASSERT(form() != 0);

  mdtSqlTableWidget *widget;
  QPushButton *pbAddComponent;
  QPushButton *pbEditComponent;
  QPushButton *pbRemoveComponents;

  if(!form()->addChildTable("ArticleComponent_view", tr("Components"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "ArticleComponent_view", "Article_Id_PK")){
    return false;
  }
  widget = form()->sqlTableWidget("ArticleComponent_view");
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
  Q_ASSERT(form() != 0);

  mdtSqlTableWidget *widget;

  if(!form()->addChildTable("ArticleComponentUsage_view", tr("Used by"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "ArticleComponentUsage_view", "Component_Id_PK")){
    return false;
  }
  widget = form()->sqlTableWidget("ArticleComponentUsage_view");
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

bool mdtClArticleEditor::setupArticleConnectionTable()
{
  Q_ASSERT(form() != 0);

  mdtSqlTableWidget *widget;
  QPushButton *pbAddConnector;
  QPushButton *pbRemoveConnectors;
  QPushButton *pbAddConnection;
  QPushButton *pbRemoveConnections;

  if(!form()->addChildTable("ArticleConnection_view", tr("Connections"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "ArticleConnection_view", "Article_Id_FK")){
    return false;
  }
  widget = form()->sqlTableWidget("ArticleConnection_view");
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
  pbAddConnector = new QPushButton(tr("Add connector ..."));
  connect(pbAddConnector, SIGNAL(clicked()), this, SLOT(addConnector()));
  widget->addWidgetToLocalBar(pbAddConnector);
  pbRemoveConnectors = new QPushButton(tr("Remove connectors"));
  connect(pbRemoveConnectors, SIGNAL(clicked()), this, SLOT(removeConnectors()));
  widget->addWidgetToLocalBar(pbRemoveConnectors);
  pbAddConnection = new QPushButton(tr("Add connection ..."));
  connect(pbAddConnection, SIGNAL(clicked()), this, SLOT(addConnection()));
  widget->addWidgetToLocalBar(pbAddConnection);
  pbRemoveConnections = new QPushButton(tr("Remove connections"));
  connect(pbRemoveConnections, SIGNAL(clicked()), this, SLOT(removeConnections()));
  widget->addWidgetToLocalBar(pbRemoveConnections);
  widget->addStretchToLocalBar();

  return true;
}

bool mdtClArticleEditor::setupArticleLinkTable()
{
  Q_ASSERT(form() != 0);

  mdtSqlTableWidget *widget;
  QPushButton *pbAddLink;
  QPushButton *pbEditLink;
  QPushButton *pbRemoveLinks;

  if(!form()->addChildTable("ArticleLink_view", tr("Links"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "ArticleLink_view", "StartArticle_Id_FK")){
    return false;
  }
  if(!form()->addRelation("Id_PK", "ArticleLink_view", "EndArticle_Id_FK")){
    return false;
  }
  widget = form()->sqlTableWidget("ArticleLink_view");
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
