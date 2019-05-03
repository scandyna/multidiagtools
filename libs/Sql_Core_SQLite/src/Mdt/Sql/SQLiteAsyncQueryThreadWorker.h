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
#ifndef MDT_SQL_SQLITE_ASYNC_QUERY_THREAD_WORKER_H
#define MDT_SQL_SQLITE_ASYNC_QUERY_THREAD_WORKER_H

#include "SQLiteConnectionParameters.h"
#include "Mdt/Sql/AbstractAsyncQueryThreadWorker.h"
#include "Mdt/Expected.h"
#include "MdtSql_Core_SQLiteExport.h"
#include <QString>

namespace Mdt{ namespace Sql{

  /*! \brief SQLite implementation of async query thread worker
   */
  class MDT_SQL_CORE_SQLITE_EXPORT SQLiteAsyncQueryThreadWorker : public AbstractAsyncQueryThreadWorker
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    SQLiteAsyncQueryThreadWorker(const SQLiteConnectionParameters & parameters, const QString & connectionNamePrefix = QString());

    /*! \brief Open a database file
     *
     * \sa SQLiteDatabase::open()
     */
    Mdt::ExpectedResult open();

   public slots:

    /*! \brief Open a database file
     *
     * \sa SQLiteDatabase::open()
     */
    void processOpen();

   private:

    SQLiteConnectionParameters mParameters;
    QString mConnectionNamePrefix;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SQLITE_ASYNC_QUERY_THREAD_WORKER_H
