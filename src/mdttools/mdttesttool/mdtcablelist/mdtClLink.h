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
#ifndef MDT_CL_LINK_H
#define MDT_CL_LINK_H

#include "mdtTtBase.h"
#include "mdtClLinkData.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClUnitConnectorData.h"
#include "mdtSqlTableSelection.h"
#include <QSqlDatabase>
#include <QList>
#include <QVariant>
#include <QString>
#include <QModelIndex>


/*! \brief Criteria that defines if 2 connectors are connectable
 *
 * 
 */
struct mdtClConnectableCriteria
{
  /*! \brief Contruct a default criteria struct.
   *
   * Will set parameters as follow:
   *  - checkGenderAreOpposite: true
   *  - checkContactCount: true
   *  - checkContactType: true
   *  - checkContactName: true
   *  - checkForm: true
   *  - checkInsert: true
   *  - checkInsertRotation: true
   */
  mdtClConnectableCriteria()
   : checkGenderAreOpposite(true),
     checkContactCount(true),
     checkContactType(true),
     checkContactName(true),
     checkForm(true),
     checkInsert(true),
     checkInsertRotation(true)
  {
  }

  /*! \brief Gender must be opposite
   *
   * If true, gender of both connectors must be opposite (male and female)
   *  Note: this concerns the connector gender, not necessarely the gender contacts.
   *   See checkContactType for contact gender checking.
   */
  bool checkGenderAreOpposite;

  /*! \brief Contact quantity must be the same
   *
   * If true, it will be checked that both connectors have the same quantity of contacts.
   */
  bool checkContactCount;

  /*! \brief Contact gender must be compatible
   *
   * If true, each contact gender are checked:
   *  - A socket (S) can be connected to a pin (P) , and reverse (P to T)
   *  - Terminal (T) can be connected to a terminal (T)
   */
  bool checkContactType;

  /*! \brief Contact name must match (be the same)
   */
  bool checkContactName;

  /*! \brief Form must match for both connectors
   *
   * If true, it will be check that both connectors have the same form
   */
  bool checkForm;

  /*! \brief Insert must match for both connector
   *
   * If true, it will be checked that both connectors have the same insert (f.ex. 24-5)
   */
  bool checkInsert;

  /*! \brief Insert rotation code must match for both connector
   *
   * If true, it will be checked that both connectors have the same insert rotation code (Q, X, W, ...)
   */
  bool checkInsertRotation;
};

/*! \brief Helper class for link management
 */
