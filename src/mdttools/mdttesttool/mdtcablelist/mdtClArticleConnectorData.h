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
#ifndef MDT_CL_ARTICLE_CONNECTOR_DATA_H
#define MDT_CL_ARTICLE_CONNECTOR_DATA_H

#include "mdtSqlRecord.h"
#include "mdtClConnectorData.h"
#include "mdtClArticleConnectionData.h"
#include <QList>
#include <QVariant>
#include <QSqlDatabase>

/*! \brief Data container class for article connector data
 *
 * Permit to exchange data with ArticleConnector_tbl and ArticleConnection_tbl.
 */
class mdtClArticleConnectorData : public mdtSqlRecord
{
 public:

  /*! \brief Construct a empty mdtClArticleConnectorData
   */
  mdtClArticleConnectorData();

  /*! \brief Contruct a mdtClArticleConnectorData from a QSqlRecord
   *
   * Note: if this method is used, setup is not relevant.
   *
   * \pre All fields from ArticleConnector_tbl must exist in record
   */
  mdtClArticleConnectorData(const QSqlRecord & record);

  /*! \brief Setup fields from ArticleConnector_tbl
   *
   * \param setupCd If true, fields from connector part are also added.
   */
  bool setup(const QSqlDatabase & db, bool setupCd);

  /*! \brief Clear values
   *
   * Will clear values, including connection data.
   *  Fields are keeped.
   */
  void clearValues();

  /*! \brief Clear values and fields
   *
   * Will clear values, including connection data.
   *  Fields are also removed.
   */
  void clear();

  /*! \brief Add a list of connection data
   *
   * By using this method, the caller is responsible to give
   *  valid records, that contains needed fields.
   *
   * \sa addConnectionData().
   */
  void setConnectionDataList(const QList<mdtClArticleConnectionData> & dataList);

  /*! \brief Get list of connection data
   */
  const QList<mdtClArticleConnectionData> & connectionDataList() const;

  /*! \brief Add connection data
   *
   * \pre data must contains following fields:
   *  - Id_PK
   *  - Article_Id_FK
   *  - ArticleConnector_Id_FK
   *  - ArticleContactName
   *  - IoType
   *  - FunctionEN
   *  - FunctionFR
   *  - FunctionDE
   *  - FunctionIT
   */
  void addConnectionData(const mdtClArticleConnectionData & data);

  /*! \brief Update existing connection data
   *
   * Will update connection data for witch Id_PK matches given connectionId.
   *
   * \return False if connectionId was not found.
   */
  bool setConnectionData(const QVariant & connectionId, const mdtClArticleConnectionData & data);

  /*! \brief Get connection data for given connection ID
   *
   * Return the connection data for witch Id_PK matches given connectionId.
   *  ok will be set to false if connectionId was not found.
   */
  mdtClArticleConnectionData connectionData(const QVariant & connectionId, bool *ok) const;

  /*! \brief Set connector data
   *
   * Will also update Connector_Id_FK
   *
   * \pre data must contain a non Null Id_PK
   */
  void setConnectorData(const mdtClConnectorData & data);

  /*! \brief Get connector data (from Connector_tbl)
   */
  mdtClConnectorData connectorData() const;

  /*! \brief Check if current artice connector is based on a connector
   */
  bool isBasedOnConnector() const;

 private:

  QList<mdtClArticleConnectionData> pvConnectionDataList;
  mdtClConnectorData pvConnectorData;
};

#endif // #ifndef MDT_CL_ARTICLE_CONNECTOR_DATA_H
