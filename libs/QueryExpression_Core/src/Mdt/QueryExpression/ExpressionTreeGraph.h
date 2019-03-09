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
#ifndef MDT_QUERY_EXPRESSION_EXPRESSION_TREE_GRAPH_H
#define MDT_QUERY_EXPRESSION_EXPRESSION_TREE_GRAPH_H

#include "EntityAndField.h"
#include "LogicalOperator.h"
#include "ComparisonOperator.h"
#include "LikeExpression.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QVariant>
#include <boost/variant.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace Mdt{ namespace QueryExpression{

  /*! \internal Vertex data used in ExpressionTreeGraph
   */
  using ExpressionTreeVertexData = boost::variant<LogicalOperator, ComparisonOperator, QVariant, EntityAndField, LikeExpressionData>;

  /*! \internal Graph for ExpressionTree
   */
  using ExpressionTreeGraph = boost::adjacency_list<
    boost::vecS,              // OutEdgeList: vecS (std::vector)
    boost::vecS,              // VertexList: vecS (std::vector)
    boost::directedS,         // Directed: directedS (directed graph)
    ExpressionTreeVertexData, // VertexProperties: we use our custom struct
    boost::no_property,       // EdgeProperties: we use no property
    boost::no_property,       // GraphProperties: no_property (None)
    boost::listS              // EdgeList: listS (std::list)
  >;

  /*! \brief Vertex descriptor used in ExpressionTreeGraph and ExpressionTree
   */
  using ExpressionTreeVertex = boost::graph_traits<ExpressionTreeGraph>::vertex_descriptor;

  /*! \brief Edge descriptor used in ExpressionTreeGraph and ExpressionTree
   */
  using ExpressionTreeEdge = boost::graph_traits<ExpressionTreeGraph>::edge_descriptor;

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_EXPRESSION_TREE_GRAPH_H
