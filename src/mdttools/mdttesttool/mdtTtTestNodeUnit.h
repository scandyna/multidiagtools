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
#ifndef MDT_TT_TEST_NODE_UNIT_H
#define MDT_TT_TEST_NODE_UNIT_H


#include "mdtTtTestNodeUnitData.h"
#include "mdtTtBase.h"
#include "mdtSqlTableSelection.h"
#include <QVariant>
#include <QList>
#include <QSqlDatabase>
#include <QString>
#include <QStringList>

class QObject;

/*! \brief Helper class for test node unit data manipulation
 */
class mdtTtTestNodeUnit : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtTtTestNodeUnit(QObject *parent, const QSqlDatabase & db);

  /*! \brief Constructor
   */
  mdtTtTestNodeUnit(const QSqlDatabase & db);

  /*! \brief Get SQL statement for test connection selection
   */
  QString sqlForTestConnectionSelection(const QList<QVariant> connectionIdList) const;

  /*! \brief Get connection ID by test node alias, schema position and contact name
   */
  QVariant getConnectionId(const QString & testNodeAlias, const QString & schemaPosition, const QString & contactName, bool & ok);

  /*! \brief Get a list of unit connection IDs for given unit ID
   */
  QList<QVariant> getConnectionIdListOfUnitId(const QVariant & unitId, bool excludeAllreadyAffectedToTestNode, bool  & ok);

  /*! \brief Get unit connection IDs that are linked to given unit connection IDs
   *
   * Will return IDs that not currently exists in TestNodeUnitConnection_tbl.
   */
  QList<QVariant> getConnectionIdListLinkedToConnectionIdList(const QList<QVariant> connectionIdList, bool *ok);

  /*! \brief Get unit data
   */
  mdtSqlRecord getUnitData(const QVariant & unitId, bool *ok);

  /*! \brief Get test node unit data
   */
  mdtTtTestNodeUnitData getData(const QVariant & nodeUnitId, bool *ok, bool includeTestConnectionData);

  /*! \brief Add a test node unit
   */
  bool addNodeUnit(const mdtTtTestNodeUnitData & data);

  /*! \brief Edit a test node unit
   */
  bool editNodeUnit(const QVariant & nodeUnitId, const mdtTtTestNodeUnitData & data);

  /*! \brief Remove a test node unit
   *
   * Will also remove related connections from TestNodeUnitConnection_tbl
   */
  bool removeNodeUnit(const QVariant & nodeUnitId, bool handleTransaction);

 /*! \brief Remove test node units
   *
   * Will also remove related connections from TestNodeUnitConnection_tbl
   */
  bool removeNodeUnits(const mdtSqlTableSelection & s);

  /*! \brief Add a connection
   */
  bool addConnection(const QVariant & unitConnectionId, const QVariant & testNodeUnitId, const QVariant & testNodeBusId);

  /*! \brief Add a list of connections
   */
  bool addConnections(const QList<QVariant> & unitConnectionIdList, const QVariant & testNodeUnitId, const QVariant & testNodeBusId, bool handleTransaction);

  /*! \brief Add connections that are related to given test node unit
   */
  bool addConnections(const QVariant & testNodeUnitId, const QVariant & testNodeBusId, bool handleTransaction);

  /*! \brief Get a list of test links on witch depends on given test node unit connection
   */
  QStringList getTestLinksDependingOnConnection(const QVariant & testNodeUnitConnectionId, bool & ok);

  /*! \brief Remove a test node unit connection from TestNodeUnitConnection_tbl
   */
  bool removeConnection(const QVariant & unitConnectionId);

  /*! \brief Remove given test node unit connections from TestNodeUnitConnection_tbl
   */
  bool removeConnections(const QList<QVariant> & unitConnectionIdList, bool handleTransaction);

  /*! \brief Remove connections for given test node unit ID
   */
  bool removeConnections(const QVariant & testNodeUnitId, bool handleTransaction);

  /*! \brief Set bus ID to given connection ID
   */
  bool setBusIdToConnection(const QVariant & unitConnectionId, const QVariant & busId);

  /*! \brief Set bus ID to given connection ID list
   */
  bool setBusIdToConnections(const QList<QVariant> & unitConnectionIdLits, const QVariant & busId);

 private:

  Q_DISABLE_COPY(mdtTtTestNodeUnit);
};

#endif // #ifndef MDT_TT_TEST_NODE_UNIT_H
