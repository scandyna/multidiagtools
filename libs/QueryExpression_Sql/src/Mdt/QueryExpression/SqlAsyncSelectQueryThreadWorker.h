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
#ifndef MDT_QUERY_EXPRESSION_SQL_ASYNC_SELECT_QUERY_THREAD_WORKER_H
#define MDT_QUERY_EXPRESSION_SQL_ASYNC_SELECT_QUERY_THREAD_WORKER_H

#include "Mdt/Sql/AsyncQueryThreadWorker.h"
#include "Mdt/QueryExpression/QuerySelectStatement.h"
#include "MdtQueryExpression_SqlExport.h"

namespace Mdt{ namespace QueryExpression{

  /*! \brief Process SQL select queries in a separate thread
   */
  class SqlAsyncSelectQueryThreadWorker : public Mdt::Sql::AsyncQueryThreadWorker
  {
   Q_OBJECT

    using BaseClass = AsyncQueryThreadWorker;

   public:

    /*! \brief Constructor
     */
    explicit SqlAsyncSelectQueryThreadWorker(QObject *parent = nullptr);

   public slots:

    /*! \brief Process a new query
     */
    void processQuery(const QVariant & query, int instanceId) override;

   private:

    void processSelectStatement(const QuerySelectStatement & statement, int instanceId);
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SQL_ASYNC_SELECT_QUERY_THREAD_WORKER_H
