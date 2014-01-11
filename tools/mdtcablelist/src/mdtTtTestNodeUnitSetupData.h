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
#ifndef MDT_TT_TEST_NODE_UNIT_SETUP_DATA_H
#define MDT_TT_TEST_NODE_UNIT_SETUP_DATA_H

#include "mdtSqlRecord.h"
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlRecord>

/*! \brief Storage class for test node unit setup
 *
 * This class contains all fields present in TestNodeUnitSetup_tbl,
 *  and some additionnal fields from some linked tables.
 *  See mdtTtTestNodeUnitSetupData(const QSqlDatabase &) for details.
 */
class mdtTtTestNodeUnitSetupData : public mdtSqlRecord
{
 public:

  /*! \brief Build a empty mdtTtTestNodeUnitSetupData
   *
   * Will contain no fields.
   */
  mdtTtTestNodeUnitSetupData();

  /*! \brief Build a mdtTtTestNodeUnitSetupData with fields
   *
   * Will add all fields available in TestNodeUnitSetup_tbl.
   *
   * Following fields from Unit_tbl are alos added:
   *  - SchemaPosition
   *
   * Following fields from TestNodeUnit_tbl are alos added:
   *  - Bus
   *  - IoPosition
   *
   * Following fields from TestNode_tbl are also added:
   *  - NodeId  \todo Should be renamed HardwareNodeId
   */
  mdtTtTestNodeUnitSetupData(const QSqlDatabase & db);

  /*! \brief Build a mdtTtTestNodeUnitSetupData from a QSqlRecord
   */
  mdtTtTestNodeUnitSetupData(const QSqlRecord & record);

  /*! \brief Set test node unit setup ID
   */
  ///void setId(QVariant value);

  /*! \brief Get test node unit setup ID
   */
  ///QVariant id() const;

  /*! \brief Set state of node unit (ON/OFF)
   */
  ///void setState(QVariant value);

  /*! \brief Get state of node unit (ON/OFF)
   */
  ///QVariant state() const;

  /*! \brief Set value of node unit
   */
  ///void setValue(QVariant value);

  /*! \brief Get value of node unit
   */
  ///QVariant value() const;

  /*! \brief Set schema position of node unit
   */
  ///void setSchemaPosition(QVariant value);

  /*! \brief Get schema position of node unit
   */
  ///QVariant schemaPosition() const;

  /*! \brief Set node ID
   *
   * Note: this is NOT the database primary key,
   *  but NodeId field in TestNode_tbl .
   */
  ///void setNodeId(QVariant value);

  /*! \brief Get node ID
   *
   * Note: this is NOT the database primary key,
   *  but NodeId field in TestNode_tbl .
   */
  ///QVariant nodeId() const;

  /*! \brief Set test model item ID
   */
  ///void setTestModelItemId(QVariant value);

  /*! \brief Get test model item ID
   */
  ///QVariant testModelItemId() const;

  /*! \brief Set test item ID
   */
  ///void setTestItemId(QVariant value);

  /*! \brief Get test item ID
   */
  ///QVariant testItemId() const;

 private:

  ///QVariant pvId;
  /**
  QVariant pvState;
  QVariant pvValue;
  QVariant pvSchemaPosition;
  QVariant pvNodeId;
  QVariant pvTestModelItemId;
  QVariant pvTestItemId;
  */
};


#endif // #ifndef MDT_TT_TEST_NODE_UNIT_SETUP_DATA_H
