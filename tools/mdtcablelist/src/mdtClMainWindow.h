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

class mdtSqlDatabaseManager;
class mdtClVehicleTypeEditor;
class mdtClUnitEditor;
class mdtClConnectorEditor;
class mdtClArticleEditor;
class mdtTtTestNodeEditor;
class mdtTtTestModelEditor;
class mdtTtTestModelItemEditor;
class mdtSqlWindow;
class mdtCcTestConnectionCableEditor;
class mdtTtCableChecker;
class mdtTtCableCheckerWindow;

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

  /*! \brief Edit vehicle types
   */
  void editVehicleType();

  /*! \brief Edir connectors
   */
  void editConnector();

  /*! \brief Edit units
   */
  void editUnit();

  /*! \brief Edit articles
   */
  void editArticle();

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
  void editTestItem();

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

 private:

  Q_DISABLE_COPY(mdtClMainWindow);

  /*! \brief Create actions
   */
  void createActions();

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

  // Database members
  mdtSqlDatabaseManager *pvDatabaseManager;
  QDir pvWorkDirectory;
  // Vehicle type editor
  mdtClVehicleTypeEditor *pvVehicleTypeEditor;
  mdtSqlWindow *pvVehicleTypeEditorWindow;
  // Connector editor
  mdtClConnectorEditor *pvConnectorEditor;
  mdtSqlWindow *pvConnectorEditorWindow;
  // Articel editor
  mdtClArticleEditor *pvArticleEditor;
  mdtSqlWindow *pvArticleEditorWindow;
  
  // Editors
  
  mdtClUnitEditor *pvUnitEditor;
  
  mdtTtTestNodeEditor *pvTestNodeEditor;
  // Test connection cable editor
  mdtCcTestConnectionCableEditor *pvTestConnectionCableEditor;
  mdtSqlWindow *pvTestConnectionCableEditorWindow;
  // Test editor
  mdtTtTestModelEditor *pvTestEditor;
  // Test item editor
  mdtTtTestModelItemEditor *pvTestItemEditor;
  mdtSqlWindow *pvTestItemEditorWindow;
  // Cable checker
  mdtTtCableChecker *pvCableChecker;
  mdtTtCableCheckerWindow *pvCableCheckerWindow;
};

#endif // #ifndef MDT_CL_MAIN_WINDOW_H
