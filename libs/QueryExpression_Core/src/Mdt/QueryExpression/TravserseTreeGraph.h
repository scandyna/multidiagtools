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
#ifndef MDT_QUERY_EXPRESSION_TRAVERSE_TREE_GRAPH_H
#define MDT_QUERY_EXPRESSION_TRAVERSE_TREE_GRAPH_H

#include "ExpressionTree.h"
#include "AbstractExpressionTreeVisitor.h"
#include "TypeTraits/Graph.h"
#include "MdtQueryExpression_CoreExport.h"

namespace Mdt{ namespace QueryExpression{

  /*! \brief Traverse a graph as a tree
   *
   * This function takes the proposal discussed here:
   *  http://boost-users.boost.narkive.com/sN4cBmiE/bgl-traversal-algorithm-for-tree
   *
   * \sa traverseExpressionTree()
   */
  template<typename TreeGraph, typename TreeGraphVisitor>
  void traverseTreeGraph(const TreeGraph & g, typename boost::graph_traits<TreeGraph>::vertex_descriptor v, TreeGraphVisitor & visitor)
  {
    static_assert( TypeTraits::isDirectedGraph<TreeGraph>()  , "traverseTreeGraph() requires a directed graph (else it will run to infinite loop)" );

    visitor.preorder(v, g);

    typename boost::graph_traits<TreeGraph>::adjacency_iterator it, end;
    std::tie(it, end) = boost::adjacent_vertices(v, g);
    if(it != end){
      // Current node is not a leaf
      visitor.onEdge( boost::edge(v, *it, g).first, g );
      traverseTreeGraph(g, *it, visitor);
      visitor.postEdge( boost::edge(v, *it++, g).first, g );
      while(it != end){
        visitor.inorder(v, g);
        visitor.onEdge( boost::edge(v, *it, g).first, g );
        traverseTreeGraph(g, *it, visitor);
        visitor.postEdge( boost::edge(v, *it++, g).first, g );
      }
    }else{
      visitor.inorder(v, g);
      visitor.postorder(v, g);
    }
  }

  /*! \brief Traverse a expression tree with a visitor
   */
  void MDT_QUERYEXPRESSION_CORE_EXPORT traverseExpressionTree(const ExpressionTree & tree, AbstractExpressionTreeVisitor & visitor)
  {
    traverseTreeGraph(tree.internalGraph(), tree.internalRootVertex(), visitor);
  }

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_TRAVERSE_TREE_GRAPH_H
