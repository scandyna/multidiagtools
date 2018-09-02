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
#ifndef MDT_QUERY_EXPRESSION_QUERY_SELECT_STATEMENT_H
#define MDT_QUERY_EXPRESSION_QUERY_SELECT_STATEMENT_H

#include "SelectStatement.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QMetaType>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Holds a SelectStatement and other attributes
   *
   * QuerySelectStatement is used to create backend specific
   *  implementation of AbstractSelectQuery or AbstractAsyncSelectQuery .
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT QuerySelectStatement
  {
   public:

    /*! \brief Construct a empty query select statement
     */
    QuerySelectStatement()
     : mMaxRows(0)
    {
    }

    /*! \brief Construct a query select statement
     *
     * \pre \a maxRows must be >= 0
     */
    QuerySelectStatement(const SelectStatement & statement, int maxRows)
     : mStatement(statement),
       mMaxRows(maxRows)
    {
      Q_ASSERT(mMaxRows >= 0);
    }

    /*! \brief Copy construct a query select statement from \a other
     */
    QuerySelectStatement(const QuerySelectStatement & other) = default;

    /*! \brief Copy assign \a other to this query select statement
     */
    QuerySelectStatement & operator=(const QuerySelectStatement & other) = default;

    /*! \brief Move construct a query select statement from \a other
     */
    QuerySelectStatement(QuerySelectStatement && other) = default;

    /*! \brief Move assign \a other to this query select statement
     */
    QuerySelectStatement & operator=(QuerySelectStatement && other) = default;

    /*! \brief Access the select statement
     */
    const SelectStatement & statement() const
    {
      return mStatement;
    }

    /*! \brief Get max rows
     */
    int maxRows() const
    {
      return mMaxRows;
    }

   private:

    SelectStatement mStatement;
    int mMaxRows;
  };

}} // namespace Mdt{ namespace QueryExpression{
Q_DECLARE_METATYPE(Mdt::QueryExpression::QuerySelectStatement)

#endif // #ifndef MDT_QUERY_EXPRESSION_QUERY_SELECT_STATEMENT_H
