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
#ifndef MDT_SQL_DATABASE_MANAGER_H
#define MDT_SQL_DATABASE_MANAGER_H

#include "mdtError.h"
#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QFileInfo>
#include <QDir>

class mdtSqlSchemaTable;
class QWidget;  

/*! \brief Helper class to create and open (connect to) database
 *
 * QSqlDatabase allready offers needed methods to open a database.
 *  This helper class use it, but gives some methods witch will
 *  deal with common problems. For example, with Sqlite, checking if a file exists,
 *  what to do if it exists and we are creating a new database, ...
 */
class mdtSqlDatabaseManager : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Creation mode
   */
  enum createMode_t{
                    OverwriteExisting,  /*!< If file/database/table allready exists, it will be overwritten */
                    KeepExisting,       /*!< If file/database/table allready exists, it will simply be open */
                    FailIfExists,       /*!< If file/database/table allready exists, create method will fail */
                    AskUserIfExists     /*!< If file/database/table allready exists, 
                                              a dialog box will ask the user if it must be overwritten or not */
  };

  /*! \brief Constructor
   */
  mdtSqlDatabaseManager(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtSqlDatabaseManager();

  /*! \brief Set a allready existing database
   */
  void setDatabase(QSqlDatabase db);

  /*! \brief Get current database object
   */
  QSqlDatabase database();

  /*! \brief Set the parent widget for dialogs
   *
   * In interactive mode, some dialogs will be displayed.
   *  If widget is set, dialogs will be created as parent of it.
   */
  void setForDialogParentWidget(QWidget *widget);

  /*! \brief Get last error
   */
  mdtError lastError() const;

  /*! \brief Let the user choose a Sqlite database file
   *
   * The startDirectory is used as directory to go when displaying the file dialog
   */
  QFileInfo chooseDatabaseSqlite(const QDir & startDirectory = QDir());

  /*! \brief Open a sqlite database
   */
  bool openDatabaseSqlite(const QFileInfo & fileInfo, const QString & connectionName = QString());

  /*! \brief Open a sqlite database
   *
   * Will ask the user to choose a file and try to open it .
   *  The startDirectory is used as directory to go when displaying the file dialog .
   */
  bool openDatabaseSqlite(const QDir & startDirectory = QDir(), const QString & connectionName = QString());

  /*! \brief Ask the user to give a path and create a Sqlite database
   *
   * The startDirectory is used as directory to go when displaying the file dialog
   */
  bool createDatabaseSqlite(const QDir & startDirectory = QDir(), const QString & connectionName = QString());

  /*! \brief Create a Sqlite database
   *
   * Note: a open connection to database will not be closed by this method.
   *  The caller must close existing connection to target database explicitly before overwrite the same.
   */
  bool createDatabaseSqlite(const QFileInfo & fileInfo, createMode_t createMode, const QString & connectionName = QString());

  /*! \brief Create a table
   *
   * Will also setup database specific thing (like driver name) on table.
   */
  bool createTable(mdtSqlSchemaTable & table, createMode_t createMode);

 private:

  /*! \brief Let the user choose if a table must be overwritten
   */
  bool userChooseToOverwriteTable(const QString &tableName);

  /*! \brief Let the oser choose if a file must be overwritten
   */
  bool userChooseToOverwriteFile(const QFileInfo &fileInfo);

  /*! \brief Check if database is allready open
   *
   * If askUser is set and database is open, a dialog will ask
   *  the user if current database can be closed.
   *
   * pvLastError will also contain the error if database is still open.
   * \return True if database is open.
   */
  bool databaseIsOpen(bool askUser);

  Q_DISABLE_COPY(mdtSqlDatabaseManager);

  QSqlDatabase pvDatabase;
  QString pvDatabaseName;
  QWidget *pvForDialogParentWidget;
  mdtError pvLastError;
};


#endif // #ifndef MDT_SQL_DATABASE_MANAGER_H
