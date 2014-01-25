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
#ifndef MDT_CL_UNIT_H
#define MDT_CL_UNIT_H

///#include "mdtClUnitConnectionData.h"
#include "mdtClUnitConnectorData.h"
#include "mdtClLinkData.h"
#include "mdtTtBase.h"
#include "mdtError.h"
#include "mdtSqlRecord.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QList>
#include <QModelIndex>

/*! \brief Helper class to mana unit and related data
 */
class mdtClUnit : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtClUnit(QObject *parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClUnit();

  /*! \brief Get SQL statement for article connector selection
   *
   * List all article connectors for given articleId wich are not
   *  used by given unitId .
   */
  QString sqlForArticleConnectorSelection(const QVariant & articleId, const QVariant & unitId) const;

  /*! \brief Get SQL statement for article connection selection
   *
   * List all article connections for a given article connector ID,
   *  and that are not used by a given unit ID .
   */
  QString sqlForArticleConnectionLinkedToArticleConnectorSelection(const QVariant & articleConnectorId, const QVariant & unitId) const;

  /*! \brief Get SQL statement for article connection selection
   *
   * List all article connections for a given article ID,
   *  and that are not used by a given unit ID .
   */
  QString sqlForArticleConnectionLinkedToArticleSelection(const QVariant & articleId, const QVariant & unitId) const;

  /*! \brief Get SQL statement for article connection linked to a unit connector selection
   *
   * List all article connections that are related to given unit connector,
   *  and that are not allready used in given unit .
   */
  QString sqlForArticleConnectionLinkedToUnitConnectorSelection(const QVariant & unitConnectorId, const QVariant & unitId) const;

  /*! \brief Get unit connection data for given unit connection ID
   *
   * If given unit connection is based on a article connection,
   *  the article connection data part will also be set.
   */
  mdtClUnitConnectionData getConnectionData(const QVariant & unitConnectionId, bool *ok);

  /*! \brief Get connector contact data (from ConnectorContact_tbl) for a given contact ID
   * \brief Get (unit) connection data from connector contact data (ConnectorContact_tbl)
   *
   * 
   */
  ///mdtClUnitConnectionData getBaseConnectorContactData(const QVariant & contactId, mdtClUnitConnectionData data = mdtClUnitConnectionData());
  ///mdtSqlRecord getConnectionDataFromConnectorContact(const QVariant & connectorContactId, bool *ok, mdtSqlRecord data = mdtSqlRecord());

  /*! \brief Get unit connection data for given unit connection ID
   *
   * Will also get unit connector data and article connector + connection data if unit connection is based on article connection .
   */
  ///mdtClUnitConnectionData getConnectionDataByUnitConnectionId(const QVariant & unitConnectionId);

  /*! \brief Get unit connection data for given article connection ID and unit ID
   *
   * \sa getConnectionDataByUnitConnectionId()
   */
  ///mdtClUnitConnectionData getConnectionDataByArticleConnectionId(const QVariant & articleConnectionId, const QVariant & unitId);

  /*! \brief Get article connection data
   *
   * Will get article connection data and article connector data .
   */
  ///mdtClUnitConnectionData getArticleConnectionData(const QVariant & articleConnectionId, mdtClUnitConnectionData data = mdtClUnitConnectionData());

  /*! \brief Get unit connector data for given unit connector ID
   */
  ///mdtClUnitConnectionData getUnitConnectorData(const QVariant & unitConnectorId, mdtClUnitConnectionData data = mdtClUnitConnectionData());

  /*! \brief Get article connector data for given article connector ID
   */
  ///mdtClUnitConnectionData getArticleConnectorData(const QVariant & articleConnectorId, mdtClUnitConnectionData data = mdtClUnitConnectionData());

  /*! \brief Get unit connector data for a given article connector ID and unit ID
   *
   * If given unit has no connector based on given article connector,
   *  data will not be updated
   */
  ///mdtClUnitConnectionData getUnitConnectorDataByArticleConnectorId(const QVariant & articleConnectorId, const QVariant & unitId, mdtClUnitConnectionData data = mdtClUnitConnectionData());

  /*! \brief Get a model with units for compononent selection
   *
   * Will setup a query model witch contains all units except unitId
   *  and units that allready are component of unitId .
   */
  QSqlQueryModel *unitModelForComponentSelection(const QVariant &unitId);

  /*! \brief Add a unit as component
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool addComponent(const QVariant &unitId, const QVariant &componentId);

  /*! \brief Remove a single component
   */
  bool removeComponent(const QVariant &componentId);

  /*! \brief Remove a list of components
   */
  bool removeComponents(const QList<QVariant> &componentIdList);

  /*! \brief Remove each component that is contained in selection
   *
   * This is usefull used together with mdtSqlTableWidget .
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeComponents(const QModelIndexList & indexListOfSelectedRows);

  /*! \brief Return the number unit connections that are related to article connections
   *
   * \return A number >= 0 on success, a value < 0 on error.
   *         On error, use lastError() to check what goes wrong.
   */
  int toUnitRelatedArticleConnectionCount(const QVariant & unitId);

  /*! \brief
   */
  QSqlQueryModel *toUnitConnectionRelatedRangesModel(const QVariant & unitConnectionId);

  /*! \brief Get a model that contains links that are related to given unit ID and a list of unit connection IDs
   */
  QSqlQueryModel *toUnitRelatedLinksModel(const QVariant &unitId, const QList<QVariant> &unitConnectionIdList);

  /*! \brief Get a list of links that are related to given unit ID
   *
   * This is a helper method to display a message to the user in a simple way.
   *  Do not use the result for technical processing.
   */
  QStringList toUnitRelatedLinksList(const QVariant &unitId, const QList<QVariant> &unitConnectionIdList);

  /*! \brief Get a list of links that are related to given unit ID
   *
   * This is a helper method to display a message to the user in a simple way.
   *  Do not use the result for technical processing.
   */
  QString toUnitRelatedLinksListStr(const QVariant &unitId, const QList<QVariant> &unitConnectionIdList);

  /*! \brief Get a list of links that are related to given unit ID
   *
   * This is a helper method to display a message to the user in a simple way.
   *  Do not use the result for technical processing.
   */
  QString toUnitRelatedLinksListStr(const QVariant &unitId, const QModelIndexList & indexListOfSelectedRows);


  /*! \brief Add a unit connector
   *
   * Note: will only add connector, not the connections .
   */
  bool addConnector(const QVariant & unitId, const QVariant & baseConnectorId, const QVariant & articleConnectorId, const QVariant & name);

  /*! \brief Add a unit connector and a list of connections
   */
  ///bool addConnector(const QVariant & unitId, const QVariant & baseConnectorId, const QVariant & articleConnectorId, const QVariant & name, const QList<mdtClUnitConnectionData> connectionList);

  /*! \brief Remove a unit connector
   *
   * Note: will only remove unit connector
   */
  bool removeConnector(const QVariant & unitConnectorId);

  /*! \brief Add a unit connection
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  ///bool addUnitConnection(const mdtClUnitConnectionData & data);

  /*! \brief Edit a unit connection
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  ///bool editUnitConnection(const mdtClUnitConnectionData & data);

  /*! \brief Remove a single unit connection
   */
  bool removeUnitConnection(const QVariant & unitConnectionId);

  /*! \brief Remove a list of unit connections
   */
  bool removeUnitConnections(const QList<QVariant> &unitConnectionIdList);

  /*! \brief Remove each unit connection that is contained in selection
   *
   * This is usefull used together with mdtSqlTableWidget .
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeUnitConnections(const QModelIndexList & indexListOfSelectedRows);

  /*! \brief
   */
  bool addRange(const QVariant & baseUnitConnectionId, const mdtClUnitConnectionData & rangeData);

  /*! \brief Get a data object for a given link
   */
  mdtClLinkData getUnitLinkData(const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId, bool *ok);

  /*! \brief Add a unit link
   */
  bool addLink(const mdtClLinkData &data);

  /*! \brief Edit a unit link
   */
  bool editLink(const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId, const mdtClLinkData &data);

  /*! \brief Remove a unit link
   *
   * Will also remove all vehicle type related links
   */
  bool removeLink(const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId);

  /*! \brief Remove each unit link that is contained in selection
   */
  bool removeLinks(const QList<QModelIndexList> &indexListOfSelectedRowsByRows);

 private:

  /*! \brief Get unit connection data and article connection data if based on
   *
   * \pre Given SQL statement must return 1 unit connection row
   * \pre ok must be valid
   */
  mdtClUnitConnectionData getConnectionDataPv(const QString & sql, bool *ok);

  /*! \brief Add link to vehicle type table
   */
  bool addLinkToVehicleType(const QVariant &vehicleTypeStartId, const QVariant &vehicleTypeEndId, const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId);

  /*! \brief Remove a link from vehicle type table
   */
  bool removeLinkFromVehicleType(const QVariant &vehicleTypeStartId, const QVariant &vehicleTypeEndId, const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId, QSqlQuery &query);

  /*! \brief Fill unit connection data part
   *
   * This method is used by getConnectionDataByUnitConnectionId() and getConnectionDataByArticleConnectionId() .
   */
  ///bool fillUnitConnectionDataPart(mdtClUnitConnectionData & data, const QVariant & unitConnectionId);

  /*! \brief Fill unit connector data part
   *
   * This method is used by getConnectionDataByUnitConnectionId() and getConnectionDataByArticleConnectionId() .
   */
  ///bool fillUnitConnectorDataPart(mdtClUnitConnectionData & data, const QVariant & unitConnectorId);

  /*! \brief Fill article connection data part
   *
   * This method is used by getConnectionDataByUnitConnectionId() and getConnectionDataByArticleConnectionId() .
   */
  ///bool fillArticleConnectionDataPart(mdtClUnitConnectionData & data, const QVariant & articleConnectionId);

  /*! \brief Fill article connector data part
   *
   * This method is used by getConnectionDataByUnitConnectionId() and getConnectionDataByArticleConnectionId() .
   */
  ///bool fillArticleConnectorDataPart(mdtClUnitConnectionData & data, const QVariant & articleConnectorId);

  Q_DISABLE_COPY(mdtClUnit);

  QSqlQueryModel *pvToUnitConnectionRelatedRangesModel;
  QSqlQueryModel *pvUnitModel;
  QSqlQueryModel *pvUnitLinkModel;
};

#endif  // #ifndef MDT_CL_UNIT_H
