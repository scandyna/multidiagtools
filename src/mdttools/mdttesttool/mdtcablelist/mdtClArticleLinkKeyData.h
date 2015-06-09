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
#ifndef MDT_CL_ARTICLE_LINK_KEY_DATA_H
#define MDT_CL_ARTICLE_LINK_KEY_DATA_H

#include "mdtClArticleConnectionData.h"
#include "mdtClLinkTypeData.h"
#include "mdtClLinkDirectionData.h"

/*! \brief Article link primary key data
 *
 * Refers to ArticleLink_tbl
 */
struct mdtClArticleLinkPkData
{
  /*! \brief Start connection ID (ArticleConnectionStart_Id_FK)
   */
  QVariant connectionStartId;

  /*! \brief End connection ID (ArticleConnectionEnd_Id_FK)
   */
  QVariant connectionEndId;

  /*! \brief Check if primary key is null
   *
   * Primary key is null if one of the connection ID is null
   */
  bool isNull() const
  {
    return (connectionStartId.isNull() || connectionEndId.isNull());
  }

  /*! \brief Clear primary key
   */
  void clear()
  {
    connectionStartId.clear();
    connectionEndId.clear();
  }
};

/*! \brief Article link key data
 *
 * Refers to ArticleLink_tbl
 */
struct mdtClArticleLinkKeyData
{
  /*! \brief Primary key data
   */
  mdtClArticleLinkPkData pk;

  /*! \brief Start connection (ArticleConnectionStart_Id_FK)
   */
//   mdtClArticleConnectionKeyData connectionStartFk;

  /*! \brief End connection (ArticleConnectionEnd_Id_FK)
   */
//   mdtClArticleConnectionKeyData connectionEndFk;

  /*! \brief Link type (LinkType_Code_FK)
   */
  mdtClLinkTypeKeyData linkTypeFk;

  /*! \brief Link direction (LinkDirection_Code_FK)
   */
  mdtClLinkDirectionKeyData linkDirectionFk;

  /*! \brief Check if key data is null
   *
   * Key is null if pk is null
   *  or linkTypeFk or linkDirectionFk is null
   */
  bool isNull() const
  {
//     return (connectionStartFk.isNull() || connectionEndFk.isNull() || linkTypeFk.isNull() || linkDirectionFk.isNull());
    return (pk.isNull() || linkTypeFk.isNull() || linkDirectionFk.isNull());
  }

  /*! \brief Clear
   */
  void clear()
  {
//     connectionStartFk.clear();
//     connectionEndFk.clear();
    pk.clear();
    linkTypeFk.clear();
    linkDirectionFk.clear();
  }
};

#endif // #ifndef MDT_CL_ARTICLE_LINK_KEY_DATA_H
