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
#include "mdtClMainWindow.h"
#include "mdtSqlWindow.h"
#include "mdtSqlForm.h"

#include "mdtSqlDatabaseManager.h"

#include "mdtSqlDatabaseDialogSqlite.h"
#include "mdtSqlDatabaseSchemaDialog.h"
#include "mdt/sql/copier/DatabaseCopyDialog.h"
#include "mdtClLinkVersion.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtClConnectorEditor.h"
#include "mdtClWireEditor.h"
#include "mdtClLinkBeamEditor.h"
#include "mdtClArticleEditor.h"
///#include "mdtTtTestCableEditor.h"
#include "mdtTtLogicalTestCableEditor.h"
#include "mdtTtTestModelEditor.h"
///#include "mdtTtTestModelItemEditor.h"
#include "mdtTtCableChecker.h"
#include "mdtTtCableCheckerWindow.h"
#include "mdtSqlTableWidget.h"
///#include "mdtSqlFormWidget.h"
#include "mdtSqlTableSelection.h"
#include "mdtTtBasicTester.h"
#include "mdtTtBasicTesterWindow.h"
#include "mdtTtBasicTestNodeCalibrationWindow.h"
#include "mdt/sql/error/Error.h"
#include "mdtErrorDialog.h"
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
  
  pvW750CalibrationWindow = 0;

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
  mdtClApplicationWidgets::clear();
  mdtTtApplicationWidgets::clear();
}

void mdtClMainWindow::openDatabase()
{
  mdtSqlDatabaseDialogSqlite dialog(this);

  // Check that no database is allready open
  if(pvDatabase.isOpen()){
    displayWarning(tr("A database is allready open."), tr("Close current database and try again."));
    return;
  }
  // Init database
  if(!initDatabase()){
    return;
  }
  // Setup and show dialog
  dialog.updateConnectionsList();
  dialog.setDatabase(pvDatabase);
  dialog.setDefaultDirectory(pvWorkDirectory.absolutePath());
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(pvDatabase.isOpen());
  // Check that we have open a cable list schema
  mdtTtDatabaseSchema dbSchema;
  if(!dbSchema.checkSchema(pvDatabase)){
    displayWarning(tr("Choosen file does not contain a valid cable list database."));
    closeDatabase();
    return;
  }
  // Set also open database to Widgets containers
  mdtClApplicationWidgets::setDatabase(pvDatabase);
  mdtTtApplicationWidgets::setDatabase(pvDatabase);
  // Set last available link version to current one
  mdtClLinkVersion lv(pvDatabase);
  lv.setLastVersionAsCurrentVersion();
}

void mdtClMainWindow::closeDatabase()
{
  if(pvDatabase.isOpen()){
    if(!deleteEditors()){
      return;
    }
    mdtClApplicationWidgets::clear();
    mdtTtApplicationWidgets::clear();
    deleteTableviews();
    pvDatabase.close();
  }
  if(pvDatabase.isValid()){
    QSqlDatabase::removeDatabase(pvDatabase.connectionName());
  }
  ///pvDatabaseManager->database().close();
}

void mdtClMainWindow::createNewDatabase()
{
  mdtSqlDatabaseSchemaDialog dialog(this);
  mdtTtDatabaseSchema dbSchema;

  // Check that no database is allready open
  if(pvDatabase.isOpen()){
    displayWarning(tr("A database is allready open."), tr("Close current database and try again."));
    return;
  }
  // Init database
  if(!initDatabase()){
    return;
  }
  // Setup and show dialog
  dbSchema.buildSchema();
  dialog.setSchema(dbSchema.databaseSchema());
  dialog.setDatabase(pvDatabase);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
}

// bool mdtClMainWindow::importDatabaseSqlite()
// {
//   // Check that we have currently a database open
//   if(!pvDatabaseManager->database().isOpen()){
//     displayWarning(tr("Cannot import a database."), tr("Please open a database and try again."));
//     return false;
//   }
//   /// \todo Save current edition !
//   /// \todo Create a backup from current database !
//   // Let the user a chance to cancel import
//   QMessageBox msgBox(this);
//   msgBox.setText(tr("You are about to import a database into current database."));
//   msgBox.setInformativeText(tr("Do you want to continue ?"));
//   msgBox.setIcon(QMessageBox::Warning);
//   msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
//   if(msgBox.exec() != QMessageBox::Yes){
//     return false;
//   }
//   // Import ...
//   mdtTtDatabaseSchema dbSchema(pvDatabaseManager);
// 
//   return dbSchema.importDatabase(pvWorkDirectory);
// }

