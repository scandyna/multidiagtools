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
#ifndef MDT_CABLE_LIST_LINK_KEY_H
#define MDT_CABLE_LIST_LINK_KEY_H

#include "LinkPk.h"
#include "LinkTypePk.h"
#include "LinkDirectionPk.h"
#include "ArticleLinkPk.h"
#include "WirePk.h"
#include "LinkBeamPk.h"

namespace Mdt{ namespace CableList{

  /*! \brief Link key data
   *
   * Refers to Link_tbl
   */
  class LinkKey
  {
   public:

    // Declare default constructor noexcept
    constexpr LinkKey() noexcept = default;
    // Declare copy noexcept
    constexpr LinkKey(const LinkKey &) noexcept = default;
    LinkKey & operator=(const LinkKey &) noexcept = default;
    // Declare move noexcept
    constexpr LinkKey(LinkKey &&) noexcept = default;
    LinkKey & operator=(LinkKey &&) noexcept = default;

    /*! \brief Set PK (UnitConnectionStart_Id_FK and UnitConnectionEnd_Id_FK)
     */
    constexpr void setPk(const LinkPk & pk) noexcept
    {
      pvPk = pk;
    }

    /*! \brief Get PK (UnitConnectionStart_Id_FK and UnitConnectionEnd_Id_FK)
     */
    constexpr LinkPk pk() const noexcept
    {
      return pvPk;
    }

    /*! \brief Set link type (LinkType_Code_FK)
     */
    constexpr void setLinkType(LinkTypePk fk) noexcept
    {
      pvLinkType = fk;
    }

    /*! \brief Get link type (LinkType_Code_FK)
     */
    constexpr LinkTypePk linkType() const noexcept
    {
      return pvLinkType;
    }

    /*! \brief Set link direction (LinkDirection_Code_FK)
     */
    constexpr void setLinkDirection(LinkDirectionPk fk) noexcept
    {
      pvLinkDirection = fk;
    }

    /*! \brief Get link direction (LinkDirection_Code_FK)
     */
    constexpr LinkDirectionPk linkDirection() const noexcept
    {
      return pvLinkDirection;
    }

    /*! \brief Set article link FK (ArticleConnectionStart_Id_FK and ArticleConnectionEnd_Id_FK)
     */
    constexpr void setArticleLink(const ArticleLinkPk & fk) noexcept
    {
      pvArticleLink = fk;
    }

    /*! \brief Get article link FK (ArticleConnectionStart_Id_FK and ArticleConnectionEnd_Id_FK)
     */
    constexpr ArticleLinkPk articleLink() const noexcept
    {
      return pvArticleLink;
    }

    /*! \brief Set wire FK (Wire_Id_FK)
     */
    constexpr void setWire(WirePk fk) noexcept
    {
      pvWire = fk;
    }

    /*! \brief Get wire FK (Wire_Id_FK)
     */
    constexpr WirePk wire() const noexcept
    {
      return pvWire;
    }

    /*! \brief Set link beam FK (LinkBeam_Id_FK)
     */
    constexpr void setLinkBeam(LinkBeamPk fk) noexcept
    {
      pvLinkBeam = fk;
    }

    /*! \brief Get link beam FK (LinkBeam_Id_FK)
     */
    constexpr LinkBeamPk linkBeam() const noexcept
    {
      return pvLinkBeam;
    }

    /*! \brief Check if key is null
     *
     * Key is null if one of the mandatory FK is null, which are:
     *  - pk
     *  - type
     *  - direction
     */
    constexpr bool isNull() const noexcept
    {
      return (pvPk.isNull() || pvLinkType.isNull() || pvLinkDirection.isNull() );
    }

    /*! \brief Clear key
     */
    constexpr void clear() noexcept
    {
      pvPk.clear();
      pvLinkType.clear();
      pvLinkDirection.clear();
      pvArticleLink.clear();
      pvWire.clear();
      pvLinkBeam.clear();
    }

   private:

    LinkPk pvPk;
    LinkTypePk pvLinkType;
    LinkDirectionPk pvLinkDirection;
    ArticleLinkPk pvArticleLink;
    WirePk pvWire;
    LinkBeamPk pvLinkBeam;
  };

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_LINK_KEY_H
