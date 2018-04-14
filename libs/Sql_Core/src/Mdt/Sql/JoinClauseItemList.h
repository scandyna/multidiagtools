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
#ifndef MDT_SQL_JOIN_CLAUSE_ITEM_LIST_H
#define MDT_SQL_JOIN_CLAUSE_ITEM_LIST_H

#include "JoinClauseItem.h"
#include <vector>

namespace Mdt{ namespace Sql{

  /*! \brief List of JoinClauseItem
   */
  class JoinClauseItemList
  {
   public:

    /*! \brief Append a item to join table on constraint
     *
     * \pre table must not be null
     * \pre constraint must not be null
     */
    void append(JoinOperator op, const SelectTable & table, const JoinConstraintExpression & constraint)
    {
      mList.emplace_back(op, table, constraint);
    }

    /*! \brief Append a item to join table on constraint linking constraintOnTable
     *
     * \pre table must not be null
     * \pre constraintOnTable must not be null
     * \pre both table and constraintOnTable must have foreign keys
     */
    void append(JoinOperator op, const SelectTable & table, const SelectTable & constraintOnTable)
    {
      mList.emplace_back(op, table, constraintOnTable);
    }

    /*! \brief Get item at index
     *
     * \pre index must be in valid range
     */
    const JoinClauseItem & at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());
      return mList[index];
    }

    /*! \brief Get count of items
     */
    int size() const
    {
      return mList.size();
    }

    /*! \brief Check if list is empty
     */
    bool isEmpty() const
    {
      return mList.empty();
    }

   private:

    std::vector<JoinClauseItem> mList;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_JOIN_CLAUSE_ITEM_LIST_H
