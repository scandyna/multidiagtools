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
#ifndef MDT_SQL_ABSTRACT_ASYNC_QUERY_THREAD_WORKER_H
#define MDT_SQL_ABSTRACT_ASYNC_QUERY_THREAD_WORKER_H

#include "AsyncQueryOperationType.h"
#include "Connection.h"
#include "InsertStatement.h"
#include "UpdateStatement.h"
#include "DeleteStatement.h"
#include "Mdt/QueryExpression/SelectStatement.h"
#include "Mdt/Container/VariantRecord.h"
#include "Mdt/Error.h"
#include "Mdt/Expected.h"
#include "MdtSql_CoreExport.h"
#include <QObject>
#include <QString>
#include <memory>

namespace Mdt{ namespace Sql{

  class SelectQuery;

  /*! \brief Base class to implement a async query thread worker
   */
  class MDT_SQL_CORE_EXPORT AbstractAsyncQueryThreadWorker : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractAsyncQueryThreadWorker();

    /*! \brief Close the database handle and remove the connection
     */
    ~AbstractAsyncQueryThreadWorker();

    AbstractAsyncQueryThreadWorker(const AbstractAsyncQueryThreadWorker &) = delete;
    AbstractAsyncQueryThreadWorker & operator=(const AbstractAsyncQueryThreadWorker &) = delete;
    AbstractAsyncQueryThreadWorker(AbstractAsyncQueryThreadWorker &&) = delete;
    AbstractAsyncQueryThreadWorker & operator=(AbstractAsyncQueryThreadWorker &&) = delete;

    /*! \brief Get the database connection this worker is using
     *
     * \pre Get a connection is only allowed once it have been set
     */
    Connection connection() const
    {
      Q_ASSERT(!mConnectionName.isEmpty());

      return Connection(mConnectionName);
    }

    /*! \brief Check if the database handle is open
     *
     * Returns true if a connection was set and the database handle it refers to is open,
     *  otherwise false.
     */
    bool isDatabaseOpen() const
    {
      if(mConnectionName.isEmpty()){
        return false;
      }
      return connection().database().isOpen();
    }

    /*! \brief Close database connection handle and remove the connection
     */
    void close();

   public Q_SLOTS:

    /*! \brief Process a insert statement
     */
    void processInsertStatement(const Mdt::Sql::InsertStatement & statement, int instanceId);

    /*! \brief Process a select statement
     */
    void processSelectStatement(const Mdt::QueryExpression::SelectStatement & statement, int instanceId, bool fetchRecords);

    /*! \brief Process fetch next record
     */
    void processSelectQueryFetchNext(int instanceId);

    /*! \brief Process a update statement
     */
    void processUpdateStatement(const Mdt::Sql::UpdateStatement & statement, int instanceId);

    /*! \brief Process a delete statement
     */
    void processDeleteStatement(const Mdt::Sql::DeleteStatement & statement, int instanceId);

   Q_SIGNALS:

    /*! \brief Emitted once the database has been open successfully
     */
    void openDone();

    /*! \brief Emitted when a open error occured
     */
    void openErrorOccured(const Mdt::Error & error);

    /*! \brief Emitted when a new ID have been inserted
     */
    void newIdInserted(const QVariant & id, int instanceId);

    /*! \brief Emitted whenever a new record is available
     */
    void newRecordAvailable(const Mdt::Container::VariantRecord & record, int instanceId);

    /*! \brief Emitted whenever fetching a new record is done
     */
    void selectQueryFetchNextDone(bool result, int instanceId);

    /*! \brief Emitted whenever a query operation is done
     */
    void queryOperationDone(Mdt::Sql::AsyncQueryOperationType operationType, int instanceId);

    /*! \brief Emitted whenever a query error occured
     */
    void queryErrorOccured(const Mdt::Error & error, int instanceId);

   protected:

    /*! \brief Set the database connection this worker must use
     */
    void setConnection(const Connection & connection);

   private:

    void initSelectQueryIfNot();

    QString mConnectionName;
    std::unique_ptr<SelectQuery> mSelectQuery;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ABSTRACT_ASYNC_QUERY_THREAD_WORKER_H
