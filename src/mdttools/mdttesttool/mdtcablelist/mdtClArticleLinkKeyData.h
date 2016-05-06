/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "Mdt/CableList/LinkTypePk.h"       /// \todo update once migrated
#include "Mdt/CableList/LinkDirectionPk.h"  /// \todo update once migrated

using Mdt::CableList::LinkType;           /// \todo Update once migrated
using Mdt::CableList::LinkTypePk;         /// \todo Update once migrated
using Mdt::CableList::LinkDirectionType;  /// \todo Remove once migrated
using Mdt::CableList::LinkDirectionPk;    /// \todo Remove once migrated

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

 private:

  /*! \brief Link type (LinkType_Code_FK)
   */
  LinkTypePk pvLinkTypeFk;

  /*! \brief Link direction (LinkDirection_Code_FK)
   */
  LinkDirectionPk pvLinkDirectionFk;

 public:

  /*! \brief Set link type
   */
  void setLinkType(LinkType t)
  {
    pvLinkTypeFk.setType(t);
  }

  /*! \brief Set link type code (LinkType_Code_FK)
   */
  void setLinkTypeCode(const QVariant & c)
  {
    pvLinkTypeFk = LinkTypePk::fromQVariant(c);
  }

  /*! \brief Get link type FK (LinkType_Code_FK)
   */
  inline LinkTypePk linkTypeFk() const
  {
    return pvLinkTypeFk;
  }

  /*! \brief Set link direction
   */
  void setLinkDirection(LinkDirectionType d)
  {
    pvLinkDirectionFk.setDirection(d);
  }

  /*! \brief Set link direction code (LinkDirection_Code_FK)
   */
  void setLinkDirectionCode(const QVariant & c)
  {
    pvLinkDirectionFk = LinkDirectionPk::fromQVariant(c);
  }

  /*! \brief Get link direction FK (LinkDirection_Code_FK)
   */
  inline LinkDirectionPk linkDirectionFk() const
  {
    return pvLinkDirectionFk;
  }

  /*! \brief Check if key data is null
   *
   * Key is null if pk is null
   *  or linkTypeFk or linkDirectionFk is null
   */
  bool isNull() const
  {
    return (pk.isNull() || pvLinkTypeFk.isNull() || pvLinkDirectionFk.isNull());
  }

  /*! \brief Clear
   */
  void clear()
  {
    pk.clear();
    pvLinkTypeFk.clear();
    pvLinkDirectionFk.clear();
  }
};

#endif // #ifndef MDT_CL_ARTICLE_LINK_KEY_DATA_H