void mdtClMainWindow::importDatabase()
{
  mdt::sql::copier::DatabaseCopyDialog dialog(this);

  // Check that we have currently a database open
  if(!pvDatabase.isOpen()){
    displayWarning(tr("Cannot import a database."), tr("Please open a database and try again."));
    return;
  }
//   // Init database
//   if(!initDatabase()){
//     return;
//   }
  // Setup and show copy dialog
  dialog.initSourceDatabase(mdtSqlDriverType::SQLite);
  dialog.setDestinationDatabase(pvDatabase);
  dialog.setDestinationDatabaseSelectable(false);
  dialog.exec();
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
  ///mdtClUnitEditor *editor;

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
  /**
  editor = getUnitEditor();
  if(editor == 0){
    return;
  }
  */
  /**
  if(!editor->setWorkingOnVehicleTypeIdList(pvWorkingOnVehicleTypeList)){
    displayError(editor->lastError());
  }
  */
}

void mdtClMainWindow::updateVehicleTypeMenu()
{
  removeVehicleTypeActions();
  createVehicleTypeActions();
}

void mdtClMainWindow::viewConnector()
{
  if(!displayTableView("Connector_tbl")){
    createConnectorTableView();
  }
}

void mdtClMainWindow::editSelectedConnector()
{
  mdtSqlTableSelection s;
  mdtClConnectorPkData pk;
  mdtSqlTableWidget *view;

  // Get connector view
  view = getTableView("Connector_tbl");
  if(view == 0){
    return;
  }
  // Get ID of selected connector
  s = view->currentSelection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  // Setup PK and call editor
  pk.id = s.data(0, "Id_PK");
  mdtClApplicationWidgets::editConnector(pk);
}

void mdtClMainWindow::viewArticle()
{
  if(!displayTableView("Article_tbl")){
    createArticleTableView();
  }
}

void mdtClMainWindow::editSelectedArticle()
{
  mdtSqlTableSelection s;
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
  // Edit article
  mdtClApplicationWidgets::editArticle(s.data(0, "Id_PK"));
}

void mdtClMainWindow::viewUnit()
{
  if(!displayTableView("Unit_view")){
    createUnitTableView();
  }
}

