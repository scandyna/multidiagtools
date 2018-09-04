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
#ifndef MDT_QUERY_EXPRESSION_SQL_ASYNC_SELECT_QUERY_FACTORY_H
#define MDT_QUERY_EXPRESSION_SQL_ASYNC_SELECT_QUERY_FACTORY_H

#include "Mdt/QueryExpression/AbstractAsyncSelectQueryFactory.h"
#include "Mdt/Sql/AsyncQueryConnection.h"
#include "Mdt/Sql/ConnectionParameters.h"
#include "MdtQueryExpression_SqlExport.h"

namespace Mdt{ namespace QueryExpression{

  /*! \brief SQL implementation of a asynchronous select query factory
   */
  class MDT_QUERYEXPRESSION_SQL_EXPORT SqlAsyncSelectQueryFactory : public AbstractAsyncSelectQueryFactory
  {
   public:

    /*! \brief Setup this query factory
     *
     * \note If setup was previously done,
     *   the queries that are currently running will be aborted.
     */
    bool setup(const Mdt::Sql::ConnectionParameters & parameters);

    /*! \brief Create a asynchronous select query
     */
    std::unique_ptr<AbstractAsyncSelectQuery> createSelectQuery();

   private:

    Mdt::Sql::AsyncQueryConnection mConnection;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SQL_ASYNC_SELECT_QUERY_FACTORY_H
