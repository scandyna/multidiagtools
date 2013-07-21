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
#ifndef MDT_GRAPH_VERTEX_ADJACENT_H
#define MDT_GRAPH_VERTEX_ADJACENT_H

class mdtGraphVertex;
class mdtGraphEdgeData;

/*! \brief Storage class vor vertex adjacent
 *
 * This class was made to be able to store edge specific data
 */
class mdtGraphVertexAdjacent
{
 public:

  /*! \brief Construct a new adjacent
   *
   * New created object will be adjacent to vertex (this is handled by mdtGraphVertex).
   *
   * \pre vertex and data must be valid.
   */
  mdtGraphVertexAdjacent(mdtGraphVertex *vertex, mdtGraphEdgeData *data);

  /*! \brief Destructor
   */
  ~mdtGraphVertexAdjacent();

  /*! \brief Get vertex on witch current object is adjacent
   */
  mdtGraphVertex *vertex();

  /*! \brief Get edge data
   */
  mdtGraphEdgeData *edgeData();

 private:

  mdtGraphVertex *pvVertex;
  mdtGraphEdgeData *pvData;
};

#endif  // #ifndef MDT_GRAPH_VERTEX_ADJACENT_H
