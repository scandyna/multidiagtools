/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_TT_TEST_NODE_ROUTE_H
#define MDT_TT_TEST_NODE_ROUTE_H

#include "mdtTtBase.h"
#include "mdtTtTestNodeRouteData.h"
#include "mdtClPathGraph.h"
#include "mdtSqlTableSelection.h"
#include "mdtValue.h"
#include <QDateTime>
#include <QSqlDatabase>
#include <QVariant>
#include <QString>

/*! \brief Test node route creation and access
 */
class mdtTtTestNodeRoute : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtTtTestNodeRoute(QObject* parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtTtTestNodeRoute(QSqlDatabase db);

  /*! \brief Build a route between 2 connections of the test node
   *
   * Will search a path that links given connections and defines wich relays that must be enabled for it.
   *
   * \param testNodeId ID of test node
   * \param testNodeConnectionAId ID of first (external) connection of test node
   * \param testNodeConnectionBId ID of second (external) connection of test node
   * \param graph Graph to use for path search.
   *               Link list must allready been loaded (see mdtClPathGraph::loadLinkList() )
   *               and (coupling and channel) relays of test node been added (see mdtTtTestNode::addRelaysToGraph() ).
   * \param ok Will be set false on error, true else.
   * \return Created route. A empty route means that no route could be found between given connections, or a error occured.
   *         Use the ok parameter to differenciate this 2 cases.
   */
  mdtTtTestNodeRouteData buildRoute(const QVariant & testNodeId, const QVariant & testNodeConnectionAId, const QVariant & testNodeConnectionBId, mdtClPathGraph & graph, bool & ok);

  /*! \brief Add a route to TestNodeRoute_tbl
   *
   * Will also add related elements (relays) in TestNodeRouteUnit_tbl
   */
  bool addRoute(const mdtTtTestNodeRouteData & data);

  /*! \brief Get a route from TestNodeRoute_tbl
   *
   * \param testNodeId ID of test node
   * \param testNodeConnectionAId ID of first (external) connection of test node
   * \param testNodeConnectionBId ID of second (external) connection of test node
   * \param ok Will be set false on error, true else.
   * \return Requested route. A empty route means that no route could be found between given connections, or a error occured.
   *         Use the ok parameter to differenciate this 2 cases.
   */
  mdtTtTestNodeRouteData getRoute(const QVariant & testNodeId, const QVariant & testNodeConnectionAId, const QVariant & testNodeConnectionBId, bool & ok);

  /*! \brief Set route resistance
   */
  bool setRouteResistance(const QVariant & routeId, const mdtValueDouble & resistance, const QDateTime d = QDateTime::currentDateTime());

  /*! \brief Remove a route from TestNodeRoute_tbl
   *
   * Will also remove related elements (relays) from TestNodeRouteUnit_tbl
   */
  bool removeRoute(const QVariant & routeId);

  /*! \brief Remove routes from TestNodeRoute_tbl
   *
   * Will also remove related elements (relays) from TestNodeRouteUnit_tbl.
   * In given table selection, field Id_PK is threated as test node route ID
   */
  bool removeRoutes(const mdtSqlTableSelection & s);

 private:

  /*! \brief Remove a route from TestNodeRoute_tbl
   *
   * No transaction is initiated by this helper function
   */
  bool removeRoutePv(const QVariant & routeId);

  /*! \brief Add a relay to list of relays to enable
   */
  void addRelayToRoute(const QVariant & relayId, mdtTtTestNodeRouteData & routeData);

  /*! \brief Add a relay to TestNodeRouteUnit_tbl
   */
//   bool addRelayToDatabase(const mdtTtTestNodeRouteRelay & relay);

  /*! \brief Get string with schemaPosition;contactName for given unit connection ID
   */
  QString getSchemaPosAndContactName(const QVariant & connectionId);

  Q_DISABLE_COPY(mdtTtTestNodeRoute);
};

#endif // #ifndef MDT_TT_TEST_NODE_ROUTE_H
