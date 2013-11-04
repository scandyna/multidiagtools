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
class mdtClArticleEditor;

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

  /*! \brief Edit units
   */
  void editUnit();

  /*! \brief Edit articles
   */
  void editArticle();

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
  // Editors
  mdtClVehicleTypeEditor *pvVehicleTypeEditor;
  mdtClUnitEditor *pvUnitEditor;
  mdtClArticleEditor *pvArticleEditor;
  // Actions
  ///QAction *pvActEditVehicleType;
  QAction *pvActEditUnit;
  QAction *pvActEditArticle;
};

#endif // #ifndef MDT_CL_MAIN_WINDOW_H
