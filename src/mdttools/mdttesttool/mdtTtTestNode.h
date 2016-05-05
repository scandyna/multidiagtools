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
#ifndef MDT_TT_TEST_NODE_H
#define MDT_TT_TEST_NODE_H

#include "mdtTtBase.h"
#include "mdtTtTestNodeUnitData.h"
#include "mdtTtTestNodeUnitSetupData.h"
#include "mdtClPathGraph.h"
#include "mdtClLinkData.h"
#include "mdtTtTestNodeRouteData.h"
#include <QList>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QSqlRecord>
#include <QPair>
#include <vector>

/*! \brief Helper class for test node edition
 */
class mdtTtTestNode : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtTtTestNode(QObject *parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtTtTestNode(QSqlDatabase db);

  /*! \brief Get SQL statement for units selection
   *
   * Give a list of units related to given TestNodeId (i.e. VehicleType ID)
   *  that are not currently linked to a TestNodeUnit .
   */
  QString sqlForUnitSelection(const QVariant & VehicleTypeId);

  /*! \brief Get SQL statement for unit selection for given list of unit IDs
   */
  QString sqlForUnitSelectionByUnitIdList(const QList<QVariant> & unitIdList);

  /*! \brief Get SQL statement for unit connection selection
   */
  QString sqlForUnitConnectionSelection(const QVariant & unitId);

  /*! \brief Get test node data
   *
   * Will include fields from TestNode_tbl and VehicleType_tbl.
   */
  QSqlRecord getTestNodeData(const QVariant & testNodeId, bool &ok);

  /*! \brief Get alias of given test node
   */
  QString getTestNodeAlias(const QVariant & testNodeId, bool & ok);

  /*! \brief Get test node ID for given Alias
   */
  QVariant getTestNodeIdForAlias(const QString & alias);

  /*! \brief Get a list of unit connection IDs that are linked (meaning wiered) to given unit connection ID
   */
  QList<QVariant> getIdListOfUnitConnectionsLinkedToUnitConnectionId(const QVariant & unitConnectionId);

  /*! \brief Get a list of unit IDs for given list of unit connection IDs
   */
  QList<QVariant> getIdListOfUnitIdForUnitConnectionIdList(const QList<QVariant> & unitConnectionIdList);

  /*! \brief Get a list of unit connection IDs that are part of given unit ID
   */
  QList<QVariant> getIdListOfUnitConnectionsPartOfUnit(const QVariant & unitId);

  /*! \brief Get a list of channel test connections for given bus name and node ID
   */
  QList<QVariant> getChannelTestConnectionIdList(const QVariant & testNodeId, const QVariant & busName);

  /*! \brief Get a list of test node route relays for given test node and list of schema positions
   *
   * \return List of route relays. On error, a empty list is returned (non found relays is also considered as a error).
   */
  std::vector<mdtTtTestNodeRouteRelay> getTestNodeRouteRelays(const QVariant & testNodeId, const QStringList & schemaPositionList);

  /*! \brief Get test node route relay ID by schema position
   *
   * \return ID of relay. If returned ID is null, it can be due that is not exists, or a error occured.
   *          Use the ok flag to differiencate both cases.
   *
   * Note: if schema position is not unique for given test node, the first found is returned.
   */
  QVariant getTestNodeRouteRelayIdBySchemaPosition(const QVariant & testNodeId, const QString & schemaPosition, bool & ok);

  /*! \brief Update node unit connections of given test node
   *
   * For each unit contained in given test node, the missing connections
   *  (those that exist in UnitConnection_tbl but not in TestNodeUnitConnection_tbl)
   *  will be added in TestNodeUnitConnection_tbl.
   */
  bool addMissingConnections(const QVariant & testNodeId);

  /*! \brief Set I/O position for a range of test node units
   *
   * Note: this method will not check if all given test node units
   *  are of the same type (AI, AO, ...).
   *
   * \pre Each item in selection must contain Unit_Id_FK_PK field
   */
  bool setTestNodeUnitIoRange(const mdtSqlTableSelection & s, int startIoPosition);

  /*! \brief Add (coupling and channel) relays to graph
   *
   * Will add all coupling and channel relays of given test node to graph
   *  as a link with a cost of 2.
   *  The user data will also contain unit ID of each relay.
   */
  bool addRelaysToGraph(const QVariant & testNodeId, mdtClPathGraph & graph, const QList<QVariant> & relaysToIgnoreIdList = QList<QVariant>());

  /*! \brief Get a list of test node unit setup data for a given path
   *
   * For each connection in given path,
   *  related test node unit id (i.e. relay ID) will be searched.
   *  For each relay that was found, a new setup data, with State set to true, is added to list.
   *
   * \param relayPathConnectionIdList List of connection ID of path (as returned by mdtClPathGraph::getShortestPath()).
   * \param graph Graph object. Relays must allready been loaded (see addRelaysToGraph()).
   * \param ok Will be set false if a error occured, true else
   * 
   * \deprecated ?
   */
  QList<mdtTtTestNodeUnitSetupData> getRelayPathSetupDataList(const QList<QVariant> & relayPathConnectionIdList, mdtClPathGraph & graph, bool & ok);

  /*! \brief Check if enabling a set of coupling and channel relays produces a link between connections A and B
   *
   * \todo rename !
   * 
   * \param connectionIdA Fisrt connection from witch it must be checked about short circuit
   * \param connectionIdB Second connection from witch it must be checked about short circuit
   * \param relays List of coupling and channel relays that will be turned ON in given scenario
   * \param graph Graph to use for path search.
   *               Link list must allready been loaded (see mdtClPathGraph::loadLinkList() ).
   * \param ok Will be set false if a error occured, true else
   * \return true if given list of relays produces a short between connections A and B
   * \pre connectionIdA and connectionIdB must not be null
   */
  bool makesShortCircuit(const QVariant & connectionIdA, const QVariant & connectionIdB, const std::vector<mdtTtTestNodeRouteRelay> & relays, mdtClPathGraph & graph, bool & ok);

  /*! \brief Check that enabling a set of coupling and channel relays does not produce a short circuit between connections A and B
   *
   * \param connectionIdA Fisrt connection from witch it must be checked about short circuit
   * \param connectionIdB Second connection from witch it must be checked about short circuit
   * \param testNodeUnitSetupDataList Test node unit setup data list of coupling and channel relays that will be turned on in given scenario
   * \param graph Graph object. Cable list must allready be loaded (see mdtClPathGraph::loadLinkList()).
   * \param ok Will be set false if a error occured, true else
   * \return True if a short circuit was detected, false if OK
   * 
   * \deprecated 
   */
  ///[[deprecated]]
  bool ensureAbsenceOfShortCircuit(const QVariant & connectionIdA, const QVariant & connectionIdB, const QList<mdtTtTestNodeUnitSetupData> & testNodeUnitSetupDataList, mdtClPathGraph & graph, bool & ok);

  /*! \brief Get a list of links that are travsersed from connectionIdA to connectionIdB
   *
   * \param connectionIdA Starting connection (f.ex. a measure connection)
   * \param connectionIdB Ending connection (f.ex. a test connection)
   * \param testNodeUnitSetupDataList Test node unit setup data list of coupling and channel relays that are turned on
   * \param graph Graph object. Cable list must allready be loaded (see mdtClPathGraph::loadLinkList()).
   * \param ok Will be set false if a error occured, true else
   */
  QList<mdtClLinkData> getLinkDataListForPath(const QVariant & connectionIdA, const QVariant & connectionIdB,
                                              const QList<mdtTtTestNodeUnitSetupData> & testNodeUnitSetupDataList,
                                              mdtClPathGraph & graph, bool & ok);

 private:

  /*! \brief Get 2 non linked connections of given relay
   */
  QPair<QVariant, QVariant> getTwoRelayConnections(const QVariant & testNodeUnitId, mdtClPathGraph & graph, bool & ok);

  /*! \brief Add given list of relays to graph
   */
  bool addRelayListToGraphPv(const QList<QVariant> & testNodeUnitIdList, mdtClPathGraph & graph, int relayCost);

  Q_DISABLE_COPY(mdtTtTestNode);
};

#endif // #ifndef MDT_TT_TEST_NODE_H
