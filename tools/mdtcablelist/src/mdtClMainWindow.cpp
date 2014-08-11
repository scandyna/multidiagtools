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
#include "mdtClMainWindow.h"
#include "mdtSqlWindow.h"
#include "mdtSqlForm.h"
#include "mdtSqlDatabaseManager.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtClVehicleTypeEditor.h"
#include "mdtClConnectorEditor.h"
#include "mdtClUnitEditor.h"
#include "mdtClLinkBeamEditor.h"
#include "mdtClArticleEditor.h"
#include "mdtTtTestConnectionCableEditor.h"
#include "mdtTtTestNodeEditor.h"
#include "mdtTtTestModelEditor.h"
///#include "mdtTtTestModelItemEditor.h"
#include "mdtTtCableChecker.h"
#include "mdtTtCableCheckerWindow.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableSelection.h"
#include "mdtTtBasicTester.h"
#include "mdtTtBasicTesterWindow.h"

#include <boost/concept_check.hpp>

#include <QAction>
#include <QMessageBox>
#include <QApplication>
#include <QFileInfo>
#include <QMutableListIterator>
#include <QCloseEvent>
#include <QTabWidget>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QTableView>
#include <QMapIterator>
#include <QMutableMapIterator>

#include <QDebug>

mdtClMainWindow::mdtClMainWindow()
{
  setupUi(this);
  // Database manager
  pvDatabaseManager = new mdtSqlDatabaseManager(this);
  pvDatabaseManager->setForDialogParentWidget(this);
  /// \todo provisoire
  initWorkDirectory();
  ///openDatabaseSqlite();
  // Basic tester
  ///pvBasicTester = 0;
  pvBasicTesterWindow = 0;
  // Central widget
  pvTabWidget = 0;

  // Editors - Will be setup on first call
  ///pvTestItemEditor = 0;
  ///pvTestItemEditorWindow = 0;
  pvCableChecker = 0;
  pvCableCheckerWindow = 0;

  connectActions();
}

mdtClMainWindow::~mdtClMainWindow()
{
}

void mdtClMainWindow::openDatabase()
{
  openDatabaseSqlite();
  createVehicleTypeActions();
}

void mdtClMainWindow::closeDatabase()
{
  if(!deleteEditors()){
    return;
  }
  deleteTableviews();
  pvDatabaseManager->database().close();
}

void mdtClMainWindow::createNewDatabase()
{
  createDatabaseSqlite();
}

void mdtClMainWindow::importDatabase()
{
  importDatabaseSqlite();
}

void mdtClMainWindow::viewVehicleType()
{
  if(!displayTableView("VehicleType_tbl")){
    createVehicleTypeTableView();
  }
}

void mdtClMainWindow::updateWorkingOnVehicleTypes()
{
  QMapIterator<QAction*, int> it(pvVehicleTypeActions);
  QAction *action;
  mdtClUnitEditor *editor;

  pvWorkingOnVehicleTypeList.clear();
  while(it.hasNext()){
    it.next();
    action = it.key();
    Q_ASSERT(action != 0);
    if(action->isChecked()){
      pvWorkingOnVehicleTypeList.append(it.value());
    }
  }
  qDebug() << "Working on vehicle type: " << pvWorkingOnVehicleTypeList;
  // Update unit editor
  editor = getUnitEditor();
  if(editor == 0){
    return;
  }
  if(!editor->setWorkingOnVehicleTypeIdList(pvWorkingOnVehicleTypeList)){
    displayError(editor->lastError());
  }
}

void mdtClMainWindow::updateVehicleTypeMenu()
{
  removeVehicleTypeActions();
  createVehicleTypeActions();
}

void mdtClMainWindow::editVehicleType()
{
  mdtClVehicleTypeEditor *editor;
  mdtSqlWindow *window;

  // Get or create editor
  editor = getVehicleTypeEditor();
  if(editor == 0){
    return;
  }
  // Get window
  window = getEditorWindow(editor);
  Q_ASSERT(window != 0);
  // Select and show
  Q_ASSERT(editor != 0);
  if(!editor->select()){
    displayError(editor->lastError());
    return;
  }
  window->enableNavigation();
  window->raise();
  window->show();
}

void mdtClMainWindow::viewConnector()
{
  if(!displayTableView("Connector_tbl")){
    createConnectorTableView();
  }
}

void mdtClMainWindow::editConnector()
{
  mdtClConnectorEditor *editor;
  mdtSqlWindow *window;

  // Get or create editor
  editor = getConnectorEditor();
  if(editor == 0){
    return;
  }
  // Get window
  window = getEditorWindow(editor);
  Q_ASSERT(window != 0);
  // Select and show
  Q_ASSERT(editor != 0);
  if(!editor->select()){
    displayError(editor->lastError());
    return;
  }
  window->enableNavigation();
  window->raise();
  window->show();
}

void mdtClMainWindow::viewArticle()
{
  if(!displayTableView("Article_tbl")){
    createArticleTableView();
  }
}

void mdtClMainWindow::editArticle()
{
  mdtClArticleEditor *editor;
  mdtSqlWindow *window;

  // Get or create editor
  editor = getArticleEditor();
  if(editor == 0){
    return;
  }
  // Get window
  window = getEditorWindow(editor);
  Q_ASSERT(window != 0);
  // Select and show
  Q_ASSERT(editor != 0);
  if(!editor->select()){
    displayError(editor->lastError());
    return;
  }
  window->enableNavigation();
  window->raise();
  window->show();
}

