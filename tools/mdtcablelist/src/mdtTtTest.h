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
#ifndef MDT_TT_TEST_H
#define MDT_TT_TEST_H

#include "mdtClBase.h"
#include <QVariant>
#include <QList>
#include <QModelIndex>

/*! \brief Helper class for test edition
 */
class mdtTtTest : public mdtClBase
{
 public:

  /*! \brief Constructor
   */
  mdtTtTest(QSqlDatabase db);

  /*! \brief Get a list of test item IDs for given test ID
   */
  QList<QVariant> getListOfTestItemIdListByTestId(const QVariant & testId);

  /*! \brief Get a list of node IDs (database keys) for given test ID
   */
  QList<QVariant> getListOfUsedNodeIdListByTestId(const QVariant & testId);

  /*! \brief Get a list of used node unit IDs for given test item ID
   *
   * If given type is not null, only matching test node unit will be returned
   */
  ///QList<QVariant> getListOfUsedNodeUnitIdListByTestItemId(const QVariant & testItemId, const QVariant & type);

  /*! \brief Get a list of unused node unit IDs for given test ID
   *
   * If given type is not null, only matching test node unit will be returned
   */
  QList<QVariant> getListOfUnusedNodeUnitIdListByTestId(const QVariant & testId, const QVariant & type);

  /*! \brief Add a test item
   */
  bool addTestItem(const QVariant & testId, const QVariant & testLinkBusAId, const QVariant & testLinkBusBId, const QVariant & expectedValue);

  /*! \brief Remove a test item
   */
  bool removeTestItem(const QVariant & testItemId);

  /*! \brief Remove test items
   */
  bool removeTestItems(const QModelIndexList & indexListOfSelectedRows);

  /*! \brief Add a test node unit setup
   */
  ///bool addTestNodeUnitSetup(const QVariant & testItemId, const QVariant & testNodeUnitId, const QVariant & state);

  /*! \brief Generate test node unit setup for given test item ID
   */
  ///bool generateTestNodeUnitSetupForTestItem(const QVariant & testItemId);

  /*! \brief Generate test node unit setup for given test ID
   */
  bool generateTestNodeUnitSetupForTest(const QVariant & testId);

  /*! \brief Remove test node unit setup
   */
  ///bool removeTestNodeUnitSetup(const QVariant & tnusId);

  /*! \brief Remove test node unit setup
   */
  ///bool removeTestNodeUnitSetups(const QModelIndexList & indexListOfSelectedRows);

 private:

  Q_DISABLE_COPY(mdtTtTest);
};

#endif // #ifndef MDT_TT_TEST_H
