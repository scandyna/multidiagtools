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
#ifndef MDT_DEPLOY_UTILS_IMPL_LIBRARY_TREE_LABELED_GRAPH_H
#define MDT_DEPLOY_UTILS_IMPL_LIBRARY_TREE_LABELED_GRAPH_H

#include "Graph.h"
#include <boost/graph/labeled_graph.hpp>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace LibraryTree{

  using LabeledGraph = boost::labeled_graph<Graph, int>;

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace LibraryTree{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_LIBRARY_TREE_LABELED_GRAPH_H