class mdtClLink : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtClLink(QObject* parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClLink();

  /*! \brief Build vehicle type link data based on given start and end vehicle ID lists
   *
   * The list is built regarding distinct cases:
   *  - If both start and end vehicle types lists contains exactly the same count of items,
   *     each start ID is put together with each end ID , in the order of both lists.
   *  - If one vehicle types list has only 1 item, and the other has many items,
   *     the ID of the 1 item list is copied to the other list
   *     (case of 1 vehicle type linked to many vehicle types) .
   *  - All other cases are wrong.
   */
  bool buildVehicleTypeLinkDataList(mdtClLinkData & linkData, const QList<QVariant> & vtStartIdList, const QList<QVariant> & vtEndIdList);

  /*! \brief Add a link
   *
   * Will also add required vehicle type links stored in linkData.
   *  To generate vehicle type links, use buildVehicleTypeLinkDataList().
   */
  bool addLink(const mdtClLinkData & linkData, bool handleTransaction = true);

  /*! \brief Add a list of links
   *
   * \sa addLink().
   */
  bool addLinks(const QList<mdtClLinkData> & linkDataList, bool handleTransaction = true);

  /*! \brief Check if a link exists
   */
  bool linkExists(const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId, bool *ok);

  /*! \brief Get link data
   */
  mdtClLinkData getLinkData(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId, bool includeConnectionData, bool includeVehicleTypeLinkData , bool & ok);

  /*! \brief Edit a unit link
   *
   * If linkData has following criteria:
   *  - Flag hasValue set false for UnitConnectionStart_Id_FK
   *  - Flag hasValue set false for UnitConnectionEnd_Id_FK
   *  - Flag vehicleTypeLinksEdited not set
   *
   * then, data are only updated in database.
   * For other cases, record will be removed then created again
   *  (this is due to complexity because of handling vehicle type links).
   */
  bool editLink(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId, const mdtClLinkData & linkData);

  /*! \brief Remove a unit link
   *
   * Will also remove all vehicle type related links
   */
  bool removeLink(const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId, bool handleTransaction = true);

  /*! \brief Remove each unit link that is contained in selection
   */
  bool removeLinks(const mdtSqlTableSelection & s);

  /*! \brief Get vehicle type link data for given unit ID
   */
  QList<mdtClVehicleTypeLinkData> getVehicleTypeLinkDataByUnitId(const QVariant & unitId, bool *ok);

  /*! \brief Remove vehicle type links for given unit ID
   */
  bool removeVehicleTypeLinkByUnitId(const QVariant & unitId, bool handleTransaction);

  /*! \brief Remove vehicle type links
   */
  bool removeVehicleTypeLinks(const QList<mdtClVehicleTypeLinkData> & vtLinkList, bool handleTransaction);

  /*! \brief Get a list of connection links between A and B connections, by name
   *
   * Will search connections that have the same name, and that can be connected together, in A and B list.
   *  Currently, connections can be connected if item of A is a pin and item of B is a socket,
   *  or in reverse case.
   *
   * This method is typically used to create connections links between 2 connectors (i.e. connect them).
   *
   * For each link that was found, following fields are also set:
   *  - UnitConnectionStart_Id_FK : ID of item in A
   *  - UnitConnectionEnd_Id_FK : ID of item in B
   *  - LinkType_Code_FK : CONNECTION
   *  - LinkDirection_Code_FK : BID
   *
   * Note: this method is mainly used for unit testing. Consider following helper functions:
   *  - canConnectConnections()
   *  - canConnectConnectors()
   *  - sqlForConnectableUnitConnectorsSelection()
   *  - connectByContactName()
   */
  QList<mdtClLinkData> getConnectionLinkListByName(const QList<mdtClUnitConnectionData> & A, const QList<mdtClUnitConnectionData> & B, const mdtClConnectableCriteria & criteria);

  /*! \brief Check if unit connection S can be connected with unit connection E
   *
   * \pre Following fields must be set (not null) for S and E:
   *    - ConnectionType_Code_FK
   */
  bool canConnectConnections(const mdtClUnitConnectionData & S, const mdtClUnitConnectionData & E, const mdtClConnectableCriteria & criteria);

  /*! \brief Check if unit connection ID A can be connected with unit connection ID B
   */
  bool canConnectConnections(const QVariant & unitConnectionIdA, const QVariant & unitConnectionIdB, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Check if unit connector S can be connected with unit connector E
   */
  bool canConnectConnectors(const mdtClUnitConnectorData & S, const mdtClUnitConnectorData & E, const mdtClConnectableCriteria & criteria);

  /*! \brief Check if unit connector start can be connected with unit connector end
   */
  bool canConnectConnectors(const QVariant & startUnitConnectorId, const QVariant & endUnitConnectorId, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Get SQL statement for connectable unit connection selection
   *
   * SQL statement will select unit connections that can be connected
   *  to given unitConnectionId.
   *  If given unit ID is not null, only connections related to this unit will be included.
   *
   * \return SQL statement, or empty string on error.
   */
  QString sqlForConnectableUnitConnectionsSelection(const QVariant & unitConnectionId, const QVariant & unitId, const mdtClConnectableCriteria & criteria);

  /*! \brief Get SQL statement for connectable unit connector selection
   *
   * SQL statement will select unit connectors that can be connected
   *  to given unitConnectorId.
   *  If given unit ID is not null, only connectors related to this unit will be included.
   *
   * \return SQL statement, or empty string on error.
   */
  QString sqlForConnectableUnitConnectorsSelection(const QVariant & unitConnectorId, const QVariant & unitId, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Get SQL statement for connectable unit connector selection
   *
   * SQL statement will select unit connectors that can be connected
   *  to given unitConnectorId.
   *  If given unit ID list contains IDs, only connectors related to this units will be included.
   *
   * \return SQL statement, or empty string on error.
   */
  QString sqlForConnectableUnitConnectorsSelection(const QVariant & unitConnectorId, const QList<QVariant> & unitIdList, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Get SQL statement for connectable unit connector's connections selection
   *
   * SQL statement will select unit connection for connectors that can be connected
   *  to given unitConnectorId.
   *  If given unit ID is not null, only connections related to this unit will be included.
   *
   * \return SQL statement, or empty string on error.
   */
  ///QString sqlForConnectableUnitConnectorConnectionsSelection(const QVariant & unitConnectorId, const QVariant & unitId, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Connect start and end connector by contact name
   *
   * Will create a link of type CONNECTION for each
   *  connection from start connector to a connection of end connector
   *  that can be connected regarding given criteria.
   *
   * For each link that was found, following fields are also set:
   *  - UnitConnectionStart_Id_FK : ID of item in S
   *  - UnitConnectionEnd_Id_FK : ID of item in E
   *  - LinkType_Code_FK : CONNECTION
   *  - LinkDirection_Code_FK : BID
   */
  bool connectByContactName(const mdtClUnitConnectorData & S, const mdtClUnitConnectorData & E,
                            const QVariant & startVehicleTypeId, const QVariant & endVehicleTypeId,
                            const mdtClConnectableCriteria & criteria);

  /*! \brief Connect start and end connector by contact name
   *
   * Will create a link of type CONNECTION for each
   *  connection from start connector to a connection of end connector
   *  that can be connected. See getConnectionLinkListByName() for details.
   */
  bool connectByContactName(const QVariant & startUnitConnectorId, const QVariant & endUnitConnectorId,
                            const QVariant & startVehicleTypeId, const QVariant & endVehicleTypeId,
                            const mdtClConnectableCriteria & criteria);

  /*! \brief Disconnect connector S from E
   *
   * \todo In current version, vehicle type list are not token in acount. This will be the case once removeLink() is fixed.
   */
  bool disconnectConnectors(const QVariant & startUnitConnectorId, const QVariant & endUnitConnectorId, const QList<QVariant> & startVehicleTypeIdList, const QList<QVariant> & endVehicleTypeIdList);

 private:

  /*! \brief Get a list of connection links between A and B connections, by name
   *
   * Will search connections that have the same name, and that can be connected together, in A and B list
   *  regarding given criteria.
   *
   * This method is typically used to create connections links between 2 connectors (i.e. connect them).
   *
   * If connectionLinkDataList pointer is not null, a list of found links is also created.
   *  In this case, for each link that was found, following fields are also set:
   *  - UnitConnectionStart_Id_FK : ID of item in A
   *  - UnitConnectionEnd_Id_FK : ID of item in B
   *  - LinkType_Code_FK : CONNECTION
   *  - LinkDirection_Code_FK : BID
   */
  bool checkOrBuildConnectionLinkListByName(const QList<mdtClUnitConnectionData> & A, const QList<mdtClUnitConnectionData> & B, 
                                            const mdtClConnectableCriteria & criteria, QList<mdtClLinkData> *connectionLinkDataList = 0);

  /*! \brief Check vehicle type start and end ID lists
   */
  bool checkVehicleTypeStartEndIdLists(const QList<QVariant> & vtStartIdList, const QList<QVariant> & vtEndIdList);

  /*! \brief Add link to vehicle type table
   */
  bool addLinkToVehicleType(const mdtClVehicleTypeLinkData & data);

  /*! \brief Add a list of links to vehicle type table
   */
  bool addLinkToVehicleTypeList(const QList<mdtClVehicleTypeLinkData> & dataList);

  /*! \brief Get start and end connection data
   *
   * Note: will set start and end connection data in given link data
   */
  bool getConnectionData(mdtClLinkData & linkData, const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId);

  /*! \brief Get data from vehicle type link table
   *
   * Note: will fill list in given link data
   */
  bool getVehicleTypeLinkData(mdtClLinkData & linkData, const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId);

  Q_DISABLE_COPY(mdtClLink);
};

#endif // MDT_CL_LINK_H
