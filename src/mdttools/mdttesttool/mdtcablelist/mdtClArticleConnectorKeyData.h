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
#ifndef MDT_CL_ARTICLE_CONNECTOR_KEY_DATA_H
#define MDT_CL_ARTICLE_CONNECTOR_KEY_DATA_H

#include "mdtClConnectorKeyData.h"
#include <QVariant>

/*! \brief Data container for article connector key
 *
 * Refers to ArticleConnector_tbl
 */
struct mdtClArticleConnectorKeyData
{
  /*! \brief Article connector ID (Id_PK)
   */
  QVariant id;

 private:

  /*! \brief Article ID (Article_Id_FK)
   *
   * \todo mettre private
   */
  QVariant pvArticleId;

  /*! \brief Connector FK (Connector_Id_FK)
   */
  mdtClConnectorKeyData pvConnectorFk;

 public:

  /*! \brief Set article ID (Article_Id_FK)
   */
  void setArticleId(const QVariant & aid)
  {
    pvArticleId = aid;
  }

  /*! \brief Get article ID (Article_Id_FK)
   */
  inline QVariant articleId() const
  {
    return pvArticleId;
  }

  /*! \brief Set connector FK (Connector_Id_FK)
   */
  void setConnectorFk(const mdtClConnectorKeyData & fk)
  {
    pvConnectorFk = fk;
  }

  /*! \brief Get connector FK (Connector_Id_FK)
   */
  inline mdtClConnectorKeyData connectorFk() const
  {
    return pvConnectorFk;
  }

  /*! \brief Check if key data is null
   *
   * Key data is null when id or articleId is null
   *  (connectorFk is optionnal)
   */
  bool isNull() const
  {
    return (id.isNull() || pvArticleId.isNull());
  }

  /*! \brief Check if key data is based on a connector (from Connector_tbl)
   */
  bool isBasedOnConnector() const
  {
    return !pvConnectorFk.isNull();
  }

  /*! \brief Clear key data
   */
  void clear()
  {
    id.clear();
    pvArticleId.clear();
    pvConnectorFk.clear();
  }
};

#endif // #ifndef MDT_CL_ARTICLE_CONNECTOR_KEY_DATA_H
