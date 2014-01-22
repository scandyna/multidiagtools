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
///#include "mdtClArticleConnectionData.h"
///#include "mdtClLinkData.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
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

  /*! \brief Get a model with articles for compononent selection
   *
   * Will setup a query model witch contains all articles except articleId
   *  and articles that allready are component of articleID .
   */
  QSqlQueryModel *articleModelForComponentSelection(const QVariant &articleId);

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
   */
  ///QList<mdtClArticleConnectionData> connectorContactData(const QList<QVariant> & connectorContactIdList);
  QList<QSqlRecord> connectorContactData(const QList<QVariant> & connectorContactIdList, bool *ok);

  /*! \brief Add a connection
   */
  ///bool addConnection(const mdtClArticleConnectionData &data);
  bool addConnection(const mdtSqlRecord & data);

  /*! \brief Add many connections
   *
   * If single transaction is set, a transaction is used .
   */
  ///bool addConnections(const QList<mdtClArticleConnectionData> & dataList, bool singleTransaction);
  bool addConnections(const QList<mdtSqlRecord> & dataList, bool singleTransaction);

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

  /*! \brief Add a connector and its contacts
   *
   * \param articleConnectorData Article connector data part. Must contain following fields:
   *                      - Article_Id_FK
   *                      - Name
   *                      Can also contain Connector_Id_FK.
   * \param articleConnectionDataList List of article connection data part.
   *                      Note: Article_Id_FK and ArticleConnector_Id_FK
   *                            are not relevant, because they are token from articleConnectorData.
   */
  ///bool addConnector(const QList<mdtClArticleConnectionData> & dataList);
  bool addConnector(const mdtSqlRecord & articleConnectorData, const QList<QSqlRecord> & articleConnectionDataList);

  /*! \brief Remove a connector and all its contacts
   */
  bool removeConnector(const QVariant & articleConnectorId);

  /*! \brief Remove a list of connectors and all related contacts
   */
  bool removeConnectors(const QList<QVariant> & articleConnectorIdList);

  /*! \brief Add a record in Link table
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  ///bool addLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double value, const QVariant & directionCode, const QVariant & typeCode);
  bool addLink(const mdtSqlRecord & linkData);

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
  ///bool editLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, const mdtClLinkData &data);
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

  QSqlQueryModel *pvArticleModel;
};

#endif  // #ifndef MDT_CL_ARTICLE_H
