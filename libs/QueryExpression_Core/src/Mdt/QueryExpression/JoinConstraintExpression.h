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
#ifndef MDT_QUERY_EXPRESSION_JOIN_CONSTRAINT_EXPRESSION_H
#define MDT_QUERY_EXPRESSION_JOIN_CONSTRAINT_EXPRESSION_H

#include "ExpressionTree.h"
#include "BuildExpressionTree/Transform.h"
#include "TypeTraits/JoinConstraintExpression.h"
#include "MdtQueryExpression_CoreExport.h"

namespace Mdt{ namespace QueryExpression{

  /*! \brief Join constraint expression
   *
   * This class should not be used in application code.
   *
   * \sa SelectStatement
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT JoinConstraintExpression
  {
   public:

    /*! \brief Construct a null join constraint expression
     */
    JoinConstraintExpression() = default;

    /*! \brief Copy construct a join constraint expression from \a other
     */
    JoinConstraintExpression(const JoinConstraintExpression & other) = default;

    /*! \brief Copy assign \a other to this join constraint expression
     */
    JoinConstraintExpression & operator=(const JoinConstraintExpression & other) = default;

    /*! \brief Move construct a join constraint expression from \a other
     */
    JoinConstraintExpression(JoinConstraintExpression && other) = default;

    /*! \brief Move assign \a other to this join constraint expression
     */
    JoinConstraintExpression & operator=(JoinConstraintExpression && other) = default;

    /*! \brief Set \a join to this expression
     */
    template<typename JoinExpr>
    void setJoin(const JoinExpr & join)
    {
      static_assert(TypeTraits::isJoinConstraintExpression<JoinExpr>() , "join is not a valid join constraint expression");

      mTree.clear();
      BuildExpressionTree::LogicalTransform transform;
      transform(join, 0, mTree);
    }

    /*! \brief Check if this expression is null
     */
    bool isNull() const noexcept
    {
      return mTree.isNull();
    }

    /*! \brief Clear this join constraint expression
     */
    void clear();

    /*! \brief
     *
     * \pre \a exprTree must not be null
     */
    void setJoinConstraintExpressionTree(const ExpressionTree & exprTree)
    {
      Q_ASSERT(!exprTree.isNull());

      mTree = exprTree;
    }

    /*! \brief Access the internal tree of this join constraint expression
     *
     * This is typically used to transform this expression
     *  (for example to a SQL string)
     */
    const ExpressionTree & internalTree() const
    {
      return mTree;
    }

   private:

    ExpressionTree mTree;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_JOIN_CONSTRAINT_EXPRESSION_H
