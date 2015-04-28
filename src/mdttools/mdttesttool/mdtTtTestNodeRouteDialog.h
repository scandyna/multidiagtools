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
#ifndef MDT_TT_TEST_NODE_ROUTE_DIALOG_H
#define MDT_TT_TEST_NODE_ROUTE_DIALOG_H

#include "ui_mdtTtTestNodeRouteDialog.h"
#include "mdtTtTestNodeRouteData.h"
#include "mdtError.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QVariant>
#include <QString>
#include <QList>
#include <QMap>
#include <memory>

class mdtClPathGraph;
class QWidget;

/*! \brief Edit path from a starting connection to a end connection in a test node
 */
class mdtTtTestNodeRouteDialog : public QDialog, Ui::mdtTtTestNodeRouteDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestNodeRouteDialog(QSqlDatabase db, QWidget *parent = 0);

  /*! \brief Set test node ID
   *
   * Will also populate comboboxes with units of given test node.
   *
   * \param testNodeId Id (database primary key) of test node
   * \param graph Graph to use for path search.
   *               Link list must allready been loaded (see mdtClPathGraph::loadLinkList() )
   *               and (coupling and channel) relays of test node been added (see mdtTtTestNode::addRelaysToGraph() ).
   * \pre graph must be a valid pointer
   */
  void setTestNodeId(const QVariant & testNodeId, const std::shared_ptr<mdtClPathGraph> & graph);

  /*! \brief Get route data
   */
  mdtTtTestNodeRouteData routeData() const
  {
    return pvRouteData;
  }

  /*! \brief Set data
   */
//  void setData(const QVariant & testModelItemId, const QVariant & testNodeId);

  /*! \brief Get list if relays IDs to enable
   */
//   QList<QVariant> idListOfRelaysToEnable() const;

  /*! \brief Get currently selected test connection
   */
//   QVariant selectedTestConnection() const;

  /*! \brief Get currently selected measure connection
   */
//   QVariant selectedMeasureConnection() const;

 private slots:

  /*! \brief Update unit A list regarding current unit A type
   */
  void updateUnitA(int index);

  /*! \brief Update connections list of unit A regarding current unit A
   */
  void updateUnitAConnections(int index);

  /*! \brief Update unit B list regarding current unit A type
   */
  void updateUnitB(int index);

  /*! \brief Update connections list of unit B regarding current unit B
   */
  void updateUnitBConnections(int index);

  /*! \brief Search path form source connection to destination connection
   */
  void searchPath(int index);

 private:

  /*! \brief Display test node related data
   */
  void displayTestNodeData();

  /*! \brief Populate test node unit A type combobox
   */
  bool populateTestNodeUnitATypeCombobox();

  /*! \brief Populate test node unit A combobox
   */
  bool populateTestNodeUnitACombobox(const QString & unitType);

  /*! \brief Populate test node unit A connections combobox
   */
  bool populateConnectionACombobox(const QVariant & testNodeUnitId);

  /*! \brief Populate test node unit B type combobox
   */
  bool populateTestNodeUnitBTypeCombobox();

  /*! \brief Populate test node unit B combobox
   */
  bool populateTestNodeUnitBCombobox(const QString & unitType);

  /*! \brief Populate test node unit B connections combobox
   */
  bool populateConnectionBCombobox(const QVariant & testNodeUnitId);

  /*! \brief Load relays
   */
//   bool loadRelays(const QVariant & testNodeId);

  /*! \brief Load channel relays
   */
  ///bool loadChannelRelays(const QVariant & testNodeId);

  /*! \brief Get SQL statement to get unit type data list
   */
  QString sqlForUnitTypeData() const;

  /*! \brief Get SQL statement to get unit data list
   */
  QString sqlForUnitData(const QString & unitType) const;

  /*! \brief Get SQL statement to get connection data list
   */
  QString sqlForConnectionData(const QVariant & testNodeUnitId) const;

  /*! \brief Display a error
   */
  void displayError(const mdtError & error);

  Q_DISABLE_COPY(mdtTtTestNodeRouteDialog);

  QSqlDatabase pvDatabase;
  QVariant pvTestNodeId;
  mdtTtTestNodeRouteData pvRouteData;
  ///mdtClPathGraph *pvGraph;
  std::shared_ptr<mdtClPathGraph> pvGraph;
  ///QVariant pvTestModelItemId;
  bool pvLoadingData;
  QList<QVariant> pvRelaysToEnableIds;
  QMap<int, QString> pvRelayNameMap;
};

#endif  // #ifndef MDT_TT_TEST_NODE_ROUTE_DIALOG_H
