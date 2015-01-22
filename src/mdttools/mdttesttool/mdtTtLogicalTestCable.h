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
#ifndef MDT_TT_LOGICAL_TEST_CABLE_H
#define MDT_TT_LOGICAL_TEST_CABLE_H

#include "mdtTtBase.h"
#include "mdtClLinkData.h"  /// \todo Obselete ??
#include "mdtTtTestLinkData.h"
#include <QString>
#include <QVariant>
#include <QList>

class mdtClPathGraph;

/*! \brief Helper class for test connection cable edition
 */
class mdtTtLogicalTestCable : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtTtLogicalTestCable(QObject *parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtTtLogicalTestCable();

  /*! \brief Get SQL statement for test node unit selection
   *
   * List all test node units that are not currently affected to given test cable.
   */
  QString sqlForTestNodeUnitSelection(const QVariant & testCableId);

  /*! \brief Get SQL statement for DUT unit selection
   *
   * List all DUT units that are not currently affected to given test cable.
   */
  QString sqlForDutUnitSelection(const QVariant & testCableId);

  /*! \brief Add a test node unit
   */
  bool addTestNodeUnit(const QVariant & testNodeUnitId, const QVariant & testCableId);

  /*! \brief Remove test node units
   */
  bool removeTestNodeUnits(const mdtSqlTableSelection & s);

  /*! \brief Add a DUT unit
   */
  bool addDutUnit(const QVariant & unitId, const QVariant & testCableId);

  /*! \brief Remove DUT units
   */
  bool removeDutUnits(const mdtSqlTableSelection & s);

  
  /*! \brief Get SQL statement for test node selection
   */
  QString sqlForTestNodeSelection() const;

  /*! \brief Get SQL statement for start unit connector selection
   *
   * \param dutUnitId ID of unit that will become the DUT (Device Under Test)
   */
  QString sqlForStartConnectorSelection(const QVariant & dutUnitId) const;

  /*! \brief Get SQL statement for unit connector selection for given unit connector ID list
   */
  QString sqlForUnitConnectorSelectionFromUnitConnectorIdList(const QList<QVariant> & connectorIdList) const;

  /*! \brief Get SQL statement for test cable selection
   */
  QString sqlForTestCableSelection();

  /*! \brief Load link list
   *
   * Will load link list into internal graph.
   *  Some methods will use this graph to find a path starting at a unit connection,
   *  connections that are part of a given path, ...
   */
  bool loadLinkList();

  /*! \brief Get a list of unit connections IDs related to given unit connector ID
   */
  QList<QVariant> getToUnitConnectorRelatedUnitConnectionIdList(const QVariant & unitConnectorId);

  /*! \brief Get a list of unit connections that are part of a connector
   *
   * This method will return unit connections that are part of given unit connector,
   *  starting with given unit connections id list (filter) .
   */
  ///QList<QVariant> getToUnitConnectorRelatedUnitConnectionIdList(const QVariant & unitConnectorId, const QList<QVariant> & unitConnectionIdList);

  /*! \brief Get a list of unit connectors that are linked to given unit connection
   *
   * Note: link list must be loaded with loadLinkList() , else this method will fail.
   */
  QList<QVariant> getToUnitConnectionLinkedUnitConnectorIdList(const QVariant & fromUnitConnectionId);

  /*! \brief Get a list of unit connections that are linked to given start unit connection, and that are part of given unit connector
   *
   * Will get unit connections that are linked to given fromUnitConnectionId ,
   *  but will only return those that are part of given unitConnectorId .
   *
   * Note: link list must be loaded with loadLinkList() , else this method will fail.
   */
  QList<QVariant> getToUnitConnectionIdLinkedUnitConnectionIdListPartOfUnitConnector(const QVariant & fromUnitConnectionId, const QVariant & unitConnectorId);

  /*! \brief Get a list of unit connections that are linked to given start unit connection, and that are part of given unit connector list
   *
   * \sa getToUnitConnectionLinkedUnitConnectionIdListPartOfUnitConnector()
   */
  QList<QVariant> getToUnitConnectionIdLinkedUnitConnectionIdListPartOfUnitConnectorList(const QVariant & fromUnitConnectionId, const QList<QVariant> & unitConnectorIdList);

  /*! \brief Get a list of unit connections that are linked to given start unit connection list, and that are part of given unit connector list
   *
   * \sa getToUnitConnectionLinkedUnitConnectionIdListPartOfUnitConnector()
   *
   * Note: will alter the given fromUnitConnectionIdList
   */
  QList<QVariant> getToUnitConnectionIdListLinkedUnitConnectionIdListPartOfUnitConnectorList(QList<QVariant> & fromUnitConnectionIdList, const QList<QVariant> & unitConnectorIdList);

  /*! \brief Create a test cable
   */
  ///bool createTestCable(const QVariant & nodeId, const QList<QVariant> & busAtestConnectionIdList, const QList<QVariant> & busAdutConnectionIdList, const QList<QVariant> & busBtestConnectionIdList, const QList<QVariant> & busBdutConnectionIdList);

  /*! \brief Check if a logical test cable with key allready exists
   */
  bool cableExistsByKey(const QString & key, bool & ok);

  /*! \brief Create a logical test cable
   *
   * For each item in linkDataList, field LogicalTestCable_Id_FK will be updated
   *  with freashly created logical test cable.
   */
  bool createCable(const mdtSqlRecord & cableData, QList<mdtTtTestLinkData> & linkDataList);

  /*! \brief Get test link data
   */
  mdtTtTestLinkData getLinkData(const QVariant & testConnectionId, const QVariant & dutConnectionId, bool *ok);

  /*! \brief Add a link into test link table
   */
  bool addLink(const mdtTtTestLinkData & data);

  /*! \brief Add links in link table
   *
   * \pre dutConnectionIdList size must be <= testConnectionIdList
   */
  bool addLinks(const QVariant & nodeId, const QVariant & testCableId, const QList<QVariant> & testConnectionIdList, const QList<QVariant> & dutConnectionIdList);

  /*! \brief Edit a test link
   */
  bool editLink(const QVariant & testConnectionId, const QVariant & dutConnectionId, const mdtTtTestLinkData & linkData);

  /*! \brief Remove a test link
   */
  bool removeLink(const QVariant & testConnectionId, const QVariant & dutConnectionId);

  /*! \brief Add a cable into test cable table
   */
  bool addCable(const QVariant & identification);

  /*! \brief Add a link into test link table
   *
   * \todo Obselete this version
   */
  bool addLink(const QVariant & testConnectionId, const QVariant & dutConnectionId, const QVariant & testCableId, const QVariant & identification, const QVariant & value);

  /*! \brief Connect a test cable
   */
  bool connectTestCable(const QVariant & testCableId, const QVariant & testNodeId, const QVariant & dutVehicleTypeId);

  /*! \brief Get test link data
   */
  ///mdtClLinkData getTestLinkData(const QVariant & testLinkId);

  /*! \brief Get test link data for given test cable ID
   */
  ///QList<mdtClLinkData> getTestLinkDataByTestCableId(const QVariant & testCableId);

  /*! \brief Disconnect a test cable
   */
  bool disconnectTestCable(const QVariant & testCableId);

 private:

  /*! \brief Get unit ID list of connectors used by a list of test links
   */
  QList<QVariant> getUnitIdListOfTestConnectorsUsedInTestLinks(const QList<QVariant> testLinkIdList, bool & ok);

  Q_DISABLE_COPY(mdtTtLogicalTestCable);

  mdtClPathGraph *pvPathGraph;
};

#endif // #ifndef MDT_TT_LOGICAL_TEST_CABLE_H
