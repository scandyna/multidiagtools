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
#ifndef MDT_TT_TEST_NODE_EDITOR_H
#define MDT_TT_TEST_NODE_EDITOR_H

#include "mdtSqlForm.h"

#include <QSqlDatabase>
#include <QList>
#include <QLocale>

class QWidget;

/*! \brief This class provides a test node editor
 */
class mdtTtTestNodeEditor : public mdtSqlForm
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestNodeEditor(QWidget *parent, QSqlDatabase db);

  /*! \brief Setup tables
   */
  bool setupTables();

 public slots:

  /*! \brief Show test node MODBUS I/O tool
   */
  void showTestNodeModbusIoTool();

 private slots:

  /*! \brief Set base vehicle type
   */
  void setBaseVehicleType();

  /*! \brief Add a test node unit
   */
  void addUnit();

  /*! \brief Add test node units
   */
  ///void addUnits();

  /*! \brief Edit a node unit
   */
  void editUnit();

  /*! \brief Remove test node units
   */
  void removeUnits();

  /*! \brief Set I/O pos for a range of test node units
   */
  void setIoPositionRange();

  /*! \brief Update node unit connections
   *
   * Will add connections that are missing in TestNodeUnitConnection_tbl.
   */
  void updateConnections();

  /*! \brief Affect a unit connection to a bus
   */
  void setBusToUnitConnection();

  /*! \brief Remove a bus from unit connection
   */
  void removeBusFromUnitConnection();

 private:

  /*! \brief Select a TestNodeUnit type
   */
  QVariant selectTestNodeUnitType();

  /*! \brief Select a unit to use
   */
  QVariant selectUnitToUseAsTestNode(const QVariant & vehicleTypeId);

  /*! \brief Select a unit connection
   */
  QVariant selectUnitConnection(const QVariant & unitId);

  /*! \brief Select a list of unit IDs from given unit ID list
   */
  QList<QVariant> selectUnitIdList(const QList<QVariant> & unitIdList);

  /*! \brief Ask the user to give a BUS name
   */
  QVariant getBusName();

  /*! \brief Assign a unit connection as test connection to a unit
   *
   * Will search unit connections linked to given testNodeUnitId,
   *  starting from unit connections that are part of test node unit,
   *   but ignoring those given in busSideTestNodeUnitConnectionIdList .
   *
   * If more than one connection is found, the user cann choose witch one to use .
   *  \todo Selection is not implemented now .
   */
  ///bool assignTestConnectionToTestNodeUnit(const QVariant & testNodeUnitId, const QList<QVariant> & busSideTestNodeUnitConnectionIdList);

  /*! \brief Assign a unit connection as test connection to a list of units
   *
   * \sa assignTestConnectionToTestNodeUnit()
   */
  ///bool assignTestConnectionToTestNodeUnitLits(const QList<QVariant> & testNodeUnitIdList, const QList<QVariant> & busSideTestNodeUnitConnectionIdList);

  /*! \brief Setup TestNode table (main form)
   */
  bool setupTestNodeTable();

  /*! \brief Setup TestNodeUnit table
   */
  bool setupTestNodeUnitTable();

  /*! \brief Update TestNodeUnit table regarding language
   */
  void updateTestNodeUnitTable(const QLocale & locale);

  /*! \brief Setup TestNodeBus_tbl
   */
  bool setupTestNodeBusTable();

  /*! \brief Setup TestNodeUnitConnection_view
   */
  bool setupTestNodeUnitConnectionTable();

  Q_DISABLE_COPY(mdtTtTestNodeEditor);
};

#endif // #ifndef MDT_TT_TEST_NODE_EDITOR_H
