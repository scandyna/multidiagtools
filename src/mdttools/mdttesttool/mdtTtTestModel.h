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
#ifndef MDT_TT_TEST_MODEL_H
#define MDT_TT_TEST_MODEL_H

#include "mdtTtBase.h"
#include "mdtSqlTableSelection.h"
#include "mdtTtTestModelData.h"
#include "mdtClPathGraph.h"
#include <QVariant>
#include <QList>
#include <QModelIndex>

class mdtTtTestModel;

/*! \brief Helper class for test model generation
 */
class mdtTtTestModelAbstractGeneratorHelper
{
 public:

  /*! \brief Action
   */
  enum action_t {
                  Fail,               /*!< Fail test model generation */
                  IgnoreCurrentItem   /*!< Do not add a item, continue processing */
  };

  /*! \brief Constructor
   */
  mdtTtTestModelAbstractGeneratorHelper(mdtTtTestModel *tm)
  {
    Q_ASSERT(tm != 0);
    pvTestModel = tm;
  }

  /*! \brief Destructor
   */
  virtual ~mdtTtTestModelAbstractGeneratorHelper() {}

  /*! \brief Get a list of test node unit ID of relays to not use
   */
  virtual QList<QVariant> relaysToIgnoreIdList();

  /*! \brief Get related connections
   *
   * What rRelated connections are depends on implementation.
   *  For example, for a continuity check generator,
   *  they will be linked connections.
   */
  virtual QList<QVariant> getRelatedTestConnectionIdList(const QVariant & testCableId, const QVariant & fromTestConnectionId, mdtClPathGraph & graph, bool & ok) = 0;

  /*! \brief Tell if a related test connection must be queued to solved connections once processed
   */
  virtual bool queueRelatedConnectionToSolved() = 0;

  /*! \brief Tell what to do if a setup is about to produce a short circuit in test node
   */
  virtual action_t actionOnShortInTestNode() = 0;

  /*! \brief Get expected value (type double or Null)
   */
  virtual QVariant expectedValue() = 0;

  /*! \brief Get value unit
   */
  virtual QVariant valueUnit() = 0;

  /*! \brief Get a string for the test model item's designation
   */
  virtual QVariant getDesignationEN(const QVariant & fromTestLinkId, const QVariant & toTestLinkId);

  /*! \brief Get DUT connection ID for given test connection ID
   */
  QVariant getDutConnectionId(const QVariant & testCableId, const QVariant & testConnectionId, bool & ok);

  /*! \brief Get test connection ID for given DUT connection ID
   */
  QVariant getTestConnectionId(const QVariant & testCableId, const QVariant & dutConnectionId, bool & ok, const QVariant & testConnectionToExclude = QVariant());

  /*! \brief Get last error
   */
  mdtError lastError() const {
    return pvLastError;
  }

 protected:

  mdtTtTestModel *pvTestModel;
  mdtError pvLastError;

 private:

  Q_DISABLE_COPY(mdtTtTestModelAbstractGeneratorHelper);
};

/*! \brief Helper class for continuity test generation
 */
class mdtTtTestModelContinuityTestGeneratorHelper : public mdtTtTestModelAbstractGeneratorHelper
{
 public:

  /*! \brief Constructor
   */
  mdtTtTestModelContinuityTestGeneratorHelper(mdtTtTestModel *tm);

  /*! \brief Destructor
   */
  ~mdtTtTestModelContinuityTestGeneratorHelper();

  /*! \brief Get related connections
   *
   * Will get the DUT connection that is linked to given fromTestConnectionId,
   *  then search about linked DUT connections.
   *  Finally, a list of test connection is built for each DUT connection found.
   *
   * \param testCableId ID of test cable (TestCable_tbl) to use.
   * \param fromTestConnectionId Test connection from witch to start.
   * \param graph Graph object. Cable list must allready be loaded (see mdtClPathGraph::loadLinkList())
   * \param ok Will be set to true on success, false else.
   */
  QList<QVariant> getRelatedTestConnectionIdList(const QVariant & testCableId, const QVariant & fromTestConnectionId, mdtClPathGraph & graph, bool & ok);

