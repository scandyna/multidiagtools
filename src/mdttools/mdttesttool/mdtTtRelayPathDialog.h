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
#ifndef MDT_TT_RELAY_PATH_DIALOG_H
#define MDT_TT_RELAY_PATH_DIALOG_H

#include "ui_mdtTtRelayPathDialog.h"
#include "mdtError.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QVariant>
#include <QString>
#include <QList>

class mdtClPathGraph;
class QWidget;

/*! \brief Container for mdtTtRelayPathDialog
 */
struct mdtTtRelayPathItem
{
  /*! \brief Constructor
   */
  ///mdtTtRelayPathItem() : startConnectionId(0), endConnectionId(0) {}

  /*! \brief Check if item is null
   */
  inline bool isNull() const{ return (startConnectionId.isNull() || endConnectionId.isNull()); }

  QVariant startConnectionId;
  QVariant endConnectionId;
  QVariant testNodeUnitId;
  QString name;
};


/*! \brief Edit path from a starting connection to a end connection in a test node
 */
class mdtTtRelayPathDialog : public QDialog, Ui::mdtTtRelayPathDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtRelayPathDialog(QSqlDatabase db, mdtClPathGraph *pg, QWidget *parent = 0);

  /*! \brief Set test node ID
   */
  void setTestNodeId(const QVariant & id);

  /*! \brief Set data
   */
  void setData(const QVariant & testModelItemId, const QVariant & testNodeId);

  /*! \brief Get list if relays IDs to enable
   */
  QList<QVariant> idListOfRelaysToEnable() const;

  /*! \brief Get currently selected test connection
   */
  QVariant selectedTestConnection() const;

  /*! \brief Get currently selected measure connection
   */
  QVariant selectedMeasureConnection() const;

 private slots:

  /*! \brief Update source connections list regarding current source connector
   */
  void updateSourceConnections(int index);

  /*! \brief Update destination connections list regarding current source connector
   */
  void updateDestinationConnections(int index);

  /*! \brief Search path form source connection to destination connection
   */
  void searchPath(int index);

 private:

  /*! \brief Display test node related data
   */
  void displayTestNodeData(const QVariant & testNodeId);

  /*! \brief Populate the source test node unit combobox
   */
  bool populateSourceTestNodeUnitCombobox(const QVariant & testNodeId);

  /*! \brief Populate the source connection combobox
   */
  bool populateSourceConnectionCombobox(const QVariant & testNodeUnitId);

  /*! \brief Populate the destination test node unit combobox
   */
  bool populateDestinationTestNodeUnitCombobox(const QVariant & testNodeId);

  /*! \brief Populate the destination connection combobox
   */
  bool populateDestinationConnectionCombobox(const QVariant & testNodeUnitId);

  /*! \brief Load coupling relays
   */
  bool loadCouplingRelays(const QVariant & testNodeId);

  /*! \brief Load channel relays
   */
  bool loadChannelRelays(const QVariant & testNodeId);

  /*! \brief Get coupling relay that has given A and B connection
   */
  mdtTtRelayPathItem getCouplingRelay(const QVariant & A, const QVariant & B);

  /*! \brief Get channel relay that has given A and B connection
   */
  mdtTtRelayPathItem getChannleRelay(const QVariant & A, const QVariant & B);

  /*! \brief Get SQL statement to get test node unit data list
   */
  ///QString sqlForTestNodeUnitData(const QVariant & testNodeId) const;

  /*! \brief Get SQL statement to get connection data list
   */
  QString sqlForTestConnectionData(const QVariant & testNodeUnitId) const;

  /*! \brief Display a error
   */
  void displayError(const mdtError & error);

  Q_DISABLE_COPY(mdtTtRelayPathDialog);

  QSqlDatabase pvDatabase;
  mdtClPathGraph *pvGraph;
  QVariant pvTestModelItemId;
  QList<mdtTtRelayPathItem> pvCouplingRelays;
  QList<mdtTtRelayPathItem> pvChannelRelays;
  bool pvLoadingData;
  QList<QVariant> pvRelaysToEnableIds;
};

#endif  // #ifndef MDT_TT_RELAY_PATH_DIALOG_H
