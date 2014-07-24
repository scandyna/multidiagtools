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
#ifndef MDT_CL_MAIN_WINDOW_H
#define MDT_CL_MAIN_WINDOW_H

#include "ui_mdtClMainWindow.h"
#include "mdtError.h"
#include <QMainWindow>
#include <QDir>
#include <QString>
#include <QList>
#include <QMap>

class mdtSqlForm;
class mdtSqlTableWidget;
class mdtSqlDatabaseManager;
class mdtClVehicleTypeEditor;
class mdtClUnitEditor;
class mdtClConnectorEditor;
class mdtClArticleEditor;
class mdtTtTestNodeEditor;
class mdtTtTestModelEditor;
///class mdtTtTestModelItemEditor;
class mdtSqlWindow;
class mdtTtTestConnectionCableEditor;
class mdtTtBasicTester;
class mdtTtBasicTesterWindow;

class mdtTtCableChecker;
class mdtTtCableCheckerWindow;
class mdtClLinkBeamEditor;
class QTabWidget;
class QAction;

/*! \brief Main cable list window
 */
class mdtClMainWindow : public QMainWindow, Ui::mdtClMainWindow
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClMainWindow();

  /*! \brief Destructor
   */
  ~mdtClMainWindow();

 private slots:

  /*! \brief Open a database
   */
  void openDatabase();

  /*! \brief Close a database
   */
  void closeDatabase();

  /*! \brief Create a new database
   */
  void createNewDatabase();

  /*! \brief Import a database
   */
  void importDatabase();

  /*! \brief View vehicle types
   */
  void viewVehicleType();

  /*! \brief Update vehicle type menu
   */
  void updateVehicleTypeMenu();

  /*! \brief Update the list of working vehicle types
   */
  void updateWorkingOnVehicleTypes();

  /*! \brief Edit vehicle types
   */
  void editVehicleType();

  /*! \brief View connectors
   */
  void viewConnector();

  /*! \brief Edit connectors
   */
  void editConnector();

  /*! \brief Edit articles
   */
  void viewArticle();

  /*! \brief Edit articles
   */
  void editArticle();

  /*! \brief View units
   */
  void viewUnit();

  /*! \brief Edit units
   */
  void editUnit();

  /*! \brief Edit selected unit
   */
  void editSelectedUnit();

  /*! \brief View link list
   */
  void viewLinkList();

  /*! \brief View link beam
   */
  void viewLinkBeam();

  /*! \brief Edit link beam
   */
  void editLinkBeam();

  /*! \brief Edit test node
   */
  void editTestNode();

  /*! \brief Edit test connection cable
   */
  void editTestConnectionCable();

  /*! \brief Edit test
   */
  void editTest();

  /*! \brief Edit test items
   */
  ///void editTestItem();

  /*! \brief Run basic tester
   */
  void runBasicTester();

  /*! \brief Run cable checker
   */
  void runCableChecker();

  /*! \brief Create a test connection cable
   */
  ///void createTestConnectionCable();

  /*! \brief Connect a test cable
   */
  ///void connectTestCable();

  /*! \brief Disconnect a test cable
   */
  ///void disconnectTestCable();

  /*! \brief Called when user close a tab from pvTabWidget
   */
  void closeTableView(int index);

 private:

  Q_DISABLE_COPY(mdtClMainWindow);

  /*! \brief Create vehicle type table view
   */
  bool createVehicleTypeTableView();

  /*! \brief Create the vehicle types actions
   *
   * Will also update vehicle types menu
   */
  bool createVehicleTypeActions();

  /*! \brief Remove vehicle type actions
   */
  void removeVehicleTypeActions();

  /*! \brief Get vehicle type editor
   */
  mdtClVehicleTypeEditor *getVehicleTypeEditor();

  /*! \brief Create vehicle type editor
   */
  mdtClVehicleTypeEditor *createVehicleTypeEditor();

  /*! \brief Create connector table view
   */
  bool createConnectorTableView();

  /*! \brief Get connector editor
   */
  mdtClConnectorEditor *getConnectorEditor();

  /*! \brief Create connector editor
   */
  mdtClConnectorEditor *createConnectorEditor();

  /*! \brief Create article table view
   */
  bool createArticleTableView();

  /*! \brief Get article editor
   */
  mdtClArticleEditor *getArticleEditor();

  /*! \brief Create article editor
   */
  mdtClArticleEditor *createArticleEditor();

  /*! \brief Create unit table view
   */
  bool createUnitTableView();

  /*! \brief Get unit editor
   */
  mdtClUnitEditor *getUnitEditor();

  /*! \brief Create unit editor
   */
  mdtClUnitEditor *createUnitEditor();

  /*! \brief Create link list table view
   */
  bool createLinkListTableView();

  /*! \brief Create link beam table view
   */
  bool createLinkBeamTableView();

  /*! \brief Get link beam editor
   */
  mdtClLinkBeamEditor *getLinkBeamEditor();

  /*! \brief Create link beam editor
   */
  mdtClLinkBeamEditor *createLinkBeamEditor();

  /*! \brief Get test node editor
   */
  mdtTtTestNodeEditor *getTestNodeEditor();

  /*! \brief Create test node editor
   */
  mdtTtTestNodeEditor *createTestNodeEditor();

  /*! \brief Get test connection cable editor
   */
  mdtTtTestConnectionCableEditor *getTestConnectionCableEditor();

  /*! \brief Create test connection cable editor
   */
  mdtTtTestConnectionCableEditor *createTestConnectionCableEditor();

  /*! \brief Get test model editor
   */
  mdtTtTestModelEditor *getTestModelEditor();

  /*! \brief Create test model editor
   */
  mdtTtTestModelEditor *createTestModelEditor();

  /*! \brief Create basic tester
   */
  void createBasicTester();

  /*! \brief Create a table view
   *
   * Will create a new SQL table widget that acts on given table name.
   *  Common setup will also be done here.
   *
   * On success, a pointer to created table widget is returned,
   *  null on failure.
   *
   * On success, the view is also added to pvTabWidget and pvOpenViews.
   */
  mdtSqlTableWidget *createTableView(const QString & tableName, const QString & userFriendlyTableName);

  /*! \brief Get a table view
   *
   * If requested table view not exists in pvOpenViews,
   *  this method returns a null pointer.
   */
  mdtSqlTableWidget *getTableView(const QString & tableName);

  /*! \brief Display a table view
   *
   * If requested table view not exists in pvOpenViews,
   *  this method returns false.
   */
  bool displayTableView(const QString & tableName);

  /*! \brief Close and delete table views
   */
  void deleteTableviews();

  /*! \brief Setup given editor in mdtSqlWindow
   *
   * Will do setup of things that are common to all editors,
   *  such as calling setupTables(),
   *  create and setup a new window, ...
   *
   * Returned pointer can be used, for example, to resize the editor.
   *
   * Freshliy created window will be added to open editors list.
   *
   * Note: if setup fails, the given editor will be destroyed,
   *  and this method returns a null pointer.
   *
   * \pre editor must be a valid pointer.
   */
  mdtSqlWindow *setupEditor(mdtSqlForm *editor);

  /*! \brief Get editor of type T
   *
   * If given editor of type T (for example mdtClVehicleTypeEditor)
   *  exists in pvOpenEditors, a pointer to its instance is returned,
   *  else a null pointer is returned.
   */
  template <class T> T *getEditor();

  /*! \brief Get the window for given editor
   */
  mdtSqlWindow *getEditorWindow(mdtSqlForm *editor);

  /*! \brief Delete editors
   *
   * Will try to close each open editor.
   *  If a editor has unsaved data,
   *  it will not be deleted, and this method returns false.
   */
  bool deleteEditors();

  /*! \brief Give a chance to the user to save data before close
   */
  void closeEvent(QCloseEvent *event);

  /*! \brief Create actions
   *
   * \deprecated
   */
  void connectActions();

  /*! \brief Init work directory
   */
  bool initWorkDirectory();

  /*! \brief Open a Sqlite database
   *
   * \todo Manage default/previous opened DB, let user choose, etc...
   */
  bool openDatabaseSqlite();

  /*! \brief Create a new Sqlite database
   */
  bool createDatabaseSqlite();

  /*! \brief Import a Sqlite database into currently open database
   */
  bool importDatabaseSqlite();

  /*! \brief Display a warning to the user
   */
  void displayWarning(const QString & text, const QString & informativeText = "");

  /*! \brief Display a error to the user
   */
  void displayError(const mdtError & error);

  // View and editor container
  QList<mdtSqlTableWidget*> pvOpenViews;
  QList<mdtSqlWindow*> pvOpenEditors;
  // Basic tester
  mdtTtBasicTester *pvBasicTester;
  mdtTtBasicTesterWindow *pvBasicTesterWindow;
  
  // Vehicle types
  QMap<QAction*, int> pvVehicleTypeActions;
  QList<QVariant> pvWorkingOnVehicleTypeList;
  // Central widget
  QTabWidget *pvTabWidget;
  // Database members
  mdtSqlDatabaseManager *pvDatabaseManager;
  QDir pvWorkDirectory;

  // Test item editor
  ///mdtTtTestModelItemEditor *pvTestItemEditor;
  ///mdtSqlWindow *pvTestItemEditorWindow;
  // Cable checker
  mdtTtCableChecker *pvCableChecker;
  mdtTtCableCheckerWindow *pvCableCheckerWindow;
};

#endif // #ifndef MDT_CL_MAIN_WINDOW_H
