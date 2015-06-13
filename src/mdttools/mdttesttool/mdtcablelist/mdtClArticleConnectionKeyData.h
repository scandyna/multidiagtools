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
#ifndef MDT_CL_ARTICLE_CONNECTION_KEY_DATA_H
#define MDT_CL_ARTICLE_CONNECTION_KEY_DATA_H

#include "mdtClArticleConnectorKeyData.h"
#include "mdtClConnectionTypeData.h"
#include <QVariant>

/*! \brief Data container for article connection key data
 *
 * Refers to ArticleConnection_tbl
 */
struct mdtClArticleConnectionKeyData
{
  /*! \brief Article connection ID (Id_PK)
   */
  QVariant id;

 private:

  /*! \brief Article ID (Article_Id_FK)
   */
  QVariant pvArticleId;

  /*! \brief Article connector FK (ArticleConnector_Id_FK)
   */
  mdtClArticleConnectorKeyData pvArticleConnectorFk;

  /*! \brief Connector FK (Connector_Id_FK)
   */
  mdtClConnectionTypeKeyData pvConnectionTypeFk;

 public:

  /*! \brief Set article ID (Article_Id_FK)
   *
   * \pre articleConnectorFk must not allready been set
   */
  void setArticleId(const QVariant & aid)
  {
    /*
     * If a article connector data/key is created for adding in database,
     * its id is not allready set, so pvArticleConnectorFk is allays null.
     * To check if pvArticleConnectorFk was set, we use the only mandatory field
     * that is needed for this case: articleId
     */
    Q_ASSERT(pvArticleConnectorFk.articleId().isNull());
    pvArticleId = aid;
  }

  /*! \brief Get article ID (Article_Id_FK)
   */
  inline QVariant articleId() const
  {
    return pvArticleId;
  }

  /*! \brief Set article connector FK (ArticleConnector_Id_FK)
   *
   * \pre fk must not be completely null.
   *      At least, its articleId must be set.
   * \pre If articleId is not null (i.e. was set with setArticleId()),
   *      fk.articleId must match articleId
   */
  void setArticleConnectorFk(const mdtClArticleConnectorKeyData & fk)
  {
    Q_ASSERT(!fk.articleId().isNull());
    Q_ASSERT(fk.articleId() == pvArticleId);
    pvArticleConnectorFk = fk;
  }

  /*! \brief Get article connector FK (ArticleConnector_Id_FK)
   */
  inline mdtClArticleConnectorKeyData articleConnectorFk() const
  {
    return pvArticleConnectorFk;
  }

  /*! \brief Set connection type
   */
  void setConnectionType(mdtClConnectionType_t t)
  {
    pvConnectionTypeFk.setType(t);
  }

  /*! \brief Set connection type code (ConnectionType_Code_FK)
   */
  void setConnectionTypeCode(const QVariant & c)
  {
    pvConnectionTypeFk.code = c;
  }

  /*! \brief Get connection type FK
   */
  inline mdtClConnectionTypeKeyData connectionTypeFk() const
  {
    return pvConnectionTypeFk;
  }

  /*! \brief Check if key data is null
   *
   * Article connection key data is null
   *  if id, articleId or connectionTypeFk is null
   *  (articleConnectorFk is optionnal)
   */
  bool isNull() const
  {
    return (id.isNull() || pvArticleId.isNull() || pvConnectionTypeFk.isNull());
  }

  /*! \brief Clear key data
   */
  void clear()
  {
    id.clear();
    pvArticleId.clear();
    pvArticleConnectorFk.clear();
    pvConnectionTypeFk.clear();
  }

  /*! \brief Check if article connection key is part of a article connector
   */
  bool isPartOfArticleConnector() const
  {
    return !pvArticleConnectorFk.isNull();
  }
};

#endif // #ifndef MDT_CL_ARTICLE_CONNECTION_KEY_DATA_H
