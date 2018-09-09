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
#ifndef MDT_QUERY_EXPRESSION_BUILD_EXPRESSION_TREE_ADD_EXPRESSION_TO_TREE_H
#define MDT_QUERY_EXPRESSION_BUILD_EXPRESSION_TREE_ADD_EXPRESSION_TO_TREE_H

#include "../ExpressionTree.h"
#include "../SelectField.h"
#include "../LikeExpression.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QVariant>
#include <boost/proto/traits.hpp>

namespace Mdt{ namespace QueryExpression{ namespace BuildExpressionTree{

  struct MDT_QUERYEXPRESSION_CORE_EXPORT AddComparisonExpressionToTreeBase : boost::proto::callable
  {
    using result_type = ExpressionTreeVertex;

    ExpressionTreeVertex addNode(const SelectFieldVariant & left, ComparisonOperator op, const QVariant & right, ExpressionTree & tree) const
    {
      return tree.addNode(left, op, right);
    }

    ExpressionTreeVertex addNode(const SelectFieldVariant & left, ComparisonOperator op, const LikeExpressionData & right, ExpressionTree & tree) const
    {
      return tree.addNode(left, op, right);
    }

    ExpressionTreeVertex addNode(const SelectFieldVariant & left, ComparisonOperator op, const SelectFieldVariant & right, ExpressionTree & tree) const
    {
      return tree.addNode(left, op, right);
    }
  };

  struct MDT_QUERYEXPRESSION_CORE_EXPORT AddEqualExpressionToTree : AddComparisonExpressionToTreeBase
  {
    template<typename L, typename R>
    ExpressionTreeVertex operator()(const L & left, const R & right, ExpressionTree & tree) const
    {
      return addNode(left, ComparisonOperator::Equal, right, tree);
    }
  };

  struct MDT_QUERYEXPRESSION_CORE_EXPORT AddLikeExpressionToTree : AddComparisonExpressionToTreeBase
  {
    template<typename L, typename R>
    ExpressionTreeVertex operator()(const L & left, const R & right, ExpressionTree & tree) const
    {
      return addNode(left, ComparisonOperator::Like, right, tree);
    }
  };

  struct MDT_QUERYEXPRESSION_CORE_EXPORT AddNotEqualExpressionToTree : AddComparisonExpressionToTreeBase
  {
    template<typename L, typename R>
    ExpressionTreeVertex operator()(const L & left, const R & right, ExpressionTree & tree) const
    {
      return addNode(left, ComparisonOperator::NotEqual, right, tree);
    }
  };

  struct MDT_QUERYEXPRESSION_CORE_EXPORT AddLessExpressionToTree : AddComparisonExpressionToTreeBase
  {
    template<typename L, typename R>
    ExpressionTreeVertex operator()(const L & left, const R & right, ExpressionTree & tree) const
    {
      return addNode(left, ComparisonOperator::Less, right, tree);
    }
  };

  struct MDT_QUERYEXPRESSION_CORE_EXPORT AddLessEqualExpressionToTree : AddComparisonExpressionToTreeBase
  {
    template<typename L, typename R>
    ExpressionTreeVertex operator()(const L & left, const R & right, ExpressionTree & tree) const
    {
      return addNode(left, ComparisonOperator::LessEqual, right, tree);
    }
  };

  struct MDT_QUERYEXPRESSION_CORE_EXPORT AddGreaterExpressionToTree : AddComparisonExpressionToTreeBase
  {
    template<typename L, typename R>
    ExpressionTreeVertex operator()(const L & left, const R & right, ExpressionTree & tree) const
    {
      return addNode(left, ComparisonOperator::Greater, right, tree);
    }
  };

  struct MDT_QUERYEXPRESSION_CORE_EXPORT AddGreaterEqualExpressionToTree : AddComparisonExpressionToTreeBase
  {
    template<typename L, typename R>
    ExpressionTreeVertex operator()(const L & left, const R & right, ExpressionTree & tree) const
    {
      return addNode(left, ComparisonOperator::GreaterEqual, right, tree);
    }
  };

  struct MDT_QUERYEXPRESSION_CORE_EXPORT AddLogicalOrExpressionToTree : boost::proto::callable
  {
    using result_type = ExpressionTreeVertex;

    ExpressionTreeVertex operator()(ExpressionTreeVertex leftChild, ExpressionTreeVertex rightChild, ExpressionTree & tree) const
    {
      return tree.addNode(leftChild, LogicalOperator::Or, rightChild);
    }
  };

  struct MDT_QUERYEXPRESSION_CORE_EXPORT AddLogicalAndExpressionToTree : boost::proto::callable
  {
    using result_type = ExpressionTreeVertex;

    ExpressionTreeVertex operator()(ExpressionTreeVertex leftChild, ExpressionTreeVertex rightChild, ExpressionTree & tree) const
    {
      return tree.addNode(leftChild, LogicalOperator::And, rightChild);
    }
  };

}}} // namespace Mdt{ namespace QueryExpression{ namespace BuildExpressionTree{

#endif // #ifndef MDT_QUERY_EXPRESSION_BUILD_EXPRESSION_TREE_ADD_EXPRESSION_TO_TREE_H
