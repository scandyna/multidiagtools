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
#ifndef MDT_QUERY_EXPRESSION_TYPE_TRAITS_GRAPH_H
#define MDT_QUERY_EXPRESSION_TYPE_TRAITS_GRAPH_H

#include <boost/graph/graph_traits.hpp>
#include <type_traits>

namespace Mdt{ namespace QueryExpression{ namespace TypeTraits{

  /*! \brief Check if Graph is a directed graph
   */
  template<typename Graph>
  constexpr bool isDirectedGraph()
  {
    return std::is_same< typename boost::graph_traits<Graph>::directed_category, boost::directed_tag >::value;
  }

}}} // namespace Mdt{ namespace QueryExpression{ namespace TypeTraits{

#endif // #ifndef MDT_QUERY_EXPRESSION_TYPE_TRAITS_GRAPH_H
