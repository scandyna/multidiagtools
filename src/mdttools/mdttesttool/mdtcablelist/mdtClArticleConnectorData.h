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
#ifndef MDT_CL_ARTICLE_CONNECTOR_DATA_H
#define MDT_CL_ARTICLE_CONNECTOR_DATA_H

#include "mdtSqlRecord.h"
#include "mdtClConnectorData.h" /// \todo could be removed (once done)
#include "mdtClArticleConnectorKeyData.h"
#include "mdtClArticleConnectionData.h"
#include <QList>
#include <QVariant>
#include <QSqlDatabase>

/*! \brief Data container for article connector data
 *
 * Refers to ArticleConnector_tbl
 */
struct mdtClArticleConnectorData : public mdtSqlRecord  /// \todo When all is adapted, remove this inheritance
{
 private:

  /*! \brief Article connector key data
   */
  mdtClArticleConnectorKeyData pvKeyData;

 public:

  /*! \brief Get key data
   */
  mdtClArticleConnectorKeyData keyData() const
  {
    return pvKeyData;
  }

  /*! \brief Set key data
   *
   * If connector contains connections, they will also updated
   */
  void setKeyData(const mdtClArticleConnectorKeyData & key);

  /*! \brief Check if data is null
   *
   * Article connector data is null if its key is null
   */
  inline bool isNull() const
  {
    return pvKeyData.isNull();
  }

  /*! \brief Clear data
   */
  void clear();

  /*! \brief Check if data is based on a connector (from Connector_tbl)
   */
  inline bool isBasedOnConnector() const
  {
    return pvKeyData.isBasedOnConnector();
  }

  /*! \brief Article connector name
   */
  QVariant name;

  /*! \brief Add connection data
   *
   * In key that given data contains, only id and connectionTypeFk are keeped.
   *  articleId and articleConnectorFk are set to match article connector data.
   */
  void addConnectionData(mdtClArticleConnectionData data);

  /*! \brief Set list of connection data
   *
   * \sa addConnectionData().
   */
  void setConnectionDataList(const QList<mdtClArticleConnectionData> & dataList);

  /*! \brief Access list of connection data
   */
  inline const QList<mdtClArticleConnectionData> & connectionDataList() const
  {
    return pvConnectionDataList;
  }

 public:

  /*! \brief Construct a empty mdtClArticleConnectorData
   *
   * \deprecated
   */
  mdtClArticleConnectorData();

  /*! \brief Contruct a mdtClArticleConnectorData from a QSqlRecord
   *
   * Note: if this method is used, setup is not relevant.
   *
   * \pre All fields from ArticleConnector_tbl must exist in record
   * \deprecated
   */
  mdtClArticleConnectorData(const QSqlRecord & record);

  /*! \brief Setup fields from ArticleConnector_tbl
   *
   * \param setupCd If true, fields from connector part are also added.
   * \deprecated
   */
  bool setup(const QSqlDatabase & db, bool setupCd);

  /*! \brief Clear values
   *
   * Will clear values, including connection data.
   *  Fields are keeped.
   * \deprecated
   */
  void clearValues();

  /*! \brief Clear values and fields
   *
   * Will clear values, including connection data.
   *  Fields are also removed.
   * \deprecated
   */
//   void clear();

  /*! \brief Update existing connection data
   *
   * Will update connection data for witch Id_PK matches given connectionId.
   *
   * \return False if connectionId was not found.
   * \deprecated
   */
  bool setConnectionData(const QVariant & connectionId, const mdtClArticleConnectionData & data);

  /*! \brief Get connection data for given connection ID
   *
   * Return the connection data for witch Id_PK matches given connectionId.
   *  ok will be set to false if connectionId was not found.
   * \deprecated
   */
  mdtClArticleConnectionData connectionData(const QVariant & connectionId, bool *ok) const;

  /*! \brief Set connector data
   *
   * Will also update Connector_Id_FK
   *
   * \pre data must contain a non Null Id_PK
   * \deprecated
   */
  void setConnectorData(const mdtClConnectorData & data);

  /*! \brief Get connector data (from Connector_tbl)
   */
  mdtClConnectorData connectorData() const;

  /*! \brief Check if current artice connector is based on a connector
   * \deprecated
   */
//   bool isBasedOnConnector() const;

 private:

  QList<mdtClArticleConnectionData> pvConnectionDataList;
  mdtClConnectorData pvConnectorData;
};

#endif // #ifndef MDT_CL_ARTICLE_CONNECTOR_DATA_H