  /*! \brief Tell if a related test connection must be queued to solved connections once processed
   */
  bool queueRelatedConnectionToSolved();

  /*! \brief Return Fail
   */
  action_t actionOnShortInTestNode();

  /*! \brief Get expected value
   *
   * Return 0.0
   */
  QVariant expectedValue();

  /*! \brief Get value unit
   *
   * Return "Ohm"
   */
  QVariant valueUnit();
};

/*! \brief Helper class for short detection and isolation test generation
 */
class mdtTtTestModelIsolationTestGeneratorHelper : public mdtTtTestModelAbstractGeneratorHelper
{
 public:

  /*! \brief Constructor
   */
  mdtTtTestModelIsolationTestGeneratorHelper(mdtTtTestModel *tm);

  /*! \brief Destructor
   */
  ~mdtTtTestModelIsolationTestGeneratorHelper();

  /*! \brief Set a list of test node unit ID of relays to not use
   */
   void setRelaysToIgnoreIdList(const QList<QVariant> & idList);

  /*! \brief Get a list of test node unit ID of relays to not use
   */
  QList<QVariant> relaysToIgnoreIdList();

  /*! \brief Get related connections
   *
   * Will get the DUT connection that is linked to given fromTestConnectionId,
   *  then search about not linked DUT connections.
   *  Finally, a list of test connection is built for each DUT connection found.
   *
   * \param testCableId ID of test cable (TestCable_tbl) to use.
   * \param fromTestConnectionId Test connection from witch to start.
   * \param graph Graph object. Cable list must allready be loaded (see mdtClPathGraph::loadLinkList())
   * \param ok Will be set to true on success, false else.
   */
  QList<QVariant> getRelatedTestConnectionIdList(const QVariant & testCableId, const QVariant & fromTestConnectionId, mdtClPathGraph & graph, bool & ok);

  /*! \brief Tell if a related test connection must be queued to solved connections once processed
   */
  bool queueRelatedConnectionToSolved();

  /*! \brief Return IgnoreCurrentItem
   */
  action_t actionOnShortInTestNode();

  /*! \brief Get expected value
   *
   * Return 0.0
   */
  QVariant expectedValue();

  /*! \brief Get value unit
   *
   * Return "Ohm"
   */
  QVariant valueUnit();

 private:

  QList<QVariant> pvRelaysToIgnoreIdList;
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
  mdtTtTestModelData getTestModelData(const QVariant & testModelId, bool *ok);

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

  /*! \brief Add a test item
   */
  ///bool addTestItem(const QVariant & testId, const QVariant & testLinkBusAId, const QVariant & testLinkBusBId, const QVariant & expectedValue);

  /*! \brief Get link ID for given cable ID and test connection ID
   */
  QVariant getTestLinkId(const QVariant & testCableId, const QVariant & testConnectionId, bool & ok);

  /*! \brief Get test model item identification
   */
  ///QVariant getTestModelItemDesignation(const QVariant & testLinkIdA, const QVariant & testLinkIdB);

  /*! \brief Get a list of test connections that are linked to given test connection, via DUT connection
   *
   * Will get the DUT connection that is linked to given fromTestConnectionId,
   *  then search about linked DUT connections.
   *  Finally, a list of test connection is built for each DUT connection found.
   *
   * \param testCableId ID of test cable (TestCable_tbl) to use.
   * \param fromTestConnectionId Test connection from witch to start.
   * \param graph Graph object. Cable list must allready be loaded (see mdtClPathGraph::loadLinkList())
   * \param ok Will be set to true on success, false else.
   */
  ///QList<QVariant> getTestConnectionIdListLinkedToTestConnectionByDutLinks(const QVariant & testCableId, const QVariant & fromTestConnectionId, mdtClPathGraph & graph, bool & ok);

  /*! \brief Generate test items for a continuity check (2 wire)
   *
   * \param testModelId ID of test model
   * \param testCableId ID of test cable (TestCable_tbl) to use.
   * \param testNodeId ID of test node to use.
   * \param measureConnexionIdA ID of first measure connection to use (TestNodeUnitConnection_tbl)
   * \param measureConnexionIdB ID of second measure connection to use (TestNodeUnitConnection_tbl)
   * \param graph Graph object. Cable list must allready be loaded (see mdtClPathGraph::loadLinkList())
   */
  bool generateContinuityTest(const QVariant & testModelId, const QVariant & testCableId, const QVariant & testNodeId,
                              const QVariant & measureConnexionIdA, const QVariant & measureConnexionIdB, mdtClPathGraph & graph);

