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
#ifndef MDT_QUERY_EXPRESSION_EXPRESSION_TREE_H
#define MDT_QUERY_EXPRESSION_EXPRESSION_TREE_H

// #include "QueryFieldVariant.h"

#include "EntityAndField.h"
#include "ComparisonOperator.h"
#include "LogicalOperator.h"
#include "ExpressionTreeGraph.h"
#include "LikeExpression.h"
#include "MdtQueryExpression_CoreExport.h"

namespace Mdt{ namespace QueryExpression{

  /*! \brief Contains a runtime expression
   *
   * The following examples assume using the QueryExpression namespace:
   * \code
   * using namespace Mdt::QueryExpression
   * \endcode
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT ExpressionTree
  {
   public:

    /*! \brief Create a empty expression
     */
    ExpressionTree() = default;

    /*! \brief Copy construct \a other to this expression
     */
    ExpressionTree(const ExpressionTree & other) = default;

    /*! \brief Copy assign \a other to this expression
     */
    ExpressionTree & operator=(const ExpressionTree & other) = default;

    /*! \brief Move construct \a other to this expression
     */
    ExpressionTree(ExpressionTree && other) = default;

    /*! \brief Move assign \a other to this expression
     */
    ExpressionTree & operator=(ExpressionTree && other) = default;

    /*! \brief Add a node for \a left field, \a op operation and \a right value
     */
    ExpressionTreeVertex addNode(const EntityAndField & left, ComparisonOperator op, const QVariant & right)
    {
      return addComparisonNode(left, op, right);
    }

    /*! \brief Add a node for \a left field, \a op operation and \a right like expression
     */
    ExpressionTreeVertex addNode(const EntityAndField & left, ComparisonOperator op, const LikeExpressionData & right)
    {
      return addComparisonNode(left, op, right);
    }

    /*! \brief Add a node for \a left field, \a op operation and \a right field
     */
    ExpressionTreeVertex addNode(const EntityAndField & left, ComparisonOperator op, const EntityAndField & right)
    {
      return addComparisonNode(left, op, right);
    }

    /*! \brief Add a \a op operation node with \a leftChild and \a rightChild
     */
    ExpressionTreeVertex addNode(ExpressionTreeVertex leftChild, LogicalOperator op, ExpressionTreeVertex rightChild);

    /*! \brief Clear this expression
     */
    void clear();

    /*! \brief Check if this expression tree is null
     */
    bool isNull() const noexcept;

    /*! \internal Access the internal graph of this expression tree
     *
     * Used by transforms and unit tests
     */
    const ExpressionTreeGraph & internalGraph() const
    {
      return mGraph;
    }

    /*! \internal Get the root vertex of the interna graph of this expression tree
     *
     * Used by transforms and unit tests
     */
    ExpressionTreeVertex internalRootVertex() const
    {
      return mRootVertex;
    }

   private:

    template<typename L, typename R>
    ExpressionTreeVertex addComparisonNode(const L & left, ComparisonOperator op, const R & right)
    {
      const auto lv = addVertex(left);
      const auto rv = addVertex(right);
      const auto v = addVertex(op);

      mRootVertex = v;

      addEdge(v, lv);
      addEdge(v, rv);

      return v;
    }

    ExpressionTreeVertex addVertex(ComparisonOperator op);
    ExpressionTreeVertex addVertex(LogicalOperator op);
    ExpressionTreeVertex addVertex(const EntityAndField & field);
    ExpressionTreeVertex addVertex(const QVariant & value);
    ExpressionTreeVertex addVertex(const LikeExpressionData & data);
    void addEdge(ExpressionTreeVertex left, ExpressionTreeVertex right);

    ExpressionTreeVertex mRootVertex;
    ExpressionTreeGraph mGraph;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_EXPRESSION_TREE_H
