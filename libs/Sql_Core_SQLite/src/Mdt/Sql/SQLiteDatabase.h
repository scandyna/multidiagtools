/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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

#include "Mdt/Error.h"
#include "Mdt/Expected.h"
#include "MdtSql_Core_SQLiteExport.h"
#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QLatin1String>
#include <QCoreApplication>

namespace Mdt{ namespace Sql{

  /*! \brief Connection to a SQLite database
   *
   * \code
   * SQLiteDatabase sqliteDb;
   * if(!sqliteDb.openExisting("/path/to/db.sqlite"){
   *   // Error handling
   * }
   * QSqlDatabase dbConnection = sqliteDb.database();
   * \endcode
   */
  class MDT_SQL_CORE_SQLITE_EXPORT SQLiteDatabase : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Open mode
     */
    enum OpenMode
    {
      ReadOnly, /*!< Read only mode */
      ReadWrite /*!< Read and write mode */
    };

    /*! \brief Construct a SQLite database connection
     */
    SQLiteDatabase(const QString & connectionName = QLatin1String("qt_sql_default_connection"), QObject *parent = nullptr);

    /*! \brief Open a existing database file
     */
    bool openExisting(const QString & dbFilePath, OpenMode openMode = ReadWrite);

    /*! \brief Create a new database file
     */
    bool createNew(const QString & dbFilePath);

    /*! \brief Get the database connection
     */
    QSqlDatabase database() const
    {
      return mDatabase;
    }

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

    /*! \brief Check if \a driverName is the name of a SQLite driver
     */
    static bool isSQLiteDriver(const QString & driverName) noexcept;

   private:

    static bool hasSQLiteDriverLoaded(const QSqlDatabase & db) noexcept;
    Mdt::Expected<qlonglong> getSchemaVersion();
    bool enableForeignKeySupport();
    void setLastError(const Mdt::Error & error);

    QSqlDatabase mDatabase;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SQLITE_DATABASE_H
