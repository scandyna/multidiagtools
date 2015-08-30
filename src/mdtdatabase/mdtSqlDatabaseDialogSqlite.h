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
#ifndef MDT_SQL_DATABASE_DIALOG_SQLITE_H
#define MDT_SQL_DATABASE_DIALOG_SQLITE_H

#include "mdtSqlDatabaseSqlite.h"
#include "ui_mdtSqlDatabaseDialogSqlite.h"
#include "mdtError.h"
#include <QDialog>

/*! \brief Dialog for SQLite connection
 *
 * \sa mdtSqlDatabaseSqlite
 */
class mdtSqlDatabaseDialogSqlite : public QDialog, Ui::mdtSqlDatabaseDialogSqlite
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   *
   * Will also list currently available database connections.
   *  Note: if a connection is created with QSqlDatabase::addDatabase()
   *        after creation of this dialog, it will not be listed.
   *        Use updateConnectionsList() in this case.
   */
  mdtSqlDatabaseDialogSqlite(QWidget *parent = nullptr);

  /*! \brief Update connections list
   *
   * Update the connections combo box with currently available
   *  connections that uses the SQLite driver.
   */
  void updateConnectionsList();

  /*! \brief Set database
   *
   * Will also set db's connection as selected connection
   */
  void setDatabase(const mdtSqlDatabaseSqlite & db);

  /*! \brief Get selected database
   */
  mdtSqlDatabaseSqlite database() const
  {
    return pvDatabase;
  }

 private slots:

  /*! \brief Update regarding given database
   */
  void updateDatabase(const QSqlDatabase & db);

  /*! \brief Check if internal database must be cleared before connection is removed
   */
  void maybeClearDatabase(const QString & connectionName);

  /*! \brief Let user choose a databse file and open it
   */
  void openDatabase();

  /*! \brief Clear database name
   */
  void clearDatabaseName();

  /*! \brief Create a database
   */
  void createDatabase();

  /*! \brief Close database
   */
  void closeDatabase();

  /*! \brief Delete database
   */
  void deleteDatabase();

 private:

  /*! \brief Update database informations
   */
  void updateDatabaseInformations();

  /*! \brief Update buttons state
   */
  void updateButtonsState();

  /*! \brief Display error
   */
  void displayError(const mdtError & error);

  Q_DISABLE_COPY(mdtSqlDatabaseDialogSqlite)

  mdtSqlDatabaseSqlite pvDatabase;
};

#endif // #ifndef MDT_SQL_DATABASE_DIALOG_SQLITE_H
