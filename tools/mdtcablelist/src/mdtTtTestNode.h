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
#ifndef MDT_TT_TEST_NODE_H
#define MDT_TT_TEST_NODE_H

#include "mdtClBase.h"
#include <QList>
#include <QVariant>
#include <QString>
#include <QModelIndex>

/*! \brief Helper class for test node edition
 */
class mdtTtTestNode : public mdtClBase
{
 public:

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

  /*! \brief Get a list of unit connection IDs that are linked (meaning wiered) to given unit connection ID
   */
  QList<QVariant> getIdListOfUnitConnectionsLinkedToUnitConnectionId(const QVariant & unitConnectionId);

  /*! \brief Get a list of unit IDs for given list of unit connection IDs
   */
  QList<QVariant> getIdListOfUnitIdForUnitConnectionIdList(const QList<QVariant> & unitConnectionIdList);

  /*! \brief Get a list of unit connection IDs that are part of given unit ID
   */
  QList<QVariant> getIdListOfUnitConnectionsPartOfUnit(const QVariant & unitId);

  /*! \brief Add a test node unit
   */
  bool addTestNodeUnit(const QVariant & UnitId, const QVariant & TestNodeId , const QVariant & typeCode, const QVariant & busName);

  /*! \brief Add a list of test node units
   */
  bool addTestNodeUnits(const QList<QVariant> & unitIdList, const QVariant & testNodeId , const QVariant & typeCode, const QVariant & busName);

  /*! \brief Remove test node units
   */
  bool removeTestNodeUnits(const QList<QVariant> & TestNodeIdList);

  /*! \brief Remove test node units
   */
  bool removeTestNodeUnits(const QModelIndexList & indexListOfSelectedRows);

  /*! \brief Set test connection to given test node unit
   */
  bool setTestConnection(const QVariant & testNodeUnitId, const QVariant & testConnectionId);
};

#endif // #ifndef MDT_TT_TEST_NODE_H
