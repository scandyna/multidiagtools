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
#ifndef MDT_SQL_SQLITE_ASYNC_QUERY_CONNECTION_H
#define MDT_SQL_SQLITE_ASYNC_QUERY_CONNECTION_H

#include "SQLiteConnectionParameters.h"
#include "Mdt/Sql/AsyncQueryConnection.h"
#include "MdtSql_Core_SQLiteExport.h"

namespace Mdt{ namespace Sql{

  /*! \brief Connection to a SQLite database for async queries
   *
   * \code
   * using namespace Mdt::Sql;
   *
   * SQLiteConnectionParameters parameters;
   * parameters.setDatabaseFile("/path/to/dbFile.sql");
   *
   * auto connection = std::make_shared<SQLiteAsyncQueryConnection>();
   * if( !connection->open(parameters) ){
   *   handleError( connection->lastError() );
   * }
   * \endcode
   */
  class MDT_SQL_CORE_SQLITE_EXPORT SQLiteAsyncQueryConnection : public AsyncQueryConnection
  {
    Q_OBJECT

   public:

    /*! \brief Create a async query connection
     */
    explicit SQLiteAsyncQueryConnection(QObject *parent = nullptr);

    /*! \brief Submit to open a database for this connection
     *
     * \note If a database connection is currently open,
     *   it will be closed first.
     *
     * \sa close()
     */
    void submitOpen(const SQLiteConnectionParameters & parameters, const QString & connectionNamePrefix = QString());

    /*! \brief Open a database for this connection
     *
     * \note If a database connection is currently open,
     *   it will be closed first.
     *
     * \sa close()
     */
    bool open(const SQLiteConnectionParameters & parameters, const QString & connectionNamePrefix = QString());
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SQLITE_ASYNC_QUERY_CONNECTION_H
