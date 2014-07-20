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

/*! \brief Helper class for test edition
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

  /*! \brief Add a test item
   */
  bool addTestItem(const QVariant & testId, const QVariant & testLinkBusAId, const QVariant & testLinkBusBId, const QVariant & expectedValue);

  /*! \brief Remove a test item
   */
  bool removeTestItem(const QVariant & testItemId);

  /*! \brief Remove test items
   */
  bool removeTestItems(const QModelIndexList & indexListOfSelectedRows);

  /*! \brief Generate test items for a continuity check (2 wire)
   *
   *
   * \param testCableId ID of test cable (TestCable_tbl) to use.
   * \param measureConnexionIdA ID of first measure connection to use (TestNodeUnitConnection_tbl)
   * \param measureConnexionIdB ID of second measure connection to use (TestNodeUnitConnection_tbl)
   * \param graph Graph object. Cable list must allready be loaded (see mdtClPathGraph::loadLinkList())
   */
  bool generateContinuityTest(const QVariant & testCableId, const QVariant & measureConnexionIdA, const QVariant & measureConnexionIdB, mdtClPathGraph & graph);

  /*! \brief Generate test node unit setup for given test ID
   * 
   * \todo Obselete ?
   */
  bool generateTestNodeUnitSetup(const QVariant & testId);

 private:

  Q_DISABLE_COPY(mdtTtTestModel);
};

#endif // #ifndef MDT_TT_TEST_MODEL_H
