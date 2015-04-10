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
#ifndef MDT_TT_TEST_MODEL_H
#define MDT_TT_TEST_MODEL_H

#include "mdtTtBase.h"
#include "mdtSqlTableSelection.h"
#include "mdtTtTestModelData.h"
#include "mdtClPathGraph.h"
#include "mdtTtTestModelItemData.h"
#include "mdtTtTestModelItemRouteData.h"
#include <QVariant>
#include <QList>
#include <QModelIndex>


/*! \brief Data container for test model generation parameters
 */
struct mdtTtTestModelGenerationParameter
{
  /*! \brief ID of test model
   */
  QVariant testModelId;

  /*! \brief ID of test cable (TestCable_tbl) to use.
   */
  QVariant testCableId;

  /*! \brief ID of test node to use.
   */
  QVariant testNodeId;

  /*! \brief ID of first measure connection to use (TestNodeUnitConnection_tbl)
   */
  QVariant measureConnexionIdA;

  /*! \brief ID of second measure connection to use (TestNodeUnitConnection_tbl)
   */
  QVariant measureConnexionIdB;

  /*! \brief Expected value between linked connections
   */
  double continuityExpectedValue;

  /*! \brief If true, test model items will also be generated for non linked connections
   */
  bool generateForNonLinkedConnections;

  /*! \brief Expected value between not linked connections
   */
  double isolationExpectedValue;
};



/*! \brief Helper class for test model edition
 */
class mdtTtTestModel : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtTtTestModel(QObject *parent, QSqlDatabase db);

  /*! \brief Get SQL statement for test node selection
   *
   * Will list all test nodes that are not currently assigned to given test model
   */
  QString sqlForTestNodeSelection(const QVariant & testModelId) const;

  /*! \brief Get test model data for given test model ID
   */
  mdtTtTestModelData getTestModelData(const QVariant & testModelId, bool & ok);

  /*! \brief Get a list of test item IDs for given test ID
   */
  QList<QVariant> getListOfTestItemIdListByTestId(const QVariant & testId);

  /*! \brief Get a list of node IDs (database keys) for given test ID
   */
  QList<QVariant> getListOfUsedNodeIdListByTestId(const QVariant & testId);

  /*! \brief Get a list of unused node unit IDs for given test ID
   *
   * If given type is not null, only matching test node unit will be returned
   */
  QList<QVariant> getListOfUnusedNodeUnitIdListByTestId(const QVariant & testId, const QVariant & type);

  /*! \brief Get next sequence number for a given test
   *
   * This method is used to create test items .
   *  It will get the highest sequence number
   *  existing for given testModelId, and return the next number .
   */
  QVariant getNextSequenceNumber(const QVariant & testModelId);

  /*! \brief Get list of test node unit setup for given test ID
   */
  QList<QVariant> getTestNodeUnitSetupIdList(const QVariant & testId);

  /*! \brief Add a test node
   */
  bool addTestNode(const QVariant & testNodeId, const QVariant & testModelId);

  /*! \brief Get a list of test node data for given test model ID
   *
   * Data will contain fields from TestNode_tbl and VehicleType_tbl
   */
  QList<QSqlRecord> getTestNodeDataList(const QVariant & testModelId, bool & ok);

  /*! \brief Remove a test node
   */
  bool removeTestNodes(const mdtSqlTableSelection & s);

  /*! \brief Remove a test model item
   *
   * \param testModelItemId ID of test model item to remove
   * \param removeSetupAndRoute If true, related items in TestNodeUnitSetup_tbl and TestModelItemRoute_tbl will be removed first.
   * \param removeTestLinks If true, related items in TestModelItem_TestLink_tbl will be removed first.
   * \param handleTransaction If true, transaction will be handled internally.
   */
  bool removeTestModelItem(const QVariant & testModelItemId, bool removeSetupAndRoute, bool removeTestLinks, bool handleTransaction);

  /*! \brief Remove a list of test model items
   */
  bool removeTestModelItems(const mdtSqlTableSelection & s, bool removeSetupAndRoute, bool removeTestLinks);

  /*! \brief Get link ID list for given cable ID and DUT connection ID
   */
  QList<QVariant> getTestLinkIdListForDutConnectionId(const QVariant & testCableId, const QVariant & dutConnectionId, bool & ok);

  /*! \brief Get test connection ID list for given test link ID and DUT connection ID
   */
  QVariant getTestConnectionId(const QVariant & testLinkId, const QVariant & dutConnectionId, bool & ok);

  /*! \brief Build route data for given path ID list
   *
   * \param pathConnectionIdList List of connections (IDs) that represent the path from a test connection to a measure connection
   * \return Route data with setupDataList set. All other members are not touched (route ID, testModelItemId, ...)
   * \pre pathConnectionIdList must contain at least 2 connections
   */
  mdtTtTestModelItemRouteData buildRouteData(const QList<QVariant> & pathConnectionIdList, mdtClPathGraph & graph, bool & ok);

  /*! \brief Search 2 path from given DUT connections and measure connections and add routes to item data
   *
   * \param dutConnectionId1 First test connection
   * \param dutConnectionId2 Second test connection
   * \param parameters At least testCableId, measureConnexionIdA and measureConnexionIdB must be set
   * \param measureConnexionIdA ID of measure connection to witch to find path
   * \param measureConnexionIdB ID of other measure connection
   * \param testCableId ID of used test cable
   * \param itemData Routes will be added to itemData
   * \param graph Graph object.
   * \param graphForShortDetection Graph object - Will be altered.
   */
  bool addRoutesToItemData(const QVariant & dutConnectionId1, const QVariant & dutConnectionId2,
                           const mdtTtTestModelGenerationParameter & parameters, mdtTtTestModelItemData & itemData,
                           mdtClPathGraph & graph, mdtClPathGraph & graphForShortDetection);

  /*! \brief Generate test items
   *
   * \param parameters Setup for test model
   * \param graph Graph object. Cable list must allready be loaded (see mdtClPathGraph::loadLinkList())
   */
  bool generateTestModel(mdtTtTestModelGenerationParameter & parameters, mdtClPathGraph & graph);

 private:

  /*! \brief Search 2 path from given test connections and measure connections
   *
   * \param testConnectionId1 First test connection
   * \param testConnectionId2 Second test connection
   * \param measureConnexionIdA ID of measure connection to witch to find path
   * \param measureConnexionIdB ID of other measure connection
   * \param pathConnectionIdList1 First path will be stored here
   * \param pathConnectionIdList2 Second path will be stored here
   * \param graph Graph object.
   * \post If this method returns true, pathConnectionIdList1 and pathConnectionIdList2 has at least 2 items each.
   */
  bool searchPathFromTestConnectionsToMeasureConnections(const QVariant & testConnectionId1, const QVariant & testConnectionId2,
                                                         const QVariant & measureConnexionIdA, const QVariant & measureConnexionIdB,
                                                         QList<QVariant> & pathConnectionIdList1, QList<QVariant> & pathConnectionIdList2,
                                                         mdtClPathGraph & graph);


  /*! \brief Get contact name of given ID
   */
  QString getUnitContactName(const QVariant & id);

  /*! \brief Get test cable ID string
   */
  QString getTestCableIdString(const QVariant & id);

  /*! \brief Get a string for the test model item's designation
   */
  QVariant getDesignationEN(const QVariant & fromTestLinkId, const QVariant & toTestLinkId);

  Q_DISABLE_COPY(mdtTtTestModel);
};

#endif // #ifndef MDT_TT_TEST_MODEL_H
