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
#ifndef MDT_CL_ARTICLE_H
#define MDT_CL_ARTICLE_H

#include "mdtTtBase.h"
#include "mdtSqlRecord.h"
#include "mdtClArticleConnectionData.h"
#include "mdtClArticleConnectorData.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <QString>
#include <QModelIndex>
#include <QList>

/*! \brief Helper class for Article and related tables edition
 */
class mdtClArticle : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtClArticle(QObject *parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClArticle();

  /*! \brief Get SQL statement for article component selection
   */
  QString sqlForArticleComponentSelection(const QVariant &articleId) const;

  /*! \brief Get SQL statement for connector contact selection
   *
   * Get a list of connector contact (including some connector data)
   *  for witch connector exists in given articleId.
   */
  

  /*! \brief Add a article as component
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool addComponent(const QVariant &articleId, const QVariant &componentId, const QVariant &qty, const QVariant &qtyUnit);

  /*! \brief Edit a component
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool editComponent(const QVariant &articleId, const QVariant &currentComponentId, const QVariant &newComponentId, const QVariant &qty, const QVariant &qtyUnit);

  /*! \brief Remove a single component
   */
  bool removeComponent(const QVariant &articleId, const QVariant &componentId);

  /*! \brief Remove a list of components
   */
  bool removeComponents(const QVariant &articleId, const QList<QVariant> &componentIdList);

  /*! \brief Remove each component that is contained in selection
   *
   * This is usefull used together with mdtSqlTableWidget .
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeComponents(const QVariant &articleId, const QModelIndexList & indexListOfSelectedRows);

  /*! \brief Get a list of connector contact data for given connector contact ID list
   *
   * Note: result will contain data from ConnectorContact_tbl
   */
  QList<QSqlRecord> getConnectorContactDataList(const QList<QVariant> & connectorContactIdList, bool *ok);

  /*! \brief Get article connection data for given article connection ID
   *
   * \pre ok must be a valid pointer.
   */
  mdtClArticleConnectionData getConnectionData(const QVariant & articleConnectionId, bool *ok);

  /*! \brief Get a list af article connection data from given article connection ID list
   */
  QList<mdtClArticleConnectionData> getConnectionDataListFromConnectionIdList(const QList<QVariant> & connectionIdList, bool *ok);

  /*! \brief Get article connector data for given article connector ID
   *
   * \param includeConnectionData If true, and SQL statement points to a article connector
   *            that contains article connections, result will be populated with these article connection data.
   * \param includeBaseConnectorData If true, and SQL statement points to a article connector
   *            that is based on a connector (from Connector_tbl), result will be populated with these connector data.
   * \pre ok must be valid
   */
  mdtClArticleConnectorData getConnectorData(const QVariant & articleConnectorId, bool *ok, bool includeConnectionData, bool includeBaseConnectorData);

  /*! \brief Add article connections into article connector data based on a list of given connector contact ID list
   *
   * Will also get some values from data (ArticleConnector_tbl) and set it to created connections (ArticleConnection_tbl):
   *  - ArticleConnector_tbl.Id_PK -> ArticleConnection_tbl.ArticleConnector_Id_FK
   *  - ArticleConnector_tbl.Article_Id_FK -> ArticleConnection_tbl.Article_Id_FK
   *
   * Note: will only update given data, nothing is written to database.
   */
  bool addConnectionDataListFromConnectorContactIdList(mdtClArticleConnectorData & data, const QList<QVariant> & connectorContactIdList);

  /*! \brief Add a connection
   */
  bool addConnection(const mdtClArticleConnectionData & data);

  /*! \brief Edit a article connection
   */
  bool editConnection(const QVariant & connectionId, const mdtClArticleConnectionData & data);

  /*! \brief Remove a single connection
   */
  bool removeConnection(const QVariant & articleConnectionId);

  /*! \brief Remove connections
   */
  bool removeConnections(const QList<QVariant> & articleConnectionIdList);

  /*! \brief Remove each connection that is contained in selection
   *
   * This is usefull used together with mdtSqlTableWidget .
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeConnections(const QModelIndexList & indexListOfSelectedRows);

  /*! \brief Remove connections for a given article connector
   *
   * If single transaction is set, a transaction is used .
   */
  bool removeConnectorConnections(const QVariant & articleConnectorId);

  /*! \brief Remove connections for a given list article connectors
   */
  bool removeConnectorsConnections(const QList<QVariant> & articleConnectorIdList);

  /*! \brief Add a connector
   *
   * If given article connector contains connections,
   *  they will be added.
   *
   * Note: in connection data (contained in data),
   *  Article_Id_FK and ArticleConnector_Id_FK are not relevant,
   *  because they are token from data directly.
   */
  bool addConnector(const mdtClArticleConnectorData & data);

  /*! \brief Edit connector name
   */
  bool editConnectorName(const QVariant & articleConnectorId, const QVariant & name);

  /*! \brief Remove a connector and all its contacts
   */
  bool removeConnector(const QVariant & articleConnectorId);

  /*! \brief Remove each connector that is contained in selection
   *
   * Will also remove related connections.
   *
   * This is usefull used together with mdtSqlTableWidget .
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeConnectors(const QModelIndexList & indexListOfSelectedRows);

  /*! \brief Add a record in Link table
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool addLink(const mdtSqlRecord & linkData);

  /*! \brief Add a cable link
   */
  bool addCableLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, const QVariant & identification, const QVariant & R);

  /*! \brief Add a link of type resistor
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  //bool addResistor(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double value);

  /*! \brief Add a link of type diode
   */
  //bool addDiode(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double Vf, const QVariant & directionCode);

  /*! \brief Add a link of type bridge
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  //bool addBridge(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId);

  /*! \brief Edit a record in Link table
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool editLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, const mdtSqlRecord &data);

  /*! \brief Remove a signle link
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId);

  /*! \brief Remove each unit link that is contained in selection
   */
  bool removeLinks(const QList<QModelIndexList> &indexListOfSelectedRowsByRows);

 private:

  /*! \brief Get article connection data
   *
   * \pre Given SQL statement must return 1 article connection row
   * \pre ok must be valid
   */
  mdtClArticleConnectionData getConnectionDataPv(const QString & sql, bool *ok);

  /*! \brief Get article connector data
   *
   * \param includeConnectionData If true, and SQL statement points to a article connector
   *            that contains article connections, result will be populated with these article connection data.
   * \param includeBaseConnectorData If true, and SQL statement points to a article connector
   *            that is based on a connector (from Connector_tbl), result will be populated with these connector data.
   * \pre Given SQL statement must return 1 article connector row
   * \pre ok must be valid
   */
  mdtClArticleConnectorData getConnectorDataPv(const QString & sql, bool *ok, bool includeConnectionData, bool includeBaseConnectorData);
};

#endif  // #ifndef MDT_CL_ARTICLE_H
