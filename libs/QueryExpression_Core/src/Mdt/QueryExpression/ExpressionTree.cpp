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
#include "ExpressionTree.h"
#include <boost/proto/traits.hpp>

namespace Mdt{ namespace QueryExpression{

ExpressionTreeVertex ExpressionTree::addNode(const SelectField & left, ComparisonOperator op, const QVariant& right)
{
  return addNode( boost::proto::value(left), op, right );
}

// ExpressionTreeVertex ExpressionTree::addNode(const SelectFieldVariant & left, ComparisonOperator op, const QVariant & right)
// {
//   const auto lv = addVertex(left);
//   const auto rv = addVertex(right);
//   const auto v = addVertex(op);
// 
//   mRootVertex = v;
// 
//   addEdge(v, lv);
//   addEdge(v, rv);
// 
//   return v;
// }

ExpressionTreeVertex ExpressionTree::addNode(ExpressionTreeVertex leftChild, LogicalOperator op, ExpressionTreeVertex rightChild)
{
  const auto v = addVertex(op);

  mRootVertex = v;

  addEdge(v, leftChild);
  addEdge(v, rightChild);

  return v;
}

void ExpressionTree::clear()
{
  mGraph.clear();
  mRootVertex = 0;
}

bool ExpressionTree::isNull() const noexcept
{
  return boost::num_vertices(mGraph) == 0;
}

ExpressionTreeVertex ExpressionTree::addVertex(ComparisonOperator op)
{
  return boost::add_vertex(ExpressionTreeVertexData(op), mGraph);
}

ExpressionTreeVertex ExpressionTree::addVertex(LogicalOperator op)
{
  return boost::add_vertex(ExpressionTreeVertexData(op), mGraph);
}

ExpressionTreeVertex ExpressionTree::addVertex(const SelectFieldVariant & field)
{
  return boost::add_vertex(ExpressionTreeVertexData(field), mGraph);
}

ExpressionTreeVertex ExpressionTree::addVertex(const QVariant & value)
{
  return boost::add_vertex(ExpressionTreeVertexData(value), mGraph);
}

ExpressionTreeVertex ExpressionTree::addVertex(const LikeExpressionData& data)
{
  return boost::add_vertex(ExpressionTreeVertexData(data), mGraph);
}

void ExpressionTree::addEdge(ExpressionTreeVertex left, ExpressionTreeVertex right)
{
  boost::add_edge(left, right, mGraph);
}

}} // namespace Mdt{ namespace QueryExpression{
