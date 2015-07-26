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
#ifndef MDT_CL_AUTO_CONNECTION_H
#define MDT_CL_AUTO_CONNECTION_H

#include "mdtTtBase.h"
#include "mdtClLinkData.h"
#include "mdtClLinkVersionKeyData.h"
#include "mdtClModificationKeyData.h"
#include "mdtClVehicleTypeLinkKeyData.h"
#include "mdtClConnectorData.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClUnitConnectorData.h"
#include "mdtClConnectableCriteria.h"
#include <QSqlDatabase>
#include <QList>
#include <QVariant>
#include <QString>

/*! \brief Generate links to make a connection
 */
class mdtClAutoConnection : public mdtTtBase
{
  friend class mdtClAutoConnectionTest;

 public:

  /*! \brief Constructor
   */
  mdtClAutoConnection(QSqlDatabase db);

  /*! \brief Check if unit connection a can be connected with unit connection b
   *
   * \pre a and b muts have connection type set
   */
  bool canConnectConnections(const mdtClUnitConnectionData & a, const mdtClUnitConnectionData & b, const mdtClConnectableCriteria & criteria);

  /*! \brief Check if unit connection a can be connected with unit connection b
   */
  bool canConnectConnections(const mdtClUnitConnectionPkData & pkA, const mdtClUnitConnectionPkData & pkB, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Get a list of unit connections that can be connected to given one
   *
   * \todo Also fink about versions + vehicle types assignations !!
   */
  QList<mdtClUnitConnectionPkData> getConnectableConnectionPkList(const mdtClUnitConnectionPkData & toPk, 
                                                                const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Get a list of unit connections that can be connected to given one
   *
   * This version will limit result to connections that belongs to given unit.
   *
   * \todo Also fink about versions + vehicle types assignations !!
   */
  QList<mdtClUnitConnectionPkData> getConnectableConnectionPkList(const mdtClUnitConnectionPkData & toPk, const QVariant & unitId, 
                                                                const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Check if connector a can be connected with connector b
   */
  bool canConnectConnectors(const mdtClConnectorData & a, const mdtClConnectorData & b, const mdtClConnectableCriteria & criteria);

  /*! \brief Check if connector a can be connected with connector b
   */
  bool canConnectConnectors(const mdtClConnectorPkData & pkA, const mdtClConnectorPkData & pkB, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Check if unit connector a can be connected with unit connector b
   */
  bool canConnectConnectors(const mdtClUnitConnectorData & a, const mdtClUnitConnectorData & b, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Check if unit connector a can be connected with unit connector b
   */
  bool canConnectConnectors(const mdtClUnitConnectorPkData & pkA, const mdtClUnitConnectorPkData & pkB, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Get a list of unit connectors that can be connected to given one
   *
   * This version will limit result to given unit list.
   *
   * \todo Also fink about versions + vehicle types assignations !!
   */
  QList<mdtClUnitConnectorPkData> getConnectableConnectorPkList(const mdtClUnitConnectorPkData & toPk, const QList<QVariant> & unitIdList,
                                                                const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Connect a and b connector by contact name
   *
   * Will create a link of type Connection for each
   *  connection from a connector to a connection of b connector
   *  that can be connected regarding given criteria.
   *
   * For each link that was found, following fields are also set:
   *  - PK : its connectionStartId and connectionEndId will be set with ID of a and b (order is depending of contacts count of each connector, and is also not defined)
   *  - linkType : will be set to Connection
   *  - linkDirection : will be set to Bidirectionnal
   */
  bool connectByContactName(const mdtClUnitConnectorData & a, const mdtClUnitConnectorData & b,
                            const mdtClLinkVersionPkData & versionPk, const mdtClModificationPkData & modificationPk,
                            const QList<mdtClVehicleTypeStartEndKeyData> & vtList,
                            const mdtClConnectableCriteria & criteria, bool handleTransaction);

  /*! \brief Disconnect connector a from b
   *
   * Will also remove vehicle type assignations and history for each links that are concerned.
   */
  bool disconnectConnectors(const mdtClUnitConnectorPkData & pkA, const mdtClUnitConnectorPkData & pkB);

 private:

  /*! \brief Get a list of connection links between a and b connections, by name
   *
   * Will search connections that have the same name, and that can be connected together, in a and b list
   *  regarding given criteria.
   *
   * This method is typically used to create connections links between 2 connectors (i.e. connect them).
   *
   * If connectionLinkDataList pointer is not null, a list of found links is also created.
   *  In this case, for each link that was found, following fields are also set:
   *  - PK : its connectionStartId and connectionEndId will be set with ID of a and b (order is depending of size of each list, and is also not defined)
   *  - linkType : will be set to Connection
   *  - linkDirection : will be set to Bidirectionnal
   */
  bool checkOrBuildConnectionLinkListByName(const QList<mdtClUnitConnectionData> & a, const QList<mdtClUnitConnectionData> & b, 
                                            const mdtClConnectableCriteria & criteria, QList<mdtClLinkData> *connectionLinkDataList = nullptr,
                                            const mdtClLinkVersionPkData *versionPk = nullptr, const mdtClModificationPkData *modificationPk = nullptr);

  Q_DISABLE_COPY(mdtClAutoConnection)
};

#endif // #ifndef MDT_CL_AUTO_CONNECTION_H
