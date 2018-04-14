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
#ifndef MDT_SQL_JOIN_CLAUSE_H
#define MDT_SQL_JOIN_CLAUSE_H

#include "JoinOperator.h"
#include "SelectTable.h"
#include "JoinClauseItemList.h"

namespace Mdt{ namespace Sql{

  class JoinConstraintExpression;

  /*! \brief Join clause of a SelectStatement
   */
  class JoinClause
  {
   public:

    /*! \brief Construct a join clause
     *
     * \pre fromTable must not be null
     */
    JoinClause(const SelectTable & fromTable);

    /*! \brief Join a table
     *
     * Join table with expr as constraint
     *
     * \pre table must not be null
     * \pre expr must not be null
     */
    void joinTableOn(JoinOperator op, const SelectTable & table, const JoinConstraintExpression & expr);

    /*! \brief Join a table automatically
     *
     * Will fetch table's foreign key list
     *  and fromTable's foreign key list about relation
     *  and also generate the apropriate join constraint.
     *
     * \pre table must not be null
     * \pre table or fromTable must have a relation defined by their foreign key
     */
    void joinTableOn(JoinOperator op, const SelectTable & table);

    /*! \brief Join a table automatically
     *
     * Will fetch table's foreign key list
     *  and constraintOnTable's foreign key list about relation
     *  and also generate the apropriate join constraint.
     *
     * \pre table must not be null
     * \pre constraintOnTable must not be null
     * \pre table or constraintOnTable must have a relation defined by their foreign key
     */
    void joinTableOn(JoinOperator op, const SelectTable & table, const SelectTable & constraintOnTable);

    /*! \brief Get from table name
     */
    QString tableName() const
    {
      return mFromTable.tableName();
    }

    /*! \brief Get from table alias
     */
    QString tableAlias() const
    {
      return mFromTable.alias();
    }

    /*! \brief Check if this join clause is null
     *
     * Join clause is null if fromTable is null.
     */
    bool isNull() const
    {
      return mFromTable.isNull();
    }

    /*! \brief Check if this clause has at least 1 joined table
     */
    bool hasJoinedTable() const
    {
      return !mItemList.isEmpty();
    }

    /*! \internal Access from table
     *
     * Used for unit tests.
     */
    const SelectTable & fromTable() const
    {
      return mFromTable;
    }

    /*! \internal Access list of join clause items
     *
     * Used by transforms and unit tests.
     */
    const JoinClauseItemList & itemList() const
    {
      return mItemList;
    }

   private:

    SelectTable mFromTable;
    JoinClauseItemList mItemList;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_JOIN_CLAUSE_H
