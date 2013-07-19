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
#include "mdtClUnitConnectionSelectionDialog.h"
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
#include <QLineEdit>

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
  pvStartUnitModel = new QSqlTableModel(this, pvDatabase);
  pvStartUnitWidgetMapper = new QDataWidgetMapper(this);
  pvStartUnitConnectionModel = new QSqlTableModel(this, pvDatabase);
  pvStartUnitConnectionWidgetMapper = new QDataWidgetMapper(this);
  // Setup End Unit
  pvEndUnitModel = new QSqlTableModel(this, pvDatabase);
  pvEndUnitWidgetMapper = new QDataWidgetMapper(this);
  pvEndUnitConnectionModel = new QSqlTableModel(this, pvDatabase);
  pvEndUnitConnectionWidgetMapper = new QDataWidgetMapper(this);
  // Setup connection buttons
  connect(pvLinkUiWidget->pbSelectStartConnection, SIGNAL(clicked()), this, SLOT(selectStartConnection()));
  connect(pvLinkUiWidget->pbSelectEndConnection, SIGNAL(clicked()), this, SLOT(selectEndConnection()));
  // Setup List
  pvListWidget = new mdtSqlTableWidget;
  pvListModel = new QSqlTableModel(this, pvDatabase);

}

mdtClLinkEditor::~mdtClLinkEditor()
{
  delete pvArticleLinkRelation;
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

/**
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
  emit linkEdited();
}
*/

void mdtClLinkEditor::selectStartConnection()
{
  QVariant selectedConnectionId;
  QModelIndex index;
  QSqlError sqlError;
  mdtClUnitConnectionSelectionDialog dialog;
  QWidget *w;
  QLineEdit *le;

  // Show selection dialog to user
  
  dialog.exec();
  selectedConnectionId = dialog.selectedConnectionId();
  if(!selectedConnectionId.isValid()){
    return;
  }
  // Set start ID in Link table
  index = pvLinkModel->index(pvLinkWidget->currentRow(), pvLinkModel->fieldIndex("UnitConnectionStart_Id_FK"));
  if(!pvLinkModel->setData(index, selectedConnectionId)){
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
  // Update widgets
  w = pvStartUnitWidgetMapper->mappedWidgetAt(pvStartUnitModel->fieldIndex("SchemaPosition"));
  if(w != 0){
    le = dynamic_cast<QLineEdit*>(w);
    if(le != 0){
      le->setText(dialog.selectedSchemaPosition().toString());
    }
  }

  w = pvStartUnitConnectionWidgetMapper->mappedWidgetAt(pvStartUnitConnectionModel->fieldIndex("ConnectorName"));
  if(w != 0){
    le = dynamic_cast<QLineEdit*>(w);
    if(le != 0){
      le->setText(dialog.selectedConnectorName().toString());
    }
  }
  w = pvStartUnitConnectionWidgetMapper->mappedWidgetAt(pvStartUnitConnectionModel->fieldIndex("ContactName"));
  if(w != 0){
    le = dynamic_cast<QLineEdit*>(w);
    if(le != 0){
      le->setText(dialog.selectedContactName().toString());
    }
  }

  emit linkEdited();
}

void mdtClLinkEditor::selectEndConnection()
{
  QVariant selectedConnectionId;
  QModelIndex index;
  QSqlError sqlError;
  mdtClUnitConnectionSelectionDialog dialog;
  QWidget *w;
  QLineEdit *le;

  // Show selection dialog to user
  
  dialog.exec();
  selectedConnectionId = dialog.selectedConnectionId();
  if(!selectedConnectionId.isValid()){
    return;
  }
  // Set end ID in Link table
  index = pvLinkModel->index(pvLinkWidget->currentRow(), pvLinkModel->fieldIndex("UnitConnectionEnd_Id_FK"));
  if(!pvLinkModel->setData(index, selectedConnectionId)){
    sqlError = pvLinkModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to set end connect ID in table 'Link_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    QMessageBox msgBox;
    msgBox.setText("A error occured when trying to set End connection.");
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update widgets
  w = pvEndUnitWidgetMapper->mappedWidgetAt(pvEndUnitModel->fieldIndex("SchemaPosition"));
  if(w != 0){
    le = dynamic_cast<QLineEdit*>(w);
    if(le != 0){
      le->setText(dialog.selectedSchemaPosition().toString());
    }
  }

  w = pvEndUnitConnectionWidgetMapper->mappedWidgetAt(pvEndUnitConnectionModel->fieldIndex("ConnectorName"));
  if(w != 0){
    le = dynamic_cast<QLineEdit*>(w);
    if(le != 0){
      le->setText(dialog.selectedConnectorName().toString());
    }
  }
  w = pvEndUnitConnectionWidgetMapper->mappedWidgetAt(pvEndUnitConnectionModel->fieldIndex("ContactName"));
  if(w != 0){
    le = dynamic_cast<QLineEdit*>(w);
    if(le != 0){
      le->setText(dialog.selectedContactName().toString());
    }
  }

  emit linkEdited();
}

void mdtClLinkEditor::updateUnitConnectionCurrent(int linkRow)
{
  QVariant startUnitId;
  QVariant startConnectionId;
  QVariant endUnitId;
  QVariant endConnectionId;
  QVariant currentLinkId;
  QModelIndex index;
  QSqlQuery query(pvDatabase);
  QSqlError sqlError;
  int row;

  // Get current Link ID
  index = pvLinkModel->index(linkRow, pvLinkModel->fieldIndex("Id_PK"));
  if(index.isValid()){
    currentLinkId = pvLinkModel->data(index);
  }
  ///currentLinkId = pvLinkModel->data(index);
  if(!currentLinkId.isValid()){
    clearWidgetContents(pvStartUnitWidgetMapper);
    clearWidgetContents(pvStartUnitConnectionWidgetMapper);
    clearWidgetContents(pvEndUnitWidgetMapper);
    clearWidgetContents(pvEndUnitConnectionWidgetMapper);
    return;
  }
  // Get start Unit ID
  if(!query.exec("SELECT UnitStart_Id_PK FROM LinkList_view WHERE Link_Id_PK = " + currentLinkId.toString())){
    sqlError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to get UnitStart_Id_PK in table 'LinkList_view'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    clearWidgetContents(pvStartUnitWidgetMapper);
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
    clearWidgetContents(pvStartUnitConnectionWidgetMapper);
    return;
  }
  if(query.next()){
    startConnectionId = query.value(0);
  }
  // Update Start Unit and connections widgets
  row = modelIndexRowForValue(pvStartUnitModel, "Unit_Id_FK", startUnitId);
  if(row < 0){
    clearWidgetContents(pvStartUnitWidgetMapper);
  }else{
    pvStartUnitWidgetMapper->setCurrentIndex(row);
  }
  row = modelIndexRowForValue(pvStartUnitConnectionModel, "Id_PK", startConnectionId);
  if(row < 0){
    clearWidgetContents(pvStartUnitConnectionWidgetMapper);
  }else{
    pvStartUnitConnectionWidgetMapper->setCurrentIndex(row);
  }
  // Get end Unit ID
  if(!query.exec("SELECT UnitEnd_Id_PK FROM LinkList_view WHERE Link_Id_PK = " + currentLinkId.toString())){
    sqlError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to get UnitStart_Id_PK in table 'LinkList_view'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    clearWidgetContents(pvEndUnitWidgetMapper);
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
    clearWidgetContents(pvEndUnitConnectionWidgetMapper);
    return;
  }
  if(query.next()){
    endConnectionId = query.value(0);
  }
  // Update End Unit and connections widgets
  row = modelIndexRowForValue(pvEndUnitModel, "Unit_Id_FK", endUnitId);
  if(row < 0){
    clearWidgetContents(pvEndUnitWidgetMapper);
  }else{
    pvEndUnitWidgetMapper->setCurrentIndex(row);
  }
  row = modelIndexRowForValue(pvEndUnitConnectionModel, "Id_PK", endConnectionId);
  if(row < 0){
    clearWidgetContents(pvEndUnitConnectionWidgetMapper);
  }else{
    pvEndUnitConnectionWidgetMapper->setCurrentIndex(row);
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
  connect(pvLinkWidget, SIGNAL(currentRowChanged(int)), this, SLOT(updateUnitConnectionCurrent(int)));
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
  // Setup Unit widgets mapping
  pvStartUnitWidgetMapper->setModel(pvStartUnitModel);
  pvStartUnitWidgetMapper->addMapping(pvLinkUiWidget->fldStartUnitSchemaPosition, pvStartUnitModel->fieldIndex("SchemaPosition"));
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
  pvStartUnitConnectionWidgetMapper->setModel(pvStartUnitConnectionModel);
  pvStartUnitConnectionWidgetMapper->addMapping(pvLinkUiWidget->fldStartConnectorName, pvStartUnitConnectionModel->fieldIndex("ConnectorName"));
  pvStartUnitConnectionWidgetMapper->addMapping(pvLinkUiWidget->fldStartContactName, pvStartUnitConnectionModel->fieldIndex("ContactName"));

  return true;
}

bool mdtClLinkEditor::setupEndUnitTables()
{
  QSqlError sqlError;

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
  // Setup Unit widgets mapping
  pvEndUnitWidgetMapper->setModel(pvEndUnitModel);
  pvEndUnitWidgetMapper->addMapping(pvLinkUiWidget->fldEndUnitSchemaPosition, pvEndUnitModel->fieldIndex("SchemaPosition"));
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
  pvEndUnitConnectionWidgetMapper->setModel(pvEndUnitConnectionModel);
  pvEndUnitConnectionWidgetMapper->addMapping(pvLinkUiWidget->fldEndConnectorName, pvEndUnitConnectionModel->fieldIndex("ConnectorName"));
  pvEndUnitConnectionWidgetMapper->addMapping(pvLinkUiWidget->fldEndContactName, pvEndUnitConnectionModel->fieldIndex("ContactName"));

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
  pvListWidget->setHeaderData("StartVehicleType", "Start\nVehicle");
  pvListWidget->setHeaderData("StartVehicleSubType", "Start\nVariant");
  pvListWidget->setHeaderData("StartVehicleSerie", "Start\nSerie");
  pvListWidget->setHeaderData("Identification_FK", "Identification");
  pvListWidget->setHeaderData("StartSchemaPosition", "Start\nSch. pos.");
  pvListWidget->setHeaderData("StartCabinet", "Start\nCabinet");
  pvListWidget->setHeaderData("StartCoordinate", "Start\nCoordiante");
  pvListWidget->setHeaderData("StartConnectorName", "Start\nConnector");
  pvListWidget->setHeaderData("StartContactName", "Start\nContact");
  pvListWidget->setHeaderData("EndVehicleType", "End\nVehicle");
  pvListWidget->setHeaderData("EndVehicleSubType", "End\nVariant");
  pvListWidget->setHeaderData("EndVehicleSerie", "End\nSerie");
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

int mdtClLinkEditor::modelIndexRowForValue(QSqlTableModel *model, const QString &fieldName, const QVariant &value)
{
  Q_ASSERT(model != 0);

  int columnIndex;
  int row;
  int rowCount;
  QModelIndex index;

  if(!value.isValid()){
    return -1;
  }
  // Get columnIndex of given fieldName
  columnIndex = model->fieldIndex(fieldName);
  if(columnIndex < 0){
    return -1;
  }
  // Search ...
  rowCount = model->rowCount();
  for(row = 0; row < rowCount; ++row){
    index = model->index(row, columnIndex);
    if(index.isValid()){
      if(model->data(index) == value){
        return row;
      }
    }
  }

  return -1;
}

void mdtClLinkEditor::clearWidgetContents(QDataWidgetMapper *mapper)
{
  Q_ASSERT(mapper != 0);
  Q_ASSERT(mapper->model() != 0);

  int column;
  int columnCount;
  QWidget *w;
  QLineEdit *le;

  columnCount = mapper->model()->columnCount();
  for(column = 0; column < columnCount; ++column){
    w = mapper->mappedWidgetAt(column);
    if(w != 0){
      le = dynamic_cast<QLineEdit*>(w);
      if(le != 0){
        le->clear();
      }
    }
  }
}
