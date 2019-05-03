/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_SQL_CONNECTION_H
#define MDT_SQL_CONNECTION_H

#include "MdtSql_CoreExport.h"
#include <QSqlDatabase>
#include <QString>
#include <QStringList>

namespace Mdt{ namespace Sql{

  /*! \brief Identifier for a connection to a database
   *
   * A usual way to refer to a specific connection using Qt SQL API
   *  is use a connection name:
   * \code
   * QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "Connection1");
   * if(!db.isValid()){
   *   // Error, probably no SQLite driver could be loaded
   * }
   * db.setDatabaseName("/path/to/db.sqlite");
   * if(!db.open()){
   *   // Error
   * }
   * \endcode
   *
   * Later in the code, a handle to this connection can be retrieved:
   * \code
   * QSqlDatabase db = QSqlDatabase::database("Connection1");
   * QSqlQuery query(db);
   * \endcode
   *
   * Having to ty the connection name everywhere is not ideal.
   *  It should be passed to functions and classes:
   * \code
   * bool executeQuery(const QString & sql, const QString & connectionName)
   * {
   *   auto db = QSqlDatabase::database(connectionName);
   *   QSqlQuery query(db);
   *   ...
   * }
   * \endcode
   *
   * Above code is not type safe:
   * \code
   * // Wrong! And it compiles
   * executeQuery("Connection1", "SELECT * FROM Customer");
   * \endcode
   *
   * Using Connection to init a database can be done like:
   * \code
   * Connection connection1("Connection1");
   * QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connection1.name());
   * if(!db.isValid()){
   *   // Error, probably no SQLite driver could be loaded
   * }
   * db.setDatabaseName("/path/to/db.sqlite");
   * if(!db.open()){
   *   // Error
   * }
   * \endcode
   *
   * The connection can be passed to function and classes:
   * \code
   * bool executeQuery(const QString & sql, const Connection & connection)
   * {
   *   auto db = connection.database();
   *   QSqlQuery query(db);
   *   ...
   * }
   * \endcode
   *
   * Calling this function with wrong arguments will now fail to compile:
   * \code
   * // Wrong! Compile error
   * executeQuery(connection1, "SELECT * FROM Customer");
   * \endcode
   *
   * At some place, the database handle should be closed:
   * \code
   * Connection::close(connection1);
   * \endcode
   *
   * When no more used, it should also be removed from the list of connections:
   * \code
   * Connection::remove(connection1);
   * \endcode
   *
   * \note Connection is only a wrapper to a connection name.
   *   It does not manage any lifetime to the database handle.
   *
   * Notice that Connection does not provide methods to add connections and open databases.
   *  Those methods are available in database specific implementations.
   *  \sa SQLiteDatabase
   *
   * \sa \ref libs_Sql
   */
  class MDT_SQL_CORE_EXPORT Connection
  {
   public:

    /*! \brief Construct a connection
     *
     * \pre \a name must not be empty
     */
    explicit Connection(const QString & name)
     : mName(name.trimmed())
    {
      Q_ASSERT(!mName.isEmpty());
    }

    /*! \brief Copy construct a connection from \a other
     */
    Connection(const Connection & other) = default;

    /*! \brief Copy assign \a other to this connection
     */
    Connection & operator=(const Connection & other) = default;

    /*! \brief Move construct a connection from \a other
     */
    Connection(Connection && other) = default;

    /*! \brief Move assign \a other to this connection
     */
    Connection & operator=(Connection && other) = default;

    /*! \brief Get the name of this connection
     */
    QString name() const
    {
      return mName;
    }

    /*! \brief Ge the database connection called by the name of this connection
     *
     * This is equivalent to:
     * \code
     * Connection connection("A");
     *
     * QSqlDatabase db = QSqlDatabase::database(connection.name(), false);
     * \endcode
     */
    QSqlDatabase database() const
    {
      return QSqlDatabase::database(mName, false);
    }

    /*! \brief Close a connection
     *
     * This is equivalent to:
     * \code
     * QSqlDatabase db = QSqlDatabase::database(connection.name(), false);
     * db.close();
     * \endcode
     */
    static void close(const Connection & connection)
    {
      QSqlDatabase::database(connection.name(), false).close();
    }

    /*! \brief Remove a connection from the list of database connections
     *
     * This is equivalent to:
     * \code
     * QSqlDatabase::removeDatabase(connection.name());
     * \endcode
     */
    static void remove(const Connection & connection)
    {
      QSqlDatabase::removeDatabase(connection.name());
    }

    /*! \brief Generate a connection name regarding the existing onces
     */
    static QString generateConnectionName(const QStringList & existingNames, const QString & namePrefix = QString());

   private:

    QString mName;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_CONNECTION_H
