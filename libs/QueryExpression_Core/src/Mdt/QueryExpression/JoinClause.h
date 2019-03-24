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
#ifndef MDT_QUERY_EXPRESSION_JOIN_CLAUSE_H
#define MDT_QUERY_EXPRESSION_JOIN_CLAUSE_H

#include "JoinOperator.h"
#include "QueryEntity.h"
#include "JoinConstraintExpression.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Representation of a join clause
   *
   * This class should not be used in application code.
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT JoinClause
  {
   public:

    /*! \brief Construct a join clause
     *
     * \pre \a entity must not be null
     * \pre \a join must not be null
     */
    JoinClause(JoinOperator op, const QueryEntity & entity, const JoinConstraintExpression & join)
     : mJoinOperator(op),
       mEntity(entity),
       mJoinConstraintExpression(join)
    {
      Q_ASSERT(!mEntity.isNull());
      Q_ASSERT(!mJoinConstraintExpression.isNull());
    }

    /*! \brief Copy construct a join clause from \a other
     */
    JoinClause(const JoinClause & other) = default;

    /*! \brief Copy assign \a other to this join clause
     */
    JoinClause & operator=(const JoinClause & other) = default;

    /*! \brief Move construct a join clause from \a other
     */
    JoinClause(JoinClause && other) = default;

    /*! \brief Move assign \a other to this join clause
     */
    JoinClause & operator=(JoinClause && other) = default;

    /*! \brief Get join operator
     */
    JoinOperator joinOperator() const noexcept
    {
      return mJoinOperator;
    }

    /*! \brief Get entity
     */
    const QueryEntity & entity() const
    {
      return mEntity;
    }

    /*! \brief Get entity alias or name
     */
    QString entityAliasOrName() const
    {
      return mEntity.aliasOrName();
    }

    /*! \brief Get join constraint expression
     */
    const JoinConstraintExpression & joinConstraintExpression() const noexcept
    {
      return mJoinConstraintExpression;
    }

   private:

    JoinOperator mJoinOperator;
    QueryEntity mEntity;
    JoinConstraintExpression mJoinConstraintExpression;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_JOIN_CLAUSE_H
