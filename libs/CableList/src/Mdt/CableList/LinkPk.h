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
#include "mdtClModificationKeyData.h"

namespace Mdt{ namespace CableList{

  /*! \brief Link primary key data
   *
   * Refers to Link_tbl
   */
  class LinkPk
  {
   public:

    /*! \brief Set start connection
     */
    void setConnectionStart(UnitConnectionPk fk) noexcept
    {
      pvConnectionStart = fk;
    }

    /*! \brief Get start connection
     */
    UnitConnectionPk connectionStart() const noexcept
    {
      return pvConnectionStart;
    }

    /*! \brief Set end connection
     */
    void setConnectionEnd(UnitConnectionPk fk) noexcept
    {
      pvConnectionEnd = fk;
    }

    /*! \brief Get end connection
     */
    UnitConnectionPk connectionEnd() const noexcept
    {
      return pvConnectionEnd;
    }

    /*! \brief Set link version
     */
    void setVersion(const LinkVersionPk & fk) noexcept
    {
      pvVersion = fk;
    }

    /*! \brief Get link version
     */
    LinkVersionPk version() const noexcept
    {
      return pvVersion;
    }

    /*! \brief Set modification
     */
    void setModification(const mdtClModificationPkData & fk)
    {
      pvModification = fk;
    }

    /*! \brief Get modification
     */
    mdtClModificationPkData modification() const
    {
      return pvModification;
    }

    /*! \brief Check if primary key is null
     *
     * Primary key is null if one of the connection  or version or modification is null
     */
    bool isNull() const noexcept
    {
      return (pvConnectionStart.isNull() || pvConnectionEnd.isNull() || pvVersion.isNull() || pvModification.isNull() );
    }

   private:

    UnitConnectionPk pvConnectionStart;
    UnitConnectionPk pvConnectionEnd;
    LinkVersionPk pvVersion;
    mdtClModificationPkData pvModification;
  };

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_LINK_PK_H
