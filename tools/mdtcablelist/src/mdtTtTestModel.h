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

#include "mdtClBase.h"
#include <QVariant>
#include <QList>
#include <QModelIndex>

/*! \brief Helper class for test edition
 */
class mdtTtTestModel : public mdtClBase
{
 public:

  /*! \brief Constructor
   */
  mdtTtTestModel(QSqlDatabase db);

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
   *  existing for given testId, and return the next number .
   */
  QVariant getNextSequenceNumber(const QVariant & testId);

  /*! \brief Get list of test node unit setup for given test ID
   */
  QList<QVariant> getTestNodeUnitSetupIdList(const QVariant & testId);

  /*! \brief Add a test item
   */
  bool addTestItem(const QVariant & testId, const QVariant & testLinkBusAId, const QVariant & testLinkBusBId, const QVariant & expectedValue);

  /*! \brief Remove a test item
   */
  bool removeTestItem(const QVariant & testItemId);

  /*! \brief Remove test items
   */
  bool removeTestItems(const QModelIndexList & indexListOfSelectedRows);

  /*! \brief Generate test node unit setup for given test ID
   */
  bool generateTestNodeUnitSetup(const QVariant & testId);

 private:

  Q_DISABLE_COPY(mdtTtTestModel);
};

#endif // #ifndef MDT_TT_TEST_MODEL_H
