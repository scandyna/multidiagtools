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
#include "mdtClLinkEditor.h"
#include "mdtSqlWindow.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlRelation.h"
#include "mdtError.h"
#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QPushButton>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDataWidgetMapper>
#include <QTableView>

#include <QDebug>

mdtClLinkEditor::mdtClLinkEditor(QObject *parent, QSqlDatabase db)
 : QObject(parent)
{
  pvDatabase = db;
  // Setup Link
  pvLinkModel = new QSqlTableModel(this, pvDatabase);
  pvLinkWidget = new mdtSqlFormWidget;
  pvLinkUiWidget = new Ui::mdtClLinkEditor;
  pvLinkUiWidget->setupUi(pvLinkWidget);
  // Setup Article
  pvArticleWidgetMapper = new QDataWidgetMapper(this);
  pvArticleModel = new QSqlTableModel(this, pvDatabase);
  pvArticleLinkRelation = new mdtSqlRelation;
  // Setup Start Unit
  pvStartUnitWidget = new mdtSqlTableWidget;
  pvStartUnitConnectionWidget = new mdtSqlTableWidget;
  pvStartUnitModel = new QSqlTableModel(this, pvDatabase);
  pvStartUnitConnectionModel = new QSqlTableModel(this, pvDatabase);
  pvStartUnitConnectionRelation = new mdtSqlRelation;
  // Setup End Unit
  pvEndUnitWidget = new mdtSqlTableWidget;
  pvEndUnitConnectionWidget = new mdtSqlTableWidget;
  pvEndUnitModel = new QSqlTableModel(this, pvDatabase);
  pvEndUnitConnectionModel = new QSqlTableModel(this, pvDatabase);
  pvEndUnitConnectionRelation = new mdtSqlRelation;
  // Setup connection buttons
  connect(pvLinkUiWidget->pbConnect, SIGNAL(clicked()), this, SLOT(connectLink()));
  // Setup List
  pvListWidget = new mdtSqlTableWidget;
  pvListModel = new QSqlTableModel(this, pvDatabase);

}

mdtClLinkEditor::~mdtClLinkEditor()
{
  delete pvArticleLinkRelation;
  delete pvStartUnitConnectionRelation;
  delete pvEndUnitConnectionRelation;
}

bool mdtClLinkEditor::setupTables()
{
  if(!setupLinkTable()){
    return false;
  }
  if(!setupArticleTable()){
    return false;
  }
  if(!setupStartUnitTables()){
    return false;
  }
  if(!setupEndUnitTables()){
    return false;
  }
  if(!setupListTable()){
    return false;
  }

  return true;
}

void mdtClLinkEditor::setupUi(mdtSqlWindow *window)
{
  Q_ASSERT(window != 0);

  window->setSqlWidget(pvLinkWidget);
  window->enableNavigation();
  window->enableEdition();
  window->resize(950, 700);
}

