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
#ifndef MDT_CL_UNIT_CONNECTION_H
#define MDT_CL_UNIT_CONNECTION_H

#include "mdtClArticleConnection.h"
#include "mdtClUnitConnectionKeyData.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClUnitConnectorKeyData.h"
#include "mdtClUnitConnectorData.h"

/*! \brief Helper class for unit connection and unit connector manipulations
 *
 * Acts mainly on UnitConnection_tbl and UnitConnector_tbl
 */
class mdtClUnitConnection : public mdtClArticleConnection
{
 public:

  /*! \brief Constructor
   */
  mdtClUnitConnection(QObject *parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtClUnitConnection(QSqlDatabase db);

  /*! \brief Add a unit connection to database
   *
   * \param data Unit connection data to store
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   * \return Id_PK of added unit connection, or a null key on error
   */
  mdtClUnitConnectionKeyData addUnitConnection(const mdtClUnitConnectionData & data, bool handleTransaction);

  /*! \brief Add a list of unit connections to database
   *
   * \param dataList List of unit connections data to store
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool addUnitConnectionList(const QList<mdtClUnitConnectionData> & dataList, bool handleTransaction);

  /*! \brief Get unit connection data from database
   *
   * \param key Data that contains unit connection ID (Id_PK).
   *     Note: only id is used, other key memebers are ignored.
   * \return data for given key.
   *       A null data is returned if given key does not exist, or a error occured.
   *       Use ok parameter to diffrenciate both cases.
   */
  mdtClUnitConnectionData getUnitConnectionData(const mdtClUnitConnectionKeyData & key, bool & ok);

  /*! \brief Get unit connection data from database
   *
   * \param key Data that contains article connection ID (ArticleConnection_Id_FK).
   *     Note: only id is used, other key memebers are ignored.
   * \return data for given key.
   *       A null data is returned if given key does not exist, or a error occured.
   *       Use ok parameter to diffrenciate both cases.
   */
  mdtClUnitConnectionData getUnitConnectionData(const mdtClArticleConnectionKeyData & key, bool & ok);

  /*! \brief Get a list of unit connections data for given unit connector
   *
   * \param pk PK of unit connector that contains connections
   * \return List of unit connections.
   *        A empty list is returned if connector contains no connection, or a error occured.
   *        Use ok parameter to diffrenciate both cases.
   */
  QList<mdtClUnitConnectionData> getUnitConnectionDataList(const mdtClUnitConnectorPkData & key, bool & ok);

  /*! \brief Update unit connection
   *
   * \param key Key of unit connection to edit (only id is used in key)
   * \param data Data to set
   */
  bool updateUnitConnection(const mdtClUnitConnectionKeyData & key, const mdtClUnitConnectionData & data);

  /*! \brief Remove a unit connection
   */
  bool removeUnitConnection(const mdtClUnitConnectionKeyData & key);

  /*! \brief Remove each unit connection that is contained in selection
   *
   * This is usefull used together with mdtSqlTableWidget .
   *  Note: is selection s, Id_PK from UnitConnection_tbl is used.
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeUnitConnections(const mdtSqlTableSelection & s);

  /*! \brief Add a unit connector to database
   *
   * \param data Unit connector data to store
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   * \return Id_PK of added unit connector, or a null PK on error
   */
  mdtClUnitConnectorPkData addUnitConnector(mdtClUnitConnectorData data, bool handleTransaction);

  /*! \brief Get unit connector data from database
   *
   * \param pk Unit connector PK.
   * \param includeConnectionData If true, related connections are also included in data
   * \param ok Is set false on error
   * \return data for given key.
   *       A null data is returned if given key does not exist, or a error occured.
   *       Use ok parameter to diffrenciate both cases.
   */
  mdtClUnitConnectorData getUnitConnectorData(mdtClUnitConnectorPkData pk, bool includeConnectionData, bool & ok);

  /*! \brief Get unit connector data from database
   *
   * \param key Article connection, that also is presumed to be based on a article connector, on witch unit connector is based.
   *            Note: in key, only id is used, other members are ignored.
   * \param ok Is set false on error
   * \return data for given key.
   *       A null data is returned if given key does not return a result, or a error occured.
   *       Use ok parameter to diffrenciate both cases.
   */
  mdtClUnitConnectorKeyData getUnitConnectorKeyData(const mdtClArticleConnectionKeyData & key, bool & ok);

  /*! \brief Update unit connector name
   *
   * \param pk Unit connector PK.
   * \param name New name to give to unit connector
   */
  bool updateUnitConnectorName(const mdtClUnitConnectorPkData & pk, const QVariant & name);

  /*! \brief Remove unit connector
   *
   * \param pk PK of unit connector to remove
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool removeUnitConnector(const mdtClUnitConnectorPkData & pk, bool handleTransaction);

  /*! \brief Remove each unit connector that is contained in selection
   *
   * This is usefull used together with mdtSqlTableWidget .
   *  Note: is selection s, Id_PK from UnitConnector_tbl is used.
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeUnitConnectors(const mdtSqlTableSelection & s);

  /*! \brief Add unit connections to given unit connector by taking given connector contacts as base
   *
   * \param ucnrData Unit connector data to update
   * \param contactDataList List of contacts (from ConnectorContact_tbl) to use for unit connections creation
   */
  void addConnectionsToUnitConnector(mdtClUnitConnectorData & ucnrData, const QList<mdtClConnectorContactData> & contactDataList);

  /*! \brief Add unit connections to given unit connector by taking given article connections as base
   *
   * \param ucnrData Unit connector data to update
   * \param articleConnectionDataList List of article connections to use for unit connections creation
   */
  void addConnectionsToUnitConnector(mdtClUnitConnectorData & ucnrData, const QList<mdtClArticleConnectionData> & articleConnectionDataList);

 private:

  /*! \brief Get base SQL statement to get unit connections
   */
  QString baseSqlForUnitConnection() const;

  /*! \brief Fill given record with given unit connection data
   *
   * \pre record must be setup
   */
  void fillRecord(mdtSqlRecord & record, const mdtClUnitConnectionData & data);

  /*! \brief Fill given unit connection data with given record
   *
   * \pre record must contain all fields from UnitConnection_tbl
   *       and Connector_Id_FK from UnitConnector_tbl
   */
  void fillData(mdtClUnitConnectionData & data, const QSqlRecord & record);

  /*! \brief Get base SQL statement to get unit connectors
   */
  QString baseSqlForUnitConnector() const;

  /*! \brief Fill given record with given unit connector data
   *
   * \pre record must be setup
   */
  void fillRecord(mdtSqlRecord & record, const mdtClUnitConnectorData & data);

  /*! \brief Fill given unit connector data with given record
   *
   * \pre record must contain all fields from UnitConnector_tbl,
   *       Connector_Id_FK and Article_Id_FK from ArticleConnector_tbl
   */
  void fillData(mdtClUnitConnectorData & data, const QSqlRecord & record);

  Q_DISABLE_COPY(mdtClUnitConnection);
};

#endif // #ifndef MDT_CL_UNIT_CONNECTION_H
