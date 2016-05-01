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
#ifndef MDT_CABLE_LIST_UNIT_CONNECTION_PK_H
#define MDT_CABLE_LIST_UNIT_CONNECTION_PK_H

#include <cstdint>

namespace Mdt{ namespace CableList{

  /*! \brief Unit connection Primary Key (PK)
   *
   * Refers to UnitConnection_tbl , field Id_PK
   */
  class UnitConnectionPk
  {
   public:

    /*! \brief Contruct a null PK
     */
    constexpr UnitConnectionPk() noexcept
     : pvIsNull(true), pvId(0) {}

    /*! \brief Contruct a null PK
     */
    explicit constexpr UnitConnectionPk(int64_t id) noexcept
     : pvIsNull(false), pvId(id) {}

    /*! \brief Set id (Id_PK)
     */
    constexpr void setId(int64_t id) noexcept
    {
      pvIsNull = false;
      pvId = id;
    }

    /*! \brief Get id (Id_PK)
     */
    constexpr int64_t id() const noexcept
    {
      return pvId;
    }

    /*! \brief Check if PK is null
     */
    constexpr bool isNull() const noexcept
    {
      return pvIsNull;
    }

    /*! \brief Clear PK
     */
    constexpr void clear() noexcept
    {
      pvIsNull = true;
      pvId = 0;
    }

   private:

    bool pvIsNull;
    int64_t pvId;
  };

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_UNIT_CONNECTION_PK_H
