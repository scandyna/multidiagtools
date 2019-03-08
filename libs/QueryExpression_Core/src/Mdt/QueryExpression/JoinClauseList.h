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
#ifndef MDT_QUERY_EXPRESSION_JOIN_CLAUSE_LIST_H
#define MDT_QUERY_EXPRESSION_JOIN_CLAUSE_LIST_H

#include "JoinClause.h"
#include "JoinOperator.h"
#include "QueryEntity.h"
#include "JoinConstraintExpression.h"
#include "MdtQueryExpression_CoreExport.h"
#include <vector>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Used by SelectStatement to hold a list of entities to join
   *
   * This class should not be used in application code.
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT JoinClauseList
  {
   public:

    /*! \brief STL const iterator
     */
    using const_iterator = std::vector<JoinClause>::const_iterator;

    /*! \brief Add a join clause
     *
     * \pre \a entity must not be null
     * \pre \a join must not be null
     */
    void addClause(JoinOperator joinOperator, const QueryEntity & entity, const JoinConstraintExpression & join);

    /*! \brief Get count of clauses
     */
    int clauseCount() const noexcept
    {
      return mList.size();
    }

    /*! \brief Check if this list is empty
     */
    bool isEmpty() const noexcept
    {
      return mList.empty();
    }

    /*! \brief Access the join clause at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < clauseCount() )
     */
    const JoinClause & clauseAt(int index) const noexcept
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < clauseCount());

      return mList[index];
    }

    /*! \brief Clear this list
     */
    void clear();

//     /*! \brief Get a iterator to the first clause thet refers to \a entity
//      */
//     const_iterator findEntity(const QueryEntity & entity) const;

    /*! \brief Get a STL iterator to the beginning of this list
     */
    const_iterator begin() const
    {
      return mList.cbegin();
    }

    /*! \brief Get a STL const iterator past the end of this list
     */
    const_iterator end() const
    {
      return mList.cend();
    }

   private:

    std::vector<JoinClause> mList;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_JOIN_CLAUSE_LIST_H
