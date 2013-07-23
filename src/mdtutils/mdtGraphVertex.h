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
#ifndef MDT_GRAPH_VERTEX_H
#define MDT_GRAPH_VERTEX_H

#include "mdtGraphVertexData.h"
#include <QList>

class mdtGraphVertexAdjacent;
class mdtGraphEdgeData;

/*! \brief Vertex of mdtGraph
 *
 * This class is used by mdtGraph to store
 *  vertexes in the graph.
 *
 * The data are stored in mdtGraphVertexData objects.
 *
 * \sa mdtGraph
 */
class mdtGraphVertex
{
 public:

  /*! \brief Color of a vertex
   */
  enum color_t {
                White,  /*!< Vertex was never visited */
                Gray,   /*!< Vertex was visited */
                Black   /*!< All adjacent vertices have been visited */
               };

  /*! \brief Constructor
   *
   * Vertex color will be set to White.
   */
  mdtGraphVertex(bool autoDeleteEdgeData);

  /*! \brief Destructor
   *
   * Note: will not destroy stored data objects.
   */
  ~mdtGraphVertex();

  /*! \brief Set data
   *
   * \pre data must be valid.
   */
  void setData(mdtGraphVertexData *data);

  /*! \brief Get data
   *
   * Can return a Null pointer if data was not set.
   */
  mdtGraphVertexData *data();

  /*! \brief Get key
   *
   * Returns a invalid QVariant when data was not set
   */
  QVariant key() const;

  /*! \brief Add v to the adjacency list
   *
   * \pre v and d must be valid
   */
  bool addAdjacent(mdtGraphVertex *v, mdtGraphEdgeData *d);

  /*! \brief Remove adjacent vertex that has given key
   *
   * \return true if vertex was removed.
   *         false if vertex was not found.
   */
  bool removeAdjacent(const QVariant &key);

  /*! \brief Check if a vertex with given key exists in adjacency list
   */
  bool containsAdjacent(const QVariant &key);

  /*! \brief Check if at least 1 vertex exits in adjacency list
   */
  bool hasAdjacent() const;

  /*! \brief Get number of vertecies stored in adjacency list
   */
  int adjacentCount() const;

  /*! \brief Get adjacency list
   */
  const QList<mdtGraphVertexAdjacent*> adjacencyList() const;

  /*! \brief Set current color
   */
  void setColor(color_t color);

  /*! \brief Get current color
   */
  color_t currentColor() const;

 private:

  Q_DISABLE_COPY(mdtGraphVertex);

  color_t pvColor;
  ///QList<mdtGraphVertex*> pvAdjacencyList;
  QList<mdtGraphVertexAdjacent*> pvAdjacencyList;
  mdtGraphVertexData * pvData;
  bool pvAutoDeleteEdgeData;
};

#endif  // #ifndef MDT_GRAPH_VERTEX_H