void mdtClMainWindow::editSelectedUnit()
{
   mdtSqlTableSelection s;
   mdtSqlTableWidget *view;

   // Get unit view
   view = getTableView("Unit_view");
   if(view == nullptr){
     return;
   }
  // Get ID of selected unit
  s = view->currentSelection("Unit_Id_PK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  mdtClApplicationWidgets::editUnit(s.data(0, "Unit_Id_PK"));
}

void mdtClMainWindow::viewWire()
{
  if(!displayTableView("Wire_tbl")){
    createWireTableView();
  }
}

void mdtClMainWindow::editWire()
{
  mdtClWireEditor *editor;
  mdtSqlWindow *window;

  // Get or create editor
  editor = getWirEditor();
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

// void mdtClMainWindow::editTestConnectionCable()
// {
//   mdtTtTestCableEditor *editor;
//   mdtSqlWindow *window;
// 
//   // Get or create editor
//   editor = getTestCableEditor();
//   if(editor == 0){
//     return;
//   }
//   // Get window
//   window = getEditorWindow(editor);
//   Q_ASSERT(window != 0);
//   // Select and show
//   Q_ASSERT(editor != 0);
//   if(!editor->select()){
//     displayError(editor->lastError());
//     return;
//   }
//   window->enableNavigation();
//   window->raise();
//   window->show();
// }

void mdtClMainWindow::editSelectedTestConnectionCable()
{
  mdtSqlTableSelection s;
  ///mdtTtTestCableEditor *editor;
  ///mdtSqlWindow *window;
  mdtSqlTableWidget *view;

  // Get test connection cable view
  view = getTableView("TestCable_tbl");
  if(view == 0){
    return;
  }
  // Get ID of selected test connection cable
  s = view->currentSelection("Unit_Id_FK_PK");
  if(s.isEmpty()){
    return;
  }
  // Get or create editor
//   editor = getTestCableEditor();
//   if(editor == 0){
//     return;
//   }
  // Get window
//   window = getEditorWindow(editor);
//   Q_ASSERT(window != 0);
  // Select and show
//   Q_ASSERT(editor != 0);
//   if(!editor->select()){
//     displayError(editor->lastError());
//     return;
//   }
  Q_ASSERT(s.rowCount() == 1);
//   mdtTtApplicationWidgets::editTestCable(s.data(0, "Unit_Id_FK_PK"));
  /// \todo Corriger !! + filtre ??
//   if(!editor->setCurrentRow("Id_PK", s.data(0, "Unit_Id_FK_PK"))){
//     displayError(editor->lastError());
//   }
//   window->enableNavigation();
//   window->raise();
//   window->show();
}

void mdtClMainWindow::calibrateW750TestNode()
{
  if(pvW750CalibrationWindow == 0){
    pvW750CalibrationWindow = new mdtTtBasicTestNodeCalibrationWindow(pvDatabaseManager->database(), this);
    if(!pvW750CalibrationWindow->init()){
      delete pvW750CalibrationWindow;
      pvW750CalibrationWindow = 0;
      return;
    }
  }
  Q_ASSERT(pvW750CalibrationWindow != 0);
  pvW750CalibrationWindow->raise();
  pvW750CalibrationWindow->show();
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
  mdtTtLogicalTestCableEditor editor(this, pvDatabaseManager->database());

  editor.createCable();
}

void mdtClMainWindow::connectTestCable()
{
  mdtTtLogicalTestCableEditor editor(this, pvDatabaseManager->database());

  editor.connectTestCable();
}

void mdtClMainWindow::disconnectTestCable()
{
  mdtTtLogicalTestCableEditor editor(this, pvDatabaseManager->database());

  editor.disconnectTestCable();
}
*/

void mdtClMainWindow::closeTableView(int index)
{
  Q_ASSERT(pvTabWidget != nullptr);

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
    auto error = mdtErrorNewQ(tr("Unable to get vehicle type list."), mdtError::Error, this);
    error.stackError(ErrorFromQSqlQuery(query));
    error.commit();
    displayError(error);
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

bool mdtClMainWindow::createConnectorTableView()
{
  mdtSqlTableWidget *tableWidget;

  tableWidget = createTableView("Connector_tbl", tr("Connectors"));
  if(tableWidget == 0){
    return false;
  }
  connect(tableWidget->tableView(), SIGNAL(doubleClicked(const QModelIndex&)),this, SLOT(editSelectedConnector()));
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
  Q_ASSERT(pvTabWidget != nullptr);
  pvTabWidget->clear();
  qDeleteAll(pvOpenViews);
  pvOpenViews.clear();
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
  tableWidget->resizeViewToContents();

  return true;
}

mdtClWireEditor* mdtClMainWindow::getWirEditor()
{
  mdtClWireEditor *editor;

  editor = getEditor<mdtClWireEditor>();
  if(editor != 0){
    return editor;
  }else{
    return createWireEditor();
  }
}

mdtClWireEditor* mdtClMainWindow::createWireEditor()
{
  mdtClWireEditor *editor;
  mdtSqlWindow *window;

  editor = new mdtClWireEditor(0, pvDatabaseManager->database());
  window = setupEditor(editor);
  if(window == 0){
    return 0;
  }
  window->setWindowTitle(tr("Wire edition"));
  window->resize(800, 600);

  return editor;
}

bool mdtClMainWindow::createWireTableView()
{
  mdtSqlTableWidget *tableWidget;

  tableWidget = createTableView("Wire_tbl", tr("Wires"));
  if(tableWidget == 0){
    return false;
  }
  // Resize
  tableWidget->resizeViewToContents();

  return true;
}

bool mdtClMainWindow::createLinkListTableView()
{
  mdtSqlTableWidget *tableWidget;

  tableWidget = createTableView("LinkList_view", tr("Link list"));
  if(tableWidget == 0){
    return false;
  }
  // Hide technical fields
  tableWidget->setColumnHidden("UnitConnectionStart_Id_FK", true);
  tableWidget->setColumnHidden("UnitConnectionEnd_Id_FK", true);
  tableWidget->setColumnHidden("LinkBeam_Id_FK", true);
  tableWidget->setColumnHidden("StartVehicleType_Id_PK", true);
  tableWidget->setColumnHidden("SinceVersion", true);
  tableWidget->setColumnHidden("Modification", true);
  tableWidget->setColumnHidden("LinkDirection_Code_FK", true);
  tableWidget->setColumnHidden("LinkType_Code_FK", true);
  tableWidget->setColumnHidden("StartUnit_Id_PK", true);
  tableWidget->setColumnHidden("UnitConnectorStart_Id_FK", true);
  tableWidget->setColumnHidden("StartConnectionType_Code_FK", true);
  tableWidget->setColumnHidden("EndVehicleType_Id_PK", true);
  tableWidget->setColumnHidden("EndUnit_Id_PK", true);
  tableWidget->setColumnHidden("UnitConnectorEnd_Id_FK", true);
  // Rename fields to user friendly ones
  
  // Setup sorting
  tableWidget->addColumnToSortOrder("StartVehicleType", Qt::AscendingOrder);
  tableWidget->addColumnToSortOrder("StartVehicleSubType", Qt::AscendingOrder);
  tableWidget->addColumnToSortOrder("StartVehicleSeriesNumber", Qt::AscendingOrder);
  tableWidget->addColumnToSortOrder("StartSchemaPosition", Qt::AscendingOrder);
  tableWidget->sort();
  // Resize
  tableWidget->resizeViewToContents();

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

// mdtTtTestCableEditor *mdtClMainWindow::getTestCableEditor()
// {
//   mdtTtTestCableEditor *editor;
// 
//   editor = getEditor<mdtTtTestCableEditor>();
//   if(editor != 0){
//     return editor;
//   }else{
//     return createTestCableEditor();
//   }
// }
// 
// mdtTtTestCableEditor *mdtClMainWindow::createTestCableEditor()
// {
//   mdtTtTestCableEditor *editor;
//   mdtSqlWindow *window;
// 
//   editor = new mdtTtTestCableEditor(0, pvDatabaseManager->database());
//   window = setupEditor(editor);
//   if(window == 0){
//     return 0;
//   }
//   window->setWindowTitle(tr("Test cable edition"));
//   window->resize(800, 600);
// 
//   return editor;
// }

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

  // Check that we have currently a database open
  if(!pvDatabase.isOpen()){
    displayWarning(tr("No database is open."), tr("Please open a database and try again."));
    return 0;
  }
  // Setup widget
  tableWidget = new mdtSqlTableWidget;
  tableWidget->setTableName(tableName, pvDatabase, userFriendlyTableName);
  // Setup tab widget (if needed)
  if(pvTabWidget == 0){
    pvTabWidget = new QTabWidget;
    pvTabWidget->setTabsClosable(true);
    setCentralWidget(pvTabWidget);  /// \todo Check if current central widget deleted ..
    connect(pvTabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTableView(int)));
  }
  // Start table widget
  tableWidget->start();
  if(!tableWidget->select()){
    displayError(tableWidget->lastError());
    delete tableWidget;
    return 0;
  }
  // Add view
  pvTabWidget->addTab(tableWidget, tableWidget->userFriendlyTableName());
  pvOpenViews.append(tableWidget);
  pvTabWidget->setCurrentWidget(tableWidget);

  return tableWidget;
}

mdtSqlTableWidget *mdtClMainWindow::getTableView(const QString & tableName)
{
  mdtSqlTableWidget *tableWidget;
  int i;

  // Search requested table widget
  for(i = 0; i < pvOpenViews.size(); ++i){
    tableWidget = pvOpenViews.at(i);
    Q_ASSERT(tableWidget != 0);
    if(tableWidget->tableName() == tableName){
      return tableWidget;
    }
  }

  return 0;
}

bool mdtClMainWindow::displayTableView(const QString& tableName)
{
  mdtSqlTableWidget *tableWidget;

  // Search requested table widget
  tableWidget = getTableView(tableName);
  if(tableWidget == 0){
    return false;
  }
  // We have the requested widget, refresh data
  Q_ASSERT(tableWidget != 0);
  if(!tableWidget->select()){
    displayError(tableWidget->lastError());
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

  if(!mdtClApplicationWidgets::closeOpenWidgets()){
    event->ignore();
    return;
  }
  if(!mdtTtApplicationWidgets::closeOpenWidgets()){
    event->ignore();
    return;
  }
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
  connect(actEditVehicleType, &QAction::triggered, mdtClApplicationWidgets::instance(), &mdtClApplicationWidgets::editVehicleTypes);
  // Connector
  connect(actViewConnector, SIGNAL(triggered()), this, SLOT(viewConnector()));
  connect(actEditConnector, &QAction::triggered, mdtClApplicationWidgets::instance(), &mdtClApplicationWidgets::editConnectors);
  connect(actEditSelectedConnector, &QAction::triggered, this, &mdtClMainWindow::editSelectedConnector);
  // Article edition
  connect(actViewArticle, SIGNAL(triggered()), this, SLOT(viewArticle()));
  connect(actEditArticle, &QAction::triggered, mdtClApplicationWidgets::instance(), &mdtClApplicationWidgets::editArticles);
  connect(actEditSelectedArticle, &QAction::triggered, this, &mdtClMainWindow::editSelectedArticle);
  // Unit edition
  connect(actViewUnit, &QAction::triggered, this, &mdtClMainWindow::viewUnit);
  connect(actEditUnit, &QAction::triggered, mdtClApplicationWidgets::instance(), &mdtClApplicationWidgets::editUnits);
  connect(actEditSelectedUnit, &QAction::triggered, this, &mdtClMainWindow::editSelectedUnit);

  // Link list
  connect(actViewLinkList, SIGNAL(triggered()), this, SLOT(viewLinkList()));
  // Link wires
  connect(actViewWires, SIGNAL(triggered()), this, SLOT(viewWire()));
  connect(actEditWire, SIGNAL(triggered()), this, SLOT(editWire()));
  // Link beam
  connect(actViewLinkBeam, SIGNAL(triggered()), this, SLOT(viewLinkBeam()));
  connect(actEditLinkBeam, SIGNAL(triggered()), this, SLOT(editLinkBeam()));
  // Link version
  connect(actCreateLinkVersion, &QAction::triggered, mdtClApplicationWidgets::instance(), &mdtClApplicationWidgets::createLinkVersion);

  // Test connection cable
  connect(actViewTestConnectionCable, SIGNAL(triggered()), this, SLOT(viewTestConnectionCable()));
//   connect(actEditTestConnectionCable, &QAction::triggered, mdtTtApplicationWidgets::instance(), &mdtTtApplicationWidgets::slotEditTestCables);
  ///connect(actEditTestConnectionCable, SIGNAL(triggered()), this, SLOT(editTestConnectionCable()));
  /**
  connect(actCreateTestConnectionCable, SIGNAL(triggered()), this, SLOT(createTestConnectionCable()));
  connect(actConnectTestCable, SIGNAL(triggered()), this, SLOT(connectTestCable()));
  connect(actDisconnectTestCable, SIGNAL(triggered()), this, SLOT(disconnectTestCable()));
  */
  
  // Test system edition
  connect(actEditTestSystem, &QAction::triggered, mdtTtApplicationWidgets::instance(), &mdtTtApplicationWidgets::slotEditTestSystems);
  connect(actEditTestSystemComponent, &QAction::triggered, mdtTtApplicationWidgets::instance(), &mdtTtApplicationWidgets::slotEditTestSystemComponents);
  connect(actEditTestSystemUnit, &QAction::triggered, mdtTtApplicationWidgets::instance(), &mdtTtApplicationWidgets::slotEditTestSystemUnits);

  // Test node edition
  ///connect(actEditTestNode, SIGNAL(triggered()), this, SLOT(editTestNode()));
  ///connect(actEditTestNode, SIGNAL(triggered()), mdtTtApplicationWidgets::instancePtr(), SLOT(slotEditTestNodes()));
  connect(actCalibrateW750_2BusNode, SIGNAL(triggered()), this, SLOT(calibrateW750TestNode()));
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
      auto error = mdtErrorNewQ(tr("Cannot create 'mdtcablelist' directory."), mdtError::Error, this);
      error.commit();
      displayError(error);
      /// \todo Let the user a chance to choose another place to create this directory !
      return false;
    }
    if(!pvWorkDirectory.cd("mdtcablelist")){
      ///QMessageBox msgBox(this);
      auto error = mdtErrorNewQ(tr("Cannot create 'mdtcablelist' directory."), mdtError::Error, this);
      error.commit();
      displayError(error);
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

bool mdtClMainWindow::initDatabase()
{
  if(pvDatabase.isValid()){
    return true;
  }
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE", "cablelist");
  if(!pvDatabase.isValid()){
    displayWarning(tr("Could not create database connection."), tr("Check that qt driver for SQLite is available."));
    return false;
  }
  return true;
}

// bool mdtClMainWindow::importDatabaseSqlite()
// {
//   // Check that we have currently a database open
//   if(!pvDatabaseManager->database().isOpen()){
//     displayWarning(tr("Cannot import a database."), tr("Please open a database and try again."));
//     return false;
//   }
//   /// \todo Save current edition !
//   /// \todo Create a backup from current database !
//   // Let the user a chance to cancel import
//   QMessageBox msgBox(this);
//   msgBox.setText(tr("You are about to import a database into current database."));
//   msgBox.setInformativeText(tr("Do you want to continue ?"));
//   msgBox.setIcon(QMessageBox::Warning);
//   msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
//   if(msgBox.exec() != QMessageBox::Yes){
//     return false;
//   }
//   // Import ...
//   mdtTtDatabaseSchema dbSchema(pvDatabaseManager);
// 
//   return dbSchema.importDatabase(pvWorkDirectory);
// }

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
  mdtErrorDialog dialog(error, this);
  dialog.exec();
}
