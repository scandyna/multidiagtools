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
#ifndef MDT_CABLE_LIST_ARTICLE_LINK_PK_H
#define MDT_CABLE_LIST_ARTICLE_LINK_PK_H

#include "ArticleConnectionPk.h"

namespace Mdt{ namespace CableList{

  /*! \brief Article link primary key data
   *
   * Refers to ArticleLink_tbl
   */
  class ArticleLinkPk
  {
   public:

    // Declare default constructor noexcept
    constexpr ArticleLinkPk() noexcept = default;
    // Declare copy noexcept
    constexpr ArticleLinkPk(const ArticleLinkPk &) noexcept = default;
    ArticleLinkPk & operator=(const ArticleLinkPk &) noexcept = default;
    // Declare move noexcept
    constexpr ArticleLinkPk(ArticleLinkPk &&) noexcept = default;
    ArticleLinkPk & operator=(ArticleLinkPk &&) noexcept = default;

    /*! \brief Clear start connection
     */
    constexpr void clearConnectionStart() noexcept
    {
      pvConnectionStart.clear();
    }

    /*! \brief Set start connection
     */
    constexpr void setConnectionStart(ArticleConnectionPk fk) noexcept
    {
      pvConnectionStart = fk;
    }

    /*! \brief Get start connection
     */
    constexpr ArticleConnectionPk connectionStart() const noexcept
    {
      return pvConnectionStart;
    }

    /*! \brief Clear end connection
     */
    constexpr void clearConnectionEnd() noexcept
    {
      pvConnectionEnd.clear();
    }

    /*! \brief Set end connection
     */
    constexpr void setConnectionEnd(ArticleConnectionPk fk) noexcept
    {
      pvConnectionEnd = fk;
    }

    /*! \brief Get end connection
     */
    constexpr ArticleConnectionPk connectionEnd() const noexcept
    {
      return pvConnectionEnd;
    }

    /*! \brief Check if primary key is null
     *
     * Primary key is null if one of the connection (or both) is null
     */
    constexpr bool isNull() const noexcept
    {
      return (pvConnectionStart.isNull() || pvConnectionEnd.isNull() );
    }

    /*! \brief Clear PK
     */
    constexpr void clear() noexcept
    {
      pvConnectionStart.clear();
      pvConnectionEnd.clear();
    }

   private:

    ArticleConnectionPk pvConnectionStart;
    ArticleConnectionPk pvConnectionEnd;
  };

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_ARTICLE_LINK_PK_H
