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

#include <QSqlDatabase>
#include <QStringList>

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
   * Returns true if database has a valid driver (exactly like QSqlDatabase::isValid()).
   */
  bool isValid() const
  {
    return pvDatabase.isValid();
  }

  /*! \brief Clear
   *
   * If database is open, it will first close it before clear.
   *  After clear, database object becomes invalid.
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

  QSqlDatabase pvDatabase;
};

#endif // #ifndef MDT_SQL_DATABASE_SQLITE_H
