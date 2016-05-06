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
#ifndef MDT_CABLE_LIST_LINK_PK_H
#define MDT_CABLE_LIST_LINK_PK_H

#include "UnitConnectionPk.h"
#include "LinkVersionPk.h"
#include "ModificationPk.h"

namespace Mdt{ namespace CableList{

  /*! \brief Link primary key data
   *
   * Refers to Link_tbl
   */
  class LinkPk
  {
   public:

    // Declare default constructor noexcept
    constexpr LinkPk() noexcept = default;
    // Declare copy noexcept
    constexpr LinkPk(const LinkPk &) noexcept = default;
    LinkPk & operator=(const LinkPk &) noexcept = default;
    // Declare move noexcept
    constexpr LinkPk(LinkPk &&) noexcept = default;
    LinkPk & operator=(LinkPk &&) noexcept = default;

    /*! \brief Clear start connection
     */
    constexpr void clearConnectionStart() noexcept
    {
      pvConnectionStart.clear();
    }

    /*! \brief Set start connection
     */
    constexpr void setConnectionStart(UnitConnectionPk fk) noexcept
    {
      pvConnectionStart = fk;
    }

    /*! \brief Get start connection
     */
    constexpr UnitConnectionPk connectionStart() const noexcept
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
    constexpr void setConnectionEnd(UnitConnectionPk fk) noexcept
    {
      pvConnectionEnd = fk;
    }

    /*! \brief Get end connection
     */
    constexpr UnitConnectionPk connectionEnd() const noexcept
    {
      return pvConnectionEnd;
    }

    /*! \brief Set link version
     */
    constexpr void setVersion(const LinkVersionPk & fk) noexcept
    {
      pvVersion = fk;
    }

    /*! \brief Get link version
     */
    constexpr LinkVersionPk version() const noexcept
    {
      return pvVersion;
    }

    /*! \brief Set modification
     */
    constexpr void setModification(const ModificationPk & fk)
    {
      pvModification = fk;
    }

    /*! \brief Get modification
     */
    constexpr ModificationPk modification() const
    {
      return pvModification;
    }

    /*! \brief Check if primary key is null
     *
     * Primary key is null if one of the connection  or version or modification is null
     */
    constexpr bool isNull() const noexcept
    {
      return (pvConnectionStart.isNull() || pvConnectionEnd.isNull() || pvVersion.isNull() || pvModification.isNull() );
    }

    /*! \brief Check if lhs == rhs
     *
     * lhs and rhs are equal if their version are equal
     *  If lhs or rhs (or both) is null,
     *  they are never equal.
     */
    friend
    constexpr bool operator==(LinkPk lhs, LinkPk rhs) noexcept
    {
      if( lhs.isNull() || rhs.isNull() ){
        return false;
      }
      return (  (lhs.pvConnectionStart == rhs.pvConnectionStart) && (lhs.pvConnectionEnd == rhs.pvConnectionEnd)
             && (lhs.pvVersion == rhs.pvVersion) && (lhs.pvModification == rhs.pvModification) );
    }

    /*! \brief Check if lhs == rhs
     *
     * \sa operator==()
     */
    friend
    constexpr bool operator!=(LinkPk lhs, LinkPk rhs) noexcept
    {
      return !(lhs == rhs);
    }


    /*! \brief Clear PK
     */
    constexpr void clear() noexcept
    {
      pvConnectionStart.clear();
      pvConnectionEnd.clear();
      pvVersion.clear();
      pvModification.clear();
    }

   private:

    UnitConnectionPk pvConnectionStart;
    UnitConnectionPk pvConnectionEnd;
    LinkVersionPk pvVersion;
    ModificationPk pvModification;
  };

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_LINK_PK_H
