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
#ifndef MDT_SQL_DATABASE_SQLITE_H
#define MDT_SQL_DATABASE_SQLITE_H

#include "mdtError.h"
#include <QSqlDatabase>
#include <QString>
#include <QStringList>
#include <QFileInfo>

/*! \brief Helper class for SQLite database connection and creation
 *
 * To handle database connections, QSqlDatabase provides all the needed stuff.
 *  mdtSqlDatabaseSqlite is a helper class the adds some logic to deal with common cases
 *  when creating or opening a SQLite database (behaviour whenn opening a non existing file
 *  or creating a existing file, etc...)
 *
 * \sa mdtSqlDatabaseDialogSqlite
 */
class mdtSqlDatabaseSqlite
{
 public:

  /*! \brief Creation mode
   */
  enum CreateMode{
    OverwriteExisting,  /*!< If file/database/table allready exists, it will be overwritten */
    KeepExisting,       /*!< If file/database/table allready exists, it will simply be open */
    FailIfExists,       /*!< If file/database/table allready exists, create function will fail */
  };

  /*! \brief Default constructor
   *
   * Contruct a invalid database object
   */
  mdtSqlDatabaseSqlite();

  /*! \brief Constructor
   *
   * Construct a database object based on db.
   *
   * \pre db must use the SQLite driver (see Qt's QSqlDatabase documentation about driver for details)
   */
  mdtSqlDatabaseSqlite(const QSqlDatabase & db);

  /*! \brief Check validity
   *
   * Returns true if database has a valid SQLite driver.
   */
  bool isValid() const;

  /*! \brief Clear
   *
   * After clear, this instance does not refer to a databse connection
   *  anymore and becomes invalid.
   *
   * \note It is not checked if database is open or not before clear.
   */
  void clear();

  /*! \brief Set database
   *
   * \pre db must use the SQLite driver (see Qt's QSqlDatabase documentation about driver for details)
   */
  void setDatabase(const QSqlDatabase & db);

  /*! \brief Get database
   */
  QSqlDatabase database() const
  {
    return pvDatabase;
  }

  /*! \brief Open a database
   *
   * Will check if given file exists, set it as database name
   *  and open it.
   *
   * \pre Database must be valid (\sa isValid() )
   * \pre Database must not allready be open (\sa isOpen() )
   * \pre fileInfo must contain a non empty path (existence of file is checked, and is not a precondition)
   * \sa lastError()
   *
   * \todo Create options (like FK support, sync mode, ...)
   */
  bool openDatabase(const QFileInfo & fileInfo);

  /*! \brief Open allready set database
   *
   * \pre Database must be valid (\sa isValid() )
   * \pre Database must not allready be open (\sa isOpen() )
   * \pre Database name must allready been set (f.ex. was previously open with openDatabase(const QFileInfo &) )
   * \sa lastError()
   *
   * \todo Create options (like FK support, sync mode, ...)
   */
  bool openDatabase();

  /*! \brief Create a database and open it
   *
   * \pre Database must be valid (\sa isValid() )
   * \pre Database must not allready be open (\sa isOpen() )
   * \pre fileInfo must contain a non empty path (existence of file is checked, and is not a precondition)
   * \sa lastError()
   *
   * \todo Create options (like FK support, sync mode, ...)
   */
  bool createDatabase(const QFileInfo & fileInfo, CreateMode createMode);

  /*! \brief Check if database is open
   */
  bool isOpen() const
  {
    return pvDatabase.isOpen();
  }

  /*! \brief Close database
   *
   * Closes the database connection, freeing any resources acquired, and invalidating any existing QSqlQuery objects that are used with the database.
   *
   * This will also affect copies of this QSqlDatabase object.
   *
   * \sa clearDatabaseName()
   */
  void close()
  {
    pvDatabase.close();
  }

  /*! \brief Clear database name
   *
   * \pre Database mut not be open (\sa isOpen() )
   */
  void clearDatabaseName()
  {
    Q_ASSERT(!isOpen());
    pvDatabase.setDatabaseName("");
  }

  /*! \brief Delete database
   *
   * \warning Will delete given file path without confirmation.
   * \pre Database mut not be open (\sa isOpen() )
   */
  bool deleteDatabase(const QString & filePath);

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

  /*! \brief Get a list containing the name of all SQLite connections
   *
   * This is the same as QSqlDatabase::connectionNames() ,
   *  but only contains connections that uses the SQLite driver.
   *
   * Note: each call of this function will build the list, also fetching some information, wich is not fast.
   */
  [[deprecated]]
  static QStringList getConnectionNames();

 private:

  /*! \brief Open database (common stuff)
   */
  bool openDatabasePv();

  /*! \brief Call QObject::tr()
   */
  static QString tr(const char *sourceText, const char *disambiguation = nullptr, int n = -1);

  QSqlDatabase pvDatabase;
  mdtError pvLastError;
};

#endif // #ifndef MDT_SQL_DATABASE_SQLITE_H