void mdtClMainWindow::editSelectedArticle()
{
  mdtSqlTableSelection s;
  mdtClArticleEditor *editor;
  mdtSqlWindow *window;
  mdtSqlTableWidget *view;

  // Get article view
  view = getTableView("Article_tbl");
  if(view == 0){
    return;
  }
  // Get ID of selected article
  s = view->currentSelection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  // Get or create editor
  editor = getArticleEditor();
  if(editor == 0){
    return;
  }
  // Get window
  window = getEditorWindow(editor);
  Q_ASSERT(window != 0);
  // Select and show
  Q_ASSERT(editor != 0);
  if(!editor->select()){
    displayError(editor->lastError());
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  if(!editor->setCurrentRecord("Id_PK", s.data(0, "Id_PK"))){
    displayError(editor->lastError());
  }
  window->enableNavigation();
  window->raise();
  window->show();
}

void mdtClMainWindow::viewUnit()
{
  if(!displayTableView("Unit_view")){
    createUnitTableView();
  }
}

void mdtClMainWindow::editUnit()
{
  mdtClUnitEditor *editor;
  mdtSqlWindow *window;

  // Get or create editor
  editor = getUnitEditor();
  if(editor == 0){
    return;
  }
  // Get window
  window = getEditorWindow(editor);
  Q_ASSERT(window != 0);
  // Select and show
  Q_ASSERT(editor != 0);
  if(!editor->select()){
    displayError(editor->lastError());
    return;
  }
  window->enableNavigation();
  window->raise();
  window->show();
}

void mdtClMainWindow::editSelectedUnit()
{
  mdtSqlTableSelection s;
  mdtClUnitEditor *editor;
  mdtSqlWindow *window;
  mdtSqlTableWidget *view;

  // Get unit view
  view = getTableView("Unit_view");
  if(view == 0){
    return;
  }
  // Get ID of selected unit
  s = view->currentSelection("Unit_Id_PK");
  if(s.isEmpty()){
    return;
  }
  // Get or create editor
  editor = getUnitEditor();
  if(editor == 0){
    return;
  }
  // Get window
  window = getEditorWindow(editor);
  Q_ASSERT(window != 0);
  // Select and show
  Q_ASSERT(editor != 0);
  if(!editor->select()){
    displayError(editor->lastError());
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  if(!editor->setCurrentRecord("Id_PK", s.data(0, "Unit_Id_PK"))){
    displayError(editor->lastError());
  }
  window->enableNavigation();
  window->raise();
  window->show();
}

void mdtClMainWindow::viewLinkList()
{
  if(!displayTableView("LinkList_view")){
    createLinkListTableView();
  }
}

void mdtClMainWindow::viewLinkBeam()
{
  if(!displayTableView("LinkBeam_tbl")){
    createLinkBeamTableView();
  }
}

void mdtClMainWindow::editLinkBeam()
{
  mdtClLinkBeamEditor *editor;
  mdtSqlWindow *window;

  // Get or create editor
  editor = getLinkBeamEditor();
  if(editor == 0){
    return;
  }
  // Get window
  window = getEditorWindow(editor);
  Q_ASSERT(window != 0);
  // Select and show
  Q_ASSERT(editor != 0);
  if(!editor->select()){
    displayError(editor->lastError());
    return;
  }
  window->enableNavigation();
  window->raise();
  window->show();
}

void mdtClMainWindow::viewTestConnectionCable()
{
  if(!displayTableView("TestCable_tbl")){
    createTestConnectionCableTableView();
  }
}

void mdtClMainWindow::editTestConnectionCable()
{
  mdtTtTestConnectionCableEditor *editor;
  mdtSqlWindow *window;

  // Get or create editor
  editor = getTestConnectionCableEditor();
  if(editor == 0){
    return;
  }
  // Get window
  window = getEditorWindow(editor);
  Q_ASSERT(window != 0);
  // Select and show
  Q_ASSERT(editor != 0);
  if(!editor->select()){
    displayError(editor->lastError());
    return;
  }
  window->enableNavigation();
  window->raise();
  window->show();
}

void mdtClMainWindow::editSelectedTestConnectionCable()
{
  mdtSqlTableSelection s;
  mdtTtTestConnectionCableEditor *editor;
  mdtSqlWindow *window;
  mdtSqlTableWidget *view;

  // Get test connection cable view
  view = getTableView("TestCable_tbl");
  if(view == 0){
    return;
  }
  // Get ID of selected test connection cable
  s = view->currentSelection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  // Get or create editor
  editor = getTestConnectionCableEditor();
  if(editor == 0){
    return;
  }
  // Get window
  window = getEditorWindow(editor);
  Q_ASSERT(window != 0);
  // Select and show
  Q_ASSERT(editor != 0);
  if(!editor->select()){
    displayError(editor->lastError());
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  if(!editor->setCurrentRecord("Id_PK", s.data(0, "Id_PK"))){
    displayError(editor->lastError());
  }
  window->enableNavigation();
  window->raise();
  window->show();
}

void mdtClMainWindow::editTestNode()
{
  mdtTtTestNodeEditor *editor;
  mdtSqlWindow *window;

  // Get or create editor
  editor = getTestNodeEditor();
  if(editor == 0){
    return;
  }
  // Get window
  window = getEditorWindow(editor);
  Q_ASSERT(window != 0);
  // Select and show
  Q_ASSERT(editor != 0);
  if(!editor->select()){
    displayError(editor->lastError());
    return;
  }
  window->enableNavigation();
  window->raise();
  window->show();
}

void mdtClMainWindow::editTest()
{
  mdtTtTestModelEditor *editor;
  mdtSqlWindow *window;

  // Get or create editor
  editor = getTestModelEditor();
  if(editor == 0){
    return;
  }
  Q_ASSERT(editor != 0);
  // Get window
  window = getEditorWindow(editor);
  Q_ASSERT(window != 0);
  // Select and show
  if(!editor->select()){
    displayError(editor->lastError());
    return;
  }
  window->enableNavigation();
  window->raise();
  window->show();
}

/**
void mdtClMainWindow::editTestItem()
{
  if(pvTestItemEditor == 0){
    pvTestItemEditor = new mdtTtTestModelItemEditor(0, pvDatabaseManager->database());
    if(!pvTestItemEditor->setupTables()){
      QMessageBox msgBox(this);
      msgBox.setText(tr("Cannot setup test item editor."));
      msgBox.setInformativeText(tr("This can happen if selected database has wrong format (is also not a database made for ")\
                                + qApp->applicationName() + tr(")"));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      delete pvTestItemEditor;
      pvTestItemEditor = 0;
      return;
    }
    pvTestItemEditorWindow = new mdtSqlWindow(this);
    pvTestItemEditorWindow->setSqlForm(pvTestItemEditor);
    pvTestItemEditorWindow->resize(800, 500);
    pvTestItemEditorWindow->enableNavigation();
    pvTestItemEditorWindow->enableEdition();
  }
  Q_ASSERT(pvTestItemEditor != 0);
  Q_ASSERT(pvTestItemEditorWindow != 0);
  pvTestItemEditor->select();
  pvTestItemEditorWindow->show();
}
*/

void mdtClMainWindow::runBasicTester()
{
  if(pvBasicTesterWindow == 0){
    createBasicTester();
  }
  Q_ASSERT(pvBasicTesterWindow != 0);
  pvBasicTesterWindow->raise();
  pvBasicTesterWindow->show();
}

void mdtClMainWindow::runCableChecker()
{
  if(pvCableChecker == 0){
    pvCableChecker = new mdtTtCableChecker(0, pvDatabaseManager->database());
    if(!pvCableChecker->setupTables()){
      QMessageBox msgBox(this);
      msgBox.setText(tr("Cannot setup test item editor."));
      msgBox.setInformativeText(tr("This can happen if selected database has wrong format (is also not a database made for ")\
                                + qApp->applicationName() + tr(")"));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      delete pvCableChecker;
      pvCableChecker = 0;
      return;
    }
    pvCableCheckerWindow = new mdtTtCableCheckerWindow(this);
    pvCableCheckerWindow->setCableChecker(pvCableChecker);
    pvCableCheckerWindow->resize(800, 500);
  }
  Q_ASSERT(pvCableChecker != 0);
  Q_ASSERT(pvCableCheckerWindow != 0);
  pvCableChecker->select();
  pvCableCheckerWindow->show();
}

/**
void mdtClMainWindow::createTestConnectionCable()
{
  mdtTtTestConnectionCableEditor editor(this, pvDatabaseManager->database());

  editor.createCable();
}

void mdtClMainWindow::connectTestCable()
{
  mdtTtTestConnectionCableEditor editor(this, pvDatabaseManager->database());

  editor.connectTestCable();
}

void mdtClMainWindow::disconnectTestCable()
{
  mdtTtTestConnectionCableEditor editor(this, pvDatabaseManager->database());

  editor.disconnectTestCable();
}
*/

void mdtClMainWindow::closeTableView(int index)
{
  QWidget *widget;
  int i;

  widget = pvTabWidget->widget(index);
  pvTabWidget->removeTab(index);
  for(i = 0; i < pvOpenViews.size(); ++i){
    if(pvOpenViews.at(i) == widget){
      pvOpenViews.removeAt(i);
      delete widget;
      widget = 0;
    }
  }
  Q_ASSERT(widget == 0);
}

bool mdtClMainWindow::createVehicleTypeTableView()
{
  mdtSqlTableWidget *tableWidget;

  tableWidget = createTableView("VehicleType_tbl", tr("Vehicle types"));
  if(tableWidget == 0){
    return false;
  }
  // Hide technical fields
  tableWidget->setColumnHidden("Id_PK", true);
  // Rename fields to user friendly ones
  tableWidget->setHeaderData("Type", tr("Vehicle\ntype"));
  tableWidget->setHeaderData("SubType", tr("Vehicle\nsub type"));
  tableWidget->setHeaderData("SeriesNumber", tr("Vehicle\nserie"));
  // Setup sorting
  tableWidget->addColumnToSortOrder("Type", Qt::AscendingOrder);
  tableWidget->addColumnToSortOrder("SubType", Qt::AscendingOrder);
  tableWidget->addColumnToSortOrder("SeriesNumber", Qt::AscendingOrder);
  tableWidget->sort();
  // Resize
  tableWidget->tableView()->resizeColumnsToContents();
  tableWidget->tableView()->resizeRowsToContents();

  return true;
}

bool mdtClMainWindow::createVehicleTypeActions()
{
  QSqlQuery query(pvDatabaseManager->database());
  QString sql;
  QAction *action;
  QString txt;
  int vehicleTypeId;

  // Get vehicle types from DB
  sql = "SELECT Id_PK, Type, SubType, SeriesNumber FROM VehicleType_tbl";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    mdtError e(tr("Unable to get vehicle type list."), mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClMainWindow");
    e.commit();
    displayError(e);
    return false;
  }
  // Create actions
  while(query.next()){
    txt = query.value(1).toString() + " , " + query.value(2).toString() + " , " + query.value(3).toString();
    action = new QAction(txt, this);
    action->setCheckable(true);
    action->setChecked(true);
    Q_ASSERT(!query.value(0).isNull());
    vehicleTypeId = query.value(0).toInt();
    pvVehicleTypeActions.insert(action, vehicleTypeId);
    ///pvWorkingOnVehicleTypeList.append(vehicleTypeId);
  }
  // Add actions to menu
  ///menuVehicle_types->addSeparator();
  QMapIterator<QAction*, int> it(pvVehicleTypeActions);
  while(it.hasNext()){
    it.next();
    Q_ASSERT(it.key() != 0);
    menuVehicle_types->addAction(it.key());
    connect(it.key(), SIGNAL(toggled(bool)), this, SLOT(updateWorkingOnVehicleTypes()));
  }
  updateWorkingOnVehicleTypes();

  return true;
}

void mdtClMainWindow::removeVehicleTypeActions()
{
  QAction *action;
  QMutableMapIterator<QAction*, int> it(pvVehicleTypeActions);
  while(it.hasNext()){
    it.next();
    action = it.key();
    Q_ASSERT(action != 0);
    disconnect(action, SIGNAL(toggled(bool)), this, SLOT(updateWorkingOnVehicleTypes()));
    menuVehicle_types->removeAction(action);
    delete action;
    it.remove();
  }
}

mdtClVehicleTypeEditor* mdtClMainWindow::getVehicleTypeEditor()
{
  mdtClVehicleTypeEditor *editor;

  editor = getEditor<mdtClVehicleTypeEditor>();
  if(editor != 0){
    return editor;
  }else{
    return createVehicleTypeEditor();
  }
}

mdtClVehicleTypeEditor *mdtClMainWindow::createVehicleTypeEditor()
{
  mdtClVehicleTypeEditor *editor;
  mdtSqlWindow *window;

  editor = new mdtClVehicleTypeEditor(0, pvDatabaseManager->database());
  Q_ASSERT(editor->mainSqlWidget() != 0);
  connect(editor->mainSqlWidget(), SIGNAL(stateVisualizingEntered()), this, SLOT(updateVehicleTypeMenu()));
  window = setupEditor(editor);
  if(window == 0){
    return 0;
  }
  window->setWindowTitle(tr("Vehicle type edition"));
  window->resize(800, 600);

  return editor;
}

bool mdtClMainWindow::createConnectorTableView()
{
  mdtSqlTableWidget *tableWidget;

  tableWidget = createTableView("Connector_tbl", tr("Connectors"));
  if(tableWidget == 0){
    return false;
  }
  // Hide technical fields
  tableWidget->setColumnHidden("Id_PK", true);
  // Rename fields to user friendly ones
  tableWidget->setHeaderData("ContactQty", tr("Contact\nqty"));
  tableWidget->setHeaderData("InsertRotation", tr("Insert\nrotation\ncode"));
  tableWidget->setHeaderData("ManufacturerConfigCode", tr("Manufacturer\nconfiguration\ncode"));
  tableWidget->setHeaderData("ManufacturerArticleCode", tr("Manufacturer\narticle\ncode"));
  // Setup sorting
  tableWidget->addColumnToSortOrder("Gender", Qt::AscendingOrder);
  tableWidget->addColumnToSortOrder("ContactQty", Qt::AscendingOrder);
  tableWidget->sort();
  // Resize
  tableWidget->tableView()->resizeColumnsToContents();
  tableWidget->tableView()->resizeRowsToContents();

  return true;
}

mdtClConnectorEditor *mdtClMainWindow::getConnectorEditor()
{
  mdtClConnectorEditor *editor;

  editor = getEditor<mdtClConnectorEditor>();
  if(editor != 0){
    return editor;
  }else{
    return createConnectorEditor();
  }
}

mdtClConnectorEditor *mdtClMainWindow::createConnectorEditor()
{
  mdtClConnectorEditor *editor;
  mdtSqlWindow *window;

  editor = new mdtClConnectorEditor(0, pvDatabaseManager->database());
  window = setupEditor(editor);
  if(window == 0){
    return 0;
  }
  window->setWindowTitle(tr("Connector edition"));
  window->resize(800, 600);

  return editor;
}

bool mdtClMainWindow::createArticleTableView()
{
  mdtSqlTableWidget *tableWidget;

  tableWidget = createTableView("Article_tbl", tr("Articles"));
  if(tableWidget == 0){
    return false;
  }
  // Hide technical fields
  tableWidget->setColumnHidden("Id_PK", true);
  connect(tableWidget->tableView(), SIGNAL(doubleClicked(const QModelIndex&)),this, SLOT(editSelectedArticle()));
  // Rename fields to user friendly ones
  tableWidget->setHeaderData("ArticleCode", tr("Article\ncode"));
  tableWidget->setHeaderData("IdentificationDocument", tr("Identification\ndocument"));
  tableWidget->setHeaderData("ManufacturerCode", tr("Manufacturer\ncode"));
  tableWidget->setHeaderData("ManufacturerIdentificationDocument", tr("Manufacturer\nidentification\ndocument"));
  // Setup sorting
  tableWidget->addColumnToSortOrder("ArticleCode", Qt::AscendingOrder);
  tableWidget->sort();
  // Resize
  tableWidget->tableView()->resizeColumnsToContents();
  tableWidget->tableView()->resizeRowsToContents();

  return true;
}

void mdtClMainWindow::deleteTableviews()
{
  pvTabWidget->clear();
  qDeleteAll(pvOpenViews);
  pvOpenViews.clear();
}

mdtClArticleEditor *mdtClMainWindow::getArticleEditor()
{
  mdtClArticleEditor *editor;

  editor = getEditor<mdtClArticleEditor>();
  if(editor != 0){
    return editor;
  }else{
    return createArticleEditor();
  }
}

mdtClArticleEditor *mdtClMainWindow::createArticleEditor()
{
  mdtClArticleEditor *editor;
  mdtSqlWindow *window;

  editor = new mdtClArticleEditor(0, pvDatabaseManager->database());
  window = setupEditor(editor);
  if(window == 0){
    return 0;
  }
  window->setWindowTitle(tr("Article edition"));
  window->resize(800, 600);

  return editor;
}

bool mdtClMainWindow::createUnitTableView()
{
  mdtSqlTableWidget *tableWidget;

  // Create table view
  tableWidget = createTableView("Unit_view", tr("Units"));
  if(tableWidget == 0){
    return false;
  }
  // Connect the double clicked signalt to current unit edition
  Q_ASSERT(tableWidget->tableView() != 0);
  connect(tableWidget->tableView(), SIGNAL(doubleClicked(const QModelIndex&)),this, SLOT(editSelectedUnit()));
  // Hide technical fields
  tableWidget->setColumnHidden("VehicleType_Id_PK", true);
  tableWidget->setColumnHidden("Unit_Id_PK", true);
  tableWidget->setColumnHidden("Article_Id_PK", true);
  // Rename fields to user friendly ones
  tableWidget->setHeaderData("Type", tr("Vehicle\ntype"));
  tableWidget->setHeaderData("SubType", tr("Vehicle\nsub type"));
  tableWidget->setHeaderData("SeriesNumber", tr("Vehicle\nserie"));
  tableWidget->setHeaderData("SchemaPosition", tr("Schema\nposition"));
  // Setup sorting
  tableWidget->addColumnToSortOrder("Type", Qt::AscendingOrder);
  tableWidget->addColumnToSortOrder("SubType", Qt::AscendingOrder);
  tableWidget->addColumnToSortOrder("SeriesNumber", Qt::AscendingOrder);
  tableWidget->addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  tableWidget->sort();
  // Resize
  tableWidget->tableView()->resizeColumnsToContents();
  tableWidget->tableView()->resizeRowsToContents();

  return true;
}

mdtClUnitEditor *mdtClMainWindow::getUnitEditor()
{
  mdtClUnitEditor *editor;

  editor = getEditor<mdtClUnitEditor>();
  if(editor != 0){
    return editor;
  }else{
    return createUnitEditor();
  }
}

mdtClUnitEditor *mdtClMainWindow::createUnitEditor()
{
  mdtClUnitEditor *editor;
  mdtSqlWindow *window;

  editor = new mdtClUnitEditor(0, pvDatabaseManager->database());
  window = setupEditor(editor);
  if(window == 0){
    return 0;
  }
  window->setWindowTitle(tr("Unit edition"));
  window->resize(800, 600);

  return editor;
}

bool mdtClMainWindow::createLinkListTableView()
{
  mdtSqlTableWidget *tableWidget;

  tableWidget = createTableView("LinkList_view", tr("Link list"));
  if(tableWidget == 0){
    return false;
  }

  return true;
}

bool mdtClMainWindow::createLinkBeamTableView()
{
  mdtSqlTableWidget *tableWidget;

  tableWidget = createTableView("LinkBeam_tbl", tr("Link beams"));
  if(tableWidget == 0){
    return false;
  }
  tableWidget->addColumnToSortOrder("Identification", Qt::AscendingOrder);
  tableWidget->sort();
  tableWidget->resizeViewToContents();

  return true;
}

mdtClLinkBeamEditor *mdtClMainWindow::getLinkBeamEditor()
{
  mdtClLinkBeamEditor *editor;

  editor = getEditor<mdtClLinkBeamEditor>();
  if(editor != 0){
    return editor;
  }else{
    return createLinkBeamEditor();
  }
}

mdtClLinkBeamEditor *mdtClMainWindow::createLinkBeamEditor()
{
  mdtClLinkBeamEditor *editor;
  mdtSqlWindow *window;

  editor = new mdtClLinkBeamEditor(0, pvDatabaseManager->database());
  window = setupEditor(editor);
  if(window == 0){
    return 0;
  }
  window->setWindowTitle(tr("Link beam edition"));
  window->resize(800, 600);

  return editor;
}

mdtTtTestNodeEditor *mdtClMainWindow::getTestNodeEditor()
{
  mdtTtTestNodeEditor *editor;

  editor = getEditor<mdtTtTestNodeEditor>();
  if(editor != 0){
    return editor;
  }else{
    return createTestNodeEditor();
  }
}

mdtTtTestNodeEditor *mdtClMainWindow::createTestNodeEditor()
{
  mdtTtTestNodeEditor *editor;
  mdtSqlWindow *window;

  editor = new mdtTtTestNodeEditor(0, pvDatabaseManager->database());
  window = setupEditor(editor);
  if(window == 0){
    return 0;
  }
  window->setWindowTitle(tr("Test node edition"));
  window->resize(800, 600);

  return editor;
}

bool mdtClMainWindow::createTestConnectionCableTableView()
{
  mdtSqlTableWidget *tableWidget;

  tableWidget = createTableView("TestCable_tbl", tr("Test cables"));
  if(tableWidget == 0){
    return false;
  }
  connect(tableWidget->tableView(), SIGNAL(doubleClicked(const QModelIndex&)),this, SLOT(editSelectedTestConnectionCable()));
  tableWidget->setColumnHidden("Id_PK", true);
  tableWidget->setHeaderData("DescriptionEN", tr("Description"));
  tableWidget->setHeaderData("OffsetResetDate", tr("Last offset reset"));
  tableWidget->addColumnToSortOrder("Identification", Qt::AscendingOrder);
  tableWidget->sort();
  tableWidget->resizeViewToContents();

  return true;
}

mdtTtTestConnectionCableEditor *mdtClMainWindow::getTestConnectionCableEditor()
{
  mdtTtTestConnectionCableEditor *editor;

  editor = getEditor<mdtTtTestConnectionCableEditor>();
  if(editor != 0){
    return editor;
  }else{
    return createTestConnectionCableEditor();
  }
}

mdtTtTestConnectionCableEditor *mdtClMainWindow::createTestConnectionCableEditor()
{
  mdtTtTestConnectionCableEditor *editor;
  mdtSqlWindow *window;

  editor = new mdtTtTestConnectionCableEditor(0, pvDatabaseManager->database());
  window = setupEditor(editor);
  if(window == 0){
    return 0;
  }
  window->setWindowTitle(tr("Test cable edition"));
  window->resize(800, 600);

  return editor;
}

mdtTtTestModelEditor *mdtClMainWindow::getTestModelEditor()
{
  mdtTtTestModelEditor *editor;

  editor = getEditor<mdtTtTestModelEditor>();
  if(editor != 0){
    return editor;
  }else{
    return createTestModelEditor();
  }
}

mdtTtTestModelEditor *mdtClMainWindow::createTestModelEditor()
{
  mdtTtTestModelEditor *editor;
  mdtSqlWindow *window;

  editor = new mdtTtTestModelEditor(0, pvDatabaseManager->database());
  window = setupEditor(editor);
  if(window == 0){
    return 0;
  }
  window->setWindowTitle(tr("Test edition"));
  window->resize(800, 600);

  return editor;
}

void mdtClMainWindow::createBasicTester()
{
  ///Q_ASSERT(pvBasicTester == 0);
  Q_ASSERT(pvBasicTesterWindow == 0);

  ///pvBasicTester = new mdtTtBasicTester(pvDatabaseManager->database());
  pvBasicTesterWindow = new mdtTtBasicTesterWindow(pvDatabaseManager->database(), this);
  if(!pvBasicTesterWindow->init()){
    displayError(pvBasicTesterWindow->lastError());
  }
  /**
  if(!pvBasicTester->setup()){
    displayError(pvBasicTester->lastError());
  }
  pvBasicTesterWindow->setTesterWidget(pvBasicTester);
  */
}

mdtSqlTableWidget *mdtClMainWindow::createTableView(const QString & tableName, const QString & userFriendlyTableName)
{
  mdtSqlTableWidget *tableWidget;
  QSqlTableModel *model;
  QString uftn;

  // Check that we have currently a database open
  if(!pvDatabaseManager->database().isOpen()){
    displayWarning(tr("No database is open."), tr("Please open a database and try again."));
    return 0;
  }
  // Set user friendly table name
  if(userFriendlyTableName.isEmpty()){
    uftn = tableName;
  }else{
    uftn = userFriendlyTableName;
  }
  // Setup model
  model = new QSqlTableModel(this, pvDatabaseManager->database());
  model->setTable(tableName);
  if(!model->select()){
    QSqlError sqlError = model->lastError();
    mdtError e(tr("Unable to select data in table '") + tableName + tr("'"), mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClMainWindow");
    e.commit();
    displayError(e);
    delete model;
    return 0;
  }
  // Setup widget
  tableWidget = new mdtSqlTableWidget;
  tableWidget->setModel(model);
  tableWidget->setUserFriendlyTableName(uftn);
  // Setup tab widget (if needed)
  if(pvTabWidget == 0){
    pvTabWidget = new QTabWidget;
    pvTabWidget->setTabsClosable(true);
    setCentralWidget(pvTabWidget);  /// \todo Check if current central widget deleted ..
    connect(pvTabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTableView(int)));
  }
  // Add view
  pvTabWidget->addTab(tableWidget, uftn);
  pvOpenViews.append(tableWidget);
  pvTabWidget->setCurrentWidget(tableWidget);

  return tableWidget;
}

mdtSqlTableWidget *mdtClMainWindow::getTableView(const QString & tableName)
{
  mdtSqlTableWidget *tableWidget;
  QSqlTableModel *model;
  int i;

  // Search requested table widget
  for(i = 0; i < pvOpenViews.size(); ++i){
    tableWidget = pvOpenViews.at(i);
    Q_ASSERT(tableWidget != 0);
    model = tableWidget->model();
    Q_ASSERT(model != 0);
    if(model->tableName() == tableName){
      return tableWidget;
    }
  }

  return 0;
}

bool mdtClMainWindow::displayTableView(const QString& tableName)
{
  mdtSqlTableWidget *tableWidget;
  QSqlTableModel *model;
  ///int i;

  // Search requested table widget
  tableWidget = getTableView(tableName);
  if(tableWidget == 0){
    return false;
  }
  // We have the requested widget, refresh data
  Q_ASSERT(tableWidget != 0);
  model = tableWidget->model();
  Q_ASSERT(model != 0);
  if(!model->select()){
    QSqlError sqlError = model->lastError();
    mdtError e(tr("Unable to select data in table '") + tableName + tr("'"), mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClMainWindow");
    e.commit();
    return false;
  }
  // Show the widget - If a view exists in pvOpenViews, tab widget was allready created
  Q_ASSERT(pvTabWidget != 0);
  pvTabWidget->setCurrentWidget(tableWidget);

  return true;
}

mdtSqlWindow *mdtClMainWindow::setupEditor(mdtSqlForm *editor)
{
  Q_ASSERT(editor != 0);

  mdtSqlWindow *window;

  // Setup editor
  if(!editor->setupTables()){
    displayError(editor->lastError());
    delete editor;
    return 0;
  }
  // Setup window
  window = new mdtSqlWindow(this);
  window->setSqlForm(editor);
  window->enableEdition();
  pvOpenEditors.append(window);

  return window;
}

template <class T> T *mdtClMainWindow::getEditor()
{
  int i;
  mdtSqlForm *form;
  T *editor;

  for(i = 0; i < pvOpenEditors.size(); ++i){
    form = pvOpenEditors.at(i)->sqlForm();
    if(form != 0){
      editor = dynamic_cast<T*>(form);
      if(editor != 0){
        return editor;
      }
    }
  }

  return 0;
}

mdtSqlWindow *mdtClMainWindow::getEditorWindow(mdtSqlForm *editor)
{
  int i;

  for(i = 0; i < pvOpenEditors.size(); ++i){
    if(pvOpenEditors.at(i)->sqlForm() == editor){
      return pvOpenEditors.at(i);
    }
  }

  return 0;
}

bool mdtClMainWindow::deleteEditors()
{
  QMutableListIterator<mdtSqlWindow*> it(pvOpenEditors);
  mdtSqlWindow *window;
  mdtSqlForm *form;

  while(it.hasNext()){
    window = it.next();
    Q_ASSERT(window != 0);
    form = window->sqlForm();
    if(form != 0){
      if(form->allDataAreSaved()){
        it.remove();
        delete window;
      }else{
        break;
      }
    }
  }
  if(pvBasicTesterWindow != 0){
    /// \todo Check that all data are saved
    delete pvBasicTesterWindow;
    pvBasicTesterWindow = 0;
  }

  return pvOpenEditors.isEmpty();
}

void mdtClMainWindow::closeEvent(QCloseEvent* event)
{
  Q_ASSERT(event != 0);

  if(!deleteEditors()){
    event->ignore();
  }else{
    event->accept();
  }
}

void mdtClMainWindow::connectActions()
{
  // Open database
  connect(actOpenDatabase, SIGNAL(triggered()), this, SLOT(openDatabase()));
  // Close database
  connect(actCloseDatabase, SIGNAL(triggered()), this, SLOT(closeDatabase()));
  // Create new database
  connect(actCreateNewDatabase, SIGNAL(triggered()), this, SLOT(createNewDatabase()));
  // Import database
  connect(actImportDatabase, SIGNAL(triggered()), this, SLOT(importDatabase()));

  // Vehicle type edition
  connect(actViewVehicleType, SIGNAL(triggered()), this, SLOT(viewVehicleType()));
  connect(actEditVehicleType, SIGNAL(triggered()), this, SLOT(editVehicleType()));
  ///connect(pbEditVehicleType, SIGNAL(clicked()), this, SLOT(editVehicleType()));
  // Connector
  connect(actViewConnector, SIGNAL(triggered()), this, SLOT(viewConnector()));
  connect(actEditConnector, SIGNAL(triggered()), this, SLOT(editConnector()));
  // Article edition
  connect(actViewArticle, SIGNAL(triggered()), this, SLOT(viewArticle()));
  connect(actEditArticle, SIGNAL(triggered()), this, SLOT(editArticle()));
  connect(actEditSelectedArticle, SIGNAL(triggered()), this, SLOT(editSelectedArticle()));
  ///connect(pbEditArticle, SIGNAL(clicked()), this, SLOT(editArticle()));
  // Unit edition
  connect(actViewUnit, SIGNAL(triggered()), this, SLOT(viewUnit()));
  connect(actEditUnit, SIGNAL(triggered()), this, SLOT(editUnit()));
  connect(actEditSelectedUnit, SIGNAL(triggered()), this, SLOT(editSelectedUnit()));
  ///connect(pbEditUnit, SIGNAL(clicked()), this, SLOT(editUnit()));
  // Link list
  connect(actViewLinkList, SIGNAL(triggered()), this, SLOT(viewLinkList()));
  // Link beam
  connect(actViewLinkBeam, SIGNAL(triggered()), this, SLOT(viewLinkBeam()));
  connect(actEditLinkBeam, SIGNAL(triggered()), this, SLOT(editLinkBeam()));

  // Test connection cable
  connect(actViewTestConnectionCable, SIGNAL(triggered()), this, SLOT(viewTestConnectionCable()));
  connect(actEditTestConnectionCable, SIGNAL(triggered()), this, SLOT(editTestConnectionCable()));
  /**
  connect(actCreateTestConnectionCable, SIGNAL(triggered()), this, SLOT(createTestConnectionCable()));
  connect(actConnectTestCable, SIGNAL(triggered()), this, SLOT(connectTestCable()));
  connect(actDisconnectTestCable, SIGNAL(triggered()), this, SLOT(disconnectTestCable()));
  */
  // Test node edition
  connect(actEditTestNode, SIGNAL(triggered()), this, SLOT(editTestNode()));
  // Test edition
  connect(actEditTest, SIGNAL(triggered()), this, SLOT(editTest()));
  // Test item edition
  ///connect(actEditTestItem, SIGNAL(triggered()), this, SLOT(editTestItem()));
  // Basic tester
  connect(actBasicTester, SIGNAL(triggered()), this, SLOT(runBasicTester()));
  // Cable checker
  connect(actCableChecker, SIGNAL(triggered()), this, SLOT(runCableChecker()));

}

bool mdtClMainWindow::initWorkDirectory()
{
  pvWorkDirectory = QDir::home();
  if(!pvWorkDirectory.cd("mdtcablelist")){
    if(!pvWorkDirectory.mkdir("mdtcablelist")){
      QMessageBox msgBox(this);
      mdtError e(tr("Cannot create 'mdtcablelist' directory."), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtClMainWindow");
      e.commit();
      msgBox.setText(e.text());
      /// \todo Let the user a chance to choose another place to create this directory !
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      return false;
    }
    if(!pvWorkDirectory.cd("mdtcablelist")){
      ///QMessageBox msgBox(this);
      mdtError e(tr("Cannot create 'mdtcablelist' directory."), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtClMainWindow");
      e.commit();
      displayWarning(e.text());
      /**
      msgBox.setText(e.text());
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      */
      return false;
    }
  }

  return true;
}

bool mdtClMainWindow::openDatabaseSqlite()
{
  QFileInfo fileInfo;

  // Check that no database is allready open
  if(pvDatabaseManager->database().isOpen()){
    displayWarning(tr("A database is allready open."), tr("Close current database and try again."));
    return false;
  }
  // Let the user choose a file
  fileInfo = pvDatabaseManager->chooseDatabaseSqlite(pvWorkDirectory);
  if(fileInfo.fileName().isEmpty()){
    return false;
  }
  /// \todo Mange, let user choose, etc...
  ///QFileInfo fileInfo(pvWorkDirectory, "test01.sqlite");
  // Open database
  if(!pvDatabaseManager->openDatabaseSqlite(fileInfo)){
    displayWarning(pvDatabaseManager->lastError().text());
    /// \todo Dangerous !!
    /**
    mdtTtDatabaseSchema dbSchema(pvDatabaseManager);
    if(!dbSchema.createSchemaSqlite(fileInfo)){
      QMessageBox msgBox(this);
      mdtError e(tr("Cannot create database schema."), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtClMainWindow");
      e.commit();
      msgBox.setText(e.text());
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      return false;
    }
    */
    return false;
  }
  // Check that we have open a cable list schema
  mdtTtDatabaseSchema dbSchema(pvDatabaseManager);
  if(!dbSchema.checkSchema()){
    displayWarning(tr("Choosen file does not contain a valid cable list database."));
    closeDatabase();
    return false;
  }

  return true;
}

bool mdtClMainWindow::createDatabaseSqlite()
{
  // Check that no database is allready open
  if(pvDatabaseManager->database().isOpen()){
    displayWarning(tr("A database is allready open."), tr("Close current database and try again."));
    return false;
  }
  // Create database
  mdtTtDatabaseSchema dbSchema(pvDatabaseManager);
  if(!dbSchema.createSchemaSqlite(pvWorkDirectory)){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Database creation failed.") + "                ");
    msgBox.setDetailedText(dbSchema.lastError().text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return false;
  }

  return true;
}

bool mdtClMainWindow::importDatabaseSqlite()
{
  // Check that we have currently a database open
  if(!pvDatabaseManager->database().isOpen()){
    displayWarning(tr("Cannot import a database."), tr("Please open a database and try again."));
    return false;
  }
  /// \todo Save current edition !
  /// \todo Create a backup from current database !
  // Let the user a chance to cancel import
  QMessageBox msgBox(this);
  msgBox.setText(tr("You are about to import a database into current database."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  if(msgBox.exec() != QMessageBox::Yes){
    return false;
  }
  // Import ...
  mdtTtDatabaseSchema dbSchema(pvDatabaseManager);

  return dbSchema.importDatabase(pvWorkDirectory);
}

void mdtClMainWindow::displayWarning(const QString & text , const QString & informativeText)
{
  QMessageBox msgBox(this);
  msgBox.setText(text);
  msgBox.setInformativeText(informativeText);
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.exec();
}

void mdtClMainWindow::displayError(const mdtError & error)
{
  QMessageBox msgBox;

  msgBox.setText(error.text());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
