/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_SQL_COPIER_DATABASE_COPY_THREAD
#define MDT_SQL_COPIER_DATABASE_COPY_THREAD

#include "CopyThread.h"
#include "DatabaseMapping.h"

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Worker thread for SQL database copy
   *
   * This class is used by DatabaseCopyDialog,
   *  witch should be used by applications.
   */
  class DatabaseCopyThread : public CopyThread
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    DatabaseCopyThread(QObject* parent = nullptr);

    // Copy disabled
    DatabaseCopyThread(const DatabaseCopyThread &) = delete;
    DatabaseCopyThread & operator=(const DatabaseCopyThread &) = delete;

    // move disabled
    DatabaseCopyThread(const DatabaseCopyThread &&) = delete;
    DatabaseCopyThread & operator=(DatabaseCopyThread &&) = delete;

    /*! \brief Get table mapping count
     */
    int tableMappingCount() const override
    {
      return pvMapping.tableMappingCount();
    }

    /*! \brief Copy tables of given database mapping
     *
     * Will copy data of source tables to destination tables regarding given mapping.
     *
     * Databases objects contained in mapping will be used to get needed informations
     *  (such as driver name, database name, etc...).
     *  Internally, new connections are created (this is required because of multi-threading).
     *
     * \warning It is recommanded that no connection refers to one of the
     *           databases on witch the schema will be copied.
     *           This check is not done here, and paying no attention to
     *           this case can conduct to undefined behaviour.
     * \note If mapping's source and destination database objects refers to the same database,
     *        only one connection will be created for the copy.
     *        In other words, this is not a problem.
     *
     * \pre Thread must be stopped before calling this function
     */
    void startCopy(const DatabaseMapping & mapping);

   private:

    /*! \brief Calculate table size for given table mapping
     */
    int64_t calculateSourceTableSize(const DatabaseCopierTableMapping * const tm, const QSqlDatabase & sourceDatabase) const;

    /*! \brief Calculate size of each table in source database
     */
    void calculateTableSizes(const QSqlDatabase & sourceDatabase);

    /*! \brief Copy source table to destination table regarding table mapping
     */
    bool copyTable(const DatabaseCopierTableMapping * const tm, int index,
                   const QSqlDatabase & sourceDatabase, const QSqlDatabase & destinationDatabase);

    /*! \brief Thread function implementation
     */
    void run();

    DatabaseMapping pvMapping;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_DATABASE_COPY_THREAD
