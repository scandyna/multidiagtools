/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_CABLE_LIST_PATH_GRAPH_PRIVATE_GRAPH_H
#define MDT_CABLE_LIST_PATH_GRAPH_PRIVATE_GRAPH_H

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <boost/graph/properties.hpp>

#include <boost/property_map/property_map.hpp>
#include <cstdint>
#include <algorithm>

namespace Mdt{ namespace CableList{ namespace Path{ namespace GraphPrivate{

  /*
   * Connection
   *  Will be used as vertex property in graph
   */
  struct Connection
  {
    // Default constructor: required by boost::add_edge()
    constexpr Connection() noexcept
     : id(-1) {}

    explicit constexpr Connection(int64_t id) noexcept
     : id(id)
    {
      ///Q_ASSERT(id > 0);
    }

    int64_t id;
  };

  /*
   * Link
   * Will be used as edge property in graph
   */
  struct Link
  {
    explicit constexpr Link(Connection start, Connection end, int weigth, bool isComplement) noexcept
     : start(start), end(end), weigth(weigth), isComplement(isComplement) {}

    Connection start;
    Connection end;
    int weigth;
    bool isComplement;
  };

  ///typedef boost::property<boost::edge_weight_t, int> weigth_property_t;



  /*
   * Boost graph typedef
   */
  typedef boost::adjacency_list<
    boost::hash_setS,     // OutEdgeList: hash_setS (boost::unordered_set): choosed to avoid adding a edge more than once
    boost::vecS,          // VertexList: vecS (std::vector)
    boost::directedS,     // Directed: directedS (Directed graph)
    Connection,           // VertexProperties: we use our custom struct
    Link,                 // EdgeProperties: we use our custom struct
    boost::no_property,   // GraphProperties: no_property (None)
    boost::listS          // EdgeList: listS (std::list)
  > graph_t;

  /*
   * Vertex descriptor
   */
  typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_t;

  struct Vertex
  {
    constexpr Vertex() noexcept
     : v(0), null(true) {}

    explicit constexpr Vertex(vertex_t v) noexcept
     : v(v), null(false) {}

    constexpr bool isNull() const noexcept
    {
      return null;
    }

    vertex_t v;

   private:

    bool null;
  };

  /*
   * Edge descriptor
   */
  typedef boost::graph_traits<graph_t>::edge_descriptor Edge;

  /*
   * Graph implentation
   */
  struct GraphImpl
  {
    graph_t graph;

    /*
     * Add a vertex for connection.
     * If a vertex allready exists for connection,
     *  it will be reused.
     */
    Vertex addVertex(Connection connection)
    {
      Vertex v = getVertex(connection);

      if(v.isNull()){
        v = Vertex(boost::add_vertex(connection, graph));
      }

      return v;
    }

    /*
     * Get count of all vertices in the graph
     */
    int vertexCount() const
    {
      return boost::num_vertices(graph);
    }

    /*
     * Get vertex that represents connection
     * If no vertex was found, a null Vertex is returned
     */
    Vertex getVertex(Connection connection) const
    {
      const auto itPair = boost::vertices(graph);
      const auto first = itPair.first;
      const auto last = itPair.second;

      if(first == last){
        return Vertex();
      }
      const auto it = std::find_if(first, last, [this, connection](const vertex_t & v){ return (graph[v].id == connection.id); });
      if(it == last){
        return Vertex();
      }

      return Vertex(*it);
    }

    /*
     * Get count of vertices that represents connection.
     * This is used for unit tests
     */
    int getVertexCount(Connection connection) const
    {
      const auto itPair = boost::vertices(graph);
      const auto first = itPair.first;
      const auto last = itPair.second;

      if(first == last){
        return 0;
      }

      return std::count_if(first, last, [this, connection](const vertex_t & v){ return (graph[v].id == connection.id); });
    }

    /*
     * Add a edge for link
     * Will also automatically get, or create, vertices for start and end connections
     */
    Edge addEdge(const Link & link)
    {
      const auto start = addVertex(link.start);
      const auto end = addVertex(link.end);
      Q_ASSERT(!start.isNull());
      Q_ASSERT(!end.isNull());

      return boost::add_edge(start.v, end.v, link, graph).first;
    }

    /*
     * Remove edge from graph
     */
    void removeEdge(Edge edge)
    {
      boost::remove_edge(edge, graph);
    }

    /*
     * Get count of all edges in the graph
     */
    int edgeCount() const
    {
      return boost::num_edges(graph);
    }

    /*
     * Clear the graph
     */
    void clear()
    {
      graph.clear();
    }

    /*
     * Get connection for given vertex
     */
//     Connection getConnection(Vertex vertex) const
//     {
//       ///return graph[vertex.v];
//       ///return boost::get<Vertex>(graph);
//     }
  };

}}}} // namespace Mdt{ namespace CableList{ namespace Path{ namespace GraphPrivate{

#endif // #ifndef MDT_CABLE_LIST_PATH_GRAPH_PRIVATE_GRAPH_H