void mdtClLinkEditor::connectLink()
{
  Q_ASSERT(pvStartUnitConnectionWidget->selectionModel() != 0);
  Q_ASSERT(pvEndUnitConnectionWidget->selectionModel() != 0);

  QModelIndexList startSelectedIndexes;
  QVariant startSelectedConnectionId;
  QModelIndexList endSelectedIndexes;
  QVariant endSelectedConnectionId;
  QModelIndex index;
  QSqlError sqlError;

  // Check that we are on a valid link
  if(pvLinkWidget->currentRow() < 0){
    QMessageBox msgBox;
    msgBox.setText(tr("Cannot create connection"));
    msgBox.setInformativeText(tr("Please select a link or create a new one"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
    return;
  }
  // Get start selected connection ID
  startSelectedIndexes = pvStartUnitConnectionWidget->selectionModel()->selectedRows();
  if(startSelectedIndexes.size() != 1){
    return;
  }
  index = pvStartUnitConnectionModel->index(startSelectedIndexes.at(0).row(), pvStartUnitConnectionModel->fieldIndex("Id_PK"));
  startSelectedConnectionId = pvStartUnitConnectionModel->data(index);
  qDebug() << "Start ID: " << startSelectedConnectionId;
  // Get end selected connection ID
  endSelectedIndexes = pvEndUnitConnectionWidget->selectionModel()->selectedRows();
  if(endSelectedIndexes.size() != 1){
    return;
  }
  index = pvEndUnitConnectionModel->index(endSelectedIndexes.at(0).row(), pvEndUnitConnectionModel->fieldIndex("Id_PK"));
  endSelectedConnectionId = pvEndUnitConnectionModel->data(index);
  qDebug() << "End ID: " << endSelectedConnectionId;
  // Set start ID in Link table
  index = pvLinkModel->index(pvLinkWidget->currentRow(), pvLinkModel->fieldIndex("UnitConnectionStart_Id_FK"));
  if(!pvLinkModel->setData(index, startSelectedConnectionId)){
    sqlError = pvLinkModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to set start connect ID in table 'Link_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    QMessageBox msgBox;
    msgBox.setText("A error occured when trying to set Start connection.");
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Set end ID in Link table
  index = pvLinkModel->index(pvLinkWidget->currentRow(), pvLinkModel->fieldIndex("UnitConnectionEnd_Id_FK"));
  if(!pvLinkModel->setData(index, endSelectedConnectionId)){
    sqlError = pvLinkModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to set end connect ID in table 'Link_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    QMessageBox msgBox;
    msgBox.setText("A error occured when trying to set Start connection.");
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  ///pvListModel->select();
  emit linkEdited();
}

void mdtClLinkEditor::updateUnitConnectionSelections(int linkRow)
{
  QVariant startUnitId;
  QVariant startConnectionId;
  QVariant endUnitId;
  QVariant endConnectionId;
  QVariant currentLinkId;
  QModelIndex index;
  QSqlQuery query(pvDatabase);
  QSqlError sqlError;

  // Get current Link ID
  if(linkRow < 0){
    pvStartUnitWidget->setCurrentIndex(-1);
    pvStartUnitConnectionWidget->setCurrentIndex(-1);
    pvEndUnitWidget->setCurrentIndex(-1);
    pvEndUnitConnectionWidget->setCurrentIndex(-1);
    return;
  }
  index = pvLinkModel->index(linkRow, pvLinkModel->fieldIndex("Id_PK"));
  if(!index.isValid()){
    pvStartUnitWidget->setCurrentIndex(-1);
    pvStartUnitConnectionWidget->setCurrentIndex(-1);
    pvEndUnitWidget->setCurrentIndex(-1);
    pvEndUnitConnectionWidget->setCurrentIndex(-1);
    return;
  }
  currentLinkId = pvLinkModel->data(index);
  if(!currentLinkId.isValid()){
    pvStartUnitWidget->setCurrentIndex(-1);
    pvStartUnitConnectionWidget->setCurrentIndex(-1);
    pvEndUnitWidget->setCurrentIndex(-1);
    pvEndUnitConnectionWidget->setCurrentIndex(-1);
    return;
  }
  // Get start Unit ID
  if(!query.exec("SELECT UnitStart_Id_PK FROM LinkList_view WHERE Link_Id_PK = " + currentLinkId.toString())){
    sqlError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to get UnitStart_Id_PK in table 'LinkList_view'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    pvStartUnitWidget->setCurrentIndex(-1);
    return;
  }
  if(query.next()){
    startUnitId = query.value(0);
  }
  // Get start Connection ID
  if(!query.exec("SELECT UnitConnectionStart_Id_FK FROM LinkList_view WHERE Link_Id_PK = " + currentLinkId.toString())){
    sqlError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to get UnitStart_Id_PK in table 'LinkList_view'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    pvStartUnitConnectionWidget->setCurrentIndex(-1);
    return;
  }
  if(query.next()){
    startConnectionId = query.value(0);
  }
  // Get end Unit ID
  if(!query.exec("SELECT UnitEnd_Id_PK FROM LinkList_view WHERE Link_Id_PK = " + currentLinkId.toString())){
    sqlError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to get UnitStart_Id_PK in table 'LinkList_view'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    pvEndUnitWidget->setCurrentIndex(-1);
    return;
  }
  if(query.next()){
    endUnitId = query.value(0);
  }
  // Get end Connection ID
  if(!query.exec("SELECT UnitConnectionEnd_Id_FK FROM LinkList_view WHERE Link_Id_PK = " + currentLinkId.toString())){
    sqlError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to get UnitStart_Id_PK in table 'LinkList_view'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    pvEndUnitConnectionWidget->setCurrentIndex(-1);
    return;
  }
  if(query.next()){
    endConnectionId = query.value(0);
  }
  // Set Unit and connect current
  if(startUnitId.isValid() && startConnectionId.isValid()){
    pvStartUnitWidget->setCurrentRecord("Unit_Id_FK", startUnitId);
    pvStartUnitConnectionWidget->setCurrentRecord("Id_PK", startConnectionId);
  }
  if((endUnitId.isValid()) && (endConnectionId.isValid())){
    pvEndUnitWidget->setCurrentRecord("Unit_Id_FK", endUnitId);
    pvEndUnitConnectionWidget->setCurrentRecord("Id_PK", endConnectionId);
  }
}

void mdtClLinkEditor::updateListView()
{
  pvListModel->select();
}

void mdtClLinkEditor::setCurrentRowByList(int listRow)
{
  QVariant linkId;
  QModelIndex index;

  // If we are saving current Link, we do nothing
  if(pvLinkWidget->currentState() != mdtAbstractSqlWidget::Visualizing){
    return;
  }
  // Get ID of current row in List
  index = pvListModel->index(listRow, pvListModel->fieldIndex("Link_Id_PK"));
  if(!index.isValid()){
    return;
  }
  linkId = pvListModel->data(index);
  if(!linkId.isValid()){
    return;
  }
  // Set current index
  pvLinkWidget->setCurrentRecord("Id_PK", linkId);
}

bool mdtClLinkEditor::setupLinkTable()
{
  QSqlError sqlError;

  pvLinkModel->setTable("Link_tbl");
  if(!pvLinkModel->select()){
    sqlError = pvLinkModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'Link_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    return false;
  }
  pvLinkWidget->setModel(pvLinkModel);
  pvLinkWidget->mapFormWidgets();
  connect(pvLinkWidget, SIGNAL(currentRowChanged(int)), this, SLOT(updateUnitConnectionSelections(int)));
  connect(this, SIGNAL(linkEdited()), pvLinkWidget, SIGNAL(dataEdited()));

  return true;
}

bool mdtClLinkEditor::setupArticleTable()
{
  QSqlError sqlError;

  pvArticleModel->setTable("Article_tbl");
  if(!pvArticleModel->select()){
    sqlError = pvArticleModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'Article_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    return false;
  }
  /*
   * Setup Link <-> Article relation
   * Here we make it reversed,
   *  because the parent table is the Article table,
   *  but we need to display the article on witch
   *  current Link is based
   */
  pvArticleLinkRelation->setParentModel(pvLinkModel);
  pvArticleLinkRelation->setChildModel(pvArticleModel);
  if(!pvArticleLinkRelation->addRelation("Article_Id_FK", "Id_PK")){
    return false;
  }
  // Setup widget mapper
  pvArticleWidgetMapper->setModel(pvArticleModel);
  pvArticleWidgetMapper->addMapping(pvLinkUiWidget->leArticleCode, pvArticleModel->fieldIndex("ArticleCode"));
  pvArticleWidgetMapper->addMapping(pvLinkUiWidget->leArticleDesignationEN, pvArticleModel->fieldIndex("DesignationEN"));
  // We must catch link index changed + article model select to update widget mapper
  connect(pvLinkWidget, SIGNAL(currentRowChanged(int)), pvArticleLinkRelation, SLOT(setParentCurrentIndex(int)));
  connect(pvLinkWidget, SIGNAL(modelSelected()), pvArticleWidgetMapper, SLOT(toFirst()));
  // Force a update of relation
  pvArticleLinkRelation->setParentCurrentIndex(pvLinkWidget->currentRow());

  return true;
}

bool mdtClLinkEditor::setupStartUnitTables()
{
  QSqlError sqlError;
  QVBoxLayout *vLayout;
  QLabel *lb;

  // Setup labels
  vLayout = new QVBoxLayout;
  // Setup Unit model
  pvStartUnitModel->setTable("VehicleType_Unit_view");
  if(!pvStartUnitModel->select()){
    sqlError = pvStartUnitModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'VehicleType_Unit_view'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    return false;
  }
  // Setup Unit widget
  lb = new QLabel(tr("Units"));
  vLayout->addWidget(lb);
  pvStartUnitWidget->setModel(pvStartUnitModel);
  pvStartUnitWidget->setEditionEnabled(false);
  Q_ASSERT(pvStartUnitWidget->tableView() != 0);
  pvStartUnitWidget->tableView()->setSelectionBehavior(QAbstractItemView::SelectRows);
  pvStartUnitWidget->tableView()->setSelectionMode(QAbstractItemView::SingleSelection);
  pvStartUnitWidget->setColumnHidden("VehicleType_Id_FK", true);
  pvStartUnitWidget->setColumnHidden("Unit_Id_FK", true);
  pvStartUnitWidget->setDefaultColumnToSelect("SchemaPosition");
  vLayout->addWidget(pvStartUnitWidget);
  // Setup Unit Connection model
  pvStartUnitConnectionModel->setTable("UnitConnection_tbl");
  if(!pvStartUnitConnectionModel->select()){
    sqlError = pvStartUnitConnectionModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'UnitConnection_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    return false;
  }
  // Setup Unit Connection widget
  lb = new QLabel(tr("Connections"));
  vLayout->addWidget(lb);
  pvStartUnitConnectionWidget->setModel(pvStartUnitConnectionModel);
  pvStartUnitConnectionWidget->setEditionEnabled(false);
  Q_ASSERT(pvStartUnitConnectionWidget->tableView() != 0);
  pvStartUnitConnectionWidget->tableView()->setSelectionBehavior(QAbstractItemView::SelectRows);
  pvStartUnitConnectionWidget->tableView()->setSelectionMode(QAbstractItemView::SingleSelection);
  pvStartUnitConnectionWidget->setColumnHidden("Id_PK", true);
  pvStartUnitConnectionWidget->setColumnHidden("Unit_Id_FK", true);
  pvStartUnitConnectionWidget->setColumnHidden("ArticleConnection_Id_FK", true);
  pvStartUnitConnectionWidget->setDefaultColumnToSelect("ConnectorName");
  vLayout->addWidget(pvStartUnitConnectionWidget);
  pvLinkUiWidget->gbStart->setLayout(vLayout);
  // Setup Unit <-> Connection relation
  pvStartUnitConnectionRelation->setParentModel(pvStartUnitModel);
  pvStartUnitConnectionRelation->setChildModel(pvStartUnitConnectionModel);
  if(!pvStartUnitConnectionRelation->addRelation("Unit_Id_FK", "Unit_Id_FK")){
    return false;
  }
  pvStartUnitWidget->addChildWidget(pvStartUnitConnectionWidget, pvStartUnitConnectionRelation);

  return true;
}

bool mdtClLinkEditor::setupEndUnitTables()
{
  QSqlError sqlError;
  ///QHBoxLayout *hLayout;
  QVBoxLayout *vLayout;
  QLabel *lb;

  // Setup labels
  vLayout = new QVBoxLayout;
  ///hLayout = new QHBoxLayout;
  ///vLayout->addLayout(hLayout);
  ///hLayout = new QHBoxLayout;
  // Setup Unit model
  pvEndUnitModel->setTable("VehicleType_Unit_view");
  if(!pvEndUnitModel->select()){
    sqlError = pvEndUnitModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'VehicleType_Unit_view'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    return false;
  }
  // Setup Unit widget
  lb = new QLabel(tr("Unit"));
  vLayout->addWidget(lb);
  pvEndUnitWidget->setModel(pvEndUnitModel);
  pvEndUnitWidget->setEditionEnabled(false);
  Q_ASSERT(pvEndUnitWidget->tableView() != 0);
  pvEndUnitWidget->tableView()->setSelectionBehavior(QAbstractItemView::SelectRows);
  pvEndUnitWidget->tableView()->setSelectionMode(QAbstractItemView::SingleSelection);
  pvEndUnitWidget->setColumnHidden("VehicleType_Id_FK", true);
  pvEndUnitWidget->setColumnHidden("Unit_Id_FK", true);
  pvEndUnitWidget->setDefaultColumnToSelect("SchemaPosition");
  vLayout->addWidget(pvEndUnitWidget);
  // Setup Unit Connection model
  pvEndUnitConnectionModel->setTable("UnitConnection_tbl");
  if(!pvEndUnitConnectionModel->select()){
    sqlError = pvEndUnitConnectionModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'UnitConnection_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    return false;
  }
  // Setup Unit Connection widget
  lb = new QLabel(tr("Connections"));
  vLayout->addWidget(lb);
  pvEndUnitConnectionWidget->setModel(pvEndUnitConnectionModel);
  pvEndUnitConnectionWidget->setEditionEnabled(false);
  Q_ASSERT(pvEndUnitConnectionWidget->tableView() != 0);
  pvEndUnitConnectionWidget->tableView()->setSelectionBehavior(QAbstractItemView::SelectRows);
  pvEndUnitConnectionWidget->tableView()->setSelectionMode(QAbstractItemView::SingleSelection);
  pvEndUnitConnectionWidget->setColumnHidden("Id_PK", true);
  pvEndUnitConnectionWidget->setColumnHidden("Unit_Id_FK", true);
  pvEndUnitConnectionWidget->setColumnHidden("ArticleConnection_Id_FK", true);
  pvEndUnitConnectionWidget->setDefaultColumnToSelect("ConnectorName");
  vLayout->addWidget(pvEndUnitConnectionWidget);
  ///vLayout->addLayout(hLayout);
  pvLinkUiWidget->gbEnd->setLayout(vLayout);
  // Setup Unit <-> Connection relation
  pvEndUnitConnectionRelation->setParentModel(pvEndUnitModel);
  pvEndUnitConnectionRelation->setChildModel(pvEndUnitConnectionModel);
  if(!pvEndUnitConnectionRelation->addRelation("Unit_Id_FK", "Unit_Id_FK")){
    return false;
  }
  pvEndUnitWidget->addChildWidget(pvEndUnitConnectionWidget, pvEndUnitConnectionRelation);

  return true;
}

bool mdtClLinkEditor::setupListTable()
{
  Q_ASSERT(pvLinkWidget->layout() != 0);

  QSqlError sqlError;
  QVBoxLayout *vLayout = dynamic_cast<QVBoxLayout*>(pvLinkWidget->layout());
  Q_ASSERT(vLayout != 0);

  pvListModel->setTable("LinkList_view");
  if(!pvListModel->select()){
    sqlError = pvListModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'LinkList_view'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    return false;
  }
  pvListWidget->setModel(pvListModel);
  pvListWidget->setEditionEnabled(false);
  pvListWidget->setColumnHidden("Link_Id_PK", true);
  pvListWidget->setColumnHidden("UnitStart_Id_PK", true);
  pvListWidget->setColumnHidden("UnitEnd_Id_PK", true);
  pvListWidget->setColumnHidden("UnitConnectionStart_Id_FK", true);
  pvListWidget->setColumnHidden("UnitConnectionEnd_Id_FK", true);
  pvListWidget->setHeaderData("VehicleType", "Vehicle");
  pvListWidget->setHeaderData("VehicleSubType", "Variant");
  pvListWidget->setHeaderData("VehicleSerie", "Serie");
  pvListWidget->setHeaderData("StartSchemaPosition", "Start\nSch. pos.");
  pvListWidget->setHeaderData("StartCabinet", "Start\nCabinet");
  pvListWidget->setHeaderData("StartCoordinate", "Start\nCoordiante");
  pvListWidget->setHeaderData("StartConnectorName", "Start\nConnector");
  pvListWidget->setHeaderData("StartContactName", "Start\nContact");
  pvListWidget->setHeaderData("EndSchemaPosition", "End\nSch. pos.");
  pvListWidget->setHeaderData("EndCabinet", "End\nCabinet");
  pvListWidget->setHeaderData("EndCoordinate", "End\nCoordiante");
  pvListWidget->setHeaderData("EndConnectorName", "End\nConnector");
  pvListWidget->setHeaderData("EndContactName", "End\nContact");
  connect(pvListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(setCurrentRowByList(int)));
  connect(pvLinkWidget, SIGNAL(modelSelected()), this, SLOT(updateListView()));
  vLayout->addWidget(pvListWidget);

  return true;
}
