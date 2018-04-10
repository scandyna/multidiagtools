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
#ifndef MDT_SQL_JOIN_CLAUSE_ITEM_H
#define MDT_SQL_JOIN_CLAUSE_ITEM_H

#include "JoinOperator.h"
#include "SelectTable.h"
#include "JoinConstraint.h"

namespace Mdt{ namespace Sql{

  /*! \brief Item of a JoinClause
   */
  class JoinClauseItem
  {
  public:

    /*! \brief Construct a item to join table on constraint
     *
     * \pre table must not be null
     * \pre constraint must not be null
     */
    JoinClauseItem(JoinOperator op, const SelectTable & table, const JoinConstraintExpression & constraint)
     : mOperator(op) ,
       mTable(table) ,
       mConstraint(constraint)
    {
      Q_ASSERT(!mTable.isNull());
      Q_ASSERT(!mConstraint.isNull());
    }

    /*! \brief Construct a item to join table on constraint linking constraintOnTable
     *
     * \pre table must not be null
     * \pre constraintOnTable must not be null
     * \pre both table and constraintOnTable must have foreign keys
     */
    JoinClauseItem(JoinOperator op, const SelectTable & table, const SelectTable & constraintOnTable)
     : mOperator(op) ,
       mTable(table) ,
       mConstraint(table, constraintOnTable)
    {
      Q_ASSERT(!mTable.isNull());
      Q_ASSERT(!mConstraint.isNull());
    }

    /*! \brief Get join operator
     */
    JoinOperator joinOperator() const
    {
      return mOperator;
    }

    /*! \brief Get table to join name
     */
    QString tableName() const
    {
      return mTable.tableName();
    }

    /*! \brief Get table to join alias
     */
    QString tableAlias() const
    {
      return mTable.alias();
    }

    /*! \internal Access constraint
     *
     * Used by transforms
     */
    const JoinConstraint & constraint() const
    {
      return mConstraint;
    }

  private:

    JoinOperator mOperator;
    SelectTable mTable;
    JoinConstraint mConstraint;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_JOIN_CLAUSE_ITEM_H
