/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_IMPL_LIBRARY_TREE_GRAPH_H
#define MDT_DEPLOY_UTILS_IMPL_LIBRARY_TREE_GRAPH_H

#include "Vertex.h"
#include <boost/graph/adjacency_list.hpp>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace LibraryTree{

  using Graph = boost::adjacency_list<
    boost::hash_setS,     // OutEdgeList: hash_setS (boost::unordered_set): choosed to avoid adding a edge more than once
    boost::vecS,          // VertexList: vecS (std::vector)
    boost::directedS,     // Directed: undirectedS (directed graph)
    VertexData,           // VertexProperties: we use our custom struct
    boost::no_property,   // EdgeProperties: we use no property
    boost::no_property,   // GraphProperties: no_property (None)
    boost::listS          // EdgeList: listS (std::list)
  >;

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace LibraryTree{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_LIBRARY_TREE_GRAPH_H
