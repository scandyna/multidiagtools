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
#ifndef MDT_SQL_SQLITE_CONNECTION_PARAMETERS_H
#define MDT_SQL_SQLITE_CONNECTION_PARAMETERS_H

#include "SQLiteOpenMode.h"
#include "Mdt/Sql/ConnectionParameters.h"
#include "MdtSql_Core_SQLiteExport.h"
#include <QString>

namespace Mdt{ namespace Sql{

  /*! \brief Parameters for a SQLite database connection
   */
  class MDT_SQL_CORE_SQLITE_EXPORT SQLiteConnectionParameters
  {
   public:

    /*! \brief Construct empty parameters
     */
    SQLiteConnectionParameters() noexcept = default;

    /*! \brief Copy construct parametrs from \a other
     */
    SQLiteConnectionParameters(const SQLiteConnectionParameters & other) = default;

    /*! \brief Copy assign \a other  to this parametrs
     */
    SQLiteConnectionParameters & operator=(const SQLiteConnectionParameters & other) = default;

    /*! \brief Move construct parametrs from \a other
     */
    SQLiteConnectionParameters(SQLiteConnectionParameters && other) = default;

    /*! \brief Move assign \a other  to this parametrs
     */
    SQLiteConnectionParameters & operator=(SQLiteConnectionParameters && other) = default;

    /*! \brief Set path to the SQLite database file
     */
    void setDatabaseFile(const QString & path);

    /*! \brief Get path to the SQLite database file
     */
    QString databaseFile() const
    {
      return mFilePath;
    }

    /*! \brief Set database file open mode
     *
     * The default open mode is read / write
     */
    void setOpenMode(SQLiteOpenMode mode) noexcept
    {
      mOpenMode = mode;
    }

    /*! \brief Get database file open mode
     */
    SQLiteOpenMode openMode() const noexcept
    {
      return mOpenMode;
    }

    /*! \brief Get a connection parameters from this
     */
    ConnectionParameters toConnectionParameters() const;

   private:

    QString mFilePath;
    SQLiteOpenMode mOpenMode = SQLiteOpenMode::ReadWrite;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SQLITE_CONNECTION_PARAMETERS_H
