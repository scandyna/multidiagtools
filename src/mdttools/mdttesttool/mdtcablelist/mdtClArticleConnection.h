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
#ifndef MDT_CL_ARTICLE_CONNECTION_H
#define MDT_CL_ARTICLE_CONNECTION_H

#include "mdtClConnector.h"
#include "mdtClArticleConnectorKeyData.h"
#include "mdtClArticleConnectorData.h"
#include "mdtClArticleConnectionKeyData.h"
#include "mdtClArticleConnectionData.h"
#include "mdtSqlTableSelection.h"
#include <QSqlRecord>

/*! \brief Helper class for article connection and article connector manipulations
 *
 * Acts mainly on ArticleConnection_tbl and ArticleConnector_tbl
 */
class mdtClArticleConnection : public mdtClConnector
{
 public:

  /*! \brief Constructor
   */
  mdtClArticleConnection(QObject *parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtClArticleConnection(QSqlDatabase db);

  /*! \brief Add a article connection to database
   *
   * \param data Article connection data to store
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   * \return Id_PK of added article connection, or a null key on error
   */
  mdtClArticleConnectionKeyData addArticleConnection(const mdtClArticleConnectionData & data, bool handleTransaction);

  /*! \brief Add a list of article connections to database
   *
   * \param dataList List of article connections data to store
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool addArticleConnectionList(const QList<mdtClArticleConnectionData> & dataList, bool handleTransaction);

  /*! \brief Get article connection data from database
   *
   * \param key Data that contains connection ID (Id_PK).
   *     Note: only id is used, other key memebers are ignored.
   * \return data for given key.
   *       A null data is returned if given key does not exist, or a error occured.
   *       Use ok parameter to diffrenciate both cases.
   */
  mdtClArticleConnectionData getArticleConnectionData(const mdtClArticleConnectionKeyData & key, bool & ok);

  /*! \brief Get a list of article connections data for given article connector
   *
   * \param key Key of article connector that contains connections
   * \return List of connector contacts.
   *        A empty list is returned if connector contains no contact, or a error occured.
   *        Use ok parameter to diffrenciate both cases.
   */
  QList<mdtClArticleConnectionData> getArticleConnectionDataList(const mdtClArticleConnectorKeyData & key, bool & ok);

  /*! \brief Update article connection
   *
   * \param key Key of article connection to edit (only id is used in key)
   * \param data Data to set
   */
  bool updateArticleConnection(const mdtClArticleConnectionKeyData & key, const mdtClArticleConnectionData & data);

  /*! \brief Remove article connection
   */
  bool removeArticleConnection(const mdtClArticleConnectionKeyData & key);

  /*! \brief Remove each article connection that is contained in selection
   *
   * This is usefull used together with mdtSqlTableWidget .
   *  Note: is selection s, Id_PK from ArticleConnection_tbl is used.
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeArticleConnections(const mdtSqlTableSelection & s);

  /*! \brief Add a article connector to database
   *
   * \param data Article connector data to store
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   * \return Id_PK of added article connector, or a null key on error
   */
  mdtClArticleConnectorKeyData addArticleConnector(mdtClArticleConnectorData data, bool handleTransaction);

  /*! \brief Get article connector data from database
   *
   * \param key Article connector key. Only id must be set, other data are not used.
   * \param includeConnectionData If true, related connections are also included in data
   * \param ok Is set false on error
   * \return data for given key.
   *       A null data is returned if given key does not exist, or a error occured.
   *       Use ok parameter to diffrenciate both cases.
   */
  mdtClArticleConnectorData getArticleConnectorData(mdtClArticleConnectorKeyData key, bool includeConnectionData, bool & ok);

  /*! \brief Update article connector name
   *
   * \param key Article connector key. Only id must be set, other data are not used.
   * \param name New name to give to article connector
   */
  bool updateArticleConnectorName(const mdtClArticleConnectorKeyData & key, const QVariant & name);

  /*! \brief Remove article connector
   *
   * \param key Key of article connector to remove
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool removeArticleConnector(const mdtClArticleConnectorKeyData & key, bool handleTransaction);

  /*! \brief Remove each connector that is contained in selection
   *
   * Will also remove related connections.
   *
   * This is usefull used together with mdtSqlTableWidget .
   *  Note: is selection s, Id_PK from ArticleConnector_tbl is used.
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeArticleConnectors(const mdtSqlTableSelection & s);

  /*! \brief Add article connections to given article connector by taking given connector contacts as base
   *
   * \param acnrData Article connector data to update
   * \param contactDataList List of contacts (from ConnectorContact_tbl) to use for article connections
   */
  void addConnectionsToArticleConnector(mdtClArticleConnectorData & acnrData, const QList<mdtClConnectorContactData> & contactDataList);

 private:

  /*! \brief Fill given record with given article connection data
   *
   * \pre record must be setup
   */
  void fillRecord(mdtSqlRecord & record, const mdtClArticleConnectionData & data);

  /*! \brief Fill given article connection data with given record
   *
   * \pre record must contain all fields from ArticleConnection_tbl
   *       and Connector_Id_FK from ArticleConnector_tbl
   */
  void fillData(mdtClArticleConnectionData & data, const QSqlRecord & record);

  Q_DISABLE_COPY(mdtClArticleConnection);
};

#endif // #ifndef MDT_CL_ARTICLE_CONNECTION_H
