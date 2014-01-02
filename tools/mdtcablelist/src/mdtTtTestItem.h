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
#ifndef MDT_TT_TEST_ITEM_H
#define MDT_TT_TEST_ITEM_H

#include "mdtClBase.h"
#include <QVariant>
#include <QModelIndex>

/*! \brief Helper class for test item edition
 */
class mdtTtTestItem : public mdtClBase
{
 public:

  /*! \brief Coinstructor
   */
  mdtTtTestItem(QSqlDatabase db);

  /*! \brief Get a list of used node unit IDs for given test item ID
   *
   * If given type is not null, only matching test node unit will be returned
   */
  QList<QVariant> getUsedNodeUnitIdList(const QVariant & testItemId, const QVariant & type);

  /*! \brief Get list of test node unit setup for given test item ID
   */
  QList<QVariant> getTestNodeUnitSetupIdList(const QVariant & testItemId);

  /*! \brief Set test link
   */
  bool setTestLink(const QVariant & testItemId, const QVariant & testLinkBusAId, const QVariant & testLinkBusBId);

  /*! \brief Generate test node unit setup for given test item ID
   */
  bool generateTestNodeUnitSetup(const QVariant & testItemId);

  /*! \brief Add a test node unit setup
   */
  bool addTestNodeUnitSetup(const QVariant & testItemId, const QVariant & testNodeUnitId, const QVariant & state);

  /*! \brief Remove test node unit setup
   */
  bool removeTestNodeUnitSetup(const QVariant & tnusId);

  /*! \brief Remove test node unit setup
   */
  bool removeTestNodeUnitSetups(const QModelIndexList & indexListOfSelectedRows);

  /*! \brief Remove all test node unit setup for given test item ID
   */
  bool removeTestNodeUnitSetupsForTestItemId(const QVariant & testItemId);

 private:

  Q_DISABLE_COPY(mdtTtTestItem);
};

#endif // #ifndef MDTTTTESTITEM_H
