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

#include <QVariant>
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

    /*! \brief Contruct PK
     */
    explicit constexpr UnitConnectionPk(int64_t id) noexcept
     : pvIsNull(false), pvId(id) {}

    // Declare copy noexcept
    constexpr UnitConnectionPk(const UnitConnectionPk &) noexcept = default;
    UnitConnectionPk & operator=(const UnitConnectionPk &) noexcept = default;
    // Declare move noexcept
    constexpr UnitConnectionPk(UnitConnectionPk &&) noexcept = default;
    UnitConnectionPk & operator=(UnitConnectionPk &&) noexcept = default;

    /*! \brief Set id (Id_PK)
     */
    constexpr void setId(int64_t id) noexcept
    {
      pvIsNull = false;
      pvId = id;
    }

    /*! \brief Get id (Id_PK)
     */
    // Note: we use qlonglong, else QVariant() construction is not defined
    constexpr qlonglong id() const noexcept
    {
      return pvId;
    }

    /*! \brief Check if PK is null
     */
    constexpr bool isNull() const noexcept
    {
      return pvIsNull;
    }

    /*! \brief Check if lhs == rhs
     *
     * lhs and rhs are equal if their id are equal
     *  If lhs or rhs (or both) is null,
     *  they are never equal.
     */
    friend
    constexpr bool operator==(UnitConnectionPk lhs, UnitConnectionPk rhs) noexcept
    {
      if( lhs.isNull() || rhs.isNull() ){
        return false;
      }
      return (lhs.pvId == rhs.pvId);
    }

    /*! \brief Check if lhs == rhs
     *
     * \sa operator==()
     */
    friend
    constexpr bool operator!=(UnitConnectionPk lhs, UnitConnectionPk rhs) noexcept
    {
      return !(lhs == rhs);
    }

    /*! \brief Clear PK
     */
    constexpr void clear() noexcept
    {
      pvIsNull = true;
      pvId = 0;
    }

    /*! \brief Get a unit connection PK from a QVariant id
     *
     * If id is null, a null PK is returned
     *
     * \pre If ID has a value (is not null), it must be convertible to qlonglong
     */
    static UnitConnectionPk fromQVariant(const QVariant & id)
    {
      UnitConnectionPk pk;

      if(id.isNull()){
        return pk;
      }
      Q_ASSERT(id.canConvert<qlonglong>());
      pk.setId(id.toLongLong());

      return pk;
    }

   private:

    bool pvIsNull;
    int64_t pvId;
  };

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_UNIT_CONNECTION_PK_H
