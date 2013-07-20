/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_GRAPH_H
#define MDT_GRAPH_H

#include "mdtGraphVertexData.h"
#include <QList>

class mdtGraphVertex;

/*! \brief Simple implementation of a graph
 *
 * This class was created for some basic graph needs.
 *  It does not claim to be a efficient implementation,
 *  and has some restrictions.
 *  Goal was to have a simple to use graph class.
 *  For more flexible and effective graph implementation,
 *  please take a look at other existing libraries, like Boost.
 *
 * It was choosen to implement the graph by list of adjacency lists.
 *  3 classes are used to handle a graph:
 *   - mdtGraph : this class, handles the vertices and edges between this vertices.
 *   - mdtGraphVertex : represent a vertix in the graph. This class is used internally, and is not part of the public API.
 *   - mdtGraphVertexData : stores key and data of a vertex.
 *
 * Vertices are identified by keys in the graph. Key can be of type int, QString or something else.
 *  Please take a look at Qt's QVariant documentation for details.
 *  For details about data storage, please take a look at mdtGraphVertexData class.
 *
 * Memory management:
 *  When a mdtGraph object is destroyed, each vertex is destroyed, but it's possible to choose
 *   if data ( mdtGraphVertexData objects ) must be destroyed or not.
 *   Behaviour is stored in autoDeleteData flag. See setAutoDeleteData() and autoDeleteData() .
 *
 * To build a graph, it exists 2 possible ways:
 *  - Create a mdtGraphVertexData object and add it with insertVertex(mdtGraphVertexData*).
 *     This is a more flexible solution when more than one data field is needed.
 *  - Use insertVertex(const QVariant&, const QVariant&) witch will create mdtGraphVertexData object internally.
 *     This is the easy to use solution, but is limited to one data field.
 */
class mdtGraph
{
 public:

  /*! \brief Build a empty graph
   */
  mdtGraph(bool autoDeleteData = true);

  /*! \brief Delete a graph
   *
   * Will remove each vertex.
   *  If autoDeleteData flag is set,
   *  data objects are deleted.
   */
  ~mdtGraph();

  /*! \brief set/unset autoDeleteData flag
   */
  void setAutoDeleteData(bool autoDelete);

  /*! \brief Get autoDeleteData flag
   */
  bool autoDeleteData() const;

  /*! \brief Delete all data objects contained in the graph
   */
  void deleteData();

  /*! \brief Insert a new vertex in the graph
   *
   * Data pointed by vertexData will be stored in new created vertex.
   *
   * Complexity is O(S) , where S is the number of vertices in the graph.
   *
   * \return true if insertion succeed.
   *         false if key (contained in vertexData) allready exists in graph,
   *          or on failure of a memory allocation.
   * \pre vertexData must be valid.
   */
  bool insertVertex(mdtGraphVertexData *vertexData);

  /*! \brief Create a new mdtGraphVertexData object and insert it into graph
   *
   * Created data object will contain key and data.
   *
   * \sa insertVertex(mdtGraphVertexData*)
   */
  bool insertVertex(const QVariant &key, const QVariant &data);

  /*! \brief Remove vertex referenced by key from graph
   *
   * If autoDeleteData flag is set, data object will be destroyed.
   *
   * Complexity is O(S+A) , where S is the number of vertices in the graph, and A the number of edges.
   *
   * \return true if vertex could be removed,
   *         false if:
   *          - The vertex was not found
   *          - The vertex contains vertices in his adjacency list
   *          - The vertex is referenced in the adjacency list of another vertex
   */
  bool removeVertex(const QVariant &key);

  /*! \brief Insert a edge from vertex referenced by keyFrom to vertex referenced by keyTo
   *
   * For example, if we have vertices (A) and (B),
   *  and want to link them like this: (A)->(B) ,
   *  we call this method with A as keyFrom and B as keyTo.
   *
   * Note: for undirected graph, call also the method a second time
   *  with keyFrom and keyTo reversed.
   *
   * Complexity is O(S+A) , where S is the number of vertices in the graph, and A the number of edges.
   *
   * \return true if edge could be inserted,
   *         false if:
   *          - One of the vertex (referenced by keyFrom or keyTo) was not found
   *          - The edge allready exists
   */
  bool insertEdge(const QVariant &keyFrom, const QVariant &keyTo);

  /*! \brief Remove edge from vertex referenced by keyFrom to vertex referenced by keyTo
   *
   * For keyFrom and keyTo, please have a look at insertEdge().
   *
   * Complexity is O(S+A) , where S is the number of vertices in the graph, and A the number of edges.
   *
   * \return true if edge was removed,
   *         false if it was not found in graph.
   */
  bool removeEdge(const QVariant &keyFrom, const QVariant &keyTo);

  /*! \brief Check if vertex that has key exists in the graph
   *
   * Complexity is O(S) , where S is the number of vertices in the graph.
   */
  bool containsVertex(const QVariant &key) const;

  /*! \brief Get data object of vertex referenced by key
   *
   * Can return a Null pointer if vertex was not found.
   *
   * Complexity is O(S) , where S is the number of vertices in the graph.
   */
  mdtGraphVertexData *vertexData(const QVariant &key);

  /*! \brief Get data of each vertex that is adjacent to vertex referenced by key
   *
   * Complexity is O(S+A) , where S is the number of vertices in the graph, and A the number of edges.
   */
  const QList<mdtGraphVertexData*> adjacencyDataList(const QVariant & key) const;

  /*! \brief Get number of vertices that graph contains
   *
   * Complexity is O(1)
   */
  int vertexCount() const;

  /*! \brief Get number of edges that graph contains
   *
   * Complexity is O(1)
   */
  int edgeCount() const;

  /*! \brief Get a string that contains all vertices and theire adjacents
   *
   * Could be usefull for debug
   */
  QString graphDump() const;

 private:

  QList<mdtGraphVertex*> pvVertexList;
  bool pvAutoDeleteData;
  int pvEdgeCount;
};

#endif  // #ifndef MDT_GRAPH_H
