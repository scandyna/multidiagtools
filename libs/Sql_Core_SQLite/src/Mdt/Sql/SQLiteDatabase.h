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
#ifndef MDT_SQL_SQLITE_DATABASE_H
#define MDT_SQL_SQLITE_DATABASE_H

#include "SQLiteOpenMode.h"
#include "SQLiteConnectionParameters.h"
#include "Mdt/Sql/Connection.h"
#include "Mdt/Error.h"
#include "Mdt/Expected.h"
#include "MdtSql_Core_SQLiteExport.h"
#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QLatin1String>
#include <QCoreApplication>

namespace Mdt{ namespace Sql{

  /*! \brief %Connection to a SQLite database
   *
   * This is a helper class to setup a connection
   *  to a SQLite database and open or create a database.
   *  It also applies the options present in given parameters.
   *
   * Here is a example of the recommanded usage:
   * \code
   * Mdt::Expected<Connection> connection = SQLiteDatabase::addConnection();
   * if(!connection){
   *   // Error, probably there is no SQLite driver available
   * }
   *
   * SQLiteConnectionParameters parameters;
   * parameters.setDatabaseFile("/path/to/db.sqlite");
   *
   * SQLiteDatabase sqliteDb(*connection, parameters);
   * if(!sqliteDb.openExisting(){
   *   if(!sqliteDb.createNewAndOpen()){
   *     // Error handling
   *   }
   * }
   *
   * QSqlDatabase dbConnection = connection.database();
   * QSqlQuery query(dbConnection);
   * \endcode
   *
   * SQLiteDatabase works on QSqlDatabase and it does not handle any lifetime of a connection.
   *  This makes it usable, for example, in a setup dialog, as helper class.
   *
   * \sa \ref libs_Sql
   */
  class MDT_SQL_CORE_SQLITE_EXPORT SQLiteDatabase : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Construct a SQLite database connection
     *
     * \pre \a connection must refer to a valid database connection handle,
     *    connection.database().isValid() must be true.
     *    The handle must also have a SQLite driver loaded.
     */
    explicit SQLiteDatabase(const Connection & connection, const SQLiteConnectionParameters & parameters, QObject *parent = nullptr);

    /*! \brief Create a new database file
     *
     * This method will check that the file path:
     *  - does not refers to a existing file
     *  - does not refer to a directory
     *
     * If one the checks fails, or a other error occures,
     *  a error will be returned.
     */
    Mdt::ExpectedResult createNew();

    /*! \brief Open a existing database file
     *
     * This method will check that the file path:
     *  - refers to a existing file
     *  - does not refer to a directory
     *  - is a SQLite database
     *
     * If one the checks fails, or a other error occures,
     *  a error will be returned.
     *
     */
    Mdt::ExpectedResult openExisting();

    /*! \brief Open a database file
     *
     * This method is more like a direct call to QSqlDatabase::open(),
     *  but it also set connect options, and setup the database regarding parameters.
     *
     * Regarding QSqlDatabase and SQLite sqlite3_open_v2() documentation,
     *  these rules applies:
     *   - If the open mode is read only, the database is opened in read-only mode.
     *      If the database does not already exist, an error is returned.
     *   - If the open mode is read/write, the database is opened for reading and writing,
     *      and is created if it does not already exist.
     *
     * To avoid potential corruption of a existing file that is not a SQLite database,
     *  it is also checked that the opened file is a SQLite database.
     *
     * \sa isSQLiteDatabaseOpen()
     * \sa createNew()
     * \sa openExisting()
     */
    Mdt::ExpectedResult open();

    /*! \brief Check if the connection this database handle refers to has a SQLite database open
     *
     * If no file is open, false is returned.
     *
     * If a file was open, returns true if it is a SQLite database,
     *  otherwise false.
     */
    bool isSQLiteDatabaseOpen();

    /*! \brief Add a connection to a SQLite database
     *
     * Returns a valid connection if a SQLite driver could be loaded,
     *  otherwise a error of type Mdt::ErrorCode::DriverNotFound .
     */
    static Mdt::Expected<Connection> addConnection(const QString & connectionNamePrefix = QString());

    /*! \brief Check if \a driverName is the name of a SQLite driver
     */
    static bool isSQLiteDriver(const QString & driverName) noexcept;

   private:

    static bool hasSQLiteDriverLoaded(const QSqlDatabase & db) noexcept;
    Mdt::Expected<qlonglong> getSchemaVersion(const QSqlDatabase & db);
    Mdt::ExpectedResult enableForeignKeySupport(const QSqlDatabase & db);
    void setConnectOptions(SQLiteOpenMode openMode);
//     void setLastError(const Mdt::Error & error);

    Connection mConnection;
    SQLiteConnectionParameters mParameters;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SQLITE_DATABASE_H