  /*! \brief Generate test items for a short detection test
   *
   * \param testModelId ID of test model
   * \param testCableId ID of test cable (TestCable_tbl) to use.
   * \param testNodeId ID of test node to use.
   * \param measureConnexionIdA ID of first measure connection to use (TestNodeUnitConnection_tbl)
   * \param measureConnexionIdB ID of second measure connection to use (TestNodeUnitConnection_tbl)
   * \param graph Graph object. Cable list must allready be loaded (see mdtClPathGraph::loadLinkList())
   * \param relaysToIgnoreIdList List of test node unit ID of relays to not use
   */
  bool generateShortDetectionTest(const QVariant & testModelId, const QVariant & testCableId, const QVariant & testNodeId,
                                  const QVariant & measureConnexionIdA, const QVariant & measureConnexionIdB, mdtClPathGraph & graph,
                                  const QList<QVariant> & relaysToIgnoreIdList = QList<QVariant>());

  /*! \brief Generate test items
   *
   * \param testModelId ID of test model
   * \param testCableId ID of test cable (TestCable_tbl) to use.
   * \param testNodeId ID of test node to use.
   * \param measureConnexionIdA ID of first measure connection to use (TestNodeUnitConnection_tbl)
   * \param measureConnexionIdB ID of second measure connection to use (TestNodeUnitConnection_tbl)
   * \param graph Graph object. Cable list must allready be loaded (see mdtClPathGraph::loadLinkList())
   * \param helper Object for a specific test generation.
   */
  bool generateTestModel(const QVariant & testModelId, const QVariant & testCableId, const QVariant & testNodeId, 
                         const QVariant & measureConnexionIdA, const QVariant & measureConnexionIdB, 
                         mdtClPathGraph & graph, mdtTtTestModelAbstractGeneratorHelper & helper);

  /*! \brief Generate test node unit setup for given test ID
   * 
   * \todo Obselete ?
   */
  bool generateTestNodeUnitSetup(const QVariant & testId);

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
   */
  bool searchPathFromTestConnectionsToMeasureConnections(const QVariant & testConnectionId1, const QVariant & testConnectionId2,
                                                         const QVariant & measureConnexionIdA, const QVariant & measureConnexionIdB,
                                                         QList<QVariant> & pathConnectionIdList1, QList<QVariant> & pathConnectionIdList2,
                                                         mdtClPathGraph & graph);

  /*! \brief Get contact name of given ID
   */
  QString getUnitContactName(const QVariant & id);
  
  /*! \brief Search a path from test connection to measure connection
   *
   * \param testConnectionId ID of cconnection from witch to find a path
   * \param measureConnexionId ID of measure connection to witch to find path
   * \param pathConnectionIdList Reslut will be stored here
   * \param graph Graph object.
   */
  bool searchPathFromTestConnectionToMeasureConnection(const QVariant & testConnectionId, const QVariant & measureConnexionId, QList<QVariant> & pathConnectionIdList, mdtClPathGraph & graph);

  /*! \brief Search a path from test connection to measure connection A
   *
   * First, a path from testConnectionId to measureConnexionIdA is searched.
   *  If no path was found, measure connections A and B are swapped,
   *  and search begins a second time.
   *
   * \param testConnectionId ID of cconnection from witch to find a path
   * \param measureConnexionIdA ID of measure connection to witch to find path
   * \param measureConnexionIdB ID of other measure connection
   * \param pathConnectionIdList Reslut will be stored here
   * \param graph Graph object.
   */
  bool searchPathFromTestConnectionToMeasureConnection(const QVariant & testConnectionId, QVariant & measureConnexionIdA, QVariant & measureConnexionIdB, QList<QVariant> & pathConnectionIdList, mdtClPathGraph & graph);

  Q_DISABLE_COPY(mdtTtTestModel);
};

#endif // #ifndef MDT_TT_TEST_MODEL_H
