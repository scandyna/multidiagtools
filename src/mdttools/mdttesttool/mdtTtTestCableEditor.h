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
#ifndef MDT_TT_TEST_CABLE_EDITOR_H
#define MDT_TT_TEST_CABLE_EDITOR_H

#include "mdtSqlForm.h"
#include <QObject>
#include <QSqlDatabase>
#include <QVariant>
#include <QList>

class QWidget;
class mdtTtTestCableOffsetTool;

/*! \brief Class for physical test connection cable edition
 */
class mdtTtTestCableEditor : public mdtSqlForm
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestCableEditor(QWidget *parent, QSqlDatabase db);

  /*! \brief Setup all tables
   */
  bool setupTables();

  /*! \brief Add menus to menu bar
   */
  void addMenus(QMenuBar *menuBar);

 private slots:

  /*! \brief Assign a test node to current cable
   */
  void assignTestNode();

  /*! \brief Remove a assigned test node to current unit
   */
  void removeTestNodeAssignation();

  /*! \brief Add a connector based on a one from Connector_tbl to unit connector table
   */
  void addConnectorBasedConnector();

  /*! \brief Edit connector name
   */
  void editConnectorName();

  /*! \brief Remove a connector
   */
  void removeConnectors();

  /*! \brief Add a connection to unit connection table
   */
  void addConnection();

  /*! \brief Edit a connection
   */
  void editConnection();

  /*! \brief Remove connection from unit connection table
   */
  void removeConnections();

  /*! \brief Add a link
   */
  void addLink();

  /*! \brief Edit selected link
   */
  void editLink();

  /*! \brief Remove  links
   */
  void removeLinks();

  /*! \brief Add a logical test cable
   */
  void addLogicalTestCable();

  /*! \brief Edit a logical test cable
   */
  void editLogicalTestCable();

  /*! \brief Remove logical test cables
   */
  void removeLogicalTestCables();

  /*! \brief Select a test model for offset reset
   */
  void selectOffsetResetTestModel();

  /*! \brief Run offset reset
   */
  void runOffsetReset();

 private:

  /*! \brief Let user choose a connector (from Connector table)
   */
  QVariant selectBaseConnector();

  /*! \brief Let user select connections related to a connector from Connector_tbl
   */
  QList<QVariant> selectBaseConnectorContactIdList(const QVariant & connectorId, bool multiSelection = true);

  /*! \brief Get current unit ID
   */
  QVariant currentUnitId()
  {
    Q_ASSERT(mainTableController<mdtSqlDataWidgetController>());
    return mainTableController<mdtSqlDataWidgetController>()->currentData("Id_PK");
  }

  /*! \brief Setup test cable table
   *
   * Will also setup offset reset test model
   */
  bool setupTestCableTable();

  /*! \brief Setup test node table
   */
  bool setupTestNodeTable();

  /*! \brief Setup unit connector table
   */
  bool setupUnitConnectorTable();

  /*! \brief Setup unit connection table and widget
   */
  bool setupUnitConnectionTable();

  /*! \brief Setup unit link table
   */
  bool setupUnitLinkTable();

  /*! \brief Setup logical cable table
   */
  bool setupLogicalTestCableTable();

  Q_DISABLE_COPY(mdtTtTestCableEditor);

  mdtTtTestCableOffsetTool *pvCableOffsetTool;
};

#endif  // #ifndef MDT_TT_TEST_CABLE_EDITOR_H
