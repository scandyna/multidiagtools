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
#ifndef MDT_CL_PATH_GRAPH_H
#define MDT_CL_PATH_GRAPH_H

#include "mdtClUnitConnectionData.h"
#include "mdtClLinkData.h"
#include <QSqlDatabase>
#include <QQueue>
#include <QPair>
#include <QVariant>
///#include <QList>
#include <QHash>
#include <boost/config.hpp>
#include <vector>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/breadth_first_search.hpp>

class mdtClPathGraphicsConnection;
class mdtClPathGraphicsLink;
class QSqlQueryModel;
class QGraphicsScene;
class QGraphicsView;

namespace mdtClPathGraphPrivate
{
  /*
   * Edge data container
   *
   * Note:
   *  We use e directed graph, so we can handle directed links (typically diodes).
   *  For a bidirectionnal link, we will also add 2 edges per link,
   *   The "normal" (start->end) and its complement (end->start) .
   */
  struct mdtClPathGraphEdgeData
  {
    QVariant startConnectionId;
    QVariant endConnectionId;
    bool isComplement;
  };

  /*
   * Boost graph
   */
  typedef boost::adjacency_list<
    boost::vecS, boost::vecS, boost::directedS, // OutEdgeList: vecS (std::vector) , VertexList: vecS (std::vector) , Directed: directedS (Directed graph)
    boost::no_property,                         // VertexProperties: no_property (None)
    ///boost::property<boost::edge_bundle_t, QPair<QVariant, QVariant> >,  // EdgeProperties: we store each link as <start,end> connection IDs
    boost::property<boost::edge_bundle_t, mdtClPathGraphEdgeData>,
    boost::no_property,                         // GraphProperties: no_property (None)
    boost::listS                                // EdgeList: listS (std::list)
  > graph_t;

  // Set vertex and edge types
  typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_t;
  typedef boost::graph_traits<graph_t>::edge_descriptor edge_t;

  /*
   * Visitor class
   */
  class mdtClPathGraphVisitor : public boost::default_bfs_visitor
  {
  public:

    mdtClPathGraphVisitor();
    ///mdtClPathGraphVisitor(QQueue<QPair<QVariant, QVariant> > *edgeQueue);
    mdtClPathGraphVisitor(QQueue<mdtClPathGraphEdgeData> *edgeQueue);
    void discover_vertex(vertex_t v, const graph_t & g);
    void examine_edge(edge_t e, const graph_t &g);

  private:

    ///QQueue<QPair<QVariant, QVariant> > *pvEdgeQueue;
    QQueue<mdtClPathGraphEdgeData> *pvEdgeQueue;
  };
};  // end namespace mdtClPathGraphPrivate

//Handle also the (Boost) graph
/*! \brief Represents the link list in a graph
 */
class mdtClPathGraph
{
 public:

  /*! \brief Constructor
   */
  mdtClPathGraph(QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClPathGraph();

  /*! \brief Load the link list
   */
  bool loadLinkList();

  /*! \brief Draw the graphic representation starting from a connection
   */
  bool drawPath(const QVariant & fromConnectionId);

  /*! \brief
   */
  QVariant getLinkedConnectionIds(const QVariant & fromConnectionId);

  /*! \brief Attach a view to the scene of the path graph
   */
  void attachView(QGraphicsView *view);

 private:

  /*! \brief Create a new graphics connection item
   */
  mdtClPathGraphicsConnection *newConnectionItem(int id, mdtClPathGraphicsConnection *itemForNextPos, bool reverse);

  /*! \brief Set connection and link data
   */
  bool setGraphicsItemsData(mdtClPathGraphicsConnection *startConnection, mdtClPathGraphicsConnection *endConnection, mdtClPathGraphicsLink *link, int startConnectionId, int endConnectionId);

  /*! \brief
   */
  mdtClUnitConnectionData getUnitConnectionStartDataFromModel(int row);

  /*! \brief
   */
  mdtClUnitConnectionData getUnitConnectionEndDataFromModel(int row);

  /*! \brief
   */
  mdtClLinkData getLinkDataFromModel(int row);

  Q_DISABLE_COPY(mdtClPathGraph);

  QSqlDatabase pvDatabase;
  QSqlQueryModel *pvLinkListModel;
  ///QQueue<QPair<QVariant, QVariant> > pvEdgeQueue;
  QQueue<mdtClPathGraphPrivate::mdtClPathGraphEdgeData> pvEdgeQueue;
  ///QList<mdtClPathGraphicsConnection *> pvDrawnConnections;
  QHash<int, mdtClPathGraphicsConnection *> pvDrawnConnections;
  QGraphicsScene *pvGraphicsScene;
  mdtClPathGraphPrivate::graph_t pvGraph;
  QHash<int, mdtClPathGraphPrivate::vertex_t> pvGraphVertices;
};

#endif // #ifndef MDT_CL_PATH_GRAPH_H
