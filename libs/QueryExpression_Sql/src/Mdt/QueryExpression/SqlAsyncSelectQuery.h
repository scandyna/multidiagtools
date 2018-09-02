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
#ifndef MDT_QUERY_EXPRESSION_SQL_ASYNC_SELECT_QUERY_H
#define MDT_QUERY_EXPRESSION_SQL_ASYNC_SELECT_QUERY_H

#include "Mdt/QueryExpression/AbstractAsyncSelectQuery.h"
#include "Mdt/Sql/AsyncQuery.h"
#include "MdtQueryExpression_SqlExport.h"
#include <QObject>
#include <memory>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Execute a select statement to fetch data from a SQL database asynchronously
   */
  class MDT_QUERYEXPRESSION_SQL_EXPORT SqlAsyncSelectQuery : public AbstractAsyncSelectQuery
  {
    using BaseClass = AbstractAsyncSelectQuery;

   Q_OBJECT

   public:

    /*! \brief Construct a select query
     */
    SqlAsyncSelectQuery(std::unique_ptr<Mdt::Sql::AsyncQuery> && query, QObject *parent = nullptr);

    /*! \brief Submit a select statement
     *
     * \pre \a maxRows must be >= 0
     */
    void submitStatement(const SelectStatement & statement, int maxRows = 0) override;

   private:

    std::unique_ptr<Mdt::Sql::AsyncQuery> mQuery;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SQL_ASYNC_SELECT_QUERY_H
