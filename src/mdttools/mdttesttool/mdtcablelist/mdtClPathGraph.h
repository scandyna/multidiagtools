/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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

#include "mdtError.h"
#include <QSqlDatabase>
#include <QPair>
#include <QVariant>
#include <QList>
#include <QVector>
#include <QStringList>
#include <QHash>
#include <boost/config.hpp>
#include <vector>
#include <string>
#include <map>
#include <utility> // std::pair
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>

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
   *  We use a directed graph, so we can handle directed links (typically diodes).
   *  For a bidirectionnal link, we will also add 2 edges per link,
   *   The "normal" (start->end) and its complement (end->start) .
   */
  struct mdtClPathGraphEdgeData
  {
    QVariant startConnectionId;
    QVariant endConnectionId;
    bool isComplement;
    QVariant userData;
  };

  typedef boost::property<boost::edge_weight_t, int> weigth_property_t;

  /*
   * Boost graph
   */
  typedef boost::adjacency_list<
    boost::vecS, boost::vecS, boost::directedS, // OutEdgeList: vecS (std::vector) , VertexList: vecS (std::vector) , Directed: directedS (Directed graph)
    boost::no_property,                         // VertexProperties: no_property (None)
    weigth_property_t,                          // EdgeProperties: edge weigth
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
      mdtClPathGraphVisitor(QVector<edge_t> *edgeList);
      void examine_edge(edge_t e, const graph_t &g);

    private:

      QVector<edge_t> *pvVisitedEdgeList;
  };
};  // end namespace mdtClPathGraphPrivate

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

  /*! \brief Add a link to the graph
   *
   * Can be used to add some kind of "ghost links" to the graph.
   *  For normal usage, only call loadLinkList().
   */
  ///void addLink(const QVariant & startConnectionId, const QVariant & endConnectionId, bool isBidirectional = true, int weight = 1);

  /*! \brief Add a link to the graph
   *
   * Can be used to add some kind of "ghost links" to the graph.
   *  For normal usage, only call loadLinkList().
   */
  void addLink(const QVariant & startConnectionId, const QVariant & endConnectionId, const QVariant & userData = QVariant(), bool isBidirectional = true, int weight = 1);

  /*! \brief Get user data for item between start and end connection
   */
  QVariant getUserData(const QVariant & startConnectionId, const QVariant & endConnectionId);

  /*! \brief Remove links that where added with addLink()
   */
  void removeAddedLinks();

  /*! \brief Get a list of unit connections IDs that are linked to a given connection
   *
   * Note: fromConnectionId is not included in result.
   */
  QList<QVariant> getLinkedConnectionIdList(const QVariant & fromConnectionId);

  /*! \brief Get a list of unit connector ID that are linked to given one
   *
   * \param fromConnectorId Connector from witch linked ones will be searched
   * \param connectionIdListToIgnore For example, if each connector has a GND, probably are they all connected together.
   *                        In such case, this method has no sense.
   *                        If given fromConnectorId contains such special connections, give their IDs here.
   *
   * Note: fromConnectorId is not included in result.
   */
  QList<QVariant> getLinkedConnectorIdList(const QVariant & fromConnectorId, bool *ok, const QList<QVariant> & connectionIdListToIgnore = QList<QVariant>());

  /*! \brief Get shorted path from start connection ID to end connection ID
   *
   * Will return a list of connections ID that represent
   *  the shorted path from a connection ID to destination connection ID,
   *  including given fromConnectionId and toConnectionId.
   */
  QList<QVariant> getShortestPath(const QVariant & fromConnectionId, const QVariant & toConnectionId, bool & ok);

  /*! \brief Draw the graphic representation starting from a connection
   */
  bool drawPath(const QVariant & fromConnectionId);

  /*! \brief Attach a view to the scene of the path graph
   */
  void attachView(QGraphicsView *view);

  /*! \brief Get last error message
   *
   * The returned list is ready to be used with a QMessageBox .
   *  The first string contains also the text, second the informativeText
   *  and last the detailedText .
   *
   * \post Returned list has allways a size of 3 (detailedText can simply contain a empty string)
   * 
   * \todo Should become Obselete .
   */
  QStringList lastErrorMessage() const;

  /*! \brief Get last error
   */
  mdtError lastError() const;

 private:

  /*! \brief Add a link to the graph
   */
  void addLinkPv(const QVariant & startConnectionId, const QVariant & endConnectionId, bool isBidirectional, int weight, bool addToManuallyList, const QVariant & userData);

  /*! \brief Create a new graphics connection item
   */
  mdtClPathGraphicsConnection *newConnectionItem(int id, mdtClPathGraphicsConnection *itemForNextPos, bool reverse);

  /*! \brief Set connection and link data
   */
  bool setGraphicsItemsData(mdtClPathGraphicsConnection *startConnection, mdtClPathGraphicsConnection *endConnection, mdtClPathGraphicsLink *link, int startConnectionId, int endConnectionId);

  Q_DISABLE_COPY(mdtClPathGraph);

  QSqlDatabase pvDatabase;
  QSqlQueryModel *pvLinkListModel;
  QHash<int, mdtClPathGraphicsConnection *> pvDrawnConnections;
  QGraphicsScene *pvGraphicsScene;
  mdtClPathGraphPrivate::graph_t pvGraph;
  QHash<int, mdtClPathGraphPrivate::vertex_t> pvGraphVertices;
  std::map<mdtClPathGraphPrivate::edge_t, mdtClPathGraphPrivate::mdtClPathGraphEdgeData> pvEdgeDataMap;
  QVector<mdtClPathGraphPrivate::edge_t> pvManuallyAddedEdges;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CL_PATH_GRAPH_H
