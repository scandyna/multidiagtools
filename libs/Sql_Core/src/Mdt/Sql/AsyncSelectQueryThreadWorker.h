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
#ifndef MDT_SQL_ASYNC_SELECT_QUERY_THREAD_WORKER_H
#define MDT_SQL_ASYNC_SELECT_QUERY_THREAD_WORKER_H

#include "Connection.h"
#include "AsyncQueryOperationType.h"
#include "AsyncSelectQueryRecordFetching.h"
#include "SelectQuery.h"
#include "Mdt/QueryExpression/SelectStatement.h"
#include "Mdt/Container/VariantRecord.h"
#include "Mdt/Error.h"
#include "MdtSql_CoreExport.h"
#include <QObject>
#include <map>

namespace Mdt{ namespace Sql{

  /*! \internal Helper class to implement AbstractAsyncQueryThreadWorker
   */
  class MDT_SQL_CORE_EXPORT AsyncSelectQueryThreadWorker : public QObject
  {
    Q_OBJECT

   public:

    explicit AsyncSelectQueryThreadWorker(const Connection & connection, QObject *parent = nullptr);

    /*! \brief Process a select statement
     */
    void processStatement(const Mdt::QueryExpression::SelectStatement & statement, AsyncSelectQueryRecordFetching recordFetching, int instanceId);

    /*! \brief Process fetch a single record
     */
    void processFetchSingleRecord(int instanceId);

    /*! \brief Process fetch next records
     */
    void processFetchNextRecords(int maxRecords,  int instanceId);

   Q_SIGNALS:

    /*! \brief Emitted whenever a new record is available
     */
    void newRecordAvailable(const Mdt::Container::VariantRecord & record, int instanceId);

    /*! \brief Emitted whenever a query operation is done
     */
    void operationDone(Mdt::Sql::AsyncQueryOperationType operationType, int instanceId);

    /*! \brief Emitted whenever a query error occured
     */
    void errorOccured(const Mdt::Error & error, int instanceId);

   private:

    void fetchSingleRecord(SelectQuery & query, int instanceId);
    void fetchAllRecords(SelectQuery & query, int instanceId);
    void notifyOperationPartiallyDone(int instanceId);
    void setQueryDone(int instanceId);
    bool contiansQuery(int instanceId) const;
    SelectQuery & getQuery(int instanceId);
    void initQueryIfNot(int instanceId);
    void removeQuery(int instanceId);

    Connection mConnection;
    std::map<int, SelectQuery> mQueryMap;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ASYNC_SELECT_QUERY_THREAD_WORKER_H
