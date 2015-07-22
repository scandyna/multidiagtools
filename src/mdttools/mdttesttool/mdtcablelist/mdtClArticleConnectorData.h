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

#include "mdtClArticleConnectorKeyData.h"
#include "mdtClArticleConnectionData.h"
#include <QList>
#include <QVariant>

/*! \brief Data container for article connector data
 *
 * Refers to ArticleConnector_tbl
 */
struct mdtClArticleConnectorData
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
   * \pre key must at least have its articleId set
   * \pre keyData must not allready been set
   */
  void setKeyData(const mdtClArticleConnectorKeyData & key)
  {
    Q_ASSERT(!key.articleId().isNull());
    Q_ASSERT(pvKeyData.articleId().isNull());
    Q_ASSERT(!pvKeyData.isBasedOnConnector());
    pvKeyData = key;
  }

  /*! \brief Set/update article connector ID (Id_PK)
   *
   * If connector contains connections,
   *  they will be updated.
   */
  void setId(const QVariant & id);

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
   *
   * \pre In keyData, articleId must allready been set.
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

 private:

  QList<mdtClArticleConnectionData> pvConnectionDataList;
};

#endif // #ifndef MDT_CL_ARTICLE_CONNECTOR_DATA_H
