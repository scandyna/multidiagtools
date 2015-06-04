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

  /*! \brief Article ID (Article_Id_FK)
   */
  QVariant articleId;

  /*! \brief Article connector FK (ArticleConnector_Id_FK)
   */
  mdtClArticleConnectorKeyData articleConnectorFk;

  /*! \brief Connector FK (Connector_Id_FK)
   */
  mdtClConnectionTypeKeyData connectionTypeFk;

  /*! \brief Check if key data is null
   *
   * Article connection key data is null
   *  if id, articleId or connectionTypeFk is null
   *  (articleConnectorFk is optionnal)
   */
  bool isNull() const
  {
    return (id.isNull() || articleId.isNull() || connectionTypeFk.isNull());
  }

  /*! \brief Clear key data
   */
  void clear()
  {
    id.clear();
    articleId.clear();
    articleConnectorFk.clear();
    connectionTypeFk.clear();
  }

  /*! \brief Check if article connection key is part of a article connector
   */
  bool isPartOfArticleConnector() const
  {
    return !articleConnectorFk.isNull();
  }
};

#endif // #ifndef MDT_CL_ARTICLE_CONNECTION_KEY_DATA_H
